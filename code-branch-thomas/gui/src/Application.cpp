#include "Application.h"
#define THISCLASS Application

#include <wx/app.h>
#include <wx/image.h>
#include <wx/filefn.h>

IMPLEMENT_APP(Application)

bool THISCLASS::OnInit() {
	// Set some main application parameters.
	SetVendorName(wxT("Swarm-Intelligent Systems Group, EPFL, Lausanne, Switzerland"));
	SetAppName(wxT("SwisTrack"));

	// Take the current directory as the application folder
	mApplicationFolder=wxGetCwd();

	// Initialize all available image handlers
	wxInitAllImageHandlers();

	// Create the main application window
	mSwisTrack = new SwisTrack(_T("SwisTrack"), wxPoint(-1, -1), wxSize(800, 600));

	// Open a file
	if (argc>1) {
		mSwisTrack->OpenFile(argv[1], true, false);
	} else {
		mSwisTrack->OpenFile(mApplicationFolder+"/default.swistrack", false, true);
	}

#if defined(__WIN16__) || defined(__WXMOTIF__)
	int width, height;
	frame->GetSize(&width, &height);
	frame->SetSize(-1, -1, width, height);
#endif

	// Show
    mSwisTrack->Show(TRUE);
	SetTopWindow(mSwisTrack);
    return TRUE;
}

int THISCLASS::OnExit() {
	//delete mSwisTrack;
	return 0;
}
