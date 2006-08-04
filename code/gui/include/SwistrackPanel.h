#ifndef _SWISTRACKPANEL_
#define _SWISTRACKPANEL_

#include "wx/minifram.h"  // Imports the mini frame class (tracker and segmenter panel)
#include "wx/valgen.h"	  // Generic validators, used in almost every dialgo
#include "wx/slider.h"

#include "constants.h"
#include "SwisTrack.h"

class SwisTrackPanel : public wxMiniFrame
{
public:
	SwisTrackPanel(SwisTrack* parent, char* title, char* mode, char* path);
	void ChangeIntParam(wxScrollEvent& event);
	void ChangeParam(wxCommandEvent& event);

protected:
	int actid;
	SwisTrack* parent;
	vector<wxString> idxpath; // keeps track of the x-path of each control
    vector<wxControl*> controls; // keeps track of all controls
	wxPanel* panel;
	char* component;
	char* path;

};


#endif
