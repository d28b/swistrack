#include "SwisTrackCore.h"
#define THISCLASS SwisTrackCore

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

THISCLASS::SwisTrackCore(xmlpp::Element* cfgroot):
		mConfigurationRoot(cfgroot),
		mDataStructureInput(),
		mDataStructureImageBGR("ImageBGR", "Color image (BGR)"),
		mDataStructureBackgroundBGR("BackgroundBGR", "Background color image (BGR)"),
		mDataStructureImageGray("ImageGray", "Grayscale image"),
		mDataStructureBackgroundGray("BackgroundGray", "Grayscale background image"),
		mDataStructureImageBinary("ImageBinary", "Binary image"),
		mDataStructureMaskBinary("MaskBinary", "Binary mask"),
		mDataStructureParticles(),
		mCategoryInput("Input", 100, ComponentCategory::sFlagRequired|ComponentCategory::sFlagExclusive),
		mCategoryInputConversion("Input conversion", 200, ComponentCategory::sFlagRequired),
		mCategoryPreprocessing("Preprocessing", 300),
		mCategoryThresholding("Thresholding", 400),
		mCategoryBlobDetection("Blob detection", 500),
		mCategoryOutput("Output", 10000)
		{

	// Initialize the list of available components
	mComponents.push_back(new ComponentInputCamera1394(this));
	mComponents.push_back(new ComponentInputCameraUSB(this));
	mComponents.push_back(new ComponentInputCameraGBit(this));
	mComponents.push_back(new ComponentInputFileAVI(this));
	mComponents.push_back(new ComponentConvertToGray(this));
	mComponents.push_back(new ComponentConvertToBGR(this));
	mComponents.push_back(new ComponentBackgroundSubtractionGray(this));
	mComponents.push_back(new ComponentThresholdGray(this));
	mComponents.push_back(new ComponentBlobDetectionMinMax(this));
	mComponents.push_back(new ComponentBlobDetectionCircle(this));

	// Initialize the list of available data structures
	mDataStructures.push_back(&mDataStructureInput);
	mDataStructures.push_back(&mDataStructureImageBGR);
	mDataStructures.push_back(&mDataStructureBackgroundBGR);
	mDataStructures.push_back(&mDataStructureBackgroundGray);
	mDataStructures.push_back(&mDataStructureImageBinary);
	mDataStructures.push_back(&mDataStructureMaskBinary);
	mDataStructures.push_back(&mDataStructureParticles);
}

bool THISCLASS::Start() {
	bool allok=true;

	// Start all components (until first error)
	tComponentList::iterator it=mComponentList.begin();
	while (it!=mComponentList.end()) {
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
	tComponentList::iterator it=mComponentList.end();
	while (it!=mComponentList.begin()) {
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
	tComponentList::iterator it=mComponentList.begin();
	while (it!=mComponentList.end()) {
		if (! (*it)->mStarted) {break;}
		(*it)->ClearStatus();
		(*it)->OnStep();
		if (! (*it)->mStatusHasError) {allok=false; break;}
		it++;
	}

	// and then cleanup what we run
	while (it!=mComponentList.begin()) {
		it--;
		(*it)->OnStepCleanup();
		if (! (*it)->mStatusHasError) {allok=false;}
	}

	return allok;
}
