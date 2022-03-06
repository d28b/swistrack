#include "SwisTrackCore.h"
#define THISCLASS SwisTrackCore

#include <algorithm>
#include "ComponentAdaptiveBackgroundSubtractionColor.h"
#include "ComponentAdaptiveBackgroundSubtractionGray.h"
#include "ComponentAdaptiveBackgroundSubtractionMedian.h"
#include "ComponentAdaptiveThreshold.h"
#include "ComponentBackgroundSubtractionColor.h"
#include "ComponentBackgroundSubtractionGray.h"
#include "ComponentBinaryBlur.h"
#include "ComponentBinaryDilation.h"
#include "ComponentBinaryErosion.h"
#include "ComponentBinaryMask.h"
#include "ComponentBlobDetectionCircularHough.h"
#include "ComponentBlobDetectionMinMax.h"
#include "ComponentBlobDetectionRedGreen.h"
#include "ComponentBlobDetectionTwoColors.h"
#include "ComponentCalibrationFileTSAI.h"
#include "ComponentCalibrationLinear.h"
#include "ComponentCalibrationTSAI.h"
#include "ComponentCannyEdgeDetection.h"
#include "ComponentChannelCalculationColor.h"
#include "ComponentChannelCalculationColorToGray.h"
#include "ComponentChannelCalculationGray.h"
#include "ComponentColorBlur.h"
#include "ComponentColorGaussianBlur.h"
#include "ComponentColorMask.h"
#include "ComponentColorSwapper.h"
#include "ComponentConvertBayerToColor.h"
#include "ComponentConvertBinaryToColor.h"
#include "ComponentConvertBinaryToGray.h"
#include "ComponentConvertColorToGray.h"
#include "ComponentConvertGrayToColor.h"
#include "ComponentConvertInputToColor.h"
#include "ComponentConvertInputToGray.h"
#include "ComponentCorrectMean.h"
#include "ComponentCropBinary.h"
#include "ComponentCropColor.h"
#include "ComponentCropGray.h"
#include "ComponentCropInput.h"
#include "ComponentDoubleThresholdColorIndependent.h"
#include "ComponentFilterParticles.h"
#include "ComponentGrayMask.h"
#include "ComponentIDReaderRing.h"
#include "ComponentInputCameraUSB.h"
#include "ComponentInputFileAVI.h"
#include "ComponentInputFileImage.h"
#include "ComponentInvertBinary.h"
#include "ComponentMorphology.h"
#include "ComponentOutputBufferedFileAVI.h"
#include "ComponentOutputFileAVI.h"
#include "ComponentOutputFile.h"
#include "ComponentOutputFramesImages.h"
#include "ComponentOutputImageOverlayColor.h"
#include "ComponentOutputImageStatisticsBinary.h"
#include "ComponentOutputImageStatisticsColor.h"
#include "ComponentOutputImageStatisticsGray.h"
#include "ComponentOutputMarkFrameManual.h"
#include "ComponentOutputParticles.h"
#include "ComponentOutputSQLite.h"
#include "ComponentSimulationParticles.h"
#include "ComponentSobelDifferentiation.h"
#include "ComponentSpecificColorSubtraction.h"
#include "ComponentThresholdColorIndependent.h"
#include "ComponentThresholdGray.h"
#include "ComponentTracking.h"
#include "ComponentTrackSmoothing.h"
#include "ComponentTriggerCounter.h"
#include "ComponentTriggerTimer.h"
#include "ComponentWeightedSum.h"
#include "NMEALog.h"

THISCLASS::SwisTrackCore(wxString componentconfigurationfolder):
	mAvailableComponents(), mDataStructures(), mSwisTrackCoreInterfaces(), mComponentConfigurationFolder(componentconfigurationfolder),
	mComponentCategories(), mCommunicationInterface(0), mTrigger(new SwisTrackCoreTrigger(this)), mEventRecorder(new SwisTrackCoreEventRecorder(this)),
	mCategoryTrigger(wxT("Trigger"), wxT("Trigger"), 0),
	mCategoryInput(wxT("Input"), wxT("Input"), 100, ComponentCategory::sTypeOne),
	mCategoryInputConversion(wxT("InputConversion"), wxT("Input conversion"), 200, ComponentCategory::sTypeAuto),
	mCategoryProcessingColor(wxT("PreprocessingColor"), wxT("Color processing"), 300),
	mCategoryProcessingGray(wxT("PreprocessingGray"), wxT("Grayscale processing"), 350),
	mCategoryProcessingBinary(wxT("PreprocessingBinary"), wxT("Binary image processing"), 500),
	mCategoryParticleDetection(wxT("ParticleDetection"), wxT("Particle detection"), 600),
	mCategoryCalibration(wxT("Calibration"), wxT("Calibration"), 700),
	mCategoryTracking(wxT("Tracking"), wxT("Tracking"), 800),
	mCategoryOutput(wxT("Output"), wxT("Output"), 10000),
	mDataStructureInput(),
	mDataStructureImageColor(wxT("ImageColor"), wxT("Color image")),
	mDataStructureImageGray(wxT("ImageGray"), wxT("Grayscale image")),
	//mDataStructureImageFFT(wxT("ImageFFT"), wxT("Fourier image")),
	mDataStructureImageBinary(wxT("ImageBinary"), wxT("Binary image")),
	mDataStructureParticles(),
	mDataStructureTracks(),
	mDataStructureCommands(),
	mStarted(false), mProductionMode(false), mStepCounter(0), mEditLocks(0), mDeployedComponents() {

	// Initialize the list of available components

	// Trigger
	mAvailableComponents.push_back(new ComponentTriggerTimer(this));
	mAvailableComponents.push_back(new ComponentTriggerCounter(this));

	// Input
	mAvailableComponents.push_back(new ComponentInputCameraUSB(this));
	mAvailableComponents.push_back(new ComponentInputFileAVI(this));
	mAvailableComponents.push_back(new ComponentInputFileImage(this));

	// Input conversion
	mAvailableComponents.push_back(new ComponentCropInput(this));
	mAvailableComponents.push_back(new ComponentConvertInputToGray(this));
	mAvailableComponents.push_back(new ComponentConvertInputToColor(this));
	mAvailableComponents.push_back(new ComponentConvertBayerToColor(this));

	// Color processing
	mAvailableComponents.push_back(new ComponentCropColor(this));
	mAvailableComponents.push_back(new ComponentChannelCalculationColor(this));
	mAvailableComponents.push_back(new ComponentChannelCalculationColorToGray(this));
	mAvailableComponents.push_back(new ComponentBackgroundSubtractionColor(this));
	mAvailableComponents.push_back(new ComponentAdaptiveBackgroundSubtractionColor(this));
	mAvailableComponents.push_back(new ComponentAdaptiveBackgroundSubtractionMedian(this));
	mAvailableComponents.push_back(new ComponentSpecificColorSubtraction(this));
	mAvailableComponents.push_back(new ComponentColorMask(this));
	mAvailableComponents.push_back(new ComponentColorBlur(this));
	mAvailableComponents.push_back(new ComponentColorGaussianBlur(this));
	mAvailableComponents.push_back(new ComponentCorrectMean(this));
	mAvailableComponents.push_back(new ComponentConvertColorToGray(this));
	mAvailableComponents.push_back(new ComponentWeightedSum(this));
	mAvailableComponents.push_back(new ComponentThresholdColorIndependent(this));
	mAvailableComponents.push_back(new ComponentDoubleThresholdColorIndependent(this));
	mAvailableComponents.push_back(new ComponentAdaptiveThreshold(this));
	mAvailableComponents.push_back(new ComponentColorSwapper(this));

	// Grayscale processing
	mAvailableComponents.push_back(new ComponentCropGray(this));
	mAvailableComponents.push_back(new ComponentChannelCalculationGray(this));
	mAvailableComponents.push_back(new ComponentBackgroundSubtractionGray(this));
	mAvailableComponents.push_back(new ComponentAdaptiveBackgroundSubtractionGray(this));
	mAvailableComponents.push_back(new ComponentGrayMask(this));
	mAvailableComponents.push_back(new ComponentConvertGrayToColor(this));
	mAvailableComponents.push_back(new ComponentThresholdGray(this));
	mAvailableComponents.push_back(new ComponentCannyEdgeDetection(this));
	mAvailableComponents.push_back(new ComponentSobelDifferentiation(this));

	// Binary processing
	mAvailableComponents.push_back(new ComponentCropBinary(this));
	mAvailableComponents.push_back(new ComponentMorphology(this));
	mAvailableComponents.push_back(new ComponentBinaryDilation(this));
	mAvailableComponents.push_back(new ComponentBinaryErosion(this));
	mAvailableComponents.push_back(new ComponentBinaryMask(this));
	mAvailableComponents.push_back(new ComponentInvertBinary(this));
	mAvailableComponents.push_back(new ComponentBinaryBlur(this));
	mAvailableComponents.push_back(new ComponentConvertBinaryToColor(this));
	mAvailableComponents.push_back(new ComponentConvertBinaryToGray(this));

	// Particle Detection
	mAvailableComponents.push_back(new ComponentBlobDetectionMinMax(this));
	mAvailableComponents.push_back(new ComponentBlobDetectionTwoColors(this));
	mAvailableComponents.push_back(new ComponentBlobDetectionRedGreen(this));
	mAvailableComponents.push_back(new ComponentBlobDetectionCircularHough(this));
	mAvailableComponents.push_back(new ComponentIDReaderRing(this));
	mAvailableComponents.push_back(new ComponentSimulationParticles(this));
	mAvailableComponents.push_back(new ComponentFilterParticles(this));

	// Calibration
	mAvailableComponents.push_back(new ComponentCalibrationLinear(this));
	mAvailableComponents.push_back(new ComponentCalibrationTSAI(this));
	mAvailableComponents.push_back(new ComponentCalibrationFileTSAI(this));

	// Tracking
	mAvailableComponents.push_back(new ComponentTracking(this));
	mAvailableComponents.push_back(new ComponentTrackSmoothing(this));

	// Output
	mAvailableComponents.push_back(new ComponentOutputFile(this));
	mAvailableComponents.push_back(new ComponentOutputSQLite(this));
	mAvailableComponents.push_back(new ComponentOutputFileAVI(this));
	mAvailableComponents.push_back(new ComponentOutputBufferedFileAVI(this));
	mAvailableComponents.push_back(new ComponentOutputFramesImages(this));
	mAvailableComponents.push_back(new ComponentOutputImageStatisticsBinary(this));
	mAvailableComponents.push_back(new ComponentOutputImageStatisticsColor(this));
	mAvailableComponents.push_back(new ComponentOutputImageStatisticsGray(this));
	mAvailableComponents.push_back(new ComponentOutputImageOverlayColor(this));
	mAvailableComponents.push_back(new ComponentOutputMarkFrameManual(this));
	mAvailableComponents.push_back(new ComponentOutputParticles(this));

	// Initialize the available components
	tComponentList::iterator ita = mAvailableComponents.begin();
	while (ita != mAvailableComponents.end()) {
		(*ita)->OnInitializeStatic();
		ita++;
	}

	// Initialize the list of available data structures
	mDataStructures.push_back(&mDataStructureInput);
	mDataStructures.push_back(&mDataStructureImageColor);
	mDataStructures.push_back(&mDataStructureImageGray);
	//mDataStructures.push_back(&mDataStructureImageFFT);
	mDataStructures.push_back(&mDataStructureImageBinary);
	mDataStructures.push_back(&mDataStructureParticles);
	mDataStructures.push_back(&mDataStructureTracks);
	mDataStructures.push_back(&mDataStructureCommands);

}

THISCLASS::~SwisTrackCore() {
	Stop();

	// Delete deployed components
	tComponentList::iterator itd = mDeployedComponents.begin();
	while (itd != mDeployedComponents.end()) {
		delete (*itd);
		itd++;
	}

	mDeployedComponents.clear();

	// Terminate and delete available components
	tComponentList::iterator ita = mAvailableComponents.begin();
	while (ita != mAvailableComponents.end()) {
		(*ita)->OnTerminateStatic();
		delete (*ita);
		ita++;
	}

	mAvailableComponents.clear();

	// Delete associated objects
	delete mTrigger;
	delete mEventRecorder;
}

void THISCLASS::SetFileName(const wxFileName & filename) {
	mFileName = filename;
}

wxFileName THISCLASS::GetProjectFileName(const wxString & filenameString) {
	// Check filename
	wxFileName filename(filenameString);
	if (! filename.IsOk()) return wxFileName();

	// Normalize
	if (filename.IsAbsolute()) {
		filename.Normalize(wxPATH_NORM_ENV_VARS | wxPATH_NORM_DOTS | wxPATH_NORM_ABSOLUTE | wxPATH_NORM_TILDE);
	} else {
		// If there is no valid project folder (i.e. the project has not been saved yet), we give up
		if (! mFileName.IsOk()) return wxFileName();

		// Normalize with respect to project folder
		filename.Normalize(wxPATH_NORM_ENV_VARS | wxPATH_NORM_DOTS | wxPATH_NORM_ABSOLUTE | wxPATH_NORM_TILDE, mFileName.GetPath());
	}

	return filename;
}

wxFileName THISCLASS::GetRunFileName(const wxString & filenameString) {
	// Check filename
	wxFileName filename(filenameString);
	if (! filename.IsOk()) return wxFileName();

	// Normalize
	if (filename.IsAbsolute()) {
		filename.Normalize(wxPATH_NORM_ENV_VARS | wxPATH_NORM_DOTS | wxPATH_NORM_ABSOLUTE | wxPATH_NORM_TILDE);
	} else {
		// If there is no valid project folder (i.e. the project has not been saved yet), we give up
		if (! mFileName.IsOk()) return wxFileName();

		// Normalize with respect to run folder
		wxFileName run_folder(mFileName);
		run_folder.AppendDir(mRunTitle);
		filename.Normalize(wxPATH_NORM_ENV_VARS | wxPATH_NORM_DOTS | wxPATH_NORM_ABSOLUTE | wxPATH_NORM_TILDE, run_folder.GetPath());
	}

	return filename;
}

bool THISCLASS::Start(bool productionMode) {
	if (mStarted) return false;
	if (mEditLocks > 0) return false;

	// Timestamp of start
	wxDateTime now = wxDateTime::Now();

	// Prepare the run folder
	if (productionMode) {
		// Find a run folder which does not exist yet
		int unique_id = 1;
		wxString runtitlebase = now.Format(wxT("%Y-%m-%d-%H-%M-%S"));
		mRunTitle = runtitlebase;
		while (1) {
			wxFileName filename_output = GetRunFileName(wxT("output"));
			if (! filename_output.IsOk()) break;
			if (! filename_output.DirExists()) break;
			unique_id++;
			mRunTitle = runtitlebase + wxString::Format(wxT("-%d"), unique_id);
		}
	} else {
		// Always use the folder "testing"
		mRunTitle = wxT("testing");
	}

	// Create the run folder and open the main output file
	wxFileName filename_output = GetRunFileName(wxT("output"));
	if (filename_output.IsOk()) {
		filename_output.Mkdir(0777, wxPATH_MKDIR_FULL);
		//wxFFile file_output(filename_output.GetFullPath(), wxT("w"));
		//file_output.Write(wxT("$TIMESTAMP,") + now.Format() + wxT("\n"));
	}

	// Event recorder
	mEventRecorder->Add(productionMode ?
		SwisTrackCoreEventRecorder::sType_SetModeProduction :
		SwisTrackCoreEventRecorder::sType_SetModeNormal
		);
	mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_BeforeStart);

	// Notify the interfaces
	for (auto interface : mSwisTrackCoreInterfaces)
		interface->OnBeforeStart(productionMode);

	// Update the flags
	mStarted = true;
	mProductionMode = productionMode;
	mStepCounter = 0;

	// Start all components (until first error)
	for (auto component : mDeployedComponents) {
		component->ClearStatus();
		component->OnStart();
		if (component->mStatusHasError) break;
		component->mStarted = true;
	}

	// Notify the clients
	if (mCommunicationInterface) {
		CommunicationMessage mstart(wxT("START"));
		mstart.AddString(now.Format());
		mCommunicationInterface->Send(&mstart);
	}

	// Notify the interfaces
	for (auto interface : mSwisTrackCoreInterfaces)
		interface->OnAfterStart(productionMode);

	// Event recorder
	mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_AfterStart);

	return true;
}

bool THISCLASS::Stop() {
	if (! mStarted) return false;

	// Event recorder
	mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_BeforeStop);

	// Notify the interfaces
	for (auto interface : mSwisTrackCoreInterfaces)
		interface->OnBeforeStop();

	// Stop all components
	tComponentList::iterator it = mDeployedComponents.end();
	while (it != mDeployedComponents.begin()) {
		it--;
		if (! (*it)->mStarted) continue;
		(*it)->ClearStatus();
		(*it)->OnStop();
		(*it)->mStarted = false;
	}

	// Update flags
	mStarted = false;
	mProductionMode = false;

	// Notify the clients
	if (mCommunicationInterface) {
		wxDateTime now = wxDateTime::Now();
		CommunicationMessage mstop(wxT("STOP"));
		mstop.AddString(now.Format());
		mCommunicationInterface->Send(&mstop);
	}

	// Notify the interfaces
	for (auto interface : mSwisTrackCoreInterfaces)
		interface->OnAfterStop();

	// Event recorder
	mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_AfterStop);

	// Reset the run folder
	mRunTitle = wxT("");
	return true;
}

bool THISCLASS::Step() {
	if (! mStarted) return false;

	// Initialization
	mTimeCritical = false;

	// Event recorder
	mEventRecorder->AddStepStart();

	// Notify the interfaces (OnBeforeStep)
	for (auto interface : mSwisTrackCoreInterfaces)
		interface->OnBeforeStep();

	// Notify the displays (OnBeforeStep)
	for (auto component : mDeployedComponents)
		for (auto display : component->mDisplays)
			display->OnBeforeStep();

	// Notify the clients
	if (mCommunicationInterface) {
		CommunicationMessage mstart(wxT("STEP_START"));
		mCommunicationInterface->Send(&mstart);
	}

	// Reset the step durations
	for (auto component : mDeployedComponents)
		component->mStepDuration = -1;

	// Run until first error, or until the end (all started components)
	auto it = mDeployedComponents.begin();
	while (it != mDeployedComponents.end()) {
		int enabledInterval = (*it)->GetEnabledInterval();
		if ((*it)->mStarted && ! (*it)->mHasReloadError && enabledInterval > 0 && mStepCounter % enabledInterval == 0) {
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
			(*it)->mStepDuration = mEventRecorder->CalculateDuration(&starttime, &endtime);

			// Error handling
			if ((*it)->mStatusHasError) break;
		}

		it++;
	}

	// Notify the interfaces (OnStepReady)
	for (auto interface : mSwisTrackCoreInterfaces)
		interface->OnStepReady();

	// and then cleanup what we run
	while (it != mDeployedComponents.begin()) {
		it--;
		(*it)->OnStepCleanup();
	}

	// Notify the clients
	if (mCommunicationInterface) {
		CommunicationMessage mstop(wxT("STEP_STOP"));
		mCommunicationInterface->Send(&mstop);
	}

	// Notify the displays (OnAfterStep)
	for (auto component : mDeployedComponents)
		for (auto display : component->mDisplays)
			display->OnAfterStep();

	// Notify the interfaces (OnAfterStep)
	for (auto interface : mSwisTrackCoreInterfaces)
		interface->OnAfterStep();

	// Event recorder
	mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_StepStop);

	mStepCounter++;
	return true;
}

bool THISCLASS::ReloadConfiguration() {
	if (! mStarted) return false;

	// Event recorder
	mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_BeforeReloadConfiguration);

	// Reload the configuration of all components
	for (auto component : mDeployedComponents) {
		if (! component->mStarted) continue;
		component->ClearStatus();
		component->OnReloadConfiguration();
		component->mHasReloadError = component->mStatusHasError;
	}

	// Event recorder
	mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_AfterReloadConfiguration);
	return true;
}

bool THISCLASS::ReloadConfigurationOfSingleComponent(Component * component) {
	if (! mStarted) return false;
	if (! component->mStarted) return false;

	// Event recorder
	mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_BeforeReloadConfiguration);

	// Reload the configuration of this component
	component->ClearStatus();
	component->OnReloadConfiguration();
	component->mHasReloadError = component->mStatusHasError;

	// Event recorder
	mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_AfterReloadConfiguration);
	return true;
}

void THISCLASS::TriggerStart() {
	if (mTrigger->GetActive()) return;

	// Event recorder
	mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_BeforeTriggerStart);

	// Notify the interfaces
	for (auto interface : mSwisTrackCoreInterfaces)
		interface->OnBeforeTriggerStart();

	// Activate the trigger
	mTrigger->SetActive(true);

	// Notify the interfaces
	for (auto interface : mSwisTrackCoreInterfaces)
		interface->OnAfterTriggerStart();

	// Notify the clients
	if (mCommunicationInterface) {
		CommunicationMessage mstart(wxT("TRIGGER_START"));
		mCommunicationInterface->Send(&mstart);
	}

	// Event recorder
	mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_AfterTriggerStart);
}

void THISCLASS::TriggerStop() {
	if (! mTrigger->GetActive()) return;

	// Event recorder
	mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_BeforeTriggerStop);

	// Notify the interfaces
	for (auto interface : mSwisTrackCoreInterfaces)
		interface->OnBeforeTriggerStop();

	// Deactivate the trigger
	mTrigger->SetActive(false);

	// Notify the interfaces
	for (auto interface : mSwisTrackCoreInterfaces)
		interface->OnAfterTriggerStop();

	// Notify the clients
	if (mCommunicationInterface) {
		CommunicationMessage mstop(wxT("TRIGGER_STOP"));
		mCommunicationInterface->Send(&mstop);
	}

	// Event recorder
	mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_AfterTriggerStop);
}

void THISCLASS::ConfigurationWriteXML(wxXmlNode * configuration, ErrorList * xmlerr) {
	// Add an element for each component
	for (auto component : mDeployedComponents) {
		wxXmlNode * node = new wxXmlNode(0, wxXML_ELEMENT_NODE, wxT("component"));
		configuration->AddChild(node);
		node->AddAttribute(wxT("type"), component->mName);
		component->ConfigurationWriteXML(node, xmlerr);
	}
}

Component * THISCLASS::GetComponentByName(const wxString & name) {
	for (auto component : mAvailableComponents)
		if (component->mName == name) return component;

	return NULL;
}

void THISCLASS::AddInterface(SwisTrackCoreInterface * stc) {
	mSwisTrackCoreInterfaces.push_back(stc);
}

void THISCLASS::RemoveInterface(SwisTrackCoreInterface * stc) {
	tSwisTrackCoreInterfaceList::iterator it = find(mSwisTrackCoreInterfaces.begin(), mSwisTrackCoreInterfaces.end(), stc);
	if (it == mSwisTrackCoreInterfaces.end()) return;
	mSwisTrackCoreInterfaces.erase(it);
}

void THISCLASS::OnIdle() {
	mTrigger->OnIdle();
}

bool THISCLASS::IncrementEditLocks() {
	if (mEditLocks > 0) {
		mEditLocks++;
		return true;
	}

	// If started in production mode, editing is not allowed
	if (IsStartedInProductionMode())
		return false;

	// If started in test mode, stop and allow editing
	Stop();

	// Notify the interfaces
	for (auto interface : mSwisTrackCoreInterfaces)
		interface->OnBeforeEdit();

	mEditLocks = 1;
	return true;
}

void THISCLASS::DecrementEditLocks() {
	if (mEditLocks < 1) return;

	// Decrement and return if there are still other locks
	mEditLocks--;
	if (mEditLocks > 0) return;

	// Notify the interfaces
	for (auto interface : mSwisTrackCoreInterfaces)
		interface->OnAfterEdit();
}
