#include "DataStructureInput.h"
#define THISCLASS DataStructureInput

void THISCLASS::SetFrameTimestamp(wxDateTime timestamp) {
	mTimeSinceLastFrame = mFrameTimestamp.IsValid() ? timestamp - mFrameTimestamp : wxTimeSpan(0);
	mFrameTimestamp = timestamp;
}
