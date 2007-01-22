#ifndef HEADER_SwisTrackCommandLine
#define HEADER_SwisTrackCommandLine

class SwisTrackCommandLine;

#include "SwisTrackCore.h"

//! The SwisTrack command line interface.
class SwisTrackCommandLine {

public:
	mSwisTrackCore *mSwisTrackCore;		//!< The SwisTrackCore object.
	std::string mExeName;				//!< The name of the executable file.
	std::string mFileName;				//!< The name of the file to open.
	bool mHelp;							//!< True if the --help option was given on the command line. 
	bool mVersion;						//!< True if the --version option was given on the command line. 
	
	//! Constructor.
	SwisTrackCommandLine();
	//! Destructor.
	~SwisTrackCommandLine() {}

	//! Runs the program.
	int Run(int argc, char *argv[]);

	//! Parse the command line.
	bool ParseCommandLine(int argc, char *argv[]);

};

#endif
