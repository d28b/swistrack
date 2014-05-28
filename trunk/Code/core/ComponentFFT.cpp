#include "ComponentFFT.h"
#define THISCLASS ComponentFFT

#include <highgui.h>
#include "DisplayEditor.h"




using namespace cv;
using namespace std;

THISCLASS::ComponentFFT(SwisTrackCore *stc):
		Component(stc, wxT("FFT")),
		mDisplayOutput(wxT("Output"), wxT("Fourier amplitude")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryInputConversion);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureWrite(&(mCore->mDataStructureImageFFT));
	AddDisplay(&mDisplayOutput);
	
	mDisplayOutputImage=NULL;	
	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentFFT() {
}

void THISCLASS::OnStart() {

	// Load other parameters
	OnReloadConfiguration();
	return;
}

void THISCLASS::OnReloadConfiguration() {
}

void THISCLASS::OnStep() {
	IplImage *inputimage = mCore->mDataStructureImageGray.mImage;
	if (! inputimage)
	{
	  AddError(wxT("Cannot access input image."));
	  return;
	}
	
	if (inputimage->nChannels != 1)
	{
	  AddError(wxT("Input must be a greyscale (1 channel)."));
	  return;
	}

	// get configuration options
	mGenerateOutput = GetConfigurationBool(wxT("GenerateOutput"), true);


	
    cv::Mat Im=inputimage;

#ifdef GPU_ENABLED
	//cv::gpu::GpuMat complexI_Gpu;
	computeFFTGpu(Im, &complexI_Gpu);
	// update output data structure - note that this is using the new cv::Mat data structure from OpenCV2, as it is much easier to use. 
	mCore->mDataStructureImageFFT.mMat_Gpu=complexI_Gpu;
#else	
	
	computeFFT(Im, &complexI);
	// update output data structure - note that this is using the new cv::Mat data structure from OpenCV2, as it is much easier to use. 
	
	mCore->mDataStructureImageFFT.mMat=complexI;
#endif

	if (mGenerateOutput) {
#ifdef GPU_ENABLED
		Mat from_gpu(complexI_Gpu);
		complexI=from_gpu;
#endif	
		//create display output image (log scale magnitude)
		if (mDisplayOutputImage == NULL) {
		  mDisplayOutputImage = cvCreateImage(cvSize(complexI.cols, 
							  complexI.rows), 8, 1);
		  
		}


		generateFFTPreview(complexI, &output);
		
		//cvGetImage(output, mDisplayOutputImage);
		*mDisplayOutputImage=output;
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

void THISCLASS::computeFFT(Mat Im, Mat *complexI) {
	Im.convertTo(I, CV_32F);
	I=(I-128.0)/128.0;
	
	

    int m = cv::getOptimalDFTSize( I.rows );
    int n = cv::getOptimalDFTSize( I.cols ); // on the border add zero values
	
    
	cv::copyMakeBorder(I, padded, 0, max(m,n) - I.rows, 0, max(n,m) - I.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));

	
    cv::Mat planes[] = {cv::Mat_<float>(padded), cv::Mat::zeros(padded.size(), CV_32F)};
	
    cv::merge(planes, 2, *complexI);         // Add to the expanded another plane with zeros

    dft(*complexI, *complexI, CV_DXT_FORWARD, I.rows);            // this way the result may fit in the source matrix
	
}


#ifdef GPU_ENABLED
void THISCLASS::computeFFTGpu(Mat Im, cv::gpu::GpuMat *complexI) {
	Im.convertTo(I, CV_32F);
	I=(I-128.0)/128.0;
	
	

    int m = cv::getOptimalDFTSize( I.rows );
    int n = cv::getOptimalDFTSize( I.cols ); // on the border add zero values
	
    
	cv::copyMakeBorder(I, padded, 0, max(m,n) - I.rows, 0, max(n,m) - I.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));

	
    cv::Mat planes[] = {cv::Mat_<float>(padded), cv::Mat::zeros(padded.size(), CV_32F)};
	
	Mat src;
    cv::merge(planes, 2, src);         // Add to the expanded another plane with zeros

	
	src_gpu.upload(src);
	//for (int i=0; i<1000; i++) {
	cv::gpu::dft(src_gpu, *complexI, src.size());
	//}
	//cv::Mat result(dst);
	//*complexI=result;
    
}
#endif

void THISCLASS::generateFFTPreview(Mat complexI, Mat *output) {
			// compute the magnitude and switch to logarithmic scale
		// => log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))
		cv::Mat planes[2];
		
		split(complexI, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
		magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude
		cv::Mat magI = planes[0];

		magI += cv::Scalar::all(1);                    // switch to logarithmic scale
		cv::log(magI, magI);

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
												// viewable image form (float between values 0 and 1).
		magI.convertTo(*output, CV_8UC1);
}
