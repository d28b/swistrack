#ifndef _INPUT_H
#define _INPUT_H

#include "DataStructure.h"
#include <string>
#include "cv.h"

class DataStructureImage: public DataStructure {

public:
	//! Image
	IplImage* mImage;
	//! Color flag
	int mIsColor;
		/** Frame number */
		int nFrame;
#ifdef _1394
		/** 1394 Camera handle */
		C1394Camera theCamera;  //!< Camera handle (CMU 1394 Camera Driver)
#endif		
		void QueryFrame1394(IplImage* input);

	//! Constructor.
	DataStructure(): mName("") {}
	DataStructure(std::string &nam): mName(nam) {}

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
};

#endif

