// Segmenter.cpp: implementation of the Segmenter class.
//
//////////////////////////////////////////////////////////////////////

#include "Segmenter.h"



//////////////////////////////////////////////////////////////////////
// Private function called in the segementer
//////////////////////////////////////////////////////////////////////

/** To be removed when mode 3 will no more work with highgui*/
extern int cmx;
/** To be removed when mode 3 will no more work with highgui*/
extern int cmy;

/** Private function to be removed and transfered to the gui functions
*
*	Get the color value in a image using highgui, when you left click on the image and put it in specifiedColor
*
*/
extern void UpdateMouse(int event, int x, int y, int flags, void* param);
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/** Constructor
*
* Initializes segmentation parameters with defaults, opens video and background
* file, allocates necessary IplImages, and finally flips the background image in order
* to fit the format of the video. After successful construction, segmenter is in status
* READY_TO_START.
*
*/
Segmenter::Segmenter(xmlpp::Element* cfgRoot, TrackingImage* trackingimg) : binary(0),background(0),inputImg(0)
{    

	this->cfgRoot=cfgRoot;
	this->trackingimg=trackingimg;


	input = new Input(cfgRoot);
	binary = cvCreateImage(input->GetInputDim(),input->GetInputDepth(),1);
	binary->origin = input->GetInputOrigin();
	trackingimg->Init(input->GetInputIpl(),binary);


	if(!IsAttrByXPath(cfgRoot,"/CFG/COMPONENTS/SEGMENTER","mode"))
		throw "[Segmenter::Segmenter] Segmenter mode undefined (/CFG/COMPONENTS/INPUT)";

	/**
	* \todo We ignore particlefilter, imagefilter modes, all this should be 
	* transported in their own classes. "mode" will instead solely been used for the 
	* segmenter mode
	*/


	status = READY_TO_START;


	mode=GetIntAttrByXPath(cfgRoot,"/CFG/COMPONENTS/SEGMENTER","mode");

	switch(mode)
	{
	case 0 :
		{
			///////// Background Subtraction, working in grayscale, with fixed threshold ///////////////
			CreateExceptionIfEmpty(cfgRoot,"/CFG/SEGMENTER[@mode='0']");
			CreateExceptionIfEmpty(cfgRoot,"/CFG/SEGMENTER[@mode='0']/BGIMAGE");
			CreateExceptionIfEmpty(cfgRoot,"/CFG/SEGMENTER[@mode='0']/THRESHOLD");
			CreateExceptionIfEmpty(cfgRoot,"/CFG/SEGMENTER[@mode='0']/FIXED");

			IplImage* bg = cvLoadImage(GetValByXPath(cfgRoot,"/CFG/SEGMENTER[@mode='0']/BGIMAGE"), -1); 						
			if (!bg){
				status = CAN_NOT_OPEN_BACKGROUND_FILE;
				throw "Can not open background file";
			}

			background = cvCreateImage(input->GetInputDim(),input->GetInputDepth(),1);
			int i=bg->nChannels;
			switch(i)
			{
				//RGB case, we convert in Gray
			case 3:
				cvCvtColor(bg,background,CV_RGB2GRAY);
				break;
				//Already in Gray
			case 1:
				cvCopy(bg,background);
				break;
				//other cases, we take the first channel
			default:
				cvCvtPixToPlane(bg,background,NULL,NULL,NULL);
				break;
			}			

			// Flip image because AVI provides bottom left images and BMP top left.
			cvFlip(background,background,0);
			
			if (!background)
			{
				status = CAN_NOT_OPEN_BACKGROUND_FILE;
				throw "Can not open background file";
			}
			//We always calculate the background average, so we can select if we use the moving threshold during the segmentation
			backgroundAverage=(int)cvMean(background);
			break;
			cvReleaseImage(&bg);
		}
		/////////// Estimate Subtraction //////////////////
	case 1 :
		{
			CreateExceptionIfEmpty(cfgRoot,"/CFG/SEGMENTER[@mode='1']");
			CreateExceptionIfEmpty(cfgRoot,"/CFG/SEGMENTER[@mode='1']/THRESHOLD");
			CreateExceptionIfEmpty(cfgRoot,"/CFG/SEGMENTER[@mode='1']/ALPHA");
			
			background = cvCreateImage(input->GetInputDim(),input->GetInputDepth(),1);
			int i=input->GetInputNbChannels();
			switch(i)
			{
				//RGB case, we convert in Gray
			case 3:
				cvCvtColor(input->GetInputIpl(),background,CV_RGB2GRAY);
				break;
				//Already in Gray
			case 1:
				cvCopy(input->GetInputIpl(),background);
				break;
				//other cases, we take the first channel
			default:
				cvCvtPixToPlane(input->GetInputIpl(),background,NULL,NULL,NULL);
				break;
			}
			break;
		}
		// Bacground Soustration working by color ratio
	case 2:
		{
			CreateExceptionIfEmpty(cfgRoot,"/CFG/SEGMENTER[@mode='2']");				
			CreateExceptionIfEmpty(cfgRoot,"/CFG/SEGMENTER[@mode='2']/BGIMAGE");		
			CreateExceptionIfEmpty(cfgRoot,"/CFG/SEGMENTER[@mode='2']/THRESHOLD");

			IplImage* bg = cvLoadImage(GetValByXPath(cfgRoot,"/CFG/SEGMENTER[@mode='2']/BGIMAGE"), -1); 						
			if (!bg){
				status = CAN_NOT_OPEN_BACKGROUND_FILE;
				throw "[Segmenter::Segmenter] Can not open background file";
			}			
			int i=bg->nChannels;
			switch(i)
			{
				//RGB, perfect
			case 3:
				{
				//Create an image with the R/G and R/B factor
				background = cvCreateImage(input->GetInputDim(),IPL_DEPTH_32F,2);
				IplImage* tmpImage = cvCreateImage(input->GetInputDim(),IPL_DEPTH_32F,3);
				IplImage* Red = cvCreateImage(input->GetInputDim(),IPL_DEPTH_32F,1);
				IplImage* Green = cvCreateImage(input->GetInputDim(),IPL_DEPTH_32F,1);
				IplImage* Blue = cvCreateImage(input->GetInputDim(),IPL_DEPTH_32F,1);
				cvConvert(bg,tmpImage);
				cvSplit(tmpImage,Blue,Green,Red,NULL);				
				cvDiv(Red,Green,Green);				
				cvSetImageCOI(background,1);
				cvCopy(Green,background);
				cvDiv(Red,Blue,Blue);
				cvSetImageCOI(background,2);
				cvCopy(Blue,background);
				cvSetImageCOI(background,0);
				cvReleaseImage(&Red);
				cvReleaseImage(&Green);
				cvReleaseImage(&Blue);
				cvReleaseImage(&tmpImage);
				cvFlip(background,background,0);
				break;
				}
				//We will not be able to work
			default:
				throw "[Segmenter::Segmenter] The background image don't have 3 channels";
				break;
			}
			break;
		}
	//Searching for a specific color, no need of a background
	case 3:
		{
			CreateExceptionIfEmpty(cfgRoot,"/CFG/SEGMENTER[@mode='3']");
			CreateExceptionIfEmpty(cfgRoot,"/CFG/SEGMENTER[@mode='3']/THRESHOLD");
			CreateExceptionIfEmpty(cfgRoot,"/CFG/SEGMENTER[@mode='3']/RED");
			CreateExceptionIfEmpty(cfgRoot,"/CFG/SEGMENTER[@mode='3']/BLUE");
			CreateExceptionIfEmpty(cfgRoot,"/CFG/SEGMENTER[@mode='3']/GREEN");
			CreateExceptionIfEmpty(cfgRoot,"/CFG/SEGMENTER[@mode='3']/SELECTCOLORATSTART");
			CreateExceptionIfEmpty(cfgRoot,"/CFG/SEGMENTER[@mode='3']/COLORDESCRIBEFOREGROUND");
			CreateExceptionIfEmpty(cfgRoot,"/CFG/SEGMENTER[@mode='3']/WORKINGWITHCOLORRATIO");
			
			//If we use an GUI to select color
			if(GetIntValByXPath(cfgRoot,"/CFG/SEGMENTER[@mode='3']/SELECTCOLORATSTART"))
			{
				if (input->GetInputNbChannels()!=3)
					throw "[Segmenter::Segmenter] The input image needs 3 channels (RGB) for the chosen mode";
				cvNamedWindow("Click on the desired color in the image",CV_WINDOW_AUTOSIZE);
				cvShowImage("Click on the desired color in the image",input->GetInputIpl());
				cmx=-1;
				cvSetMouseCallback("Click on the desired color in the image", UpdateMouse);
				while(cmx<0)
					cvWaitKey();
				cvDestroyWindow("Click on the desired color in the image");
				if ((input->GetInputIpl())->origin==1)//Image is bottom left
					cmy=(input->GetInputIpl())->height-cmy-1;
				specifiedColor=cvGet2D(input->GetInputIpl(),cmy,cmx);//matrix reading, we define first the row (height/y) and then the column (width/x)
				char redChar[20];
				sprintf_s(redChar,"%f",(specifiedColor.val)[0]);
				SetParamByXPath(cfgRoot,"/CFG/SEGMENTER[@mode='3']/RED",redChar);
				char blueChar[20];
				sprintf_s(blueChar,"%f",(specifiedColor.val)[1]);
				SetParamByXPath(cfgRoot,"/CFG/SEGMENTER[@mode='3']/BLUE",blueChar);
				char greenChar[20];
				sprintf_s(greenChar,"%f",(specifiedColor.val)[2]);
				SetParamByXPath(cfgRoot,"/CFG/SEGMENTER[@mode='3']/GREEN",greenChar);
			}
		}
		break;
	default :
		throw "[Segmenter::Segmenter] Segmenter mode not implemented";
	}

	SetParameters(); // should query all VARIABLE parameters from CFG 
}

/** Destructor
*/
Segmenter::~Segmenter()
{

	if (binary) cvReleaseImage( &binary );
	if (background) cvReleaseImage( &background );
}

/** Returns Status
*
* \return
*  - FINISHED        = end of video
*  - RUNNING         = successful segmentation
*  - NO_AVI_SEQUENCE = video file not ready
*/
int Segmenter::GetStatus()
{
	return(status);
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
double Segmenter::GetProgress(int kind){
	return(input->GetProgress(kind));
}

/**  \brief Does one segmentation step
* stores resulting contours in "contours", that are retrieved
* by GetContours()
*
* \return
*  - FINISHED        = end of video
*  - RUNNING         = successful segmentation
*  - NO_AVI_SEQUENCE = video file not ready
*/
int Segmenter::Step()
{
	inputImg=input->GetFrame();
	if(inputImg){
		Segmentation();
		trackingimg->Refresh(input->GetInputIpl(),binary);				    
		status = RUNNING;
	}
	else                                // no more input, so stop cleanly.
	{                                   //
		status = FINISHED;                //
	} 

	return status;
}


/**  \brief Segments image by subtracting against background 
*
*
* 
*/
void Segmenter::Segmentation()
{
	switch(mode)
	{
	case 0 : 
		{ /////////// BACKGROUND SUBTRACTION, BW image, fixed threshold ////////////////
			IplImage* tmpImage = cvCreateImage(input->GetInputDim(),input->GetInputDepth(),1);
			try
			{
				//We convert the input image in black and white
				switch(inputImg->nChannels)
				{
					//RGB case, we convert in Gray
				case 3:
					cvCvtColor(inputImg,tmpImage,CV_RGB2GRAY);
					break;
					//Already in Gray
				case 1:
					cvCopy(inputImg,tmpImage);
					break;
					//other cases, we take the first channel
				default:
					cvCvtPixToPlane(inputImg,tmpImage,NULL,NULL,NULL);
					break;
				}

				cvAbsDiff(tmpImage, background, binary);
				if (fixedThresholdBoolean==0)
				{
					int currentImageAverage=(int)cvMean(inputImg);
					cvThreshold(binary,binary, (bin_threshold+currentImageAverage-backgroundAverage), 255,  CV_THRESH_BINARY);
				}
				else
					cvThreshold(binary,binary, bin_threshold, 255,  CV_THRESH_BINARY);
			}
			catch(...)
			{
				throw "[Segmenter::Segmentation] with fixed background failed";
			}
		cvReleaseImage(&tmpImage);
		break;
		}

	case 1 : 
		{ /////////// ESTIMATE SUBTRACTION, BW image //////////////////
        			IplImage* tmpImage = cvCreateImage(input->GetInputDim(),input->GetInputDepth(),1);
			try
			{
				//We convert the input image in black and white
				switch(inputImg->nChannels)
				{
					//RGB case, we convert in Gray
				case 3:
					cvCvtColor(inputImg,tmpImage,CV_RGB2GRAY);
					break;
					//Already in Gray
				case 1:
					cvCopy(inputImg,tmpImage);
					break;
					//other cases, we take the first channel
				default:
					cvCvtPixToPlane(inputImg,tmpImage,NULL,NULL,NULL);
					break;
				}				
                cvAddWeighted(tmpImage,alpha,background,1.0-alpha,0,background);				
				cvAbsDiff(tmpImage, background, binary);
				cvThreshold(binary,binary, bin_threshold, 255,  CV_THRESH_BINARY);
			}
			catch(...)
			{
				 throw "[Segmenter::Segmentation] with background estimate failed";
			}
			cvReleaseImage(&tmpImage);
		}
		break;
	case 2:
		{ /////////// BACKGROUND SUBTRACTION, color ration, color image ////////////////
			IplImage* tmpImage = cvCreateImage(input->GetInputDim(),IPL_DEPTH_32F,3);
			try
			{
				// no special treatment needed in this case
				switch(inputImg->nChannels)
				{
					//RGB, perfect
				case 3:
					{
						//Create an image with the R/G and R/B factor
						IplImage* Red = cvCreateImage(input->GetInputDim(),IPL_DEPTH_32F,1);
						IplImage* Green = cvCreateImage(input->GetInputDim(),IPL_DEPTH_32F,1);
						IplImage* Blue = cvCreateImage(input->GetInputDim(),IPL_DEPTH_32F,1);
						cvConvert(inputImg,tmpImage);
						cvSplit(tmpImage,Blue,Green,Red,NULL);
						//We put R/G in Green
						cvDiv(Red,Green,Green);
						//We put R/B in Blue
						cvDiv(Red,Blue,Blue);

						//We get the Rb/Gb from the background and put it in Red
						cvSetImageCOI(background,1);
						cvCopy(background,Red);
						//We put R/G-Rb/Gb in Green
						cvSub(Green,Red,Green);
						//We divide by Rb/Gb
						cvDiv(Green,Red,Green);
						cvAbs(Green,Green);

						//We get the Rb/Bb from the background and put it in Red
						cvSetImageCOI(background,2);
						cvCopy(background,Red);
						//We put R/G-Bb/Bb in Blue
						cvSub(Blue,Red,Blue);
						//We divide by Rb/Bb
						cvDiv(Blue,Red,Blue);
						cvAbs(Blue,Blue);

						//We combined the two in Red
						cvAddWeighted(Green,50,Blue,50,0,Red);
   						cvThreshold(Red,binary, bin_threshold, 255,  CV_THRESH_BINARY);
						cvSetImageCOI(background,0);
						cvReleaseImage(&Red);
						cvReleaseImage(&Green);
						cvReleaseImage(&Blue);
						break;
					}
					//We will not be able to work
				default:
					throw "[Segmenter::Segmentation] The input image doesn't have 3 channels";
					break;
				}
			}
			catch(...)
			{
				throw "[Segmenter::Segmentation] with fixed background failed and color ratio";
			}
			cvReleaseImage(&tmpImage);
			break;
		}
	case 3://Tracking a specified color
		{	
			try
			{
				IplImage* tmpImage;
				// no special treatment needed in this case
				switch(inputImg->nChannels)
				{
					//RGB, perfect
				case 3:
					{	
						if (workingWithColorRatioBoolean)
						{
							//Create an image with the R/G and R/B factor
							tmpImage = cvCreateImage(input->GetInputDim(),IPL_DEPTH_32F,3);
							IplImage* Red = cvCreateImage(input->GetInputDim(),IPL_DEPTH_32F,1);
							IplImage* Green = cvCreateImage(input->GetInputDim(),IPL_DEPTH_32F,1);
							IplImage* Blue = cvCreateImage(input->GetInputDim(),IPL_DEPTH_32F,1);
							cvConvert(inputImg,tmpImage);
							cvSplit(tmpImage,Blue,Green,Red,NULL);
							//We put R/G in Green
							cvDiv(Red,Green,Green);
							//We put R/B in Blue
							cvDiv(Red,Blue,Blue);
							//Calculate Rb/Gb
							CvScalar rbRatio;
							if ((specifiedColor.val)[1])
								rbRatio=cvScalar((specifiedColor.val)[0]/(specifiedColor.val)[1]);
							else 
								if ((specifiedColor.val)[0])
									rbRatio=cvScalar(0);
								else
									rbRatio=cvScalar(255);
							//We put R/G-Rb/Gb in Green
							cvSubS(Green,rbRatio,Green);
							cvAbs(Green,Green);
							double rgWeight=(rbRatio.val)[0];
							
							//Calculate Rb/Bb
							if ((specifiedColor.val)[2])
								rbRatio=cvScalar((specifiedColor.val)[0]/(specifiedColor.val)[2]);
							else 
								if ((specifiedColor.val)[0])
									rbRatio=cvScalar(0);
								else
									rbRatio=cvScalar(255);
							//We put R/B-Rb/Bb in Blue
							cvSubS(Blue,rbRatio,Blue);
							cvAbs(Blue,Blue);
							double rbWeight=(rbRatio.val)[0];

							//We combined the two in Red
							cvAddWeighted(Green,rgWeight,Blue,rbWeight,0,Red);
   							cvThreshold(Red,binary, bin_threshold, 255,  CV_THRESH_BINARY);
							cvReleaseImage(&Red);
							cvReleaseImage(&Green);
							cvReleaseImage(&Blue);
						}
						else
						{
 							cvNamedWindow("toto",CV_WINDOW_AUTOSIZE);
 							tmpImage = cvCreateImage(input->GetInputDim(),IPL_DEPTH_8U,3);
							cvShowImage("toto",inputImg);
							cvWaitKey();
							cvCopy(inputImg,tmpImage);
							//cvAbsDiffS(inputImg,tmpImage,specifiedColor);
							IplImage* Red = cvCreateImage(input->GetInputDim(),IPL_DEPTH_8U,1);
							IplImage* Green = cvCreateImage(input->GetInputDim(),IPL_DEPTH_8U,1);
							IplImage* Blue = cvCreateImage(input->GetInputDim(),IPL_DEPTH_8U,1);
							cvShowImage("toto",tmpImage);
							cvWaitKey();
 							cvSplit(tmpImage,Blue,Green,Red,NULL);
							cvShowImage("toto",Red);
							cvWaitKey();
							cvShowImage("toto",Blue);
							cvWaitKey();							
							cvShowImage("toto",Green);
							cvWaitKey();
							cvShowImage("toto",tmpImage);
							cvWaitKey();
							cvSplit(tmpImage,Blue,Green,Red,NULL);
							cvAbsDiffS(Red,Red,cvScalarAll(150));
							cvShowImage("toto",Red);
							cvWaitKey();
							cvAbsDiffS(Blue,Blue,cvScalarAll(50));
							cvShowImage("toto",Blue);
							cvWaitKey();
							cvAbsDiffS(Green,Green,cvScalarAll(50));
							cvShowImage("toto",Green);
							cvWaitKey();
							cvThreshold(Red,Red,bin_threshold,255,CV_THRESH_BINARY);
							cvThreshold(Blue,Blue,bin_threshold,255,CV_THRESH_BINARY);
							cvThreshold(Green,Green,bin_threshold,255,CV_THRESH_BINARY);
							cvZero(binary);
							
							cvShowImage("toto",Red);
							cvWaitKey();
							cvShowImage("toto",Blue);
							cvWaitKey();
							cvShowImage("toto",Green);
							cvWaitKey();
							cvAnd(Red,Blue,binary,Green);
						}
						break;
					}
					//We will not be able to work
				default:
					throw "[Segmenter::Segmentation] The input image doesn't have 3 channels";
					break;
				}
				cvReleaseImage(&tmpImage);
			}
			catch(...)
			{
				throw "[Segmenter::Segmentation] with specified color tracking failed";
			}
			break;
		}
	default : 
		{
			throw "Invalid segmentation mode";
		}
		break;
	}
	// Perform a morphological opening to reduce noise. 
	
	cvNamedWindow("DEBUG",CV_WINDOW_AUTOSIZE);
	cvShowImage("DEBUG",binary);
	cvWaitKey();
	cvDestroyWindow("DEBUG");
	
	cvErode(binary, binary, NULL, 2);
	cvDilate(binary, binary, NULL, 2);

}





/**	\brief Return the binary image resulting of the segmentation
*	
*	Return a pointer to the binary image resulting of the segmentation.
*	
*	\return IplImage pointer to the binary image
*/
IplImage* Segmenter::GetBinaryImage()
{
	return binary;
}

/**	\brief Return the background image used for the segmentation
*	
*	Return a pointer to the background image.
*	
*	\return IplImage pointer to the background image
*/
IplImage* Segmenter::GetBackgroundImage()
{
	return background;
}

/**	\brief Return the input image used for the segemenation
*	
*	Return a pointer to the input image used for the segmetation.
*	
*	\return IplImage point to the input image used for the segementation
*/
IplImage* Segmenter::GetInputImage()
{
	return inputImg;
}




/**  \brief Initializes segmenter, by discarding the first few frames.
*
* \param overhead: Number of inital frames to discard
* \result Returns the current status
* \todo The segmenter has to be divided into input and segmenter part
*     
*/
int Segmenter::init(int overhead)
{
	/*
	status = READY_TO_START;						// change status
	for(int i=1; i<=overhead; i++){
		if(mode == 1){ // ESTIMATE
			inputImg=input->GetFrame();
			cvConvertScale(inputImg,inputImg,0.3);
			cvConvertScale(background,background,0.7);
			cvAdd(inputImg,background,background);
		}
	}
	trackingimg->RefreshCoverage(background);
	*/
	return(OK);
}


double Segmenter::GetSqrDist(CvPoint2D32f p1, CvPoint2D32f p2)
{
	return(
		(p1.x-p2.x)*
		(p1.x-p2.x)
		+
		(p1.y-p2.y)*
		(p1.y-p2.y)
		);
}

void Segmenter::SetParameters()
{
	switch(mode)
	{
	case 0 : 
		{ /////////// BACKGROUND SUBTRACTION ////////////////
			bin_threshold = GetIntValByXPath(cfgRoot,"/CFG/SEGMENTER[@mode='0']/THRESHOLD");
			fixedThresholdBoolean = GetIntValByXPath(cfgRoot,"/CFG/SEGMENTER[@mode='0']/FIXED");
		}
		break;
	case 1 : 
		{ /////////// ESTIMATE SUBTRACTION //////////////////
			bin_threshold = GetIntValByXPath(cfgRoot,"/CFG/SEGMENTER[@mode='1']/THRESHOLD");
			alpha = GetDoubleValByXPath(cfgRoot,"/CFG/SEGMENTER[@mode='1']/ALPHA");
		}
		break;
	case 2 : 
		{ /////////// BACKGROUND SUBTRACTION ////////////////
			bin_threshold = GetIntValByXPath(cfgRoot,"/CFG/SEGMENTER[@mode='2']/THRESHOLD");
		}
		break;
	case 3:
		{//////////// SPECIFIC COLOR TRACKING //////////////
			specifiedColor=cvScalar(GetIntValByXPath(cfgRoot,"/CFG/SEGMENTER[@mode='3']/RED"),GetIntValByXPath(cfgRoot,"/CFG/SEGMENTER[@mode='3']/BLUE"),GetIntValByXPath(cfgRoot,"/CFG/SEGMENTER[@mode='3']/GREEN"));
			bin_threshold = GetIntValByXPath(cfgRoot,"/CFG/SEGMENTER[@mode='3']/THRESHOLD");
			colorDescribeForegroundBoolean = GetIntValByXPath(cfgRoot,"/CFG/SEGMENTER[@mode='3']/COLORDESCRIBEFOREGROUND");
			workingWithColorRatioBoolean = GetIntValByXPath(cfgRoot,"/CFG/SEGMENTER[@mode='3']/WORKINGWITHCOLORRATIO");
		}
		break;
	default : 
		{
			throw "[Segmenter::SetParameters] Invalid segmentation mode";
		}
		break;
	}
}

double Segmenter::GetFPS()
{
	return(input->GetFPS());
}

void Segmenter::RefreshCoverage()
{
	trackingimg->RefreshCoverage(background);
}
