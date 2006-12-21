#ifndef _DATASTRUCTURE_H
#define _DATASTRUCTURE_H

#include <string>

class DataStructure {

public:
	//! Name of this data structure.
	std::string mName;
	std::string mDisplayName;

	//! Constructor.
	DataStructure(): mName("") {}
	DataStructure(const std::string &name): mName(name) {}

	//! Destructor.
	virtual ~DataStructure() {}

};

#endif

