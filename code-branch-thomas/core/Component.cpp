#include "Component.h"
#define THISCLASS Component

void THISCLASS::ClearStatus() {
	mStatusList->clear();
}

void THISCLASS::AddError(const std::string &msg) {
	mStatusList->push_back(StatusItem(StatusItem::sTypeError, msg));
}

void THISCLASS::AddWarning(const std::string &msg) {
	mStatusList->push_back(StatusItem(StatusItem::sTypeWarning, msg));
}

void THISCLASS::AddInfo(const std::string &msg) {
	mStatusList->push_back(StatusItem(StatusItem::sTypeInfo, msg));
}
