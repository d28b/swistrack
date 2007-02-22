#ifndef HEADER_ComponentInputCameraGBit
#define HEADER_ComponentInputCameraGBit

#define USE_CAMERA_PYLON_GBIT

#include "Component.h"

#ifdef USE_CAMERA_PYLON_GBIT
#include <pylon/TlFactory.h>
#include <pylon/Result.h>
#include <pylon/gige/BaslerGigECamera.h>
#include "DisplayImageStandard.h"

//! An input component for GBit cameras.
class ComponentInputCameraGBit: public Component {

public:
	//! Constructor.
	ComponentInputCameraGBit(SwisTrackCore *stc);
	//! Destructor.
	~ComponentInputCameraGBit();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {return new ComponentInputCameraGBit(mCore);}

private:
	Pylon::ITransportLayer *mTransportLayer;			//!< Transport layer object.
	Pylon::CBaslerGigECamera *mCamera;					//!< Camera object.
	Pylon::CBaslerGigEStreamGrabber *mStreamGrabber;	//!< Stream grabber object.
	IplImage *mInputBufferImages[8];					//!< The images for the input queue.
	Pylon::StreamBufferHandle mInputBufferHandles[8];	//!< The corresponding buffer handles.

	int mCurrentImageIndex;								//!< The index of the last acquired image.
	int mFrameNumber;									//!< The frame number since the component was started.

	DisplayImageStandard mDisplayImageOutput;		//!< The DisplayImage showing the last acquired image.

};

#else

class ComponentInputCameraGBit: public Component {

public:
	ComponentInputCameraGBit(SwisTrackCore *stc): Component(stc, "CameraGBit") {}
	~ComponentInputCameraGBit() {}

	// Overwritten Component methods
	void OnStart() {AddError("GBit support was not compiled into this executable.");}
	void OnReloadConfiguration() {AddError("GBit support was not compiled into this executable.");}
	void OnStep() {AddError("GBit support was not compiled into this executable.");}
	void OnStepCleanup() {}
	void OnStop() {AddError("GBit support was not compiled into this executable.");}
	Component *Create() {return new ComponentInputCameraGBit(mCore);}
};

#endif

#endif

