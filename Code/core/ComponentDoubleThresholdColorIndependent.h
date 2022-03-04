#ifndef HEADER_ComponentDoubleThresholdColorIndependent
#define HEADER_ComponentDoubleThresholdColorIndependent

#include <opencv2/core.hpp>
#include "Component.h"

//! A component that applies two thresholds to a color image, there are two independent thresholds for each channels. An "AND" function is then applied between the different channels. The output is a binary image.
class ComponentDoubleThresholdColorIndependent: public Component {

public:
	//! Constructor.
	ComponentDoubleThresholdColorIndependent(SwisTrackCore * stc);
	//! Destructor.
	~ComponentDoubleThresholdColorIndependent();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentDoubleThresholdColorIndependent(mCore);
	}

private:
	cv::Scalar mLowThreshold;			//!< (configuration) Low threshold
	cv::Scalar mHighThreshold;			//!< (configuration) High threshold
	char thresholdColorSeq[4];			//!< Channel sequence of the thresholds.
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.
};

#endif

