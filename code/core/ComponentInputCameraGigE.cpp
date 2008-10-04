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
	mCategory=&(mCore->mCategoryInput);
	AddDataStructureWrite(&(mCore->mDataStructureInput));

	// Trigger
	mTrigger=new ComponentTrigger(this);
	
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
	mCameraFullName=GetConfigurationString(wxT("CameraFullName"), wxT(""));
	mColor=GetConfigurationBool(wxT("Color"), true);
	mTriggerMode=(eTriggerMode)GetConfigurationInt(wxT("TriggerMode"), 0);
	mTriggerTimerFPS=GetConfigurationInt(wxT("TriggerTimerFPS"), 10);
	mInputBufferSize=GetConfigurationInt(wxT("InputBufferSize"), 8);

	// Check the maximum amount of buffers
	if (mInputBufferSize < 1) {
		mInputBufferSize = 1;
		AddWarning(wxString::Format(wxT("Using %d input buffer."), mInputBufferSize));
	}
	if (mInputBufferSize > mInputBufferSizeMax) {
		mInputBufferSize = mInputBufferSizeMax;
		AddWarning(wxString::Format(wxT("Using %d input buffers."), mInputBufferSize));
	}

	// Get the transport layer
	Pylon::CTlFactory& tlfactory=Pylon::CTlFactory::GetInstance();

	try {
		if (mCameraFullName.Len()==0) {
			Pylon::DeviceInfoList_t devices;
			if (tlfactory.EnumerateDevices(devices)==0) {
				AddError(wxT("No GigE cameras found!"));
				return;
			}
			mCore->mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_StepLapTime, this);
			mCamera=dynamic_cast<Pylon::CBaslerGigECamera*>(tlfactory.CreateDevice(devices[0]));
			mCore->mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_StepLapTime, this);
		} else {
			// Create a camera object and cast to the camera class
			mCamera=dynamic_cast<Pylon::CBaslerGigECamera*>(tlfactory.CreateDevice(mCameraFullName.mb_str(wxConvISO8859_1)));
			if (! mCamera) {
				AddError(wxT("Camera not found!"));
				return;
			}
		}

		// Open the camera object
		mCamera->Open();
	} catch (GenICam::GenericException &e) {
		AddError(e.GetDescription());
		mCamera=0;
		return;
	}

	// Pixel format: color or mono
	if (mColor) {
		mCamera->PixelFormat.SetValue(Basler_GigECameraParams::PixelFormat_YUV422Packed);
	} else {
		mCamera->PixelFormat.SetValue(Basler_GigECameraParams::PixelFormat_Mono8);
	}

	// Area of interest
	int cameraw=mCamera->Width.GetMax();
	int camerah=mCamera->Height.GetMax();
	int aoix=GetConfigurationInt("AOIOffset.x", 0);
	int aoiy=GetConfigurationInt("AOIOffset.y", 0);
	int aoiw=GetConfigurationInt("AOISize.x", 64);
	int aoih=GetConfigurationInt("AOISize.y", 64);
	if (aoix>cameraw-1) {aoix=cameraw-1;}
	if (aoiy>camerah-1) {aoiy=camerah-1;}
	if (aoiw>cameraw-aoix) {aoiw=cameraw-aoix;}
	if (aoih>camerah-aoiy) {aoih=camerah-aoiy;}
	aoiw=aoiw ^ (aoiw & 0x3);	// The image width must currently be a multiple of 4, for alignment reasons.
	if (aoiw<4) {aoiw=4;}
	if (aoih<1) {aoih=1;}
	mCamera->OffsetX.SetValue(aoix);
	mCamera->OffsetY.SetValue(aoiy);
	mCamera->Width.SetValue(aoiw);
	mCamera->Height.SetValue(aoih);

	// Continuous mode
	mCamera->AcquisitionMode.SetValue(Basler_GigECameraParams::AcquisitionMode_Continuous);

	// Trigger
	if (mTriggerMode==sTrigger_Timer) {
		mCamera->TriggerMode.SetValue(Basler_GigECameraParams::TriggerMode_Off);
		mCamera->AcquisitionFrameRateEnable.SetValue(true);
		mCamera->AcquisitionFrameRateAbs.SetValue(mTriggerTimerFPS);
	} else {
		mCamera->TriggerMode.SetValue(Basler_GigECameraParams::TriggerMode_On);
		mCamera->AcquisitionFrameRateEnable.SetValue(false);
		mCamera->TriggerSelector.SetValue(Basler_GigECameraParams::TriggerSelector_AcquisitionStart);
		mCamera->TriggerActivation.SetValue(Basler_GigECameraParams::TriggerActivation_RisingEdge);
		if (mTriggerMode==sTrigger_InputLine1) {
			mCamera->TriggerSource.SetValue(Basler_GigECameraParams::TriggerSource_Line1);
		} else if (mTriggerMode==sTrigger_InputLine2) {
			mCamera->TriggerSource.SetValue(Basler_GigECameraParams::TriggerSource_Line2);
		} else if (mTriggerMode==sTrigger_InputLine3) {
			mCamera->TriggerSource.SetValue(Basler_GigECameraParams::TriggerSource_Line3);
		} else if (mTriggerMode==sTrigger_InputLine4) {
			mCamera->TriggerSource.SetValue(Basler_GigECameraParams::TriggerSource_Line4);
		} else {
			mTriggerMode=sTrigger_Software;
			mCamera->TriggerSource.SetValue(Basler_GigECameraParams::TriggerSource_Software);
		}
	}

	// Configure reloadable values
	OnReloadConfiguration();

	// Get and open a stream grabber
	mStreamGrabber=dynamic_cast<Pylon::CBaslerGigEStreamGrabber*>(mCamera->GetStreamGrabber(0));
	mStreamGrabber->Open();

	// Parameterize the stream grabber
	const int buffersize=mCamera->PayloadSize.GetValue();
	mStreamGrabber->MaxBufferSize=buffersize;
	mStreamGrabber->MaxNumBuffer=mInputBufferSize;
	mStreamGrabber->PrepareGrab();

	// Allocate and register image buffers, put them into the grabber's input queue
	try {
		int channels=(mColor ? 2 : 1);
		for (int i=0; i<mInputBufferSize; ++i) {
			mInputBufferImages[i]=cvCreateImage(cvSize(aoiw, aoih), 8, channels);
			mInputBufferHandles[i]=mStreamGrabber->RegisterBuffer(mInputBufferImages[i]->imageData, mInputBufferImages[i]->imageSize);
			mStreamGrabber->QueueBuffer(mInputBufferHandles[i], mInputBufferImages[i]);
		}
	} catch (GenICam::GenericException &e) {
		printf("%s\n", e.GetDescription());
		AddError(e.GetDescription());
		mStreamGrabber->Close();
		mStreamGrabber=0;
		mCamera->Close();
		mCamera=0;
		return;
	}

	// Start image acquisition
	mCamera->AcquisitionStart.Execute();

	// Prepare the trigger for the next frame
	if (mTriggerMode==sTrigger_Software) {
		// When using the software trigger, we are immediately ready to read the next image
		mTrigger->SetReady();
	} else {
		// Otherwise, restart a thread waiting for the next image
		ComponentInputCameraGigE::Thread *ct=new ComponentInputCameraGigE::Thread(this);
		ct->Create();
		ct->Run();
	}
}

void THISCLASS::OnReloadConfiguration() {
	try {
		// Configure exposure time and mode
		int exposuretime=GetConfigurationInt(wxT("ExposureTime"), 100);
		mCamera->ExposureMode.SetValue(Basler_GigECameraParams::ExposureMode_Timed);
		mCamera->ExposureTimeRaw.SetValue(exposuretime);

		// Configure analog gain
		int analoggain=GetConfigurationInt(wxT("AnalogGain"), 500);
		mCamera->GainRaw.SetValue(analoggain);
	} catch (GenICam::GenericException &e) {
		AddError(e.GetDescription());
		return;
	}
}

void THISCLASS::OnStep() {
	// Get the image
	if (mTriggerMode==sTrigger_Software) {
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
		AddError(wxString::Format(wxT("Failed to retrieve an item from the output queue: %s"), mCurrentResult.GetErrorDescription()));
		return;
	}

	// This is the acquired image
	IplImage *outputimage=(IplImage*)(mCurrentResult.Context());

	// If we are acquireing a color image, we need to transform it from YUV422 to BGR, otherwise we use the raw image
	if (mColor) {
		PrepareOutputImage(outputimage);
		ImageConversion::CvtYUV422ToBGR(outputimage, mOutputImage);
		mCore->mDataStructureInput.mImage=mOutputImage;
	} else {
		mCore->mDataStructureInput.mImage=outputimage;
	}

	// Set the frame number
	mFrameNumber++;
	mCore->mDataStructureInput.mFrameNumber=mFrameNumber;
}

void THISCLASS::OnStepCleanup() {
	// Requeue the used image
	try {
		mStreamGrabber->QueueBuffer(mCurrentResult.Handle(), mCurrentResult.Context());
	} catch (GenICam::GenericException &e) {
		AddError(wxString::Format(wxT("Failed to requeue buffer: %s", e.GetDescription()));
	}

	// Prepare the trigger for the next frame
	if (mTriggerMode==sTrigger_Software) {
		// When using the software trigger, we are immediately ready to read the next image
		mTrigger->SetReady();
	} else {
		// Otherwise, restart a thread waiting for the next image
		ComponentInputCameraGigE::Thread *ct=new ComponentInputCameraGigE::Thread(this);
		ct->Create();
		ct->Run();
	}
}

void THISCLASS::OnStop() {
	// The camera is in continuous mode, stop image acquisition 
	mCamera->AcquisitionStop.Execute();

	// Flush the input queue, grabbing may have failed
	mStreamGrabber->CancelGrab();

	// Wait for the thread to quit
	wxCriticalSectionLocker csl(mThreadCriticalSection);

	// Consume all items from the output queue
	Pylon::GrabResult result;
	while (mStreamGrabber->GetWaitObject().Wait(0)) {
		mStreamGrabber->RetrieveResult(result);
	}

	// Deregister and free buffers
	for (int i=0; i<mInputBufferSize; ++i) {
		mStreamGrabber->DeregisterBuffer(mInputBufferHandles[i]);
		cvReleaseImage(&mInputBufferImages[i]);
	}

	// Clean up
	mStreamGrabber->FinishGrab();
	mStreamGrabber->Close();
	mStreamGrabber=0;
	mCamera->Close();
	Pylon::CTlFactory& tlfactory=Pylon::CTlFactory::GetInstance();
	tlfactory.DestroyDevice(mCamera);
	mCamera=0;
}

wxThread::ExitCode THISCLASS::Thread::Entry() {
	wxCriticalSectionLocker csl(mComponent->mThreadCriticalSection);
	
	// Wait for the grabbed image
	while (! mComponent->mStreamGrabber->GetWaitObject().Wait(3000)) {
	}

	mComponent->mTrigger->SetReady();
	return 0;
}

#endif // USE_CAMERA_PYLON_GIGE
