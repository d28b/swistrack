#ifndef HEADER_ComponentCropInput
#define HEADER_ComponentCropInput

#include <opencv2/opencv.hpp>
#include "Component.h"

//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentCropInput: public Component {

public:
	//! Constructor.
	ComponentCropInput(SwisTrackCore * stc);
	//! Destructor.
	~ComponentCropInput();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentCropInput(mCore);
	}

private:
	cv::Rect mCropRect;			//!< (configuration) Crop rectangle.
	Display mDisplayOutput;		//!< The DisplayImage showing the output of this component.

};

#endif

