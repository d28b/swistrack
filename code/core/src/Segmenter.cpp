// Segmenter.cpp: implementation of the Segmenter class.
//
//////////////////////////////////////////////////////////////////////

#include "Segmenter.h"

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
			if(!IsDefined(cfgRoot,"/CFG/SEGMENTER[@mode='0']"))
				throw "[Segmenter::Segmenter] no parameters for segmenter mode 0 (background subtraction) found";
			if(!IsDefined(cfgRoot,"/CFG/SEGMENTER[@mode='0']/BGIMAGE"))
				throw "[Segmenter::Segmenter] background image not specified (/CFG/SEGMENTER[@mode='0']/BGIMAGE)";
			if(!IsDefined(cfgRoot,"/CFG/SEGMENTER[@mode='0']/THRESHOLD"))
				throw "[Segmenter::Segmenter] threshold not specified (/CFG/SEGMENTER[@mode='0']/THRESHOLD)";
			if(!IsDefined(cfgRoot,"/CFG/SEGMENTER[@mode='0']/MAXNUMBER"))
				throw "[Segmenter::Segmenter] maximal number of contours not specified (/CFG/SEGMENTER[@mode='0']/MAXNUMBER)";


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
			break;
			cvReleaseImage(&bg);
		}
		/////////// Estimate Subtraction //////////////////
	case 1 :
		{
			if(!IsDefined(cfgRoot,"/CFG/SEGMENTER[@mode='1']"))
				throw "[Segmenter::Segmenter] no parameters for segmenter mode 1 (estimate subtraction) found";
			if(!IsDefined(cfgRoot,"/CFG/SEGMENTER[@mode='1']/THRESHOLD"))
				throw "[Segmenter::Segmenter] threshold not specified (/CFG/SEGMENTER[@mode='1']/THRESHOLD)";
			if(!IsDefined(cfgRoot,"/CFG/SEGMENTER[@mode='1']/ALPHA"))
				throw "[Segmenter::Segmenter] alpha not specified (/CFG/SEGMENTER[@mode='1']/ALPHA)";
			if(!IsDefined(cfgRoot,"/CFG/SEGMENTER[@mode='1']/MAXNUMBER"))
				throw "[Segmenter::Segmenter] maximal number of contours not specified (/CFG/SEGMENTER[@mode='1']/MAXNUMBER)";

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
			if(!IsDefined(cfgRoot,"/CFG/SEGMENTER[@mode='2']"))
				throw "[Segmenter::Segmenter] no parameters for segmenter mode 2(background subtraction) found";
			if(!IsDefined(cfgRoot,"/CFG/SEGMENTER[@mode='2']/BGIMAGE"))
				throw "[Segmenter::Segmenter] background image not specified (/CFG/SEGMENTER[@mode='2']/BGIMAGE)";
			if(!IsDefined(cfgRoot,"/CFG/SEGMENTER[@mode='2']/THRESHOLD"))
				throw "[Segmenter::Segmenter] threshold not specified (/CFG/SEGMENTER[@mode='2']/THRESHOLD)";
			if(!IsDefined(cfgRoot,"/CFG/SEGMENTER[@mode='2']/MAXNUMBER"))
				throw "[Segmenter::Segmenter] maximal number of contours not specified (/CFG/SEGMENTER[@mode='2']/MAXNUMBER)";


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
				cvSplit(tmpImage,Red,Green,Blue,NULL);				
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
		int numContours = GetContour(binary,&contours);    
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
						cvSplit(tmpImage,Red,Green,Blue,NULL);
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
				throw "[Segmenter::Segmentation] with fixed background failed";
			}
			cvReleaseImage(&tmpImage);
			break;
		}
	default : {
		throw "Invalid segmentation mode";
			  }
			  break;
	}

	// Perform a morphological opening to reduce noise. 

	cvErode(binary, binary, NULL, 2);
	cvDilate(binary, binary, NULL, 2);

}

/**  \brief Find Contours in the image (called by GetContour)
*
* \param src: Segmented image
* \param contour_obj: Vector to store resulting contours
* \param min_area: Minimum area for the contours not to be rejected
*/
int Segmenter::FindContours(IplImage* src, vector<resultContour>* contour_obj)
{
	CvMemStorage* storage = cvCreateMemStorage(0);		// memory space that will be used for contour storage			
	CvSeq* contour = 0;
	std::vector<CvSeq*> tmp;
	std::vector<CvSeq*>::iterator j;
	//	int i;
	int n_contours = 0; /* Number of contours found */

	//    CvPoint2D32f center = cvPoint2D32f(src->width/2,src->height/2);

	// Make a copy of source, because contents are destroyed by cvStartFindContours
	IplImage* src_tmp = cvCloneImage(src);
	CvContourScanner blobs = cvStartFindContours(src_tmp,storage,sizeof(CvContour),CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);

	//
	while((contour=cvFindNextContour(blobs))!=NULL)
	{	// inserts contours into temporary vector, sorted by size, biggest in front
		for(j=tmp.begin(); (j != tmp.end()) && (fabs(cvContourArea(contour)) < fabs(cvContourArea(*j))); j++); 
		tmp.insert(j,contour);
		n_contours++;		
	}
	CvPoint2D32f pt_centre;
	CvRect rect;

	// This is used to correct the position in case of ROI
	if(src->roi != NULL)
		rect = cvGetImageROI(src);
	else
	{
		rect.x = 0;
		rect.y = 0;
	}

	CvMoments moments;

	// Compute and store the moments and other data of each contour, but take only the 'max_number' first contours
	for(int i=0;i<n_contours && i<max_number;i++)
	{
		resultContour rc;
		cvMoments(tmp[i],&moments,0);
		pt_centre.x=(float)(rect.x + (moments.m10/moments.m00+0.5));  // moments using Green theorema
		pt_centre.y=(float)(rect.y + (moments.m01/moments.m00+0.5));  // m10 = x direction, m01 = y direction, m00 = area as edicted in theorem
		rc.center = pt_centre;
		rc.area = moments.m00;
		rc.compactness =  cvContourCompactness( tmp[i]);
		rc.boundingrect = cvContourBoundingRect(tmp[i], 0);

		contour_obj->push_back(rc);

	}


	contour=cvEndFindContours(&blobs);
	cvReleaseMemStorage(&storage);
	cvReleaseImage(&src_tmp);
	return n_contours;

}

/**  \brief Select contours in a picture
*
* This function computes all exterior contours in the picture and find the center of 
* gravity of the ones whose areas are within the defined bounds and whose compactness is
* higher than target one. It writes the retained contours and their features in a 
* user-defined array 'contour_obj'.
*
* \param src: pointer on the binary source picture
* \param contour_obj: pointer to a vector of resultContour where results are returned
* \result Returns the number of detected contours.
*/
int Segmenter::GetContour(IplImage* src, vector<resultContour>* contour_obj)
{
	int n_contours;
	//printf("GetContour\n");
	// Erase all contours in resultContour vector
	contour_obj->clear();	
	n_contours=FindContours(src,contour_obj);
	GetContourColor(input->GetInputIpl(),contour_obj);
	return n_contours;
}

/** \brief Determines contours average color 
*
* This function computes the average color value over all pixels in a contour. 
* This might be a useful parameter in determining the fitness of the data
* association algorithm.
*
* \param input: The color input image
* \param contour_obj: pointer to a vector of resultContour 
*
* \todo Implementation
*/
void Segmenter::GetContourColor(IplImage* input, vector<resultContour>* contour_obj)
{
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

vector<resultContour>* Segmenter::GetContours()
{
	return(&contours);
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
			max_number = GetIntValByXPath(cfgRoot,"/CFG/SEGMENTER[@mode='0']/MAXNUMBER");
		}
		break;
	case 1 : 
		{ /////////// ESTIMATE SUBTRACTION //////////////////
			bin_threshold = GetIntValByXPath(cfgRoot,"/CFG/SEGMENTER[@mode='1']/THRESHOLD");
			alpha = GetDoubleValByXPath(cfgRoot,"/CFG/SEGMENTER[@mode='1']/ALPHA");
			max_number = GetIntValByXPath(cfgRoot,"/CFG/SEGMENTER[@mode='1']/MAXNUMBER");
		}
		break;
	case 2 : 
		{ /////////// BACKGROUND SUBTRACTION ////////////////
			bin_threshold = GetIntValByXPath(cfgRoot,"/CFG/SEGMENTER[@mode='2']/THRESHOLD");
			max_number = GetIntValByXPath(cfgRoot,"/CFG/SEGMENTER[@mode='2']/MAXNUMBER");
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
