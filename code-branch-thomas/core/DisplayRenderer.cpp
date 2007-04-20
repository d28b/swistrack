#include "DisplayRenderer.h"
#define THISCLASS DisplayRenderer

THISCLASS::DisplayRenderer(Display *display): mDisplay(0) {
	if (! display) {return;}

	// If the display isn't active (i.e. no subscribers) we don't need to construct it.
	if (! display->IsActive()) {return;}

	// If everything is all right, we agree to work.
	mDisplay=display;
}

THISCLASS::~DisplayRenderer() {
	if (mImage) {
		cvReleaseImage(&mImage);
		mImage=0;
	}
}

void SetScalingFactor(double scalingfactor) {
	mScalingFactor=scalingfactor;
	cvReleaseImage(&mImage);
	mImage=0;
}

CvSize THISCLASS::GetSize() {
	double w=(double)(mImage->width)*scalingfactor;
	double h=(double)(mImage->height)*scalingfactor;
	return cvSize((int)floor(w+0.5), (int)floor(h+0.5));
}

CvSize THISCLASS::CalculateSize() {
	// If we have an image, use this as reference
	if (mDisplay->mImage) {
		return cvSize(mDisplay->mImage->width, mDisplay->mImage->height);
	}

	// Otherwise, use the size that was fixed by
	
}

IplImage *THISCLASS::GetImage() {
	// Return the cached image if possible
	if (mImage) {return mImage;}

	// Calculate the dimensions of our area
	CvSize 
	
	// Create a resized copy of the image
	CvSize size=CalculateSize(scalingfactor);
	mImage=cvCreateImage(size, IPL_DEPTH_8U, mImage->nChannels);
	if (mImage) {
		cvResize(mImage, mImage);
	} else {
		memset(mImage->imageData, 255, mImage->imageSize);
	}

	// Initialize font
	if (mParticles) {
		CvFont font;
		cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.4, 0.4);

		// Draw particles
		DataStructureParticles::tParticleVector::iterator it=mParticles->begin();
		while (it!=mParticles->end()) {
			int x=(int)floor(it->mCenter.x*scalingfactor+0.5);
			int y=(int)floor(it->mCenter.y*scalingfactor+0.5);
			cvRectangle(mImage, cvPoint(x-2, y-2), cvPoint(x+2, y+2), cvScalar(192, 0, 0), 1);

			float c=cosf(it->mOrientation)*20+(float)0.5;
			float s=sinf(it->mOrientation)*20+(float)0.5;
			cvLine(mImage, cvPoint(x, y), cvPoint(x+(int)floorf(c), y+(int)floorf(s)), cvScalar(192, 0, 0), 1);

			std::ostringstream oss;
			oss << it->mID << " (" << it->mOrientation/PI*180. << ") [" << it->mIDCovariance << "]";
			cvPutText(mImage, oss.str().c_str(), cvPoint(x+12, y+10), &font, cvScalar(255, 0, 0));
			it++;
		}
	}

	return mImage;
}