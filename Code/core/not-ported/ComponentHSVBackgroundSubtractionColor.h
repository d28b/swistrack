#ifndef HEADER_ComponentHSVBackgroundSubtractionColor
#define HEADER_ComponentHSVBackgroundSubtractionColor

#include <opencv2/core.hpp>
#include "Component.h"

//! A component subtracting a fixed background from a color image.
class ComponentHSVBackgroundSubtractionColor: public Component {

public:
	//! Constructor.
	ComponentHSVBackgroundSubtractionColor(SwisTrackCore * stc);
	//! Destructor.
	~ComponentHSVBackgroundSubtractionColor();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentHSVBackgroundSubtractionColor(mCore);
	}

private:
	cv::Mat mBackgroundImage;			//!< The background image.
	cv::Mat mBackgroundImageHSV;		//!< The background image in HSV.

	bool mCorrectMean;					//!< (configuration) Whether to correct for the mean or not.
	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.
};

#endif

