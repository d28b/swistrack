#include "ComponentCalibrationTSAI.h"
#define THISCLASS ComponentCalibrationTSAI

#include "DisplayEditor.h"
#include "DataStructureParticles.h"
#include "libtsai.h"
#include <wx/log.h>
#include <fstream>

THISCLASS::ComponentCalibrationTSAI(SwisTrackCore *stc):
		Component(stc, wxT("CalibrationTSAI")),
		mDisplayOutput(wxT("Output"), wxT("TSAI Calibration: Output")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryCalibration);
	AddDataStructureRead(&(mCore->mDataStructureParticles));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentCalibrationTSAI() {
}

void THISCLASS::OnStart() {
	// Read the file containing the calibration points
	wxString filename_string = GetConfigurationString(wxT("CalibrationPoints"), wxT(""));
	wxFileName filename = mCore->GetProjectFileName(filename_string);
	wxLogNull log;
	wxXmlDocument document;
	bool isopen = document.Load(filename.GetFullPath());
	if (! isopen) {
		AddError(wxT("Could not open or parse the XML file!"));
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

	//Getting camera parametrs from configuration file
	cameraParameters.Ncx = GetConfigurationDouble(wxT("Ncx"), 640);
	cameraParameters.Nfx = GetConfigurationDouble(wxT("Nfx"), 640);
	cameraParameters.dx = GetConfigurationDouble(wxT("dx"), 0.0065);
	cameraParameters.dpx = cameraParameters.dx * cameraParameters.Ncx / cameraParameters.Nfx;
	cameraParameters.dy = GetConfigurationDouble(wxT("dy"), 0.0065);
	cameraParameters.dpy = cameraParameters.dy * GetConfigurationDouble(wxT("Ncy"), 480) / GetConfigurationDouble(wxT("Nfy"), 480);
	cameraParameters.Cx = cameraParameters.Ncx / 2;
	cameraParameters.Cy = GetConfigurationDouble(wxT("Ncy"), 480) / 2;
	cameraParameters.sx = GetConfigurationDouble(wxT("sx"), 1.0);
	// Put the points into the matrix
	if (calibrationPointList.size() > TSAI_MAX_POINTS) {
		AddError(wxT("There are more calibration points than accepted by the Tsai calibration library."));
		return;
		return;
	}
	calibrationData.point_count = calibrationPointList.size();
	for (int i = 0; i < calibrationData.point_count; i++) {
		calibrationData.zw[i] = 0;
		calibrationData.xw[i] = (calibrationPointList.at(i)).xWorld;
		calibrationData.yw[i] = (calibrationPointList.at(i)).yWorld;
		calibrationData.Xf[i] = (calibrationPointList.at(i)).xImage;
		calibrationData.Yf[i] = (calibrationPointList.at(i)).yImage;
	}
	// Do the calibration
	try {
		coplanar_calibration_with_full_optimization(&calibrationData, &calibrationConstants, &cameraParameters);
	}
	catch (...) {
		AddError(wxT("Calibration using libtsai failed."));
		return;
	}

	//Compute error of calibration on calibration points
	double errorX, errorY, maxErrorX, maxErrorY, maxErrorD, errorD;
	maxErrorX = 0;
	maxErrorY = 0;
	errorX = 0;
	errorY = 0;
	maxErrorD = 0;
	errorD = 0;

	for (unsigned int i = 0; i < calibrationPointList.size(); i++)
	{
		double tmpErrorX, tmpErrorY, tmpErrorD;
		CvPoint2D32f originalImage, originalWorld, finalWorld;
		originalImage.x = (float)(calibrationPointList.at(i)).xImage;
		originalImage.y = (float)(calibrationPointList.at(i)).yImage;
		originalWorld.x = (float)(calibrationPointList.at(i)).xWorld;
		originalWorld.y = (float)(calibrationPointList.at(i)).yWorld;
		finalWorld = THISCLASS::Image2World(originalImage);
		tmpErrorX = abs(originalWorld.x - finalWorld.x);
		tmpErrorY = abs(originalWorld.y - finalWorld.y);
		tmpErrorD = sqrt(tmpErrorX * tmpErrorX + tmpErrorY * tmpErrorY);
		errorX += tmpErrorX;
		errorY += tmpErrorY;
		errorD += tmpErrorD;
		if (tmpErrorX > maxErrorX)
			maxErrorX = tmpErrorX;
		if (tmpErrorY > maxErrorY)
			maxErrorY = tmpErrorY;
		if (tmpErrorD > maxErrorD)
			maxErrorD = tmpErrorD;
	}
	errorX = errorX / calibrationPointList.size();
	errorY = errorY / calibrationPointList.size();
	errorD = errorD / calibrationPointList.size();

	//Write error into calibration.log
	std::fstream logfile;
	wxFileName logfilename = mCore->GetRunFileName(wxT("CalibrationTSAI.log"));
	logfile.open(logfilename.GetFullPath().mb_str(wxConvFile), std::fstream::out | std::fstream::trunc);
	logfile << "Calibration error" << std::endl;
	logfile << "Max error in X: \t" << maxErrorX << std::endl;
	logfile << "Max error in Y: \t" << maxErrorY << std::endl;
	logfile << "Average error in X: \t" << errorX << std::endl;
	logfile << "Average error in Y: \t" << errorY << std::endl;
	logfile << "Max distance error: \t" << maxErrorD << std::endl;
	logfile << "Average distance error: \t" << errorD << std::endl;
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
	while (it != particles->end())
	{
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

CvPoint2D32f THISCLASS::Image2World(CvPoint2D32f imageCoordinates) {
	double wx, wy;
	CvPoint2D32f worldCoordinates;
	image_coord_to_world_coord(calibrationConstants, cameraParameters, (double)imageCoordinates.x, (double)imageCoordinates.y, 0, &wx, &wy);
	worldCoordinates = cvPoint2D32f(wx, wy);
	return worldCoordinates;
}

CvPoint2D32f THISCLASS::World2Image(CvPoint2D32f worldCoordinates) {
	double ix, iy;
	CvPoint2D32f imageCoordinates;
	world_coord_to_image_coord(calibrationConstants, cameraParameters, (double)worldCoordinates.x, (double)worldCoordinates.y, 0, &ix, &iy);
	imageCoordinates = cvPoint2D32f(ix, iy);
	return imageCoordinates;
}

