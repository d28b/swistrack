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
	//Empty the vector
	calibrationPointList.clear();
	//Fill the vector with the readen points
	wxXmlNode *node=mSelectedNode->GetChildren();
	while (node) {
		if (node->GetName()=="point") 
		{
			ReadPoint(node);
		}
		node=node->GetNext();
	}

	//Getting camera parametrs from configuration file
	cameraParameters.Ncx=GetConfigurationDouble("Ncx",480);
	cameraParameters.Nfx=GetConfigurationDouble("Nfx",480);
	cameraParameters.dx=GetConfigurationDouble("dx",0.0065);
	cameraParameters.dpx=cameraParameters.dx*cameraParameters.Ncx/cameraParameters.Nfx;
	cameraParameters.dy=GetConfigurationDouble("dy",0.0065);
	cameraParameters.dpy=cameraParameters.dy;
	cameraParameters.Cx=GetConfigurationDouble("Cx",240);
	cameraParameters.Cy=GetConfigurationDouble("Cy",320);
	cameraParameters.sx=GetConfigurationDouble("sx",1.0);
	//Put the points into the matrix
	if (calibrationPointList.size()>TSAI_MAX_POINTS)
		AddError("There are more calibration points than accepted by the Tsai calibration library");
	calibrationData.point_count=calibrationPointList.size();
	for (int i=0;i<calibrationData.point_count;i++)
	{
		calibrationData.zw[i]=0;
		calibrationData.xw[i]=(calibrationPointList.at(i)).xWorld;
		calibrationData.yw[i]=(calibrationPointList.at(i)).yWorld;
		calibrationData.Xf[i]=(calibrationPointList.at(i)).xImage;
		calibrationData.Yf[i]=(calibrationPointList.at(i)).yImage;
	}
	//Do the calibration
	coplanar_calibration_with_full_optimization(&calibrationData,&calibrationConstants,&cameraParameters);
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

void THISCLASS::OnStop() 
{
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