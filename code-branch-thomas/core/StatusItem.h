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

	//! The status message type.
	eType mType;
	//! The message text.
	std::string mMessage;

	//! Constructor.
	StatusItem(): mType(sTypeInfo), mMessage("") {}
	//! Constructor.
	StatusItem(eType type, const std::string &msg): mType(type), mMessage(msg) {}
	//! Destructor.
	~StatusItem() {}

};

#endif