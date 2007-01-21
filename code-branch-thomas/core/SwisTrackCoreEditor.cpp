#include "SwisTrackCoreEditor.h"
#define THISCLASS SwisTrackCoreEditor

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

THISCLASS::SwisTrackCoreEditor(SwisTrackCore *stc): mSwisTrackCore(0) {
	if (stc->IsStartedInSeriousMode()) {return;}

	// For safety reasons, we only set this if we are allowed to edit
	mSwisTrackCore=stc;
	mSwisTrackCore->mEditLocks++;
}

THISCLASS::~SwisTrackCoreEditor() {
	if (! mSwisTrackCore) {return;}
	mSwisTrackCore->mEditLocks--;
}

void THISCLASS::Clear() {
	if (! mSwisTrackCore) {return;}

	tComponentList::iterator it=mSwisTrackCore.mDeployedComponents.begin();
	while (it!=mSwisTrackCore.mDeployedComponents.end()) {
		delete (*it);
		it++;
	}

	mSwisTrackCore.mDeployedComponents.clear();
}

SwisTrackCore::tComponentList *THISCLASS::GetComponentList() {
	if (! mSwisTrackCore) {return;}
	return &(mSwisTrackCore.mDeployedComponents);
}
