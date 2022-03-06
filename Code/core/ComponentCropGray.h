#ifndef HEADER_ComponentCropGray
#define HEADER_ComponentCropGray

#include <opencv2/opencv.hpp>
#include "Component.h"

//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentCropGray: public Component {

public:
	//! Constructor.
	ComponentCropGray(SwisTrackCore * stc);
	//! Destructor.
	~ComponentCropGray();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentCropGray(mCore);
	}

private:
	cv::Rect mCropRect;			//!< (configuration) Crop rectangle.
	Display mDisplayOutput;		//!< The DisplayImage showing the output of this component.

};

#endif

