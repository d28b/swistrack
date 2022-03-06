#ifndef HEADER_ComponentCropBinary
#define HEADER_ComponentCropBinary

#include <opencv2/opencv.hpp>
#include "Component.h"

//! A component that detects blobs that have a certain size and stores them as particles in DataStructureParticle.
class ComponentCropBinary: public Component {

public:
	//! Constructor.
	ComponentCropBinary(SwisTrackCore * stc);
	//! Destructor.
	~ComponentCropBinary();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentCropBinary(mCore);
	}

private:
	cv::Rect mCropRect;			//!< (configuration) Crop rectangle.
	Display mDisplayOutput;		//!< The DisplayImage showing the output of this component.

};

#endif

