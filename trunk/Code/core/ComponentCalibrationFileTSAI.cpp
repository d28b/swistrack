#include "ComponentCalibrationFileTSAI.h"
#define THISCLASS ComponentCalibrationFileTSAI

#include "DisplayEditor.h"
#include "DataStructureParticles.h"
#include "libtsai.h"
#include <wx/log.h>
#include <fstream>
#define rot(a,b) cvmGet(mCalParam.R,a-1,b-1)


THISCLASS::ComponentCalibrationFileTSAI(SwisTrackCore *stc):
		Component(stc, wxT("CalibrationFileTSAI")),
		mDisplayOutput(wxT("Output"), wxT("TSAI Calibration: Output")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryCalibration);
	AddDataStructureRead(&(mCore->mDataStructureParticles));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentCalibrationFileTSAI() {
}

void THISCLASS::OnStart() 
{	
	//Create the matrix header and allocate data
	mCalParam.R=cvCreateMat(3,3,CV_64FC1);	

	//Getting camera parametrs from configuration file
	wxString filename_string = GetConfigurationString(wxT("CalibrationParameters"), wxT(""));
	wxFileName filename = mCore->GetProjectFileName(filename_string);
	wxLogNull log;
	wxXmlDocument document;
	bool isopen = document.Load(filename.GetFullPath());
	if (! isopen) {
		AddError(wxT("Calibration Parameters: Could not open or parse the XML file!"));
		return;
	}

	// Select the root element and check its name
	SetRootNode(document.GetRoot());
	if (GetRootNode()->GetName() != wxT("calibration")) {
		AddError(wxT("The XML root node must be called 'calibration'!"));
		return;
	}

	// Enumerate all points in the list
	SelectChildNode(wxT("parameters"));
	if (! mSelectedNode) {
		AddError(wxT("No node 'parameters' found!"));
		return;
	}
		
	

	//Read the parameters	
	wxXmlNode *node = mSelectedNode->GetChildren();
	while (node) 
	{
		if (node->GetName()==wxT("R"))
		{
			Read3DMatrix(node,mCalParam.R);								
		}
		if (node->GetName()==wxT("T"))
		{
			mSelectedNode = node;
			(mCalParam.T).x=ConfigurationConversion::Double(ReadChildContent(wxT("Tx")), 0);
			(mCalParam.T).y=ConfigurationConversion::Double(ReadChildContent(wxT("Ty")), 0);
			(mCalParam.T).z=ConfigurationConversion::Double(ReadChildContent(wxT("Tz")), 0);
		}
		if (node->GetName()==wxT("C"))	
		{	
			mSelectedNode = node;
			(mCalParam.C).x=ConfigurationConversion::Double(ReadChildContent(wxT("Cx")), 0);
			(mCalParam.C).y=ConfigurationConversion::Double(ReadChildContent(wxT("Cy")), 0);
		}
		if (node->GetName()==wxT("dx"))
		{
			mSelectedNode = node;
			mCalParam.dx=ConfigurationConversion::Double(ReadChildContent(wxT("value")), 0);
		}
		if (node->GetName()==wxT("dy"))
		{
			mSelectedNode = node;
			mCalParam.dy=ConfigurationConversion::Double(ReadChildContent(wxT("value")), 0);
		}
		if (node->GetName()==wxT("sx"))
		{
			mSelectedNode = node;
			mCalParam.sx=ConfigurationConversion::Double(ReadChildContent(wxT("value")), 0);
		}
		if (node->GetName()==wxT("k"))
		{
			mSelectedNode = node;
			mCalParam.k1=ConfigurationConversion::Double(ReadChildContent(wxT("value")), 0);
		}
		if (node->GetName()==wxT("f"))
		{
			mSelectedNode = node;
			mCalParam.f=ConfigurationConversion::Double(ReadChildContent(wxT("value")), 0);		
		}
		node = node->GetNext();
	}		


	// Read the file containing the calibration points and compute calibration error if the file exist
	filename_string = GetConfigurationString(wxT("CalibrationPoints"), wxT(""));
	filename = mCore->GetProjectFileName(filename_string);
	isopen = document.Load(filename.GetFullPath());
	if (! isopen) {
		AddWarning(wxT("Calibration Points: could not open or parse the XML file!"));		
	}
	else
	{

			// Select the root element and check its name
			SetRootNode(document.GetRoot());
			if (GetRootNode()->GetName() != wxT("pointlist")) {
				AddError(wxT("Calibration Points: The XML root node must be called 'pointlist'!"));
				return;
			}

			// Enumerate all points in the list
			SelectChildNode(wxT("points"));
			if (! mSelectedNode) {
				AddError(wxT("Calibration Points: No node 'points' found!"));
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
	wxFileName logfilename = mCore->GetRunFileName(wxT("CalibrationFileTSAI.log"));
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

void THISCLASS::OnStop() 
{
	//Release calibration matrices
	if (mCalParam.R) 	
		cvReleaseMat(&(mCalParam.R));
}

 void THISCLASS::Read3DMatrix(wxXmlNode *node, CvMat* matrix)
 {
	 mSelectedNode = node;	 
	 for (int i=0; i<3;i++)
		 for (int j=0;j<3;j++)
		 {
			 char tmpStr[3]="aa";
			 tmpStr[0]=tmpStr[0]+i;
			 tmpStr[1]=tmpStr[1]+j;
			 cvmSet(matrix,i,j,ConfigurationConversion::Double(ReadChildContent(wxString::FromAscii(tmpStr)), 0));
		 }		
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

CvPoint2D32f THISCLASS::Image2World(CvPoint2D32f imageCoordinates) 
{
	CvPoint2D64f w; //Computed world coordinates
	CvPoint2D64f i; //Imput coordinates
	double r2;		//radial position
	i.x=((double)imageCoordinates.x-mCalParam.C.x)*mCalParam.dx/mCalParam.sx;
	i.y=-((double)imageCoordinates.y-mCalParam.C.y)*mCalParam.dy;
	r2=i.x*i.x+i.y*i.y;
	cvmGet(mCalParam.R,0,0);

	w.x=(mCalParam.f*(rot(2,2)*mCalParam.T.x-rot(1,2)*mCalParam.T.y)+(1+mCalParam.k1*r2)*((rot(1,2)*i.y-rot(2,2)*i.x)*mCalParam.T.z+rot(3,2)*(i.x*mCalParam.T.y-i.y*mCalParam.T.x)))
		/(mCalParam.f*(rot(1,2)*rot(2,1)-rot(1,1)*rot(2,2))+(1+mCalParam.k1*r2)*(i.x*(rot(2,2)*rot(3,1)-rot(2,1)*rot(3,2))+i.y*(rot(1,1)*rot(3,2)-rot(1,2)*rot(3,1))));
	w.y=-(mCalParam.f*(rot(2,1)*mCalParam.T.x-rot(1,1)*mCalParam.T.y)+(1+mCalParam.k1*r2)*((rot(1,1)*i.y-rot(2,1)*i.x)*mCalParam.T.z+rot(3,1)*(i.x*mCalParam.T.y-i.y*mCalParam.T.x)))
		/(mCalParam.f*(rot(1,2)*rot(2,1)-rot(1,1)*rot(2,2))+(1+mCalParam.k1*r2)*(i.x*(rot(2,2)*rot(3,1)-rot(2,1)*rot(3,2))+i.y*(rot(1,1)*rot(3,2)-rot(1,2)*rot(3,1))));	
	return cvPoint2D32f(w.x,w.y);
}
