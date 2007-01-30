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
	SwisTrack *mSwisTrack;		//!< The main window.

	//! Called upon program initialization.
	bool OnInit();
	//! Called upon program termination.
	int OnExit();

};
#endif
