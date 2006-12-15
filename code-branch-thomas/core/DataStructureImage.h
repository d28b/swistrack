#ifndef HEADER_DataStructureImage
#define HEADER_DataStructureImage

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

