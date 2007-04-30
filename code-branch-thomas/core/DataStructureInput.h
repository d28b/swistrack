#ifndef HEADER_DataStructureInput
#define HEADER_DataStructureInput

#include "DataStructureImage.h"

class DataStructureInput: public DataStructureImage {

public:
	int mFrameNumber;			//!< Frame number (-1 if not available).
	int mFrameCount;			//!< The total number of frames (-1 if not available).
	double mFramesPerSecond;	//!< Frames per second.

	//! Constructor.
	DataStructureInput(): DataStructureImage("Input", "Input"), mFrameNumber(-1), mFrameCount(-1), mFramesPerSecond(-1) {mDisplayName="Input";}
	//! Destructor.
	~DataStructureInput() {}
};

#endif

