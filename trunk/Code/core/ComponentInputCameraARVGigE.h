#ifndef HEADER_ComponentInputCameraARVGigE
#define HEADER_ComponentInputCameraARVGigE

#include "Component.h"

#ifdef USE_CAMERA_ARV_GIGE
#include <arv.h>
#include <wx/thread.h>
#include <wx/event.h>
#include "Display.h"
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>


//! An input component for GigE cameras.
class ComponentInputCameraARVGigE: public Component {

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
	ComponentInputCameraARVGigE(SwisTrackCore *stc);
	//! Destructor.
	~ComponentInputCameraARVGigE();

	// Overwritten Component methods
	void OnInitializeStatic();
	void OnTerminateStatic();
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentInputCameraARVGigE(mCore);
	}

	IplImage *mOutputImage;								//!< The current output image (only used for color acquisition, for mono acquision one of the mInputBufferImages is used).		
	unsigned int buffer_count;
	bool imageProcessed, imageReady;
private:

	void set_camera_options();

	wxString mCameraFullName;							//!< (configuration) Full name of the camera (as returned by Pylon::DeviceInfo::GetFullName()).
	bool mColor;										//!< (configuration) Whether to acquire color or mono images.
	int mInputBufferSize;								//!< (configuration) The number of input buffer images.


	static const int mInputBufferSizeMax = 32;							//!< The maximum number of input buffer images.
	IplImage *mInputBufferImages[mInputBufferSizeMax];					//!< The input buffer images.

	int mFrameNumberStart;								//!< The frame number of the first acquired frame, or -1 if no frame has been acquired yet.
	
	Display mDisplayOutput;	
	ArvCamera *mCamera;
	ArvStream *mStream;
	ArvBuffer *mBuffer;


	char *arv_option_camera_name;
	char *arv_option_debug_domains;
	gboolean arv_option_snaphot;
	char *arv_option_trigger;
	double arv_option_software_trigger;
	double arv_option_frequency;
	int arv_option_offset_x;
	int arv_option_offset_y;
	int arv_option_width;
	int arv_option_height;
	int arv_option_horizontal_binning;
	int arv_option_vertical_binning;
	double arv_option_exposure_time_us;
	int arv_option_gain;
	gboolean arv_option_auto_socket_buffer;
	gboolean arv_option_no_packet_resend;
	unsigned int arv_option_packet_timeout;
	unsigned int arv_option_frame_retention;
	
	GMainLoop *main_loop;
	
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

class ComponentInputCameraARVGigE: public Component {

public:
	ComponentInputCameraARVGigE(SwisTrackCore *stc): Component(stc, wxT("InputCameraARVGigE")) {
		Initialize();
	}
	~ComponentInputCameraARVGigE() {}

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
		return new ComponentInputCameraARVGigE(mCore);
	}
};

#endif // USE_CAMERA_PYLON_GIGE

#endif
