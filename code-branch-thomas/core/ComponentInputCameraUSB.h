#ifndef HEADER_ComponentInputCameraUSB
#define HEADER_ComponentInputCameraUSB

#include "TrackingImage.h"
#include <vector>
#include <math.h>
#include "cv.h"
#include "highgui.h"
#include "Component.h"

class ComponentInputCameraUSB: public Component {

public:
	ComponentInputCameraUSB(xmlpp::Element* cfgRoot);
	~ComponentInputCameraUSB();

	// Overwritten Component methods
	void SetParameters();
	void Step();
	void UpdateStatus();

private:
	//! The last acquired image.
	IplImage* mLastImage;
};

#endif

