#include "SwisTrackPanel.h"

SwisTrackPanel::SwisTrackPanel(SwisTrack* parent, char* title, char* component, char* path) : wxMiniFrame(parent, -1,
																title,
																wxDefaultPosition,
																wxDefaultSize,

																wxCAPTION | wxSIMPLE_BORDER),
																actid(wxID_HIGHEST+1), parent(parent),
																path(path),
																component(component)
{
}

void SwisTrackPanel::ChangeIntParam(wxScrollEvent& event)
	{
		int id=event.GetId()-1-wxID_HIGHEST;
		
		wxString dummy;
		dummy.Printf("%d",((wxSlider*) controls[id])->GetValue());
		SetParamByXPath(parent->cfgRoot,idxpath[id].c_str(),dummy.c_str());
		parent->ot->SetParameters(); 
	}

void SwisTrackPanel::ChangeParam(wxCommandEvent& event)
	{
		int id=event.GetId()-1-wxID_HIGHEST;
		SetParamByXPath(parent->cfgRoot,idxpath[id].c_str(),((wxTextCtrl*) controls[id])->GetValue().c_str());
		parent->ot->SetParameters();
	}


