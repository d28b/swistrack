#ifndef HEADER_ComponentsDialog
#define HEADER_ComponentsDialog

class ComponentsDialog;

#include "SwisTrackCore.h"
#include <wx/panel.h>
#include <wx/listctrl.h>
#include <wx/button.h>

class ComponentsDialog: public wxDialog {

public:
	enum eConstants {
		eID_ButtonAdd,
		eID_ButtonRemove,
	};

	//! The associated SwisTrackCore object.
	SwisTrackCore *mSwisTrackCore;
	//! The selected component.
	Component *mSelectedComponent;

	//! Constructor.
	ComponentsDialog(wxWindow *parent, SwisTrackCore *stc);
	//! Destructor.
	~ComponentsDialog() {}

	void OnUpdate();

	void OnButtonAdd(wxCommandEvent& event);
	void OnButtonRemove(wxCommandEvent& event);
	void OnButtonUp(wxCommandEvent& event);
	void OnButtonDown(wxCommandEvent& event);

private:
	wxTreeCtrl *mTree;
	wxButton *mButtonAdd;
	wxButton *mButtonCancel;

	DECLARE_EVENT_TABLE()
};

#endif
