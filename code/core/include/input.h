#ifndef _INPUT_H
#define _INPUT_H

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


class Input : public Component
{
public:
	void SetParameters();
	Input(xmlpp::Element* cfgRoot);
	IplImage* GetFrame();
	double GetProgress(int kind);
	CvSize GetInputDim();
	int GetInputDepth();
	int GetInputNbChannels();
	int GetInputOrigin();
	IplImage* GetInputIpl();
	double GetFPS();

private:
		/** Input image, from an AVI sequence */
		IplImage* input;
		/** Pointer to AVI sequence */
		CvCapture* Capture;	
		/** Color input flag */
		int isinputincolor;
		/** Frame number */
		int nFrame;
#ifdef _1394
		/** 1394 Camera handle */
		C1394Camera theCamera;  //!< Camera handle (CMU 1394 Camera Driver)
#endif		
		void QueryFrame1394(IplImage* input);

		

protected:
	~Input();
};

#endif

