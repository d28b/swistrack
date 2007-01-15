#ifndef HEADER_ErrorList
#define HEADER_ErrorList

class ErrorList;

#include <string>
#include <list>

class ErrorList {

public:
	//! Parameter list type.
	typedef std::list<std::string> tList;
	//! The list of errors.
	tParameters mList;

	//! Constructor.
	ErrorList(): mList() {}
	//! Destructor.
	~ErrorList() {}

	//! Adds an error message.
	void Add(const std::string &msg, int linenumber=0);

	//! Returns whether the list is empty.
	bool IsEmpty() {return (mList.count()==0);}
};

#endif
