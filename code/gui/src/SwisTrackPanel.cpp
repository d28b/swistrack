#include "SwisTrackPanel.h"

SwisTrackPanel::SwisTrackPanel(SwisTrack* parent, char* title, char* component, char* path, int display) 
															  : wxMiniFrame(parent, -1,
																title,
																wxDefaultPosition,
																wxDefaultSize,

																wxCAPTION | wxSIMPLE_BORDER),
																actid(wxID_HIGHEST+1), parent(parent),
																path(path),
																component(component)
{
	actid=wxID_HIGHEST+1;
	
	panel = new wxPanel(this,-1);

	controls.clear();
	idxpath.clear();
	wxGridSizer* parameter_sizer = new wxGridSizer(0,2,3,10);

	// Get current mode from actual cfg
	int current_mode=GetIntAttrByXPath(parent->cfgRoot,path,"mode");
	char xpath[255];
	sprintf(xpath,component);
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
							if(!IsAttr(*parameter,"desc")) throw "[SwisTrackPanel::SwisTrackPanel] The parameter description (attribute desc) is missing for some parameters of the current SEGMENTER mode";
							if(!IsAttr(*parameter,"type")) throw "[SwisTrackPanel::SwisTrackPanel] The parameter type (parameter type) is missing for some parameters of the current SEGMENTER mode";
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
									throw "[SwisTrackPanel::SwisTrackPanel] An INTEGER parameter has an empty content field in this component and mode";
								}
								wxSlider* m_slider = new wxSlider( panel,actid++, v, min,max,  wxDefaultPosition, wxSize(155,-1),
									wxSL_LABELS | wxSL_SELRANGE);

								//wxSpinCtrl* m_spinctrl = new wxSpinCtrl( panel,actid++,value,wxDefaultPosition,wxDefaultSize,
								//	wxSP_ARROW_KEYS, min,max,v);
								idxpath.push_back(wxString((*parameter)->get_path().c_str()));
								controls.push_back(m_slider);
								parameter_sizer->Add(m_slider,0, wxALL|wxALIGN_LEFT,0);
								this->Connect(actid-1,wxEVT_SCROLL_CHANGED, wxScrollEventHandler(SwisTrackPanel::ChangeIntParam));
							}
							//////////////// Treats type BOOLEAN /////////////////////
							else if(!param_type.compare("boolean")){
								bool v=0;
								//wxString value("0");
								if(IsDefined(parent->cfgRoot,(*parameter)->get_path().c_str()) && IsContent(parent->cfgRoot,(*parameter)->get_path().c_str())){                            
									if (GetIntValByXPath(parent->cfgRoot,(*parameter)->get_path().c_str())==0)
										v=false;
									else
										v=true;
								}
								else{
									throw "[SwisTrackPanel::SwisTrackPanel] An BOOLEAN parameter has an empty content field in this component and mode";
								}
								
								wxCheckBox* m_checkbox = new wxCheckBox(panel,actid++,_(""));
								m_checkbox->SetValue(v);
								idxpath.push_back(wxString((*parameter)->get_path().c_str()));
								controls.push_back(m_checkbox);
								parameter_sizer->Add(m_checkbox,0, wxALL|wxALIGN_LEFT,0);
								this->Connect(actid-1,wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(SwisTrackPanel::ChangeBooleanParam));
							}
							//////////////// Treats type DOUBLE //////////////////////
							else if(!param_type.compare("double")){
								wxString value("0.0");
								if(IsDefined(parent->cfgRoot,(*parameter)->get_path().c_str()) && IsContent(parent->cfgRoot,(*parameter)->get_path().c_str())){ // if in cfg
									value=wxString(GetValByXPath(parent->cfgRoot,(*parameter)->get_path().c_str()));
								}
								else{
									throw "[SwisTrackPanel::SwisTrackPanel] A DOUBLE parameter has an empty content field in this component and mode";
								}
								wxTextCtrl* m_textctrl = new wxTextCtrl(panel,actid++,value,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator);
								idxpath.push_back(wxString((*parameter)->get_path().c_str()));
								controls.push_back(m_textctrl);
								parameter_sizer->Add(m_textctrl,0,wxALL|wxALIGN_LEFT,0);
								this->Connect(actid-1,wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(SwisTrackPanel::ChangeParam));
							}
							else {
								throw "[SwisTrackPanel::SwisTrackPanel] Only INTEGER, DOUBLE, BOOLEAN can be declared VARIABLE";
							}
						}
					}
			}
		}
			
			
	}
	wxFlexGridSizer* panelsizer = new wxFlexGridSizer(2,1,10,10);

	/** \todo Get the real values for the image width and height here */
	if(display){
		canvas = new SwisTrackPanelCanvas(parent,panel,wxDefaultPosition, wxSize(640/3,480/3));
		panelsizer->Add(canvas, 0, wxALIGN_CENTER,10);	
	}

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
}

void SwisTrackPanel::ChangeIntParam(wxScrollEvent& event)
	{
		
		int id=event.GetId()-1-wxID_HIGHEST;
		
		wxString dummy;
		dummy.Printf("%d",((wxSlider*) controls[id])->GetValue());
		SetParamByXPath(parent->cfgRoot,idxpath[id].c_str(),dummy.c_str());

#ifdef MULTITHREAD
		wxCriticalSectionLocker locker(*(parent->criticalSection));
#endif
		parent->ot->SetParameters(); 
	}

void SwisTrackPanel::ChangeBooleanParam(wxCommandEvent& event)
	{
		
		int id=event.GetId()-1-wxID_HIGHEST;
		
		wxString dummy;
		dummy.Printf("%d",((wxCheckBox*) controls[id])->GetValue());
		SetParamByXPath(parent->cfgRoot,idxpath[id].c_str(),dummy.c_str());
#ifdef MULTITHREAD
		wxCriticalSectionLocker locker(*(parent->criticalSection));
#endif
		parent->ot->SetParameters(); 

	}

void SwisTrackPanel::ChangeParam(wxCommandEvent& event)
	{
		
		int id=event.GetId()-1-wxID_HIGHEST;
		SetParamByXPath(parent->cfgRoot,idxpath[id].c_str(),((wxTextCtrl*) controls[id])->GetValue().c_str());
#ifdef MULTITHREAD
		wxCriticalSectionLocker locker(*(parent->criticalSection));
#endif
		parent->ot->SetParameters();
	}

void SwisTrackPanel::Clear()
{
	canvas->Clear();
}

void SwisTrackPanel::CreateBitmapfromImage(const wxImage& img, int depth){
	canvas->CreateBitmapfromImage(img,depth);
}

void SwisTrackPanel::Refresh(){
	panel->Refresh();
}


