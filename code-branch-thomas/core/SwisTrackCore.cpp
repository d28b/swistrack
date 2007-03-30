#include "SwisTrackCore.h"
#define THISCLASS SwisTrackCore

#include <sstream>
#include <algorithm>
#include <wx/stopwatch.h>
#include "ComponentInputCamera1394.h"
#include "ComponentInputCameraUSB.h"
#include "ComponentInputCameraGBit.h"
#include "ComponentInputFileAVI.h"
#include "ComponentInputFileImage.h"
#include "ComponentConvertToGray.h"
#include "ComponentConvertToBGR.h"
#include "ComponentBackgroundSubtractionGray.h"
#include "ComponentThresholdGray.h"
#include "ComponentBinaryDilation.h"
#include "ComponentBinaryErosion.h"
#include "ComponentBinaryMask.h"
#include "ComponentBlobDetectionMinMax.h"
#include "ComponentBlobDetectionCircle.h"
#include "ComponentIDReaderRing.h"
#include "ComponentSimulationParticles.h"
#include "ComponentCalibrationLinear.h"
#include "ComponentOutputParticles.h"

THISCLASS::SwisTrackCore():
		mDataStructureInput(),
		mDataStructureImageBGR("ImageBGR", "Color image (BGR)"),
		mDataStructureImageGray("ImageGray", "Grayscale image"),
		mDataStructureImageBinary("ImageBinary", "Binary image"),
		mDataStructureParticles(),
		mCategoryInput("Input", 100, ComponentCategory::sTypeOne),
		mCategoryInputConversion("Input conversion", 200, ComponentCategory::sTypeAuto),
		mCategoryPreprocessing("Preprocessing", 300),
		mCategoryThresholding("Thresholding", 400),
		mCategoryBinaryPreprocessing("BinaryPreprocessing", 500),
		mCategoryBlobDetection("Blob detection", 600),
		mCategoryCalibration("Calibration", 700),
		mCategoryOutput("Output", 10000),
		mAvailableComponents(), mDeployedComponents(), mDataStructures(), mSwisTrackCoreInterfaces(),
		mStarted(false), mProductiveMode(false), mEditLocks(0)
		{

	// Initialize the list of available components
	mAvailableComponents.push_back(new ComponentInputCamera1394(this));
	mAvailableComponents.push_back(new ComponentInputCameraUSB(this));
	mAvailableComponents.push_back(new ComponentInputCameraGBit(this));
	mAvailableComponents.push_back(new ComponentInputFileAVI(this));
	mAvailableComponents.push_back(new ComponentInputFileImage(this));
	mAvailableComponents.push_back(new ComponentConvertToGray(this));
	mAvailableComponents.push_back(new ComponentConvertToBGR(this));
	mAvailableComponents.push_back(new ComponentBackgroundSubtractionGray(this));
	mAvailableComponents.push_back(new ComponentThresholdGray(this));
	mAvailableComponents.push_back(new ComponentBinaryDilation(this));
	mAvailableComponents.push_back(new ComponentBinaryErosion(this));
	mAvailableComponents.push_back(new ComponentBinaryMask(this));
	mAvailableComponents.push_back(new ComponentBlobDetectionMinMax(this));
	mAvailableComponents.push_back(new ComponentBlobDetectionCircle(this));
	mAvailableComponents.push_back(new ComponentIDReaderRing(this));
	mAvailableComponents.push_back(new ComponentSimulationParticles(this));
	mAvailableComponents.push_back(new ComponentCalibrationLinear(this));
	mAvailableComponents.push_back(new ComponentOutputParticles(this));

	// Initialize the list of available data structures
	mDataStructures.push_back(&mDataStructureInput);
	mDataStructures.push_back(&mDataStructureImageBGR);
	mDataStructures.push_back(&mDataStructureImageGray);
	mDataStructures.push_back(&mDataStructureImageBinary);
	mDataStructures.push_back(&mDataStructureParticles);
}

THISCLASS::~SwisTrackCore() {
	Stop();

	// Delete deployed components
	tComponentList::iterator itd=mDeployedComponents.begin();
	while (itd!=mDeployedComponents.end()) {
		delete (*itd);
		itd++;
	}

	// Delete available components
	tComponentList::iterator ita=mAvailableComponents.begin();
	while (ita!=mAvailableComponents.end()) {
		delete (*ita);
		ita++;
	}

	mAvailableComponents.clear();
	mDeployedComponents.clear();
}

bool THISCLASS::Start(bool productivemode) {
	if (mStarted) {return false;}
	if (mEditLocks>0) {return false;}

	// Notify the interfaces
	tSwisTrackCoreInterfaceList::iterator iti=mSwisTrackCoreInterfaces.begin();
	while (iti!=mSwisTrackCoreInterfaces.end()) {
		(*iti)->OnBeforeStart(productivemode);
		iti++;
	}
	
	// Update the flags
	mStarted=true;
	mProductiveMode=productivemode;

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
		(*iti)->OnAfterStart(productivemode);
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
	mProductiveMode=false;

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
		wxStopWatch sw;
		(*it)->OnStep();
		(*it)->mStepDuration=sw.Time();
		if ((*it)->mStatusHasError) {break;}
		Component::tDisplayImageList::iterator itdi=(*it)->mDisplayImages.begin();
		while (itdi!=(*it)->mDisplayImages.end()) {
			(*itdi)->OnChanged();
			itdi++;
		}
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

void THISCLASS::ConfigurationWriteXML(wxXmlNode *configuration, ErrorList *xmlerr) {
	// Add an element for each component
	tComponentList::iterator it=mDeployedComponents.begin();
	while (it!=mDeployedComponents.end()) {
		wxXmlNode *node=new wxXmlNode(0, wxXML_ELEMENT_NODE, "component");
		configuration->AddChild(node);
		node->AddProperty("type", (*it)->mName);
		(*it)->ConfigurationWriteXML(node, xmlerr);

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
	if (mEditLocks>0) {mEditLocks++; return true;}

	// If started in productive mode, editing is not allowed
	if (IsStartedInProductiveMode()) {return false;}

	// If started in test mode, stop and allow editing
	Stop();

	// Notify the interfaces
	tSwisTrackCoreInterfaceList::iterator it=mSwisTrackCoreInterfaces.begin();
	while (it!=mSwisTrackCoreInterfaces.end()) {
		(*it)->OnBeforeEdit();
		it++;
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
	tSwisTrackCoreInterfaceList::iterator it=mSwisTrackCoreInterfaces.begin();
	while (it!=mSwisTrackCoreInterfaces.end()) {
		(*it)->OnAfterEdit();
		it++;
	}
}

void THISCLASS::AddInterface(SwisTrackCoreInterface *stc) {
	mSwisTrackCoreInterfaces.push_back(stc);
}

void THISCLASS::RemoveInterface(SwisTrackCoreInterface *stc) {
	tSwisTrackCoreInterfaceList::iterator it=find(mSwisTrackCoreInterfaces.begin(), mSwisTrackCoreInterfaces.end(), stc);
	if (it==mSwisTrackCoreInterfaces.end()) {return;}
	mSwisTrackCoreInterfaces.erase(it);
}

