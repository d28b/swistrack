#ifndef _DATASTRUCTURE_H
#define _DATASTRUCTURE_H

#include <wx/string.h>

//! The base class for data structures.
class DataStructure {

public:
	wxString mName;			//!< The internal name of this data structure. This should only contain english letters and numbers.
	wxString mDisplayName;	//!< The name that is displayed to the user.

	//! Constructor.
	DataStructure(const wxString &name = wxT(""), const wxString &displayname = wxT("")): mName(name), mDisplayName(displayname) {}
	//! Destructor.
	virtual ~DataStructure() {}
};

#endif

