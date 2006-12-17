#ifndef HEADER_DataStructureInput
#define HEADER_DataStructureInput

#include "DataStructure.h"
#include <string>
#include "cv.h"

class DataStructureInput: public DataStructure {

public:
	//! Frame number
	int mFrameNumber;
	//! Frames per second
	double mFramesPerSecond;

	//! Constructor.
	DataStructureInput(): DataStructure("Input"), mFrameNumber(0), mFramesPerSecond(-1) {}
	~DataStructureInput() {}
};

#endif

