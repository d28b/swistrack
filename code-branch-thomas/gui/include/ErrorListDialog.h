#ifndef HEADER_ErrorListDialog
#define HEADER_ErrorListDialog

class ErrorListDialog;

#include "SwisTrackCore.h"
#include <wx/dialog.h>
#include <wx/listctrl.h>
#include <wx/button.h>

class ErrorListDialog: public wxDialog {

public:
	//! The associated ErrorList object.
	ErrorList *mErrorList;

	//! Constructor.
	ErrorListDialog(wxWindow *parent, ErrorList *el);
	//! Destructor.
	~ErrorListDialog() {}

	//! Closes the dialog when the user clicks the OK button.
	void OnButtonOK(wxCommandEvent& event);
	//! Updates the list.
	void OnUpdate();

protected:
	enum eConstants {
		eID_ButtonOK
	};

private:
	wxListCtrl *mTree;
	wxButton *mButtonOK;

	DECLARE_EVENT_TABLE()
};

#endif
