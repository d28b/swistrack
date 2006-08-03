#include "SegmenterPanel.h"

BEGIN_EVENT_TABLE(SegmenterPanel, wxMiniFrame)
	EVT_COMMAND_SCROLL(wxID_SPMINAREA, SegmenterPanel::OnChangeMinArea)
	EVT_COMMAND_SCROLL(wxID_SPTHRESHOLD, SegmenterPanel::OnChangeThreshold)
END_EVENT_TABLE()

void SegmenterPanel::ChangeIntParam(wxScrollEvent& event)
	{
		int id=event.GetId()-1-wxID_HIGHEST;
		
		wxString dummy;
		dummy.Printf("%d",((wxSlider*) controls[id])->GetValue());
		SetParamByXPath(parent->cfgRoot,idxpath[id].c_str(),dummy.c_str());
		parent->ot->SetParameters(); 
	}

void SegmenterPanel::ChangeParam(wxCommandEvent& event)
	{
		int id=event.GetId()-1-wxID_HIGHEST;
		//((wxTextCtrl*) controls[id])->SetValue("OK");
		SetParamByXPath(parent->cfgRoot,idxpath[id].c_str(),((wxTextCtrl*) controls[id])->GetValue().c_str());
		parent->ot->SetParameters();
	}


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

																wxCAPTION | wxSIMPLE_BORDER),
																actid(wxID_HIGHEST+1), parent(parent)
{



	binarybmp=new wxBitmap(parent->width/4,parent->height/3,3);
	panel = new wxPanel(this,-1);

	controls.clear();
	idxpath.clear();
	wxGridSizer* parameter_sizer = new wxGridSizer(0,2,3,10);

	// Get current mode from actual cfg
	int current_mode=GetIntAttrByXPath(parent->cfgRoot,"/CFG/COMPONENTS/SEGMENTER","mode");
	char xpath[255];
	sprintf(xpath,"/CFG/SEGMENTER");
	xmlpp::NodeSet modeset = parent->cfgRoot->find(xpath);
	for(xmlpp::NodeSet::iterator mode = modeset.begin(); mode != modeset.end(); ++mode){ // processes all available modes           
		if(GetIntAttr(*mode,"mode")==current_mode){ // work only on the current mode
			xmlpp::NodeSet parameterset = (*mode)->find("*");


			if(parameterset.size()==0){
				parameter_sizer->Add(new wxStaticText(panel,wxID_ANY,_T("This component requires no parameters")),0, wxALL|wxALIGN_CENTER_VERTICAL,0);
			}
			// go through all parameters of this node, and display the appropriate control
			for(xmlpp::NodeSet::iterator  parameter = parameterset.begin(); 
				parameter != parameterset.end(); ++parameter){
					if(IsAttr(*parameter,"variable")){  
						if(GetIntAttr(*parameter,"variable")==1){
							if(!IsAttr(*parameter,"desc")) throw "The parameter description (attribute desc) is missing for some parameters of the current SEGMENTER mode";
							if(!IsAttr(*parameter,"type")) throw "The parameter type (parameter type) is missing for some parameters of the current SEGMENTER mode";
							parameter_sizer->Add(new wxStaticText(panel,wxID_ANY,GetAttr(*parameter,"desc")),0, wxALL|wxALIGN_LEFT,0);
							std::string param_type(GetAttr(*parameter,"type"));

							//////////////// Treats type INTEGER /////////////////////
							if(!param_type.compare("integer")){
								int min, max;

								if(IsAttr(*parameter,"min")) min=GetIntAttr(*parameter,"min");
								if(IsAttr(*parameter,"max")) max=GetIntAttr(*parameter,"max");

								if(min==-1) min=0;
								if(max==-1) max=255;

								wxString value("0");
								int v=0;
								if(IsDefined(parent->cfgRoot,(*parameter)->get_path().c_str()) && IsContent(parent->cfgRoot,(*parameter)->get_path().c_str())){                            
									//value=wxString(GetValByXPath(parent->cfgRoot,(*parameter)->get_path().c_str()));
									v=GetIntValByXPath(parent->cfgRoot,(*parameter)->get_path().c_str());
								}
								else{
									throw "/CFG/SEGMENTER/ - an INTEGER parameter has an empty content field";
								}
								wxSlider* m_slider = new wxSlider( panel,actid++, v, min,max,  wxDefaultPosition, wxSize(155,-1),
									wxSL_LABELS | wxSL_SELRANGE);

								//wxSpinCtrl* m_spinctrl = new wxSpinCtrl( panel,actid++,value,wxDefaultPosition,wxDefaultSize,
								//	wxSP_ARROW_KEYS, min,max,v);
								idxpath.push_back(wxString((*parameter)->get_path().c_str()));
								controls.push_back(m_slider);
								parameter_sizer->Add(m_slider,0, wxALL|wxALIGN_LEFT,0);
								this->Connect(actid-1,wxEVT_SCROLL_CHANGED, wxScrollEventHandler(SegmenterPanel::ChangeIntParam));
							}
							//////////////// Treats type DOUBLE //////////////////////
							else if(!param_type.compare("double")){
								wxString value("0.0");
								if(IsDefined(parent->cfgRoot,(*parameter)->get_path().c_str()) && IsContent(parent->cfgRoot,(*parameter)->get_path().c_str())){ // if in cfg
									value=wxString(GetValByXPath(parent->cfgRoot,(*parameter)->get_path().c_str()));
								}
								else{
									throw "/CFG/SEGMENTER/ - a DOUBLE parameter has an empty content field";
								}
								wxTextCtrl* m_textctrl = new wxTextCtrl(panel,actid++,value,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator);
								idxpath.push_back(wxString((*parameter)->get_path().c_str()));
								controls.push_back(m_textctrl);
								parameter_sizer->Add(m_textctrl,0,wxALL|wxALIGN_LEFT,0);
								this->Connect(actid-1,wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(SegmenterPanel::ChangeParam));
							}
							else {
								throw "/CFG/SEGMENTER: only INTEGER and DOUBLE can be declared VARIABLE";
							}
						}
					}
			}
		}
			
			
	}
	
                        

	//minarea = parent->cfg->GetIntParam("/CFG/PARTICLEFILTER/@0/MINAREA");
	//threshold = parent->cfg->GetIntParam("/CFG/SEGMENTER/@0/THRESHOLD");

	

	
	
/*	dialogSizer->Add(new wxStaticText(panel, -1, "Minimum area", wxDefaultPosition,
		wxDefaultSize, wxALIGN_CENTER), 0, wxALIGN_CENTER,10);
	dialogSizer->Add(new wxSlider(panel,wxID_SPMINAREA, 0, 0,100,  wxDefaultPosition, wxSize(155,-1),
		wxSL_LABELS, wxGenericValidator(&minarea)), 0, wxALIGN_CENTER,10);
	
	dialogSizer->Add(new wxStaticText(panel, -1, "Threshold", wxDefaultPosition,
		wxDefaultSize, wxALIGN_CENTER), 0, wxALIGN_CENTER,10);
	dialogSizer->Add(new wxSlider( panel,wxID_SPTHRESHOLD, 0, 0,50,  wxDefaultPosition, wxSize(155,-1),
		wxSL_LABELS, wxGenericValidator(&threshold)), 0, wxALIGN_CENTER,10);
*/	

	wxFlexGridSizer* panelsizer = new wxFlexGridSizer(2,1,10,10);

	canvas = new SegmenterPanelCanvas(this,panel,wxDefaultPosition, wxSize(parent->width/3,parent->height/3));
	panelsizer->Add(canvas, 0, wxALIGN_CENTER,10);	
	panelsizer->Add(parameter_sizer, 0, wxALIGN_CENTER,10);
	
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



