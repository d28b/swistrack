#include "SwisTrackCore.h"
#define THISCLASS SwisTrackCore

#include <algorithm>
#include "ComponentTriggerTimer.h"
#include "ComponentTriggerCounter.h"
#include "ComponentInputCamera1394.h"
#include "ComponentInputCameraUSB.h"
#include "ComponentInputCameraGigE.h"
#include "ComponentInputCameraARVGigE.h"
#include "ComponentInputCameraProselicaGigE.h"
#include "ComponentInputFileAVI.h"
#include "ComponentInputFileImage.h"
#include "ComponentConvertToGray.h"
#include "ComponentConvertToColor.h"
#include "ComponentFFT.h"
#include "ComponentResize.h"
#include "ComponentInverseFFT.h"
#include "ComponentFrequencyFilter.h"
#include "ComponentCannyEdgeDetection.h"
#include "ComponentConvertBayerToColor.h"
#include "ComponentChannelArithmetic.h"
#include "ComponentBackgroundSubtractionGray.h"
#include "ComponentBackgroundSubtractionColor.h"
#include "ComponentHSVBackgroundSubtractionColor.h"
#include "ComponentAdaptiveBackgroundSubtractionGray.h"
#include "ComponentAdaptiveBackgroundSubtractionColor.h"
#include "ComponentAdaptiveBackgroundSubtractionMedian.h"
#include "ComponentAdaptiveBackgroundSubtractionMode.h"
#include "ComponentBackgroundSubtractionCheungKamath.h"
#include "ComponentGrayMask.h"
#include "ComponentSpecificColorSubtraction.h"
#include "ComponentColorMask.h"
#include "ComponentColorBlur.h"
#include "ComponentColorSwapper.h"
#include "ComponentMoveGrayToColor.h"
#include "ComponentMoveBinaryToColor.h"
#include "ComponentMoveColorToBinary.h"
#include "ComponentMoveColorToGray.h"
#include "ComponentThresholdGray.h"
#include "ComponentThresholdColorCommon.h"
#include "ComponentThresholdColorIndependent.h"
#include "ComponentAdaptiveThreshold.h"
#include "ComponentBinaryDilation.h"
#include "ComponentMorphology.h"
#include "ComponentBinaryErosion.h"
#include "ComponentBinaryMask.h"
#include "ComponentInvertBinary.h"
#include "ComponentBlobSelection.h"
#include "ComponentBlobDetectionMinMax.h"
#include "ComponentBlobDetectionTwoColors.h"
#include "ComponentBlobDetectionRedGreen.h"
#include "ComponentBlobDetectionCircularHough.h"
#include "ComponentTracking.h"
#include "ComponentNearestNeighborTracking.h"
#include "ComponentDynamicNearestNeighborTracking.h"
#include "ComponentCamShiftTracking.h"
#include "ComponentMinCostFlowTracking.h"
#include "ComponentMotionTemplateParticleDetector.h"
#include "ComponentFilterParticles.h"
#include "ComponentTrackSmoothing.h"
#include "ComponentKalmanFilterTrack.h"
#include "ComponentIDReaderRing.h"
#include "ComponentSimulationParticles.h"
#include "ComponentFourierCorrelation.h"
#include "ComponentFourierPatternTracker.h"
#include "ComponentColorHistogramParticles.h"
#include "ComponentCalibrationLinear.h"
#include "ComponentCalibrationOpenCV.h"
#include "ComponentCalibrationTSAI.h"
#include "ComponentCalibrationFileTSAI.h"
#include "ComponentOutputFile.h"
#include "ComponentOutputSQLite.h"
#include "ComponentOutputFileAVI.h"
#include "ComponentOutputFileM4V.h"
#include "ComponentOutputFramesImages.h"
#include "ComponentOutputImageStatisticsBinary.h"
#include "ComponentOutputImageStatisticsColor.h"
#include "ComponentOutputImageStatisticsGray.h"
#include "ComponentOutputImageOverlayColor.h"
#include "ComponentOutputMarkFrameManual.h"
#include "ComponentOutputParticles.h"
#include "ComponentOutputDataAssociationTraining.h"
#include "ComponentClassifierTracker.h"
#include "ComponentDoubleThresholdColorIndependent.h"
#include "ComponentSobelDifferentiation.h"
#include "ComponentBinarySmooth.h"

#include "ComponentChamberControl.h"
#include "ComponentPiezoControl.h"
#include "ComponentGraphControl.h"


#include "NMEALog.h"
THISCLASS::SwisTrackCore(wxString componentconfigurationfolder):
		mAvailableComponents(), mDataStructures(), mSwisTrackCoreInterfaces(), mComponentConfigurationFolder(componentconfigurationfolder),
		mComponentCategories(), mCommunicationInterface(0), mTrigger(new SwisTrackCoreTrigger(this)), mEventRecorder(new SwisTrackCoreEventRecorder(this)),
		mCategoryTrigger(wxT("Trigger"), wxT("Trigger"), 0),
		mCategoryInput(wxT("Input"), wxT("Input"), 100, ComponentCategory::sTypeOne),
		mCategoryInputConversion(wxT("InputConversion"), wxT("Input conversion"), 200, ComponentCategory::sTypeAuto),
		mCategoryPreprocessingColor(wxT("PreprocessingColor"), wxT("Preprocessing (color)"), 300),
		mCategoryPreprocessingGray(wxT("PreprocessingGray"), wxT("Preprocessing (grayscale)"), 350),
		mCategoryThresholdingColor(wxT("ThresholdingColor"), wxT("Thresholding (color)"), 400),
		mCategoryThresholdingGray(wxT("ThresholdingGray"), wxT("Thresholding (grayscale)"), 450),
		mCategoryPreprocessingBinary(wxT("PreprocessingBinary"), wxT("Preprocessing (binary)"), 500),
		mCategoryParticleDetection(wxT("ParticleDetection"), wxT("Particle detection"), 600),
		mCategoryCalibration(wxT("Calibration"), wxT("Calibration"), 700),
		mCategoryTracking(wxT("Tracking"), wxT("Tracking"), 800),
		mCategoryOutput(wxT("Output"), wxT("Output"), 10000),
		mDataStructureInput(),
		mDataStructureImageColor(wxT("ImageColor"), wxT("Color image")),
		mDataStructureImageGray(wxT("ImageGray"), wxT("Grayscale image")),
		mDataStructureImageFFT(wxT("ImageFFT"), wxT("Fourier image")),
		mDataStructureImageBinary(wxT("ImageBinary"), wxT("Binary image")),
		mDataStructureParticles(),
		mDataStructureTracks(),
		mDataStructureCommands(),
		mStarted(false), mProductionMode(false), mStepCounter(0), mEditLocks(0), mDeployedComponents() {


	// Initialize the list of available components
	//Trigger
	mAvailableComponents.push_back(new ComponentTriggerTimer(this));
	mAvailableComponents.push_back(new ComponentTriggerCounter(this));
	//Input
	mAvailableComponents.push_back(new ComponentInputCamera1394(this));
	mAvailableComponents.push_back(new ComponentInputCameraUSB(this));
	mAvailableComponents.push_back(new ComponentInputCameraGigE(this));
	mAvailableComponents.push_back(new ComponentInputCameraARVGigE(this));
	mAvailableComponents.push_back(new ComponentInputCameraProselicaGigE(this));
	mAvailableComponents.push_back(new ComponentInputFileAVI(this));
	mAvailableComponents.push_back(new ComponentInputFileImage(this));
	//Input conversion
	mAvailableComponents.push_back(new ComponentConvertToGray(this));
	mAvailableComponents.push_back(new ComponentConvertToColor(this));
	mAvailableComponents.push_back(new ComponentConvertBayerToColor(this));
	mAvailableComponents.push_back(new ComponentFFT(this));
	mAvailableComponents.push_back(new ComponentInverseFFT(this));
	mAvailableComponents.push_back(new ComponentResize(this));
	//Preprocessing Color
	mAvailableComponents.push_back(new ComponentChannelArithmetic(this));
	mAvailableComponents.push_back(new ComponentBackgroundSubtractionColor(this));
	mAvailableComponents.push_back(new ComponentHSVBackgroundSubtractionColor(this));
	mAvailableComponents.push_back(new ComponentAdaptiveBackgroundSubtractionColor(this));
	mAvailableComponents.push_back(new ComponentAdaptiveBackgroundSubtractionMedian(this));
	mAvailableComponents.push_back(new ComponentAdaptiveBackgroundSubtractionMode(this));
	mAvailableComponents.push_back(new ComponentBackgroundSubtractionCheungKamath(this));
	mAvailableComponents.push_back(new ComponentSpecificColorSubtraction(this));
	mAvailableComponents.push_back(new ComponentColorMask(this));
	mAvailableComponents.push_back(new ComponentColorBlur(this));
	mAvailableComponents.push_back(new ComponentColorSwapper(this));
	mAvailableComponents.push_back(new ComponentMoveColorToBinary(this));
	mAvailableComponents.push_back(new ComponentMoveColorToGray(this));
	mAvailableComponents.push_back(new ComponentMoveGrayToColor(this));
	mAvailableComponents.push_back(new ComponentMoveBinaryToColor(this));
	mAvailableComponents.push_back(new ComponentCannyEdgeDetection(this));
	//Preprocessing Gray
	mAvailableComponents.push_back(new ComponentBackgroundSubtractionGray(this));
	mAvailableComponents.push_back(new ComponentAdaptiveBackgroundSubtractionGray(this));
	mAvailableComponents.push_back(new ComponentGrayMask(this));
	mAvailableComponents.push_back(new ComponentFrequencyFilter(this));
	mAvailableComponents.push_back(new ComponentFourierCorrelation(this));
	//Thresholding Color
	mAvailableComponents.push_back(new ComponentThresholdColorCommon(this));
	mAvailableComponents.push_back(new ComponentThresholdColorIndependent(this));
	mAvailableComponents.push_back(new ComponentDoubleThresholdColorIndependent(this));
	mAvailableComponents.push_back(new ComponentAdaptiveThreshold(this));
	//Thresholding Gray
	mAvailableComponents.push_back(new ComponentThresholdGray(this));
	//Preprocessing Binary
	mAvailableComponents.push_back(new ComponentBinaryDilation(this));
	mAvailableComponents.push_back(new ComponentMorphology(this));
	mAvailableComponents.push_back(new ComponentBinaryErosion(this));
	mAvailableComponents.push_back(new ComponentBinaryMask(this));
	mAvailableComponents.push_back(new ComponentBlobSelection(this));
	mAvailableComponents.push_back(new ComponentInvertBinary(this));
	mAvailableComponents.push_back(new ComponentBinarySmooth(this));
	//Particle Detection
	mAvailableComponents.push_back(new ComponentBlobDetectionMinMax(this));
	mAvailableComponents.push_back(new ComponentBlobDetectionTwoColors(this));
	mAvailableComponents.push_back(new ComponentBlobDetectionRedGreen(this));
	mAvailableComponents.push_back(new ComponentBlobDetectionCircularHough(this));
	mAvailableComponents.push_back(new ComponentSobelDifferentiation(this));
  	mAvailableComponents.push_back(new ComponentIDReaderRing(this));
	mAvailableComponents.push_back(new ComponentSimulationParticles(this));
	mAvailableComponents.push_back(new ComponentFourierPatternTracker(this));
	
	//Calibration	
	mAvailableComponents.push_back(new ComponentCalibrationLinear(this));
	mAvailableComponents.push_back(new ComponentCalibrationOpenCV(this));
	mAvailableComponents.push_back(new ComponentCalibrationTSAI(this));
	mAvailableComponents.push_back(new ComponentCalibrationFileTSAI(this));
	//Tracking
	mAvailableComponents.push_back(new ComponentTracking(this));
	mAvailableComponents.push_back(new ComponentNearestNeighborTracking(this));
	mAvailableComponents.push_back(new ComponentDynamicNearestNeighborTracking(this));
	mAvailableComponents.push_back(new ComponentCamShiftTracking(this));
	mAvailableComponents.push_back(new ComponentMinCostFlowTracking(this));
	mAvailableComponents.push_back(new ComponentMotionTemplateParticleDetector(this));
	mAvailableComponents.push_back(new ComponentFilterParticles(this));
	mAvailableComponents.push_back(new ComponentTrackSmoothing(this));
	mAvailableComponents.push_back(new ComponentKalmanFilterTrack(this));

	//Output
	mAvailableComponents.push_back(new ComponentOutputFile(this));
  	mAvailableComponents.push_back(new ComponentOutputSQLite(this));
	mAvailableComponents.push_back(new ComponentOutputFileAVI(this));
	mAvailableComponents.push_back(new ComponentOutputFileM4V(this));
	mAvailableComponents.push_back(new ComponentOutputFramesImages(this));
	mAvailableComponents.push_back(new ComponentOutputImageStatisticsBinary(this));
	mAvailableComponents.push_back(new ComponentOutputImageStatisticsColor(this));
	mAvailableComponents.push_back(new ComponentOutputImageStatisticsGray(this));
	mAvailableComponents.push_back(new ComponentOutputImageOverlayColor(this));
	mAvailableComponents.push_back(new ComponentOutputMarkFrameManual(this));
	mAvailableComponents.push_back(new ComponentOutputParticles(this));
	mAvailableComponents.push_back(new ComponentOutputDataAssociationTraining(this));
	mAvailableComponents.push_back(new ComponentClassifierTracker(this));
 
 	mAvailableComponents.push_back(new ComponentGraphControl(this));
	mAvailableComponents.push_back(new ComponentChamberControl(this));
 	mAvailableComponents.push_back(new ComponentPiezoControl(this));
 

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
	mDataStructures.push_back(&mDataStructureImageFFT);
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

void THISCLASS::SetFileName(const wxFileName &filename) {
	mFileName = filename;
}

wxFileName THISCLASS::GetProjectFileName(const wxString &filename_str) {
	// Check filename
	wxFileName filename(filename_str);
	if (! filename.IsOk()) {
		return wxFileName();
	}

	// Normalize
	if (filename.IsAbsolute()) {
		filename.Normalize(wxPATH_NORM_ENV_VARS | wxPATH_NORM_DOTS | wxPATH_NORM_ABSOLUTE | wxPATH_NORM_TILDE);
	} else {
		// If there is no valid project folder (i.e. the project has not been saved yet), we give up
		if (! mFileName.IsOk()) {
			return wxFileName();
		}

		// Normalize with respect to project folder
		filename.Normalize(wxPATH_NORM_ENV_VARS | wxPATH_NORM_DOTS | wxPATH_NORM_ABSOLUTE | wxPATH_NORM_TILDE, mFileName.GetPath());
	}

	return filename;
}

wxFileName THISCLASS::GetRunFileName(const wxString &filename_str) {
	// Check filename
	wxFileName filename(filename_str);
	if (! filename.IsOk()) {
		return wxFileName();
	}

	// Normalize
	if (filename.IsAbsolute()) {
		filename.Normalize(wxPATH_NORM_ENV_VARS | wxPATH_NORM_DOTS | wxPATH_NORM_ABSOLUTE | wxPATH_NORM_TILDE);
	} else {
		// If there is no valid project folder (i.e. the project has not been saved yet), we give up
		if (! mFileName.IsOk()) {
			return wxFileName();
		}

		// Normalize with respect to run folder
		wxFileName run_folder(mFileName);
		run_folder.AppendDir(mRunTitle);
		filename.Normalize(wxPATH_NORM_ENV_VARS | wxPATH_NORM_DOTS | wxPATH_NORM_ABSOLUTE | wxPATH_NORM_TILDE, run_folder.GetPath());
	}

	return filename;
}

bool THISCLASS::Start(bool productionmode) {
	if (mStarted) {
		return false;
	}
	if (mEditLocks > 0) {
		return false;
	}

	// Timestamp of start
	wxDateTime now = wxDateTime::Now();

	// Prepare the run folder
	if (productionmode) {
		// Find a run folder which does not exist yet
		int unique_id = 1;
		wxString runtitlebase = now.Format(wxT("%Y-%m-%d-%H-%M-%S"));
		mRunTitle = runtitlebase;
		while (1) {
			wxFileName filename_output = GetRunFileName(wxT("output"));
			if (! filename_output.IsOk()) {
				break;
			}
			if (! filename_output.DirExists()) {
				break;
			}
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
	if (productionmode) {
		mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_SetModeProduction);
	} else {
		mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_SetModeNormal);
	}
	mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_BeforeStart);

	// Notify the interfaces
	tSwisTrackCoreInterfaceList::iterator iti = mSwisTrackCoreInterfaces.begin();
	while (iti != mSwisTrackCoreInterfaces.end()) {
		(*iti)->OnBeforeStart(productionmode);
		iti++;
	}

	// Update the flags
	mStarted = true;
	mProductionMode = productionmode;
	mStepCounter = 0;

	// Start all components (until first error)
	tComponentList::iterator it = mDeployedComponents.begin();
	while (it != mDeployedComponents.end()) {
		(*it)->ClearStatus();
		(*it)->OnStart();
		if ((*it)->mStatusHasError) {
			break;
		}
		(*it)->mStarted = true;
		it++;
	}

	// Notify the clients
	if (mCommunicationInterface) {
		CommunicationMessage mstart(wxT("START"));
		mstart.AddString(now.Format());
		mCommunicationInterface->Send(&mstart);
	}

	// Notify the interfaces
	iti = mSwisTrackCoreInterfaces.begin();
	while (iti != mSwisTrackCoreInterfaces.end()) {
		(*iti)->OnAfterStart(productionmode);
		iti++;
	}

	// Event recorder
	mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_AfterStart);

	return true;
}

bool THISCLASS::Stop() {
	if (! mStarted) {
		return false;
	}

	// Event recorder
	mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_BeforeStop);

	// Notify the interfaces
	tSwisTrackCoreInterfaceList::iterator iti = mSwisTrackCoreInterfaces.begin();
	while (iti != mSwisTrackCoreInterfaces.end()) {
		(*iti)->OnBeforeStop();
		iti++;
	}

	// Stop all components
	tComponentList::iterator it = mDeployedComponents.end();
	while (it != mDeployedComponents.begin()) {
		it--;
		if ((*it)->mStarted) {
			(*it)->ClearStatus();
			(*it)->OnStop();
			(*it)->mStarted = false;
		}
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
	iti = mSwisTrackCoreInterfaces.begin();
	while (iti != mSwisTrackCoreInterfaces.end()) {
		(*iti)->OnAfterStop();
		iti++;
	}

	// Event recorder
	mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_AfterStop);

	// Reset the run folder
	mRunTitle = wxT("");

	return true;
}

bool THISCLASS::Step() {
	if (! mStarted) {
		return false;
	}

	// Initialization
	mTimeCritical = false;

	// Event recorder
	mEventRecorder->AddStepStart();

	// Notify the interfaces (OnBeforeStep)
	tSwisTrackCoreInterfaceList::iterator iti = mSwisTrackCoreInterfaces.begin();
	while (iti != mSwisTrackCoreInterfaces.end()) {
		(*iti)->OnBeforeStep();
		iti++;
	}

	// Notify the displays (OnBeforeStep)
	tComponentList::iterator it = mDeployedComponents.begin();
	while (it != mDeployedComponents.end()) {
		Component::tDisplayList::iterator itdi = (*it)->mDisplays.begin();
		while (itdi != (*it)->mDisplays.end()) {
			(*itdi)->OnBeforeStep();
			itdi++;
		}
		it++;
	}

	// Notify the clients
	if (mCommunicationInterface) {
		CommunicationMessage mstart(wxT("STEP_START"));
		mCommunicationInterface->Send(&mstart);
	}

	// Reset the step durations
	it = mDeployedComponents.begin();
	while (it != mDeployedComponents.end()) {
		(*it)->mStepDuration = -1;
		it++;
	}

	// Run until first error, or until the end (all started components)
	it = mDeployedComponents.begin();
	while (it != mDeployedComponents.end()) {
		int enabledinterval = (*it)->GetEnabledInterval();
		if (((*it)->mStarted) && (enabledinterval > 0) && (mStepCounter % enabledinterval == 0)) {
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
			if ((*it)->mStatusHasError) {
				break;
			}
		}

		it++;
	}

	// Notify the interfaces (OnStepReady)
	iti = mSwisTrackCoreInterfaces.begin();
	while (iti != mSwisTrackCoreInterfaces.end()) {
		(*iti)->OnStepReady();
		iti++;
	}

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
	it = mDeployedComponents.begin();
	while (it != mDeployedComponents.end()) {
		Component::tDisplayList::iterator itdi = (*it)->mDisplays.begin();
		while (itdi != (*it)->mDisplays.end()) {
			(*itdi)->OnAfterStep();
			itdi++;
		}
		it++;
	}

	// Notify the interfaces (OnAfterStep)
	iti = mSwisTrackCoreInterfaces.begin();
	while (iti != mSwisTrackCoreInterfaces.end()) {
		(*iti)->OnAfterStep();
		iti++;
	}

	// Event recorder
	mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_StepStop);

	mStepCounter++;
	return true;
}

bool THISCLASS::ReloadConfiguration() {
	if (! mStarted) {
		return false;
	}

	// Event recorder
	mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_BeforeReloadConfiguration);

	// Start all components (until first error)
	tComponentList::iterator it = mDeployedComponents.begin();
	while (it != mDeployedComponents.end()) {
		if ((*it)->mStarted) {
			(*it)->ClearStatus();
			(*it)->OnReloadConfiguration();
		}
		it++;
	}

	// Event recorder
	mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_AfterReloadConfiguration);

	return true;
}

void THISCLASS::TriggerStart() {
	if (mTrigger->GetActive()) {
		return;
	}

	// Event recorder
	mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_BeforeTriggerStart);

	// Notify the interfaces
	tSwisTrackCoreInterfaceList::iterator it = mSwisTrackCoreInterfaces.begin();
	while (it != mSwisTrackCoreInterfaces.end()) {
		(*it)->OnBeforeTriggerStart();
		it++;
	}

	// Activate the trigger
	mTrigger->SetActive(true);

	// Notify the interfaces
	it = mSwisTrackCoreInterfaces.begin();
	while (it != mSwisTrackCoreInterfaces.end()) {
		(*it)->OnAfterTriggerStart();
		it++;
	}

	// Notify the clients
	if (mCommunicationInterface) {
		CommunicationMessage mstart(wxT("TRIGGER_START"));
		mCommunicationInterface->Send(&mstart);
	}

	// Event recorder
	mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_AfterTriggerStart);
}

void THISCLASS::TriggerStop() {
	if (! mTrigger->GetActive()) {
		return;
	}

	// Event recorder
	mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_BeforeTriggerStop);

	// Notify the interfaces
	tSwisTrackCoreInterfaceList::iterator it = mSwisTrackCoreInterfaces.begin();
	while (it != mSwisTrackCoreInterfaces.end()) {
		(*it)->OnBeforeTriggerStop();
		it++;
	}

	// Deactivate the trigger
	mTrigger->SetActive(false);

	// Notify the interfaces
	it = mSwisTrackCoreInterfaces.begin();
	while (it != mSwisTrackCoreInterfaces.end()) {
		(*it)->OnAfterTriggerStop();
		it++;
	}

	// Notify the clients
	if (mCommunicationInterface) {
		CommunicationMessage mstop(wxT("TRIGGER_STOP"));
		mCommunicationInterface->Send(&mstop);
	}

	// Event recorder
	mEventRecorder->Add(SwisTrackCoreEventRecorder::sType_AfterTriggerStop);
}

void THISCLASS::ConfigurationWriteXML(wxXmlNode *configuration, ErrorList *xmlerr) {
	// Add an element for each component
	tComponentList::iterator it = mDeployedComponents.begin();
	while (it != mDeployedComponents.end()) {
		wxXmlNode *node = new wxXmlNode(0, wxXML_ELEMENT_NODE, wxT("component"));
		configuration->AddChild(node);
		node->AddProperty(wxT("type"), (*it)->mName);
		(*it)->ConfigurationWriteXML(node, xmlerr);

		it++;
	}
}

Component *THISCLASS::GetComponentByName(const wxString &name) {
	tComponentList::iterator it = mAvailableComponents.begin();
	while (it != mAvailableComponents.end()) {
		if ((*it)->mName == name) {
			return (*it);
		}
		it++;
	}

	return 0;
}

void THISCLASS::AddInterface(SwisTrackCoreInterface *stc) {
	mSwisTrackCoreInterfaces.push_back(stc);
}

void THISCLASS::RemoveInterface(SwisTrackCoreInterface *stc) {
	tSwisTrackCoreInterfaceList::iterator it = find(mSwisTrackCoreInterfaces.begin(), mSwisTrackCoreInterfaces.end(), stc);
	if (it == mSwisTrackCoreInterfaces.end()) {
		return;
	}
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
	if (IsStartedInProductionMode()) {
		return false;
	}

	// If started in test mode, stop and allow editing
	Stop();

	// Notify the interfaces
	tSwisTrackCoreInterfaceList::iterator it = mSwisTrackCoreInterfaces.begin();
	while (it != mSwisTrackCoreInterfaces.end()) {
		(*it)->OnBeforeEdit();
		it++;
	}

	mEditLocks = 1;
	return true;
}

void THISCLASS::DecrementEditLocks() {
	if (mEditLocks < 1) {
		return;
	}

	// Decrement and return if there are still other locks
	mEditLocks--;
	if (mEditLocks > 0) {
		return;
	}

	// Notify the interfaces
	tSwisTrackCoreInterfaceList::iterator it = mSwisTrackCoreInterfaces.begin();
	while (it != mSwisTrackCoreInterfaces.end()) {
		(*it)->OnAfterEdit();
		it++;
	}
}
