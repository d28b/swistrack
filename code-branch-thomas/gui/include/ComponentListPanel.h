#ifndef HEADER_ComponentListPanel
#define HEADER_ComponentListPanel

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

class ComponentListPanel;

#include "SwisTrackCore.h"
#include "ComponentsDialog.h"
#include <wx/panel.h>
#include <wx/listctrl.h>
#include <wx/button.h>

class ComponentListPanel: public wxPanel, public SwisTrackCoreInterface {

public:
	//! The associated SwisTrackCore object.
	SwisTrackCore *mSwisTrackCore;
	//! The selected component.
	Component *mSelectedComponent;

	//! Constructor.
	ComponentListPanel(wxWindow *parent, SwisTrackCore *stc);
	//! Destructor.
	~ComponentListPanel() {}

	//! Updates the list completely.
	void Update();
	//! Updates the status messages.
	void UpdateStatus();

	//! Handles the corresponding GUI event.
	void OnButtonAddClick(wxCommandEvent& event);
	//! Handles the corresponding GUI event.
	void OnButtonRemoveClick(wxCommandEvent& event);
	//! Handles the corresponding GUI event.
	void OnButtonUpClick(wxCommandEvent& event);
	//! Handles the corresponding GUI event.
	void OnButtonDownClick(wxCommandEvent& event);
	//! Handles the corresponding GUI event.
	void OnListItemSelected(wxListEvent& event);

	// SwisTrackCoreInterface methods
	void OnBeforeStart(bool seriousmode) {};
	void OnAfterStart(bool seriousmode);
	void OnBeforeStop() {};
	void OnAfterStop();
	void OnBeforeStep() {};
	void OnStepReady() {};
	void OnAfterStep();
	void OnBeforeEdit() {};
	void OnAfterEdit();

private:
	//! GUI constants.
	enum eConstants {
		eID_ButtonAdd,
		eID_ButtonRemove,
		eID_ButtonUp,
		eID_ButtonDown,
		eID_List,
	};

	//! The corresponding GUI widget.
	wxListCtrl *mList;
	//! The column number of the message.
	int mColumnMessages;
	//! The corresponding GUI widget.
	wxButton *mButtonAdd;
	//! The corresponding GUI widget.
	wxButton *mButtonRemove;
	//! The corresponding GUI widget.
	wxButton *mButtonUp;
	//! The corresponding GUI widget.
	wxButton *mButtonDown;

	//! The component selector dialog used when adding a new component.
	ComponentsDialog *mComponentsDialog;

	DECLARE_EVENT_TABLE()
};

#endif
