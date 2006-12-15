#ifndef _DATASTRUCTURE_H
#define _DATASTRUCTURE_H

#include <string>

class DataStructure {

public:
	//! Name of this data structure.
	std::string mName;

	//! Constructor.
	DataStructure(): mName("") {}
	DataStructure(std::string &nam): mName(nam) {}

	//! Destructor.
	virtual ~DataStructure() {}

};

#endif

