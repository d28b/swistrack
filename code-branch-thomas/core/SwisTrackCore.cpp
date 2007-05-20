#include "SwisTrackCore.h"
#define THISCLASS SwisTrackCore

#include <sstream>
#include <algorithm>
#include "ComponentTriggerTimer.h"
#include "ComponentTriggerCounter.h"
#include "ComponentInputCamera1394.h"
#include "ComponentInputCameraUSB.h"
#include "ComponentInputCameraGBit.h"
#include "ComponentInputFileAVI.h"
#include "ComponentInputFileImage.h"
#include "ComponentConvertToGray.h"
#include "ComponentConvertToColor.h"
#include "ComponentConvertBayerToColor.h"
#include "ComponentBackgroundSubtractionGray.h"
#include "ComponentBackgroundSubtractionColor.h"
#include "ComponentHSVBackgroundSubtractionColor.h"
#include "ComponentAdaptiveBackgroundSubtractionGray.h"
#include "ComponentAdaptiveBackgroundSubtractionColor.h"
#include "ComponentSpecificColorSubtraction.h"
#include "ComponentThresholdGray.h"
#include "ComponentThresholdColorCommon.h"
#include "ComponentThresholdColorIndependant.h"
#include "ComponentBinaryDilation.h"
#include "ComponentBinaryErosion.h"
#include "ComponentBinaryMask.h"
#include "ComponentBlobDetectionMinMax.h"
#include "ComponentBlobDetectionCircle.h"
#include "ComponentIDReaderRing.h"
#include "ComponentSimulationParticles.h"
#include "ComponentCalibrationLinear.h"
#include "ComponentOutputParticles.h"

THISCLASS::SwisTrackCore(std::string componentconfigurationfolder):
		mAvailableComponents(), mDataStructures(), mSwisTrackCoreInterfaces(), mComponentConfigurationFolder(componentconfigurationfolder),
		mComponentCategories(), mCommunicationInterface(0), mTrigger(new SwisTrackCoreTrigger(this)),
		mCategoryTrigger("Trigger", 0),
		mCategoryInput("Input", 100, ComponentCategory::sTypeOne),
		mCategoryInputConversion("Input conversion", 200, ComponentCategory::sTypeAuto),
		mCategoryPreprocessing("Preprocessing", 300),
		mCategoryThresholding("Thresholding", 400),
		mCategoryBinaryPreprocessing("BinaryPreprocessing", 500),
		mCategoryBlobDetection("Blob detection", 600),
		mCategoryCalibration("Calibration", 700),
		mCategoryOutput("Output", 10000),
		mDataStructureInput(),
		mDataStructureImageColor("ImageColor", "Color image"),
		mDataStructureImageGray("ImageGray", "Grayscale image"),
		mDataStructureImageBinary("ImageBinary", "Binary image"),
		mDataStructureParticles(), 
		mStarted(false), mProductiveMode(false), mEditLocks(0), mDeployedComponents() {

	// Initialize the list of available components
	mAvailableComponents.push_back(new ComponentTriggerTimer(this));
	mAvailableComponents.push_back(new ComponentTriggerCounter(this));
	mAvailableComponents.push_back(new ComponentInputCamera1394(this));
	mAvailableComponents.push_back(new ComponentInputCameraUSB(this));
	mAvailableComponents.push_back(new ComponentInputCameraGBit(this));
	mAvailableComponents.push_back(new ComponentInputFileAVI(this));
	mAvailableComponents.push_back(new ComponentInputFileImage(this));
	mAvailableComponents.push_back(new ComponentConvertToGray(this));
	mAvailableComponents.push_back(new ComponentConvertToColor(this));
	mAvailableComponents.push_back(new ComponentConvertBayerToColor(this));
	mAvailableComponents.push_back(new ComponentBackgroundSubtractionGray(this));
	mAvailableComponents.push_back(new ComponentBackgroundSubtractionColor(this));
	mAvailableComponents.push_back(new ComponentHSVBackgroundSubtractionColor(this));
	mAvailableComponents.push_back(new ComponentAdaptiveBackgroundSubtractionGray(this));
	mAvailableComponents.push_back(new ComponentAdaptiveBackgroundSubtractionColor(this));
	mAvailableComponents.push_back(new ComponentSpecificColorSubtraction(this));
	mAvailableComponents.push_back(new ComponentThresholdGray(this));
	mAvailableComponents.push_back(new ComponentThresholdColorCommon(this));
	mAvailableComponents.push_back(new ComponentThresholdColorIndependant(this));
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
	mDataStructures.push_back(&mDataStructureImageColor);
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

	mDeployedComponents.clear();

	// Delete available components
	tComponentList::iterator ita=mAvailableComponents.begin();
	while (ita!=mAvailableComponents.end()) {
		delete (*ita);
		ita++;
	}

	mAvailableComponents.clear();

	// Delete the trigger
	delete mTrigger;
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

	// Notify the interfaces (OnBeforeStep)
	tSwisTrackCoreInterfaceList::iterator iti=mSwisTrackCoreInterfaces.begin();
	while (iti!=mSwisTrackCoreInterfaces.end()) {
		(*iti)->OnBeforeStep();
		iti++;
	}

	// Notify the displays (OnBeforeStep)
	tComponentList::iterator it=mDeployedComponents.begin();
	while (it!=mDeployedComponents.end()) {
		Component::tDisplayList::iterator itdi=(*it)->mDisplays.begin();
		while (itdi!=(*it)->mDisplays.end()) {
			(*itdi)->OnBeforeStep();
			itdi++;
		}
		it++;
	}

	// Reset the step durations
	it=mDeployedComponents.begin();
	while (it!=mDeployedComponents.end()) {
		(*it)->mStepDuration=-1;
		it++;
	}

	// Initialize performance measurements
	LARGE_INTEGER performancestart;
	LARGE_INTEGER performanceend;
	LARGE_INTEGER performancefrequency;
	QueryPerformanceCounter(&performancestart);
	QueryPerformanceFrequency(&performancefrequency);

	// Run until first error, or until the end (all started components)
	it=mDeployedComponents.begin();
	while (it!=mDeployedComponents.end()) {
		if (! (*it)->mStarted) {break;}

		// Execute the step
		(*it)->ClearStatus();
		(*it)->OnStep();

		// Performance measurement
		QueryPerformanceCounter(&performanceend);
		unsigned long diff=performanceend.LowPart-performancestart.LowPart;
		(*it)->mStepDuration=(double)(diff)/(double)performancefrequency.LowPart;
		performancestart=performanceend;

		// Error handling
		if ((*it)->mStatusHasError) {break;}

		it++;
	}

	// Notify the interfaces (OnStepReady)
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

	// Notify the displays (OnAfterStep)
	it=mDeployedComponents.begin();
	while (it!=mDeployedComponents.end()) {
		Component::tDisplayList::iterator itdi=(*it)->mDisplays.begin();
		while (itdi!=(*it)->mDisplays.end()) {
			(*itdi)->OnAfterStep();
			itdi++;
		}
		it++;
	}

	// Notify the interfaces (OnAfterStep)
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

void THISCLASS::StartTrigger() {
	// Notify the interfaces
	tSwisTrackCoreInterfaceList::iterator it=mSwisTrackCoreInterfaces.begin();
	while (it!=mSwisTrackCoreInterfaces.end()) {
		(*it)->OnBeforeTriggerStart();
		it++;
	}

	// Activate the trigger
	mTrigger->SetActive(true);

	// Notify the interfaces
	it=mSwisTrackCoreInterfaces.begin();
	while (it!=mSwisTrackCoreInterfaces.end()) {
		(*it)->OnAfterTriggerStart();
		it++;
	}
}

void THISCLASS::StopTrigger() {
	// Notify the interfaces
	tSwisTrackCoreInterfaceList::iterator it=mSwisTrackCoreInterfaces.begin();
	while (it!=mSwisTrackCoreInterfaces.end()) {
		(*it)->OnBeforeTriggerStop();
		it++;
	}

	// Deactivate the trigger
	mTrigger->SetActive(false);

	// Notify the interfaces
	it=mSwisTrackCoreInterfaces.begin();
	while (it!=mSwisTrackCoreInterfaces.end()) {
		(*it)->OnAfterTriggerStop();
		it++;
	}
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

bool THISCLASS::IsTriggerActive() {
	return mTrigger->GetActive();
}

void THISCLASS::AddInterface(SwisTrackCoreInterface *stc) {
	mSwisTrackCoreInterfaces.push_back(stc);
}

void THISCLASS::RemoveInterface(SwisTrackCoreInterface *stc) {
	tSwisTrackCoreInterfaceList::iterator it=find(mSwisTrackCoreInterfaces.begin(), mSwisTrackCoreInterfaces.end(), stc);
	if (it==mSwisTrackCoreInterfaces.end()) {return;}
	mSwisTrackCoreInterfaces.erase(it);
}

void THISCLASS::OnIdle() {
	mTrigger->OnIdle();
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
