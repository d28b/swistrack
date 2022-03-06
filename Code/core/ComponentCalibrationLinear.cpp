#include "ComponentCalibrationLinear.h"
#define THISCLASS ComponentCalibrationLinear

#include "DisplayEditor.h"
#include "DataStructureParticles.h"
#include <wx/log.h>
#include <fstream>

THISCLASS::ComponentCalibrationLinear(SwisTrackCore * stc):
	Component(stc, wxT("CalibrationLinear")),
	mDisplayOutput(wxT("Output"), wxT("After calibration")) {

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
	wxString filenameString = GetConfigurationString(wxT("CalibrationPoints"), wxT(""));
	wxFileName filename = mCore->GetProjectFileName(filenameString);
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
	wxXmlNode * node = mSelectedNode->GetChildren();
	while (node) {
		if (node->GetName() == wxT("point")) {
			ReadPoint(node);
		}
		node = node->GetNext();
	}

	// Create the matrices
	cv::Mat imageMatrix(calibrationPointList.size() * 2, 12, CV_32F);	// matrix containing points in image space
	cv::Mat objectMatrix(calibrationPointList.size() * 2, 1, CV_32F);	// matrix containing points in world space

	// Build up matrices
	int count = calibrationPointList.size();
	for (int i = 0; i < count; i++) {
		cv::Point2f p;

		p.x = calibrationPointList.at(i).xImage;
		p.y = calibrationPointList.at(i).yImage;

		imageMatrix.at<float>(i, 0) = p.x;
		imageMatrix.at<float>(i, 1) = p.y;
		imageMatrix.at<float>(i, 2) = 1;
		imageMatrix.at<float>(i, 3) = p.x * p.x;
		imageMatrix.at<float>(i, 4) = p.y * p.y;
		imageMatrix.at<float>(i, 5) = p.x * p.y;
		imageMatrix.at<float>(i, 6) = 0;
		imageMatrix.at<float>(i, 7) = 0;
		imageMatrix.at<float>(i, 8) = 0;
		imageMatrix.at<float>(i, 9) = 0;
		imageMatrix.at<float>(i, 10) = 0;
		imageMatrix.at<float>(i, 11) = 0;

		imageMatrix.at<float>(count + i, 0) = 0;
		imageMatrix.at<float>(count + i, 1) = 0;
		imageMatrix.at<float>(count + i, 2) = 0;
		imageMatrix.at<float>(count + i, 3) = 0;
		imageMatrix.at<float>(count + i, 4) = 0;
		imageMatrix.at<float>(count + i, 5) = 0;
		imageMatrix.at<float>(count + i, 6) = p.x;
		imageMatrix.at<float>(count + i, 7) = p.y;
		imageMatrix.at<float>(count + i, 8) = 1;
		imageMatrix.at<float>(count + i, 9) = p.x * p.x;
		imageMatrix.at<float>(count + i, 10) = p.y * p.y;
		imageMatrix.at<float>(count + i, 11) = p.x * p.y;

		objectMatrix.at<float>(i, 0) = calibrationPointList.at(i).xWorld;
		objectMatrix.at<float>(count + i, 0) = calibrationPointList.at(i).yWorld;
	}

	cv::Mat mulTransposed(12, 12, CV_32F);
	cv::mulTransposed(imageMatrix, mulTransposed, true);
	cv::Mat cameraTransform = mulTransposed.inv() * imageMatrix.t() * objectMatrix;

	// Copy cameratransformation to appropriate format
	for (int i = 0; i < 12; i++)
		cameraMatrix[i] = cameraTransform.at<float>(i, 0);

	//Compute error of calibration on calibration points
	double errorX, errorY, maxErrorX, maxErrorY, maxErrorD, errorD;
	maxErrorX = 0;
	maxErrorY = 0;
	errorX = 0;
	errorY = 0;
	maxErrorD = 0;
	errorD = 0;

	for (unsigned int i = 0; i < calibrationPointList.size(); i++) {
		cv::Point2f originalImage;
		originalImage.x = (float) calibrationPointList.at(i).xImage;
		originalImage.y = (float) calibrationPointList.at(i).yImage;

		cv::Point2f originalWorld;
		originalWorld.x = (float) calibrationPointList.at(i).xWorld;
		originalWorld.y = (float) calibrationPointList.at(i).yWorld;

		cv::Point2f finalWorld = THISCLASS::Image2World(originalImage);

		double tmpErrorX = (double) fabsf(originalWorld.x - finalWorld.x);
		double tmpErrorY = (double) fabsf(originalWorld.y - finalWorld.y);
		double tmpErrorD = sqrt(tmpErrorX * tmpErrorX + tmpErrorY * tmpErrorY);

		errorX += tmpErrorX;
		errorY += tmpErrorY;
		errorD += tmpErrorD;

		if (tmpErrorX > maxErrorX) maxErrorX = tmpErrorX;
		if (tmpErrorY > maxErrorY) maxErrorY = tmpErrorY;
		if (tmpErrorD > maxErrorD) maxErrorD = tmpErrorD;
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

	for (int i = 0; i < 6; i++)
		logfile << "\t" << cameraMatrix[i];
	logfile << std::endl;

	for (int i = 6; i < 12; i++)
		logfile << "\t" << cameraMatrix[i];
	logfile << std::endl;

	logfile.close();
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
	// These are the particles to transform
	DataStructureParticles::tParticleVector * particles = mCore->mDataStructureParticles.mParticles;
	if (! particles) return;

	// Transform all particle positions
	for (auto & particle : *particles)
		particle.mWorldCenter = Image2World(particle.mCenter);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}

void THISCLASS::ReadPoint(wxXmlNode * node) {
	mSelectedNode = node;

	CalibrationPoint calibrationPoint;
	calibrationPoint.xImage = ConfigurationConversion::Double(ReadChildContent(wxT("ximage")), 0);
	calibrationPoint.yImage = ConfigurationConversion::Double(ReadChildContent(wxT("yimage")), 0);
	calibrationPoint.xWorld = ConfigurationConversion::Double(ReadChildContent(wxT("xworld")), 0);
	calibrationPoint.yWorld = ConfigurationConversion::Double(ReadChildContent(wxT("yworld")), 0);
	calibrationPointList.push_back(calibrationPoint);
}

cv::Point2f THISCLASS::Image2World(cv::Point2f p) {
	float pxx = p.x * p.x;
	float pyy = p.y * p.y;
	float pxy = p.x * p.y;

	cv::Point2f w;
	w.x = cameraMatrix[0] * p.x + cameraMatrix[1] * p.y + cameraMatrix[2] + cameraMatrix[3] * pxx + cameraMatrix[4] * pyy + cameraMatrix[5] * pxy;
	w.y = cameraMatrix[6] * p.x + cameraMatrix[7] * p.y + cameraMatrix[8] + cameraMatrix[9] * pxx + cameraMatrix[10] * pyy + cameraMatrix[11] * pxy;
	return w;
}
