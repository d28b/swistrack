#include "SimulationParticles.h"
#define THISCLASS SimulationParticles

#include <fstream>
#include <algorithm>

bool THISCLASS::Read(const std::string &filename) {
	// Reset the data
	mFrames.clear();
	mCurrentFrame=mFrames.end();

	// Open file
	std::ifstream file(filename, std::ios::in);
	if (! file.is_open()) {return false;}

	// Read
	char buffer[1024];
	int len;
	while (file.read(buffer, sizeof(buffer))) {
		len=file.gcount();
		NMEAProcessData(buffer, len);
	}

	// Close
	file.close();

	// Sort the frames by their frame number.
	sort(mFrames.begin(), mFrames.end(), class{bool operator < (const tFrame &frame1, const tFrame &frame2) {return frame1.number<frame2.number;}})
}

void THISCLASS::OnNMEAProcessMessage(CommunicationMessage *m, bool withchecksum) {
	if (m->mCommand=="BEGINFRAME") {
		tFrame frame;
		frame.number=m->GetInteger(0);
		mFrames.push_back(frame);
	} else if (m->mCommand=="ROBOT") {
		if (! mFrames.isempty()) {
			Particle p;
			p.mID=m->GetInteger(0);
			p.mCenter.x=m->GetDouble(0);
			p.mCenter.y=m->GetDouble(0);
			p.mOrientation=m->GetDouble(0);
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

tFrame *THISCLASS::FirstFrame() {
	mCurrentFrame=mFrames.begin();
	if (mCurrentFrame==mFrames.end()) {
		mEmptyFrame.number=0;
		return &EmptyFrame;
	}

	return &(mCurrentFrame->value());
}

tFrame *THISCLASS::NextFrame(int number) {
	while (mCurrentFrame!=mFrames.end()) {
		if (mCurrentFrame->number == number) {return &(mCurrentFrame->value());}
		if (mCurrentFrame->number > number) {break;}
		mCurrentFrame++;
	}

	// If no frame is available for this frame number, pretend to have a frame with no particles
	mEmptyFrame.number=number;
	return &EmptyFrame;
}
