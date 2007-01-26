#ifndef HEADER_ComponentInputFileAVI
#define HEADER_ComponentInputFileAVI

#include <vector>
#include "cv.h"
#include "highgui.h"
#include "Component.h"

//! An input component that reads an AVI file.
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
	Component *Create() {return new ComponentInputFileAVI(mCore);}

	double GetProgressPercent();
	double GetProgressMSec();
	int GetProgressFrameNumber();
	double GetFPS();

private:
	//! Pointer to AVI sequence.
	CvCapture* mCapture;
	//! The last acquired image.
	IplImage* mLastImage;
};

#endif

