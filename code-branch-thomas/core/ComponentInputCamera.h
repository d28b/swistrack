#ifndef _COMPONENTINPUTCAMERA_H
#define _COMPONENTINPUTCAMERA_H

#include "TrackingImage.h"
#include <vector>
#include <math.h>
#include "cv.h"
#include "highgui.h"
#include "Component.h"

#define _1394
#ifdef _1394
#include <1394camera.h>
#endif

class ComponentInputCamera: public Component {

public:
	ComponentInputCamera(xmlpp::Element* cfgRoot);
	~ComponentInputCamera();

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
#ifdef _1394
	/** 1394 Camera handle */
	C1394Camera theCamera;  //!< Camera handle (CMU 1394 Camera Driver)
#endif
	void QueryFrame1394(IplImage* input);
};

#endif

