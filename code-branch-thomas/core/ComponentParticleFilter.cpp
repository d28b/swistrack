#include "ComponentParticleFilter.h"
#define THISCLASS ComponentParticleFilter

THISCLASS::ComponentParticleFilter(SwisTrackCore *stc):
		Component(stc, "ParticleFilter"), mCapture(0), mLastImage(0) {

	// User-friendly information about this component
	mDisplayName="Particle filter";
	AddDataStructureWrite(mCore->mDataStructureImageBinary);
	AddDataStructureWrite(mCore->mDataStructureParticles);
}

THISCLASS::~ComponentParticleFilter() {
}

bool THISCLASS::Start() {
	mMinArea=GetConfigurationDouble("MinArea");
	mMaxArea=GetConfigurationDouble("MaxArea");
	mMaxNumber=GetConfigurationInt("MaxNumber");
	
	//Control if max_number is not to small
	if (mMaxNumber<1) {
		AddError("Max number of particles must be greater or equal to 1");
		return false;
	}

	return true;
}

bool THISCLASS::Step() {
	std::vector<particle> rejectedparticles;

	// We get the binary image from the segmenter, we make a copy because the image is modified during blobs extraction
	IplImage* src_tmp = cvCloneImage(mCore->mDataStructureImageBinary.mImage);
	
	// Perform a morphological opening to reduce noise.
	if (mFirstDilate) {
		cvDilate(src_tmp, src_tmp, NULL, mFirstDilate);
	}
	if (mFirstErode) {
		cvErode(src_tmp, src_tmp, NULL, mFirstErode);
	}
	if (mSecondDilate) {
		cvDilate(src_tmp, src_tmp, NULL, mSecondDilate);
	}

	// We clear the ouput vector
	Particle tmpParticle; // Used to put the calculated value in memory
	CvMoments moments; // Used to calculate the moments
	std::vector<particle>::iterator j; // Iterator used to stock the particles by size

	// We allocate memory to extract the contours from the binary image
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* contour = 0;

	// Init blob extraxtion
	CvContourScanner blobs = cvStartFindContours(src_tmp,storage,sizeof(CvContour),CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);

	// This is used to correct the position in case of ROI
	CvRect rectROI;
	if(src_tmp->roi != NULL) {
		rectROI = cvGetImageROI(src_tmp);
	} else {
		rectROI.x = 0;
		rectROI.y = 0;
	}

	while ((contour=cvFindNextContour(blobs))!=NULL) {
		// Calculating the moments
		cvMoments(contour, &moments);

		// Calculating particle area
		tmpParticle.mArea=moments.m00;
		tmpParticle.mPosition.x=(float)(rectROI.x + (moments.m10/moments.m00+0.5));  // moments using Green theorem
		tmpParticle.mPosition.y=(float)(rectROI.y + (moments.m01/moments.m00+0.5));  // m10 = x direction, m01 = y direction, m00 = area as edicted in theorem

		// Selection based on area
		if ((tmpParticle.mArea<=mMaxArea) && (tmpParticle.mArea>=mMinArea)) {
			// Check if we have already enough particles
			if (particles.size()==mMaxNumber) {
				// If the particle is bigger than the smallest stored particle, store it, else do nothing
				if (tmpParticle.mArea>mCore->mDataStructureParticles.mParticles.back().mArea) {
					// Find the place were it must be inserted, sorted by size
					for (j=mCore->mDataStructureParticles.mParticles.begin(); (j!=mCore->mDataStructureParticles.mParticles.end()) && (tmpParticle.mArea<(*j).mArea); j++);

					// Calculate the important values
					//tmpParticle.mPosition.x=(float)(rectROI.x + (moments.m10/moments.m00+0.5));  // moments using Green theorema
					//tmpParticle.mPosition.y=(float)(rectROI.y + (moments.m01/moments.m00+0.5));  // m10 = x direction, m01 = y direction, m00 = area as edicted in theorem
					tmpParticle.mCompactness=GetContourCompactness(contour);
					tmpParticle.mID=-1;
					tmpParticle.mOrientation=0;

					// Insert the particle
					mCore->mDataStructureParticles.mParticles.insert(j, tmpParticle);
					// Remove the smallest one
					mCore->mDataStructureParticles.mParticles.pop_back();
				}
			} else { // The particle is added at the correct place
				// Find the place were it must be inserted, sorted by size
				for (j=mCore->mDataStructureParticles.mParticles.begin(); (j!=mCore->mDataStructureParticles.mParticles.end()) && (tmpParticle.mArea<(*j).mArea); j++);

				// Caculate the important values
				tmpParticle.mCompactness=GetContourCompactness(contour);
				tmpParticle.mID=-1;
				tmpParticle.mOrientation=0;

				// Insert the particle
				mCore->mDataStructureParticles.mParticles.insert(j, tmpParticle);
			}
		} else {
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

	cvReleaseImage(&src_tmp);
	cvRelease((void**)&contour);
	cvReleaseMemStorage(&storage);
	return true;
}

bool THISCLASS::StepCleanup() {
	mCore->mDataStructureParticles.mParticles.clear();
	return true;
}

bool THISCLASS::Stop() {
	return true;
}

double THISCLASS::GetContourCompactness(const void* contour) {
	double l = cvArcLength(contour, CV_WHOLE_SEQ, 1);
	return fabs(12.56*cvContourArea(contour)/(l*l));	
}
