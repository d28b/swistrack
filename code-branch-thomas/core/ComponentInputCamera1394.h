#ifndef HEADER_ComponentInputCamera1394
#define HEADER_ComponentInputCamera1394

#include "Component.h"

#define _1394

#ifdef _1394
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
	C1394Camera mCamera;		//! 1394 Camera handle (CMU 1394 Camera Driver)
	IplImage* mCurrentImage;	//! The last acquired image.
	int mFrameNumber;			//! The frame number since the component was started.

	DisplayImageStandard mDisplayImageOutput;	//!< The DisplayImage showing the last acquired image.
};

#else

class ComponentInputCamera1394: public Component {

public:
	ComponentInputCamera1394(SwisTrackCore *stc): Component(stc, "Camera1394") {}
	~ComponentInputCamera1394();

	// Overwritten Component methods
	bool OnStart() {AddError("1394 support was not compiled into this executable."); return false;}
	bool OnStep() {AddError("1394 support was not compiled into this executable."); return false;}
	bool OnStop() {AddError("1394 support was not compiled into this executable."); return false;}
};

#endif

#endif

