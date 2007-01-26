#ifndef HEADER_DataStructureInput
#define HEADER_DataStructureInput

#include "DataStructureImage.h"

class DataStructureInput: public DataStructureImage {

public:
	int mFrameNumber;			//!< Frame number.
	double mFramesPerSecond;	//!< Frames per second.

	//! Constructor.
	DataStructureInput(): DataStructureImage("Input", "Input"), mFrameNumber(0), mFramesPerSecond(-1) {mDisplayName="Input";}
	//! Destructor.
	~DataStructureInput() {}
};

#endif

