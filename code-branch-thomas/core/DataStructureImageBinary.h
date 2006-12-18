#ifndef HEADER_DataStructureImageBinary
#define HEADER_DataStructureImageBinary

#include "DataStructure.h"
#include <string>
#include "cv.h"

class DataStructureImageBinary: public DataStructure {

public:
	//! Image
	IplImage* mImage;

	//! Constructor.
	DataStructureImageBinary(): DataStructure("ImageGray"), mImage(0) {}
	~DataStructureImageBinary() {}
	
	CvSize GetInputDim();
};

#endif

