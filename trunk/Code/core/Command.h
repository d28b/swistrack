#ifndef HEADER_Command
#define HEADER_Command
#include <wx/datetime.h>
#include <cv.h>

//! A Command.
class Command {

public:
	int mID;					//!< ID of the Command, or of its associated track.
	wxString mName;
	float mParamValue;
	
	wxDateTime mTimestamp;

	//! Constructor.
	Command(int id, wxString name, float paramVal): mID(id), mName(name), mParamValue(paramVal) , mTimestamp() {}
	//! Destructor.
	~Command() {
	}

	
};

#endif

