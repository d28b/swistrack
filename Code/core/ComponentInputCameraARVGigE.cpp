#include "ComponentInputCameraARVGigE.h"
#define THISCLASS ComponentInputCameraARVGigE

#ifdef USE_CAMERA_ARV_GIGE
#include "DisplayEditor.h"
#include "ImageConversion.h"
#include <time.h>

static void new_buffer_cb (ArvStream *stream, ComponentInputCameraARVGigE *data)
{
	ArvBuffer *buffer;
	buffer = arv_stream_try_pop_buffer (stream);
	if (buffer != NULL) {
		//printf("frame received\n");
		if (buffer->status == ARV_BUFFER_STATUS_SUCCESS)
			data->buffer_count++;
			//if (data->imageProcessed) {
				data->mOutputImage->imageData=(char*)buffer->data;
				//data->mTrigger->SetReady();
				data->imageProcessed=false;
				data->imageReady=true;
				//printf("!");
			//} //else printf(".");
			
		arv_stream_push_buffer (stream, buffer);
	}
}

static gboolean
emit_software_trigger (void *abstract_data)
{
	ArvCamera *camera = (ArvCamera*)abstract_data;

	arv_camera_software_trigger (camera);

	return TRUE;
}

static void
control_lost_cb (ArvGvDevice *gv_device)
{
	printf ("Control lost\n");

	
}

THISCLASS::ComponentInputCameraARVGigE(SwisTrackCore *stc):
		Component(stc, wxT("InputCameraARVGigE")),
		 mDisplayOutput(wxT("capture"), wxT("GigE image capture")){

	// Data structure relations
	mCategory = &(mCore->mCategoryInput);
	AddDataStructureWrite(&(mCore->mDataStructureInput));
	AddDisplay(&mDisplayOutput);

	mCamera=NULL;
	// Trigger
	//mTrigger = new ComponentTrigger(this);
	arv_option_camera_name=NULL;
	// Read the XML configuration file
	Initialize();
	
	
}

THISCLASS::~ComponentInputCameraARVGigE() {
	// Free the output image
	if (mOutputImage) {
		cvReleaseImage(&mOutputImage);
	}

	delete mTrigger;
}



void THISCLASS::OnInitializeStatic() {

	buffer_count = 0;

	arv_g_thread_init (NULL);
	arv_g_type_init ();



}

void THISCLASS::OnTerminateStatic() {
	g_main_loop_unref (main_loop);


	arv_camera_stop_acquisition (mCamera);

	g_object_unref (mStream);
	
}
void THISCLASS::set_camera_options(){
	gint payload;
	gint x, y, width, height;
	gint dx, dy;
	double exposure;
	int gain;
	arv_option_offset_x= (GetConfigurationInt(wxT("AOIOffset.x"), 0) / 24) * 24;
	arv_option_offset_y= GetConfigurationInt(wxT("AOIOffset.y"), 0);

	arv_option_width = GetConfigurationInt(wxT("AOISize.x"),512);
	arv_option_height = GetConfigurationInt(wxT("AOISize.y"), 512);
	arv_option_exposure_time_us=GetConfigurationInt(wxT("ExposureTime"), 100);
	arv_option_gain=GetConfigurationInt(wxT("AnalogGain"), 500);
	arv_option_frequency=GetConfigurationInt(wxT("TriggerTimerFPS"), 10);
	printf("setting to (%i, %i)", arv_option_width, arv_option_height);

	arv_camera_set_region (mCamera, arv_option_offset_x, arv_option_offset_y, arv_option_width, arv_option_height);
	arv_camera_set_binning (mCamera, arv_option_horizontal_binning, arv_option_vertical_binning);
	arv_camera_set_exposure_time (mCamera, arv_option_exposure_time_us);
	arv_camera_set_gain (mCamera, arv_option_gain);

	arv_camera_get_region (mCamera, &x, &y, &width, &height);
	arv_camera_get_binning (mCamera, &dx, &dy);
	payload = arv_camera_get_payload (mCamera);
	arv_camera_get_region (mCamera, &x, &y, &width, &height);
	
	exposure = arv_camera_get_exposure_time (mCamera);
	gain = arv_camera_get_gain (mCamera);

	printf ("vendor name         = %s\n", arv_camera_get_vendor_name (mCamera));
	printf ("model name          = %s\n", arv_camera_get_model_name (mCamera));
	printf ("device id           = %s\n", arv_camera_get_device_id (mCamera));
	printf ("image width         = %d\n", width);
	printf ("image height        = %d\n", height);
	printf ("horizontal binning  = %d\n", dx);
	printf ("vertical binning    = %d\n", dy);
	printf ("payload             = %d bytes\n", payload);
	printf ("exposure            = %g µs\n", exposure);
	printf ("gain                = %d dB\n", gain);
}

void THISCLASS::OnStart() {
	if (mCamera==NULL) {
		printf("connecting to camera\n");
		mCamera = arv_camera_new (arv_option_camera_name);
	}
	if (mCamera != NULL) {
		printf("starting camera\n");
		void (*old_sigint_handler)(int);
		guint64 n_completed_buffers;
		guint64 n_failures;
		guint64 n_underruns;
		gint payload;
		gint x, y, width, height;
		guint software_trigger_source = 0;


		set_camera_options();
		payload = arv_camera_get_payload (mCamera);
		arv_camera_get_region (mCamera, &x, &y, &width, &height);

		mStream = arv_camera_create_stream (mCamera, NULL, NULL);
		if (mStream != NULL) {
			if (ARV_IS_GV_STREAM (mStream)) {
				if (arv_option_auto_socket_buffer)
					g_object_set (mStream,
						      "socket-buffer", ARV_GV_STREAM_SOCKET_BUFFER_AUTO,
						      "socket-buffer-size", 0,
						      NULL);
				if (arv_option_no_packet_resend)
					g_object_set (mStream,
						      "packet-resend", ARV_GV_STREAM_PACKET_RESEND_NEVER,
						      NULL);
				g_object_set (mStream,
					      "packet-timeout", (unsigned) arv_option_packet_timeout * 1000,
					      "frame-retention", (unsigned) arv_option_frame_retention * 1000,
					      NULL);
			}

			for (int i = 0; i < 50; i++)
				arv_stream_push_buffer (mStream, arv_buffer_new (payload, NULL));

			arv_camera_set_acquisition_mode (mCamera, ARV_ACQUISITION_MODE_CONTINUOUS);
			arv_option_frequency=GetConfigurationInt(wxT("TriggerTimerFPS"), 10);

			if (arv_option_frequency > 0.0)
				arv_camera_set_frame_rate (mCamera, arv_option_frequency);

			//if (arv_option_trigger != NULL)
			//	arv_camera_set_trigger (mCamera, arv_option_trigger);

			//if (arv_option_software_trigger > 0.0) {
			//	arv_camera_set_trigger (mCamera, "Software");
			//	software_trigger_source = g_timeout_add ((double) (0.5 + 1000.0 /
			//							   arv_option_software_trigger),
			//						 emit_software_trigger, mCamera);
			//}

			mOutputImage = cvCreateImage(cvSize(width,height), 8, 1);

			arv_camera_start_acquisition (mCamera);

			g_signal_connect (mStream, "new-buffer", G_CALLBACK (new_buffer_cb), this);
			arv_stream_set_emit_signals (mStream, TRUE);

			g_signal_connect (arv_camera_get_device (mCamera), "control-lost",
					  G_CALLBACK (control_lost_cb), NULL);


			//main_loop = g_main_loop_new (NULL, FALSE);

			//old_sigint_handler = signal (SIGINT, set_cancel);

			//g_main_loop_run (main_loop);
			imageProcessed=true;
		}
	} else {
		printf("no camera found");
	}
}

void THISCLASS::OnReloadConfiguration() {
	if (! mCamera) {
		return;
	}
	
	
	
	//set_camera_options();
	

}

void THISCLASS::OnStep() {
	
	if (! mCamera) {
		return;
	}

	// This is the acquired image
	//IplImage *outputimage = (IplImage*)(mCurrentResult.Context());
	
	// dirty spinlock to wait for new camera image...
	int loopcounter=0;
	while ((imageProcessed)&&(loopcounter<10000)) {
		usleep(100);
		loopcounter++;
	}
	//printf("loop %i", loopcounter);
	mCore->mDataStructureInput.mImage = mOutputImage;
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(mOutputImage);
	}
	// Parse the frame number (frame counter of the camera)


	// The camera returns a time stamp, but we prefer the time stamp of the computer here
	mCore->mDataStructureInput.SetFrameTimestamp(wxDateTime::UNow());
	imageProcessed=true;
	
}

void THISCLASS::OnStepCleanup() {
	if (! mCamera) {
		return;
	}


}

void THISCLASS::OnStop() {


	arv_camera_stop_acquisition (mCamera);

	g_object_unref (mStream);
	
}


#endif // USE_CAMERA_PYLON_GIGE
