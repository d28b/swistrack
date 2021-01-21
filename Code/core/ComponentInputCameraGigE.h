#ifndef HEADER_ComponentInputCameraGigE
#define HEADER_ComponentInputCameraGigE

#include "Component.h"

#ifdef USE_CAMERA_PYLON_GIGE

// This is only defined for builds with more recent Pylon drivers, which require the master header
#ifdef USE_PYLON3
#include <pylon/PylonIncludes.h>
#endif

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
		sTrigger_Timer = 0,			//!< Use the internal camera timer to start acquisition.
		sTrigger_InputLine1 = 1,	//!< Use input line 1 to start acquisition.
		sTrigger_InputLine2 = 2,	//!< Use input line 2 to start acquisition.
		sTrigger_InputLine3 = 3,	//!< Use input line 3 to start acquisition.
		sTrigger_InputLine4 = 4,	//!< Use input line 4 to start acquisition.
		sTrigger_Software = 5,		//!< Send a packet over the ethernet cable to start acquisition.
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
	Component *Create() {
		return new ComponentInputCameraGigE(mCore);
	}

private:
	wxString mCameraFullName;							//!< (configuration) Full name of the camera (as returned by Pylon::DeviceInfo::GetFullName()).
	bool mColor;										//!< (configuration) Whether to acquire color or mono images.
	eTriggerMode mTriggerMode;							//!< (configuration) The trigger source.
	double mTriggerTimerFPS;							//!< (configuration) The FPS of the trigger timer.
	int mInputBufferSize;								//!< (configuration) The number of input buffer images.
  int mEthernetPacketSize;							//!< (configuration) The ethernet packet size.

	Pylon::CBaslerGigECamera *mCamera;					//!< Camera object.
#ifdef USE_PYLON3
	Pylon::CPylonGigEStreamGrabber *mStreamGrabber;		//!< Stream grabber object.
#else
	Pylon::CBaslerGigEStreamGrabber *mStreamGrabber;	//!< Stream grabber object.
#endif
	Pylon::IChunkParser *mChunkParser;					//!< Chunk parser object.

	static const int mInputBufferSizeMax = 32;							//!< The maximum number of input buffer images.
	IplImage *mInputBufferImages[mInputBufferSizeMax];					//!< The input buffer images.
	Pylon::StreamBufferHandle mInputBufferHandles[mInputBufferSizeMax];	//!< The corresponding buffer handles.
	Pylon::GrabResult mCurrentResult;									//!< The current result.

	int mFrameNumberStart;								//!< The frame number of the first acquired frame, or -1 if no frame has been acquired yet.
	IplImage *mOutputImage;								//!< The current output image (only used for color acquisition, for mono acquision one of the mInputBufferImages is used).

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

	//! Prepares the output image (recreates the image if necessary).
	inline void PrepareOutputImage(IplImage *inputimage) {
		if (mOutputImage) {
			if ((mOutputImage->width == inputimage->width) && (mOutputImage->height == inputimage->height) && (mOutputImage->depth == inputimage->depth)) {
				return;
			}
			cvReleaseImage(&mOutputImage);
		}
		mOutputImage = cvCreateImage(cvSize(inputimage->width, inputimage->height), inputimage->depth, 3);
	}
};

#else // USE_CAMERA_PYLON_GIGE

class ComponentInputCameraGigE: public Component {

public:
	ComponentInputCameraGigE(SwisTrackCore *stc): Component(stc, wxT("InputCameraGigE")) {
		Initialize();
	}
	~ComponentInputCameraGigE() {}

	// Overwritten Component methods
	void OnStart() {
		AddError(wxT("GigE support was not compiled into this executable."));
	}
	void OnReloadConfiguration() {
		AddError(wxT("GigE support was not compiled into this executable."));
	}
	void OnStep() {
		AddError(wxT("GigE support was not compiled into this executable."));
	}
	void OnStepCleanup() {}
	void OnStop() {
		AddError(wxT("GigE support was not compiled into this executable."));
	}
	Component *Create() {
		return new ComponentInputCameraGigE(mCore);
	}
};

#endif // USE_CAMERA_PYLON_GIGE

#endif
