#ifndef HEADER_StatusItem
#define HEADER_StatusItem

#include <string>

class StatusItem {

public:
	enum eType {
		sTypeError,
		sTypeWarning,
		sTypeInfo
	};

	eType mType;
	std::string mMessage;

	StatusItem(): mType(sTypeInfo), mMessage("") {}
	StatusItem(eType type, const std::string &msg): mType(type), mMessage(msg) {}
	~StatusItem() {}

};

#endif