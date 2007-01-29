#include "ConfigurationXML.h"
#define THISCLASS ConfigurationXML

bool THISCLASS::Open(const std::string &filename, ErrorList *errorlist) {
	// Read the file
	xmlpp::DomParser parser;
	xmlpp::Document *document=0;
	try {
		parser.parse_file(filename.c_str());
		if (parser==true) {
			document=parser.get_document();
		}
	} catch (...) {
		document=0;
	}

	if (document==0) {return false;}
	return true;
}

	if ((breakonerror) && (document==0)) {
		wxMessageDialog dlg(this, "The file \n\n"+filename+" \n\ncould not be loaded. Syntax error?", "Open Configuration", wxOK);
		dlg.ShowModal();
		return;
	}

	// At this point, we consider the SwisTrack configuration to be valid. The next few lines close the current configuration and read the configuration.

	// Close the current configuration
	if (mChanged) {
		// TODO if necessary, ask the user whether he'd like to save the changes
		// return false;
	}

	// Set the new file name
	if (astemplate) {
		mFileName="";
	} else {
		mFileName=filename;
	}

	// Read the configuration
	ErrorList errorlist;
	ConfigurationReadXML(&stce, document, &errorlist);
	if (errorlist.mList.empty()) {return;}


}



	if (! element) {return;}


	// Get the mode attribute
	xmlpp::Attribute *att_type=element->get_attribute("mode");
	if (! att_type) {
		xmlerr->Add("The 'mode' attribute of the node 'ConfigurationXML' is missing.", element->get_line());
		return;
	}
	std::string type=att_type->get_value();

	if (type=="timer") {
		xmlpp::Node::NodeList nodes_interval=rootnode->get_children("Interval");
		if (nodes_ConfigurationXML.empty()) {
			errorlist->Add("The interval for the timer ConfigurationXML is not specified!", rootnode->get_line());
			return;
		}
	}

}

void THISCLASS::ConfigurationWriteXML(xmlpp::Element *configuration, ErrorList *xmlerr) {
	// Add an element for each component
	tComponentList::iterator it=mDeployedComponents.begin();
	while (it!=mDeployedComponents.end()) {
		xmlpp::Element *element=configuration->add_child("component");
		element->set_attribute("type", (*it)->mName);
		(*it)->ConfigurationWriteXML(element, xmlerr);

		it++;
	}
}
