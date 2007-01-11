#include "ComponentInputCameraGBit.h"
#define THISCLASS ComponentInputCameraGBit

THISCLASS::ComponentInputCameraGBit(SwisTrackCore *stc):
		Component(stc, "CameraGBit") {

	// Data structure relations
	mDisplayName="GBit Camera";
	mCategory=&(mCore->mCategoryInput);
	AddDataStructureWrite(&(mCore->mDataStructureInput));
}

THISCLASS::~ComponentInputCameraGBit() {
}

void THISCLASS::OnStart() {
	//theCamera.SetVideoFrameRate(GetIntValByXPath(cfgRoot, "/Configuration/InputCameraGBit/FrameRate")); 
}

void THISCLASS::OnStep() {
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
}
