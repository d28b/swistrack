#ifndef HEADER_ComponentInputFileAVI
#define HEADER_ComponentInputFileAVI

#include <opencv2/core.hpp>
#include "Component.h"

//! An input component that reads an AVI file using the CV library.
class ComponentInputFileAVI: public Component {

public:
	//! Constructor.
	ComponentInputFileAVI(SwisTrackCore * stc);
	//! Destructor.
	~ComponentInputFileAVI();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();

	Component * Create() {
		return new ComponentInputFileAVI(mCore);
	}

	double GetProgressPercent();
	double GetProgressMSec();
	int GetProgressFrameNumber();
	double GetFPS();

private:
	bool mFlipHorizontally;				//!< (configuration) Flip the image horizontally.
	bool mFlipVertically;				//!< (configuration) Flip the image vertically.

	cv::VideoCapture mCapture;			//!< OpenCV capture object.

	Display mDisplayOutput;				//!< The Display showing the output of this component.
};

#endif

