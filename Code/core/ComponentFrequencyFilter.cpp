#include "ComponentFrequencyFilter.h"
#define THISCLASS ComponentFrequencyFilter

#include <highgui.h>
#include "DisplayEditor.h"

using namespace cv;
using namespace std;

THISCLASS::ComponentFrequencyFilter(SwisTrackCore *stc):
		Component(stc, wxT("FrequencyFilter")),
		mDisplayOutput(wxT("Output"), wxT("Fourier amplitude")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryPreprocessingGray);
	AddDataStructureRead(&(mCore->mDataStructureImageFFT));
	AddDataStructureWrite(&(mCore->mDataStructureImageFFT));
	AddDisplay(&mDisplayOutput);

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentFrequencyFilter() {
}

void THISCLASS::OnStart() {

	// Load other parameters
	OnReloadConfiguration();
	return;
}

void THISCLASS::OnReloadConfiguration() {
	mConfigChanged=true;
}

void THISCLASS::OnStep() {
#ifdef GPU_ENABLED
	cv::gpu::GpuMat input=mCore->mDataStructureImageFFT.mMat_Gpu;
#else
	Mat input= mCore->mDataStructureImageFFT.mMat;
#endif

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
					f_value=1.0-f_value;
				}
				
				f_value=filter_gain*f_value;
				f_value=filter_gain*f_value;
				//if ((i==0) || (j==0)) f_value=0.0;
				
				planes[0].at<float>(i,j) = f_value;
				planes[1].at<float>(i,j) = f_value;
				
			}
		}
		
		cv::merge(planes, 2, mFilterMatrix);

		#ifdef GPU_ENABLED
		mFilterMatrixGPU.upload(mFilterMatrix);
		#endif
		mConfigChanged=false;
	}
	
	// convolute input with filter matrix (point-wise multiplication in fourier domain)
	
	#ifdef GPU_ENABLED
		cv::gpu::mulAndScaleSpectrums(input, mFilterMatrixGPU, output_gpu, DFT_COMPLEX_OUTPUT,2.0/(input.cols*input.rows), true);
		//output_gpu=input;
		//if( GetConfigurationBool(wxT("InvertFilter"), true)) {
		//	output=input-output;
		//}

		mCore->mDataStructureImageFFT.mMat_Gpu=output_gpu;
	#else
		cv::mulSpectrums(input, mFilterMatrix, output, DFT_ROWS);
		// update output data structure - note that this is using the new cv::Mat data structure from OpenCV2, as it is much easier to use. 
		if( GetConfigurationBool(wxT("InvertFilter"), true)) {
			output=input-output;
		}
		mCore->mDataStructureImageFFT.mMat=output;
	#endif



	if (mGenerateOutput) {
		#ifdef GPU_ENABLED
			cv::Mat fromGPU(output_gpu);
			output=fromGPU;
		#endif
			
		
		
		//create display output image (log scale magnitude)
		if (mDisplayOutputImage == NULL) {
		  mDisplayOutputImage = cvCreateImage(cvSize(output.cols, 
							  output.rows), 8, 1);
		  
		}
		
		cv::Mat tmp;
		generateFFTPreview(output, &tmp);
		
		//cvGetImage(output, mDisplayOutputImage);
		*mDisplayOutputImage=tmp;
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

void THISCLASS::generateFFTPreview(Mat complexI, Mat *output) {
			// compute the magnitude and switch to logarithmic scale
		// => log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))
		cv::Mat planes[2];
		
		split(complexI, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
		magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude
		cv::Mat magI = planes[0];

		magI += cv::Scalar::all(1);                    // switch to logarithmic scale
		cv::log(magI, magI);
		magI =magI*3;
		// crop the spectrum, if it has an odd number of rows or columns
		//magI = magI(cv::Rect(0, 0, magI.cols & -2, magI.rows & -2));

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
		//magI =magI*3;
										// viewable image form (float between values 0 and 1).
		magI.convertTo(*output, CV_8UC1);
}

