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
		mDataStructureImageGray("ImageGray", "Grayscale image"),
		mDataStructureImageBinary("ImageBinary", "Binary image"),
		mDataStructureMaskBinary("MaskBinary", "Binary mask"),
		mDataStructureParticles(),
		mCategoryInput("Input", 100, ComponentCategory::sTypeOne),
		mCategoryInputConversion("Input conversion", 200, ComponentCategory::sTypeAuto),
		mCategoryPreprocessing("Preprocessing", 300),
		mCategoryThresholding("Thresholding", 400),
		mCategoryBlobDetection("Blob detection", 500),
		mCategoryOutput("Output", 10000),
		mAvailableComponents(), mDeployedComponents(), mDataStructures(), mSwisTrackCoreInterfaces(),
		mStarted(false), mSeriousMode(false), mEditLocks(0)
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
	mDataStructures.push_back(&mDataStructureImageGray);
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

bool THISCLASS::Start(bool seriousmode) {
	if (mStarted) {return false;}
	if (mEditLocks>0) {return false;}

	// Notify the interfaces
	tSwisTrackCoreInterfaceList::iterator iti=mSwisTrackCoreInterfaces.begin();
	while (iti!=mSwisTrackCoreInterfaces.end()) {
		(*iti)->OnBeforeStart(seriousmode);
		iti++;
	}
	
	// Update the flags
	mStarted=true;
	mSeriousMode=seriousmode;

	// Start all components (until first error)
	tComponentList::iterator it=mDeployedComponents.begin();
	while (it!=mDeployedComponents.end()) {
		(*it)->ClearStatus();
		(*it)->OnStart();
		if ((*it)->mStatusHasError) {break;}
		(*it)->mStarted=true;
		it++;
	}

	// Notify the interfaces
	iti=mSwisTrackCoreInterfaces.begin();
	while (iti!=mSwisTrackCoreInterfaces.end()) {
		(*iti)->OnAfterStart(seriousmode);
		iti++;
	}

	return true;
}

bool THISCLASS::Stop() {
	if (! mStarted) {return false;}

	// Notify the interfaces
	tSwisTrackCoreInterfaceList::iterator iti=mSwisTrackCoreInterfaces.begin();
	while (iti!=mSwisTrackCoreInterfaces.end()) {
		(*iti)->OnBeforeStop();
		iti++;
	}

	// Stop all components
	tComponentList::iterator it=mDeployedComponents.end();
	while (it!=mDeployedComponents.begin()) {
		it--;
		if ((*it)->mStarted) {
			(*it)->ClearStatus();
			(*it)->OnStop();
			(*it)->mStarted=false;
		}
	}

	// Update flags
	mStarted=false;
	mSeriousMode=false;

	// Notify the interfaces
	iti=mSwisTrackCoreInterfaces.begin();
	while (iti!=mSwisTrackCoreInterfaces.end()) {
		(*iti)->OnAfterStop();
		iti++;
	}

	return true;
}

bool THISCLASS::Step() {
	if (! mStarted) {return false;}

	// Notify the interfaces
	tSwisTrackCoreInterfaceList::iterator iti=mSwisTrackCoreInterfaces.begin();
	while (iti!=mSwisTrackCoreInterfaces.end()) {
		(*iti)->OnBeforeStep();
		iti++;
	}

	// Run until first error, or until the end (all started components)
	tComponentList::iterator it=mDeployedComponents.begin();
	while (it!=mDeployedComponents.end()) {
		if (! (*it)->mStarted) {break;}
		(*it)->ClearStatus();
		(*it)->OnStep();
		if (! (*it)->mStatusHasError) {break;}
		it++;
	}

	// Notify the interfaces
	iti=mSwisTrackCoreInterfaces.begin();
	while (iti!=mSwisTrackCoreInterfaces.end()) {
		(*iti)->OnStepReady();
		iti++;
	}

	// and then cleanup what we run
	while (it!=mDeployedComponents.begin()) {
		it--;
		(*it)->OnStepCleanup();
	}

	// Notify the interfaces
	iti=mSwisTrackCoreInterfaces.begin();
	while (iti!=mSwisTrackCoreInterfaces.end()) {
		(*iti)->OnAfterStep();
		iti++;
	}

	return true;
}

bool THISCLASS::ReloadConfiguration() {
	if (! mStarted) {return false;}

	// Start all components (until first error)
	tComponentList::iterator it=mDeployedComponents.begin();
	while (it!=mDeployedComponents.end()) {
		if (! (*it)->mStarted) {break;}
		(*it)->ClearStatus();
		(*it)->OnReloadConfiguration();
		it++;
	}

	return true;
}

void THISCLASS::Clear() {
	tComponentList::iterator it=mDeployedComponents.begin();
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
	tComponentList::iterator it=mDeployedComponents.begin();
	while (it!=mDeployedComponents.end()) {
		xmlpp::Element *element=configuration->add_child("component");
		element->set_attribute("type", (*it)->mName);
		(*it)->ConfigurationWriteXML(element, xmlerr);

		it++;
	}
}

Component *THISCLASS::GetComponentByName(const std::string &name) {
	tComponentList::iterator it=mAvailableComponents.begin();
	while (it!=mAvailableComponents.end()) {
		if ((*it)->mName==name) {return (*it);}
		it++;
	}

	return 0;
}

void AddInterface(SwisTrackCoreInterface *stc) {
	mSwisTrackCoreInterfaces.push_back(stc);
}

void RemoveInterface(SwisTrackCoreInterface *stc) {
	tSwisTrackCoreInterfaceList::iterator it=find(mSwisTrackCoreInterfaces.begin(), mSwisTrackCoreInterfaces.end());
	if (it==mSwisTrackCoreInterfaces.end()) {return;}
	mSwisTrackCoreInterfaces.erase(it);
}
