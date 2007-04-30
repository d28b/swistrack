#include "DisplayRenderer.h"
#define THISCLASS DisplayRenderer

#include <sstream>
#include <cmath>
#define PI 3.14159265358979

THISCLASS::DisplayRenderer(Display *display):
		mDisplay(0), mImage(0),
		mScalingFactor(1), mCropRectangle(cvRect(0, 0, 0, 0)), mFlipVertical(false), mFlipHorizontal(false),
		mDrawImage(true), mDrawParticles(true), mDrawErrors(true), mUseMask(true) {

	SetDisplay(display);

	// Initialize font
	cvInitFont(&mFontMain, CV_FONT_HERSHEY_SIMPLEX, 0.4, 0.4);
}

THISCLASS::~DisplayRenderer() {
	DeleteCache();
}

void THISCLASS::SetDisplay(Display *display) {
	mDisplay=display;
	DeleteCache();
}

void THISCLASS::SetScalingFactor(double scalingfactor) {
	mScalingFactor=scalingfactor;
	DeleteCache();
}

void THISCLASS::SetScalingFactorMax(CvSize maxsize) {
	// Get the size of the image
	CvSize srcsize=GetSize();

	// Find out the smallest ratio such that the image fits into [maxwidth, maxheight]
	double rw=(double)(maxsize.width)/(double)(srcsize.width);
	double rh=(double)(maxsize.height)/(double)(srcsize.height);
	double mScalingFactor=1;
	if (mScalingFactor>rw) {mScalingFactor=rw;}
	if (mScalingFactor>rh) {mScalingFactor=rh;}
}

void THISCLASS::SetFlipHorizontal(bool flip) {
	mFlipHorizontal=flip;
	DeleteCache();
}

void THISCLASS::SetFlipVertical(bool flip) {
	mFlipVertical=flip;
	DeleteCache();
}

void THISCLASS::SetCropRectangle(CvRect rect) {
	mCropRectangle=rect;
	DeleteCache();
}

void THISCLASS::DeleteCache() {
	if (! mImage) {return;}
	cvReleaseImage(&mImage);
	mImage=0;
}

CvSize THISCLASS::GetSize() {
	if (! mDisplay) {return cvSize(300, 200);}
	double w=(double)(mDisplay->mSize.width)*mScalingFactor;
	double h=(double)(mDisplay->mSize.height)*mScalingFactor;
	return cvSize((int)floor(w+0.5), (int)floor(h+0.5));
}

IplImage *THISCLASS::GetImage() {
	// Return the cached image if possible
	if (mImage) {return mImage;}

	// Create an empty image
	CvSize size=GetSize();
	mImage=cvCreateImage(size, IPL_DEPTH_8U, 3);
	memset(mImage->imageData, 255, mImage->imageSize);

	// If the display is null, just display an error message
	if (! mDisplay) {
		cvRectangle(mImage, cvPoint(0, 0), cvPoint(size.width-1, size.height-1), cvScalar(0, 0, 0), 1);
		CvSize textsize;
		int ymin;
		std::string str="No display selected.";
		cvGetTextSize(str.c_str(), &mFontMain, &textsize, &ymin);
		cvPutText(mImage, str.c_str(), cvPoint((size.width-textsize.width)/2, (size.height+textsize.height)/2), &mFontMain, cvScalar(255, 0, 0));
		return mImage;
	}

	// Draw the image
	DrawMainImage();
	DrawParticles();
	DrawErrors();

	return mImage;
}

bool THISCLASS::DrawMainImage() {
	if (! mDrawImage) {return false;}
	if (! mDisplay) {return false;}
	if (! mDisplay->mMainImage) {return false;}

	// Resize main image
	IplImage *resizedmainimage=cvCreateImage(cvSize(mImage->width, mImage->height), IPL_DEPTH_8U, 3);
	cvResize(mDisplay->mMainImage, resizedmainimage, CV_INTER_LINEAR);

	// Draw this main image
	if ((mUseMask) && (mDisplay->mMaskImage)) {
		// Resize mask image
		IplImage *resizedmaskimage=cvCreateImage(cvSize(mImage->width, mImage->height), IPL_DEPTH_8U, 3);
		cvResize(mDisplay->mMaskImage, resizedmaskimage, CV_INTER_LINEAR);
		cvCopy(resizedmainimage, mImage, resizedmaskimage);
	} else {
		cvCopy(resizedmainimage, mImage);
	}

	return true;
}

bool THISCLASS::DrawParticles() {
	if (! mDrawParticles) {return false;}
	if (! mDisplay) {return false;}

	// Draw particles
	DataStructureParticles::tParticleVector::iterator it=mDisplay->mParticles.begin();
	while (it!=mDisplay->mParticles.end()) {
		int x=(int)floor(it->mCenter.x*mScalingFactor+0.5);
		int y=(int)floor(it->mCenter.y*mScalingFactor+0.5);
		cvRectangle(mImage, cvPoint(x-2, y-2), cvPoint(x+2, y+2), cvScalar(192, 0, 0), 1);

		float c=cosf(it->mOrientation)*20+(float)0.5;
		float s=sinf(it->mOrientation)*20+(float)0.5;
		cvLine(mImage, cvPoint(x, y), cvPoint(x+(int)floorf(c), y+(int)floorf(s)), cvScalar(192, 0, 0), 1);

		std::ostringstream oss;
		oss << it->mID << " (" << it->mOrientation/PI*180. << ") [" << it->mIDCovariance << "]";
		cvPutText(mImage, oss.str().c_str(), cvPoint(x+12, y+10), &mFontMain, cvScalar(255, 0, 0));
		it++;
	}

	return true;
}

bool THISCLASS::DrawErrors() {
	if (! mDrawErrors) {return false;}
	if (! mDisplay) {return false;}

	// Draw all error messages
	int y=4;
	Display::tErrorList::iterator it=mDisplay->mErrors.begin();
	while (it!=mDisplay->mErrors.end()) {
		cvPutText(mImage, (*it).c_str(), cvPoint(4, y), &mFontMain, cvScalar(0, 0, 255));
		y+=20;
	}

	return true;
}

