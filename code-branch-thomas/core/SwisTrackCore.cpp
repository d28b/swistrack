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

bool THISCLASS::IncrementEditLocks() {
	if (mEditLocks>0) {return true;}

	// If started in serious mode, editing is not allowed
	if (IsStartedInSeriousMode()) {return false;}

	// If started in test mode, stop and allow editing
	Stop();

	// Notify the interfaces
	iti=mSwisTrackCoreInterfaces.begin();
	while (iti!=mSwisTrackCoreInterfaces.end()) {
		(*iti)->OnBeforeEdit();
		iti++;
	}

	mEditLocks=1;
	return true;
}

void THISCLASS::DecrementEditLocks() {
	if (mEditLocks<1) {return;}

	// Decrement and return if there are still other locks
	mEditLocks--;
	if (mEditLocks>0) {return;}

	// Notify the interfaces
	iti=mSwisTrackCoreInterfaces.begin();
	while (iti!=mSwisTrackCoreInterfaces.end()) {
		(*iti)->OnAfterEdit();
		iti++;
	}

	return true;
}

void THISCLASS::AddInterface(SwisTrackCoreInterface *stc) {
	mSwisTrackCoreInterfaces.push_back(stc);
}

void THISCLASS::RemoveInterface(SwisTrackCoreInterface *stc) {
	tSwisTrackCoreInterfaceList::iterator it=find(mSwisTrackCoreInterfaces.begin(), mSwisTrackCoreInterfaces.end());
	if (it==mSwisTrackCoreInterfaces.end()) {return;}
	mSwisTrackCoreInterfaces.erase(it);
}

