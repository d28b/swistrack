#include "SegmenterPanel.h"

BEGIN_EVENT_TABLE(SegmenterPanel, wxMiniFrame)
	EVT_COMMAND_SCROLL(wxID_SPMINAREA, SegmenterPanel::OnChangeMinArea)
	EVT_COMMAND_SCROLL(wxID_SPTHRESHOLD, SegmenterPanel::OnChangeThreshold)
END_EVENT_TABLE()


/** \brief Constructor
*
* Creates a floating panel where the user can change segmenter parameters and watch
* their influence on the binary (segmented) image.
* 
* \brief parent : Owner of the panel (usually SwisTrack)
*/

SegmenterPanel::SegmenterPanel(SwisTrack *parent) : wxMiniFrame(parent, -1,
																  _("Segmenter parameters"),
																  wxDefaultPosition,
																  wxDefaultSize,

                                                                  wxCAPTION | wxSIMPLE_BORDER){


    /** \todo  fetch all necessary parameters from config, similar to settingsdialog
/*	this->parent=parent;
	binarybmp=new wxBitmap(parent->width/4,parent->height/3,3);
	minarea = parent->cfg->GetIntParam("/CFG/PARTICLEFILTER/@0/MINAREA");
	threshold = parent->cfg->GetIntParam("/CFG/SEGMENTER/@0/THRESHOLD");
	

	panel = new wxPanel(this,-1);

	wxFlexGridSizer *dialogSizer = new wxFlexGridSizer(2,2,10,10);
	
	dialogSizer->Add(new wxStaticText(panel, -1, "Minimum area", wxDefaultPosition,
		wxDefaultSize, wxALIGN_CENTER), 0, wxALIGN_CENTER,10);
	dialogSizer->Add(new wxSlider(panel,wxID_SPMINAREA, 0, 0,100,  wxDefaultPosition, wxSize(155,-1),
		wxSL_LABELS, wxGenericValidator(&minarea)), 0, wxALIGN_CENTER,10);
	
	dialogSizer->Add(new wxStaticText(panel, -1, "Threshold", wxDefaultPosition,
		wxDefaultSize, wxALIGN_CENTER), 0, wxALIGN_CENTER,10);
	dialogSizer->Add(new wxSlider( panel,wxID_SPTHRESHOLD, 0, 0,50,  wxDefaultPosition, wxSize(155,-1),
		wxSL_LABELS, wxGenericValidator(&threshold)), 0, wxALIGN_CENTER,10);
	

	wxFlexGridSizer* panelsizer = new wxFlexGridSizer(2,1,10,10);

	canvas = new SegmenterPanelCanvas(this,panel,wxDefaultPosition, wxSize(parent->width/3,parent->height/3));
	panelsizer->Add(canvas, 0, wxALIGN_CENTER,10);	
	panelsizer->Add(dialogSizer, 0, wxALIGN_CENTER,10);
	
	panel->SetSizer(panelsizer);
    panel->SetAutoLayout(TRUE);
	panel->InitDialog();

    panelsizer->Fit(panel);
    SetClientSize(panel->GetSize());
    wxSize sz = GetSize();
    SetSizeHints(sz.x, sz.y, sz.x, sz.y);
	
    // move it to a sensible position
    wxRect parentRect = parent->GetRect();
    wxSize childSize  = GetSize();
    int x = (int)(parentRect.GetX() + parentRect.GetWidth()*0.75+20);
    int y = parentRect.GetY() +
		(parentRect.GetHeight() - childSize.GetHeight()) / 4 + 20;
    Move(x, y);
	
    // done
*/
  }

/** \brief Eventhandler for the 'min area' slider bar
*
* This event handler calls the TransferDataFromWindow() method of the
* panel holding the controls. By this, data from the controls are transported
* to variables private to the SegmenterPanel class. From there, they are copied
* to the global parameters (ExperimentCfg class) and send to the ObjectTracker class
* using the SetTrackingSegmenterParameters() method.
*/
void SegmenterPanel::OnChangeMinArea(wxScrollEvent& WXUNUSED(event)){
/*	panel->TransferDataFromWindow();
	parent->cfg->SetParam("/CFG/SEGMENTER/@0/MINAREA",minarea);
	parent->SetTrackingSegmenterParameters();*/
	}


/** \brief Eventhandler for the 'threshold' slider bar
*
* This event handler calls the TransferDataFromWindow() method of the
* panel holding the controls. By this, data from the controls are transported
* to variables private to the SegmenterPanel class. From there, they are copied
* to the global parameters (ExperimentCfg class) and send to the ObjectTracker class
* using the SetTrackingSegmenterParameters() method.
*/
void SegmenterPanel::OnChangeThreshold(wxScrollEvent& WXUNUSED(event)){
/*	panel->TransferDataFromWindow();
	parent->cfg->SetParam("/CFG/SEGMENTER/@0/THRESHOLD",threshold);
	parent->SetTrackingSegmenterParameters();*/
	}



