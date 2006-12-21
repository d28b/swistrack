#ifndef HEADER_ComponentListPanel
#define HEADER_ComponentListPanel

class ComponentListPanel;

#include "SwisTrackCore.h"
#include <wx/panel.h>
#include <wx/listview.h>
#include <wx/button.h>

class ComponentListPanel: public wxPanel {

public:
	//! The associated SwisTrackCore object.
	SwisTrackCore *mSwisTrackCore;

	//! Constructor.
	ComponentListPanel(wxWindow *parent, SwisTrackCore *stc);
	//! Destructor.
	~Component() {}

	void OnUpdate();

	void OnButtonAdd(wxCommandEvent& event);
	void OnButtonRemove(wxCommandEvent& event);
	void OnButtonUp(wxCommandEvent& event);
	void OnButtonDown(wxCommandEvent& event);

private:
	wxListView *mList;
	wxButton *mButtonAdd;
	wxButton *mButtonRemove;
	wxButton *mButtonUp;
	wxButton *mButtonDown;

	DECLARE_EVENT_TABLE()
};

#endif
