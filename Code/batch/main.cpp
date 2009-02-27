#include "main.h"
#include "ConfigurationReaderXML.h"
#include "SwisTrackCore.h"
#include "NMEALog.h"
#include <stdio.h>
#include <wx/filename.h>

int main(int argc, char **argv) {
	if (argc == 2) {
		return runBatch(argv[1]);
	} else {
		help();
		return -1;
	}
}

void help() {
	wxPrintf(wxT("SwisTrack 4 - batch processing.\n"));
	wxPrintf(wxT("Usage: SwisTrack-batch FILENAME\n"));
	wxPrintf(wxT("For detailed information about SwisTrack and its command line options please consult the SwisTrack Wikibook.\n"));
}

int runBatch(char *filename_str) {
	wxPrintf(wxT("SwisTrack 4 - batch processing."));

	// Initialize the core
	SwisTrackCore *core = new SwisTrackCore(wxT("./Components"));
	core->mCommunicationInterface = new NMEALog();

	// Open the file
	wxFileName filename(wxString::FromAscii(filename_str));
	if (! filename.IsFileReadable()) {
		wxFprintf(stderr, wxT("Could not read ") + filename.GetFullPath());
		return -1;
	}
	ConfigurationReaderXML cr;
	if (! cr.Open(filename)) {
		wxFprintf(stderr, wxT("Could not read ") + filename.GetFullPath() + wxT(" Syntax error?"));
		return -1;
	}

	// Read the components
	wxFileName::SetCwd(wxT(".."));
	cr.ReadComponents(core);
	if (cr.mErrorList.mList.size() != 0) {
		wxFprintf(stderr, wxT("The following errors occured while reading ") + filename.GetFullPath() + wxT("\n"));
		for (ErrorList::tList::iterator it = cr.mErrorList.mList.begin(); it != cr.mErrorList.mList.end(); it++) {
			std::cerr << "Line " << it->mLineNumber << ": " << it->mMessage.ToAscii() << std::endl;
		}
		return -1;
	}

	// Check if there are any components
	if (core->GetDeployedComponents()->size() == 0) {
		wxFprintf(stderr, wxT("The configuration file is empty (no components).\n"));
		return -1;
	}
	wxPrintf(wxT("Running with %d components.\n"), core->GetDeployedComponents()->size());

	// Start execution
	bool has_error = false;
	core->TriggerStart();
	core->Start(false);
	for (std::list<Component*>::const_iterator it = core->GetDeployedComponents()->begin(); it != core->GetDeployedComponents()->end(); it++) {
		Component *component = *it;
		std::cout << component->mDisplayName.ToAscii() << ": " << (component->mStarted ? "started" : "not started") << std::endl;
		for (std::list<StatusItem>::iterator it = component->mStatus.begin(); it != component->mStatus.end(); it++) {
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
		for (std::list<Component*>::const_iterator it = core->GetDeployedComponents()->begin(); it != core->GetDeployedComponents()->end(); it++) {
			Component *component = *it;
			for (std::list<StatusItem>::iterator it = component->mStatus.begin(); it != component->mStatus.end(); it++) {
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
