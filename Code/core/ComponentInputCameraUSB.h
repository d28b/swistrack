#ifndef HEADER_ComponentInputCameraUSB
#define HEADER_ComponentInputCameraUSB

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include "Component.h"

class ComponentInputCameraUSB: public Component {

public:
	//! Constructor.
	ComponentInputCameraUSB(SwisTrackCore * stc);
	//! Destructor.
	~ComponentInputCameraUSB();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentInputCameraUSB(mCore);
	}

private:
	cv::VideoCapture mCapture;			//!< OpenCV capture object.
	int mFrameNumber;					//!< The frame number since the component was started.

	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.
};

#endif

