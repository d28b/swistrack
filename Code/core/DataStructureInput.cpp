#include "DataStructureInput.h"
#define THISCLASS DataStructureInput

void THISCLASS::SetFrameTimestamp(wxDateTime d) {
	if (mFrameTimestamp.IsValid()) {
		mTimeSinceLastFrame = d - mFrameTimestamp;
	}
	mFrameTimestamp = d;
}
