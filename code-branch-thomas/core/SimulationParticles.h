#ifndef HEADER_SimulationParticles
#define HEADER_SimulationParticles

#include <string>
#include <list>
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

	tFrameList mFrames;					//!< The frames.

	//! Constructor.
	SimulationParticles(): mFileName(""), mFrames(), mCurrentFrame(mFrames.end()), mEmptyFrame() {}
	//! Destructor.
	~SimulationParticles() {}

	//! Reads a simulation file.
	bool Read(const std::string &filename);
	//! Returns the file name of the current simulation.
	std::string GetFileName() {return mFileName;}

	//! Moves to the first frame and returns a pointer to it.
	tFrame *FirstFrame();
	//! Moves to the next frame with the given number and returns a pointer to it. This function must be called with increasing frame numbers, but frame numbers may be left out.
	tFrame *NextFrame(int number);

protected:
	std::string mFileName;				//!< The name of the file currently open.
	tFrameList::iterator mCurrentFrame;	//!< The current frame.
	tFrame mEmptyFrame;				//!< Used if a frame was not available in the simualtion file.

	// CommuncationNMEAInterface methods.
	void OnNMEAProcessMessage(CommunicationMessage *m, bool withchecksum);
	void OnNMEAProcessMessageChecksumError(CommunicationMessage *m);
	void OnNMEAProcessUnrecognizedChar(unsigned char chr);
	void OnNMEASend(const std::string &str);

};

#endif