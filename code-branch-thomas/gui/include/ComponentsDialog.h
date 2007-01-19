#ifndef HEADER_ComponentsDialog
#define HEADER_ComponentsDialog

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

class ComponentsDialog;

#include "SwisTrackCore.h"
#include <wx/dialog.h>
#include <wx/treectrl.h>
#include <wx/button.h>

class ComponentsDialog: public wxDialog {

public:
	//! The associated SwisTrackCore object.
	SwisTrackCore *mSwisTrackCore;
	//! The selected component.
	Component *mSelectedComponent;

	//! Constructor.
	ComponentsDialog(wxWindow *parent, SwisTrackCore *stc);
	//! Destructor.
	~ComponentsDialog() {}

	//! Updates the component list.
	void OnUpdate();

	//! Handles the corresponding GUI event.
	void OnButtonAdd(wxCommandEvent& event);
	//! Handles the corresponding GUI event.
	void OnButtonCancel(wxCommandEvent& event);

protected:
	enum eConstants {
		eID_ButtonAdd,
		eID_ButtonCancel
	};

private:
	//! The tree with the components.
	wxTreeCtrl *mTree;
	//! The corresponding GUI object.
	wxButton *mButtonAdd;
	//! The corresponding GUI object.
	wxButton *mButtonCancel;

	DECLARE_EVENT_TABLE()
};

#endif
