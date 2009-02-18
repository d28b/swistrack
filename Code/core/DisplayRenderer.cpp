#include "DisplayRenderer.h"
#define THISCLASS DisplayRenderer

#include "ImageConversion.h"
#include <cmath>
#include <iomanip>
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
	mDisplay = display;
	DeleteCache();
}

void THISCLASS::SetScalingFactor(double scalingfactor) {
	mScalingFactor = scalingfactor;
	DeleteCache();
}

void THISCLASS::SetScalingFactorMax(CvSize maxsize) {
	// Get the size of the image
	CvSize srcsize = GetSize();

	// Find out the smallest ratio such that the image fits into [maxwidth, maxheight]
	double rw = (double)(maxsize.width) / (double)(srcsize.width);
	double rh = (double)(maxsize.height) / (double)(srcsize.height);
	double mScalingFactor = 1;
	if (mScalingFactor > rw) {
		mScalingFactor = rw;
	}
	if (mScalingFactor > rh) {
		mScalingFactor = rh;
	}
}

void THISCLASS::SetFlipHorizontal(bool flip) {
	mFlipHorizontal = flip;
	DeleteCache();
}

void THISCLASS::SetFlipVertical(bool flip) {
	mFlipVertical = flip;
	DeleteCache();
}

void THISCLASS::SetCropRectangle(CvRect rect) {
	mCropRectangle = rect;
	DeleteCache();
}

void THISCLASS::DeleteCache() {
	if (! mImage) {
		return;
	}
	cvReleaseImage(&mImage);
	mImage = 0;
}

CvSize THISCLASS::GetSize() {
	if (! mDisplay) {
		return cvSize(300, 200);
	}
	double w = (double)(mDisplay->mSize.width) * mScalingFactor;
	double h = (double)(mDisplay->mSize.height) * mScalingFactor;
	CvSize size = cvSize((int)floor(w + 0.5), (int)floor(h + 0.5));
	if ((size.width <= 0) || (size.height <= 0)) {
		return cvSize(300, 200);
	}
	return size;
}

IplImage *THISCLASS::GetImage() {
	// Return the cached image if possible
	if (mImage) {
		return mImage;
	}

	// Create an empty image
	CvSize size = GetSize();
	mImage = cvCreateImage(size, IPL_DEPTH_8U, 3);
	strcpy(mImage->channelSeq, "RGB");
	memset(mImage->imageData, 255, mImage->imageSize);

	// If the display is null, just display an error message
	if (! mDisplay) {
		DrawMessagePanel(wxT("No display selected."));
		return mImage;
	}

	// Prepare the error list
	ErrorList errors;

	// If the display size is too small, show an error message
	if ((mDisplay->mSize.width < 10) || (mDisplay->mSize.height < 10)) {
		errors.Add(wxT("No image or too small image."));
	}

	// Draw the image
	DrawMainImage(&errors);
	DrawParticles(&errors);
	DrawTrajectories(&errors);
	DrawErrors(&errors);

	return mImage;
}

bool THISCLASS::DrawMainImage(ErrorList *errors) {
	if (! mDrawImage) {
		return false;
	}
	if (! mDisplay) {
		return false;
	}
	if (! mDisplay->mMainImage) {
		return false;
	}

	// Resize and convert the image
	IplImage *mainimage = cvCreateImage(cvSize(mImage->width, mImage->height), IPL_DEPTH_8U, 3);
	IplImage *imagergb = ImageConversion::ToRGB(mDisplay->mMainImage);
	cvResize(imagergb, mainimage, CV_INTER_LINEAR);
	if (mDisplay->mMainImage != imagergb) {
		cvReleaseImage(&imagergb);
	}

	// Draw this main image
	if ((mUseMask) && (mDisplay->mMaskImage)) {
		// Resize mask image
		IplImage *resizedmaskimage = cvCreateImage(cvSize(mImage->width, mImage->height), IPL_DEPTH_8U, 1);
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
	if (! mDrawParticles) {
		return false;
	}
	if (! mDisplay) {
		return false;
	}

	// Draw particles
	DataStructureParticles::tParticleVector::iterator it = mDisplay->mParticles.begin();
	while (it != mDisplay->mParticles.end()) {
		int x = (int)floor(it->mCenter.x * mScalingFactor + 0.5);
		int y = (int)floor(it->mCenter.y * mScalingFactor + 0.5);
		cvRectangle(mImage, cvPoint(x - 2, y - 2), cvPoint(x + 2, y + 2), cvScalar(192, 0, 0), 1);

		float c = cosf(it->mOrientation) * 8; //cosf(it->mOrientation/57.29577951)*20; // TODO: mOrientation contains an angle (rad), and the appropriate conversion to rad should be done when filling this structure
		float s = sinf(it->mOrientation) * 8; //sinf(it->mOrientation/57.29577951)*20;
		cvLine(mImage, cvPoint(x, y), cvPoint(x + (int)floorf(c + 0.5), y + (int)floorf(s + 0.5)), cvScalar(192, 0, 0), 1);

		wxString label = wxString::Format(wxT("%d [%f]"), it->mID, it->mIDCovariance);
		cvPutText(mImage, label.mb_str(wxConvISO8859_1), cvPoint(x + 12, y + 10), &mFontMain, cvScalar(255, 0, 0));
		it++;
	}

	return true;
}

bool THISCLASS::DrawTrajectories(ErrorList *errors) {
	if (! mDrawTrajectories) {
		return false;
	}
	if (! mDisplay) {
		return false;
	}
	if (! mDisplay->mTrajectories) {
		return false;
	}

	// Draw trajectories
	DataStructureTracks::tTrackMap *tracks = mDisplay->mComponent->GetSwisTrackCore()->mDataStructureTracks.mTracks;
	DataStructureTracks::tTrackMap::iterator it = tracks->begin();
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_PLAIN,1.0, 1.0);

	while (it != tracks->end()) {
		// Color for this track
		CvScalar color = cvScalar((it->first * 50) % 255, 
					  (it->first * 50) % 255, 
					  255);

		// Draw trajectory polyline
		std::vector<CvPoint2D32f>::iterator p = it->second.trajectory.begin();
		int xprev = (int)floor((*p).x * mScalingFactor + 0.5);
		int yprev = (int)floor((*p).y * mScalingFactor + 0.5);
		while (p != it->second.trajectory.end()) {
			int x = (int)floor((*p).x * mScalingFactor + 0.5);
			int y = (int)floor((*p).y * mScalingFactor + 0.5);
			cvLine(mImage, cvPoint(xprev, yprev), cvPoint(x, y), color, 3);
			xprev = x;
			yprev = y;
			p++;
		}

		cvRectangle(mImage, cvPoint(xprev - 2, yprev - 2), cvPoint(xprev + 2, yprev + 2), color, 1);
		wxString text;
		text << it->first;
		cvPutText(mImage, text.ToAscii(), cvPoint(xprev + 3, yprev + 3),
			  &font, color);
		it++;
	}

	return true;
}
bool THISCLASS::DrawErrors(ErrorList *errors) {
	if (! mDrawErrors) {
		return false;
	}
	if (! mDisplay) {
		return false;
	}

	// The first line starts at the bottom
	int y = mImage->height - 6;

	// Draw all error messages
	ErrorList::tList::iterator it = errors->mList.begin();
	while (it != errors->mList.end()) {
		cvPutText(mImage, (*it).mMessage.mb_str(wxConvISO8859_1), cvPoint(4, y), &mFontMain, cvScalar(0, 0, 255));
		y += 20;
		it++;
	}

	// Draw all error messages
	it = mDisplay->mErrors.mList.begin();
	while (it != mDisplay->mErrors.mList.end()) {
		cvPutText(mImage, (*it).mMessage.mb_str(wxConvISO8859_1), cvPoint(4, y), &mFontMain, cvScalar(0, 0, 255));
		y += 20;
		it++;
	}

	return true;
}

bool THISCLASS::DrawMessagePanel(wxString errstr) {
	cvRectangle(mImage, cvPoint(0, 0), cvPoint(mImage->width - 1, mImage->height - 1), cvScalar(0, 0, 0), 1);
	if (errstr.Len() == 0) {
		return true;
	}
	CvSize textsize;
	int ymin;
	cvGetTextSize(errstr.mb_str(wxConvISO8859_1), &mFontMain, &textsize, &ymin);
	cvPutText(mImage, errstr.mb_str(wxConvISO8859_1), cvPoint((mImage->width - textsize.width) / 2, (mImage->height + textsize.height) / 2), &mFontMain, cvScalar(255, 0, 0));
	return true;
}
