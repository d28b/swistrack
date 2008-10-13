#ifndef HEADER_DataStructureInput
#define HEADER_DataStructureInput

#include "DataStructureImage.h"
#include <wx/datetime.h>

class DataStructureInput: public DataStructureImage {

public:
	int mFrameNumber;			//!< Frame number (-1 if not available).
	int mFramesCount;			//!< The total number of frames (-1 if not available).
	double mFramesPerSecond;	//!< Frames per second.
	wxDateTime mFrameTimestamp; //!< The time the frame was recorded (wxInvalidDateTime if not availabe)

	//! Constructor.
	  DataStructureInput(): DataStructureImage(wxT("Input"), wxT("Input")), mFrameNumber(-1), mFramesCount(-1), mFramesPerSecond(-1), mFrameTimestamp(wxInvalidDateTime) {
		mDisplayName = wxT("Input");
	}
	//! Destructor.
	~DataStructureInput() {}
};

#endif

