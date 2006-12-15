#include "ComponentInputCameraGBit.h"
#define THISCLASS ComponentInputCameraGBit

THISCLASS::ComponentInputCameraGBit(xmlpp::Element* cfgroot):
		Component(cfgroot) {

}

void THISCLASS::SetParameters() {
	//theCamera.SetVideoFrameRate(GetIntValByXPath(cfgRoot, "/Configuration/InputCameraGBit/FrameRate")); 
}

void THISCLASS::Step() {
}

bool THISCLASS::HasError() {
	return false;
}

std::string THISCLASS::GetError() {
}

