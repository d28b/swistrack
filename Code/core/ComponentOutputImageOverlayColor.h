#ifndef HEADER_ComponentOutputImageOverlayColor
#define HEADER_ComponentOutputImageOverlayColor

#include <opencv2/opencv.hpp>
#include "Component.h"

//! A component subtracting a fixed background from a color image.
class ComponentOutputImageOverlayColor: public Component {

public:
	//! Constructor.
	ComponentOutputImageOverlayColor(SwisTrackCore * stc);
	//! Destructor.
	~ComponentOutputImageOverlayColor();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentOutputImageOverlayColor(mCore);
	}

private:
	enum eMode {
		Addition,
		Subtraction,
		Multiplication,
	};

	cv::Mat mFinalImage;			//!< The image being constructed.
	eMode mMode;					//!< (configuration) The combination mode.
	std::string mFileName;			//!< (configuration) directory and filename prefix

	Display mDisplayOutput;			//!< The DisplayImage showing the output of this component.
};

#endif

