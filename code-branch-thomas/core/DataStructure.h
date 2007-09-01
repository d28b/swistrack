#ifndef _DATASTRUCTURE_H
#define _DATASTRUCTURE_H

#include <string>

//! The base class for data structures.
class DataStructure {

public:
	std::string mName;			//!< The internal name of this data structure. This should only contain english letters and numbers.
	std::string mDisplayName;	//!< The name that is displayed to the user.

	//! Constructor.
	DataStructure(const std::string &name="", const std::string &displayname=""): mName(name), mDisplayName(displayname) {}
	//! Destructor.
	virtual ~DataStructure() {}
};

#endif

