#include "main.h"
#include "ConfigurationReaderXML.h"
#include "SwisTrackCore.h"
#include "Random.h"
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
	std::cout << "SwisTrack 4 - batch processing." << std::endl;
	std::cout << "Usage: SwisTrack-batch FILENAME" << std::endl;
	std::cout << "For detailed information about SwisTrack and its command line options please consult the SwisTrack Wikibook." << std::endl;
}

int runBatch(char *filename) {
	std::cout << "SwisTrack 4 - batch processing." << std::endl;

	// Initialize the core
	SwisTrackCore * core = new SwisTrackCore(wxT("./Components"));

	// Open the file
	wxFileName fn(wxString::FromAscii(filename));
	if (! fn.IsFileReadable()) {
		std::cout << "Could not read " << fn.GetFullPath().ToAscii() << std::endl;
		return -1;
	}
	ConfigurationReaderXML cr;
	if (! cr.Open(fn.GetFullPath())) {
		std::cout << "Could not read " << fn.GetFullPath().ToAscii() << " Syntax error? " << std::endl;
		return -1;
	}

	// Read the components
	wxFileName::SetCwd(wxT(".."));
	cr.ReadComponents(core);
	if (cr.mErrorList.mList.size() != 0) {
		std::cout << "The following errors occured while reading " << filename << std::endl;
		for (ErrorList::tList::iterator it = cr.mErrorList.mList.begin(); it != cr.mErrorList.mList.end(); it++) {
			std::cout << "Line " << it->mLineNumber << ": " << it->mMessage.ToAscii() << std::endl;
		}
		return -1;
	}

	// Check if there are any components
	if (core->GetDeployedComponents()->size() == 0) {
		std::cout << "No available components." << std::endl;
		return -1;
	}
	std::cout << "Running with " << core->GetDeployedComponents()->size() << " components." << std::endl;

	// Start execution
	bool has_error = false;
	core->TriggerStart();
	core->Start(false);
	for (std::list<Component*>::const_iterator it = core->GetDeployedComponents()->begin(); it != core->GetDeployedComponents()->end(); it++) {
		Component *component = *it;
		std::cout << component->mDisplayName.ToAscii() << ": " << (component->mStarted ? "started" : "not started") << std::endl;
		for (std::list<StatusItem>::iterator it = component->mStatus.begin(); it != component->mStatus.end(); it++) {
			if (it->mType == StatusItem::sTypeError) {
				std::cout << "  Error: " << it->mMessage.ToAscii() << std::endl;
				has_error = true;
			} else if (it->mType == StatusItem::sTypeWarning) {
				std::cout << "  Warning: " << it->mMessage.ToAscii() << std::endl;
			} else {
				std::cout << "  Info: " << it->mMessage.ToAscii() << std::endl;
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
					std::cout << component->mDisplayName.ToAscii() << ": Error: " << it->mMessage.ToAscii() << std::endl;
					has_error = true;
				} else if (it->mType == StatusItem::sTypeWarning) {
					std::cout << component->mDisplayName.ToAscii() << ": Warning: " << it->mMessage.ToAscii() << std::endl;
				} else {
					std::cout << component->mDisplayName.ToAscii() << ": Info: " << it->mMessage.ToAscii() << std::endl;
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
	return 0;
}
