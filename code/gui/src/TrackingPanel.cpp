#include "TrackingPanel.h"

BEGIN_EVENT_TABLE(TrackingPanel, wxMiniFrame)
	EVT_COMMAND_SCROLL(wxID_TPSPEED, TrackingPanel::OnChangeMaxSpeed)
END_EVENT_TABLE()

/** \brief Event handler for 'trackingpanel->maxspeed' sliderbar
*
* Calls the TransferDataFromWindow() of the tracking panel and sends 
* the maximum speed to the Tracking class.
*/
void TrackingPanel::OnChangeMaxSpeed(wxScrollEvent& WXUNUSED(event))
{
	TransferDataFromWindow();
//	parent->cfg.maxspeed.Printf("%d",maxspeed);
//	parent->SetTrackingSegmenterParameters();
}

/** \brief Constructor
*
* Creates a floating panel where the user can change tracking parameters.
*
* \param parent : Owner of the panel (usually SwisTrack)
*/
TrackingPanel::TrackingPanel(SwisTrack *parent) : wxMiniFrame(parent, -1,
																_("Tracker parameters"),
																wxDefaultPosition,
																wxDefaultSize,
																wxCAPTION | wxSIMPLE_BORDER)
	{
	
//	maxspeed = parent->cfg.GetValue(&(parent->cfg.maxspeed));
	this->parent=parent;
	
	wxPanel* panel = new wxPanel(this,-1);
	
	wxFlexGridSizer *dialogSizer = new wxFlexGridSizer(1,2,10,10);
	
	dialogSizer->Add(new wxStaticText(panel, -1, "Maximum speed", wxDefaultPosition,
		wxDefaultSize, wxALIGN_CENTER), 0, wxALIGN_CENTER,10);
	dialogSizer->Add(new wxSlider( panel,wxID_TPSPEED, maxspeed, 0,(int) (parent->width/8),  wxDefaultPosition, wxSize(155,-1),
		wxSL_LABELS, wxGenericValidator(&maxspeed)), 0, wxALIGN_CENTER,10);

	/*dialogSizer->Add(new wxStaticText(this, -1, "Maximum Area", wxDefaultPosition,
	wxDefaultSize, wxALIGN_CENTER), 0, wxALIGN_LEFT | wxALIGN_CENTER);
	dialogSizer->Add(wxSlider( this,wxID_TPSPEED, 0, 0,(int) fps,  wxDefaultPosition, wxSize(155,-1),
	wxSL_AUTOTICKS | wxSL_LABELS, wxGenericValidator(&(parent->max_object_speed)));
	*/
	
	panel->SetSizer(dialogSizer);
    panel->SetAutoLayout(TRUE);
	
    dialogSizer->Fit(panel);
    SetClientSize(panel->GetSize());
    wxSize sz = GetSize();
    SetSizeHints(sz.x, sz.y, sz.x, sz.y);
	
    // move it to a sensible position
    wxRect parentRect = parent->GetRect();
    wxSize childSize  = GetSize();
    int x = (int)(parentRect.GetX() + parentRect.GetWidth()*0.75);
    int y = parentRect.GetY() +
		(parentRect.GetHeight() - childSize.GetHeight()) / 4;
    Move(x, y);
	
    // done
	}

