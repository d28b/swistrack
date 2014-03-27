#include "ObjectList.h"
#define THISCLASS ObjectList

#include <fstream>
#include <algorithm>
#include <wx/log.h>
#include "ConfigurationConversion.h"

THISCLASS::ObjectList(const wxFileName &filename):
		ConfigurationXML(0, true),
		mObjects(), mError(), mFileName(filename) {

	// Read the file
	wxLogNull log;
	wxXmlDocument document;
	bool isopen = document.Load(filename.GetFullPath());
	if (! isopen) {
		mError = wxT("Could not open or parse the XML file!");
		return;
	}

	// Select the root element and check its name
	SetRootNode(document.GetRoot());
	if (GetRootNode()->GetName() != wxT("objectlist")) {
		mError = wxT("The XML root node must be called 'objectlist'!");
		return;
	}

	// Enumerate all robots in the list
	SelectChildNode(wxT("objects"));
	if (! mSelectedNode) {
		mError = wxT("No node 'objects' found!");
		return;
	}

	wxXmlNode *node = mSelectedNode->GetChildren();
	while (node) {
		if (node->GetName() == wxT("object")) {
			ReadObject(node);
			if (mError != wxT("")) {
				return;
			}
		}
		node = node->GetNext();
	}
}

THISCLASS::~ObjectList() {
}

void THISCLASS::ReadObject(wxXmlNode *node) {
	mSelectedNode = node;

	// Create id
	Object object;
	object.objectid = ConfigurationConversion::Int(ReadProperty(wxT("id")), 0);
	SelectChildNode(wxT("barcode"));
	if (! mSelectedNode) {
		return;
	}
	object.angle = (float)ConfigurationConversion::Double(ReadChildContent(wxT("angle")), 0);

	// Read chips
	wxString chips = ReadChildContent(wxT("chips"), wxT(""));
	wxString chipsdirection = ReadChildProperty(wxT("chips"), wxT("direction"), wxT(""));
	int length = ConfigurationConversion::Int(ReadChildProperty(wxT("chips"), wxT("length")), 0);
	int symbol = ConfigurationConversion::Int(ReadChildProperty(wxT("chips"), wxT("symbol")), 0);

	// Add the chips given by length and symbol
	while (length > 0) {
		object.chips.push_back((symbol & 1 ? 1 : -1));
		symbol = (symbol >> 1);
		length--;
	}
	std::reverse(object.chips.begin(), object.chips.end());

	// Add the chips given by the 01-string
	wxString::iterator it = chips.begin();
	while (it != chips.end()) {
		if (*it == '0') {
			object.chips.push_back(-1);
		} else if (*it == '1') {
			object.chips.push_back(1);
		}
		it++;
	}

	// If empty, return with error
	if (object.chips.empty()) {
		mError = wxString::Format(wxT("Invalid barcode for object id %d!"), object.objectid);
		return;
	}

	// Reverse the chips if necessary
	if (chipsdirection == wxT("counter-clockwise")) {
		std::reverse(object.chips.begin(), object.chips.end());
	}

	// Center the chips around the mean
	float sum = 0;
	tChipVector::iterator itc = object.chips.begin();
	while (itc != object.chips.end()) {
		sum += *itc;
		itc++;
	}
	float mean = sum / (float)(object.chips.size());
	itc = object.chips.begin();
	while (itc != object.chips.end()) {
		*itc -= mean;
		itc++;
	}

	// Normalize the variance
	float varsum = 0;
	int varcc = -1;
	itc = object.chips.begin();
	while (itc != object.chips.end()) {
		varsum += (*itc) * (*itc);
		varcc++;
		itc++;
	}
	float stddev = sqrt(varsum / varcc);
	itc = object.chips.begin();
	while (itc != object.chips.end()) {
		*itc /= stddev;
		itc++;
	}

	// Add the object
	mObjects.push_back(object);
}
