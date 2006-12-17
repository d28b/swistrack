#ifndef HEADER_DataStructureImage
#define HEADER_DataStructureImage

#include "DataStructure.h"
#include <string>
#include "cv.h"

class DataStructureImage: public DataStructure {

public:
	//! Image
	IplImage* mImage;

	//! Constructor.
	DataStructureImage(): DataStructure("Image"), mImage(0) {}
	~DataStructureImage() {}
	
	CvSize GetInputDim();
};

#endif

