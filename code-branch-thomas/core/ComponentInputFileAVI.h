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
	ComponentInputFileAVI(xmlpp::Element* cfgRoot);
	~ComponentInputFileAVI();

	// Overwritten Component methods
	void SetParameters();
	void Step();

	void SetParameters();
	Input(xmlpp::Element* cfgRoot);
	virtual ~Input();
	IplImage* GetFrame();
	double GetProgress(int kind);
	CvSize GetInputDim();
	int GetInputDepth();
	int GetInputNbChannels();
	int GetInputOrigin();
	IplImage* GetInputIpl();
	double GetFPS();

private:
	/** Pointer to AVI sequence */
	CvCapture* mCapture;
};

#endif

