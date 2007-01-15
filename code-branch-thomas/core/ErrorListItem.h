#ifndef HEADER_ErrorListItem
#define HEADER_ErrorListItem

class ErrorListItemItem;

#include <string>
#include <list>

class ErrorListItem {

public:
	//! The error message.
	std::string mMessage;
	//! The line number.
	int mLineNumber;

	//! Constructor.
	ErrorListItem(): mMessage(""), mLineNumber(0) {}
	//! Destructor.
	~ErrorListItem() {}
};

#endif
