#include "ComponentInputCameraGBit.h"
#define THISCLASS ComponentInputCameraGBit

#ifdef USE_CAMERA_PYLON_GBIT
#include <sstream>

THISCLASS::ComponentInputCameraGBit(SwisTrackCore *stc):
		Component(stc, "CameraGBit"),
		mTransportLayer(0), mCamera(0), mStreamGrabber(0),
		mCurrentImageIndex(-1), mFrameNumber(0),
		mDisplayImageOutput("Output", "GBit Camera: Input Frame") {

	// Data structure relations
	mDisplayName="GBit Camera";
	mCategory=&(mCore->mCategoryInput);
	AddDataStructureWrite(&(mCore->mDataStructureInput));
	AddDisplayImage(&mDisplayImageOutput);
}

THISCLASS::~ComponentInputCameraGBit() {
}

void THISCLASS::OnStart() {
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
		mCamera=dynamic_cast<Pylon::CBaslerGigECamera*>(mTransportLayer->CreateDevice(devices[0]));

		// Open the camera object
		mCamera->Open();
	} catch (GenICam::GenericException &e) {
		AddError(e.GetDescription());
		mCamera=0;
		mTransportLayer=0;
		return;
	}

	// Mono8 pixel format
	mCamera->PixelFormat.SetValue(Basler_GigECameraParams::PixelFormat_Mono8);

	// Maximized AOI
	mCamera->OffsetX.SetValue(0);
	mCamera->OffsetY.SetValue(0);
	mCamera->Width.SetValue(mCamera->Width.GetMax());
	mCamera->Height.SetValue(mCamera->Height.GetMax());

	// Continuous mode, no external trigger used
	mCamera->TriggerSelector.SetValue(Basler_GigECameraParams::TriggerSelector_AcquisitionStart);
	mCamera->TriggerMode.SetValue(Basler_GigECameraParams::TriggerMode_On);
	mCamera->TriggerSource.SetValue(Basler_GigECameraParams::TriggerSource_Software);
	mCamera->AcquisitionMode.SetValue(Basler_GigECameraParams::AcquisitionMode_Continuous);

	// Configure exposure time and mode
	mCamera->ExposureMode.SetValue(Basler_GigECameraParams::ExposureMode_Timed);
	mCamera->ExposureTimeRaw.SetValue(100);

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
	for (int i=0; i<8; ++i) {
		mInputBufferImages[i]=cvCreateImage(cvSize(mCamera->Width.GetMax(), mCamera->Height.GetMax()), 8, 1);
		mInputBufferHandles[i]=mStreamGrabber->RegisterBuffer(mInputBufferImages[i]->imageData, mInputBufferImages[i]->imageSize);
		mStreamGrabber->QueueBuffer(mInputBufferHandles[i], (void*)i);
	}

	// Start image acquisition
	mCamera->AcquisitionStart.Execute();
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
    // Let the camera acquire one single image
    mCamera->TriggerSoftware.Execute();

	// Wait for the grabbed image with a timeout of 3 seconds
	if (! mStreamGrabber->GetWaitObject().Wait(3000)) {
		AddError("Failed to retrieve an image: the camera did not send any image.");
		return;
	}

	// Requeue the previous image
	if (mCurrentImageIndex>-1) {
		mStreamGrabber->QueueBuffer(mInputBufferHandles[mCurrentImageIndex], (void*)mCurrentImageIndex);
		mCurrentImageIndex=-1;
	}

	// Get an item from the grabber's output queue
	Pylon::GrabResult result;
	mStreamGrabber->RetrieveResult(result);
	if (! result.Succeeded()) {
		std::ostringstream oss;
		oss << "Failed to retrieve an item from the output queue: " << result.GetErrorDescription();
		AddError(oss.str());
		return;
	}

	// Set the current image
	mFrameNumber++;
	mCurrentImageIndex=(int)result.Context();

	// Set this image in the DataStructureImage
	mCore->mDataStructureInput.mImage=mInputBufferImages[mCurrentImageIndex];
	mCore->mDataStructureInput.mFrameNumber=mFrameNumber;

	// Let the DisplayImage know about our image
	mDisplayImageOutput.mImage=mInputBufferImages[mCurrentImageIndex];
	std::ostringstream oss;
	oss << "Frame " << mFrameNumber << ", " << mInputBufferImages[mCurrentImageIndex]->width << "x" << mInputBufferImages[mCurrentImageIndex]->height;
	mDisplayImageOutput.mAnnotation1=oss.str();
}

void THISCLASS::OnStepCleanup() {
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
	mCamera=0;
	mTransportLayer=0;
	//Pylon::CTlFactory& tlfactory=Pylon::CTlFactory::GetInstance();
	//tlfactory.DestroyTl(mTransportLayer);
}

#endif
