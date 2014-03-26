#include "SimulationParticles.h"
#define THISCLASS SimulationParticles

#include <fstream>
#include <algorithm>

THISCLASS::SimulationParticles(const wxFileName &filename):
		mFrames(), mFrameRead(), mFileName(filename), mFile(0),
		mCurrentFrame(mFrames.end()), mEmptyFrame() {

	// Initialize structures
	mFrameRead.number = 0;
	mFrameRead.particles.clear();
	mCurrentFrame = mFrames.end();

	// Open file
	mFile = new std::ifstream();
	mFile->clear();
	mFile->open(filename.GetFullPath().mb_str(wxConvFile), std::ios::in);
}

THISCLASS::~SimulationParticles() {
	delete mFile;
}

void THISCLASS::OnNMEAProcessMessage(CommunicationMessage *m, bool withchecksum) {
	// We process known messages only (and discard other messages without warning)
	if (m->mCommand == wxT("STEP_START")) {
		mFrameRead.number = 0;
		mFrameRead.particles.clear();
	} else if (m->mCommand == wxT("FRAMENUMBER")) {
		mFrameRead.number = m->PopInt(0);
	} else if (m->mCommand == wxT("PARTICLE")) {
		Particle p;
		p.mID = m->PopInt(0);
		p.mCenter.x = (float)m->PopDouble(0);
		p.mCenter.y = (float)m->PopDouble(0);
		p.mOrientation = (float)m->PopDouble(0);
		p.mWorldCenter.x = (float)m->PopDouble(0);
		p.mWorldCenter.y = (float)m->PopDouble(0);
		mFrameRead.particles.push_back(p);
	} else if (m->mCommand == wxT("STEP_STOP")) {
		mFrames.push_back(mFrameRead);
	}
}

void THISCLASS::OnNMEAProcessMessageChecksumError(CommunicationMessage *m) {
	// Don't do anything for now
	// TODO: such incidents should be logged or at least counted somewhere
}

void THISCLASS::OnNMEAProcessUnrecognizedChar(unsigned char chr) {
	// Don't do anything, just ignore such chars
}

void THISCLASS::OnNMEASend(const char *buffer, int len) {
	// We don't write
}

bool THISCLASS::ReadBlock() {
	if (! mFile->is_open()) {
		return false;
	}

	char buffer[1024];
	mFile->read(buffer, sizeof(buffer));
	int len = mFile->gcount();
	if (len < 1) {
		return false;
	}
	NMEAProcessData(buffer, len);
	return true;
}

THISCLASS::Frame *THISCLASS::FirstFrame() {
	while (1) {
		// Move to the first frame
		mCurrentFrame = mFrames.begin();

		// Found? Great ...
		if (mCurrentFrame != mFrames.end()) {
			return &*mCurrentFrame;
		}

		// Read the next block in the file and quit if we are at the end of the file
		if (! ReadBlock()) {
			return 0;
		}
	}
}

THISCLASS::Frame *THISCLASS::NextFrame() {
	while (1) {
		// Try incrementing
		tFrameList::iterator nextframe = mCurrentFrame;
		nextframe++;

		// Found? Great ...
		if (nextframe != mFrames.end()) {
			mCurrentFrame = nextframe;
			return &*mCurrentFrame;
		}

		// Read the next block in the file and quit if we are at the end of the file
		if (! ReadBlock()) {
			mCurrentFrame = mFrames.end();
			return 0;
		}
	}
}

THISCLASS::Frame *THISCLASS::GetCurrentFrame() {
	if (mCurrentFrame == mFrames.end()) {
		return 0;
	}
	return &*mCurrentFrame;
}

THISCLASS::Frame *THISCLASS::GetFutureFrameByNumber(int number) {
	while (mCurrentFrame != mFrames.end()) {
		if (mCurrentFrame->number == number) {
			return &*mCurrentFrame;
		}
		if (mCurrentFrame->number > number) {
			break;
		}
		if (! NextFrame()) {
			break;
		}
	}

	// If no frame is available for this frame number, pretend to have a frame with no particles
	mEmptyFrame.number = number;
	return &mEmptyFrame;
}

