#include "InterceptionPanel.h"

BEGIN_EVENT_TABLE(InterceptionPanel, wxMiniFrame)
	EVT_SPIN(wxID_SPINNINS, InterceptionPanel::OnSpinUpdate)
	EVT_BUTTON(Gui_Intercept_Done, InterceptionPanel::OnInterceptDone)
	EVT_BUTTON(Gui_Intercept_Pick, InterceptionPanel::OnInterceptPick)
END_EVENT_TABLE()

/** \brief Constructor
*
* Creates a floating panel where the user can manually choose a trajectory.
*
* \param parent : Owner of the panel (usually SwisTrack)
*/
InterceptionPanel::InterceptionPanel(SwisTrack *parent) : wxMiniFrame(parent, -1,
																_("Trajectory Picker"),
																wxDefaultPosition,
																wxDefaultSize,
																wxCAPTION | wxSIMPLE_BORDER)
	{
	

	nobj = GetIntValByXPath(parent->cfgRoot,"/CFG/TRACKER/NOBJECTS");
	chosen_id.Printf("0");
	int spinvalue=0;
		

	this->parent=parent;
	
	wxPanel* panel = new wxPanel(this,-1);
	
	wxFlexGridSizer* spinsizer = new wxFlexGridSizer(1, 3, 10, 10);

		spinsizer->Add(new wxStaticText(panel, -1, "Trajectory", wxDefaultPosition,
			wxDefaultSize, wxALIGN_CENTER), 0, wxALIGN_LEFT | wxALIGN_CENTER);
		
		spintext = new wxTextCtrl( panel, -1, chosen_id, wxDefaultPosition, wxSize(80,-1),0);
	    
		wxSpinButton* spinbutton = new wxSpinButton( panel, wxID_SPINNINS, wxDefaultPosition,wxSize(40,20));
		spinbutton->SetRange(0,nobj-1); 
		
		spinbutton->SetValue(spinvalue);
		
		spinsizer->Add(spintext,0, wxALIGN_LEFT);
		spinsizer->Add(spinbutton, 0, wxALIGN_LEFT);
			
	

	wxBoxSizer *button_sizer = new wxBoxSizer( wxHORIZONTAL );

		button_sizer->Add(new wxButton( panel, Gui_Intercept_Pick, "Pick" ),0,wxALL,10 );
        button_sizer->Add(new wxButton( panel, Gui_Intercept_Done, "Done" ),0,wxALL,10 );
	   
	wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );
		
		topsizer->Add(spinsizer,1,wxEXPAND | wxALL, 10 );        	   
	    topsizer->Add(button_sizer,0,wxALIGN_RIGHT ); 

	panel->SetSizer(topsizer);
    panel->SetAutoLayout(TRUE);
	
    topsizer->Fit(panel);
    SetClientSize(panel->GetSize());
    wxSize sz = GetSize();
    SetSizeHints(sz.x, sz.y, sz.x, sz.y);
	
    // move it to a sensible position
    wxRect parentRect = parent->GetRect();
    wxSize childSize  = GetSize();
    int x = (int)(parentRect.GetX() + parentRect.GetWidth()*0.75);
    int y = parentRect.GetY() + 
		(parentRect.GetHeight() - childSize.GetHeight()) / 2;
    Move(x, y);
	

	panel->InitDialog();  // very important, calls all validators

    // done
	}

void InterceptionPanel::OnSpinUpdate( wxSpinEvent &event )
{
    wxString value;
    value.Printf( _T("%d"), event.GetPosition() );
    spintext->SetValue( value );
}

void InterceptionPanel::OnInterceptDone( wxCommandEvent &event )
{
	wxToolBarBase *tb = parent->GetToolBar();
    for(int i=Gui_Ctrl_Rewind; i<= Gui_Ctrl_Pause; i++) tb->ToggleTool(i, 0 );
	tb->ToggleTool(Gui_Ctrl_Continue, 1 );
	this->Show(FALSE);
	parent->menuControl->Enable(Gui_Ctrl_Pause,TRUE);
	parent->menuControl->Enable(Gui_Ctrl_Continue,FALSE);	

	parent->SetStatus(parent->ot->GetStatus());
}

void InterceptionPanel::OnInterceptPick( wxCommandEvent &event )
{
	Show(FALSE);

	unsigned long id;

	spintext->GetValue().ToULong(&id);

	CvPoint ue = parent->GetUserEstimateFor(id);

	if(ue.x != -1 && ue.y != -1){
		int height = parent->ot->GetImagePointer()->height;
		CvPoint2D32f uef = cvPoint2D32f(ue.x,height - ue.y);
		parent->ot->SetPos(id,&uef);
	}
	Show(TRUE);
//	parent->RefreshAllDisplays();
}
