#ifndef HEADER_ComponentTreeItem
#define HEADER_ComponentTreeItem

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class ComponentTreeItem;

#include "Component.h"
#include <wx/treectrl.h>

class ComponentTreeItem: public wxTreeItemData {

public:
	Component *mComponent;		//!< The associated Component object.

	//! Constructor.
	ComponentTreeItem(Component *c): mComponent(c) {}
	//! Destructor.
	~ComponentTreeItem() {}

private:
};

#endif
