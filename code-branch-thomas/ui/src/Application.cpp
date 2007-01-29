//! Command line interface for SwisTrack
/*!
	This is a command line user interface for SwisTrack. It reads a configuration and executes it in serious mode.
*/

#include "SwisTrackCommandLine.h"

int main(int argc, char *argv[]) {
	SwisTrackCommandLine stcl;
	return stcl->Run(argc, argv);
}
