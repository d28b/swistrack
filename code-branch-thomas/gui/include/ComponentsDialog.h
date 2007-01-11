#ifndef HEADER_ComponentsDialog
#define HEADER_ComponentsDialog

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

	void OnUpdate();

	void OnButtonAdd(wxCommandEvent& event);
	void OnButtonCancel(wxCommandEvent& event);

protected:
	enum eConstants {
		eID_ButtonAdd,
		eID_ButtonCancel
	};

private:
	wxTreeCtrl *mTree;
	wxButton *mButtonAdd;
	wxButton *mButtonCancel;

	DECLARE_EVENT_TABLE()
};

#endif
