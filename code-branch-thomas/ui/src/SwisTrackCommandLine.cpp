#include "SwisTrackCommandLine.h"
#define THISCLASS SwisTrackCommandLine

//#include <sys/select.h>
#include <iostream>
#include "SwisTrackCore.h"
#include "ConfigurationReaderXML.h"

THISCLASS::SwisTrackCommandLine():
		mSwisTrackCore(0), mExeName(),
		mFileName(), mHelp(false), mVersion(false) {

}

int THISCLASS::Run(int argc, char *argv[]) {
	// Parse the command line
	if (! ParseCommandLine(argc, argv)) {
		return 1;
	}

	// Help command: print help and quit
	if (mHelp) {
		PrintHelp();
		return 0;
	}

	// Version command: print version and quit
	if (mVersion) {
		PrintVersion();
	}

	// Check if a file name is provided
	if (mFileName=="") {
		std::cerr << "Usage: " << mExeName << " FILENAME" << std::endl;
		return 1;
	}

	// Create a new SwisTrackCore object and open the file
	mSwisTrackCore=new SwisTrackCore();
	OpenFile(mFileName);

	// Execute
	mSwisTrackCore->Start(true);
	while (1) {
		//fd_set fhset;
		//FD_ZERO(&fhset);
		//FD_SET(0, &fhset);

		//struct timeval timeout;
		//timeout.tv_sec=mTriggerInterval / 1000;
		//timeout.tv_usec=(mTriggerInterval-timeout.tv_sec*1000)*1000;
		//int res = select(1, &fhset, NULL, NULL, &timeout);
		int res=0;

		if (res>0) {
			break;
		} else {
			mSwisTrackCore->Step();
		}
	}

	mSwisTrackCore->Stop();
	return 0;
}

bool THISCLASS::ParseCommandLine(int argc, char *argv[]) {
	mExeName=argv[0];

	for (int i=1; i<argc; i++) {
		std::string arg=argv[i];
		if ((arg=="--help") || (arg=="-h")) {
			mHelp=true;
			return true;
		} else if ((arg=="--version") || (arg=="-v")) {
			mVersion=true;
			return true;
		} else {
			if (mFileName!="") {
				std::cerr << "Only one file name is allowed" << std::endl;
				return false;
			}
			mFileName=arg;
		}
	}

	return true;
}

void THISCLASS::PrintHelp() {
	std::cout << "SwisTrack Command Line Tool 4.0" << std::endl;
	std::cout << std::endl;
	std::cout << "Usage: " << mExeName << " [OPTIONS] FILENAME" << std::endl;
	std::cout << "Options:" << std::endl;
	std::cout << "  -h  --help     Displays this help screen and quits" << std::endl;
	std::cout << "  -v  --version  Displays the version and quits" << std::endl;
	std::cout << std::endl;
	std::cout << "More information about SwisTrack are available on swistrack.sourceforge.net." << std::endl;
}

void THISCLASS::PrintVersion() {
	std::cout << "SwisTrack Command Line Tool 4.0" << std::endl;
}

bool THISCLASS::OpenFile(const std::string &filename) {
	ConfigurationReaderXML cr;

	if (! cr.Open(filename)) {
		std::cerr << "The file \'" << filename << "\' could not be loaded. Syntax error?" << std::endl;
		return false;
	}

	// Read the configuration
	mSwisTrackCore->Stop();
	cr.ReadComponents(mSwisTrackCore);
	mTriggerInterval=cr.ReadTriggerInterval(0);
	if (cr.mErrorList.mList.empty()) {return true;}

	// Display the errors
	std::cerr << "The following errors occured while reading the XML file:" << std::endl;
	ErrorList::tList::iterator it=cr.mErrorList.mList.begin();
	while (it!=cr.mErrorList.mList.end()) {
		std::cerr << "  Line " << it->mLineNumber << ": " << it->mMessage << std::endl;		
	}
	return false;
}
