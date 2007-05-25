#include "ComponentCalibrationLinear.h"
#define THISCLASS ComponentCalibrationLinear

#include "DisplayEditor.h"
#include "DataStructureParticles.h"
#include <wx/log.h>

THISCLASS::ComponentCalibrationLinear(SwisTrackCore *stc):
		Component(stc, "CalibrationLinear"),
		mDisplayOutput("Output", "Linear Calibration: Output") {

	// Data structure relations
	mCategory=&(mCore->mCategoryCalibration);
	AddDataStructureRead(&(mCore->mDataStructureParticles));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentCalibrationLinear() {
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
	//Create the matrices
	CvMat* pseudoinverse32f   = cvCreateMat(12,calibrationPointList.size()*2,CV_32F);	// matrix to hold the pseudoinverse
	CvMat* imagematrix32f     = cvCreateMat(calibrationPointList.size()*2,12,CV_32F);	// matrix containing points in image space
	CvMat* objectmatrix32f    = cvCreateMat(calibrationPointList.size()*2,1,CV_32F);	// matrix containing points in world space
	CvMat* cameratransform32f = cvCreateMat(12,1,CV_32F);								// matrix containing camera matrix (nonlinear model)
	cameraMatrix32f = cvCreateMat(2,6,CV_32F);											// Create the matrix for the coordinate transformation

	//Compute the calibration matrix
	for(unsigned int i=0; i<calibrationPointList.size(); i++)
	{
		meani.x+=(calibrationPointList.at(i)).xImage;
		meani.y+=(calibrationPointList.at(i)).yImage;
		meano.x+=(calibrationPointList.at(i)).xWorld;
		meano.y+=(calibrationPointList.at(i)).yWorld;
	}
	meani.x/=calibrationPointList.size();
	meani.y/=calibrationPointList.size();
	meano.x/=calibrationPointList.size();
	meano.y/=calibrationPointList.size();
	for(unsigned int i=0; i<calibrationPointList.size(); i++)
	{
		stdi.x+=pow((calibrationPointList.at(i)).xImage-meani.x,2);
		stdi.y+=pow((calibrationPointList.at(i)).yImage-meani.y,2);
		stdo.x+=pow((calibrationPointList.at(i)).xWorld-meano.x,2);
		stdo.y+=pow((calibrationPointList.at(i)).yWorld-meano.y,2);
	}

	stdi.x=sqrt(stdi.x/(calibrationPointList.size()));
	stdi.y=sqrt(stdi.y/(calibrationPointList.size()));
	stdo.x=sqrt(stdo.x/(calibrationPointList.size())); 
	stdo.y=sqrt(stdo.y/(calibrationPointList.size()));
	
	// build up matrices
	for(unsigned int i=0; i<calibrationPointList.size(); i++)
	{

		CvPoint2D32f p;

		p.x=((calibrationPointList.at(i)).xImage-meani.x)/stdi.x;
		p.y=((calibrationPointList.at(i)).yImage-meani.y)/stdi.y;

		imagematrix32f->data.fl[i*12]=p.x;
		imagematrix32f->data.fl[i*12+1]=p.y;
		imagematrix32f->data.fl[i*12+2]=1;
		imagematrix32f->data.fl[i*12+3]=p.x*p.x;
		imagematrix32f->data.fl[i*12+4]=p.y*p.y;
		imagematrix32f->data.fl[i*12+5]=p.x*p.y;
		imagematrix32f->data.fl[i*12+6]=0;
		imagematrix32f->data.fl[i*12+7]=0;
		imagematrix32f->data.fl[i*12+8]=0;
		imagematrix32f->data.fl[i*12+9]=0;
		imagematrix32f->data.fl[i*12+10]=0;
		imagematrix32f->data.fl[i*12+11]=0;
				
		imagematrix32f->data.fl[calibrationPointList.size()*12+i*12]=0;
		imagematrix32f->data.fl[calibrationPointList.size()*12+i*12+1]=0;
		imagematrix32f->data.fl[calibrationPointList.size()*12+i*12+2]=0;
		imagematrix32f->data.fl[calibrationPointList.size()*12+i*12+3]=0;
		imagematrix32f->data.fl[calibrationPointList.size()*12+i*12+4]=0;
		imagematrix32f->data.fl[calibrationPointList.size()*12+i*12+5]=0;
		imagematrix32f->data.fl[calibrationPointList.size()*12+i*12+6]=p.x;
		imagematrix32f->data.fl[calibrationPointList.size()*12+i*12+7]=p.y;
		imagematrix32f->data.fl[calibrationPointList.size()*12+i*12+8]=1;
		imagematrix32f->data.fl[calibrationPointList.size()*12+i*12+9]=p.x*p.x;
		imagematrix32f->data.fl[calibrationPointList.size()*12+i*12+10]=p.y*p.y;
		imagematrix32f->data.fl[calibrationPointList.size()*12+i*12+11]=p.x*p.y;

		objectmatrix32f->data.fl[i]=((calibrationPointList.at(i)).xWorld-meano.x)/stdo.x;
		objectmatrix32f->data.fl[i+calibrationPointList.size()]=((calibrationPointList.at(i)).yWorld-meano.y)/stdo.y;
	}


	CvMat* multransposed    = cvCreateMat(12,12,CV_32F);
	CvMat* invmultransposed = cvCreateMat(12,12,CV_32F);
	CvMat* transposed       = cvCreateMat(12,calibrationPointList.size()*2,CV_32F);

	cvMulTransposed(imagematrix32f,multransposed,1);
	cvInv(multransposed,invmultransposed,CV_LU);
	cvTranspose(imagematrix32f,transposed);
	cvMatMul(invmultransposed,transposed,pseudoinverse32f);
	cvMatMul(pseudoinverse32f, objectmatrix32f, cameratransform32f);
	// copy cameratransformation to appropriate format
	for(int i=0;i<12;i++)
		cameraMatrix32f->data.fl[i]=cameratransform32f->data.fl[i];
	//Release unused matrix
	cvReleaseMat(&pseudoinverse32f); 
	cvReleaseMat(&imagematrix32f);
	cvReleaseMat(&objectmatrix32f);
	cvReleaseMat(&cameratransform32f);

}

void THISCLASS::OnReloadConfiguration() 
{
}

void THISCLASS::OnStep() 
{
	
	// These are the particles to transform
	DataStructureParticles::tParticleVector *particles=mCore->mDataStructureParticles.mParticles;
	if (! particles) {return;}

	// Transform all particle positions
	DataStructureParticles::tParticleVector::iterator it=particles->begin();
	while (it!=particles->end()) 
	{
		it->mCenter=Image2World(it->mCenter);
		it++;
	}
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() 
{
	cvReleaseMat(&cameraMatrix32f);
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

CvPoint2D32f THISCLASS::Image2World(CvPoint2D32f p)
{

	CvPoint2D32f w;	
	
	CvMat* ispace    = cvCreateMat(6,1,CV_32F);
	CvMat* result	 = cvCreateMat(2,1,CV_32F);

	p.x-=meani.x;
	p.y-=meani.y;

	p.x/=stdi.x;
	p.y/=stdi.y;

	ispace->data.fl[0]=p.x;
	ispace->data.fl[1]=p.y;
	ispace->data.fl[2]=1;
	ispace->data.fl[3]=p.x*p.x;
	ispace->data.fl[4]=p.y*p.y;
	ispace->data.fl[5]=p.x*p.y;

	
	cvMatMul(cameraMatrix32f,ispace,result);

	result->data.fl[0]=result->data.fl[0]*stdo.x+meano.x;
	result->data.fl[1]=result->data.fl[1]*stdo.y+meano.y;
	
	w.x=result->data.fl[0];
	w.y=result->data.fl[1];

	cvReleaseMat(&ispace);
	cvReleaseMat(&result);
	return(w);
}

CvPoint2D32f THISCLASS::World2Image(CvPoint2D32f p)
{
	CvMat* invcam = cvCreateMat(3,3,CV_32F);
	CvMat* w      = cvCreateMat(3,1,CV_32F);
	CvMat* u      = cvCreateMat(3,1,CV_32F);
	CvPoint2D32f r;

	w->data.fl[0]=p.x;
	w->data.fl[1]=p.y;
	w->data.fl[2]=1;
	cvInv(cameraMatrix32f,invcam,CV_SVD);
	cvMatMul(invcam,w,u);
	r.x=u->data.fl[0];
	r.y=u->data.fl[1];

	return(r);
}