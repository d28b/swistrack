#include "Application.h"
#define THISCLASS Application

#include "ConfigurationReaderXML.h"
#include "SwisTrackCore.h"
#include "NMEALog.h"
#include <stdio.h>
#include <iostream>
#include <wx/filename.h>
using namespace std;

IMPLEMENT_APP(Application)

int THISCLASS::OnRun() {
  // if you comment out this printf, then no other printfs or couts
  // work.  I have no idea why.  This is on Ubuntu Jaunty, and also os
  // X leopard.
  printf("SwisTrack 4 - batch processing.\n");
  return MainLoop();
}
bool THISCLASS::OnInit() {
	// Set some main application parameters.
	SetVendorName(wxT("SwisTrack Community"));
	SetAppName(wxT("SwisTrack"));
	//SetExitOnFrameDelete(false);

	// Initialize all available image handlers
	//wxInitAllImageHandlers();

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

int THISCLASS::RunBatch(const wxString filename_str) {
	// Initialize the core
	SwisTrackCore *core = new SwisTrackCore(wxT("./Components"));
	core->mCommunicationInterface = new NMEALog(wxT("swistrack.log"));

	// Open the file
	wxFileName filename(filename_str);
	if (! filename.IsFileReadable()) {
		wxFprintf(stderr, wxT("Could not read ") + filename.GetFullPath() + wxT("\n"));
		return -1;
	}
	ConfigurationReaderXML cr;
	if (! cr.Open(filename)) {
		wxFprintf(stderr, wxT("Could not read ") + filename.GetFullPath() + wxT(" Syntax error?\n"));
		return -1;
	}

	// Read the components
	cr.ReadComponents(core);
	if (cr.mErrorList.mList.size() != 0) {
		wxFprintf(stderr, wxT("The following errors occured while reading ") + filename.GetFullPath() + wxT("\n"));
		for (ErrorList::tList::iterator it = cr.mErrorList.mList.begin(); it != cr.mErrorList.mList.end(); it++) {
			cerr << "Line " << it->mLineNumber << ": " << it->mMessage.ToAscii() << endl;
		}
		return -1;
	}
	// Check if there are any components
	if (core->GetDeployedComponents()->size() == 0) {
		wxFprintf(stderr, wxT("The configuration file is empty (no components).\n"));
		return -1;
	}
	wxPrintf(wxT("Running with %d components.\n"), core->GetDeployedComponents()->size());
	// Set the folder
	core->SetFileName(filename);

	// Start execution
	bool has_error = false;
	core->TriggerStart();
	core->Start(false);
	for (list<Component*>::const_iterator it = core->GetDeployedComponents()->begin(); it != core->GetDeployedComponents()->end(); it++) {
		Component *component = *it;

		cout << component->mDisplayName.ToAscii() << ": " << (component->mStarted ? "started" : "not started") << endl;
		for (list<StatusItem>::iterator it = component->mStatus.begin(); it != component->mStatus.end(); it++) {
			if (it->mType == StatusItem::sTypeError) {
				wxFprintf(stderr, wxT("  Error: ") + it->mMessage + wxT("\n"));
				has_error = true;
			} else if (it->mType == StatusItem::sTypeWarning) {
				wxFprintf(stderr, wxT("  Warning: ") + it->mMessage + wxT("\n"));
			} else {
				wxFprintf(stderr, wxT("  Info: ") + it->mMessage + wxT("\n"));
			}
		}
	}

	// Stop on error
	if (has_error) {
		return -1;
	}

	// Process frames until an error occurs
	while (core->IsTriggerActive()) {
		// Process the next frame
		core->Step();

		// Print warnings and errors
		for (list<Component*>::const_iterator it = core->GetDeployedComponents()->begin(); it != core->GetDeployedComponents()->end(); it++) {
			Component *component = *it;
			for (list<StatusItem>::iterator it = component->mStatus.begin(); it != component->mStatus.end(); it++) {
				if (it->mType == StatusItem::sTypeError) {
					wxFprintf(stderr, component->mDisplayName + wxT(": Error: ") + it->mMessage + wxT("\n"));
					has_error = true;
				} else if (it->mType == StatusItem::sTypeWarning) {
					wxFprintf(stderr, component->mDisplayName + wxT(": Warning: ") << it->mMessage + wxT("\n"));
				} else {
					wxFprintf(stderr, component->mDisplayName + wxT(": Info: ") << it->mMessage + wxT("\n"));
				}
			}
		}

		// Stop on error
		if (has_error) {
			return -1;
		}
	}

	// Stop and quit
	core->Stop();
	delete core->mCommunicationInterface;
	delete core;
	return 0;
}
