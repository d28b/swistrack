#include "Application.h"
#define THISCLASS Application

#include "ConfigurationReaderXML.h"
#include "SwisTrackCore.h"
#include "NMEALog.h"
#include <stdio.h>
#include <iostream>
#include <wx/filename.h>
using namespace std;

IMPLEMENT_APP_CONSOLE(Application)

bool THISCLASS::OnInit() {
	// Set some main application parameters.
	SetVendorName(wxT("SwisTrack Community"));
	SetAppName(wxT("SwisTrack"));
	return true;
}

int THISCLASS::MainLoop() {
	// Run if a filename is provided

	if (argc == 2) {
		RunBatch(argv[1]);
	} else {
		Help();
	}

	return 0;
}

int THISCLASS::OnExit() {
	return 0;
}

void THISCLASS::Help() {
	wxPrintf(wxT("SwisTrack 4 - batch processing.\n"));
	wxPrintf(wxT("Usage: SwisTrack-batch FILENAME\n"));
	wxPrintf(wxT("For detailed information about SwisTrack and its command line options please consult the SwisTrack Wikibook.\n"));
}

int THISCLASS::RunBatch(const wxString filenameString) {
	// Initialize the core
	SwisTrackCore * core = new SwisTrackCore(wxT("./Components"));
	core->mCommunicationInterface = new NMEALog(wxT("swistrack.log"));

	// Open the file
	wxFileName filename(filenameString);
	if (! filename.IsFileReadable()) {
		wxPrintf(wxT("Could not read %s.\n"), filename.GetFullPath());
		return -1;
	}

	ConfigurationReaderXML cr;
	if (! cr.Open(filename)) {
		wxPrintf(wxT("Could not parse %s. Syntax error?\n"), filename.GetFullPath());
		return -1;
	}

	// Read the components
	cr.ReadComponents(core);
	if (cr.mErrorList.mList.size() != 0) {
		wxPrintf(wxT("The following errors occured while reading %s:\n"), filename.GetFullPath());
		for (ErrorList::tList::iterator it = cr.mErrorList.mList.begin(); it != cr.mErrorList.mList.end(); it++)
			wxPrintf(wxT("Line %d: %s\n"), it->mLineNumber, it->mMessage);
		return -1;
	}

	// Check if there are any components
	if (core->GetDeployedComponents()->size() == 0) {
		wxPrintf(wxT("The configuration file is empty (no components).\n"));
		return -1;
	}

	wxPrintf(wxT("Running with %ld components.\n"), core->GetDeployedComponents()->size());

	// Set the folder
	core->SetFileName(filename);

	// Start execution
	bool has_error = false;
	core->TriggerStart();
	core->Start(false);
	for (auto component : *core->GetDeployedComponents()) {
		wxPrintf(wxT("%s: %s\n"), component->mDisplayName, component->mStarted ? "started" : "not started");
		for (auto & status : component->mStatus) {
			if (status.mType == StatusItem::sTypeError) {
				wxPrintf(wxT("  Error: %s\n"), status.mMessage);
				has_error = true;
			} else if (status.mType == StatusItem::sTypeWarning) {
				wxPrintf(wxT("  Warning: %s\n"), status.mMessage);
			} else {
				wxPrintf(wxT("  Info: %s\n"), status.mMessage);
			}
		}
	}

	// Stop on error
	if (has_error) return -1;

	// Process frames until an error occurs
	while (core->IsTriggerActive()) {
		// Process the next frame
		core->Step();

		// Print warnings and errors
		for (auto component : *core->GetDeployedComponents()) {
			for (auto & status : component->mStatus) {
				if (status.mType == StatusItem::sTypeError) {
					wxPrintf(wxT("%s: Error: %s\n"), component->mDisplayName, status.mMessage);
					has_error = true;
				} else if (status.mType == StatusItem::sTypeWarning) {
					wxPrintf(wxT("%s: Warning: %s\n"), component->mDisplayName, status.mMessage);
				} else {
					wxPrintf(wxT("%s: Info: %s\n"), component->mDisplayName, status.mMessage);
				}
			}
		}

		// Stop on error
		if (has_error) return -1;
	}

	// Stop and quit
	core->Stop();
	delete core->mCommunicationInterface;
	delete core;
	return 0;
}
