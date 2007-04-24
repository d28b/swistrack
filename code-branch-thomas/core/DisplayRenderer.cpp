#include "DisplayRenderer.h"
#define THISCLASS DisplayRenderer

THISCLASS::DisplayRenderer(Display *display):
		mDisplay(0), mImage(0),
		mScalingFactor(1), mCropRectangle(cvRect(0, 0, 0, 0)), mViewFlipVertically(false), mViewFlipHorizontally(false),
		mDrawImage(true), mDrawMask(true), mDrawParticles(true) {

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

void THISCLASS::SetScalingFactorMax(int maxwidth, int maxheight) {
	// Get the size of the image
	CvSize srcsize=GetSize();

	// Find out the smallest ratio such that the image fits into [maxwidth, maxheight]
	double rw=(double)(maxwidth)/(double)(srcsize.width);
	double rh=(double)(maxheight)/(double)(srcsize.height);
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
	if (! mDisplay) {return cvSize(100, 100);}
	double w=(double)(mDisplay->mSize->width)*scalingfactor;
	double h=(double)(mDisplay->mSize->height)*scalingfactor;
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
		cvPutText(mImage, "No display selected.", cvPoint(4, 4), &mFontMain, cvScalar(255, 0, 0));
		return mImage;
	}

	// Draw the image
	DrawImage()
	DrawMask();
	DrawParticles();
	DrawErrors();

	return mImage;
}

bool THISCLASS::DrawImage() {
	if (! mDrawImage) {return false;}
	if (! mDisplay) {return false;}
	if (! mDisplay->mMainImage) {return false;}

	cvResize(mDisplay->mMainImage, mImage);
	return true;
}

bool THISCLASS::DrawMask() {
	if (! mDrawMask) {return false;}
	if (! mDisplay) {return false;}
	if (! mDisplay->mMaskImage) {return false;}

	IplImage resizedmask=cvCreateImage(size, IPL_DEPTH_8U, 1);
	cvResize(mDisplay->mMaskImage, resizedmask);
	cvCopy(resizedmask, mImage, some_combination);
	return true;
}

bool THISCLASS::DrawParticles() {
	if (! mDrawParticles) {return false;}
	if (! mDisplay) {return false;}
	if (! mDisplay->mParticles) {return false;}

	// Draw particles
	DataStructureParticles::tParticleVector::iterator it=mDisplay->mParticles->begin();
	while (it!=mParticles->end()) {
		int x=(int)floor(it->mCenter.x*scalingfactor+0.5);
		int y=(int)floor(it->mCenter.y*scalingfactor+0.5);
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
	Display::tErrorList::iterator it=mDisplay.mErrors.begin();
	while (it!=mDisplay.mErrors.end()) {
		cvPutText(mImage, (*it), cvPoint(4, y), &mFontMain, cvScalar(0, 0, 255));
		y+=20;
	}
}

