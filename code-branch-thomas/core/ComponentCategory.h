#ifndef HEADER_ComponentCategory
#define HEADER_ComponentCategory

class ComponentCategory;

#include <string>

class ComponentCategory {

public:
	enum eCategoryType {
		sTypeAny=0,
		sTypeOne,
		sTypeAuto
	};

	//! The name that is displayed to the user.
	std::string mDisplayName;
	//! The order (0 if any ordering is fine).
	int mOrder;
	//! The flags.
	eCategoryType mType;

	//! Constructor.
	ComponentCategory(const std::string &name, int order, eCategoryType type=sTypeAny): mDisplayName(name), mOrder(order), mType(type) {}
	//! Destructor.
	~ComponentCategory() {}
};

#endif
