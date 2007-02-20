#include "ComponentInputCameraGBit.h"
#define THISCLASS ComponentInputCameraGBit

THISCLASS::ComponentInputCameraGBit(SwisTrackCore *stc):
		Component(stc, "CameraGBit"),
		mDisplayImageOutput("Output", "GBit Camera: Input Frame") {

	// Data structure relations
	mDisplayName="GBit Camera";
	mCategory=&(mCore->mCategoryInput);
	AddDataStructureWrite(&(mCore->mDataStructureInput));
	AddDisplayImage(&mDisplayImageOutput);
}

THISCLASS::~ComponentInputCameraGBit() {
}

void THISCLASS::OnStart() {
	//theCamera.SetVideoFrameRate(GetIntValByXPath(cfgRoot, "/Configuration/InputCameraGBit/FrameRate")); 
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}
