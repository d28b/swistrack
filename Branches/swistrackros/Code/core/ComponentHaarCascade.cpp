#include "ComponentHaarCascade.h"
#define THISCLASS ComponentHaarCascade

#include <cv.h>
#include <iostream>
#include <highgui.h>
#include <cctype>
#include <algorithm>
#include "DisplayEditor.h"
#include <queue>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>
using namespace std;
using namespace cv;

THISCLASS::ComponentHaarCascade(SwisTrackCore *stc):
  Component(stc, wxT("HaarCascade")),
  mDisplayOutput(wxT("Output"), wxT("After Haar Cascade")),
  storage(0),
  cascade(0)
{
  
  // Data structure relations
  mCategory = &(mCore->mCategoryPreprocessingColor);
  AddDataStructureRead(&(mCore->mDataStructureImageColor));
  AddDataStructureWrite(&(mCore->mDataStructureImageColor));
  AddDisplay(&mDisplayOutput);
  
  // Initialize Haar classifier variables
  //cascade_name = "haarcascade_block.xml";
  optlen = strlen("--cascade=");
  
  
  // Read the XML configuration file
  Initialize();
}

THISCLASS::~ComponentHaarCascade() {
}

void THISCLASS::OnStart() {
  OnReloadConfiguration();
  
  // Get the trained classifier xml file
  wxString filename_string = GetConfigurationString(wxT("XMLFile"), wxT(""));
  wxFileName filename = mCore->GetProjectFileName(filename_string);
  if (! filename.IsOk()) {
    AddError(wxT("Invalid xml file path/filename."));
    return;
  }
  cascade_name = filename;
  
  // Get a color template for the block: simply a cropped image of a block
  wxString template_string = GetConfigurationString(wxT("TemplateFile"), wxT(""));
  wxFileName template_name = mCore->GetProjectFileName(template_string);
  if (! template_name.IsOk()) {
    AddError(wxT("Invalid image file path/filename."));
    return;
  }
  template_name = filename;

  /*
  templateImage = cvLoadImage(template_name.GetFullPath().mb_str(wxConvFile), 
			     CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
  if (! templateImage) {
    AddError(wxT("Cannot open image file."));
    return;
  }
  */
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {

  IplImage *inputimage = mCore->mDataStructureImageColor.mImage;
  IplImage *outputimage = cvCreateImage(cvGetSize(inputimage),IPL_DEPTH_8U, 3);
  if (! inputimage) {
    AddError(wxT("No input image."));
    return;
  }
  if (inputimage->nChannels < 3) {
    AddError(wxT("The input image is not a color image."));
    return;
  }
  cvCopy(inputimage, outputimage,NULL);
  
  
  // Load the HaarClassifierCascade
  cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name.GetFullPath().mb_str(wxConvFile), 0, 0, 0 );
	
  // Check whether the cascade has loaded successfully. Else report and error and quit
  if( !cascade )
    {
      fprintf( stderr, "ERROR: Could not load classifier cascade\n" );
      return;
    }
  
  // Allocate the memory storage
  storage = cvCreateMemStorage(0);
  
  // Create a new named window with title: result
  cvNamedWindow( "result", 1 );
  
  // Call the function to detect and draw the face
  detect_and_draw( outputimage );
  
  // Destroy the window previously created with filename: "result"
  cvDestroyWindow("result");

  // SET DISPLAY
  
  mCore->mDataStructureImageColor.mImage = outputimage;
  DisplayEditor de(&mDisplayOutput);
  if (de.IsActive()) {
    de.SetMainImage(mCore->mDataStructureImageColor.mImage);
  }

  /* free memory */
}


// Function to detect and draw any faces that is present in an image
void THISCLASS::detect_and_draw( IplImage* img )
{
  int scale = 1;

  // Create a new image based on the input image
  IplImage* temp = cvCreateImage( cvSize(img->width/scale,img->height/scale), 8, 3 );
  
  // Create two points to represent the face locations
  CvPoint pt1, pt2;
  int i;
  
  // Clear the memory storage which was used before
  cvClearMemStorage( storage );
  
  // Find whether the cascade is loaded, to find the faces. If yes, then:
  if( cascade )
    {
      // There can be more than one face in an image. So create a 
      // growable sequence of faces.
      // These parameters are suited for accurate but slow detection
      /*
	CvSeq* faces = cvHaarDetectObjects( img, cascade, storage,
					  1.1, 3, CV_HAAR_DO_CANNY_PRUNING,
					  cvSize(40, 40) );
      */

      // Parameters for faster detection
      CvSeq* faces = cvHaarDetectObjects( img, cascade, storage,
					  1.2, 2, CV_HAAR_DO_CANNY_PRUNING,
					  cvSize(40, 40) );
      
      CvSeq* rankedFaces;
      // Loop the number of faces found.
      for( i = 0; i < (faces ? faces->total : 0); i++ )
        {
	  // Create a new rectangle for drawing the face
	  CvRect* r = (CvRect*)cvGetSeqElem( faces, i );
	  
	  // Find the dimensions of the face,and scale it if necessary
	  pt1.x = r->x*scale;
	  pt2.x = (r->x+r->width)*scale;
	  pt1.y = r->y*scale;
	  pt2.y = (r->y+r->height)*scale;
	  
	  // Evaluate Color in rectangle 
	  cvSetImageROI(img, *r);
	  IplImage* regionOfInterest = 
	    cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 3);
	  cvCopy(img, regionOfInterest, NULL);
	  cvResetImageROI(img);

	  int colorRanking = RankFace(regionOfInterest);
	  int haarRanking = 100 - (10*(i));
	  double haarWeight = .5;
	  double colorWeight = .5;
	  double ranking = haarWeight * haarRanking + colorWeight * colorRanking;
	  // Draw the rectangle in the input image
	  if(ranking > .5)
	    cvRectangle( img, pt1, pt2, CV_RGB(127,255,0), 11, 4, 0 );
	  else
	    cvRectangle( img, pt1, pt2, CV_RGB(255,255,0), 11-i, 4, 0 );
        }
    }
  
  // Show the image in the window named "result"
  //cvShowImage( "result", img );
  // SET DISPLAY
  //mCore->mDataStructureImageColor.mImage = img;
  //DisplayEditor de(&mDisplayOutput);
  //if (de.IsActive()) {
  //  de.SetMainImage(mCore->mDataStructureImageColor.mImage);
  //}
  
  
  // Release the temp image created.
  cvReleaseImage( &temp );
}

float THISCLASS::RankFace(IplImage *face) {
  int bins = 256;
  float ranges[] = { 0, bins };
  float* c_ranges[] = {ranges};
  
  IplImage* Tr_plane = cvCreateImage( cvGetSize(face), IPL_DEPTH_8U, 1 );
  IplImage* Tg_plane = cvCreateImage( cvGetSize(face), IPL_DEPTH_8U, 1 );
  IplImage* Tb_plane = cvCreateImage( cvGetSize(face), IPL_DEPTH_8U, 1 );
  cvCvtPixToPlane( face, Tr_plane, Tg_plane, Tb_plane, 0 );

  CvHistogram* TrHist = cvCreateHist( 1, &bins, CV_HIST_ARRAY, c_ranges, 1);
  CvHistogram* TgHist = cvCreateHist( 1, &bins, CV_HIST_ARRAY, c_ranges, 1);
  CvHistogram* TbHist = cvCreateHist( 1, &bins, CV_HIST_ARRAY, c_ranges, 1);

  cvCalcHist(&Tr_plane, TrHist, 0, 0);
  cvCalcHist(&Tg_plane, TgHist, 0, 0);
  cvCalcHist(&Tb_plane, TbHist, 0, 0);

  int Tr_sum = 0;
  for(int i = 225; i < bins; i++){
    float* binVal = cvGetHistValue_1D(TrHist,i);
    Tr_sum += (int)*binVal;
  }
  int Tg_sum = 0;
  for(int i = 225; i < bins; i++){
    float* binVal = cvGetHistValue_1D(TgHist,i);
    Tg_sum += (int)*binVal;
  }
  int Tb_sum = 0;
  for(int i = 225; i < bins; i++){
    float* binVal = cvGetHistValue_1D(TbHist,i);
    Tb_sum += (int)*binVal;
  }
  float total = Tr_sum + Tb_sum + Tg_sum;
  if(total == 0)
    total = 1;

  return (float)(Tg_sum/total)*100;
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {

}
