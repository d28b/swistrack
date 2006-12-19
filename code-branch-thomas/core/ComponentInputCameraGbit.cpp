#include "ComponentInputCameraGBit.h"
#define THISCLASS ComponentInputCameraGBit

THISCLASS::ComponentInputCameraGBit(SwisTrackCore *stc):
		Component(stc, "CameraGBit") {

	// User-friendly information about this component
	mDisplayName="GBit camera";
	AddDataStructureWrite(mCore->mDataStructureImage);
	AddDataStructureWrite(mCore->mDataStructureInput);
}

bool THISCLASS::Start() {
	//theCamera.SetVideoFrameRate(GetIntValByXPath(cfgRoot, "/Configuration/InputCameraGBit/FrameRate")); 
	return true;
}

bool THISCLASS::Step() {
	return true;
}

bool THISCLASS::StepCleanup() {
	return true;
}

bool THISCLASS::stop() {
	return false;
}
