#ifndef HEADER_ComponentMorphology
#define HEADER_ComponentMorphology

#include <opencv2/core.hpp>
#include <string>
#include "Component.h"
#include "DataStructureParticles.h"

//! Multi-purpose Morphology component.
class ComponentMorphology: public Component {

public:

	//! Constructor.
	ComponentMorphology(SwisTrackCore * stc);
	//! Destructor.
	~ComponentMorphology();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentMorphology(mCore);
	}

private:
	int mOperation;						//!< (configuration) The operation type.
	cv::Mat mKernel;					//!< (configuration) The erosion/dilation kernel.
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.
};

#endif

