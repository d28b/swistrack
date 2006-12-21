#ifndef HEADER_ComponentListPanel
#define HEADER_ComponentListPanel

class ComponentListPanel;

#include "SwisTrackCore.h"
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
	};

	//! The associated SwisTrackCore object.
	SwisTrackCore *mSwisTrackCore;

	//! Constructor.
	ComponentListPanel(wxWindow *parent, SwisTrackCore *stc);
	//! Destructor.
	~ComponentListPanel() {}

	void OnUpdate();

	void OnButtonAdd(wxCommandEvent& event);
	void OnButtonRemove(wxCommandEvent& event);
	void OnButtonUp(wxCommandEvent& event);
	void OnButtonDown(wxCommandEvent& event);

private:
	wxListCtrl *mList;
	wxButton *mButtonAdd;
	wxButton *mButtonRemove;
	wxButton *mButtonUp;
	wxButton *mButtonDown;

	DECLARE_EVENT_TABLE()
};

#endif
