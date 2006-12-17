#include "ComponentInputCameraGBit.h"
#define THISCLASS ComponentInputCameraGBit

THISCLASS::ComponentInputCameraGBit(SwisTrackCore *stc):
		Component(stc, "CameraGBit") {

}

bool THISCLASS::Start() {
	//theCamera.SetVideoFrameRate(GetIntValByXPath(cfgRoot, "/Configuration/InputCameraGBit/FrameRate")); 
}

bool THISCLASS::Step() {
}

bool THISCLASS::stop() {
	return false;
}
