#ifndef HEADER_Application
#define HEADER_Application

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

class Application;

#include "SwisTrack.h"

/** \class Application
* \brief Main application.
*/ 
class Application: public wxApp {

public:
	//! Runs the program.
	bool OnInit();

	//! The SwisTrack 
	SwisTrack* mSwisTrack;
};
#endif
