#include "input.h"



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

            if (theCamera.CheckLink() != CAM_SUCCESS){
				throw "[Input::Input] Can not access 1394 Camera";
                return;
                }
            
            theCamera.InitCamera();
            
            /**
            * \todo Fetch parameters from CFG instead setting default values 
            */
            theCamera.SetVideoFrameRate(3); // 15fps
            theCamera.SetVideoFormat(0);    // 640x480, 
            theCamera.SetVideoMode(4);		//rgb   yuv411	
			theCamera.m_controlGain.SetAutoMode(false); //Set gain control to manual
			theCamera.m_controlAutoExposure.TurnOn(false); //Set auto exposure to manual
			theCamera.m_controlWhiteBalance.SetAutoMode(false); //Set White balance to manual
			theCamera.SetBrightness(240);
			theCamera.SetAutoExposure(267);
			theCamera.SetSharpness(73);
			theCamera.SetWhiteBalance(172,69);
			theCamera.SetSaturation(103);
			
            
            theCamera.StartImageAcquisition();
            input=cvCreateImage(cvSize(theCamera.m_width,theCamera.m_height),8,3);
            QueryFrame1394(input);
            if(input){
                if(strcmp(input->colorModel,"GRAY")==0)
                    isinputincolor=0;
                else
                    isinputincolor=1;
			}
			else{
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
					cvNamedWindow("Input",0);
					cvShowImage("Input",input);
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
			return 0;
}


void Input::QueryFrame1394(IplImage* input)
			{
#ifdef _1394			
			while (theCamera.AcquireImage() != CAM_SUCCESS)
				throw "[Input::QueryFrame1394] Camera acquisition error";
			input->imageData=(char*) theCamera.m_pData;
			cvCvtColor(input,input,CV_RGB2BGR);
			/** \todo get dimensions from camera */
			input->width=640;
			input->height=480;
			input->nChannels=3;
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
		if (input) cvReleaseImage( &input);
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
