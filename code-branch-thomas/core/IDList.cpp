#include "IDList.h"
#define THISCLASS IDList

#include <fstream>
#include <algorithm>

THISCLASS::IDList(const std::string &filename):
		ConfigurationXML(0, true),
		mIDs(), mLastRobotID(0) {

	// Read the file
	wxLogNull log;
	wxXmlDocument document;
	isopen=document.Load(filename);
	if (! isopen) {
		mError="Could not open or parse the XML file!";
		return;
	}

	// Select the root element and check its name
	SetRootNode(document.GetRoot());
	if (GetRootNode()->GetName() != "particlelist") {
		mError="The XML root node must be called 'particlelist'!";
		return;
	}

	// Enumerate all robots in the list
	SelectChildNode("particles");
	if (! mSelectedNode) {
		mError="No node 'particles' found!";
		return;
	}

	wxXmlNode *node=mSelectedNode->GetChildren();
	while (node) {
		if (node->GetName()=="particle") {
			ReadParticle(node);
		}
		node=node->GetNext();
	}
}

THISCLASS::~IDList() {
}

void THISCLASS::ReadParticle(wxXmlNode *node) {
	mSelectedNode=node;
	SelectChildNode("barcode");
	std::string chips=ReadString("chips", "0");
	std::string chipsdirection=ReadAttributeString("chips", "direction", "0");

	mSelectedNode=node;
	SelectChildNode("barcode");

	if (m->mCommand=="CODE") {
		tID id;
		id.robotid=m->GetInt(INT_MIN);
		if (id.robotid==INT_MIN) {id.robotid=mLastRobotID+1;}
		mLastRobotID=id.robotid;

		int numchips=m->GetInt(1);
		int chips=m->GetInt(0);
		while (numchips>0) {
			id.chips.push_back((chips & 1 ? 1 : -1));
			chips=(chips >> 1);
			numchips--;
		}
		id.angle=(float)(m->GetDouble(0));
	} else if (m->mCommand=="CHIPS") {
		tID id;
		id.robotid=m->GetInt(INT_MIN);
		if (id.robotid==INT_MIN) {id.robotid=mLastRobotID+1;}
		mLastRobotID=id.robotid;

		std::string chips=m->GetString("0");
		std::string::iterator it=chips.begin();
		while (it!=chips.end()) {
			if (*it=='0') {
				id.chips.push_back(-1);
			} else if (*it=='1') {
				id.chips.push_back(1);
			}
			it++;
		}

		std::string direction=m->GetString("");
		if (direction=="counter-clockwise") {
			std::reverse(id.chips.begin(), id.chips.end());
		}
	} else if (m->mCommand=="ENDFRAME") {
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

void THISCLASS::OnNMEASend(const std::string &str) {
	// We don't write
}

bool THISCLASS::ReadBlock() {

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

