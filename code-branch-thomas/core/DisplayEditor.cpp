#include "DisplayEditor.h"
#define THISCLASS DisplayEditor

THISCLASS::DisplayEditor(Display *display): mDisplay(0) {
	if (! display) {return;}

	// If the display isn't active (i.e. no subscribers) we don't need to construct it.
	if (! display->IsActive()) {return;}

	// If everything is all right, we agree to work.
	mDisplay=display;

	// Set default values
	mDisplay->mSize=cvSize(0, 0);
	SetImage(0);
	SetMask(0);
	SetParticles(0);
}

THISCLASS::~DisplayEditor() {
	// If the user didn't set a size
	if ((mDisplay->mSize->width==0) && (mDisplay->mSize->width==0)) {
		if (! SetSizeAuto()) {
			mDisplay->mSize=cvSize(100, 100);
			mDisplay->mErrors.add("Unable to determine display size.");
		}
	}
}

void THISCLASS::SetSize(CvSize size) {
	mSize=size;
}

bool THISCLASS::SetSizeAuto() {
	// Take the size of the main image if available
	if (mDisplay->mImage) {
		mDisplay->mSize=cvSize(mDisplay->mImage->width, mDisplay->mImage->height);
		return true;
	}

	// Take the size of the mask if available
	if (mDisplay->mMask) {
		mDisplay->mSize=cvSize(mDisplay->mImage->width, mDisplay->mImage->height);
		return true;
	}

	// Take the size of the input frame
	SwisTrackCore *core=mDisplay->mComponent->GetSwisTrackCore();
	if (core->mDataStructureInput.mImage) {
		mDisplay->mSize=cvSize(core->mDataStructureInput.mImage->width, core->mDataStructureInput.mImage->height);
		return true;
	}
	
	return false;
}

void THISCLASS::SetImage(IplImage *img) {
	if (! mDisplay) {return;}

	// Delete the old image
	cvReleaseImage(&(mDisplay->mImage));

	// Set the empty image if necessary
	if (! img) {
		mDisplay->mImage=0;
		return;
	}

	// Copy and convert the image
	if (img->nChannels==3) {
		mDisplay->mImage=cvCreateImage(cvSize(img->width, img->height), img->depth, 3);
		cvCopy(img, mDisplay->mImage);
	} else if (img->nChannels==1) {
		mDisplay->mImage=cvCreateImage(cvSize(img->width, img->height), img->depth, 3);
		cvCvtColor(img, mDisplay->mImage, CV_GRAY2BGR);
	} else {
		mDisplay->mErrors.add("Cannot display image: wrong format.");
		mDisplay->mImage=0;
	}
}

void THISCLASS::SetMask(IplImage *img) {
	// Delete the old image
	cvReleaseImage(&(mDisplay->mMaskImage));

	// Set an empty mask if necessary
	if (! img) {
		mDisplay->mMaskImage=0;
		return;
	}

	// Copy and convert the image
	if (img->nChannels==3) {
		mDisplay->mMaskImage=cvCreateImage(cvSize(img->width, img->height), img->depth, 3);
		cvCvtColor(img, mDisplay->mMaskImage, CV_BGR2GRAY);
	} else if (img->nChannels==1) {
		mDisplay->mMaskImage=cvCreateImage(cvSize(img->width, img->height), img->depth, 3);
		cvCopy(img, mDisplay->mMaskImage);
	} else {
		mDisplay->mErrors.add("Cannot display mask: wrong format.");
		mDisplay->mMaskImage=0;
	}
}

void THISCLASS::SetParticles(DataStructureParticles::ParticleVector *pv) {
	// Delete the old particles
	delete mDisplay->mParticles;
}

