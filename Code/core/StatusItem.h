#ifndef HEADER_StatusItem
#define HEADER_StatusItem

#include <wx/string.h>

//! A status item.
class StatusItem {

public:
	//! The status message types.
	enum eType {
		sTypeError,
		sTypeWarning,
		sTypeInfo
	};

	eType mType;			//!< The status message type.
	wxString mMessage;	//!< The message text.

	//! Constructor.
	StatusItem(): mType(sTypeInfo), mMessage(wxT("")) {}
	//! Constructor.
	StatusItem(eType type, const wxString &msg): mType(type), mMessage(msg) {}
	//! Destructor.
	~StatusItem() {}

};

#endif
