#include "DisplayEditor.h"
#define THISCLASS DisplayEditor

THISCLASS::DisplayEditor(Display *display): mDisplay(0) {
	if (! display) {
		return;
	}

	// If the display isn't active (i.e. no subscribers) we don't do anything
	if (! display->mActive) {
		return;
	}

	// If everything is all right, we agree to work
	mDisplay = display;

	// Set default values
	mDisplay->mChanged = true;
	mDisplay->mSize = cvSize(0, 0);
	SetMainImage(0);
	SetMaskImage(0);
	SetParticles(0);
	SetTrajectories(false);
}

THISCLASS::~DisplayEditor() {
	if (! mDisplay) {
		return;
	}

	// If the user didn't set a size, try to find it out automatically
	if ((mDisplay->mSize.width == 0) && (mDisplay->mSize.width == 0)) {
		if (! SetSizeAuto()) {
			mDisplay->mSize = cvSize(300, 200);
			AddError(wxT("Unable to determine display size."));
		}
	}

	// Set frame number and time
	SwisTrackCore *core = mDisplay->mComponent->GetSwisTrackCore();
	mDisplay->mStepCounter = core->GetStepCounter();
	mDisplay->mFrameNumber = core->mDataStructureInput.mFrameNumber;
	mDisplay->mFramesCount = core->mDataStructureInput.mFramesCount;
	mDisplay->mTime = wxDateTime::UNow();
}

void THISCLASS::SetMainImage(IplImage *img) {
	if (! mDisplay) {
		return;
	}

	// Delete the old image
	cvReleaseImage(&(mDisplay->mMainImage));

	// Set the empty image if necessary
	if (! img) {
		mDisplay->mMainImage = 0;
		return;
	}

	// Take a copy the image
	mDisplay->mMainImage = cvCreateImage(cvSize(img->width, img->height), img->depth, img->nChannels);
	cvCopy(img, mDisplay->mMainImage);
	strcpy((mDisplay->mMainImage)->channelSeq, img->channelSeq);
}

void THISCLASS::SetMaskImage(IplImage *img) {
	if (! mDisplay) {
		return;
	}

	// Delete the old image
	cvReleaseImage(&(mDisplay->mMaskImage));

	// Set an empty mask if necessary
	if (! img) {
		mDisplay->mMaskImage = 0;
		return;
	}

	// Copy and convert the image
	if (img->nChannels == 1) {
		mDisplay->mMaskImage = cvCreateImage(cvSize(img->width, img->height), img->depth, 1);
		cvCopy(img, mDisplay->mMaskImage);
	} else {
		AddError(wxT("Cannot display mask: wrong format."));
		mDisplay->mMaskImage = 0;
	}
}

void THISCLASS::SetParticles(DataStructureParticles::tParticleVector *pv) {
	if (! mDisplay) {
		return;
	}

	// Clear the particle list
	mDisplay->mParticles.clear();

	// Return unless there are particles
	if (! pv) {
		return;
	}

	// Add a copy of all particles
	DataStructureParticles::tParticleVector::iterator it = pv->begin();
	while (it != pv->end()) {
		mDisplay->mParticles.push_back((*it));
		it++;
	}
}

void THISCLASS::SetTrajectories(bool trajectories) {
	if (! mDisplay) {
		return;
	}

	// Set the trajectory draw flag
	mDisplay->mTrajectories = trajectories;
}

void THISCLASS::SetSize(CvSize size) {
	if (! mDisplay) {
		return;
	}
	mDisplay->mSize = size;
}

bool THISCLASS::SetSizeAuto() {
	if (! mDisplay) {
		return true;
	}

	// Take the size of the main image if available
	if (mDisplay->mMainImage) {
		mDisplay->mSize = cvSize(mDisplay->mMainImage->width, mDisplay->mMainImage->height);
		return true;
	}

	// Take the size of the mask if available
	if (mDisplay->mMaskImage) {
		mDisplay->mSize = cvSize(mDisplay->mMaskImage->width, mDisplay->mMaskImage->height);
		return true;
	}

	// Take the size of the input frame
	SwisTrackCore *core = mDisplay->mComponent->GetSwisTrackCore();
	if (core->mDataStructureInput.mImage) {
		mDisplay->mSize = cvSize(core->mDataStructureInput.mImage->width, core->mDataStructureInput.mImage->height);
		return true;
	}

	return false;
}

void THISCLASS::AddError(const wxString &message) {
	if (! mDisplay) {
		return;
	}
	mDisplay->mErrors.Add(message);
}
