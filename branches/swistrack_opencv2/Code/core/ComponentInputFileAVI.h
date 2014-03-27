#ifndef HEADER_ComponentInputFileAVI
#define HEADER_ComponentInputFileAVI

#include <vector>
#include <cv.h>
#include <highgui.h>
#include "Component.h"

//! An input component that reads an AVI file using the CV library.
class ComponentInputFileAVI: public Component {

public:
	//! Constructor.
	ComponentInputFileAVI(SwisTrackCore *stc);
	//! Destructor.
	~ComponentInputFileAVI();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentInputFileAVI(mCore);
	}

	double GetProgressPercent();
	double GetProgressMSec();
	int GetProgressFrameNumber();
	double GetFPS();

private:
	bool mFlipVertically;				//!< (configuration) Flip the image vertically (on some computers, the image appears upside down)

	CvCapture* mCapture;				//!< Pointer to AVI sequence.
	IplImage* mOutputImage;				//!< The last acquired image.

	Display mDisplayOutput;				//!< The Display showing the output of this component.
};

#endif

