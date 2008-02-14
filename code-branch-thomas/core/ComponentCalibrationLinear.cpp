#include "ComponentCalibrationLinear.h"
#define THISCLASS ComponentCalibrationLinear

#include "DisplayEditor.h"
#include "DataStructureParticles.h"
#include <wx/log.h>
#include <fstream>

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

	// Create the matrices
	CvMat* pseudoinverse32f   = cvCreateMat(12,calibrationPointList.size()*2,CV_32F);	// matrix to hold the pseudoinverse
	CvMat* imagematrix32f     = cvCreateMat(calibrationPointList.size()*2,12,CV_32F);	// matrix containing points in image space
	CvMat* objectmatrix32f    = cvCreateMat(calibrationPointList.size()*2,1,CV_32F);	// matrix containing points in world space
	CvMat* cameratransform32f = cvCreateMat(12,1,CV_32F);								// matrix containing camera matrix (nonlinear model)

	// Build up matrices
	for (unsigned int i=0; i<calibrationPointList.size(); i++) {
		CvPoint2D32f p;

		p.x=(calibrationPointList.at(i)).xImage;
		p.y=(calibrationPointList.at(i)).yImage;

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
		
		objectmatrix32f->data.fl[i]=(calibrationPointList.at(i)).xWorld;
		objectmatrix32f->data.fl[i+calibrationPointList.size()]=(calibrationPointList.at(i)).yWorld;
	}

	CvMat* multransposed    = cvCreateMat(12,12,CV_32F);
	CvMat* invmultransposed = cvCreateMat(12,12,CV_32F);
	CvMat* transposed       = cvCreateMat(12,calibrationPointList.size()*2,CV_32F);

	cvMulTransposed(imagematrix32f,multransposed,1);
	cvInv(multransposed,invmultransposed,CV_LU);
	cvTranspose(imagematrix32f,transposed);
	cvMatMul(invmultransposed,transposed,pseudoinverse32f);
	cvMatMul(pseudoinverse32f, objectmatrix32f, cameratransform32f);

	// Copy cameratransformation to appropriate format
	for (int i=0; i<12; i++) {
		cameraMatrix[i]=cameratransform32f->data.fl[i];
	}

	// Release unused matrix
	cvReleaseMat(&pseudoinverse32f); 
	cvReleaseMat(&imagematrix32f);
	cvReleaseMat(&objectmatrix32f);
	cvReleaseMat(&cameratransform32f);

	//Compute error of calibration on calibration points
	double errorX,errorY,maxErrorX,maxErrorY,maxErrorD,errorD;
	maxErrorX=0;
	maxErrorY=0;
	errorX=0;
	errorY=0;
	maxErrorD=0;
	errorD=0;

	for (unsigned int i=0; i<calibrationPointList.size(); i++) 
	{
		double tmpErrorX,tmpErrorY,tmpErrorD;
		CvPoint2D32f originalImage,originalWorld,finalWorld;
		originalImage.x=(float)(calibrationPointList.at(i)).xImage;
		originalImage.y=(float)(calibrationPointList.at(i)).yImage;
		originalWorld.x=(float)(calibrationPointList.at(i)).xWorld;
		originalWorld.y=(float)(calibrationPointList.at(i)).yWorld;
		finalWorld=THISCLASS::Image2World(originalImage);
		tmpErrorX=abs(originalWorld.x-finalWorld.x);
		tmpErrorY=abs(originalWorld.y-finalWorld.y);
		tmpErrorD=sqrt(tmpErrorX*tmpErrorX+tmpErrorY*tmpErrorY);
		errorX+=tmpErrorX;
		errorY+=tmpErrorY;
		errorD+=tmpErrorD;
		if (tmpErrorX>maxErrorX)
			maxErrorX=tmpErrorX;
		if (tmpErrorY>maxErrorY)
			maxErrorY=tmpErrorY;
		if (tmpErrorD>maxErrorD)
			maxErrorD=tmpErrorD;
	}
	errorX=errorX/calibrationPointList.size();
	errorY=errorY/calibrationPointList.size();
	errorD=errorD/calibrationPointList.size();
	
	//Write error into calibration.log
	std::fstream outputFile;
	outputFile.open("calibration.log",std::fstream::out | std::fstream::trunc);
	outputFile<<"Calibration error"<<std::endl;
	outputFile<<"Max error in X: \t"<<maxErrorX<<std::endl;
	outputFile<<"Max error in Y: \t"<<maxErrorY<<std::endl;
	outputFile<<"Average error in X: \t"<<errorX<<std::endl;
	outputFile<<"Average error in Y: \t"<<errorY<<std::endl;
	outputFile<<"Max distance error: \t"<<errorY<<std::endl;
	outputFile<<"Average distance error: \t"<<errorY<<std::endl;
	outputFile.close();
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
	// These are the particles to transform
	DataStructureParticles::tParticleVector *particles=mCore->mDataStructureParticles.mParticles;
	if (! particles) {return;}

	// Transform all particle positions
	DataStructureParticles::tParticleVector::iterator it=particles->begin();
	while (it!=particles->end()) {
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

CvPoint2D32f THISCLASS::Image2World(CvPoint2D32f p) {
	CvPoint2D32f w;
	float pxx,pxy,pyy;
	pxx=p.x*p.x;
	pyy=p.y*p.y;
	pxy=p.x*p.y;
	w.x=cameraMatrix[0]*p.x+cameraMatrix[1]*p.y+cameraMatrix[2]+cameraMatrix[3]*pxx+cameraMatrix[4]*pyy+cameraMatrix[5]*pxy;
	w.y=cameraMatrix[6]*p.x+cameraMatrix[7]*p.y+cameraMatrix[8]+cameraMatrix[9]*pxx+cameraMatrix[10]*pyy+cameraMatrix[11]*pxy;
	return(w);
}
