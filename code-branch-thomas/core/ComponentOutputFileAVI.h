#ifndef HEADER_ComponentOutputFileAVI
#define HEADER_ComponentOutputFileAVI

#include <vector>
#include <cv.h>
#include <highgui.h>
#include "Component.h"

//! An input component that reads an AVI file using the CV library.
class ComponentOutputFileAVI: public Component {

public:
	//! Constructor.
	ComponentOutputFileAVI(SwisTrackCore *stc);
	//! Destructor.
	~ComponentOutputFileAVI();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {return new ComponentOutputFileAVI(mCore);}

	double GetProgressPercent();
	double GetProgressMSec();
	int GetProgressFrameNumber();
	double GetFPS();

private:
	CvVideoWriter* mWriter;				//!< Pointer to AVI sequence.
	IplImage* mOutputImage;				//!< The last acquired image.
	int mFrameRate;						//!< The frame rate of the output AVI
	int mInputSelection;				//!< Select the input
	std::string filename;				//!< Name of the saved AVI

	Display mDisplayOutput;				//!< The DisplayImage showing the output of this component.
};

#endif

