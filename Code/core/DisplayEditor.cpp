#include "DisplayEditor.h"
#define THISCLASS DisplayEditor

THISCLASS::DisplayEditor(Display * display): mDisplay(0) {
	if (! display) return;

	// If the display isn't active (i.e. no subscribers) we don't do anything
	if (! display->mActive) return;

	// If everything is all right, we agree to work
	mDisplay = display;

	// Set default values
	mDisplay->mChanged = true;
	mDisplay->mSize = cv::Size(0, 0);
	mDisplay->mMainImage = cv::Mat();
	mDisplay->mMaskImage = cv::Mat();
	SetParticles(0);
	SetTrajectories(false);
}

THISCLASS::~DisplayEditor() {
	if (! mDisplay) return;

	// If the user didn't set a size, try to find it out automatically
	if ((mDisplay->mSize.width == 0) && (mDisplay->mSize.width == 0)) {
		if (! SetSizeAuto()) {
			mDisplay->mSize = cv::Size(300, 200);
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

void THISCLASS::SetMainImage(cv::Mat img) {
	if (! mDisplay) return;
	mDisplay->mMainImage = img;
}

void THISCLASS::SetMaskImage(cv::Mat img) {
	if (! mDisplay) return;

	// Copy and convert the image
	if (img.channels() == 1) {
		mDisplay->mMaskImage = img;
	} else {
		AddError(wxT("Cannot display mask: wrong format."));
		mDisplay->mMaskImage = 0;
	}
}

void THISCLASS::SetParticles(DataStructureParticles::tParticleVector * pv) {
	if (! mDisplay) return;

	// Clear the particle list
	mDisplay->mParticles.clear();

	// Return unless there are particles
	if (! pv) return;

	// Add a copy of all particles
	for (auto particle : *pv)
		mDisplay->mParticles.push_back(particle);
}

void THISCLASS::SetTrajectories(bool trajectories) {
	if (! mDisplay) return;

	// Set the trajectory draw flag
	mDisplay->mTrajectories = trajectories;
}

void THISCLASS::SetSize(cv::Size size) {
	if (! mDisplay) return;
	mDisplay->mSize = size;
}

bool THISCLASS::SetSizeAuto() {
	if (! mDisplay) return true;

	// Take the size of the main image if available
	if (! mDisplay->mMainImage.empty()) {
		mDisplay->mSize = mDisplay->mMainImage.size();
		return true;
	}

	// Take the size of the mask if available
	if (! mDisplay->mMaskImage.empty()) {
		mDisplay->mSize = mDisplay->mMaskImage.size();
		return true;
	}

	// Take the size of the input frame
	SwisTrackCore * core = mDisplay->mComponent->GetSwisTrackCore();
	if (! core->mDataStructureInput.mImage.empty()) {
		mDisplay->mSize = core->mDataStructureInput.mImage.size();
		return true;
	}

	return false;
}

void THISCLASS::AddError(const wxString &message) {
	if (! mDisplay) return;
	mDisplay->mErrors.Add(message);
}
