#include "ComponentBlobDetectionTwoColors.h"
#define THISCLASS ComponentBlobDetectionTwoColors

#include <sstream>
#include "DisplayEditor.h"
#define PI 3.14159265358979

THISCLASS::ComponentBlobDetectionTwoColors(SwisTrackCore *stc):
		Component(stc, "BlobDetectionTwoColors"),
		mMinArea(0), mMaxArea(1000000), mMaxNumber(10), mParticles(),
		mDisplayColor1("Output", "Particles of color 1"),
		mDisplayColor2("Output", "Particles of color 2"),
		mDisplayOutput("Output", "Particles") {

	// Data structure relations
	mCategory=&(mCore->mCategoryParticleDetection);
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
	AddDisplay(&mDisplayColor1);
	AddDisplay(&mDisplayColor2);
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentBlobDetectionTwoColors() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
	return;
}

void THISCLASS::OnReloadConfiguration() {
	mMaxNumberOfParticles=GetConfigurationInt("mMaxNumberOfParticles", 10);
	mMaxDistance=GetConfigurationDouble("MaxDistance", 10.);
	mSelectionByArea=GetConfigurationBool("SelectionByArea",false);
	mAreaMin=GetConfigurationInt("AreaMin", 1);
	mAreaMax=GetConfigurationInt("AreaMax", 1000);
	mSelectionByCompactness=GetConfigurationBool("SelectionByCompactness",false);
	mCompactnessMin=GetConfigurationDouble("CompactnessMin", 1.);
	mCompactnessMax=GetConfigurationDouble("CompactnessMax", 1000.);

	// Check for stupid configurations
	if (mMaxNumberOfParticles<1) {
		AddError("The maximum number of particles must be greater or equal to 1.");
	}

	if (mAreaMin>mAreaMax) {
		AddError("The minimum area must be smaller than the maximum area.");
	}

	if (mMaxDistance<0) {
		AddError("The maximum distane must be equal to 0.");
	}

	if (mCompactnessMin>mCompactnessMax) {
		AddError("The minimum compactness must be small than the maximum compactness.");
	}
}

void THISCLASS::OnStep() {
	// Get and check input image
	IplImage *inputimage = mCore->mDataStructureImageColor.mImage;
	if (! inputimage) {
		AddError("No input image.");
		return;
	}
	if (inputimage->nChannels != 3) {
		AddError("The input image is not a color image.");
		return;
	}

	// Detect the blobs of the two colors
	DataStructureParticles::tParticleVector particles_1;
	FindColorBlobs(inputimage, mColor1, &particles_1, &mDisplayColor1);
	DataStructureParticles::tParticleVector particles_2;
	FindColorBlobs(inputimage, mColor2, &particles_2, &mDisplayColor2);

	// We clear the output vector
	mParticles.clear();

	// Match blobs of color 1 to blobs of color 2
	for (std::vector<Particle>::iterator i=particlevector_1.begin(); i!=particlevector_1.end(); i++) {
		// Select blob of color 2 which is closest
		std::vector<Particle>::iterator k_min=particlevector_2.end();
		double distance2_min = mMaxDistance +1;
		for (std::vector<Particle>::iterator k=particlevector_2.begin(); k!=particlevector_2.end(); k++) {
			int distance2=pow((*i).mCenter.x-(*k).mCenter.x, 2.0)+pow((*i).mCenter.y-(*k).mCenter.y), 2.0);
			
			if (distance2 < k_min_distance2) {
				k_min = k;
				k_min_distance2 = distance2;	
			}
		}

		// Create particle with this combination of blobs		
		if (k_min!=particlevector_2.end()) {
			Particle newparticle;
			newparticle.mArea=(*k_min).mArea+(*i).mArea;
			newparticle.mCenter.x=((*k_min).mCenter.x+(*i).mCenter.x)*0.5;
			newparticle.mCenter.y=((*k_min).mCenter.y+(*i).mCenter.y)*0.5;
			newparticle.mOrientation=atan2((*k_min).mCenter.y - (*i).mCenter.y, (*k_min).mCenter.x - (*i).mCenter.x);
			mParticles.push_back(newparticle);
		}
	}

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

void FindColorBlobs(IplImage *colorimage, cvScalar color, DataStructureParticles::tParticleVector &particlevector, Display &display) {
	// Take a copy of the input image
	IplImage *inputimage=cvCloneImage(colorimage);

	// Subtract the color
	cvAbsDiffS(inputimage, inputimage, color);

	// Split the image into channels
	IplImage* imagechannels[3];
	imagechannels[0]=cvCreateImage(cvGetSize(inputimage),8,1);
	imagechannels[1]=cvCreateImage(cvGetSize(inputimage),8,1);
	imagechannels[2]=cvCreateImage(cvGetSize(inputimage),8,1);
	cvSplit(inputimage, imagechannels[0], imagechannels[1], imagechannels[2], NULL);

	// Threshold each channel
	for (int i=0;i<3;i++) {
		switch(inputimage->channelSeq[i]) {
		case 'B':
			cvThreshold(imagechannels[i], imagechannels[i], mBlueThreshold, 255, CV_THRESH_BINARY);
			break;
		case 'G':
			cvThreshold(imagechannels[i], imagechannels[i], mGreenThreshold, 255, CV_THRESH_BINARY);
			break;
		case 'R':
			cvThreshold(imagechannels[i], imagechannels[i], mRedThreshold, 255, CV_THRESH_BINARY);
			break;
		default:
			AddWarning("Invalid channel (other than R, G or B) found in input image.");
		}
	}

	// Combine channels (ch0 & ch1 -> ch0, ch0 & ch2 -> ch0)
	cvAnd(imagechannels[0], imagechannels[1], imagechannels[0]);
	cvAnd(imagechannels[0], imagechannels[2], imagechannels[0]);

	// Do blob detection
	FindBlobs(imagechannels[0], &particlevector);

	// Let the DisplayImage know about our image
	DisplayEditor de(&display);
	if (de.IsActive()) {
		de.SetParticles(&particlevector);
		de.SetMainImage(&imagechannels[0]);
	}

	// Release temporary images
	cvReleaseImage(&inputimage);
	cvReleaseImage(&imagechannels[0]);
	cvReleaseImage(&imagechannels[1]);
	cvReleaseImage(&imagechannels[2]);
}

void FindBlobs(IplImage *inputimage, DataStructureParticles::tParticleVector &particlevector) {
	// Init blob extraxtion
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvContourScanner blobs = cvStartFindContours(inputimage,storage,sizeof(CvContour),CV_RETR_LIST,CV_CHAIN_APPROX_NONE);

	// Iterate over blobs
	while (1) {
		// Get next contour (if one exists)
		CvSeq* contour=cvFindNextContour(blobs);
		if (! contour) {
			break;
		}

		// Compute the moments
		CvMoments moments;
		cvMoments(contour, &moments);

		// Compute particle position
		Particle newparticle;
		newparticle.mArea=moments.m00;
		newparticle.mCenter.x=(float)(moments.m10/moments.m00+0.5);  // moments using Green theorem
		newparticle.mCenter.y=(float)(moments.m01/moments.m00+0.5);  // m10 = x direction, m01 = y direction, m00 = area as edicted in theorem

		// Selection based on area
		if ((mAreaSelection==false)||((tmpParticle.mArea<=mMaxArea) && (tmpParticle.mArea>=mMinArea))) {
			tmpParticle.mCompactness=GetContourCompactness(contour);
			if ((mCompactnessSelection==false)||((tmpParticle.mCompactness>mMinCompactness) && (tmpParticle.mCompactness<mMaxCompactness))) {
				double centralmoment=cvGetCentralMoment(&moments,2,0)-cvGetCentralMoment(&moments,0,2);
				tmpParticle.mOrientation=atan(2*cvGetCentralMoment(&moments,1,1)/(centralmoment+sqrt(centralmoment*centralmoment+4*cvGetCentralMoment(&moments,1,1)*cvGetCentralMoment(&moments,1,1))));

				// Fill unused values
				newparticle.mID=-1;
				newparticle.mIDCovariance=-1;

				// Insert the particle at the right place, such that the list remains sorted (note that one could use a heap here to lower the complexity)
				std::vector<Particle>::iterator j;
				for (j=particlevector.begin(); (j!=particlevector.end()) && (newparticle.mArea<(*j).mArea); j++);
				particlevector.insert(j, tmpParticle);

				// Remove particles if we have too many of them
				while (particlevector.size()>mMaxNumber) {
					// Remove the smallest one
					particlevector.pop_back();
				}
			}
		}
		
		// Release the contour
		cvRelease((void**)&contour);
	}

	// Finalize blob extraction
	cvEndFindContours(&blobs);
	cvReleaseMemStorage(&storage);
}
