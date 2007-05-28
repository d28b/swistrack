#include "ObjectList.h"
#define THISCLASS ObjectList

#include <fstream>
#include <algorithm>
#include <sstream>
#include <wx/log.h>

THISCLASS::ObjectList(const std::string &filename):
		ConfigurationXML(0, true),
		mObjects(), mError(""), mFileName(filename) {

	// Read the file
	wxLogNull log;
	wxXmlDocument document;
	bool isopen=document.Load(filename);
	if (! isopen) {
		mError="Could not open or parse the XML file!";
		return;
	}

	// Select the root element and check its name
	SetRootNode(document.GetRoot());
	if (GetRootNode()->GetName() != "objectlist") {
		mError="The XML root node must be called 'objectlist'!";
		return;
	}

	// Enumerate all robots in the list
	SelectChildNode("objects");
	if (! mSelectedNode) {
		mError="No node 'objects' found!";
		return;
	}

	wxXmlNode *node=mSelectedNode->GetChildren();
	while (node) {
		if (node->GetName()=="object") {
			ReadObject(node);
			if (mError!="") {return;}
		}
		node=node->GetNext();
	}
}

THISCLASS::~ObjectList() {
}

void THISCLASS::ReadObject(wxXmlNode *node) {
	mSelectedNode=node;

	// Create id
	Object object;
	object.objectid=Int(ReadProperty("id"), 0);
	SelectChildNode("barcode");
	if (! mSelectedNode) {return;}
	object.angle=(float)Double(ReadChildContent("angle"), 0);

	// Read chips
	std::string chips=ReadChildContent("chips", "").c_str();
	std::string chipsdirection=ReadChildProperty("chips", "direction", "").c_str();
	int length=Int(ReadChildProperty("chips", "length"), 0);
	int symbol=Int(ReadChildProperty("chips", "symbol"), 0);

	// Add the chips given by length and symbol
	while (length>0) {
		object.chips.push_back((symbol & 1 ? 1 : -1));
		symbol=(symbol >> 1);
		length--;
	}
	std::reverse(object.chips.begin(), object.chips.end());

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
	if (object.chips.empty()) {
		std::ostringstream oss;
		oss << "Invalid barcode for object id " << object.objectid << "!";
		mError=oss.str();
		return;
	}

	// Reverse the chips if necessary
	if (chipsdirection=="counter-clockwise") {
		std::reverse(object.chips.begin(), object.chips.end());
	}

	// Center the chips around the mean
	float sum=0;
	tChipVector::iterator itc=object.chips.begin();
	while (itc!=object.chips.end()) {
		sum+=*itc;
		itc++;
	}
	float mean=sum/(float)(object.chips.size());
	itc=object.chips.begin();
	while (itc!=object.chips.end()) {
		*itc-=mean;
		itc++;
	}

	// Normalize the variance
	float varsum=0;
	int varcc=-1;
	itc=object.chips.begin();
	while (itc!=object.chips.end()) {
		varsum+=(*itc)*(*itc);
		varcc++;
		itc++;
	}
	float stddev=sqrt(varsum/varcc);
	itc=object.chips.begin();
	while (itc!=object.chips.end()) {
		*itc/=stddev;
		itc++;
	}

	// Add the object
	mObjects.push_back(object);
}
