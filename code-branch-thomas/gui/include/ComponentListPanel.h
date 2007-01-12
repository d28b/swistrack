#ifndef HEADER_ComponentListPanel
#define HEADER_ComponentListPanel

class ComponentListPanel;

#include "SwisTrackCore.h"
#include "ComponentsDialog.h"
#include <wx/panel.h>
#include <wx/listctrl.h>
#include <wx/button.h>

class ComponentListPanel: public wxPanel {

public:
	enum eConstants {
		eID_ButtonAdd,
		eID_ButtonRemove,
		eID_ButtonUp,
		eID_ButtonDown,
		eID_List,
	};

	//! The associated SwisTrackCore object.
	SwisTrackCore *mSwisTrackCore;
	//! The selected component.
	Component *mSelectedComponent;

	//! Constructor.
	ComponentListPanel(wxWindow *parent, SwisTrackCore *stc);
	//! Destructor.
	~ComponentListPanel() {}

	void OnUpdate();

	void OnButtonAdd(wxCommandEvent& event);
	void OnButtonRemove(wxCommandEvent& event);
	void OnButtonUp(wxCommandEvent& event);
	void OnButtonDown(wxCommandEvent& event);
	void OnListItemSelected(wxListEvent& event);

private:
	wxListCtrl *mList;
	wxButton *mButtonAdd;
	wxButton *mButtonRemove;
	wxButton *mButtonUp;
	wxButton *mButtonDown;

	ComponentsDialog *mComponentsDialog;

	DECLARE_EVENT_TABLE()
};

#endif
