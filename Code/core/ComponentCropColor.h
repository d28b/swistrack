#ifndef HEADER_ComponentCropColor
#define HEADER_ComponentCropColor

#include <opencv2/opencv.hpp>
#include "Component.h"

//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentCropColor: public Component {

public:
	//! Constructor.
	ComponentCropColor(SwisTrackCore * stc);
	//! Destructor.
	~ComponentCropColor();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentCropColor(mCore);
	}

private:
	cv::Rect mCropRect;			//!< (configuration) Crop rectangle.
	Display mDisplayOutput;		//!< The DisplayImage showing the output of this component.

};

#endif

