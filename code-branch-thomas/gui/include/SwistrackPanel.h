#ifndef _SWISTRACKPANEL_
#define _SWISTRACKPANEL_

#include "wx/minifram.h"  // Imports the mini frame class (tracker and segmenter panel)
#include "wx/valgen.h"	  // Generic validators, used in almost every dialgo
#include "wx/slider.h"

#include "SwisTrack.h"
#include "SwisTrackPanelCanvas.h"

using namespace std;

class SwisTrackPanel : public wxMiniFrame
{
public:
	SwisTrackPanel(SwisTrack* parent, char* title, char* mode, char* path, int display=1);
	void ChangeIntParam(wxScrollEvent& event);
	void ChangeBooleanParam(wxCommandEvent& event);
	void ChangeParam(wxCommandEvent& event);
	void Refresh();
	void Clear();
	void CreateBitmapfromImage(const wxImage& img, int depth = -1);

protected:
	int actid;
	SwisTrack* parent;
	vector<wxString> idxpath; // keeps track of the x-path of each control
	vector<wxControl*> controls; // keeps track of all controls
	wxPanel* panel;
	char* component;
	char* path;
	SwisTrackPanelCanvas* canvas;

};


#endif
