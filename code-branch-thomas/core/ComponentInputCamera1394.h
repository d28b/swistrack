#ifndef HEADER_ComponentInputCamera1394
#define HEADER_ComponentInputCamera1394

#include "cv.h"
#include "Component.h"

#define _1394

#ifdef _1394
#include "highgui.h"  // 1394camera.h requires this to be included before
#include <1394camera.h>

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
	//! 1394 Camera handle (CMU 1394 Camera Driver)
	C1394Camera mCamera;  //!< Camera handle 
	//! The last acquired image.
	IplImage* mLastImage;
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

