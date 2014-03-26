#include "ComponentInputCameraProselicaGigE.h"
#define THISCLASS ComponentInputCameraProselicaGigE

#ifdef USE_CAMERA_PROSILICA_GIGE
#include "cv.h"

THISCLASS::ComponentInputCameraProselicaGigE(SwisTrackCore *stc):
		Component(stc, wxT("InputCameraProselicaGigE")),mCameraUID(0),mCameraHandle(),mCameraFrame(),
		mCameraInfo(), mFrameSize(0), mWidth(0), mHeight(0),
		mFrameNumber(0), mErrcode(),
		mBayerImage(NULL),mColorImage(NULL),mGrayImage(NULL)
{

	// Data structure relations
	mCategory = &(mCore->mCategoryInput);
	AddDataStructureWrite(&(mCore->mDataStructureInput));

	// Trigger
	mTrigger = new ComponentTrigger(this);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentInputCameraProselicaGigE() {

  //delete trigger;
	delete mTrigger;
}

void THISCLASS::OnInitializeStatic() {
	PvInitialize();
}

void THISCLASS::OnTerminateStatic() {
	PvUnInitialize();
}

void THISCLASS::OnStart() {
	mCameraFullName = GetConfigurationString(wxT("CameraFullName"), wxT(""));
	mColor = GetConfigurationBool(wxT("Color"), false);	

	// Check camera initialization
	unsigned long numCams = 0;
  	numCams = PvCameraList(&mCameraInfo, 1, NULL);
    if (numCams == 1)
    {
      // Get the camera ID
      mCameraUID = mCameraInfo.UniqueId;
      printf("Found Camera ID \t\t= %d\n", mCameraUID);
      mCore->mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_StepLapTime, this);
      } else {
		if (numCams < 0) {
		AddError(wxT("No Proselica GigE camera found!"));
		return;
		}
     }

    // Open a  camera
	if(!PvCameraOpen(mCameraUID, ePvAccessMaster, &(mCameraHandle)))
    {
        //Reset camera config file to Factory or 1, 2, 3 etc.
        PvAttrEnumSet(mCameraHandle,"CofigFileIndex","Factory");
        mErrcode = PvCommandRun(mCameraHandle, "ConfigFileLoad");
        if (mErrcode != ePvErrSuccess)
        printf("Errcode: %d Could not load camera config: \n", mErrcode);

        // Get the image frame size
        mErrcode = PvAttrUint32Get(mCameraHandle, "TotalBytesPerFrame", &mFrameSize);
        if (mErrcode != ePvErrSuccess)
        printf("Errcode: %d Could not open camera: \n", mErrcode);
        printf("Frame size \t\t= %d\n", mFrameSize);

        // Allocate a buffer to store the image
        memset(&mCameraFrame, 0, sizeof(tPvFrame));
        mCameraFrame.ImageBufferSize = mFrameSize;
        mCameraFrame.ImageBuffer = cvAlloc(mFrameSize);

        // Get the width & height of the image
        PvAttrUint32Get(mCameraHandle, "Width", &mWidth);
        PvAttrUint32Get(mCameraHandle, "Height", &mHeight);
        // Get Bit depth, if needed
        unsigned long mBitDepth = 0;
        PvAttrUint32Get(mCameraHandle, "BitDepth", &mBitDepth);

        printf("Frame width \t\t= %d\n", mWidth);
        printf("Frame height \t\t= %d\n\n", mHeight);        

        // Start camera
        mErrcode = PvCaptureStart(mCameraHandle);
        if (mErrcode != ePvErrSuccess)
        printf("Errcode: %d Could not start camera: \n", mErrcode);

        // Set the camera to capture continuously
        mErrcode = PvAttrEnumSet(mCameraHandle, "AcquisitionMode", "Continuous");
        if (mErrcode != ePvErrSuccess)
        printf("Errcode: %d Could not set acquisition mode: \n", mErrcode);

        //Start image acquisition
        mErrcode = PvCommandRun(mCameraHandle, "AcquisitionStart");
        if (mErrcode != ePvErrSuccess)
        printf("Errcode: %d Could not start acquisition: \n", mErrcode);

        //Set Trigger mode to Freerun
        mErrcode = PvAttrEnumSet(mCameraHandle, "FrameStartTriggerMode", "Freerun");
        if (mErrcode != ePvErrSuccess)
        printf("Errcode: %d Could not set trigger mode: \n", mErrcode);
        if(mCameraHandle)
        printf("Handle OK\n");

        // parse xml configuration and do some camera image setup
        // Set Area/Region of interest
        tPvUint32 aoix = GetConfigurationInt("AOIOffset.x", 0);
        tPvUint32 aoiy = GetConfigurationInt("AOIOffset.y", 0);
        tPvUint32 aoiw = GetConfigurationInt("AOISize.x", 4872);
        tPvUint32 aoih = GetConfigurationInt("AOISize.y", 3248);
            if (aoix > mWidth - 1) {
                aoix = mWidth - 1;
            }
            if (aoiy > mHeight - 1) {
                aoiy = mHeight - 1;
            }
            if (aoix > mWidth) {
                aoix = mWidth;
            }
            if (aoiy > mHeight) {
                aoiy = mHeight;
            }

            if (aoiw > mWidth - aoix) {
                aoiw = mWidth - aoix;
            }
            if (aoih > mHeight - aoiy) {
                aoih = mHeight - aoiy;
            }
            aoiw = aoiw ^ (aoiw & 0x3);	// The image width must currently be a multiple of 4, for alignment reasons.
            aoih = aoih ^ (aoih & 0x3);	// The image height must currently be a multiple of 4.
            if (aoiw < 800) { // minimum width forced
                aoiw = 800;
            }
            if (aoih < 600) { // minimum height forced
                aoih = 600;
            }

            PvAttrUint32Set(mCameraHandle, "RegionX", aoix);
            PvAttrUint32Set(mCameraHandle, "RegionY", aoiy);
            PvAttrUint32Set(mCameraHandle, "Width", aoiw);
            PvAttrUint32Set(mCameraHandle, "Height", aoih);

            // Get the width & height of the image and update
            PvAttrUint32Get(mCameraHandle, "Width", &mWidth);
            PvAttrUint32Get(mCameraHandle, "Height", &mHeight);       
    }

    // Create necessary image headers and images
    
	// Create only a header for raw bayer image
    mBayerImage = cvCreateImageHeader(cvSize((int)mWidth, (int)mHeight), IPL_DEPTH_8U, 1);
    if (! mBayerImage) 
	{
		AddError(wxT("Error: Impossible to create the mBayerImage header."));
        return;
    }
    // Point bayer image to right buffer
    mBayerImage->widthStep = (int)mWidth;
    mBayerImage->imageData = (char *)mCameraFrame.ImageBuffer;

    // Create color image
    mColorImage  = cvCreateImage(cvSize((int)mWidth, (int)mHeight), IPL_DEPTH_8U, 3);
    if (!mColorImage) 
	{
        AddError(wxT("Failed to create mColorImage."));
        return;
    }
    
	//Create gray image
    mGrayImage  = cvCreateImage(cvSize((int)mWidth, (int)mHeight), IPL_DEPTH_8U, 1);
    if (!mGrayImage) 
	{
		AddError(wxT("Failed to create mGrayImage."));
        return;
    }

	// Configure reloadable values
	OnReloadConfiguration();

	
	//Start a thread waiting for the next image
	ComponentInputCameraProselicaGigE::Thread *ct = new ComponentInputCameraProselicaGigE::Thread(this);
	ct->Create();
	ct->Run();	
}

void THISCLASS::OnReloadConfiguration() {

	if (mCameraHandle)
	{
	    //Set the exposure mode: auto, manual, auto once etc. external mode
        mExposureMode = (eExposureMode)GetConfigurationInt(wxT("ExposureMode"), 0);

        if(mExposureMode == sExposure_Manual)
        {
            mErrcode = PvAttrEnumSet(mCameraHandle, "ExposureMode", wxT("Manual"));
            mExposureValue = GetConfigurationInt(wxT("ExposureValue"), 6000);
            PvAttrUint32Set(mCameraHandle, "ExposureValue", mExposureValue);
        } else if (mExposureMode == sExposure_AutoOnce) {
            mErrcode = PvAttrEnumSet(mCameraHandle, "ExposureMode", wxT("AutoOnce"));
        } else if (mExposureMode == sExposure_Auto){
            mErrcode = PvAttrEnumSet(mCameraHandle, "ExposureMode", wxT("Auto"));
        } else if (mExposureMode == sExposure_External){
            mErrcode = PvAttrEnumSet(mCameraHandle, "ExposureMode", wxT("External"));
        }

        //Set the gain
        mAnalogGain = GetConfigurationInt(wxT("AnalogGain"), 0);
        PvAttrUint32Set(mCameraHandle, "GainValue", mAnalogGain);


        //Set something else! See PVAPI SDK manual
        mColor = GetConfigurationBool(wxT("Color"), false);        
	}
	if(mErrcode != ePvErrSuccess)
	{
        printf("OnReloadConfiguration(): PV Error %d \n", mErrcode );
	}
	return;
}

void THISCLASS::OnStep() {

	// Convert image bayer8 -> Color
    cvCvtColor(mBayerImage, mColorImage,  CV_BayerGB2BGR);
	
	//Restart a thread waiting for the next image
	ComponentInputCameraProselicaGigE::Thread *ct = new ComponentInputCameraProselicaGigE::Thread(this);
	ct->Create();
	ct->Run();

	// Set Timestamp for the current frame
	// this data is not read from the camera, so we will do the
	// best we can: take a timestamp now.
	mCore->mDataStructureInput.SetFrameTimestamp(wxDateTime::UNow());

	if(!mColor)
	{
		//Gray Output
		cvCvtColor(mColorImage,mGrayImage,CV_BGRA2GRAY);    	
    	mCore->mDataStructureInput.mImage = mGrayImage;		
	} 
	else 
	{ 
		//Color Output
		mCore->mDataStructureInput.mImage = mColorImage;
	}

    // Set the frame number
    mCore->mDataStructureInput.mFrameNumber = mFrameNumber++;
}

void THISCLASS::OnStepCleanup() 
{	
}

void THISCLASS::OnStop()
{  
	mErrcode = PvCommandRun(mCameraHandle, "AcquisitionStop");
	if (mErrcode != ePvErrSuccess)
		fprintf(stderr, "Errcode: %d Could not stop video source: \n", mErrcode);

	// wait for thread to close
	wxCriticalSectionLocker csl(mThreadCriticalSection);

	PvCaptureEnd(mCameraHandle);
	PvCameraClose(mCameraHandle);

	cvFree(&(mCameraFrame.ImageBuffer));


	// Releases images 
	if (mBayerImage)   
	  cvReleaseImageHeader(&mBayerImage);
	if (mColorImage)
		cvReleaseImage(&mColorImage);

	}

	if (mGrayImage) {
		cvReleaseImage(&mGrayImage);
	}
}

wxThread::ExitCode THISCLASS::Thread::Entry() {
	wxCriticalSectionLocker csl(mComponent->mThreadCriticalSection);

	//Launch the frame capture
	if(!PvCaptureQueueFrame(mComponent->mCameraHandle, &(mComponent->mCameraFrame), NULL))
    {        
		// Wait for the grabbed image to be captured
        while(PvCaptureWaitForFrameDone(mComponent->mCameraHandle, &(mComponent->mCameraFrame), 1) == ePvErrTimeout);        
    } else {
      printf("failed to capture\n");
      return 0;
    }    
	mComponent->mTrigger->SetReady();
	return 0;
}
#endif

 	  	 
