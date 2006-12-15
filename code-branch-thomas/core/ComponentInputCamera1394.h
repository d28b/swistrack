#ifndef HEADER_ComponentInputCamera1394
#define HEADER_ComponentInputCamera1394

#include "TrackingImage.h"
#include <vector>
#include <math.h>
#include "cv.h"
#include "highgui.h"
#include "Component.h"

#define _1394
#ifdef _1394
#include <1394camera.h>

class ComponentInputCamera1394: public Component {

public:
	ComponentInputCamera1394(xmlpp::Element* cfgRoot);
	~ComponentInputCamera1394();

	// Overwritten Component methods
	void SetParameters();
	void Step();

	IplImage* GetFrame();
	double GetProgress(int kind);
	CvSize GetInputDim();
	int GetInputDepth();
	int GetInputNbChannels();
	int GetInputOrigin();
	IplImage* GetInputIpl();
	double GetFPS();

private:
	/** 1394 Camera handle (CMU 1394 Camera Driver) */
	C1394Camera theCamera;  //!< Camera handle 
	void QueryFrame1394(IplImage* input);
};

#else

class ComponentInputCamera1394: public Component {

public:
	ComponentInputCamera1394(SwisTrackCore *stc): Component(stc) {}
	~ComponentInputCamera1394();

	// Overwritten Component methods
	void SetParameters() {}
	void Step() {}
	void UpdateStatus() {ClearStatus(); AddError("1394 support was not compiled into this build.");}
};

#endif

#endif

