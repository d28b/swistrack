#ifndef HEADER_StatusItem
#define HEADER_StatusItem

#include <string>

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
	std::string mMessage;	//!< The message text.

	//! Constructor.
	StatusItem(): mType(sTypeInfo), mMessage("") {}
	//! Constructor.
	StatusItem(eType type, const std::string &msg): mType(type), mMessage(msg) {}
	//! Destructor.
	~StatusItem() {}

};

#endif
