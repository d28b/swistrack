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
	DataStructureImage(const std::string &name, const std::string &displayname): DataStructure(name), mImage(0) {mDisplayName=displayname;}
	~DataStructureImage() {}

	CvSize GetSize();
};

#endif
