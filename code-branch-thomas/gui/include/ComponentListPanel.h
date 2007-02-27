#ifndef HEADER_ComponentListPanel
#define HEADER_ComponentListPanel

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

class ComponentListPanel;

#include "SwisTrack.h"
#include "ComponentsDialog.h"
#include <wx/panel.h>
#include <wx/listctrl.h>
#include <wx/button.h>

class ComponentListPanel: public wxPanel, public SwisTrackCoreInterface {

public:
	SwisTrack *mSwisTrack;				//!< The associated SwisTrack object.
	Component *mSelectedComponent;		//!< The selected component.

	//! Constructor.
	ComponentListPanel(wxWindow *parent, SwisTrack *st);
	//! Destructor.
	~ComponentListPanel();

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
	//! Handles the corresponding GUI event.
	void OnListItemDeselected(wxListEvent& event);

	// SwisTrackCoreInterface methods
	void OnBeforeStart(bool productivemode) {};
	void OnAfterStart(bool productivemode);
	void OnBeforeStop() {};
	void OnAfterStop();
	void OnBeforeStep() {};
	void OnStepReady() {};
	void OnAfterStep();
	void OnBeforeEdit() {};
	void OnAfterEdit();
	void OnBeforeEditComponent(Component *c) {};
	void OnAfterEditComponent(Component *c) {};

private:
	//! GUI constants.
	enum eConstants {
		eID_ButtonAdd,
		eID_ButtonRemove,
		eID_ButtonUp,
		eID_ButtonDown,
		eID_List,
	};

	wxListCtrl *mList;			//!< The corresponding GUI widget.
	int mColumnMessages;		//!< The column number of the messages.
	wxButton *mButtonAdd;		//!< The corresponding GUI widget.
	wxButton *mButtonRemove;	//!< The corresponding GUI widget.
	wxButton *mButtonUp;		//!< The corresponding GUI widget.
	wxButton *mButtonDown;		//!< The corresponding GUI widget.

	ComponentsDialog *mComponentsDialog;	//!< The component selector dialog used when adding a new component.

	DECLARE_EVENT_TABLE()
};

#endif
