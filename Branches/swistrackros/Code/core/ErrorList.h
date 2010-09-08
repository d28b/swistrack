#ifndef HEADER_ErrorList
#define HEADER_ErrorList

class ErrorList;

#include <wx/string.h>
#include <list>
#include "ErrorListItem.h"

//! A list of error messages. This is used by the XML functions.
class ErrorList {

public:
	//! Parameter list type.
	typedef std::list<ErrorListItem> tList;

	tList mList;	//!< The list of errors.

	//! Constructor.
	ErrorList(): mList() {}
	//! Destructor.
	~ErrorList() {}

	//! Adds an error message.
	void Add(const wxString &msg, int linenumber = 0);
};

#endif
