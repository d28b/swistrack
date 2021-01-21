#include "ComponentCalibrationLinear.h"
#define THISCLASS ComponentCalibrationLinear

#include "DisplayEditor.h"
#include "DataStructureParticles.h"
#include <wx/log.h>
#include <fstream>

THISCLASS::ComponentCalibrationLinear(SwisTrackCore *stc):
		Component(stc, wxT("CalibrationLinear")),
		mDisplayOutput(wxT("Output"), wxT("Linear Calibration: Output")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryCalibration);
	AddDataStructureRead(&(mCore->mDataStructureParticles));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentCalibrationLinear() {
}

void THISCLASS::OnStart() {
	// Read the file containing the calibration points
	wxString filename_string = GetConfigurationString(wxT("CalibrationPoints"), wxT(""));
	wxFileName filename = mCore->GetProjectFileName(filename_string);
	wxLogNull log;
	wxXmlDocument document;
	bool isopen = document.Load(filename.GetFullPath());
	if (! isopen) {
		AddError(wxT("The file \'") + filename.GetFullPath() + wxT("\' could not be read."));
		return;
	}

	// Select the root element and check its name
	SetRootNode(document.GetRoot());
	if (GetRootNode()->GetName() != wxT("pointlist")) {
		AddError(wxT("The XML root node must be called 'pointlist'!"));
		return;
	}

	// Enumerate all points in the list
	SelectChildNode(wxT("points"));
	if (! mSelectedNode) {
		AddError(wxT("No node 'points' found!"));
		return;
	}

	// Empty the vector
	calibrationPointList.clear();

	// Fill the vector with the readen points
	wxXmlNode *node = mSelectedNode->GetChildren();
	while (node) {
		if (node->GetName() == wxT("point")) {
			ReadPoint(node);
		}
		node = node->GetNext();
	}

	// Create the matrices
	CvMat* pseudoinverse32f   = cvCreateMat(12, calibrationPointList.size() * 2, CV_32F);	// matrix to hold the pseudoinverse
	CvMat* imagematrix32f     = cvCreateMat(calibrationPointList.size() * 2, 12, CV_32F);	// matrix containing points in image space
	CvMat* objectmatrix32f    = cvCreateMat(calibrationPointList.size() * 2, 1, CV_32F);	// matrix containing points in world space
	CvMat* cameratransform32f = cvCreateMat(12, 1, CV_32F);								// matrix containing camera matrix (nonlinear model)

	// Build up matrices
	for (unsigned int i = 0; i < calibrationPointList.size(); i++) {
		CvPoint2D32f p;

		p.x = (calibrationPointList.at(i)).xImage;
		p.y = (calibrationPointList.at(i)).yImage;

		imagematrix32f->data.fl[i*12] = p.x;
		imagematrix32f->data.fl[i*12+1] = p.y;
		imagematrix32f->data.fl[i*12+2] = 1;
		imagematrix32f->data.fl[i*12+3] = p.x * p.x;
		imagematrix32f->data.fl[i*12+4] = p.y * p.y;
		imagematrix32f->data.fl[i*12+5] = p.x * p.y;
		imagematrix32f->data.fl[i*12+6] = 0;
		imagematrix32f->data.fl[i*12+7] = 0;
		imagematrix32f->data.fl[i*12+8] = 0;
		imagematrix32f->data.fl[i*12+9] = 0;
		imagematrix32f->data.fl[i*12+10] = 0;
		imagematrix32f->data.fl[i*12+11] = 0;

		imagematrix32f->data.fl[calibrationPointList.size()*12+i*12] = 0;
		imagematrix32f->data.fl[calibrationPointList.size()*12+i*12+1] = 0;
		imagematrix32f->data.fl[calibrationPointList.size()*12+i*12+2] = 0;
		imagematrix32f->data.fl[calibrationPointList.size()*12+i*12+3] = 0;
		imagematrix32f->data.fl[calibrationPointList.size()*12+i*12+4] = 0;
		imagematrix32f->data.fl[calibrationPointList.size()*12+i*12+5] = 0;
		imagematrix32f->data.fl[calibrationPointList.size()*12+i*12+6] = p.x;
		imagematrix32f->data.fl[calibrationPointList.size()*12+i*12+7] = p.y;
		imagematrix32f->data.fl[calibrationPointList.size()*12+i*12+8] = 1;
		imagematrix32f->data.fl[calibrationPointList.size()*12+i*12+9] = p.x * p.x;
		imagematrix32f->data.fl[calibrationPointList.size()*12+i*12+10] = p.y * p.y;
		imagematrix32f->data.fl[calibrationPointList.size()*12+i*12+11] = p.x * p.y;

		objectmatrix32f->data.fl[i] = (calibrationPointList.at(i)).xWorld;
		objectmatrix32f->data.fl[i+calibrationPointList.size()] = (calibrationPointList.at(i)).yWorld;
	}

	CvMat* multransposed    = cvCreateMat(12, 12, CV_32F);
	CvMat* invmultransposed = cvCreateMat(12, 12, CV_32F);
	CvMat* transposed       = cvCreateMat(12, calibrationPointList.size() * 2, CV_32F);

	cvMulTransposed(imagematrix32f, multransposed, 1);
	cvInv(multransposed, invmultransposed, CV_LU);
	cvTranspose(imagematrix32f, transposed);
	cvMatMul(invmultransposed, transposed, pseudoinverse32f);
	cvMatMul(pseudoinverse32f, objectmatrix32f, cameratransform32f);

	// Copy cameratransformation to appropriate format
	for (int i = 0; i < 12; i++) {
		cameraMatrix[i] = cameratransform32f->data.fl[i];
	}

	// Release unused matrix
	cvReleaseMat(&pseudoinverse32f);
	cvReleaseMat(&imagematrix32f);
	cvReleaseMat(&objectmatrix32f);
	cvReleaseMat(&cameratransform32f);

	//Compute error of calibration on calibration points
	double errorX, errorY, maxErrorX, maxErrorY, maxErrorD, errorD;
	maxErrorX = 0;
	maxErrorY = 0;
	errorX = 0;
	errorY = 0;
	maxErrorD = 0;
	errorD = 0;

	for (unsigned int i = 0; i < calibrationPointList.size(); i++) {
		double tmpErrorX, tmpErrorY, tmpErrorD;
		CvPoint2D32f originalImage, originalWorld, finalWorld;
		originalImage.x = (float)(calibrationPointList.at(i)).xImage;
		originalImage.y = (float)(calibrationPointList.at(i)).yImage;
		originalWorld.x = (float)(calibrationPointList.at(i)).xWorld;
		originalWorld.y = (float)(calibrationPointList.at(i)).yWorld;
		finalWorld = THISCLASS::Image2World(originalImage);
		tmpErrorX = (double)fabsf(originalWorld.x - finalWorld.x);
		tmpErrorY = (double)fabsf(originalWorld.y - finalWorld.y);
		tmpErrorD = sqrt(tmpErrorX * tmpErrorX + tmpErrorY * tmpErrorY);
		errorX += tmpErrorX;
		errorY += tmpErrorY;
		errorD += tmpErrorD;
		if (tmpErrorX > maxErrorX) {
			maxErrorX = tmpErrorX;
		}
		if (tmpErrorY > maxErrorY) {
			maxErrorY = tmpErrorY;
		}
		if (tmpErrorD > maxErrorD) {
			maxErrorD = tmpErrorD;
		}
	}
	errorX = errorX / calibrationPointList.size();
	errorY = errorY / calibrationPointList.size();
	errorD = errorD / calibrationPointList.size();

	// Write the calibration parameters
	std::fstream logfile;
	wxFileName logfilename = mCore->GetRunFileName(wxT("CalibrationLinear.log"));
	logfile.open(logfilename.GetFullPath().mb_str(wxConvFile), std::fstream::out | std::fstream::trunc);
	logfile << "Calibration error" << std::endl;
	logfile << "Max error in X: \t" << maxErrorX << std::endl;
	logfile << "Max error in Y: \t" << maxErrorY << std::endl;
	logfile << "Average error in X: \t" << errorX << std::endl;
	logfile << "Average error in Y: \t" << errorY << std::endl;
	logfile << "Max distance error: \t" << maxErrorD << std::endl;
	logfile << "Average distance error: \t" << errorD << std::endl;
	logfile << "Camera matrix: " << std::endl;
	logfile << "\t" << "x = a11 u + a12 v + a13 + a14 u^2 + a15 v^2 + a16 uv" << std::endl;
	logfile << "\t" << "y = a21 u + a22 v + a23 + a24 u^2 + a25 v^2 + a26 uv" << std::endl;
	for (int i = 0; i < 6; i++) {
		logfile << "\t" << cameraMatrix[i];
	}
	logfile << std::endl;
	for (int i = 6; i < 12; i++) {
		logfile << "\t" << cameraMatrix[i];
	}
	logfile << std::endl;
	logfile.close();
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
	// These are the particles to transform
	DataStructureParticles::tParticleVector *particles = mCore->mDataStructureParticles.mParticles;
	if (! particles) {
		return;
	}

	// Transform all particle positions
	DataStructureParticles::tParticleVector::iterator it = particles->begin();
	while (it != particles->end()) {
		it->mWorldCenter = Image2World(it->mCenter);
		it++;
	}
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}

void THISCLASS::ReadPoint(wxXmlNode *node) {
	mSelectedNode = node;

	CalibrationPoint calibrationPoint;
	calibrationPoint.xImage = ConfigurationConversion::Double(ReadChildContent(wxT("ximage")), 0);
	calibrationPoint.yImage = ConfigurationConversion::Double(ReadChildContent(wxT("yimage")), 0);
	calibrationPoint.xWorld = ConfigurationConversion::Double(ReadChildContent(wxT("xworld")), 0);
	calibrationPoint.yWorld = ConfigurationConversion::Double(ReadChildContent(wxT("yworld")), 0);
	calibrationPointList.push_back(calibrationPoint);
}

CvPoint2D32f THISCLASS::Image2World(CvPoint2D32f p) {
	CvPoint2D32f w;
	float pxx, pxy, pyy;
	pxx = p.x * p.x;
	pyy = p.y * p.y;
	pxy = p.x * p.y;
	w.x = cameraMatrix[0] * p.x + cameraMatrix[1] * p.y + cameraMatrix[2] + cameraMatrix[3] * pxx + cameraMatrix[4] * pyy + cameraMatrix[5] * pxy;
	w.y = cameraMatrix[6] * p.x + cameraMatrix[7] * p.y + cameraMatrix[8] + cameraMatrix[9] * pxx + cameraMatrix[10] * pyy + cameraMatrix[11] * pxy;
	return(w);
}
