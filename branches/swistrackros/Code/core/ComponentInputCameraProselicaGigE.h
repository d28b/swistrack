#ifndef HEADER_ComponentInputCameraProselicaGigE
#define HEADER_ComponentInputCameraProselicaGigE

#include "Component.h"
#ifdef USE_CAMERA_PROSILICA_GIGE
#include <wx/thread.h>
#include <wx/event.h>
#include "Display.h"
//Keep this exact order of next 3 includes below
#define _LINUX
#define _x64
#include <PvApi.h>

//! An input component for GigE cameras.
class ComponentInputCameraProselicaGigE: public Component {

public:
    enum eExposureMode {
        sExposure_Manual = 0,
        sExposure_Auto = 1,
        sExposure_AutoOnce = 2,
        sExposure_External = 3,
    };
	//! Constructor.
	ComponentInputCameraProselicaGigE(SwisTrackCore *stc);
	//! Destructor.
	~ComponentInputCameraProselicaGigE();

	// Overwritten Component methods
	void OnInitializeStatic();
	void OnTerminateStatic();
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentInputCameraProselicaGigE(mCore);
	}

private:
	wxString mCameraFullName;		//!< (configuration) Full name of the camera (can bereturned by PvCameraInfo).
	bool mColor;				    //!< (configuration) Whether to acquire color or mono images.	
	eExposureMode mExposureMode;    //!< (configuration) Camera exposure mode options
	tPvUint32 mExposureValue;       //!< (configuration) Set exposure value in manual mode
	tPvUint32 mAnalogGain;		    //!< (configuration) Set the camera analog gain	

	// camera structures
    unsigned long   mCameraUID; 		//!< unique camera ID
    tPvHandle       mCameraHandle; 		//!< handle for a camera
    tPvFrame        mCameraFrame;		//!< frame description
    tPvCameraInfo	mCameraInfo;  		//!< Camera description
    unsigned long	mFrameSize;  		//!< Frame Size
	tPvUint32   mWidth; 				//!< Image width
	tPvUint32   mHeight; 				//!< Image height
    int mFrameNumber;					//!< The frame number since the component was started.
    tPvErr		mErrcode; 				//!< Prosilica tPvErr type error code
	IplImage   *mBayerImage; 			//!< captured raw (Bayer8) image
	IplImage   *mColorImage; 			//!< converted image using CV_BayerGB2BGR
	IplImage   *mGrayImage; 			//!< converted image using CV_BGRA2GRAY
	

  	//! The thread waiting for new images (in case of external trigger).
  	class Thread: public wxThread {
	  public:
		ComponentInputCameraProselicaGigE *mComponent;	//!< The associated component.

		//! Constructor.
		Thread(ComponentInputCameraProselicaGigE *c): wxThread(), mComponent(c) {}
		//! Destructor.
		~Thread() {}

		// wxThread methods
		ExitCode Entry();
	};

	wxCriticalSection mThreadCriticalSection; 	//!< The critical section object to synchronize with the thread.	
};

#else // No Proselica GigE Support Available

class ComponentInputCameraProselicaGigE: public Component {

public:
	ComponentInputCameraProselicaGigE(SwisTrackCore *stc): Component(stc, wxT("InputCameraProselicaGigE")) {
		Initialize();
	}
	~ComponentInputCameraProselicaGigE() {}

	// Overwritten Component methods
	void OnStart() {
		AddError(wxT("Proselica GigE support was not compiled into this executable."));
	}
	void OnReloadConfiguration() {
		AddError(wxT("Proselica GigE support was not compiled into this executable."));
	}
	void OnStep() {
		AddError(wxT("Proselica GigE support was not compiled into this executable."));
	}
	void OnStepCleanup() {}
	void OnStop() {
		AddError(wxT("Proselica GigE support was not compiled into this executable."));
	}
	Component *Create() {
		return new ComponentInputCameraProselicaGigE(mCore);
	}
};

#endif // USE_CAMERA_PROSILICA_GIGE

#endif


 	  	 
