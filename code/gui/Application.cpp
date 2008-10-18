#include "Application.h"
#define THISCLASS Application
using namespace std;
#include <iostream>
#include <wx/app.h>
#include <wx/image.h>
#include <wx/filefn.h>
#include "Random.h"

IMPLEMENT_APP(Application)

bool THISCLASS::OnInit() {
	// Set some main application parameters.
	SetVendorName(wxT("Swarm-Intelligent Systems Group, EPFL, Lausanne, Switzerland"));
	SetAppName(wxT("SwisTrack"));

	// Take the current directory as the application folder
	mApplicationFolder = wxGetCwd();

	// Initialize the random number generator
	Random::Initialize();

	// Initialize all available image handlers
	wxInitAllImageHandlers();

	// Create the main application window
	mSwisTrack = new SwisTrack(_T("SwisTrack"), wxPoint(-1, -1), wxSize(1024, 700));

	// Open a file
	if (argc > 1) {
		mSwisTrack->OpenFile(argv[1], true, false);
	} else {
		mSwisTrack->OpenFile(mApplicationFolder + wxT("/default.swistrack"), false, true);
	}

	// Show
	mSwisTrack->Show(TRUE);
	SetTopWindow(mSwisTrack);
	if (argc == 3) {
	  SwisTrackCore & core = *mSwisTrack->mSwisTrackCore;
	  if (wxString(argv[2]) == wxT("--batch")) {
	    cout << "Starting batch processing " << endl;
	    core.TriggerStart();
	    while (core.IsTriggerActive()) {
	      core.Step();
	    }
	    core.Stop();
	    exit(0);
	  }
	}
	


#if defined(__WIN16__) || defined(__WXMOTIF__)
	int width, height;
	frame->GetSize(&width, &height);
	frame->SetSize(-1, -1, width, height);
#endif


	return TRUE;
}

int THISCLASS::OnExit() {
	// Uninitialize the random number generator
	Random::Uninitialize();

	return 0;
}
