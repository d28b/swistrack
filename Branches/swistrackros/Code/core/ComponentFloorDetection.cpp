#include "ComponentFloorDetection.h"
#define THISCLASS ComponentFloorDetection

#include <cv.h>
#include <iostream>
#include <highgui.h>
#include <cctype>
#include <algorithm>
#include "DisplayEditor.h"
#include <queue>
using namespace std;
using namespace cv;

template<class T> class Image
{
  private:
  IplImage* imgp;
  public:
  Image(IplImage* img=0) {imgp=img;}
  ~Image(){imgp=0;}
  void operator=(IplImage* img) {imgp=img;}
  inline T* operator[](const int rowIndx) {
    return ((T *)(imgp->imageData + rowIndx*imgp->widthStep));}
};

typedef struct{
  unsigned char h,s,v;
} HsvPixel;

typedef struct{
  float h,s,v;
} HsvPixelFloat;

typedef Image<HsvPixel>       HsvImage;
typedef Image<HsvPixelFloat>  HsvImageFloat;
typedef Image<unsigned char>  BwImage;
typedef Image<float>          BwImageFloat;

//queue<CvHistogram*> hueHistograms;
//queue<CvHistogram*> intHistograms;
int counter = 0;
int const HIST_BUFFER = 10;
IplImage* huePlanes [HIST_BUFFER];
IplImage* intPlanes [HIST_BUFFER];
IplImage* hueMask [HIST_BUFFER];



THISCLASS::ComponentFloorDetection(SwisTrackCore *stc):
  Component(stc, wxT("FloorDetection")),
  mDisplayOutput(wxT("Output"), wxT("After floor detection")) {

  // Data structure relations
  mCategory = &(mCore->mCategoryPreprocessingColor);
  AddDataStructureRead(&(mCore->mDataStructureImageColor));
  AddDataStructureWrite(&(mCore->mDataStructureImageGray));
  AddDisplay(&mDisplayOutput);
  
  // Read the XML configuration file
  Initialize();
}

THISCLASS::~ComponentFloorDetection() {
}

void THISCLASS::OnStart() {
	OnReloadConfiguration();
	InitializeData();
	return;
}

void THISCLASS::OnReloadConfiguration() {
  // Hue Threshold
  mHueT = GetConfigurationInt(wxT("Hue Threshold"), 10);
  if (mHueT < 1) {
    mHueT = 1;
  }

  // Intinsity Threshold
  mIntT = GetConfigurationInt(wxT("Intinsity Threshold"), 10);
  if (mIntT < 1) {
    mIntT = 1;
  }

  // Trapezoid lower bound
  lowerBound = GetConfigurationInt(wxT("Lower Bound of Trapezoid"), 100);
  if (lowerBound < 0) {
    lowerBound = 0;
  }

  // Show Sampling Area
  showT = GetConfigurationBool(wxT("Show Sampling Area"), true);

  cout << "Updated Configuration: Hue Thresh " << mHueT << " Int Thresh " << mIntT << endl; 

}

void THISCLASS::InitializeData() {
  int h_bins = 181, s_bins = 256, v_bins = 256, i_bins = 256;
  float h_ranges[] = { 0, h_bins };
  float i_ranges[] = { 0, i_bins };
  float* hue_ranges[] = {h_ranges};
  float* int_ranges[] = {i_ranges};

  combinedHueHist = cvCreateHist( 1, &h_bins, CV_HIST_ARRAY, hue_ranges, 1);
  combinedIntHist = cvCreateHist( 1, &i_bins, CV_HIST_ARRAY, int_ranges, 1);


}

void THISCLASS::OnStep() {
	int h_bins = 181, s_bins = 256, v_bins = 256, i_bins = 256;
	IplImage *inputimage = mCore->mDataStructureImageColor.mImage;
	IplImage *outputimage = cvCreateImage(cvGetSize(inputimage),IPL_DEPTH_8U, 1);
	//cvCopy(inputimage, outputimage,NULL);
 
	if (! inputimage) {
		AddError(wxT("No input image."));
		return;
	}
	if (inputimage->nChannels < 3) {
		AddError(wxT("The input image is not a color image."));
		return;
	}

	int left_bound = 100;
	int right_bound = 100;
	int top_bound = 240;


	// Sample RBG image
	cvSetImageROI(inputimage, cvRect(left_bound, top_bound, 
					 inputimage->width-left_bound-right_bound,
					 inputimage->height-lowerBound-top_bound));
	IplImage* sampleI = cvCreateImage(cvGetSize(inputimage), IPL_DEPTH_8U, 3);
	cvCopy(inputimage, sampleI, NULL);
	cvResetImageROI(inputimage);
	IplImage* i_plane = cvCreateImage( cvGetSize( sampleI ), IPL_DEPTH_8U, 1 );

	int in_step = sampleI->widthStep/sizeof(uchar);
	int out_step = i_plane->widthStep/sizeof(uchar);
	int in_channels  = sampleI->nChannels;
	int out_channels  = i_plane->nChannels;
	uchar* indata = (uchar *)sampleI->imageData;
	uchar* outdata = (uchar *)i_plane->imageData;

	for(int i = 0; i < i_plane->height; i++){
	  for(int j = 0; j < i_plane->width; j++){
	    int r = indata[i*in_step+j*in_channels]; 
	    int g = indata[i*in_step+j*in_channels+1]; 
	    int b = indata[i*in_step+j*in_channels+2]; 
	    outdata[i*out_step+j*out_channels] = (r+g+b)/3;
	  }
	}

	// CONVERT IMAGE TO HSV
	IplImage* hsv = cvCreateImage( cvGetSize(inputimage), IPL_DEPTH_8U, 3 );
	cvCvtColor( inputimage, hsv, CV_BGR2HSV );
	int height = hsv->height;
	int width = hsv->width;

	// Sample HSV image
	cvSetImageROI(hsv, cvRect(left_bound, top_bound, 
				  hsv->width-left_bound-right_bound,
				  hsv->height-lowerBound-top_bound));
	IplImage* sample = cvCreateImage(cvGetSize(hsv), IPL_DEPTH_8U, 3);
	cvCopy(hsv, sample, NULL);
	cvResetImageROI(hsv);

	IplImage* h_plane = cvCreateImage( cvGetSize( sample ), IPL_DEPTH_8U, 1 );
	IplImage* s_plane = cvCreateImage( cvGetSize( sample ), IPL_DEPTH_8U, 1 );
	IplImage* v_plane = cvCreateImage( cvGetSize( sample ), IPL_DEPTH_8U, 1 );
	cvCvtPixToPlane( sample, h_plane, s_plane, v_plane, 0 );

	// Threshold values of saturation and use as a mask for hue plane
	IplImage* hue_mask = cvCreateImage( cvGetSize( sample ), IPL_DEPTH_8U, 1 );
	cvThreshold( s_plane, hue_mask, 40, 1, CV_THRESH_TOZERO);


	IplImage* histImage_a = cvCreateImage( cvSize(h_bins,255), IPL_DEPTH_8U, 3 );
	IplImage* histImage_b = cvCreateImage( cvSize(i_bins,255), IPL_DEPTH_8U, 3 );
	float minVal_a, maxVal_a;
	int min_a, max_a;
	float minVal_b, maxVal_b;
	int min_b, max_b;
	
	if(counter < HIST_BUFFER) {
	  huePlanes[counter]=h_plane;	  
	  intPlanes[counter]=i_plane;	  
	  hueMask[counter]=hue_mask;
	  counter++;
	  cvGetMinMaxHistValue(combinedHueHist, &minVal_a, &maxVal_a, &min_a, &max_a);
	  cvGetMinMaxHistValue(combinedIntHist, &minVal_b, &maxVal_b, &min_b, &max_b);
	}
	else {
	  counter = 0;
	  huePlanes[0]=h_plane;
	  hueMask[0]=hue_mask;
	  intPlanes[0]=i_plane;
	  cvClearHist(combinedHueHist);
	  cvClearHist(combinedIntHist);
	  
	  // Sum over all images in buffer
	  for(int i=0; i < HIST_BUFFER; i++) {
	    cvCalcHist(&huePlanes[i], combinedHueHist, 1, hueMask[i]);
	    cvCalcHist(&intPlanes[i], combinedIntHist, 1, 0);
	  }
	  // Threshold
	  //cvThreshHist(combinedHueHist, mHueT);
	  cvThreshHist(combinedIntHist, mIntT);
	  // Normalize
	  cvNormalizeHist(combinedHueHist, 255);
	  cvNormalizeHist(combinedIntHist, 255);

	  // Draw the accumulated hue histogram
	  cvGetMinMaxHistValue(combinedHueHist, &minVal_a, &maxVal_a, &min_a, &max_a);
	  //	  cout << "minval_h " << minVal_a << " maxval_h " << maxVal_a << " min h " << min_a << " maxh " << max_a << endl;
	  
	  
	  for(int h=0;h<h_bins;h++){
	    // draw the histogram data onth the histogram image
	    float* binVal = cvGetHistValue_1D(combinedHueHist,h);
	    int scaledVal;
	    if(binVal[0] == 0)
	      scaledVal = 0;
	    else
	      scaledVal = cvRound(binVal[0]*255/maxVal_a);
	    
	    //	    cout << "binVal " << *binVal << " scaled " << scaledVal << " height " << histImage_a->height << endl;
	    cvRectangle(histImage_a,cvPoint(h,histImage_a->height),
			cvPoint((h+1),histImage_a->height - scaledVal),
			CV_RGB(255,0,0), CV_FILLED);
	  }

	  // Draw the accumulated int histogram
	  cvGetMinMaxHistValue(combinedIntHist, &minVal_b, &maxVal_b, &min_b, &max_b);
	  //	  cout << "minval_b " << minVal_b << " maxval_b " << maxVal_b << " min B " << min_b << " maxb " << max_b << endl;
	  
	  
	  for(int h=0;h<i_bins;h++){
	    // draw the histogram data onth the histogram image
	    float* binVal = cvGetHistValue_1D(combinedIntHist,h);
	    int scaledVal = cvRound(binVal[0]*255/maxVal_b);
	    
	    cvRectangle(histImage_b,cvPoint(h,histImage_b->height),
			cvPoint((h+1),histImage_b->height - scaledVal),
			CV_RGB(255,255,255), CV_FILLED);
	  }
	
	  namedWindow("HueCombo",1);
	  imshow("HueCombo", histImage_a);
	  cvWaitKey(0);

	  namedWindow("IntCombo",1);
	  imshow("IntCombo", histImage_b);
	  cvWaitKey(0);
	}
	


	
	//cout << "ihist has " << intHistograms.size() << ", hhist has " << hueHistograms.size() << endl;


	//*** DRAW THE HISTOGRAMS

	/*
	cvReleaseImage(&histImage_h);
	cvReleaseImage(&histImage_s);
	cvReleaseImage(&histImage_v);
	*/


	// COMPARE WITH WHOLE IMAGE
	// loop through rgb and hsv pixels
	in_step = hsv->widthStep/sizeof(uchar);
	int in_step2 = inputimage->widthStep/sizeof(uchar);
	out_step = outputimage->widthStep/sizeof(uchar);
	in_channels = hsv->nChannels;
	int in_channels2  = inputimage->nChannels;
	out_channels  = outputimage->nChannels;
	indata = (uchar *)hsv->imageData;
	uchar* indata2 = (uchar *)inputimage->imageData;
	outdata = (uchar *)outputimage->imageData;

	for(int i=0; i<height; i++){
	  for(int j=0; j<width; j++){ 
	    // look up the values for this pixel for the current image
	    int hue = indata[i*in_step+j*in_channels];
	    int intensity = indata2[i*in_step2+j*in_channels2];
	    // look up thier counts in the accumulated histograms
	    double count_h = cvQueryHistValue_1D(combinedHueHist,hue);
	    double count_i = cvQueryHistValue_1D(combinedIntHist,intensity);

	    
	    // Output probability density as grayscale image
	    int scaledh = cvRound(count_h*255/maxVal_a);
	    int scaledi = cvRound(count_i*255/maxVal_b);
	    int scaledvalue = cvRound((scaledh+scaledi)/2);
	    outdata[i*out_step+j*out_channels] = scaledvalue;
	    outdata[i*out_step+j*out_channels+1] = 0;
	    outdata[i*out_step+j*out_channels+2] = 0;
	    
	  }
	}


	// DRAW SAMPLING REGION
	int y1 = lowerBound;
	int y2 = top_bound;
	int x1 = left_bound;
	int x2 = right_bound;
	int x3 = right_bound;
	int x4 = left_bound;
 
	CvPoint pt1 = cvPoint(x1,y1);
	CvPoint pt2 = cvPoint(x2,y1);
	CvPoint pt3 = cvPoint(x3,y2);
	CvPoint pt4 = cvPoint(x4,y2);

	if(showT){
	  //cout << x2-org_w << "," << y2-org_h << "," << m2 << "," << endl;
	  //cout << m2*org_w << "," << y3-org_h << "," <<(y3-org_h)+org_w 
	  //<< "," << endl;
	  //cvPolyLine();
	  cvRectangle(outputimage,
		      cvPoint(left_bound, top_bound),
		      cvPoint(left_bound + inputimage->width-left_bound-right_bound,
			      top_bound + inputimage->height-lowerBound-top_bound),
		      CV_RGB(255, 0, 0), 1, 0, 0 );
	}
	
	
	// SET DISPLAY

	mCore->mDataStructureImageColor.mImage = outputimage;
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(mCore->mDataStructureImageColor.mImage);
	}

	/*
	mCore->mDataStructureImageGray.mImage = hsv;
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(mCore->mDataStructureImageGray.mImage);
	}
	*/
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {

}
