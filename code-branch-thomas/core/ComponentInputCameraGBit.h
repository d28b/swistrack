#ifndef HEADER_ComponentInputCameraGBit
#define HEADER_ComponentInputCameraGBit

#include "Component.h"

#ifdef USE_CAMERA_PYLON_GBIT
#include <pylon/TlFactory.h>
#include <pylon/Result.h>
#include <pylon/gige/BaslerGigECamera.h>
#include <wx/thread.h>
#include <wx/event.h>
#include "Display.h"

//! An input component for GBit cameras.
class ComponentInputCameraGBit: public Component {

public:
	enum eTriggerMode {
		sTrigger_Timer=0,		//!< Use the internal camera timer to start acquisition.
		sTrigger_InputLine1=1,	//!< Use input line 1 to start acquisition.
		sTrigger_InputLine2=2,	//!< Use input line 2 to start acquisition.
		sTrigger_InputLine3=3,	//!< Use input line 3 to start acquisition.
		sTrigger_InputLine4=4,	//!< Use input line 4 to start acquisition.
		sTrigger_Software=5,	//!< Send a packet over the ethernet cable to start acquisition.
	};

	//! Constructor.
	ComponentInputCameraGBit(SwisTrackCore *stc);
	//! Destructor.
	~ComponentInputCameraGBit();

	// Overwritten Component methods
	void OnInitializeStatic();
	void OnTerminateStatic();
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {return new ComponentInputCameraGBit(mCore);}

private:
	std::string mCameraFullName;						//!< (configuration) Full name of the camera (as returned by Pylon::DeviceInfo::GetFullName()).
	bool mColor;										//!< (configuration) Whether to acquire color or mono images.
	eTriggerMode mTriggerMode;							//!< (configuration) The trigger source.
	double mTriggerTimerFPS;							//!< (configuration) The FPS of the trigger timer.

	Pylon::CBaslerGigECamera *mCamera;					//!< Camera object.
	Pylon::CBaslerGigEStreamGrabber *mStreamGrabber;	//!< Stream grabber object.

	static const int mInputBufferCount = 8;								//!< The number of input buffers.
	IplImage *mInputBufferImages[mInputBufferCount];					//!< The input buffer images.
	Pylon::StreamBufferHandle mInputBufferHandles[mInputBufferCount];	//!< The corresponding buffer handles.
	Pylon::GrabResult mCurrentResult;									//!< The current result.

	//int mCurrentImageIndex;								//!< The index of the last acquired image.
	int mFrameNumber;									//!< The frame number since the component was started.

	//! The thread waiting for new images (in case of external trigger).
	class Thread: public wxThread {
	public:
		ComponentInputCameraGBit *mComponent;		//!< The associated component.

		//! Constructor.
		Thread(ComponentInputCameraGBit *c): wxThread(), mComponent(c) {}
		//! Destructor.
		~Thread() {}

		// wxThread methods
		ExitCode Entry();
	};

	//Thread *mThread;							//!< The thread waiting for new images (in case of external trigger).
	wxCriticalSection mThreadCriticalSection; 	//!< The critical section object to synchronize with the thread.

};

#else

class ComponentInputCameraGBit: public Component {

public:
	ComponentInputCameraGBit(SwisTrackCore *stc): Component(stc, "InputCameraGBit") {Initialize();}
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

