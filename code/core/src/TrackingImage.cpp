// TrackingImage.cpp: implementation of the TrackingImage class.
//
//////////////////////////////////////////////////////////////////////

/** \file TrackingImage.cpp 
*	\brief Implementation of the TrackingImage class.
*/

#include "TrackingImage.h"
#include "stdio.h"

/** \class TrackingImage
* \brief Image storage and manipulation for displaying tracking progess
*
* The TrackingImage class allows for storing the current frame being
* tracked and the segmented version. Also it provides display primitives
* to the classes involved in the tracking process to allow to inform the 
* user about tracking performance.
*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TrackingImage::TrackingImage()
{
	display_vid=1;
	colorimg=NULL;
	binary=NULL;
	coverage=NULL;
	rawimage=NULL;
}

TrackingImage::~TrackingImage()
{
	if(colorimg) cvReleaseImage(&colorimg);
	if(binary) cvReleaseImage(&binary);
	if(rawimage) cvReleaseImage(&rawimage);
}

void TrackingImage::DrawRectangle(CvRect *r,CvScalar color)
	{
	if(display_vid){
		cvLine(colorimg,cvPoint(r->x,r->y),cvPoint(r->x+r->width,r->y),color,1);
		cvLine(colorimg,cvPoint(r->x+r->width,r->y),cvPoint(r->x+r->width,r->y+r->height),color,1);
		cvLine(colorimg,cvPoint(r->x+r->width,r->y+r->height),cvPoint(r->x,r->y+r->height),color,1);
		cvLine(colorimg,cvPoint(r->x,r->y+r->height),cvPoint(r->x,r->y),color,1);
		}
//	cvNamedWindow("Original",0);
//	cvShowImage("Original",GetImagePointer());
	}

/**  \brief  Turns display on and off
*
* \param display_vid if set to one, the tracking display gets updated
*/

void TrackingImage::SetDisplay(int display_vid)
{
	this->display_vid=display_vid;
}

/**  \brief Allows for quering the current state of the display
*
* \return One if the display is turned on, zero otherwise
*/
int TrackingImage::GetDisplay()
{
 return(display_vid);
}

/** \brief Returns a pointer to the current image
*
* \return A pointer to the current image
*/
IplImage* TrackingImage::GetImagePointer()
{
		return(colorimg);
}

/**  \brief Allocates memory for the input image and the binary image
*
* \param input : the current input frame
* \param binary: the current binary image
*/
void TrackingImage::Init(IplImage *input,IplImage *binary)
	{
	if(colorimg!=NULL)
		cvReleaseImage(&colorimg);
	if(binary!=NULL)
		cvReleaseImage(&(this->binary));
	if(rawimage!=NULL)
		cvReleaseImage(&rawimage);

	colorimg=(IplImage*) cvClone(input);	
	rawimage=(IplImage*) cvClone(input);

	this->binary=(IplImage*) cvClone(binary);
	if(coverage!=NULL)
		cvReleaseImage(&(this->coverage));
	coverage=(IplImage*) cvClone(input);
	}

/** \brief Copies the current image and binary to the local image
*
* \param input : the current input frame
* \param binary: the current binary image
*/
void TrackingImage::Refresh(IplImage *input,IplImage *binary)
{
if(display_vid){
		cvCopyImage(input,colorimg);
		cvCopyImage(binary,this->binary);
		cvCopyImage(input,rawimage);
	}
}

/** \brief Returns a pointer to the current binary image
*
* \return A pointer to the current binary image
*/
IplImage* TrackingImage::GetBinaryPointer()
{
	return(binary);
}

void TrackingImage::DrawCircle(CvPoint p, CvScalar color)
{
	cvCircle(colorimg,p,3,color);
}

void TrackingImage::Line(CvPoint2D32f p1, CvPoint2D32f p2,CvScalar color)
{
 cvLine(colorimg,cvPointFrom32f(p1),cvPointFrom32f(p2),color,1);
}

void TrackingImage::Text(char *smove, CvPoint pt, CvScalar color)
{
		CvFont font;
		cvInitFont( &font , CV_FONT_VECTOR0 , 0.5, 0.5, 0.0, 1);
		cvPutText(colorimg, smove, pt, &font,
		color);			
}

void TrackingImage::Cover(CvPoint p, CvScalar color, int size)
{
	cvCircle(coverage,p,1,color,size);
}

void TrackingImage::RefreshCoverage(IplImage *background)
{
	//printf("W: %d H: %d D: %d C: %d\n",background->width,background->height,coverage->depth,coverage->nChannels);
	//printf("W: %d H: %d D: %d C: %d\n",coverage->width,coverage->height,coverage->depth,coverage->nChannels);

	cvCvtPlaneToPix(background,background,background,NULL,coverage);
}

IplImage* TrackingImage::GetCoveragePointer()
{
	return(coverage);
}

void TrackingImage::DrawContours(VisBlobSet *blobs)
{
	blobs->DrawContours(colorimg,cvScalarAll(255),cvScalarAll(255));	// outline the located blobs
}

/** \brief Returns a pointer to the current image
*
* \return A pointer to the current image
*/
IplImage* TrackingImage::GetRawImagePointer()
{
		return(rawimage);
}

void TrackingImage::ClearCoverageImage()
{
	cvCopyImage(rawimage,coverage);
}
