#ifndef HEADER_ComponentInputCamera1394
#define HEADER_ComponentInputCamera1394

#include "cv.h"
#include "Component.h"

#define _1394

#ifdef _1394
#include <1394camera.h>

class ComponentInputCamera1394: public Component {

public:
	ComponentInputCamera1394(SwisTrackCore *stc);
	~ComponentInputCamera1394();

	// Overwritten Component methods
	bool Start();
	bool Step();
	bool Stop();

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
	bool Start() {AddError("1394 support was not compiled into this executable."); return false;}
	bool Step() {AddError("1394 support was not compiled into this executable."); return false;}
	bool Stop() {AddError("1394 support was not compiled into this executable."); return false;}
};

#endif

#endif

