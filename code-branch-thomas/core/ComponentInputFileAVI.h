#ifndef HEADER_ComponentInputFileAVI
#define HEADER_ComponentInputFileAVI

#include <vector>
#include "cv.h"
#include "highgui.h"
#include "Component.h"

class ComponentInputFileAVI: public Component {

public:
	ComponentInputFileAVI(SwisTrackCore *stc);
	~ComponentInputFileAVI();

	// Overwritten Component methods
	void OnStart();
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

