#include "input.h"



Input::Input(xmlpp::Element* cfgRoot)  
	  : Capture(0), current(0), isinputincolor(0), nFrame(0), m_pBitmap(0) 
{
	    if(!IsAttrByXPath(cfgRoot,"/CFG/COMPONENTS/INPUT","mode"))
        throw "[Input::Input] Input mode undefined (/CFG/COMPONENTS/INPUT)";

		mode=GetIntAttrByXPath(cfgRoot,"/CFG/COMPONENTS/INPUT","mode");

		switch(mode){
        case 0 : /////////////// video file //////////////////////////
            if(!IsDefined(cfgRoot,"/CFG/INPUT[@mode='0']"))
                throw "[Segmenter::Segmenter] no parameters for input mode 0 found";
            
            if(!IsDefined(cfgRoot,"/CFG/INPUT[@mode='0']/AVIFILE"))
                throw "[Segmenter::Segmenter] input video not specified (/CFG/INPUT[@mode='0']/AVIFILE)";
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
                
                
                current = cvCreateImage(cvSize(input->width,input->height),input->depth,1);
                cvCvtPixToPlane(input,NULL,NULL,current,NULL);              
                }
            cvSetCaptureProperty( Capture , CV_CAP_PROP_POS_FRAMES,0); // Reset to first frame
            
            
            
            break;
        case 1 : /////////////// 1394 camera /////////////////////////
#ifdef _1394

            if (theCamera.CheckLink() != CAM_SUCCESS){
                throw "Segmenter: Can not access 1394 Camera";
                return;
                }
            
            theCamera.InitCamera();
            
            /**
            * \todo Fetch parameters from CFG instead setting default values 
            */
            theCamera.SetVideoFrameRate(4); // 15fps
            theCamera.SetVideoFormat(0);    // 640x480, yuv411
            theCamera.SetVideoMode(2);
            
            m_pBitmap = new unsigned char [theCamera.m_width * theCamera.m_height * 3];
            
            theCamera.StartImageAcquisition();
            input=cvCreateImage(cvSize(theCamera.m_width,theCamera.m_height),8,3);
            QueryFrame1394(input);
            if(input){
                if(strcmp(input->colorModel,"GRAY")==0)
                    isinputincolor=0;
                else
                    isinputincolor=1;
                
                
                current = cvCreateImage(cvSize(input->width,input->height),input->depth,1);
                cvCvtPixToPlane(input,NULL,NULL,current,NULL);
                
                //binary = cvCreateImage(cvSize(current->width,current->height),current->depth,1);
                //binary->origin = current->origin;               
                }
			else{
				throw "[Segmenter::Segmenter] Could not read from IEEE1394 camera";
			}
            
#endif
            break;
        default:
            throw "[Segmenter::Segmenter] segmenter mode not implemented";
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
				if(mode)
					return(nFrame);
				else
					return(cvGetCaptureProperty(Capture, CV_CAP_PROP_POS_FRAMES));
            default:
                throw "INTERNAL ERROR: Segmenter::GetProgress invalid type";
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
                        /** \todo removing the channels like that is dangerous, if it is a 2 channels file, it will crash.*/
                        cvCvtPixToPlane(input,NULL,NULL,current,NULL);
						return(current);
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
                    
                    cvCvtPixToPlane(input,NULL,NULL,current,NULL);
      				return(current);
					} // end if input
                else
                    {
                    throw "[Input::GetFrame] Video feed broken (IEEE 1394 camera)";
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
			theCamera.AcquireImage();
			theCamera.getRGB(m_pBitmap);
			
			input->imageData=(char*) m_pBitmap;
			/** \todo get dimensions from camera */
			input->width=640;
			input->height=480;
			input->nChannels=3;
#endif
			}

CvSize Input::GetInputDim(){
	 return(cvSize(current->width,current->height));
}

int Input::GetInputDepth(){
	return current->depth;
}

int Input::GetInputOrigin(){
	return current->origin;
}

IplImage* Input::GetInputIpl(){
	return input;
}

Input::~Input()
{
		if (Capture) cvReleaseCapture( &Capture );
		if (current) cvReleaseImage( &current);
		if (m_pBitmap) delete(m_pBitmap);
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
