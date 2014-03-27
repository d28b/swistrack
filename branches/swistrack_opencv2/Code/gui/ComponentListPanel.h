#ifndef HEADER_ComponentListPanel
#define HEADER_ComponentListPanel

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class ComponentListPanel;

#include "SwisTrack.h"
#include "SwisTrackCoreInterface.h"
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

	// SwisTrackCoreInterface methods
	void OnAfterStart(bool productionmode);
	void OnAfterStop();
	void OnAfterStep();
	void OnAfterEdit();

private:
	//! GUI constants.
	enum eConstants {
		cID_ButtonAdd = 1,
		cID_ButtonRemove,
		cID_ButtonUp,
		cID_ButtonDown,
		cID_List,
		cID_PopupMenu_EnabledInterval0,
		cID_PopupMenu_EnabledInterval1,
		cID_PopupMenu_EnabledInterval2,
		cID_PopupMenu_EnabledInterval4,
		cID_PopupMenu_EnabledInterval8,
		cID_PopupMenu_EnabledInterval16,
		cID_PopupMenu_EnabledInterval32,
		cID_PopupMenu_EnabledInterval64,
		cID_PopupMenu_EnabledInterval128,
	};

	wxMenu mPopupMenu;					//!< The popup menu.
	wxListCtrl *mList;					//!< The corresponding GUI widget.
	int mColumnMessages;				//!< The column number of the messages.
	int mColumnEnabledInterval;			//!< The column number of the enabled interval.
	int mColumnStepDuration;			//!< The column number of the step duration.
	wxBitmapButton *mButtonAdd;			//!< The corresponding GUI widget.
	wxBitmapButton *mButtonRemove;		//!< The corresponding GUI widget.
	wxBitmapButton *mButtonUp;			//!< The corresponding GUI widget.
	wxBitmapButton *mButtonDown;		//!< The corresponding GUI widget.

	ComponentsDialog *mComponentsDialog;	//!< The component selector dialog used when adding a new component.

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
	//! Handles the corresponding GUI event.
	void OnListLeftDoubleClick(wxMouseEvent& event);
	//! Handles the corresponding GUI event.
	void OnListMouseRightDown(wxMouseEvent& event);
	//! The corresponding GUI event handler.
	void OnPopupMenuEnabledInterval(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()
};

#endif
