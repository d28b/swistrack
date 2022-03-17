#include "DisplayRenderer.h"
#define THISCLASS DisplayRenderer

#include "ImageTools.h"
#include <cmath>
#include <iomanip>

THISCLASS::DisplayRenderer(Display * display):
	mDisplay(0), mImage(), mFontFace(cv::FONT_HERSHEY_SIMPLEX), mFontSize(0.4), mFontSizeBig(1.0),
	mScalingFactor(1), mCropRectangle(0, 0, 0, 0), mFlipVertically(false), mFlipHorizontally(false),
	mDrawImage(true), mDrawParticles(true), mDrawTrajectories(true), mDrawErrors(true), mUseMask(true) {

	SetDisplay(display);
}

THISCLASS::~DisplayRenderer() {
	DeleteCache();
}

void THISCLASS::SetDisplay(Display * display) {
	mDisplay = display;
	DeleteCache();
}

void THISCLASS::SetScalingFactor(double scalingfactor) {
	mScalingFactor = scalingfactor;
	DeleteCache();
}

void THISCLASS::SetScalingFactorMax(cv::Size maxsize) {
	// Get the size of the image
	cv::Size srcsize = GetSize();

	// Find out the smallest ratio such that the image fits into [maxwidth, maxheight]
	double rw = (double)(maxsize.width) / (double)(srcsize.width);
	double rh = (double)(maxsize.height) / (double)(srcsize.height);
	double mScalingFactor = 1;
	if (mScalingFactor > rw) mScalingFactor = rw;
	if (mScalingFactor > rh) mScalingFactor = rh;
}

void THISCLASS::SetFlipHorizontal(bool flip) {
	mFlipHorizontally = flip;
	DeleteCache();
}

void THISCLASS::SetFlipVertical(bool flip) {
	mFlipVertically = flip;
	DeleteCache();
}

void THISCLASS::SetCropRectangle(cv::Rect rect) {
	mCropRectangle = rect;
	DeleteCache();
}

void THISCLASS::DeleteCache() {
	mImage.release();
}

cv::Size THISCLASS::GetSize() {
	if (! mDisplay) return cv::Size(300, 200);
	double w = (double) mDisplay->mSize.width * mScalingFactor;
	double h = (double) mDisplay->mSize.height * mScalingFactor;
	cv::Size size((int) floor(w + 0.5), (int) floor(h + 0.5));
	if (size.width <= 0 || size.height <= 0) return cv::Size(300, 200);
	return size;
}

cv::Mat THISCLASS::GetImage() {
	// Return the cached image if possible
	if (! mImage.empty()) return mImage;

	// Create an empty image
	cv::Size size = GetSize();
	mImage = cv::Mat::zeros(size, CV_8UC3);

	// If the display is null, just display an error message
	if (! mDisplay) {
		DrawMessagePanel(wxT("No display selected."));
		return mImage;
	}

	// Prepare the error list
	ErrorList errors;

	// If the display size is too small, show an error message
	if (mDisplay->mSize.width < 1 || mDisplay->mSize.height < 1) {
		errors.Add(wxT("No image."));
	} else if (mDisplay->mSize.width < 10 || mDisplay->mSize.height < 10) {
		errors.Add(wxT("Image too small."));
	}

	// Draw the image
	DrawMainImage(&errors);
	DrawParticles(&errors);
	DrawTrajectories(&errors);
	DrawErrors(&errors);

	return mImage;
}

bool THISCLASS::DrawMainImage(ErrorList * errors) {
	if (! mDrawImage) return false;
	if (! mDisplay) return false;
	if (mDisplay->mMainImage.empty()) return false;

	// Resize and convert the image
	cv::Mat imageRGB = ImageTools::BGRToRGB(mDisplay->mMainImage);
	cv::Mat mainImage;
	cv::resize(imageRGB, mainImage, mImage.size(), 0, 0, cv::INTER_LINEAR);

	// Draw this main image
	if (mUseMask && ! mDisplay->mMaskImage.empty()) {
		// Resize mask image
		cv::Mat resizedMaskImage;
		cv::resize(mDisplay->mMaskImage, resizedMaskImage, mImage.size(), cv::INTER_LINEAR);
		mainImage.copyTo(mImage, resizedMaskImage);
	} else {
		mImage = mainImage;
	}

	// Flip
	mImage = ImageTools::Flip(mImage, mFlipHorizontally, mFlipVertically);
	return true;
}

bool THISCLASS::DrawParticles(ErrorList * errors) {
	if (! mDrawParticles) return false;
	if (! mDisplay) return false;

	// Draw particles
	for (auto & particle : mDisplay->mParticles) {
		int x = (int) floor(particle.mCenter.x * mScalingFactor + 0.5);
		int y = (int) floor(particle.mCenter.y * mScalingFactor + 0.5);
		cv::rectangle(mImage, cv::Point(x - 2, y - 2), cv::Point(x + 2, y + 2), cv::Scalar(192, 0, 0), 1);

		float c = cosf(particle.mOrientation) * 8; //cosf(particle.mOrientation/57.29577951)*20; // TODO: mOrientation contains an angle (rad), and the appropriate conversion to rad should be done when filling this structure
		float s = sinf(particle.mOrientation) * 8; //sinf(particle.mOrientation/57.29577951)*20;
		cv::line(mImage, cv::Point(x, y), cv::Point(x + (int)floorf(c + 0.5), y + (int)floorf(s + 0.5)), cv::Scalar(192, 0, 0), 1);

		wxString label = wxString::Format(wxT("%d [%0.0f, %0.3f]"), particle.mID, particle.mArea, particle.mCompactness);
		cv::putText(mImage, label.ToStdString(), cv::Point(x + 12, y + 10), mFontFace, mFontSize, cv::Scalar(255, 0, 0));
	}

	return true;
}

bool THISCLASS::DrawTrajectories(ErrorList * errors) {
	if (! mDrawTrajectories) return false;
	if (! mDisplay) return false;
	if (! mDisplay->mTrajectories) return false;

	// Draw trajectories
	DataStructureTracks::tTrackMap * tracks = mDisplay->mComponent->GetSwisTrackCore()->mDataStructureTracks.mTracks;
	if (! tracks) return true;
	for (auto & tuple : *tracks) {
		// Color for this track
		cv::Scalar color = cv::Scalar((tuple.first * 50) % 255, (tuple.first * 50) % 255, 255);

		// Draw trajectory polyline
		auto p = tuple.second.mTrajectory.begin();
		int xprev = (int) floor(p->x * mScalingFactor + 0.5);
		int yprev = (int) floor(p->y * mScalingFactor + 0.5);
		while (p != tuple.second.mTrajectory.end()) {
			int x = (int) floor(p->x * mScalingFactor + 0.5);
			int y = (int) floor(p->y * mScalingFactor + 0.5);
			cv::line(mImage, cv::Point(xprev, yprev), cv::Point(x, y), color, 3);
			xprev = x;
			yprev = y;
			p++;
		}

		cv::rectangle(mImage, cv::Point(xprev - 2, yprev - 2), cv::Point(xprev + 2, yprev + 2), color, 1);
		wxString text;
		text << tuple.first;
		cv::putText(mImage, text.ToStdString(), cv::Point(xprev + 3, yprev + 3), mFontFace, mFontSizeBig, color);
	}

	return true;
}

bool THISCLASS::DrawErrors(ErrorList * errors) {
	if (! mDrawErrors) return false;
	if (! mDisplay) return false;

	// The first line starts at the bottom
	int y = mImage.rows - 10;

	// Draw all error messages
	for (auto & error : errors->mList) {
		cv::putText(mImage, error.mMessage.ToStdString(), cv::Point(4, y), mFontFace, mFontSize, cv::Scalar(255, 0, 0));
		y -= 25;
	}

	// Draw all error messages
	for (auto & error : mDisplay->mErrors.mList) {
		cv::putText(mImage, error.mMessage.ToStdString(), cv::Point(4, y), mFontFace, mFontSize, cv::Scalar(255, 0, 0));
		y -= 25;
	}

	return true;
}

bool THISCLASS::DrawMessagePanel(wxString errorString) {
	cv::rectangle(mImage, cv::Point(0, 0), cv::Point(mImage.cols - 1, mImage.rows - 1), cv::Scalar(0, 0, 0), 1);
	if (errorString.Len() == 0) return true;

	int ymin;
	cv::Size textsize = cv::getTextSize(errorString.ToStdString(), mFontFace, mFontSize, 1, &ymin);
	cv::putText(mImage, errorString.ToStdString(), cv::Point((mImage.cols - textsize.width) / 2, (mImage.rows + textsize.height) / 2), mFontFace, mFontSize, cv::Scalar(255, 0, 0));
	return true;
}
