#ifndef HEADER_ComponentFFT
#define HEADER_ComponentFFT

#include <cv.h>
#include "Component.h"
using namespace cv;

//#define GPU_ENABLED   // moved to Makefile

#ifdef GPU_ENABLED
#include "opencv2/opencv.hpp"
#include "opencv2/gpu/gpu.hpp"
#endif

//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentFFT: public Component {

public:
	//! Constructor.
	ComponentFFT(SwisTrackCore *stc);
	//! Destructor.
	~ComponentFFT();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentFFT(mCore);
	}

	void computeFFT(Mat Im, Mat *complexI);
	void generateFFTPreview(Mat complexI, Mat *output);
	#ifdef GPU_ENABLED
	void computeFFTGpu(Mat Im, cv::gpu::GpuMat *complexI);
	#endif
private:

	Display mDisplayOutput;		//!< The DisplayImage showing the output of this component.
	IplImage *mDisplayOutputImage;
	bool mGenerateOutput; 		// configuration parameter specifies if an magnitude image will be calculated for display (takes extra time)
	
	cv::Mat padded;
	Mat src;
	cv::Mat complexI;
	Mat output;
	cv::Mat I;
	#ifdef GPU_ENABLED
	cv::gpu::GpuMat complexI_Gpu;
	cv::gpu::GpuMat src_gpu;
    #endif
	
};

#endif

