#ifndef HEADER_ComponentCategory
#define HEADER_ComponentCategory

class ComponentCategory;

#include <string>
#include <list>
#include <libxml++/libxml++.h>
#include "StatusItem.h"
#include "SwisTrackCore.h"
#include "DataStructure.h"

class ComponentCategory {

public:
	enum eFlags {
		sFlagNone=0,
		sFlagRequired=1,
		sFlagExclusive=2,
	};

	//! The name that is displayed to the user.
	std::string mDisplayName;
	//! The order (0 if any ordering is fine).
	int mOrder;
	//! The flags.
	eFlags mFlags;

	//! Constructor.
	ComponentCategory(const std::string &name, int order, int flags): mDisplayName(name), mOrder(order), mFlags(flags) {}
	//! Destructor.
	~ComponentCategory() {}
};

#endif
