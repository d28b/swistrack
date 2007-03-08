#ifndef HEADER_IDList
#define HEADER_IDList

#include <string>
#include <list>
#include <fstream>
#include "DataStructureParticles.h"
#include "CommunicationNMEAInterface.h"

//! Reads and holds a simulation of particles.
class IDList: public ConfigurationXML {

public:
	typedef std::vector<int> tChipVector;
	//! Information about one ID.
	typedef struct {
		int robotid;			//!< The robot id.
		tChipVector chips;		//!< The corresponding chips. Note that chips are stored in clockwise order, starting at 12 o'clock (if angle=0).
		float angle;			//!< Correction angle, if the blob is rotated.
	} tID;
	//! A list of frames type.
	typedef std::list<tID> tIDList;

	int mLastRobotID;		//!< The last robot ID read from the file.
	tIDList mIDs;			//!< The IDs.
	std::string mError;		//!< The first error that occured.

	//! Constructor.
	IDList(const std::string &filename);
	//! Destructor.
	~IDList();

	//! Returns the file name of the current simulation.
	std::string GetFileName() {return mFileName;}
	//! Returns true if the file could be opened.
	bool IsOpen() {return mFile->is_open();}

protected:
	std::string mFileName;				//!< The name of the currently open file.

	// CommuncationNMEAInterface methods.
	void OnNMEAProcessMessage(CommunicationMessage *m, bool withchecksum);
	void OnNMEAProcessMessageChecksumError(CommunicationMessage *m);
	void OnNMEAProcessUnrecognizedChar(unsigned char chr);
	void OnNMEASend(const std::string &str);
};

#endif