#ifndef HEADER_ComponentInputFileImage
#define HEADER_ComponentInputFileImage

#include <vector>
#include <opencv2/core.hpp>
#include "Component.h"

//! An input component that reads an AVI file using the CV library.
class ComponentInputFileImage: public Component {

public:
	//! Constructor.
	ComponentInputFileImage(SwisTrackCore * stc);
	//! Destructor.
	~ComponentInputFileImage();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentInputFileImage(mCore);
	}

private:
	cv::Mat mInputImage;				//!< (configuration) The input image.
	Display mDisplayOutput;				//!< The Display showing the output of this component.
};

#endif

