#ifndef HEADER_ComponentInputCameraGigE
#define HEADER_ComponentInputCameraGigE

#include "Component.h"

#ifdef USE_CAMERA_PYLON_GIGE
#include <pylon/TlFactory.h>
#include <pylon/Result.h>
#include <pylon/gige/BaslerGigECamera.h>
#include <wx/thread.h>
#include <wx/event.h>
#include "Display.h"

//! An input component for GigE cameras.
class ComponentInputCameraGigE: public Component {

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
	ComponentInputCameraGigE(SwisTrackCore *stc);
	//! Destructor.
	~ComponentInputCameraGigE();

	// Overwritten Component methods
	void OnInitializeStatic();
	void OnTerminateStatic();
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {return new ComponentInputCameraGigE(mCore);}

private:
	std::string mCameraFullName;						//!< (configuration) Full name of the camera (as returned by Pylon::DeviceInfo::GetFullName()).
	bool mColor;										//!< (configuration) Whether to acquire color or mono images.
	eTriggerMode mTriggerMode;							//!< (configuration) The trigger source.
	double mTriggerTimerFPS;							//!< (configuration) The FPS of the trigger timer.
	int mInputBufferSize;								//!< (configuration) The number of input buffer images.

	Pylon::CBaslerGigECamera *mCamera;					//!< Camera object.
	Pylon::CBaslerGigEStreamGrabber *mStreamGrabber;	//!< Stream grabber object.

	static const int mInputBufferSizeMax = 32;							//!< The maximum number of input buffer images.
	IplImage *mInputBufferImages[mInputBufferSizeMax];					//!< The input buffer images.
	Pylon::StreamBufferHandle mInputBufferHandles[mInputBufferSizeMax];	//!< The corresponding buffer handles.
	Pylon::GrabResult mCurrentResult;									//!< The current result.

	int mFrameNumber;									//!< The frame number since the component was started.

	//! The thread waiting for new images (in case of external trigger).
	class Thread: public wxThread {
	public:
		ComponentInputCameraGigE *mComponent;		//!< The associated component.

		//! Constructor.
		Thread(ComponentInputCameraGigE *c): wxThread(), mComponent(c) {}
		//! Destructor.
		~Thread() {}

		// wxThread methods
		ExitCode Entry();
	};

	wxCriticalSection mThreadCriticalSection; 	//!< The critical section object to synchronize with the thread.

};

#else // USE_CAMERA_PYLON_GIGE

class ComponentInputCameraGigE: public Component {

public:
	ComponentInputCameraGigE(SwisTrackCore *stc): Component(stc, "InputCameraGigE") {Initialize();}
	~ComponentInputCameraGigE() {}

	// Overwritten Component methods
	void OnStart() {AddError("GigE support was not compiled into this executable.");}
	void OnReloadConfiguration() {AddError("GigE support was not compiled into this executable.");}
	void OnStep() {AddError("GigE support was not compiled into this executable.");}
	void OnStepCleanup() {}
	void OnStop() {AddError("GigE support was not compiled into this executable.");}
	Component *Create() {return new ComponentInputCameraGigE(mCore);}
};

#endif // USE_CAMERA_PYLON_GIGE

#endif

