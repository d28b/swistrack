#ifndef HEADER_AboutDialog
#define HEADER_AboutDialog

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

//! About dialog.
class AboutDialog: public wxDialog {

public:
	//! Constructor.
	AboutDialog(wxWindow *parent);
	//! Destructor.
	~AboutDialog() {}
};

#endif

