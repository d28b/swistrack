#include "ComponentInputCameraGBit.h"
#define THISCLASS ComponentInputCameraGBit

#ifdef USE_CAMERA_PYLON_GBIT
#include <sstream>
#include "DisplayEditor.h"

THISCLASS::ComponentInputCameraGBit(SwisTrackCore *stc):
		Component(stc, "InputCameraGBit"),
		mTransportLayer(0), mCamera(0), mStreamGrabber(0),
		mCurrentImageIndex(-1), mFrameNumber(0) {

	// Data structure relations
	mCategory=&(mCore->mCategoryInput);
	AddDataStructureWrite(&(mCore->mDataStructureInput));

	// Trigger
	mTrigger=new ComponentTrigger(this);
	
	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentInputCameraGBit() {
	delete mTrigger;
}

void THISCLASS::OnStart() {
	mCameraNumber=GetConfigurationInt("DeviceNumber", 0);
	mColor=GetConfigurationBool("Color", true);
	mTriggerMode=(eTriggerMode)GetConfigurationInt("TriggerMode", 0);
	mTriggerTimerFPS=GetConfigurationInt("TriggerTimerFPS", 10);

	try {
		// Enumerate GigE cameras
		Pylon::CTlFactory& tlfactory=Pylon::CTlFactory::GetInstance();
		mTransportLayer=tlfactory.CreateTl(Pylon::CBaslerGigECamera::DeviceClass());
		Pylon::DeviceInfoList_t devices;
		if (mTransportLayer->EnumerateDevices(devices)==0) {
			AddError("No GBit cameras found!");
			return;
		}

		// Create a camera object and cast to the concrete camera class
		mCamera=dynamic_cast<Pylon::CBaslerGigECamera*>(mTransportLayer->CreateDevice(devices[mCameraNumber]));
		if (! mCamera) {
			AddError("Camera not found!");
			return;
		}

		// Open the camera object
		mCamera->Open();
	} catch (GenICam::GenericException &e) {
		AddError(e.GetDescription());
		mCamera=0;
		mTransportLayer=0;
		return;
	}

	// Pixel format: color or mono
	if (mColor) {
		mCamera->PixelFormat.SetValue(Basler_GigECameraParams::PixelFormat_YUV422Packed);
	} else {
		mCamera->PixelFormat.SetValue(Basler_GigECameraParams::PixelFormat_Mono8);
	}

	// Maximized AOI
	// TODO: check size, because this probably needs to be a multiple of 2 or 4 for Pylon
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
	if (aoiw<1) {aoiw=1;}
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
	const int buffersize=(int)mCamera->PayloadSize();
	const int numbuffers=10;
	mStreamGrabber->MaxBufferSize=buffersize;
	mStreamGrabber->MaxNumBuffer=numbuffers;
	mStreamGrabber->PrepareGrab();

	// Allocate and register image buffers, put them into the grabber's input queue
	int channels=(mColor ? 2 : 1);
	for (int i=0; i<numbuffers; ++i) {
		mInputBufferImages[i]=cvCreateImage(cvSize(aoiw, aoih), 8, channels);
		mInputBufferHandles[i]=mStreamGrabber->RegisterBuffer(mInputBufferImages[i]->imageData, mInputBufferImages[i]->imageSize);
		mStreamGrabber->QueueBuffer(mInputBufferHandles[i], (void*)i);
	}

	// Start image acquisition
	mCamera->AcquisitionStart.Execute();

	// In case of an external trigger, start a thread waiting for the images
	if (mTriggerMode!=sTrigger_Software) {
		ComponentInputCameraGBit::Thread *ct=new ComponentInputCameraGBit::Thread(this);
		ct->Create();
		ct->Run();
	}
}

void THISCLASS::OnReloadConfiguration() {
	try {
		// Configure exposure time and mode
		int exposuretime=GetConfigurationInt("ExposureTime", 100);
		mCamera->ExposureMode.SetValue(Basler_GigECameraParams::ExposureMode_Timed);
		mCamera->ExposureTimeRaw.SetValue(exposuretime);

		// Configure analog gain
		int analoggain=GetConfigurationInt("AnalogGain", 500);
		mCamera->GainRaw.SetValue(analoggain);
	} catch (GenICam::GenericException &e) {
		AddError(e.GetDescription());
		return;
	}
}

void THISCLASS::OnStep() {
	Pylon::GrabResult result;

    // Get the image
	if (mTriggerMode==sTrigger_Software) {
		// Send the software trigger
		mCamera->TriggerSoftware.Execute();

		// Wait for the grabbed image with a timeout of 3 seconds
		if (! mStreamGrabber->GetWaitObject().Wait(3000)) {
			AddError("Failed to retrieve an image: the camera did not send any image.");
			return;
		}

		// Get an item from the grabber's output queue
		mStreamGrabber->RetrieveResult(result);
		if (! result.Succeeded()) {
			std::ostringstream oss;
			oss << "Failed to retrieve an item from the output queue: " << result.GetErrorDescription();
			AddError(oss.str());
			return;
		}
	} else {
		wxCriticalSectionLocker csl(mThread->mCriticalSection);
		// TODO
	}


	// Set the current image
	mFrameNumber++;
	mCurrentImageIndex=(int)result.Context();

	// This is the acquired image
	IplImage *outputimage=mInputBufferImages[mCurrentImageIndex];

	// Set this image in the DataStructureImage
	mCore->mDataStructureInput.mImage=outputimage;
	mCore->mDataStructureInput.mFrameNumber=mFrameNumber;
}

void THISCLASS::OnStepCleanup() {
	// Requeue the used image
	if (mCurrentImageIndex>-1) {
		mStreamGrabber->QueueBuffer(mInputBufferHandles[mCurrentImageIndex], (void*)mCurrentImageIndex);
		mCurrentImageIndex=-1;
	}
}

void THISCLASS::OnStop() {
	// The camera is in continuous mode, stop image acquisition 
	mCamera->AcquisitionStop.Execute();

	// Flush the input queue, grabbing may have failed
	mStreamGrabber->CancelGrab();

	// Consume all items from the output queue
	Pylon::GrabResult result;
	while (mStreamGrabber->GetWaitObject().Wait(0)) {
		mStreamGrabber->RetrieveResult(result);
	}

	// Deregister and free buffers
	for (int i=0; i<8; ++i) {
		mStreamGrabber->DeregisterBuffer(mInputBufferHandles[i]);
		cvReleaseImage(&mInputBufferImages[i]);
	}
	mCurrentImageIndex=-1;

	// Clean up
	mStreamGrabber->FinishGrab();
	mStreamGrabber->Close();
	mStreamGrabber=0;
	mCamera->Close();
	mTransportLayer->DestroyDevice(mCamera);
	Pylon::CTlFactory& tlfactory=Pylon::CTlFactory::GetInstance();
	tlfactory.DestroyTl(mTransportLayer);
	mCamera=0;
	mTransportLayer=0;
}

wxThread::ExitCode THISCLASS::Thread::Entry() {
	return 0;
}

#endif
