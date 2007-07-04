#include "DisplayRenderer.h"
#define THISCLASS DisplayRenderer

#include "ImageConversion.h"
#include <sstream>
#include <cmath>
#define PI 3.14159265358979

THISCLASS::DisplayRenderer(Display *display):
		mDisplay(0), mImage(0),
		mScalingFactor(1), mCropRectangle(cvRect(0, 0, 0, 0)), mFlipVertical(false), mFlipHorizontal(false),
		mDrawImage(true), mDrawParticles(true), mDrawTrajectories(true), mDrawErrors(true), mUseMask(true) {

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
	CvSize size=cvSize((int)floor(w+0.5), (int)floor(h+0.5));
	if ((size.width<=0) || (size.height<=0)) {return cvSize(300, 200);}
	return size;
}

IplImage *THISCLASS::GetImage() {
	// Return the cached image if possible
	if (mImage) {return mImage;}

	// Create an empty image
	CvSize size=GetSize();
	mImage=cvCreateImage(size, IPL_DEPTH_8U, 3);
	strcpy(mImage->channelSeq,"RGB");
	memset(mImage->imageData, 255, mImage->imageSize);

	// If the display is null, just display an error message
	if (! mDisplay) {
		DrawMessagePanel("No display selected.");
		return mImage;
	}

	// Prepare the error list
	ErrorList errors;

	// If the display size is too small, show an error message
	if ((mDisplay->mSize.width<10) || (mDisplay->mSize.height<10)) {
		errors.Add("No image or too small image.");
	}

	// Draw the image
	DrawMainImage(&errors);
	DrawParticles(&errors);
	DrawTrajectories(&errors);
	DrawErrors(&errors);

	return mImage;
}

bool THISCLASS::DrawMainImage(ErrorList *errors) {
	if (! mDrawImage) {return false;}
	if (! mDisplay) {return false;}
	if (! mDisplay->mMainImage) {return false;}

	// Resize and convert the image
	IplImage *mainimage=cvCreateImage(cvSize(mImage->width, mImage->height), IPL_DEPTH_8U, 3);
	IplImage *imagergb=ImageConversion::ToRGB(mDisplay->mMainImage);
	cvResize(imagergb, mainimage, CV_INTER_LINEAR);
	if (mDisplay->mMainImage!=imagergb) {cvReleaseImage(&imagergb);}

	// Draw this main image
	if ((mUseMask) && (mDisplay->mMaskImage)) {
		// Resize mask image
		IplImage *resizedmaskimage=cvCreateImage(cvSize(mImage->width, mImage->height), IPL_DEPTH_8U, 1);
		cvResize(mDisplay->mMaskImage, resizedmaskimage, CV_INTER_LINEAR);
		cvCopy(mainimage, mImage, resizedmaskimage);
		cvReleaseImage(&resizedmaskimage);
	} else {
		cvCopy(mainimage, mImage);
	}

	cvReleaseImage(&mainimage);
	return true;
}

bool THISCLASS::DrawParticles(ErrorList *errors) {
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

bool THISCLASS::DrawTrajectories(ErrorList *errors) {
	if (! mDrawTrajectories) {return false;}
	if (! mDisplay) {return false;}
	if (! mDisplay->mTrajectories) {return false;}

	// Draw trajectories
	DataStructureTracks::tTrackVector *tracks=mDisplay->mComponent->GetSwisTrackCore()->mDataStructureTracks.mTracks;
	DataStructureTracks::tTrackVector::iterator it=tracks->begin();
	while (it!=tracks->end()) {
		int x=(int)floor(it->trajectory.back().x*mScalingFactor+0.5);
		int y=(int)floor(it->trajectory.back().y*mScalingFactor+0.5);
		
		// draw every single point
		std::vector<CvPoint2D32f>::iterator p;
		for( p = it->trajectory.begin()+1; p != it->trajectory.end(); p++ ){
			cvLine(mImage,cvPoint((*p).x*mScalingFactor,(*p).y*mScalingFactor),cvPoint((*(p-1)).x*mScalingFactor,(*(p-1)).y*mScalingFactor),cvScalar((it->mID*50)%255,(it->mID*50)%255,(it->mID*50)%255));
		}
		cvRectangle(mImage, cvPoint(x-2, y-2), cvPoint(x+2, y+2), cvScalar(192, 0, 0), 1);
		it++;
	}

	return true;
}
bool THISCLASS::DrawErrors(ErrorList *errors) {
	if (! mDrawErrors) {return false;}
	if (! mDisplay) {return false;}

	// The first line starts at the bottom
	int y=mImage->height-6;

	// Draw all error messages
	ErrorList::tList::iterator it=errors->mList.begin();
	while (it!=errors->mList.end()) {
		cvPutText(mImage, (*it).mMessage.c_str(), cvPoint(4, y), &mFontMain, cvScalar(0, 0, 255));
		y+=20;
		it++;
	}

	// Draw all error messages
	it=mDisplay->mErrors.mList.begin();
	while (it!=mDisplay->mErrors.mList.end()) {
		cvPutText(mImage, (*it).mMessage.c_str(), cvPoint(4, y), &mFontMain, cvScalar(0, 0, 255));
		y+=20;
		it++;
	}

	return true;
}

bool THISCLASS::DrawMessagePanel(std::string errstr) {
	cvRectangle(mImage, cvPoint(0, 0), cvPoint(mImage->width-1, mImage->height-1), cvScalar(0, 0, 0), 1);
	if (errstr.length()==0) {return true;}
	CvSize textsize;
	int ymin;
	cvGetTextSize(errstr.c_str(), &mFontMain, &textsize, &ymin);
	cvPutText(mImage, errstr.c_str(), cvPoint((mImage->width-textsize.width)/2, (mImage->height+textsize.height)/2), &mFontMain, cvScalar(255, 0, 0));
	return true;
}
