#ifndef HEADER_ComponentTreeItem
#define HEADER_ComponentTreeItem

class ComponentTreeItem;

#include "Component.h"
#include <wx/treectrl.h>

class ComponentTreeItem: public wxTreeItemData {

public:
	//! The associated Component object.
	Component *mComponent;

	//! Constructor.
	ComponentTreeItem(Component *c): mComponent(c) {}
	//! Destructor.
	~ComponentTreeItem() {}

private:
};

#endif
