#ifndef HEADER_DataStructureInput
#define HEADER_DataStructureInput

#include "DataStructureImage.h"
#include <wx/datetime.h>

class DataStructureInput: public DataStructureImage {

public:
	int mFrameNumber;			//!< Frame number (-1 if not available).
	int mFramesCount;			//!< The total number of frames (-1 if not available).
	double mFramesPerSecond;	//!< Frames per second.

	wxDateTime & FrameTimestamp() {
	  return mFrameTimestamp;
	}
	wxTimeSpan & TimeSinceLastFrame() {
	  return mTimeSinceLastFrame;
	}
	void SetFrameTimestamp(wxDateTime d) {
	  if (mFrameTimestamp.IsValid()) {
	    mTimeSinceLastFrame = d - mFrameTimestamp;
	  }
	  mFrameTimestamp = d;
	}


	//! Constructor.
 DataStructureInput(): DataStructureImage(wxT("Input"), wxT("Input")), mFrameNumber(0), mFramesCount(-1), mFramesPerSecond(-1), mFrameTimestamp(wxInvalidDateTime), mTimeSinceLastFrame() {
		mDisplayName = wxT("Input");
	}
	//! Destructor.
	~DataStructureInput() {}

 private:
	wxDateTime mFrameTimestamp; //!< The time the frame was recorded (wxInvalidDateTime if not availabe)
	wxTimeSpan mTimeSinceLastFrame;

	
};

#endif

