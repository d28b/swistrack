#ifndef HEADER_SimulationParticles
#define HEADER_SimulationParticles

#include <string>
#include <list>
#include <fstream>
#include "DataStructureParticles.h"
#include "CommunicationNMEAInterface.h"

//! Reads and holds a simulation of particles.
class SimulationParticles: public CommunicationNMEAInterface {

public:
	//! Information about one frame.
	typedef struct {
		int number;
		DataStructureParticles::tParticleVector particles;
	} tFrame;
	//! A list of frames type.
	typedef std::list<tFrame> tFrameList;

	tFrameList mFrames;			//!< The frames.
	tFrame mFrameRead;			//!< The frame that is currently being read.

	//! Constructor.
	SimulationParticles(const std::string &filename);
	//! Destructor.
	~SimulationParticles();

	//! Returns the file name of the current simulation.
	std::string GetFileName() {return mFileName;}
	//! Returns true if the file could be opened.
	bool IsOpen() {return mFile->is_open();}

	//! Moves to the first frame in the file and returns it. If the file is empty, 0 is returned.
	tFrame *FirstFrame();
	//! Moves to the next frame in the file and returns it. At the end of the file, 0 is returned.
	tFrame *NextFrame();
	//! Returns the current frame (the same as the last call to FirstFrame() or NextFrame()).
	tFrame *GetCurrentFrame();
	//! Moves to the next frame with the given number and returns a pointer to it. This function must be called with increasing frame numbers, but frame numbers may be left out. If the frame with this number doesn't exist, an empty frame with that number is returned.
	tFrame *GetFutureFrameByNumber(int number);

protected:
	std::string mFileName;				//!< The name of the file currently open.
	std::ifstream *mFile;				//!< The file.
	tFrameList::iterator mCurrentFrame;	//!< The current frame.
	tFrame mEmptyFrame;					//!< Used if a frame was not available in the simulation file.

	// CommuncationNMEAInterface methods.
	void OnNMEAProcessMessage(CommunicationMessage *m, bool withchecksum);
	void OnNMEAProcessMessageChecksumError(CommunicationMessage *m);
	void OnNMEAProcessUnrecognizedChar(unsigned char chr);
	void OnNMEASend(const std::string &str);

	//! Reads the next block in the file.
	bool ReadBlock();
};

#endif