#ifndef HEADER_DataStructureInput
#define HEADER_DataStructureInput

#include "DataStructureImage.h"

class DataStructureInput: public DataStructureImage {

public:
	//! Frame number
	int mFrameNumber;
	//! Frames per second
	double mFramesPerSecond;

	//! Constructor.
	DataStructureInput(): DataStructureImage("Input", "Input"), mFrameNumber(0), mFramesPerSecond(-1) {mDisplayName="Input";}
	~DataStructureInput() {}
};

#endif

