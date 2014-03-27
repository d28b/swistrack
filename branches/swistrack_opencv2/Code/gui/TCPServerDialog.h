#ifndef HEADER_TCPServerDialog
#define HEADER_TCPServerDialog

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

//! About dialog.
class TCPServerDialog: public wxDialog {

public:
	//! Constructor.
	TCPServerDialog(wxWindow *parent);
	//! Destructor.
	~TCPServerDialog() {}
};

#endif

