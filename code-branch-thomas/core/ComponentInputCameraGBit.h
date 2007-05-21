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
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {return new ComponentInputCameraGBit(mCore);}

private:
	int mCameraNumber;									//!< (configuration) Camera number (in case there is more than one camera attached).
	bool mColor;										//!< (configuration) Whether to acquire color or mono images.
	eTriggerMode mTriggerMode;							//!< (configuration) The trigger source.
	double mTriggerTimerFPS;							//!< (configuration) The FPS of the trigger timer.

	Pylon::ITransportLayer *mTransportLayer;			//!< Transport layer object.
	Pylon::CBaslerGigECamera *mCamera;					//!< Camera object.
	Pylon::CBaslerGigEStreamGrabber *mStreamGrabber;	//!< Stream grabber object.
	IplImage *mInputBufferImages[8];					//!< The images for the input queue.
	Pylon::StreamBufferHandle mInputBufferHandles[8];	//!< The corresponding buffer handles.

	int mCurrentImageIndex;								//!< The index of the last acquired image.
	int mFrameNumber;									//!< The frame number since the component was started.

	//! The thread waiting for new images (in case of external trigger).
	class Thread: public wxThread {
	public:
		ComponentInputCameraGBit *mComponent;		//!< The associated component.
		wxCriticalSection mCriticalSection; 		//!< The critical section object (for synchronization).
		IplImage *mImage;							//!< (synchronized) The last image that the thread acquired.

		//! Constructor.
		Thread(ComponentInputCameraGBit *c): wxThread(), mComponent(c) {}
		//! Destructor.
		~Thread() {}

		// wxThread methods
		ExitCode Entry();
	};

	//! The event listener that is waiting for events from the TriggerThread.
	class EventHandler: public wxEvtHandler {
	public:
		ComponentInputCameraGBit *mComponent;			//!< The associated component.

		//! Constructor.
		EventHandler(ComponentInputCameraGBit *c): wxEvtHandler(), mComponent(c) {}
		//! Destructor.
		~EventHandler() {}
	};

	Thread *mThread;				//!< The thread waiting for new images (in case of external trigger).
	EventHandler *mEventHandler;	//!< The event listener that is waiting for events from the TriggerThread.

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

