#ifndef HEADER_DataStructureImageGray
#define HEADER_DataStructureImageGray

#include "DataStructure.h"
#include <string>
#include "cv.h"

class DataStructureImageGray: public DataStructure {

public:
	//! Image
	IplImage* mImage;

	//! Constructor.
	DataStructureImageGray(): DataStructure("ImageGray"), mImage(0) {}
	~DataStructureImageGray() {}
	
	CvSize GetInputDim();
};

#endif

