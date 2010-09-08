#ifndef HEADER_Application
#define HEADER_Application

#include <wx/wxprec.h>
#include "SwisTrackButton.h"

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class Application;

//! Application
class Application: public wxApp {

public:
	SwisTrackButton *mSwisTrackButton;	//!< The main window.

	//! Called upon program initialization.
	bool OnInit();
	//! Called upon program termination.
	int OnExit();

};

DECLARE_APP(Application)

#endif
