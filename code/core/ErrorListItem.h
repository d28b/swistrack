#ifndef HEADER_ErrorListItem
#define HEADER_ErrorListItem

class ErrorListItemItem;

#include <wx/string.h>
#include <list>

// An item in an error list.
class ErrorListItem {

public:
	wxString mMessage;	//!< The error message.
	int mLineNumber;		//!< The line number.

	//! Constructor.
	ErrorListItem(): mMessage(wxT("")), mLineNumber(0) {}
	//! Destructor.
	~ErrorListItem() {}
};

#endif
