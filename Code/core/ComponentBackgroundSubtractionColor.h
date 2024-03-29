#ifndef HEADER_ComponentBackgroundSubtractionColor
#define HEADER_ComponentBackgroundSubtractionColor

#include <opencv2/core.hpp>
#include "Component.h"

//! A component subtracting a fixed background from a color image.
class ComponentBackgroundSubtractionColor: public Component {

public:
	//! Constructor.
	ComponentBackgroundSubtractionColor(SwisTrackCore * stc);
	//! Destructor.
	~ComponentBackgroundSubtractionColor();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentBackgroundSubtractionColor(mCore);
	}

private:
	cv::Mat mBackgroundImage;			//!< The background image.
	cv::Scalar mBackgroundImageMean;	//!< The mean of the background image.
	bool mCorrectMean;					//!< (configuration) Whether to correct for the mean or not.

	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.
};

#endif

