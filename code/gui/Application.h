#ifndef HEADER_Application
#define HEADER_Application

#include <wx/wxprec.h>
#include "SwisTrack.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class Application;

//! Application
class Application: public wxApp {

public:
	SwisTrack *mSwisTrack;			//!< The main window.
	wxString mApplicationFolder;	//!< The application folder. The default file as well as all *.swistrackconfiguration files are read from this folder.

	//! Called upon program initialization.
	bool OnInit();
	//! Called upon program termination.
	int OnExit();

};

DECLARE_APP(Application)

#endif
