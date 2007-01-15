#include "SwisTrackCore.h"
#define THISCLASS SwisTrackCore

#include <sstream>
#include "ComponentInputCamera1394.h"
#include "ComponentInputCameraUSB.h"
#include "ComponentInputCameraGBit.h"
#include "ComponentInputFileAVI.h"
#include "ComponentConvertToGray.h"
#include "ComponentConvertToBGR.h"
#include "ComponentBackgroundSubtractionGray.h"
#include "ComponentThresholdGray.h"
#include "ComponentBlobDetectionMinMax.h"
#include "ComponentBlobDetectionCircle.h"

THISCLASS::SwisTrackCore():
		mDataStructureInput(),
		mDataStructureImageBGR("ImageBGR", "Color image (BGR)"),
		mDataStructureBackgroundBGR("BackgroundBGR", "Background color image (BGR)"),
		mDataStructureImageGray("ImageGray", "Grayscale image"),
		mDataStructureBackgroundGray("BackgroundGray", "Grayscale background image"),
		mDataStructureImageBinary("ImageBinary", "Binary image"),
		mDataStructureMaskBinary("MaskBinary", "Binary mask"),
		mDataStructureParticles(),
		mCategoryInput("Input", 100, ComponentCategory::sTypeOne),
		mCategoryInputConversion("Input conversion", 200, ComponentCategory::sTypeAuto),
		mCategoryPreprocessing("Preprocessing", 300),
		mCategoryThresholding("Thresholding", 400),
		mCategoryBlobDetection("Blob detection", 500),
		mCategoryOutput("Output", 10000)
		{

	// Initialize the list of available components
	mAvailableComponents.push_back(new ComponentInputCamera1394(this));
	mAvailableComponents.push_back(new ComponentInputCameraUSB(this));
	mAvailableComponents.push_back(new ComponentInputCameraGBit(this));
	mAvailableComponents.push_back(new ComponentInputFileAVI(this));
	mAvailableComponents.push_back(new ComponentConvertToGray(this));
	mAvailableComponents.push_back(new ComponentConvertToBGR(this));
	mAvailableComponents.push_back(new ComponentBackgroundSubtractionGray(this));
	mAvailableComponents.push_back(new ComponentThresholdGray(this));
	mAvailableComponents.push_back(new ComponentBlobDetectionMinMax(this));
	mAvailableComponents.push_back(new ComponentBlobDetectionCircle(this));

	// Initialize the list of available data structures
	mDataStructures.push_back(&mDataStructureInput);
	mDataStructures.push_back(&mDataStructureImageBGR);
	mDataStructures.push_back(&mDataStructureBackgroundBGR);
	mDataStructures.push_back(&mDataStructureImageGray);
	mDataStructures.push_back(&mDataStructureBackgroundGray);
	mDataStructures.push_back(&mDataStructureImageBinary);
	mDataStructures.push_back(&mDataStructureMaskBinary);
	mDataStructures.push_back(&mDataStructureParticles);

	// TODO remove this
	mDeployedComponents.push_back(new ComponentInputCameraUSB(this));
	mDeployedComponents.push_back(new ComponentConvertToGray(this));
	mDeployedComponents.push_back(new ComponentBackgroundSubtractionGray(this));
	mDeployedComponents.push_back(new ComponentThresholdGray(this));
	mDeployedComponents.push_back(new ComponentBlobDetectionMinMax(this));
}

bool THISCLASS::Start() {
	bool allok=true;

	// Start all components (until first error)
	tComponents::iterator it=mDeployedComponents.begin();
	while (it!=mDeployedComponents.end()) {
		(*it)->ClearStatus();
		(*it)->OnStart();
		if ((*it)->mStatusHasError) {
			allok=false;
			break;
		}
		(*it)->mStarted=true;
		it++;
	}

	return allok;
}

bool THISCLASS::Stop() {
	bool allok=true;

	// Stop all components
	tComponents::iterator it=mDeployedComponents.end();
	while (it!=mDeployedComponents.begin()) {
		it--;
		if ((*it)->mStarted) {
			(*it)->ClearStatus();
			(*it)->OnStop();
			(*it)->mStarted=false;
			if (! (*it)->mStatusHasError) {allok=false;}
		}
	}

	return allok;
}

bool THISCLASS::Step() {
	bool allok=true;

	// Run until first error, or until the end (all started components)
	tComponents::iterator it=mDeployedComponents.begin();
	while (it!=mDeployedComponents.end()) {
		if (! (*it)->mStarted) {break;}
		(*it)->ClearStatus();
		(*it)->OnStep();
		if (! (*it)->mStatusHasError) {allok=false; break;}
		it++;
	}

	// and then cleanup what we run
	while (it!=mDeployedComponents.begin()) {
		it--;
		(*it)->OnStepCleanup();
		if (! (*it)->mStatusHasError) {allok=false;}
	}

	return allok;
}

void THISCLASS::Clear() {
	tComponents::iterator it=mDeployedComponents.begin();
	while (it!=mDeployedComponents.end()) {
		delete (*it);
		it++;
	}

	mDeployedComponents.clear();
}

void THISCLASS::ConfigurationReadXML(xmlpp::Element* configuration, ErrorList *xmlerr) {
	// Clear the current list of components
	Clear();

	// Traverse the list and create a Component object for each "component" tag
	if (! configuration) {return;}
	xmlpp::Node::NodeList list=configuration->get_children("component");
	xmlpp::Node::NodeList::iterator it=list.begin();
	while (it!=list.end()) {
		xmlpp::Element *element=dynamic_cast<xmlpp::Element *>(*it);
		ConfigurationReadXMLElement(element, xmlerr);
		it++;
	}
}

void THISCLASS::ConfigurationReadXMLElement(xmlpp::Element* element, ErrorList *xmlerr) {
	if (! element) {return;}

	// Get the type attribute
	xmlpp::Attribute *att_type=element->get_attribute("type");
	if (! att_type) {
		std::ostringstream oss;
		oss << "The element at line " << element->get_line() << " was ignored because it does not have a 'type' attribute.";
		xmlerr->Add(oss.str(), element->get_line());
	}

	// Search for the component
	std::string type=att_type->get_value();
	Component *component=GetComponentByName(type);
	if (! component) {
		std::ostringstream oss;
		oss << "The element at line " << element->get_line() << " was ignored because there is no component called '" << type << "'.";
		xmlerr->Add(oss.str(), element->get_line());
	}

	// Add it to the list
	Component *newcomponent=component->Create();
	mDeployedComponents.push_back(newcomponent);
	newcomponent->ConfigurationReadXML(element, xmlerr);
}

void THISCLASS::ConfigurationWriteXML(xmlpp::Element *configuration, ErrorList *xmlerr) {
	// Add an element for each component
	tComponents::iterator it=mDeployedComponents.begin();
	while (it!=mDeployedComponents.end()) {
		xmlpp::Element *element=configuration->add_child("component");
		element->set_attribute("type", (*it)->mName);
		(*it)->ConfigurationWriteXML(element, xmlerr);

		it++;
	}
}

Component *THISCLASS::GetComponentByName(const std::string &name) {
	tComponents::iterator it=mAvailableComponents.begin();
	while (it!=mAvailableComponents.end()) {
		if ((*it)->mName==name) {return (*it);}
		it++;
	}

	return 0;
}
