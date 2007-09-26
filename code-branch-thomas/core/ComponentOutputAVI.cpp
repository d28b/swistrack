#include "ComponentOutputAVI.h"
#define THISCLASS ComponentOutputAVI

#ifdef __WXMSW__

THISCLASS::ComponentOutputAVI(SwisTrackCore *stc):
		Component(stc, "OutputAVI"),
		mFileName(""), mPlaybackSpeedFPS(30), mFrameRate(1), mMaxImageSize(cvSize(0, 0)),
		mAVIWriter(0), mFrameCounter(0), mErrorAVIFile(false), mWrittenFramesCount(0),
		mCurrentDisplayImage(0) {

	// Data structure relations
	mDisplayName="Write AVI movie";
	mCategory=&(mCore->mCategoryOutput);
	AddDataStructureRead(&(mCore->mDataStructureInput));
}

THISCLASS::~ComponentOutputAVI() {
}

void THISCLASS::OnStart() {
	mFileName=GetConfigurationString("File", "");
	mPlaybackSpeedFPS=GetConfigurationDouble("PlaybackSpeedFPS", 30);
	mMaxImageSize.width=GetConfigurationDouble("MaxImageSize.width", 0);
	mMaxImageSize.height=GetConfigurationDouble("MaxImageSize.height", 0);

	mFrameCounter=0;
	mErrorAVIFile=false;
	mWrittenFramesCount=0;
	OnReloadConfiguration();
}

void THISCLASS::OnStep() {
	// The video file is written upon invocation of OnDisplayImageChanged().

	// Do nothing unless we are in productive mode
	if (! mCore->IsStartedInProductiveMode()) {return;}

	// Report errors
	if (mErrorAVIFile) {
		ostringstream oss;
		oss << "Could not open file \'" << mFileName << "\' for writing!";
		AddError(oss.str());
	} else {
		ostringstream oss;
		oss << mFramesWrittenCount << " frames written.";
		AddInfo(oss.str());
	}
}

void THISCLASS::OnReloadConfiguration() {
	mFrameRate=GetConfigurationDouble("FrameRate", 1);

	// Unsubscribe
	if (mCurrentDisplayImage) {
		mCurrentDisplayImage->Unsubscribe(this);
	}

	// Subscribe
	std::string componentname=GetConfigurationString("Component", "");
	std::string displayimagename=GetConfigurationString("DisplayImage", "");
	Component *c=mCore->GetComponentByName(componentname);
	if (! c) {return;}
	DisplayImage *di=c->GetDisplayImageByName(displayimagename);
	if (! di) {return;}
	di->Subscribe(this);
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	cvReleaseAVIWriter(&mAVIWriter);
	mAVIWriter=0;
}

void THISCLASS::OnDisplayImageSubscribe(DisplayImage *di) {
	mCurrentDisplayImage=di;
}

void THISCLASS::OnDisplayImageChanged(DisplayImage *di) {
	if (mCurrentDisplayImage!=di) {return;}
	if (mErrorAVIFile) {return;}

	// Do nothing unless we are in productive mode
	if (! mCore->IsStartedInProductiveMode()) {return;}

	// Show only every mFrameRate image
	if (mFrameRate==0) {return;}
	mFrameCounter--;
	if (mFrameCounter>0) {return;}
	mFrameCounter=mFrameRate;

	// Get the new image
	IplImage *img=di->CreateImage(mMaxImageSize.width, mMaxImageSize.height);

	// Create a video file if necessary
	if (! mAVIWriter) {
		mAVIWriter=cvCreateAVIWriter(filename, -1, playfps, cvSize(img->width, img->height));
		if (! mAVIWriter) {
			mErrorAVIFile=true;
			return;
		}
	}

	// Write the frame
	cvWriteToAVI(mAVIWriter, img);
	mFramesWrittenCount++;
}

void THISCLASS::OnDisplayImageUnsubscribe(DisplayImage *di) {
	if (mCurrentDisplayImage!=di) {return;}

	mCurrentDisplayImage=0;
}

#endif // __WXMSW__
