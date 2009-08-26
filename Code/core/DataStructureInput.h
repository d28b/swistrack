#ifndef HEADER_DataStructureInput
#define HEADER_DataStructureInput

#include "DataStructureImage.h"
#include <wx/datetime.h>

class DataStructureInput: public DataStructureImage {

public:
	int mFrameNumber;			//!< Frame number as provided by the input source (if available). For a reliable
	int mFramesCount;			//!< The total number of frames available (if available).

	//! Constructor.
	DataStructureInput(): DataStructureImage(wxT("Input"), wxT("Input")), mFrameNumber(0), mFramesCount(-1), mFrameTimestamp(wxInvalidDateTime), mTimeSinceLastFrame() {
		mDisplayName = wxT("Input");
	}
	//! Destructor.
	~DataStructureInput() {}

	//! Sets the time stamp of the current frame.
	void SetFrameTimestamp(wxDateTime d);

	//! Returns the frame time stamp.
	wxDateTime & FrameTimestamp() {
		return mFrameTimestamp;
	}

	//! Returns the time difference between the last frame and the current frame.
	wxTimeSpan & TimeSinceLastFrame() {
		return mTimeSinceLastFrame;
	}

private:
	wxDateTime mFrameTimestamp;		//!< The time the frame was recorded (wxInvalidDateTime if not availabe).
	wxTimeSpan mTimeSinceLastFrame;	//!< The time difference from the last frame to the current frame.

};

#endif

