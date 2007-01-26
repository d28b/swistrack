#ifndef HEADER_ErrorListItem
#define HEADER_ErrorListItem

class ErrorListItemItem;

#include <string>
#include <list>

// An item in an error list.
class ErrorListItem {

public:
	std::string mMessage;	//!< The error message.
	int mLineNumber;		//!< The line number.

	//! Constructor.
	ErrorListItem(): mMessage(""), mLineNumber(0) {}
	//! Destructor.
	~ErrorListItem() {}
};

#endif
