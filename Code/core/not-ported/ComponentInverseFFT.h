#ifndef HEADER_ComponentInverseFFT
#define HEADER_ComponentInverseFFT

#include <opencv2/core.hpp>
#include "Component.h"

// #define GPU_ENABLED

//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentInverseFFT: public Component {

public:
	//! Constructor.
	ComponentInverseFFT(SwisTrackCore * stc);
	//! Destructor.
	~ComponentInverseFFT();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentInverseFFT(mCore);
	}

private:
	#ifdef GPU_ENABLED
	cv::gpu::GpuMat output_gpu;
	#endif
	Display mDisplayOutput;		//!< The DisplayImage showing the output of this component.
	cv::Mat mDisplayOutputImage;
    cv::Mat out;
	cv::Mat planes[2];
};

#endif

