#include "SwisTrackCore.h"
#define THISCLASS SwisTrackCore

#include <sstream>
#include <algorithm>
#include "ComponentTriggerTimer.h"
#include "ComponentTriggerCounter.h"
#include "ComponentInputCamera1394.h"
#include "ComponentInputCameraUSB.h"
#include "ComponentInputCameraGigE.h"
#include "ComponentInputFileAVI.h"
#include "ComponentInputFileImage.h"
#include "ComponentConvertToGray.h"
#include "ComponentConvertToColor.h"
#include "ComponentConvertBayerToColor.h"
#include "ComponentChannelArithmetic.h"
#include "ComponentBackgroundSubtractionGray.h"
#include "ComponentBackgroundSubtractionColor.h"
#include "ComponentHSVBackgroundSubtractionColor.h"
#include "ComponentAdaptiveBackgroundSubtractionGray.h"
#include "ComponentAdaptiveBackgroundSubtractionColor.h"
#include "ComponentSpecificColorSubtraction.h"
#include "ComponentThresholdGray.h"
#include "ComponentThresholdColorCommon.h"
#include "ComponentThresholdColorIndependent.h"
#include "ComponentBinaryDilation.h"
#include "ComponentBinaryErosion.h"
#include "ComponentBinaryMask.h"
#include "ComponentBlobSelection.h"
#include "ComponentBlobDetectionMinMax.h"
#include "ComponentTracking.h"
#include "ComponentNearestNeighborTracking.h"
#include "ComponentIDReaderRing.h"
#include "ComponentSimulationParticles.h"
#include "ComponentCalibrationLinear.h"
#include "ComponentCalibrationTSAI.h"
#include "ComponentOutputParticles.h"
#include "ComponentOutputFileAVI.h"
#include "ComponentOutputFile.h"
#include "ComponentOutputMarkFrameManual.h"

THISCLASS::SwisTrackCore(std::string componentconfigurationfolder):
		mAvailableComponents(), mDataStructures(), mSwisTrackCoreInterfaces(), mComponentConfigurationFolder(componentconfigurationfolder),
		mComponentCategories(), mCommunicationInterface(0), mTrigger(new SwisTrackCoreTrigger(this)), mEventRecorder(new SwisTrackCoreEventRecorder(this)),
		mCategoryTrigger("Trigger", "Trigger", 0),
		mCategoryInput("Input", "Input", 100, ComponentCategory::sTypeOne),
		mCategoryInputConversion("InputConversion", "Input conversion", 200, ComponentCategory::sTypeAuto),
		mCategoryPreprocessingColor("PreprocessingColor", "Preprocessing (color)", 300),
		mCategoryPreprocessingGray("PreprocessingGray", "Preprocessing (grayscale)", 350),
		mCategoryThresholding("Thresholding", "Thresholding", 400),
		mCategoryPreprocessingBinary("PreprocessingBinary", "Preprocessing (binary)", 500),
		mCategoryParticleDetection("ParticleDetection", "Particle detection", 600),
		mCategoryCalibration("Calibration", "Calibration", 700),
		mCategoryTracking("Tracking", "Tracking", 800),
		mCategoryOutput("Output", "Output", 10000),
		mDataStructureInput(),
		mDataStructureImageColor("ImageColor", "Color image"),
		mDataStructureImageGray("ImageGray", "Grayscale image"),
		mDataStructureImageBinary("ImageBinary", "Binary image"),
		mDataStructureParticles(),
		mDataStructureTracks(),
		mStarted(false), mProductionMode(false), mEditLocks(0), mDeployedComponents() {

	// Initialize the list of available components
	mAvailableComponents.push_back(new ComponentTriggerTimer(this));
	mAvailableComponents.push_back(new ComponentTriggerCounter(this));
	mAvailableComponents.push_back(new ComponentInputCamera1394(this));
	mAvailableComponents.push_back(new ComponentInputCameraUSB(this));
	mAvailableComponents.push_back(new ComponentInputCameraGigE(this));
	mAvailableComponents.push_back(new ComponentInputFileAVI(this));
	mAvailableComponents.push_back(new ComponentInputFileImage(this));
	mAvailableComponents.push_back(new ComponentConvertToGray(this));
	mAvailableComponents.push_back(new ComponentConvertToColor(this));
	mAvailableComponents.push_back(new ComponentConvertBayerToColor(this));
	mAvailableComponents.push_back(new ComponentChannelArithmetic(this));
	mAvailableComponents.push_back(new ComponentBackgroundSubtractionColor(this));
	mAvailableComponents.push_back(new ComponentHSVBackgroundSubtractionColor(this));
	mAvailableComponents.push_back(new ComponentAdaptiveBackgroundSubtractionColor(this));
	mAvailableComponents.push_back(new ComponentSpecificColorSubtraction(this));
	mAvailableComponents.push_back(new ComponentBackgroundSubtractionGray(this));
	mAvailableComponents.push_back(new ComponentAdaptiveBackgroundSubtractionGray(this));
	mAvailableComponents.push_back(new ComponentThresholdGray(this));
	mAvailableComponents.push_back(new ComponentThresholdColorCommon(this));
	mAvailableComponents.push_back(new ComponentThresholdColorIndependent(this));
	mAvailableComponents.push_back(new ComponentBinaryDilation(this));
	mAvailableComponents.push_back(new ComponentBinaryErosion(this));
	mAvailableComponents.push_back(new ComponentBinaryMask(this));
	mAvailableComponents.push_back(new ComponentBlobSelection(this));
	mAvailableComponents.push_back(new ComponentBlobDetectionMinMax(this));
	mAvailableComponents.push_back(new ComponentIDReaderRing(this));
	mAvailableComponents.push_back(new ComponentSimulationParticles(this));
	mAvailableComponents.push_back(new ComponentCalibrationLinear(this));
	mAvailableComponents.push_back(new ComponentCalibrationTSAI(this));
	mAvailableComponents.push_back(new ComponentTracking(this));
	mAvailableComponents.push_back(new ComponentNearestNeighborTracking(this));
	mAvailableComponents.push_back(new ComponentOutputParticles(this));
	mAvailableComponents.push_back(new ComponentOutputFileAVI(this));
	mAvailableComponents.push_back(new ComponentOutputMarkFrameManual(this));
	mAvailableComponents.push_back(new ComponentOutputFile(this));

	// Initialize the available components
	tComponentList::iterator ita=mAvailableComponents.begin();
	while (ita!=mAvailableComponents.end()) {
		(*ita)->OnInitializeStatic();
		ita++;
	}

	// Initialize the list of available data structures
	mDataStructures.push_back(&mDataStructureInput);
	mDataStructures.push_back(&mDataStructureImageColor);
	mDataStructures.push_back(&mDataStructureImageGray);
	mDataStructures.push_back(&mDataStructureImageBinary);
	mDataStructures.push_back(&mDataStructureParticles);
	mDataStructures.push_back(&mDataStructureTracks);
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

	// Terminate and delete available components
	tComponentList::iterator ita=mAvailableComponents.begin();
	while (ita!=mAvailableComponents.end()) {
		(*ita)->OnTerminateStatic();
		delete (*ita);
		ita++;
	}

	mAvailableComponents.clear();

	// Delete associated objects
	delete mTrigger;
	delete mEventRecorder;
}

bool THISCLASS::Start(bool productionmode) {
	if (mStarted) {return false;}
	if (mEditLocks>0) {return false;}

	// Event recorder
	if (productionmode) {
		mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_SetModeProduction);
	} else {
		mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_SetModeNormal);
	}
	mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_BeforeStart);

	// Notify the interfaces
	tSwisTrackCoreInterfaceList::iterator iti=mSwisTrackCoreInterfaces.begin();
	while (iti!=mSwisTrackCoreInterfaces.end()) {
		(*iti)->OnBeforeStart(productionmode);
		iti++;
	}

	// Update the flags
	mStarted=true;
	mProductionMode=productionmode;

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
		(*iti)->OnAfterStart(productionmode);
		iti++;
	}

	// Notify the clients
	CommunicationMessage mstart("START");
	mCommunicationInterface->Send(&mstart);

	// Event recorder
	mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_AfterStart);

	return true;
}

bool THISCLASS::Stop() {
	if (! mStarted) {return false;}

	// Event recorder
	mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_BeforeStop);

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
	mProductionMode=false;

	// Notify the interfaces
	iti=mSwisTrackCoreInterfaces.begin();
	while (iti!=mSwisTrackCoreInterfaces.end()) {
		(*iti)->OnAfterStop();
		iti++;
	}

	// Notify the clients
	CommunicationMessage mstop("STOP");
	mCommunicationInterface->Send(&mstop);

	// Event recorder
	mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_AfterStop);

	return true;
}

bool THISCLASS::Step() {
	if (! mStarted) {return false;}

	// Event recorder
	mEventRecorder->AddStepStart();

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

	// Notify the clients
	CommunicationMessage mstart("STEP_START");
	mCommunicationInterface->Send(&mstart);

	// Reset the step durations
	it=mDeployedComponents.begin();
	while (it!=mDeployedComponents.end()) {
		(*it)->mStepDuration=-1;
		it++;
	}

	// Run until first error, or until the end (all started components)
	it=mDeployedComponents.begin();
	while (it!=mDeployedComponents.end()) {
		if (! (*it)->mStarted) {break;}

		// Event recorder
		SwisTrackCoreEventRecorder::Event starttime;
		mEventRecorder->LapTime(&starttime, SwisTrackCoreEventRecorder::sType_StepStart, (*it));
		mEventRecorder->Add(&starttime);

		// Execute the step
		(*it)->ClearStatus();
		(*it)->OnStep();

		// Event recorder
		SwisTrackCoreEventRecorder::Event endtime;
		mEventRecorder->LapTime(&endtime, SwisTrackCoreEventRecorder::sType_StepStop, (*it));
		mEventRecorder->Add(&endtime);
		(*it)->mStepDuration=mEventRecorder->CalculateDuration(&starttime, &endtime);

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

	// Notify the clients
	CommunicationMessage mstop("STEP_STOP");
	mCommunicationInterface->Send(&mstop);

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

	// Event recorder
	mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_StepStop);

	return true;
}

bool THISCLASS::ReloadConfiguration() {
	if (! mStarted) {return false;}

	// Event recorder
	mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_BeforeReloadConfiguration);

	// Start all components (until first error)
	tComponentList::iterator it=mDeployedComponents.begin();
	while (it!=mDeployedComponents.end()) {
		if (! (*it)->mStarted) {break;}
		(*it)->ClearStatus();
		(*it)->OnReloadConfiguration();
		it++;
	}

	// Event recorder
	mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_AfterReloadConfiguration);

	return true;
}

void THISCLASS::TriggerStart() {
	if (mTrigger->GetActive()) {return;}

	// Event recorder
	mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_BeforeTriggerStart);

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

	// Notify the clients
	CommunicationMessage mstart("TRIGGER_START");
	mCommunicationInterface->Send(&mstart);

	// Event recorder
	mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_AfterTriggerStart);
}

void THISCLASS::TriggerStop() {
	if (! mTrigger->GetActive()) {return;}

	// Event recorder
	mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_BeforeTriggerStop);

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

	// Notify the clients
	CommunicationMessage mstop("TRIGGER_STOP");
	mCommunicationInterface->Send(&mstop);

	// Event recorder
	mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_AfterTriggerStop);
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

	// If started in production mode, editing is not allowed
	if (IsStartedInProductionMode()) {return false;}

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
