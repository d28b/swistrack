#include "DisplayEditor.h"
#define THISCLASS DisplayEditor

THISCLASS::DisplayEditor(Display *display): mDisplay(0) {
	if (! display) {return;}

	// If the display isn't active (i.e. no subscribers) we don't need to construct it.
	if (! display->IsActive()) {return;}

	// If everything is all right, we agree to work.
	mDisplay=display;
}

THISCLASS::~DisplayEditor() {
}

void THISCLASS::SetImage(IplImage *img) {
	if (! mDisplay) {return;}

	// Delete the old image
	cvReleaseImage(&mImage);

	// Set the empty image if necessary
	if (! img) {
		mImage=0;
		return;
	}

	// Copy and convert the image
	if (img->nChannels==3) {
		mImage=cvCreateImage(cvSize(img->width, img->height), img->depth, 3);
		cvCopy(img, mImage);
	} else if (img->nChannels==1) {
		mImage=cvCreateImage(cvSize(img->width, img->height), img->depth, 3);
		cvCvtColor(mImage, mImage, CV_GRAY2BGR);
	} else {
		mImageError="Cannot display image: wrong format.";
		mImage=0;
	}
}

void THISCLASS::SetMask(IplImage *img) {
}

void THISCLASS::SetParticles(DataStructureParticles::ParticleVector *pv) {
}
