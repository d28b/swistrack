#ifndef HEADER_SwisTrackCommandLine
#define HEADER_SwisTrackCommandLine

class SwisTrackCommandLine;

#include "SwisTrackCore.h"

//! The SwisTrack command line interface.
class SwisTrackCommandLine {

public:
	// Variables read from the command line
	std::string mExeName;				//!< The name of the executable file.
	std::string mFileName;				//!< The name of the file to open.
	bool mHelp;							//!< True if the --help option was given on the command line.
	bool mVersion;						//!< True if the --version option was given on the command line.

	// SwisTrackCore and configuration
	SwisTrackCore *mSwisTrackCore;		//!< The SwisTrackCore object.
	int mTriggerInterval;				//!< The interval of the trigger in milliseconds. If 0, manual trigger is chosen.

	//! Constructor.
	SwisTrackCommandLine();
	//! Destructor.
	~SwisTrackCommandLine() {}

	//! Runs the program.
	int Run(int argc, char *argv[]);

	//! Parse the command line.
	bool ParseCommandLine(int argc, char *argv[]);
	//! Prints the help screen.
	void PrintHelp();
	//! Prints version information.
	void PrintVersion();

	//! Opens a file.
	bool OpenFile(const std::string &filename);
};

#endif
