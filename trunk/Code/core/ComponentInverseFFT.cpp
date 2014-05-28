#include "ComponentInverseFFT.h"
#define THISCLASS ComponentInverseFFT

#include <highgui.h>
#include "DisplayEditor.h"

using namespace cv;
using namespace std;

THISCLASS::ComponentInverseFFT(SwisTrackCore *stc):
		Component(stc, wxT("InverseFFT")),
		mDisplayOutput(wxT("Output"), wxT("Grayscale image")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryInputConversion);
	AddDataStructureRead(&(mCore->mDataStructureImageFFT));
	AddDataStructureWrite(&(mCore->mDataStructureImageGray));
	AddDisplay(&mDisplayOutput);
	mDisplayOutputImage=NULL;	
	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentInverseFFT() {
}

void THISCLASS::OnStart() {

	// Load other parameters
	OnReloadConfiguration();
	return;
}

void THISCLASS::OnReloadConfiguration() {
}

Mat tmp1, tmp2;

void THISCLASS::OnStep() {
#ifdef GPU_ENABLED
	cv::gpu::GpuMat input=mCore->mDataStructureImageFFT.mMat_Gpu;
//	Mat input(mCore->mDataStructureImageFFT.mMat_Gpu);
#else
	Mat input= mCore->mDataStructureImageFFT.mMat;
#endif

	IplImage *originalImage = mCore->mDataStructureImageGray.mImage;
	
	if (input.empty())
	{
	  AddError(wxT("Cannot access input image."));
	  return;
	}
	
	if (input.channels() != 2)
	{
	  AddError(wxT("Input must be a complex image (fourier domain) with 2 channels"));
	}
	


	
#ifdef GPU_ENABLED
	cv::gpu::GpuMat output_gpu;
    cv::gpu::dft(input, output_gpu, input.size(), DFT_INVERSE);            
	Mat output(output_gpu);
#else
	Mat output;
    dft(input, output, DFT_INVERSE+DFT_SCALE );
#endif	

// add bias of 127 that was subtracted before
	split(output, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))

	//cv::Rect roi(Point( 2,4), Size( originalImage->width-40, originalImage->height-10));
    //cv::Mat out = planes[0](roi);
	
	

	switch (GetConfigurationInt(wxT("Output type"), true)) {
		case 0: //Real
			out = planes[0];
			out=128*out+128;
		break;
		case 1: //Im.
			out = planes[1];
			out=128*out+128;
		break;
		case 2: // Magnitude
			magnitude(planes[0], planes[1], out);
			out=255*out;
		break;
		case 3: //Phase
			multiply(planes[0], planes[1], out);
			out=128*out+128;
		break;
		case 4: //squared magnitude
			multiply(planes[0], planes[0], tmp1);
			multiply(planes[1], planes[1], tmp2);
			add(tmp1, tmp2, out);
			out=255*out;
		break;
	}
	
	//pow(out, 3, out);
	
	if (GetConfigurationBool(wxT("Normalize"), true)) {
		cv::normalize(out, out, 0,255, CV_MINMAX);
	}
    

	//create display output image
	if (mDisplayOutputImage == NULL) {
	  mDisplayOutputImage = cvCreateImage(cvSize(output.cols, 
					      output.rows), 8, 1);
	  
	}

	cv::Mat displayoutput;
	
	out.convertTo(displayoutput, CV_8UC1);

	*mDisplayOutputImage=displayoutput;
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(mDisplayOutputImage);
	}

	mCore->mDataStructureImageGray.mImage=mDisplayOutputImage;

}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	//cvReleaseImage(&mDisplayOutputImage);
}
