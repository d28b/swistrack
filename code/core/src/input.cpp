#include "input.h"


void Input::SetParameters(){

	switch(mode){
		case 0: break;
		case 1:{
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
		default : throw "[Input::SetParameters] Illegal input mode";
	}
}

Input::Input(xmlpp::Element* cfgRoot)  
: Capture(0), isinputincolor(0), nFrame(0) 
{
	if(!IsAttrByXPath(cfgRoot,"/CFG/COMPONENTS/INPUT","mode"))
		throw "[Input::Input] Input mode undefined (/CFG/COMPONENTS/INPUT)";

	mode=GetIntAttrByXPath(cfgRoot,"/CFG/COMPONENTS/INPUT","mode");

	switch(mode){
		case 0 : /////////////// video file //////////////////////////
			CreateExceptionIfEmpty(cfgRoot,"/CFG/INPUT[@mode='0']");
			CreateExceptionIfEmpty(cfgRoot,"/CFG/INPUT[@mode='0']/AVIFILE");

			Capture = cvCaptureFromFile(GetValByXPath(cfgRoot,"/CFG/INPUT[@mode='0']/AVIFILE"));
			if (!Capture){
				FILE* f;				
				fopen_s(&f,GetValByXPath(cfgRoot,"/CFG/INPUT[@mode='0']/AVIFILE"),"r");
				if(f){
					fclose(f);
					throw "Input: Can not open AVI file (codec problem, VFW codec required, not DirectShow)";
				}
				else{
					fopen_s(&f,"swistrack.log","w");
					fprintf(f,"%s not found",GetValByXPath(cfgRoot,"/CFG/INPUT[@mode='0']/AVIFILE"));
					fclose(f);
					throw "Input: Can not open AVI file (file not found)";
				}
				return;
			}
			input =  cvQueryFrame( Capture );	
			if(input){
				if(strcmp(input->colorModel,"GRAY")==0)
					isinputincolor=0;
				else
					isinputincolor=1;          
			}
			cvSetCaptureProperty( Capture , CV_CAP_PROP_POS_FRAMES,0); // Reset to first frame



			break;
		case 1 : /////////////// 1394 camera /////////////////////////
#ifdef _1394
			CreateExceptionIfEmpty(cfgRoot,"/CFG/INPUT[@mode='1']");
			CreateExceptionIfEmpty(cfgRoot,"/CFG/INPUT[@mode='1']/MODE");
			CreateExceptionIfEmpty(cfgRoot,"/CFG/INPUT[@mode='1']/FRAMERATE");
			CreateExceptionIfEmpty(cfgRoot,"/CFG/INPUT[@mode='1']/AUTOWHITEBALANCE");
			CreateExceptionIfEmpty(cfgRoot,"/CFG/INPUT[@mode='1']/AUTOGAIN");
			CreateExceptionIfEmpty(cfgRoot,"/CFG/INPUT[@mode='1']/AUTOEXPOSURE");
			CreateExceptionIfEmpty(cfgRoot,"/CFG/INPUT[@mode='1']/WHITEBALANCE");
			CreateExceptionIfEmpty(cfgRoot,"/CFG/INPUT[@mode='1']/GAIN");
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

#endif
			break;
		case 2:{ /////////////// USB Cam /////////////////

			Capture =0;
			Capture = cvCaptureFromCAM(-1);
			if(!Capture)
				throw "[Input::Input] Cannot find USB Webcam";
			input =  cvQueryFrame( Capture );	
			if(input){
				if(strcmp(input->colorModel,"GRAY")==0)
					isinputincolor=0;
				else
					isinputincolor=1;          
			}
			else throw "[Input::Input] Cannot read from USB Webcam";
			   }
			   break;
		default:
			throw "[Input::Input] input mode not implemented";
	}

}

/** \brief Gives AVI progress
* 
* @param kind : format of the progress
*               - 1 = progress in milliseconds
*               - 2 = progress in frames
*               - 3 = progress in percent
* \result Returns progress in the format specified by 'kind'
* \todo Get the framerate from the camera in order to calculate the right time
*/
double Input::GetProgress(int kind){
	switch(kind){
			case 3:
				if(mode)
					return(1);
				else
					return(cvGetCaptureProperty( Capture, CV_CAP_PROP_POS_AVI_RATIO));
			case 1:
				if(mode)
					return(nFrame*1000/15);
				else
					return(cvGetCaptureProperty(Capture, CV_CAP_PROP_POS_MSEC));
			case 2:
				if(mode==1)
					return(nFrame);
				else
					return(cvGetCaptureProperty(Capture, CV_CAP_PROP_POS_FRAMES));
			default:
				throw "[Input::GetProgress] invalid type";
	}
	return 0;
}

IplImage* Input::GetFrame()
{
	switch(mode){
			case 0 :{ ////////////// Video from file //////////////////
				if(Capture){
					input =  cvQueryFrame( Capture );
					if (input)  // Get a pointer to the current frame (if query was successful)
					{
						nFrame++;                         // Increment the frame counter.
						return(input);
					} // end if input
				}
				else
				{
					throw "[Input::GetFrame] Video feed broken (Video from file)";
				}
					}
					break;
			case 1 :{ ///////////// IEEE 1394 Camera ///////////////
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
					}
					break;
			case 2:{ ////////////////// USB Camera ////////////////
				if(Capture){
					input =  cvQueryFrame( Capture );
					if (input)  // Get a pointer to the current frame (if query was successful)
					{
						nFrame++;                         // Increment the frame counter.
						return(input);
					} // end if input
				}
				else
				{
					throw "[Input::GetFrame] Video feed broken (USB Camera)";
				}
				   }
				   break;
			default : throw "[Input::GetFrame] Illegal input mode";
	}
	return NULL;
}


void Input::QueryFrame1394(IplImage* input)
{
#ifdef _1394
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
#endif
}

CvSize Input::GetInputDim(){
	return(cvSize(input->width,input->height));
}

int Input::GetInputDepth(){
	return input->depth;
}
int Input::GetInputNbChannels(){
	return input->nChannels;
}

int Input::GetInputOrigin(){
	return input->origin;
}

IplImage* Input::GetInputIpl(){
	return input;
}

Input::~Input()
{
	if (Capture) cvReleaseCapture( &Capture );
	//if (input) cvReleaseImage( &input);
}

double Input::GetFPS(){
	if(mode){
		return(15);
		/**
		* \todo Extract the real frame rate from the camera 
		*/
	}
	else
		if(Capture)
			return(cvGetCaptureProperty( Capture, CV_CAP_PROP_FPS ));
		else
			return(0);
}
