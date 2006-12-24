#include "Display.h"
#define THISCLASS Display

THISCLASS::Display(wxWindow *parent, SwisTrack *st):
		wxPanel(parent, -1, pos, size),
		mSwisTrack(st), mSwisTrackCore(st->mSwisTrackCore) {

}
