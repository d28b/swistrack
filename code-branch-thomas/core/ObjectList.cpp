#include "ObjectList.h"
#define THISCLASS ObjectList

#include <fstream>
#include <algorithm>

THISCLASS::ObjectList(const std::string &filename):
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
			if (mError!="") {return;}
		}
		node=node->GetNext();
	}
}

THISCLASS::~ObjectList() {
}

void THISCLASS::ReadParticle(wxXmlNode *node) {
	mSelectedNode=node;
	SelectChildNode("barcode");
	if (! mSelectedNode) {return;}

	// Create id
	tObject object;
	object.id=Int(ReadProperty("id"), 0);
	object.angle=(float)Double(ReadContent("angle"), 0);

	// Read chips
	std::string chips=ReadContent("chips", "").str();
	std::string chipsdirection=ReadProperty("chips", "direction", "").str();
	int length=Int(ReadProperty("chips", "length"), 0);
	int symbol=Int(ReadProperty("chips", "symbol"), 0);

	// Add the chips given by length and symbol
	while (length>0) {
		object.chips.push_front((chips & 1 ? 1 : -1));
		symbol=(symbol >> 1);
		length--;
	}

	// Add the chips given by the 01-string
	std::string::iterator it=chips.begin();
	while (it!=chips.end()) {
		if (*it=='0') {
			object.chips.push_back(-1);
		} else if (*it=='1') {
			object.chips.push_back(1);
		}
		it++;
	}

	// If empty, return with error
	if (object.chips.isempty()) {
		std::ostringstream oss;
		oss << "Invalid barcode for object id " << object.id << "!";
		mError=oss.str();
		return;
	}

	// Reverse the chips if necessary
	if (direction=="counter-clockwise") {
		std::reverse(object.chips.begin(), object.chips.end());
	}

	// Add the object
	mObjects.push_back(object);
}
