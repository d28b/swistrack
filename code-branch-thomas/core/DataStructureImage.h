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
	bool mIsColor;
	//! Frame number
	int mFrameNumber;

	//! Constructor.
	DataStructureImage(): DataStructure("Image"), mImage(0), mIsColor(true), mFrameNumber(-1) {}
	~DataStructureImage() {}
};

#endif

