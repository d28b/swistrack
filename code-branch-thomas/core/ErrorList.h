#ifndef HEADER_ErrorList
#define HEADER_ErrorList

class ErrorList;

#include <string>
#include <list>
#include "ErrorListItem.h"

class ErrorList {

public:
	//! Parameter list type.
	typedef std::list<ErrorListItem> tList;
	//! The list of errors.
	tList mList;

	//! Constructor.
	ErrorList(): mList() {}
	//! Destructor.
	~ErrorList() {}

	//! Adds an error message.
	void Add(const std::string &msg, int linenumber=0);
};

#endif
