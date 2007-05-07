#ifndef HEADER_ComponentInputCamera1394
#define HEADER_ComponentInputCamera1394

#include "Component.h"


#ifdef USE_CAMERA_CMU_1394
#include <cv.h>
#include <highgui.h>  // 1394camera.h requires this to be included before
#include <1394camera.h>
#include "DisplayImageStandard.h"

//! An input component that interfaces a Firewire 1394 camera.
class ComponentInputCamera1394: public Component {

public:
	//! Constructor.
	ComponentInputCamera1394(SwisTrackCore *stc);
	//! Destructor.
	~ComponentInputCamera1394();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {return new ComponentInputCamera1394(mCore);}

private:
	C1394Camera mCamera;				//!< Camera handle.
	IplImage* mOutputImage;				//!< The last acquired image.
	int mFrameNumber;					//!< The frame number since the component was started.
	unsigned char *grabbedImageBuffer;	//!< Memory buffer were the grabbed image is put

	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.
};

#else

class ComponentInputCamera1394: public Component {

public:
	ComponentInputCamera1394(SwisTrackCore *stc): Component(stc, "InputCamera1394") {Initialize();}
	~ComponentInputCamera1394() {}

	// Overwritten Component methods
	void OnStart() {AddError("1394 support was not compiled into this executable.");}
	void OnReloadConfiguration() {AddError("1394 support was not compiled into this executable.");}
	void OnStep() {AddError("1394 support was not compiled into this executable.");}
	void OnStepCleanup() {}
	void OnStop() {AddError("1394 support was not compiled into this executable.");}
	Component *Create() {return new ComponentInputCamera1394(mCore);}
};

#endif

#endif

