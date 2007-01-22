#include "SwisTrackCommandLine.h"
#define THISCLASS SwisTrackCommandLine

#include "SwisTrackCore.h"

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
	OpenFile();
	
	// Execute
	mSwisTrackCore->Start(true);
	while (1) {
		select();
	}

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

bool THISCLASS::OpenFile() {
	SwisTrackCoreEditor stce(mSwisTrackCore);
	if (! stce.IsEditable()) {return false;}

	// Read the new file
	xmlpp::DomParser parser;
	xmlpp::Document *document=0;
	try {
		parser.parse_file(mFileName.c_str());
		if (parser==true) {
			document=parser.get_document();
		}
	} catch (...) {
		document=0;
	}

	if (document==0) {
		std::cerr << "The file \'" << mFileName << "\' could not be loaded. Syntax error?" << std::endl;
		return false;
	}

	// Read the configuration
	ErrorList errorlist;
	ConfigurationReadXML(&stce, document, &errorlist);
	if (errorlist.mList.empty()) {return true;}

	// Display the errors
	std::cerr << "The following errors occured while reading the XML file:" << std::endl;
	errorlist.tList::iterator it=errorlist.mList.begin();
	while (it!=errorlist.mList.end()) {
		std::cerr << "  Line " << (*it).mLine << ": " << (*it).mMessage << std::endl;		
	}
	return false;
}

void THISCLASS::ConfigurationReadXML(SwisTrackCoreEditor *stce, xmlpp::Document *document, ErrorList *errorlist) {
	// If this method is called without a document, set up an empty component list
	if (document==0) {
		stce->ConfigurationReadXML(0, errorlist);
		return;
	}

	// Get the root node
	xmlpp::Element *rootnode=document->get_root_node();
	if (! rootnode) {
		stce->ConfigurationReadXML(0, errorlist);
		return;
	}

	// Get the list of components
	xmlpp::Node::NodeList nodes_components=rootnode->get_children("Components");
	if (nodes_components.empty()) {
		stce->ConfigurationReadXML(0, errorlist);
		return;
	}

	// Read the component list
	xmlpp::Element *components=dynamic_cast<xmlpp::Element *>(nodes_components.front());
	stce->ConfigurationReadXML(components, errorlist);
}
