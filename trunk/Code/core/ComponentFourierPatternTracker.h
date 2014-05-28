#ifndef HEADER_ComponentFourierPatternTracker
#define HEADER_ComponentFourierPatternTracker

#include <cv.h>
#include "Component.h"
using namespace cv;

//#define GPU_ENABLED

#ifdef GPU_ENABLED
#include "opencv2/opencv.hpp"
#include "opencv2/gpu/gpu.hpp"
#endif

//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentFourierPatternTracker: public Component {

public:
	//! Constructor.
	ComponentFourierPatternTracker(SwisTrackCore *stc);
	//! Destructor.
	~ComponentFourierPatternTracker();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentFourierPatternTracker(mCore);
	}


	void prepareFilter(Mat input);
	void generateFFTPreview(Mat complexI, Mat *output);

private:

	

	Display mDisplayOutput;		//!< The DisplayImage showing the output of this component.
	
	Display mPatternPreview;		//!< The DisplayImage showing the output of this component.
	Display mMatchPreview;
	IplImage *mDisplayOutputImage;
	IplImage *mPatternPreviewImage;
	IplImage *mMatchPreviewImage;

	DataStructureParticles::tParticleVector mParticles, mParticlesOutput;
	
	cv::Mat mFilterMatrix;
	cv::Mat mPattern1, mPatternFourierFiltered1, mPattern2, mPatternFourierFiltered2;

#ifdef GPU_ENABLED
	cv::gpu::GpuMat mPatternFourierFiltered1_GPU, mPatternFourierFiltered2_GPU;
	cv::gpu::GpuMat planes_gpu[2];
	cv::gpu::GpuMat matchResult_gpu;
	cv::gpu::GpuMat output_gpu;
#endif

	cv::Mat mPatterns;
	cv::Mat complexI;
	cv::Mat matchResult, output, tracker, temp;
	cv::Mat displayoutput;
	bool mGenerateOutput; 		// configuration parameter specifies if an magnitude image will be calculated for display (takes extra time)
	bool mConfigChanged;
	int matchPattern;
	cv::Mat planes[2];
	float particle_size, sensing_prediction_balance, rejection_threshold;
	int particle_lifetime;
	inline float sqr(float in); 
	
};

#endif

