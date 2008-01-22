#include "ComponentCalibrationTSAI.h"
#define THISCLASS ComponentCalibrationTSAI

#include "DisplayEditor.h"
#include "DataStructureParticles.h"
#include <wx/log.h>
#include "libtsai.h"

THISCLASS::ComponentCalibrationTSAI(SwisTrackCore *stc):
		Component(stc, "CalibrationTSAI"),
		mDisplayOutput("Output", "TSAI Calibration: Output") {

	// Data structure relations
	mCategory=&(mCore->mCategoryCalibration);
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
	std::string filename=GetConfigurationString("CalibrationPoints", "");
	wxLogNull log;
	wxXmlDocument document;
	bool isopen=document.Load(filename);
	if (! isopen) {
		AddError("Could not open or parse the XML file!");
		return;
	}

	// Select the root element and check its name
	SetRootNode(document.GetRoot());
	if (GetRootNode()->GetName() != "pointlist") {
		AddError("The XML root node must be called 'pointlist'!");
		return;
	}

	// Enumerate all points in the list
	SelectChildNode("points");
	if (! mSelectedNode) {
		AddError("No node 'points' found!");
		return;
	}

	// Empty the vector
	calibrationPointList.clear();

	// Fill the vector with the readen points
	wxXmlNode *node=mSelectedNode->GetChildren();
	while (node) {
		if (node->GetName()=="point") {
			ReadPoint(node);
		}
		node=node->GetNext();
	}

	//Getting camera parametrs from configuration file
	cameraParameters.Ncx=GetConfigurationDouble("Ncx",640);
	cameraParameters.Nfx=GetConfigurationDouble("Nfx",640);
	cameraParameters.dx=GetConfigurationDouble("dx",0.0065);
	cameraParameters.dpx=cameraParameters.dx*cameraParameters.Ncx/cameraParameters.Nfx;
	cameraParameters.dy=GetConfigurationDouble("dy",0.0065);
	cameraParameters.dpy=cameraParameters.dy*GetConfigurationDouble("Ncy",480)/GetConfigurationDouble("Nfy",480);
	cameraParameters.Cx=cameraParameters.Ncx/2;
	cameraParameters.Cy=GetConfigurationDouble("Ncy",480)/2;
	cameraParameters.sx=GetConfigurationDouble("sx",1.0);
	// Put the points into the matrix
	if (calibrationPointList.size()>TSAI_MAX_POINTS) {
		AddError("There are more calibration points than accepted by the Tsai calibration library");
	}
	calibrationData.point_count=calibrationPointList.size();
	for (int i=0; i<calibrationData.point_count; i++) {
		calibrationData.zw[i]=0;
		calibrationData.xw[i]=(calibrationPointList.at(i)).xWorld;
		calibrationData.yw[i]=(calibrationPointList.at(i)).yWorld;
		calibrationData.Xf[i]=(calibrationPointList.at(i)).xImage;
		calibrationData.Yf[i]=(calibrationPointList.at(i)).yImage;
	}
	// Do the calibration
	try {
		coplanar_calibration_with_full_optimization(&calibrationData,&calibrationConstants,&cameraParameters);
	} 
	catch (...) {
		AddError("Calibration using libtsai failed.");
	}
	
#ifdef _DEBUG
	//For Debug Purpose, just compute the transformation of the 
	for (unsigned int i=0; i<calibrationPointList.size(); i++) {
		CvPoint2D32f originalImage,originalWorld,finalImage,finalWorld;
		originalImage.x=(float)(calibrationPointList.at(i)).xImage;
		originalImage.y=(float)(calibrationPointList.at(i)).yImage;
		originalWorld.x=(float)(calibrationPointList.at(i)).xWorld;
		originalWorld.y=(float)(calibrationPointList.at(i)).yWorld;
		finalWorld=THISCLASS::Image2World(originalImage);
		finalImage=THISCLASS::World2Image(originalWorld);
		int justToBeAbleToStopTheDebugger=0;
	}
#endif
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
	// These are the particles to transform
	DataStructureParticles::tParticleVector *particles=mCore->mDataStructureParticles.mParticles;
	if (! particles) {return;}

	// Transform all particle positions
	DataStructureParticles::tParticleVector::iterator it=particles->begin();
	while (it!=particles->end()) 
	{
		it->mWorldCenter=Image2World(it->mCenter);
		it++;
	}
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}

void THISCLASS::ReadPoint(wxXmlNode *node) {
	mSelectedNode=node;
	CalibrationPoint calibrationPoint;
	calibrationPoint.xImage=Double(ReadChildContent("ximage"),0);
	calibrationPoint.yImage=Double(ReadChildContent("yimage"),0);
	calibrationPoint.xWorld=Double(ReadChildContent("xworld"),0);
	calibrationPoint.yWorld=Double(ReadChildContent("yworld"),0);
	calibrationPointList.push_back(calibrationPoint);
}

CvPoint2D32f THISCLASS::Image2World(CvPoint2D32f imageCoordinates) {
	double wx,wy;
	CvPoint2D32f worldCoordinates;
	image_coord_to_world_coord(calibrationConstants,cameraParameters,(double)imageCoordinates.x,(double)imageCoordinates.y,0,&wx,&wy);
	worldCoordinates=cvPoint2D32f(wx,wy);
	return worldCoordinates;
}

CvPoint2D32f THISCLASS::World2Image(CvPoint2D32f worldCoordinates) {
	double ix,iy;
	CvPoint2D32f imageCoordinates;
	world_coord_to_image_coord(calibrationConstants,cameraParameters,(double)worldCoordinates.x,(double)worldCoordinates.y,0,&ix,&iy);
	imageCoordinates=cvPoint2D32f(ix,iy);
	return imageCoordinates;
}

