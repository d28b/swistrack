#ifndef HEADER_ComponentCategory
#define HEADER_ComponentCategory

class ComponentCategory;

#include <string>

//! Holds information about a component category. Categories (objects of this class) are created by the SwisTrackCore object.
class ComponentCategory {

public:
	//! Category types.
	enum eCategoryType {
		sTypeAny=0,		//!< The user can choose any number of these components.
		sTypeOne,		//!< The user should choose exactly one component of this category.
		sTypeAuto
	};

	std::string mName;			//!< The internal name of the category.
	std::string mDisplayName;	//!< The name that is displayed to the user.
	int mOrder;					//!< The order (0 if any ordering is fine).
	eCategoryType mType;		//!< The type.

	//! Constructor.
	ComponentCategory(const std::string &name, const std::string &displayname, int order, eCategoryType type=sTypeAny): mName(name), mDisplayName(displayname), mOrder(order), mType(type) {}
	//! Destructor.
	~ComponentCategory() {}
};

#endif
