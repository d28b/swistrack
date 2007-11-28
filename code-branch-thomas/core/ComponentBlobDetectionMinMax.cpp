#include "ComponentBlobDetectionMinMax.h"
#define THISCLASS ComponentBlobDetectionMinMax

#include <sstream>
#include "DisplayEditor.h"

THISCLASS::ComponentBlobDetectionMinMax(SwisTrackCore *stc):
		Component(stc, "BlobDetectionMinMax"),
		mMinArea(0), mMaxArea(1000000), mMaxNumber(10), mParticles(),
		mDisplayOutput("Output", "Particles") {

	// Data structure relations
	mCategory=&(mCore->mCategoryParticleDetection);
	AddDataStructureRead(&(mCore->mDataStructureImageBinary));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentBlobDetectionMinMax() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
	return;
}

void THISCLASS::OnReloadConfiguration() {
	mMinArea=GetConfigurationInt("MinArea", 1);
	mMaxArea=GetConfigurationInt("MaxArea", 1000);
	mMaxNumber=GetConfigurationInt("MaxNumber", 10);
	mAreaSelection=GetConfigurationBool("AreaBool",false);
	mMinCompactness=GetConfigurationDouble("MinCompactness", 1);
	mMaxCompactness=GetConfigurationDouble("MaxCompactness", 1000);
	mCompactnessSelection=GetConfigurationBool("CompactnessBool",false);
	mMinOrientation=GetConfigurationDouble("MinOrientation", -90);
	mMaxOrientation=GetConfigurationDouble("MaxOrientation", 90);
	mOrientationSelection=GetConfigurationBool("OrientationBool",false);
	// Check for stupid configurations
	if (mMaxNumber<1) {
		AddError("Max number of particles must be greater or equal to 1");
	}

	if (mMinArea>mMaxArea) {
		AddError("The min area must be smaller than the max area.");
	}
}

void THISCLASS::OnStep() {
	std::vector<Particle> rejectedparticles;

	// Get and check input image
	IplImage *inputimage = cvCloneImage(mCore->mDataStructureImageBinary.mImage);
	IplImage *outputImage = mCore->mDataStructureImageBinary.mImage;
	if (! inputimage) {
		AddError("No input image.");
		return;
	}
	if (inputimage->nChannels != 1) {
		AddError("The input image is not a grayscale image.");
		return;
	}
	cvZero(outputImage);

	// We clear the ouput vector
	mParticles.clear();

	// Initialization
	Particle tmpParticle; // Used to put the calculated value in memory
	CvMoments moments; // Used to calculate the moments
	std::vector<Particle>::iterator j; // Iterator used to stock the particles by size

	// We allocate memory to extract the contours from the binary image
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* contour = 0;
	
	
	// Init blob extraxtion
	CvContourScanner blobs = cvStartFindContours(inputimage,storage,sizeof(CvContour),CV_RETR_LIST,CV_CHAIN_APPROX_NONE);

	// This is used to correct the position in case of ROI
	CvRect rectROI;
	if(inputimage->roi != NULL) {
		rectROI = cvGetImageROI(inputimage);
	} else {
		rectROI.x = 0;
		rectROI.y = 0;
	}

	while ((contour=cvFindNextContour(blobs))!=NULL) {
		// Computing the moments
		cvMoments(contour, &moments);

		// Computing particle area
		tmpParticle.mArea=moments.m00;
		tmpParticle.mCenter.x=(float)(rectROI.x + (moments.m10/moments.m00+0.5));  // moments using Green theorem
		tmpParticle.mCenter.y=(float)(rectROI.y + (moments.m01/moments.m00+0.5));  // m10 = x direction, m01 = y direction, m00 = area as edicted in theorem

		// Selection based on area
		if ((mAreaSelection==false)||((tmpParticle.mArea<=mMaxArea) && (tmpParticle.mArea>=mMinArea)))
		{
			tmpParticle.mCompactness=GetContourCompactness(contour);
			if ((mCompactnessSelection==false)||((tmpParticle.mCompactness>mMinCompactness) && (tmpParticle.mCompactness<mMaxCompactness)))
			{
				double tempValue=cvGetCentralMoment(&moments,2,0)-cvGetCentralMoment(&moments,0,2);
				tmpParticle.mOrientation=atan(2*cvGetCentralMoment(&moments,1,1)/(tempValue+sqrt(tempValue*tempValue+4*cvGetCentralMoment(&moments,1,1)*cvGetCentralMoment(&moments,1,1))))*57.29577951;
				if ((mOrientationSelection==false)||(((tmpParticle.mOrientation>mMinOrientation)&&(tmpParticle.mOrientation<mMaxOrientation))||((tmpParticle.mOrientation>mMinOrientation+180)&&(tmpParticle.mOrientation<mMaxOrientation+180))||((tmpParticle.mOrientation>mMinOrientation-180)&&(tmpParticle.mOrientation<mMaxOrientation-180))))
				{
					cvDrawContours(outputImage,contour,cvScalarAll(255),cvScalarAll(255),0,CV_FILLED);
					// Check if we have already enough particles
					if (mParticles.size()==mMaxNumber) 
					{
						// If the particle is bigger than the smallest stored particle, store it, else do nothing
						if (tmpParticle.mArea>mParticles.back().mArea) 
						{
							// Find the place were it must be inserted, sorted by size
							for (j=mParticles.begin(); (j!=mParticles.end()) && (tmpParticle.mArea<(*j).mArea); j++);

							// Fill unused values			
							tmpParticle.mID=-1;
							tmpParticle.mIDCovariance=-1;

							// Insert the particle
							mParticles.insert(j, tmpParticle);
							// Remove the smallest one
							mParticles.pop_back();
						}
					}					
					else 
					{
						// The particle is added at the correct place
						// Find the place were it must be inserted, sorted by size
						for (j=mParticles.begin(); (j!=mParticles.end()) && (tmpParticle.mArea<(*j).mArea); j++);

						// Fill unused values						
						tmpParticle.mID=-1;
						tmpParticle.mIDCovariance=-1;

						// Insert the particle
						mParticles.insert(j, tmpParticle);
					}
				}
			}
		} 
		else 
		{
			rejectedparticles.push_back(tmpParticle);
		}
		cvRelease((void**)&contour);
	}
	contour = cvEndFindContours(&blobs);

	// If we need to display the particles
	/* if(trackingimg->GetDisplay())
	{
		for(j=rejectedparticles.begin();j!=rejectedparticles.end();j++)
		{
			trackingimg->DrawCircle(cvPoint((int)(((*j).p).x),(int)(((*j).p).y)),CV_RGB(255,0,0));
		}
		for(j=particles.begin();j!=particles.end();j++)
		{
			trackingimg->DrawCircle(cvPoint((int)(((*j).p).x),(int)(((*j).p).y)),CV_RGB(0,255,0));
			trackingimg->Cover(cvPoint((int)(((*j).p).x),(int)(((*j).p).y)),CV_RGB(255,0,0),2);
		}
	} */

	cvReleaseImage(&inputimage);
	cvRelease((void**)&contour);
	cvReleaseMemStorage(&storage);

	// Set these particles
	mCore->mDataStructureParticles.mParticles=&mParticles;

	// Let the DisplayImage know about our image
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetParticles(&mParticles);
		de.SetMainImage(mCore->mDataStructureImageBinary.mImage);
	}
}

void THISCLASS::OnStepCleanup() {
	mCore->mDataStructureParticles.mParticles=0;
}

void THISCLASS::OnStop() {
}

double THISCLASS::GetContourCompactness(const void* contour) {
	double l = cvArcLength(contour, CV_WHOLE_SEQ, 1);
	return fabs(12.56*cvContourArea(contour)/(l*l));	
}
