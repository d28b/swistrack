#ifndef HEADER_ConfigurationParameter
#define HEADER_ConfigurationParameter

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

class ConfigurationParameter;

#include "SwisTrackCore.h"
#include <wx/panel.h>
#include <wx/listctrl.h>
#include <wx/button.h>

class ConfigurationParameter: public wxPanel, public SwisTrackCoreInterface {

public:
	//! The associated SwisTrackCore object.
	SwisTrackCore *mSwisTrackCore;
	//! The component.
	Component *mComponent;
	//! The component.
	Component *mComponent;

	//! Constructor.
	ConfigurationParameter(wxWindow *parent, SwisTrackCore *stc);
	//! Destructor.
	~ConfigurationParameter();

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
	void OnBeforeStart(bool productivemode);
	void OnAfterStart(bool productivemode) {};
	void OnBeforeStop() {};
	void OnAfterStop();
	void OnBeforeStep() {};
	void OnStepReady() {};
	void OnAfterStep() {};
	void OnBeforeEdit() {};
	void OnAfterEdit() {};

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
