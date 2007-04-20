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
}

CvSize THISCLASS::GetSize(double scalingfactor) {
	double w=(double)(mImage->width)*scalingfactor;
	double h=(double)(mImage->height)*scalingfactor;
	return cvSize((int)floor(w+0.5), (int)floor(h+0.5));
}

IplImage *THISCLASS::GetImage(double scalingfactor) {
	// Return the cached image if possible
	if (mCachedViewScalingFactor==scalingfactor) {
		return mCachedViewImage;
	}

	// Delete the old cached view image
	cvReleaseImage(&mCachedViewImage);

	// Create a resized copy of the image
	CvSize size=CalculateSize(scalingfactor);
	mCachedViewImage=cvCreateImage(size, IPL_DEPTH_8U, mImage->nChannels);
	if (mImage) {
		cvResize(mImage, mCachedViewImage);
	} else {
		memset(mCachedViewImage->imageData, 255, mCachedViewImage->imageSize);
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
			cvRectangle(mCachedViewImage, cvPoint(x-2, y-2), cvPoint(x+2, y+2), cvScalar(192, 0, 0), 1);

			float c=cosf(it->mOrientation)*20+(float)0.5;
			float s=sinf(it->mOrientation)*20+(float)0.5;
			cvLine(mCachedViewImage, cvPoint(x, y), cvPoint(x+(int)floorf(c), y+(int)floorf(s)), cvScalar(192, 0, 0), 1);

			std::ostringstream oss;
			oss << it->mID << " (" << it->mOrientation/PI*180. << ") [" << it->mIDCovariance << "]";
			cvPutText(mCachedViewImage, oss.str().c_str(), cvPoint(x+12, y+10), &font, cvScalar(255, 0, 0));
			it++;
		}
	}

	return mCachedViewImage;
}