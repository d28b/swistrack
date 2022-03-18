#ifndef HEADER_ComponentInputFileAVI
#define HEADER_ComponentInputFileAVI

#include <opencv2/core.hpp>
#include "Component.h"

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

private:
	cv::VideoCapture mCapture;			//!< OpenCV capture object.
	Display mDisplayOutput;				//!< The Display showing the output of this component.

	double GetProgressPercent();
	double GetProgressMSec();
	int GetProgressFrameNumber();
	double GetFPS();

};

#endif

