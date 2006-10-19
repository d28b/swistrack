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
Segmenter::Segmenter(xmlpp::Element* cfgRoot, TrackingImage* trackingimg) : binary(0),background(0),inputImg(0),mask(0)
{    

	this->cfgRoot=cfgRoot;
	this->trackingimg=trackingimg;


	input = new Input(cfgRoot);
	binary = cvCreateImage(input->GetInputDim(),input->GetInputDepth(),1);
	binary->origin = input->GetInputOrigin();
	trackingimg->Init(input->GetInputIpl(),binary);

	if(!IsAttrByXPath(cfgRoot,"/CFG/COMPONENTS/SEGMENTER","mode"))
	 throw "[Segmenter::Segmenter] Segmenter mode undefined (/CFG/COMPONENTS/SEGMENTER)";

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
				throw "[Segementer::Segmenter] Can not open background file";
			}

			background = cvCreateImage(input->GetInputDim(),input->GetInputDepth(),1);
			switch(bg->nChannels)
			{
				//BGR case, we convert in Gray
			case 3:
				cvCvtColor(bg,background,CV_BGR2GRAY);
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
			//cvFlip(background,background,0);
			
			if (!background)
			{
				status = CAN_NOT_OPEN_BACKGROUND_FILE;
				throw "[Segmenter::Segmenter] Can not create background file";
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
			switch(input->GetInputNbChannels())
			{
				//BGR case, we convert in Gray
			case 3:
				cvCvtColor(input->GetInputIpl(),background,CV_BGR2GRAY);
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
			if (!background)
			{
				status = CAN_NOT_OPEN_BACKGROUND_FILE;
				throw "[Segmenter::Segmenter] Can not create background file";
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
				//Add 1 to remove the problem with 0/0 (number will be from 1 to 256)
				cvConvertScale(bg,tmpImage,1,1);
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
				// Flip image because AVI provides bottom left images and BMP top left.
				//cvFlip(background,background,0);
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
				sprintf_s(redChar,"%f",(specifiedColor.val)[2]);
				SetParamByXPath(cfgRoot,"/CFG/SEGMENTER[@mode='3']/RED",redChar);
				char blueChar[20];
				sprintf_s(blueChar,"%f",(specifiedColor.val)[1]);
				SetParamByXPath(cfgRoot,"/CFG/SEGMENTER[@mode='3']/BLUE",blueChar);
				char greenChar[20];
				sprintf_s(greenChar,"%f",(specifiedColor.val)[0]);
				SetParamByXPath(cfgRoot,"/CFG/SEGMENTER[@mode='3']/GREEN",greenChar);
			}
		}
		break;
	default :
		throw "[Segmenter::Segmenter] Segmenter mode not implemented";
	}

	//Test Post Processing Variables
	if(!IsAttrByXPath(cfgRoot,"/CFG/COMPONENTS/SEGMENTERPP","mode"))
	 throw "[Segmenter::Segmenter] Segmenter Post Processing mode undefined (/CFG/COMPONENTS/SEGMENTERPP)";
	CreateExceptionIfEmpty(cfgRoot,"/CFG/SEGMENTERPP[@mode='0']");
	CreateExceptionIfEmpty(cfgRoot,"/CFG/SEGMENTERPP[@mode='0']/FIRSTDILATE");
	CreateExceptionIfEmpty(cfgRoot,"/CFG/SEGMENTERPP[@mode='0']/FIRSTERODE");
	CreateExceptionIfEmpty(cfgRoot,"/CFG/SEGMENTERPP[@mode='0']/SECONDDILATE");
	CreateExceptionIfEmpty(cfgRoot,"/CFG/SEGMENTERPP[@mode='0']/USINGMASK");
	CreateExceptionIfEmpty(cfgRoot,"/CFG/SEGMENTERPP[@mode='0']/MASKIMAGE");
	if(GetIntValByXPath(cfgRoot,"/CFG/SEGMENTERPP[@mode='0']/USINGMASK"))
	{
		mask = cvLoadImage(GetValByXPath(cfgRoot,"/CFG/SEGMENTERPP[@mode='0']/MASKIMAGE"), -1); 						
		if (!mask)
		{
			throw "[Segementer::Segmenter] Can not open mask file";
		}
		if (mask->nChannels!=1)
			throw "[Segmenter::Segmenter] The mask must be a black and white image";
		// Flip image because AVI provides bottom left images and BMP top left.
		//cvFlip(mask);
	}


	SetParameters(); // should query all VARIABLE parameters from CFG 
}

/** Destructor
*/
Segmenter::~Segmenter()
{
	if (binary) cvReleaseImage( &binary );
	if (background) cvReleaseImage( &background );
	if (mask) cvReleaseImage(&mask);
	//if (inputImg) cvReleaseImage(&inputImg);
	if (input) delete input;

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
double Segmenter::GetProgress(int kind)
{
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
					//BGR case, we convert in Gray
				case 3:
					cvCvtColor(inputImg,tmpImage,CV_BGR2GRAY);
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
				//Correct the tmpImage with the difference in image mean
				if (fixedThresholdBoolean==0)
					cvAddS(tmpImage,cvScalar(backgroundAverage-cvMean(tmpImage)),tmpImage);
				//Background Substraction
				cvAbsDiff(tmpImage, background, binary);
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
					//BGR case, we convert in Gray
				case 3:
					cvCvtColor(inputImg,tmpImage,CV_BGR2GRAY);
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
				//Update background
                cvAddWeighted(tmpImage,alpha,background,1.0-alpha,0,background);				
				//Background substraction
				cvAbsDiff(tmpImage, background, binary);
				//Threshold
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
			try
			{
				// no special treatment needed in this case
				switch(inputImg->nChannels)
				{
					//RGB, perfect
				case 3:
					{
						IplImage* tmpImage = cvCreateImage(input->GetInputDim(),IPL_DEPTH_32F,3);
						//Create an image with the R/G and R/B factor
						IplImage* Red = cvCreateImage(input->GetInputDim(),IPL_DEPTH_32F,1);
						IplImage* Green = cvCreateImage(input->GetInputDim(),IPL_DEPTH_32F,1);
						IplImage* Blue = cvCreateImage(input->GetInputDim(),IPL_DEPTH_32F,1);
						//add 1 to remove 0/0 problems, number from 1 to 256
						cvConvertScale(inputImg,tmpImage,1,1);						
						cvSplit(tmpImage,Blue,Green,Red,NULL);
						//We put R/G in Green
						cvDiv(Red,Green,Green);
						//We put R/B in Blue
						cvDiv(Red,Blue,Blue);

						//We get the Rb/Gb from the background and put it in Red
						cvSetImageCOI(background,1);
						cvCopy(background,Red);
						//We put abs(R/G-Rb/Gb) in Green
						cvAbsDiff(Green,Red,Green);
						//We divide by Rb/Gb
						cvDiv(Green,Red,Green);

						//We get the Rb/Bb from the background and put it in Red
						cvSetImageCOI(background,2);
						cvCopy(background,Red);
						//We put abs(R/G-Bb/Bb) in Blue
						cvAbsDiff(Blue,Red,Blue);
						//We divide by Rb/Bb
						cvDiv(Blue,Red,Blue);

						//We combined the two in Red (factor 50 is to make the values more easy to variate with the threshold slider
						cvAddWeighted(Green,50,Blue,50,0,Red);
   						cvThreshold(Red,binary, bin_threshold, 255,  CV_THRESH_BINARY);
						cvSetImageCOI(background,0);						
						cvReleaseImage(&Red);
						cvReleaseImage(&Green);
						cvReleaseImage(&Blue);
						cvReleaseImage(&tmpImage);
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
			break;
		}
	case 3://Tracking a specified color
		{	
			try
			{
				// no special treatment needed in this case
				switch(inputImg->nChannels)
				{
					//RGB, perfect
				case 3:
					{
						IplImage* tmpImage;
						if (workingWithColorRatioBoolean)
						{
							//Create an image with the R/G and R/B factor
							tmpImage = cvCreateImage(input->GetInputDim(),IPL_DEPTH_32F,3);
							IplImage* Red = cvCreateImage(input->GetInputDim(),IPL_DEPTH_32F,1);
							IplImage* Green = cvCreateImage(input->GetInputDim(),IPL_DEPTH_32F,1);
							IplImage* Blue = cvCreateImage(input->GetInputDim(),IPL_DEPTH_32F,1);
							//Add 1 to the image to remove div by 0 problemes, numbers from 1 to 256
							cvConvertScale(inputImg,tmpImage,1,1);
							cvSplit(tmpImage,Blue,Green,Red,NULL);
							//We put R/G in Green
							cvDiv(Red,Green,Green);
							//We put R/B in Blue
							cvDiv(Red,Blue,Blue);
							//Calculate Rb/Gb
							double rgWeight=((specifiedColor.val)[2]+1)/((specifiedColor.val)[1]+1);							
							//We put abs(R/G-Rb/Gb) in Green
							cvAbsDiffS(Green,Green,cvScalar(rgWeight));
							
							//Calculate Rb/Bb
							double rbWeight=((specifiedColor.val)[2]+1)/((specifiedColor.val)[0]+1);													
							//We put abs(R/B-Rb/Bb) in Blue
							cvAbsDiffS(Blue,Blue,cvScalar(rbWeight));

							//We combined the two in Red							
							cvAddWeighted(Green,rgWeight*50,Blue,rbWeight*50,0,Red);
											

							//Test if the color describe the foreground or the background
							if (colorDescribeForegroundBoolean)
								cvThreshold(Red,binary, bin_threshold, 255,  CV_THRESH_BINARY_INV);
							else
								cvThreshold(Red,binary, bin_threshold, 255,  CV_THRESH_BINARY);
							cvReleaseImage(&Red);
							cvReleaseImage(&Green);
							cvReleaseImage(&Blue);
						}
						else
						{
 							tmpImage = cvCreateImage(input->GetInputDim(),IPL_DEPTH_8U,3);
							cvAbsDiffS(inputImg,tmpImage,specifiedColor);
							IplImage* Red = cvCreateImage(input->GetInputDim(),IPL_DEPTH_8U,1);
							IplImage* Green = cvCreateImage(input->GetInputDim(),IPL_DEPTH_8U,1);
							IplImage* Blue = cvCreateImage(input->GetInputDim(),IPL_DEPTH_8U,1);
							cvSplit(tmpImage,Blue,Green,Red,NULL);							
							cvThreshold(Red,Red,bin_threshold,255,CV_THRESH_BINARY_INV);
							cvThreshold(Blue,Blue,bin_threshold,255,CV_THRESH_BINARY_INV);
							cvThreshold(Green,Green,bin_threshold,255,CV_THRESH_BINARY_INV);
							cvZero(binary);						
							cvAnd(Red,Blue,binary,Green);
							if (!colorDescribeForegroundBoolean)
								cvNot(binary,binary);
							cvReleaseImage(&Red);
							cvReleaseImage(&Green);
							cvReleaseImage(&Blue);
						}
						cvReleaseImage(&tmpImage);
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
	//If we want to use a mask, perform an AND function between the binary image and the mask
	if (usingMaskBoolean)
		cvAnd(binary,mask,binary);
	// Perform a morphological opening to reduce noise.
	if (firstDilate)
		cvDilate(binary,binary,NULL,firstDilate);
	if (firstErode)
		cvErode(binary, binary, NULL, firstErode);
	if (secondDilate)
		cvDilate(binary, binary, NULL, secondDilate);

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
/*int Segmenter::init(int overhead)
{
	return(OK);
}*/


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
			specifiedColor=cvScalar(GetIntValByXPath(cfgRoot,"/CFG/SEGMENTER[@mode='3']/BLUE"),GetIntValByXPath(cfgRoot,"/CFG/SEGMENTER[@mode='3']/GREEN"),GetIntValByXPath(cfgRoot,"/CFG/SEGMENTER[@mode='3']/RED"));
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
	/** \todo segmenter post processing needs its own class segmenterpp */
	//Saving Post-Processing Values
	firstDilate = GetIntValByXPath(cfgRoot,"/CFG/SEGMENTERPP[@mode='0']/FIRSTDILATE");
	firstErode = GetIntValByXPath(cfgRoot,"/CFG/SEGMENTERPP[@mode='0']/FIRSTERODE");
	secondDilate = GetIntValByXPath(cfgRoot,"/CFG/SEGMENTERPP[@mode='0']/SECONDDILATE");
	usingMaskBoolean = GetIntValByXPath(cfgRoot,"/CFG/SEGMENTERPP[@mode='0']/USINGMASK");
	input->SetParameters();
}

double Segmenter::GetFPS()
{
	return(input->GetFPS());
}

void Segmenter::RefreshCoverage()
{
	trackingimg->RefreshCoverage(background);
}
