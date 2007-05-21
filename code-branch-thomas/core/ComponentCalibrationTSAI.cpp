#include "ComponentCalibrationTSAI.h"
#define THISCLASS ComponentCalibrationTSAI

#include "DisplayEditor.h"
#include "DataStructureParticles.h"
#include <wx/log.h>

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

void THISCLASS::OnStart() 
{
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

	wxXmlNode *node=mSelectedNode->GetChildren();
	while (node) {
		if (node->GetName()=="point") 
		{
			ReadPoint(node);
		}
		node=node->GetNext();
	}


	//Compute the calibration matrix


}

void THISCLASS::OnReloadConfiguration() 
{
}

void THISCLASS::OnStep() {
	/*
	// These are the particles to transform
	DataStructureParticles::tParticleVector *particles=mCore->mDataStructureParticles.mParticles;
	if (! particles) {return;}

	// Transform all particle positions
	DataStructureParticles::tParticleVector::iterator it=particles->begin();
	while (it!=particles->end()) {
		Transform(&*it);
		it++;
	}

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		//de.SetMode(DisplayEditor::sModeWorldCoordinates);
		de.SetArea(mWorldTopLeft, mWorldBottomRight);
		de.SetParticles(particles);
	}
	*/
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}

void THISCLASS::Transform(Particle *p) {
	/*
	CvPoint2D32f ratio;
	ratio.x=(p->mCenter.x-mCameraTopLeft.x)/(mCameraBottomRight.x-mCameraTopLeft.x);
	ratio.y=(p->mCenter.y-mCameraTopLeft.y)/(mCameraBottomRight.y-mCameraTopLeft.y);

	CvPoint2D32f ptop;
	ptop.x=ratio.x*(mWorldTopRight.x-mWorldTopLeft.x)+mWorldTopLeft.x;
	ptop.y=ratio.x*(mWorldTopRight.y-mWorldTopLeft.y)+mWorldTopLeft.y;

	CvPoint2D32f pbottom;
	pbottom.x=ratio.x*(mWorldBottomRight.x-mWorldBottomLeft.x)+mWorldBottomLeft.x;
	pbottom.y=ratio.x*(mWorldBottomRight.y-mWorldBottomLeft.y)+mWorldBottomLeft.y;

	p->mCenter.x=ratio.y*(pbottom.x-ptop.x)+ptop.x;
	p->mCenter.y=ratio.y*(pbottom.y-ptop.y)+ptop.y;
	*/
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