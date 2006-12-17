#ifndef HEADER_ComponentInputFileAVI
#define HEADER_ComponentInputFileAVI

#include "TrackingImage.h"
#include <vector>
#include <math.h>
#include "cv.h"
#include "highgui.h"
#include "Component.h"

class ComponentFileAVI: public Component {

public:
	ComponentInputFileAVI(SwisTrackCore *stc);
	~ComponentInputFileAVI();

	// Overwritten Component methods
	bool Start();
	bool Step();
	bool Stop();

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

