#include "ComponentFourierCorrelation.h"
#define THISCLASS ComponentFourierCorrelation

#include <highgui.h>
#include "DisplayEditor.h"

using namespace cv;
using namespace std;

THISCLASS::ComponentFourierCorrelation(SwisTrackCore *stc):
		Component(stc, wxT("FourierCorrelation")),
		mDisplayOutput(wxT("Output"), wxT("FilteredImage")),
		mPatternPreview(wxT("Output"), wxT("PatternPreview")){
 
	// Data structure relations
	mCategory = &(mCore->mCategoryPreprocessingGray);
	AddDataStructureRead(&(mCore->mDataStructureImageFFT));
	AddDataStructureWrite(&(mCore->mDataStructureParticles));
	AddDisplay(&mDisplayOutput);
	AddDisplay(&mPatternPreview);

	mPatternPreviewImage=NULL;
	mDisplayOutputImage=NULL;	
	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentFourierCorrelation() {
}

void THISCLASS::OnStart() {
	// Load other parameters
	OnReloadConfiguration();



	return;
}

void THISCLASS::OnReloadConfiguration() {
	// Load the image
	wxString filename_string = GetConfigurationString(wxT("Pattern1"), wxT(""));
	wxFileName filename = mCore->GetProjectFileName(filename_string);
	if (! filename.IsOk()) {
		AddError(wxT("Invalid image file path/filename."));
		return;
	}
	mPattern1 = cvLoadImage(filename.GetFullPath().mb_str(wxConvFile), CV_LOAD_IMAGE_GRAYSCALE);	mConfigChanged=true;
}

void THISCLASS::OnStep() {
	Mat input= mCore->mDataStructureImageFFT.mMat;
	if ( input.empty())
	{
	  AddError(wxT("Cannot access input image."));
	  return;
	}
	
	if (input.channels() != 2)
	{
	  AddError(wxT("Input must be a complex image (fourier domain) with 2 channels"));
	}


    cv::Mat output;
	

	// get configuration options
	mGenerateOutput = GetConfigurationBool(wxT("GenerateOutput"), true);

    // check if anything has changed (input size, filter parameters) and generate filter matrix
	if ((input.size()!=mFilterMatrix.size()) || mConfigChanged) {
		
		
		// prepare bandpass filter
		cv::Mat planes[] = {cv::Mat::ones(input.size(),CV_32F), cv::Mat::ones(input.size(), CV_32F)};
		int cx = input.cols;
		int cy = input.rows;
		float center_frequency=GetConfigurationDouble(wxT("CenterFrequency"), true)*cx/1.4;
		float filter_sigma=GetConfigurationDouble(wxT("Sigma"), true)*(cx/2.0);
		float filter_width=GetConfigurationDouble(wxT("Width"), true)*cx/2.0;
		float filter_gain=GetConfigurationDouble(wxT("Gain"), true);

		for (int j=0; j<cy; j++){
			for (int i=0; i<cx; i++){
				float dist=sqrt((float)((i+cx/2)%cx-cx/2) * (float)((i+cx/2)%cx-cx/2)  +  (float)((j+cy/2)%cy-cy/2) * (float)((j+cy/2)%cy-cy/2));
				float x=fabs(dist - center_frequency) -filter_width;
				if (x<0.0) {
					x=0.0;
				}
				//float f_value=(float)1.0/(filter_sigma*sqrt((2*3.1415))) * exp( -0.5 * pow((dist-(center_frequency))/ (filter_sigma), 2));
				float f_value= exp( -0.5 * pow( (x) / (filter_sigma), 2));
				
				if( GetConfigurationBool(wxT("InvertFilter"), true)) {
					//f_value=1.0-f_value;
				}
				
				f_value=filter_gain*f_value;
				planes[0].at<float>(i,j) = f_value;
				planes[1].at<float>(i,j) = f_value;
				
			}
		}
		
		cv::merge(planes, 2, mFilterMatrix);

		// prepare pattern image
		cv::Mat I;
		mPattern1.convertTo(I, CV_32F);
		I-=128.0;
		I/=128.0;
		//Mat I_Blurred;
		//GaussianBlur(I, I_Blurred, Size(15,15), 0,0);
		//I-=I_Blurred;
		cv::Mat padded;                            //expand input image to optimal size
		int m = cx;
		int n = cy;
	
		int shift=(int)GetConfigurationDouble(wxT("shift"), true);
		// pad pattern to same size as input
		cv::copyMakeBorder(I, padded,  n - I.rows-shift, shift, m- I.cols-shift,shift, cv::BORDER_CONSTANT, cv::Scalar::all(0));
	
		mPatterns=padded;
		
		planes[0] = cv::Mat_<float>(padded);
		planes[1] = cv::Mat::zeros(padded.size(), CV_32F);
		cv::Mat mPatternFourier1;
		cv::merge(planes, 2, mPatternFourier1);         // Add to the expanded another plane with zeros

		dft(mPatternFourier1, mPatternFourier1, 0);            // this way the result may fit in the source matrix
		mPatternFourierFiltered=mPatternFourier1;
		cv::mulSpectrums(mPatternFourier1, mFilterMatrix, mPatternFourierFiltered, DFT_ROWS);

		if( GetConfigurationBool(wxT("InvertFilter"), true)) {
			mPatternFourierFiltered=mPatternFourier1-mPatternFourierFiltered;
		}

		mConfigChanged=false;
	}
	
	// convolute input with filter matrix (point-wise multiplication in fourier domain)0.010000
	//cv::mulSpectrums(input, mFilterMatrix, output, DFT_COMPLEX_OUTPUT, false);
	
	//if( GetConfigurationBool(wxT("InvertFilter"), true)) {
	//	output=input-output;
	//}
	cv::mulSpectrums(input, mPatternFourierFiltered, output, DFT_COMPLEX_OUTPUT, true);
	
	// update output data structure - note that this is using the new cv::Mat data structure from OpenCV2, as it is much easier to use. 
	
	mCore->mDataStructureImageFFT.mMat=output;
	Mat mPatternsDisplay;
	normalize(mPatterns, mPatternsDisplay, 0, 255, CV_MINMAX);
	
	mPatternsDisplay.convertTo(mPatternsDisplay, CV_8UC1);
	
	
	if ((mPatternPreviewImage == NULL) || (mPatternPreviewImage->width!=mPatternsDisplay.cols) || (mPatternPreviewImage->height!=mPatternsDisplay.rows)){
		  mPatternPreviewImage= cvCreateImage(cvSize(mPatternsDisplay.cols, 
							  mPatternsDisplay.rows), 8, 1);
	}

	*mPatternPreviewImage=mPatternsDisplay;
	
	DisplayEditor dep(&mPatternPreview);
		if (dep.IsActive()) {
			dep.SetMainImage(mPatternPreviewImage);
		}
	if (mGenerateOutput) {
		cv::Mat planes[] = {cv::Mat::zeros(output.size(),CV_32F), cv::Mat::zeros(output.size(), CV_32F)};
		
		//create display output image (log scale magnitude)
		if (mDisplayOutputImage == NULL) {
		  mDisplayOutputImage = cvCreateImage(cvSize(output.cols, 
							  output.rows), 8, 1);
		}
		// compute the magnitude and switch to logarithmic scale
		// => log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))
		split(output, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
		//split(mFilterMatrix, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
		magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude
		cv::Mat magI = planes[0];

		magI += cv::Scalar::all(1);                    // switch to logarithmic scale
		cv::log(magI, magI);

		// crop the spectrum, if it has an odd number of rows or columns
		magI = magI(cv::Rect(0, 0, magI.cols & -2, magI.rows & -2));

		// rearrange the quadrants of Fourier image  so that the origin is at the image center
		int cx = magI.cols/2;
		int cy = magI.rows/2;

		cv::Mat q0(magI, cv::Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
		cv::Mat q1(magI, cv::Rect(cx, 0, cx, cy));  // Top-Right
		cv::Mat q2(magI, cv::Rect(0, cy, cx, cy));  // Bottom-Left
		cv::Mat q3(magI, cv::Rect(cx, cy, cx, cy)); // Bottom-Right

		cv::Mat tmp;                           // swap quadrants (Top-Left with Bottom-Right)
		q0.copyTo(tmp);
		q3.copyTo(q0);
		tmp.copyTo(q3);

		q1.copyTo(tmp);                    // swap quadrant (Top-Right with Bottom-Left)
		q2.copyTo(q1);
		tmp.copyTo(q2);

		cv::normalize(magI, magI, 0, 255, CV_MINMAX); // Transform the matrix with float values into a
												// viewable image form (float between values 0 and 1).
		cv::Mat d_output;
		magI.convertTo(d_output, CV_8UC1);
		
		//cvGetImage(output, mDisplayOutputImage);
		*mDisplayOutputImage=d_output;
		DisplayEditor de(&mDisplayOutput);
		if (de.IsActive()) {
			de.SetMainImage(mDisplayOutputImage);
		}
		
	
	}
	
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	//cvReleaseImage(&mDisplayOutputImage);
}
