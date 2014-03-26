#include "Application.h"
#define THISCLASS Application
#include <wx/app.h>
#include <wx/image.h>
#include <wx/filefn.h>

IMPLEMENT_APP(Application)

bool THISCLASS::OnInit() {
	// Set some main application parameters.
	SetVendorName(wxT("Distributed Intelligent Systems and Algorithms Laboratory, EPFL, Lausanne, Switzerland"));
	SetAppName(wxT("SwisTrack"));

	// Initialize all available image handlers
	wxInitAllImageHandlers();

	// Create the main application window
	mSwisTrackButton = new SwisTrackButton(wxT("SwisTrackButton"), wxPoint(-1, -1), wxSize(400, 200));

	// Show
	mSwisTrackButton->Show(true);
	SetTopWindow(mSwisTrackButton);

	return true;
}

int THISCLASS::OnExit() {
	return 0;
}
