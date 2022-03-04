#ifndef HEADER_ComponentSpecificColorSubtraction
#define HEADER_ComponentSpecificColorSubtraction

#include <opencv2/core.hpp>
#include "Component.h"

//! A component subtracting a fixed background from a color image.
class ComponentSpecificColorSubtraction: public Component {

public:
	//! Constructor.
	ComponentSpecificColorSubtraction(SwisTrackCore * stc);
	//! Destructor.
	~ComponentSpecificColorSubtraction();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentSpecificColorSubtraction(mCore);
	}

private:
	enum Mode {
		AbsDiff,
		ImageMinusColor,
		ColorMinusImage,
	};

	cv::Scalar mColor;				//!< (configuration) The specified color to subtract.
	Mode mMode;						//!< (configuration) The subtraction mode.
	Display mDisplayOutput;			//!< The DisplayImage showing the output of this component.
};

#endif

