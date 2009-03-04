#include "ComponentInputCameraGigE.h"
#define THISCLASS ComponentInputCameraGigE

#ifdef USE_CAMERA_PYLON_GIGE
#include "DisplayEditor.h"
#include "ImageConversion.h"

THISCLASS::ComponentInputCameraGigE(SwisTrackCore *stc):
		Component(stc, wxT("InputCameraGigE")),
		mCamera(0), mStreamGrabber(0), mCurrentResult(),
		mFrameNumber(0), mOutputImage(0) {

	// Data structure relations
	mCategory = &(mCore->mCategoryInput);
	AddDataStructureWrite(&(mCore->mDataStructureInput));

	// Trigger
	mTrigger = new ComponentTrigger(this);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentInputCameraGigE() {
	// Free the output image
	if (mOutputImage) {
		cvReleaseImage(&mOutputImage);
	}

	delete mTrigger;
}

void THISCLASS::OnInitializeStatic() {
	Pylon::PylonInitialize();
}

void THISCLASS::OnTerminateStatic() {
	Pylon::PylonTerminate();
}

void THISCLASS::OnStart() {
	mCameraFullName = GetConfigurationString(wxT("CameraFullName"), wxT(""));
	mColor = GetConfigurationBool(wxT("Color"), true);
	mTriggerMode = (eTriggerMode)GetConfigurationInt(wxT("TriggerMode"), 0);
	mTriggerTimerFPS = GetConfigurationInt(wxT("TriggerTimerFPS"), 10);
	mInputBufferSize = GetConfigurationInt(wxT("InputBufferSize"), 8);
	mFrameNumber = 0;

	printf("OnStart 1\n");

	// Check the maximum amount of buffers
	if (mInputBufferSize < 1) {
		mInputBufferSize = 1;
		AddWarning(wxString::Format(wxT("Using %d input buffer."), mInputBufferSize));
	}
	if (mInputBufferSize > mInputBufferSizeMax) {
		mInputBufferSize = mInputBufferSizeMax;
		AddWarning(wxString::Format(wxT("Using %d input buffers."), mInputBufferSize));
	}

	printf("OnStart 2\n");
	// Get the transport layer
	Pylon::CTlFactory& tlfactory = Pylon::CTlFactory::GetInstance();
	printf("OnStart 3\n");
	try {
		if (mCameraFullName.Len() == 0) {
			Pylon::DeviceInfoList_t devices;
			if (tlfactory.EnumerateDevices(devices) == 0) {
				AddError(wxT("No GigE cameras found!"));
				return;
			}
			mCore->mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_StepLapTime, this);
			mCamera = new Pylon::CBaslerGigECamera(tlfactory.CreateDevice(devices[0]));
			mCore->mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_StepLapTime, this);
		} else {
			// Create a camera object and cast to the camera class
			GenICam::gcstring camera_full_name = GenICam::gcstring(mCameraFullName.mb_str());
			mCamera = new Pylon::CBaslerGigECamera(tlfactory.CreateDevice(camera_full_name));
			if (! mCamera) {
				AddError(wxT("Camera not found!"));
				return;
			}
		}
		printf("OnStart 4\n");
		// Open the camera object
		mCamera->Open();
	} catch (GenICam::GenericException &e) {
		printf("%s\n", e.GetDescription());
		AddError(wxT("Error while opening the camera."));
		mCamera = 0;
		return;
	}
	printf("OnStart 5\n");
	// Pixel format: color or mono
	if (mColor) {
		mCamera->PixelFormat.SetValue(Basler_GigECameraParams::PixelFormat_YUV422Packed);
	} else {
		mCamera->PixelFormat.SetValue(Basler_GigECameraParams::PixelFormat_Mono8);
	}
	printf("OnStart 6\n");
	// Area of interest
	int cameraw = mCamera->Width.GetMax();
	int camerah = mCamera->Height.GetMax();
	int aoix = GetConfigurationInt(wxT("AOIOffset.x"), 0);
	int aoiy = GetConfigurationInt(wxT("AOIOffset.y"), 0);
	int aoiw = GetConfigurationInt(wxT("AOISize.x"), 64);
	int aoih = GetConfigurationInt(wxT("AOISize.y"), 64);
	if (aoix > cameraw - 1) {
		aoix = cameraw - 1;
	}
	if (aoiy > camerah - 1) {
		aoiy = camerah - 1;
	}
	if (aoiw > cameraw - aoix) {
		aoiw = cameraw - aoix;
	}
	if (aoih > camerah - aoiy) {
		aoih = camerah - aoiy;
	}
	aoiw = aoiw ^ (aoiw & 0x3);	// The image width must currently be a multiple of 4, for alignment reasons.
	aoih = aoih ^ (aoih & 0x1);	// The image height must currently be a multiple of 2.
	if (aoiw < 4) {
		aoiw = 4;
	}
	if (aoih < 2) {
		aoih = 2;
	}
	mCamera->OffsetX.SetValue(aoix);
	mCamera->OffsetY.SetValue(aoiy);
	mCamera->Width.SetValue(aoiw);
	mCamera->Height.SetValue(aoih);
	printf("OnStart 7\n");
	// Continuous mode
	mCamera->AcquisitionMode.SetValue(Basler_GigECameraParams::AcquisitionMode_Continuous);

	// Trigger
	if (mTriggerMode == sTrigger_Timer) {
		mCamera->TriggerMode.SetValue(Basler_GigECameraParams::TriggerMode_Off);
		mCamera->AcquisitionFrameRateEnable.SetValue(true);
		mCamera->AcquisitionFrameRateAbs.SetValue(mTriggerTimerFPS);
	} else {
		mCamera->TriggerMode.SetValue(Basler_GigECameraParams::TriggerMode_On);
		mCamera->AcquisitionFrameRateEnable.SetValue(false);
		mCamera->TriggerSelector.SetValue(Basler_GigECameraParams::TriggerSelector_AcquisitionStart);
		mCamera->TriggerActivation.SetValue(Basler_GigECameraParams::TriggerActivation_RisingEdge);
		if (mTriggerMode == sTrigger_InputLine1) {
			mCamera->TriggerSource.SetValue(Basler_GigECameraParams::TriggerSource_Line1);
		} else if (mTriggerMode == sTrigger_InputLine2) {
			mCamera->TriggerSource.SetValue(Basler_GigECameraParams::TriggerSource_Line2);
		} else if (mTriggerMode == sTrigger_InputLine3) {
			mCamera->TriggerSource.SetValue(Basler_GigECameraParams::TriggerSource_Line3);
		} else if (mTriggerMode == sTrigger_InputLine4) {
			mCamera->TriggerSource.SetValue(Basler_GigECameraParams::TriggerSource_Line4);
		} else {
			mTriggerMode = sTrigger_Software;
			mCamera->TriggerSource.SetValue(Basler_GigECameraParams::TriggerSource_Software);
		}
	}

	// Configure reloadable values
	OnReloadConfiguration();
	printf("OnStart 8\n");
	// Get and open a stream grabber
	mCamera->GetStreamGrabber(0);
	mStreamGrabber = new Pylon::CBaslerGigECamera::StreamGrabber_t(mCamera->GetStreamGrabber(0));
	printf("OnStart 8.1 %d\n", mStreamGrabber);
	mStreamGrabber->Open();
	printf("OnStart 8.2\n");
	// Parameterize the stream grabber
	const int buffersize = mCamera->PayloadSize.GetValue();
	mStreamGrabber->MaxBufferSize = buffersize;
	mStreamGrabber->MaxNumBuffer = mInputBufferSize;
	mStreamGrabber->PrepareGrab();
	printf("OnStart 9\n");
	// Allocate and register image buffers, put them into the grabber's input queue
	try {
		int channels = (mColor ? 2 : 1);
		for (int i = 0; i < mInputBufferSize; ++i) {
			mInputBufferImages[i] = cvCreateImage(cvSize(aoiw, aoih), 8, channels);
			mInputBufferHandles[i] = mStreamGrabber->RegisterBuffer(mInputBufferImages[i]->imageData, mInputBufferImages[i]->imageSize);
			mStreamGrabber->QueueBuffer(mInputBufferHandles[i], mInputBufferImages[i]);
		}
	} catch (GenICam::GenericException &e) {
		printf("%s\n", e.GetDescription());
		AddError(wxT("Error while registering image buffers."));
		mStreamGrabber->Close();
		mStreamGrabber = 0;
		mCamera->Close();
		mCamera = 0;
		return;
	}
	printf("OnStart 10\n");
	// Start image acquisition
	mCamera->AcquisitionStart.Execute();

	// Prepare the trigger for the next frame
	if (mTriggerMode == sTrigger_Software) {
		// When using the software trigger, we are immediately ready to read the next image
		mTrigger->SetReady();
	} else {
		// Otherwise, restart a thread waiting for the next image
		ComponentInputCameraGigE::Thread *ct = new ComponentInputCameraGigE::Thread(this);
		ct->Create();
		ct->Run();
	}
}

void THISCLASS::OnReloadConfiguration() {
	if (! mCamera) {
		return;
	}

	printf("OnReloadConfiguration 1\n");
	try {
		// Configure exposure time and mode
		int exposuretime = GetConfigurationInt(wxT("ExposureTime"), 100);
		mCamera->ExposureMode.SetValue(Basler_GigECameraParams::ExposureMode_Timed);
		mCamera->ExposureTimeRaw.SetValue(exposuretime);
		printf("OnReloadConfiguration 2\n");
		// Configure analog gain
		int analoggain = GetConfigurationInt(wxT("AnalogGain"), 500);
		mCamera->GainRaw.SetValue(analoggain);
	} catch (GenICam::GenericException &e) {
		printf("%s\n", e.GetDescription());
		AddError(wxT("Error while setting camera parameters."));
		return;
	}
}

void THISCLASS::OnStep() {
	if (! mCamera) {
		return;
	}
	printf("OnStep 1\n");
	// Get the image
	if (mTriggerMode == sTrigger_Software) {
		// Send the software trigger
		mCamera->TriggerSoftware.Execute();

		// Wait for the grabbed image with a timeout of 3 seconds
		if (! mStreamGrabber->GetWaitObject().Wait(3000)) {
			AddError(wxT("Failed to retrieve an image: the camera did not send any image."));
			return;
		}
	} else {
		// Wait for the thread to terminate (this should return immediately, as the thread should have terminated already)
		wxCriticalSectionLocker csl(mThreadCriticalSection);
	}

	// Get an item from the grabber's output queue
	mStreamGrabber->RetrieveResult(mCurrentResult);
	if (! mCurrentResult.Succeeded()) {
		AddError(wxString::Format(wxT("Failed to retrieve an item from the output queue: %s"), mCurrentResult.GetErrorDescription().c_str()));
		return;
	}

	// This is the acquired image
	IplImage *outputimage = (IplImage*)(mCurrentResult.Context());

	// If we are acquireing a color image, we need to transform it from YUV422 to BGR, otherwise we use the raw image
	if (mColor) {
		PrepareOutputImage(outputimage);
		ImageConversion::CvtYUV422ToBGR(outputimage, mOutputImage);
		mCore->mDataStructureInput.mImage = mOutputImage;
	} else {
		mCore->mDataStructureInput.mImage = outputimage;
	}

	// Set the frame number
	mFrameNumber++;
	mCore->mDataStructureInput.mFrameNumber = mFrameNumber;
}

void THISCLASS::OnStepCleanup() {
	if (! mCamera) {
		return;
	}
	printf("OnStepCleanup 1\n");
	// Requeue the used image
	try {
		mStreamGrabber->QueueBuffer(mCurrentResult.Handle(), mCurrentResult.Context());
	} catch (GenICam::GenericException &e) {
		AddError(wxString::Format(wxT("Failed to requeue buffer: %s"), e.GetDescription()));
	}

	// Prepare the trigger for the next frame
	if (mTriggerMode == sTrigger_Software) {
		// When using the software trigger, we are immediately ready to read the next image
		mTrigger->SetReady();
	} else {
		// Otherwise, restart a thread waiting for the next image
		ComponentInputCameraGigE::Thread *ct = new ComponentInputCameraGigE::Thread(this);
		ct->Create();
		ct->Run();
	}
}

void THISCLASS::OnStop() {
	if (! mCamera) {
		return;
	}
	printf("OnStop 1\n");
	// The camera is in continuous mode, stop image acquisition
	mCamera->AcquisitionStop.Execute();

	// Flush the input queue, grabbing may have failed
	mStreamGrabber->CancelGrab();
	printf("OnStop 2\n");
	// Wait for the thread to quit
	wxCriticalSectionLocker csl(mThreadCriticalSection);

	// Consume all items from the output queue
	Pylon::GrabResult result;
	while (mStreamGrabber->GetWaitObject().Wait(0)) {
		mStreamGrabber->RetrieveResult(result);
	}
	printf("OnStop 3\n");
	// Deregister and free buffers
	for (int i = 0; i < mInputBufferSize; ++i) {
		mStreamGrabber->DeregisterBuffer(mInputBufferHandles[i]);
		cvReleaseImage(&mInputBufferImages[i]);
	}
	printf("OnStop 4\n");
	// Clean up
	mStreamGrabber->FinishGrab();
	printf("OnStop 4.1\n");
	mStreamGrabber->Close();
	printf("OnStop 4.2\n");
	mStreamGrabber = 0;
	printf("OnStop 4.3\n");
	mCamera->Close();
	printf("OnStop 4.4\n");
	Pylon::CTlFactory& tlfactory = Pylon::CTlFactory::GetInstance();
	printf("OnStop 4.5\n");
	tlfactory.DestroyDevice(mCamera->GetDevice());
	printf("OnStop 4.7\n");
	mCamera = 0;
}

wxThread::ExitCode THISCLASS::Thread::Entry() {
	wxCriticalSectionLocker csl(mComponent->mThreadCriticalSection);
	printf("Thread::Entry 1\n");
	// Wait for the grabbed image
	while (! mComponent->mStreamGrabber->GetWaitObject().Wait(3000)) {
		// This is intentionally left empty
	}

	mComponent->mTrigger->SetReady();
	return 0;
}

#endif // USE_CAMERA_PYLON_GIGE
