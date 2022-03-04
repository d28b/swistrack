#ifndef HEADER_ComponentCrop
#define HEADER_ComponentCrop

#include <opencv2/opencv.hpp>
#include "Component.h"
using namespace cv;

//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentCrop: public Component {

public:
	//! Constructor.
	ComponentCrop(SwisTrackCore * stc);
	//! Destructor.
	~ComponentCrop();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentCrop(mCore);
	}

private:
	cv::Rect mCropRect;			//!< (configuration) Crop rectangle.
	Display mDisplayOutput;		//!< The DisplayImage showing the output of this component.

};

#endif

