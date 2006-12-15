#include "ComponentInputCamera1394.h"
#define THISCLASS ComponentInputCamera1394

#ifdef _1394

THISCLASS::ComponentInputCamera1394(xmlpp::Element* cfgRoot):
		Capture(0), isinputincolor(0), nFrame(0) {

			CreateExceptionIfEmpty(cfgRoot,"/CFG/INPUT[@mode='1']");
			CreateExceptionIfEmpty(cfgRoot,"/CFG/INPUT[@mode='1']/MODE");
			CreateExceptionIfEmpty(cfgRoot,"/CFG/INPUT[@mode='1']/FRAMERATE");
			CreateExceptionIfEmpty(cfgRoot,"/CFG/INPUT[@mode='1']/AUTOWHITEBALANCE");
			CreateExceptionIfEmpty(cfgRoot,"/CFG/INPUT[@mode='1']/AUTOGAIN");
			CreateExceptionIfEmpty(cfgRoot,"/CFG/INPUT[@mode='1']/AUTOEXPOSURE");
			CreateExceptionIfEmpty(cfgRoot,"/CFG/INPUT[@mode='1']/EXPOSURE");
			CreateExceptionIfEmpty(cfgRoot,"/CFG/INPUT[@mode='1']/BRIGHTNESS");
			CreateExceptionIfEmpty(cfgRoot,"/CFG/INPUT[@mode='1']/SHARPNESS");
			CreateExceptionIfEmpty(cfgRoot,"/CFG/INPUT[@mode='1']/WHITEBALANCE1");
			CreateExceptionIfEmpty(cfgRoot,"/CFG/INPUT[@mode='1']/WHITEBALANCE2");
			CreateExceptionIfEmpty(cfgRoot,"/CFG/INPUT[@mode='1']/SATURATION");
	
	   		if (theCamera.CheckLink() != CAM_SUCCESS){
				throw "[Input::Input] Can not access 1394 Camera (Make sure CMU driver is installed).";
				return;
			}
			if (theCamera.InitCamera() != CAM_SUCCESS){
				throw "[Input::Input] Can not Init 1394 Camera";
				return;
			}

			theCamera.StatusControlRegisters();

			switch(GetIntValByXPath(cfgRoot,"/CFG/INPUT[@mode='1']/MODE")){
				case 0 : theCamera.SetVideoFormat(0); theCamera.SetVideoMode(0); break;
				case 1 : theCamera.SetVideoFormat(0); theCamera.SetVideoMode(1); break;
				case 2 : theCamera.SetVideoFormat(0); theCamera.SetVideoMode(2); break;
				case 3 : theCamera.SetVideoFormat(0); theCamera.SetVideoMode(3); break;
				case 4 : theCamera.SetVideoFormat(0); theCamera.SetVideoMode(4); break;
				case 5 : theCamera.SetVideoFormat(0); theCamera.SetVideoMode(5); break;
				case 6 : theCamera.SetVideoFormat(1); theCamera.SetVideoMode(0); break;
				case 7 : theCamera.SetVideoFormat(1); theCamera.SetVideoMode(1); break;
				case 8 : theCamera.SetVideoFormat(1); theCamera.SetVideoMode(2); break;
				case 9 : theCamera.SetVideoFormat(1); theCamera.SetVideoMode(3); break;
				case 10: theCamera.SetVideoFormat(1); theCamera.SetVideoMode(4); break;
				case 11: theCamera.SetVideoFormat(1); theCamera.SetVideoMode(5); break;
				case 12: theCamera.SetVideoFormat(2); theCamera.SetVideoMode(0); break;
				case 13: theCamera.SetVideoFormat(2); theCamera.SetVideoMode(1); break;
				case 14: theCamera.SetVideoFormat(2); theCamera.SetVideoMode(2); break;
				case 15: theCamera.SetVideoFormat(2); theCamera.SetVideoMode(3); break;
				case 16: theCamera.SetVideoFormat(2); theCamera.SetVideoMode(4); break;
				case 17: theCamera.SetVideoFormat(2); theCamera.SetVideoMode(5); break;
				default : throw "[Input::Input] Video format not implemented"; break;
			}

			SetParameters();
			
			if (theCamera.StartImageAcquisition() != 0) {
				throw "[Input::Input] Could not start image acquisition";
			}
			input=cvCreateImage(cvSize(theCamera.m_width,theCamera.m_height),8,3);
			//Always color
			isinputincolor=1;
			QueryFrame1394(input);
			if(!input)
			{
				throw "[Input::Input] Could not read from IEEE1394 camera";
			}

}

void THISCLASS::SetParameters(){
	theCamera.SetVideoFrameRate(GetIntValByXPath(cfgRoot,"/CFG/INPUT[@mode='1']/FRAMERATE")); 
	theCamera.m_controlGain.SetAutoMode(GetIntValByXPath(cfgRoot,"/CFG/INPUT[@mode='1']/AUTOGAIN")); 
	theCamera.m_controlAutoExposure.TurnOn(GetIntValByXPath(cfgRoot,"/CFG/INPUT[@mode='1']/AUTOEXPOSURE")); 
	theCamera.m_controlWhiteBalance.SetAutoMode(GetIntValByXPath(cfgRoot,"/CFG/INPUT[@mode='1']/AUTOWHITEBALANCE"));
	theCamera.SetBrightness(GetIntValByXPath(cfgRoot,"/CFG/INPUT[@mode='1']/BRIGHTNESS"));
	theCamera.SetAutoExposure(GetIntValByXPath(cfgRoot,"/CFG/INPUT[@mode='1']/EXPOSURE"));
	theCamera.SetSharpness(GetIntValByXPath(cfgRoot,"/CFG/INPUT[@mode='1']/SHARPNESS"));
	theCamera.SetWhiteBalance(GetIntValByXPath(cfgRoot,"/CFG/INPUT[@mode='1']/WHITEBALANCE1"),GetIntValByXPath(cfgRoot,"/CFG/INPUT[@mode='1']/WHITEBALANCE2"));
	theCamera.SetSaturation(GetIntValByXPath(cfgRoot,"/CFG/INPUT[@mode='1']/SATURATION"));
}

void THISCLASS::Step() {
	if (! mCapture) {return 0;}	

	IplImage* inputimage = cvQueryFrame(Capture);
	if (! inputimage) {return false;}

	mCore->mDataStructureImage.mImage=inputimage;	
	mCore->mDataStructureImage.mFrameNumber++;
	return true;

				QueryFrame1394(input);
				if (input)  // Get a pointer to the current frame (if query was successful)
				{
					nFrame++;                         // Increment the frame counter.                     
					return(input);
				} // end if input
				else
				{
					throw "[Input::GetFrame] Video feed broken (IEEE 1394 camera)";
				}

	return NULL;
}


void Input::QueryFrame1394(IplImage* input) {
	//Get a new frame from the camera and throw an error if we grab nothing.
	switch(theCamera.AcquireImage())
	{
	case CAM_SUCCESS:
		{
			break;
		}
	case CAM_ERROR_NOT_INITIALIZED:
		{
			throw "[Input::QueryFrame1394] Camera not initialized";
			break;
		}
	default : throw "[Input::QueryFrame1394] Camera acquisition error";
	}

	//point the input IPLImage to the camera buffer
	input->imageData=(char*) theCamera.m_pData;
	//Convert the input in the right format (RGB to BGR)
	cvCvtColor(input,input,CV_RGB2BGR);
}

#endif
