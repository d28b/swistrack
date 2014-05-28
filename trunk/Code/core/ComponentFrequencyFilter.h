#ifndef HEADER_ComponentFrequencyFilter
#define HEADER_ComponentFrequencyFilter

#include <cv.h>
#include "Component.h"
using namespace cv;

//#define GPU_ENABLED

#ifdef GPU_ENABLED
#include "opencv2/opencv.hpp"
#include "opencv2/gpu/gpu.hpp"
#endif

//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentFrequencyFilter: public Component {

public:
	//! Constructor.
	ComponentFrequencyFilter(SwisTrackCore *stc);
	//! Destructor.
	~ComponentFrequencyFilter();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentFrequencyFilter(mCore);
	}

private:
	void generateFFTPreview(Mat complexI, Mat *output);
	cv::Mat mFilterMatrix;
	#ifdef GPU_ENABLED
		cv::gpu::GpuMat mFilterMatrixGPU;
		cv::gpu::GpuMat output_gpu;
	#endif
	Display mDisplayOutput;		//!< The DisplayImage showing the output of this component.
	IplImage *mDisplayOutputImage;
	bool mGenerateOutput; 		// configuration parameter specifies if an magnitude image will be calculated for display (takes extra time)
	bool mConfigChanged;
};

#endif

