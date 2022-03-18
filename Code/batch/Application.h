#ifndef HEADER_Application
#define HEADER_Application
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class Application;

//! Application
class Application: public wxAppConsole {

public:
	//! Called upon program initialization.
	bool OnInit();
	//! Called to run the main body of the program.
	int MainLoop();
	//! Called upon program termination.
	int OnExit();

	//! Executes a SwisTrack configuration file in batch mode.
	int RunBatch(const wxString filename_str);
	//! Displays help.
	void Help();
};

DECLARE_APP(Application)

#endif
