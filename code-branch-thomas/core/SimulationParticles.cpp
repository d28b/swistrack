#include "SimulationParticles.h"
#define THISCLASS SimulationParticles

#include <fstream>
#include <algorithm>

THISCLASS::SimulationParticles():
		mFileName(""), mFile(0), mFrames(),
		mCurrentFrame(mFrames.end()), mEmptyFrame() {

	mFile=new std::ifstream();
}

THISCLASS::~SimulationParticles() {
	delete mFile;
}

bool THISCLASS::Read(const std::string &filename) {
	// Close the file if necessary and reset the state
	if (! mFile->is_open()) {mFile->close();}
	mFrames.clear();
	mCurrentFrame=mFrames.end();
	mFileName="";

	// Open file
	mFile->clear();
	mFile->open(filename.c_str(), std::ios::in);
	if (! mFile->is_open()) {return false;}

	// Set the new filename
	mFileName=filename;
	return true;
}

void THISCLASS::OnNMEAProcessMessage(CommunicationMessage *m, bool withchecksum) {
	if (m->mCommand=="BEGINFRAME") {
		tFrame frame;
		frame.number=m->GetInt(0);
		mFrames.push_back(frame);
	} else if (m->mCommand=="PARTICLE") {
		if (! mFrames.empty()) {
			Particle p;
			p.mID=m->GetInt(0);
			p.mCenter.x=(float)m->GetDouble(0);
			p.mCenter.y=(float)m->GetDouble(0);
			p.mOrientation=(float)m->GetDouble(0);
			mFrames.back().particles.push_back(p);
		}
	} else if (m->mCommand=="ENDFRAME") {
	}
}

void THISCLASS::OnNMEAProcessMessageChecksumError(CommunicationMessage *m) {
	// Don't do anything for now
	// TODO: such incidents should be logged or at least counted somewhere
}

void THISCLASS::OnNMEAProcessUnrecognizedChar(unsigned char chr) {
	// Don't do anything, just ignore such chars
}

void THISCLASS::OnNMEASend(const std::string &str) {
	// We don't write
}

bool THISCLASS::ReadBlock() {
	if (! mFile->is_open()) {return false;}

	char buffer[1024];
	mFile->read(buffer, sizeof(buffer));
	int len=mFile->gcount();
	if (len<1) {return false;}
	NMEAProcessData(buffer, len);
	return true;
}

THISCLASS::tFrame *THISCLASS::FirstFrame() {
	while (1) {
		// Move to the first frame
		mCurrentFrame=mFrames.begin();

		// Found? Great ...
		if (mCurrentFrame!=mFrames.end()) {
			return &*mCurrentFrame;
		}

		// Read the next block in the file and quit if we are at the end of the file
		if (! ReadBlock()) {
			return 0;
		}
	}
}

THISCLASS::tFrame *THISCLASS::NextFrame() {
	while (1) {
		// Try incrementing
		tFrameList::iterator nextframe=mCurrentFrame;
		nextframe++;

		// Found? Great ...
		if (nextframe!=mFrames.end()) {
			mCurrentFrame=nextframe;
			return &*mCurrentFrame;
		}

		// Read the next block in the file and quit if we are at the end of the file
		if (! ReadBlock()) {
			mCurrentFrame=mFrames.end();
			return 0;
		}
	}
}

THISCLASS::tFrame *THISCLASS::GetCurrentFrame() {
	if (mCurrentFrame==mFrames.end()) {return 0;}
	return &*mCurrentFrame;
}

THISCLASS::tFrame *THISCLASS::GetFutureFrameByNumber(int number) {
	while (mCurrentFrame!=mFrames.end()) {
		if (mCurrentFrame->number == number) {return &*mCurrentFrame;}
		if (mCurrentFrame->number > number) {break;}
		if (! NextFrame()) {break;}
	}

	// If no frame is available for this frame number, pretend to have a frame with no particles
	mEmptyFrame.number=number;
	return &mEmptyFrame;
}

