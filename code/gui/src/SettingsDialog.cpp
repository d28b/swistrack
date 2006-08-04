#include "SettingsDialog.h"

BEGIN_EVENT_TABLE(SettingsDialog, wxPropertySheetDialog)
END_EVENT_TABLE()


void SettingsDialog::ChangeParam(wxCommandEvent& event)
	{
		int id=event.GetId()-1-wxID_HIGHEST;
		//((wxTextCtrl*) controls[id])->SetValue("OK");
		SetParamByXPath(parent->cfgRoot,idxpath[id].c_str(),((wxTextCtrl*) controls[id])->GetValue().c_str());
	}

void SettingsDialog::ChangeIntParam(wxCommandEvent& event)
	{
		int id=event.GetId()-1-wxID_HIGHEST;
		wxString dummy;
		dummy.Printf("%d",(int)((dynamic_cast<wxSpinCtrl*>(controls[id]))->GetValue()));
		
		/*wxMessageDialog* dlg =
    new wxMessageDialog(this,"Transporting Integer values does not yet work, change the configuration manually",dummy,wxOK);
    dlg->ShowModal();*/

		SetParamByXPath(parent->cfgRoot,idxpath[id].c_str(),dummy.c_str());
	}

void SettingsDialog::FileOpen(wxCommandEvent& event )
    {
    
    wxString title;
    
    int id=event.GetId()-2-wxID_HIGHEST;
    xmlpp::NodeSet set = nodeRoot->find(idxpath[id].c_str());
    
    title.Printf("Select file: %s",GetAttr((*set.begin()),"desc"));
    wxFileDialog dialog(this,title,wxEmptyString,wxEmptyString,GetAttr((*set.begin()),"param"),wxOPEN|wxFILE_MUST_EXIST);
    dialog.CentreOnParent();
    
    if (dialog.ShowModal() == wxID_OK)
        {	
		SetParamByXPath(parent->cfgRoot,idxpath[id].c_str(),dialog.GetPath().c_str());
        ((wxTextCtrl*) controls[id])->SetValue(dialog.GetPath().c_str());
        }
    }

void SettingsDialog::FileSave(wxCommandEvent& event )
    {
    
    wxString title;
    
    int id=event.GetId()-2-wxID_HIGHEST;
    xmlpp::NodeSet set = nodeRoot->find(idxpath[id].c_str());
    
    title.Printf("Select file: %s",GetAttr((*set.begin()),"desc"));
    wxFileDialog dialog(this,title,wxEmptyString,wxEmptyString,GetAttr((*set.begin()),"param"),wxSAVE|wxOVERWRITE_PROMPT);
    dialog.CentreOnParent();
    
    if (dialog.ShowModal() == wxID_OK)
        {
        SetParamByXPath(parent->cfgRoot,idxpath[id].c_str(),dialog.GetPath().c_str());
        ((wxTextCtrl*) controls[id])->SetValue(dialog.GetPath().c_str());
        }
    }


void SettingsDialog::ChangeMode(wxCommandEvent& event){
    int id=event.GetId()-1-wxID_HIGHEST;
    wxString dummy;
        dummy.Printf("%d",event.GetSelection());
    /*wxMessageDialog* dlg =
		new wxMessageDialog(this,"[SettingsDialog::ChangeMode] This feature is still under development. Problem: As soon as we connect wxTextCtrl to an event, it is not possible to change modes anymore.","Developer Message",wxOK);
    dlg->ShowModal();*/
    
    
	
	dummy.Printf("%d",event.GetSelection());
    	
    SetAttrByXPath(parent->cfgRoot,idxpath[id].c_str(),"mode",dummy.c_str());
    
    int currentab= notebook->GetSelection();
    // Clean up the notebook and reassign all callbacks to the controls


	for(int i=wxID_HIGHEST+1; i<=actid;i++)
		Disconnect(i);

	notebook->DeleteAllPages();
    actid=wxID_HIGHEST+1;
    controls.clear();
    idxpath.clear();
      
    CreateDialog();
	LayoutDialog();
    notebook->SetSelection(currentab);
    }

SettingsDialog::~SettingsDialog(){
    idxpath.clear();
    controls.clear();
    }

IMPLEMENT_CLASS(SettingsDialog, wxPropertySheetDialog)

SettingsDialog::SettingsDialog(SwisTrack *parent) : actid(wxID_HIGHEST+1), parent(parent)

   	{
    SetExtraStyle(wxDIALOG_EX_CONTEXTHELP|wxWS_EX_VALIDATE_RECURSIVELY);
    
    Create(parent, wxID_ANY, _("Preferences"), wxDefaultPosition, wxDefaultSize,
        wxDEFAULT_DIALOG_STYLE |wxRESIZE_BORDER);
    

    notebook =GetBookCtrl();
    try{
        parser.parse_file("swistrack.exp");
        document = parser.get_document();
        }
    catch (...){
        throw "Problems encountered when opening swistrack.exp. Invalid syntax?";
        }
    
    nodeRoot = parser.get_document()->get_root_node();
    current=nodeRoot;
    
    CreateDialog();
    
    }

void SettingsDialog::CreateDialog(){
    // Creates one notebook panel for every entry in the components list
    vector<wxPanel*> panels;
    xmlpp::NodeSet set = nodeRoot->find("/CFG/COMPONENTS/*");
    for(xmlpp::NodeSet::iterator i = set.begin(); i != set.end(); ++i) // loops through all components
        {
        //  std::cout << " " << (*i)->get_path() << std::endl;
        
        // read a list of all possible modes
        int current_mode=GetIntAttrByXPath(parent->cfgRoot,(*i)->get_path(),"mode"); 
        wxArrayString availablemodes;
        char xpath[255];
        sprintf(xpath,"/CFG/%s",(*i)->get_name().c_str());
        xmlpp::NodeSet modeset = nodeRoot->find(xpath);
        for(xmlpp::NodeSet::iterator mode = modeset.begin(); mode != modeset.end(); ++mode){ // loops through all available modes
            if(IsAttr(*mode,"desc")) availablemodes.Add(GetAttr(*mode,"desc"));
            }
        // display all information of the CURRENT mode given by the CFG
        for(xmlpp::NodeSet::iterator mode = modeset.begin(); mode != modeset.end(); ++mode){ // processes all available modes           
            if(GetIntAttr(*mode,"mode")==current_mode){
                
                // check whether there is already data for this mode in the configuration, and create nodes if necessary
                if(!IsDefined(parent->cfgRoot,(*mode)->get_path().c_str())){ 
                    //// import the node into CFG
                    parent->cfgRoot->import_node(*mode);
                    //xmlpp::Element* node = parent->cfgRoot->add_child((*i)->get_name().c_str());
                    //node->set_attribute("mode",GetAttr(*mode,"mode"));
                    }
                
                ///// Create Dialog ////
                panels.push_back(new wxPanel(notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                    wxTAB_TRAVERSAL | wxCLIP_CHILDREN | wxNO_BORDER | wxNO_FULL_REPAINT_ON_RESIZE));        
                wxBoxSizer *sizer_left = new wxBoxSizer( wxVERTICAL );
                wxBoxSizer *sizer_main = new wxBoxSizer( wxHORIZONTAL );
                
                // add modes into a listbox
                wxListBox* listbox = new wxListBox(panels.back(),actid++, 
                    wxDefaultPosition, wxSize(120,20), availablemodes, wxLB_SINGLE | wxLB_HSCROLL | wxLB_NEEDED_SB);
                listbox->SetSelection(current_mode);
                sizer_left->Add(listbox,0,wxGROW|wxALL,5);
                idxpath.push_back(wxString((*i)->get_path().c_str())); // store the location of the parameter related to this control
                controls.push_back(listbox);
                this->Connect(actid-1,wxEVT_COMMAND_LISTBOX_SELECTED,wxCommandEventHandler(SettingsDialog::ChangeMode));                
                
                // add description box
                wxStaticBox* desc_box = new wxStaticBox(panels.back(), wxID_ANY, _("Description"));
                wxBoxSizer* desc_box_sizer = new wxStaticBoxSizer( desc_box, wxVERTICAL );
                sizer_left->Add(desc_box_sizer, 0, wxGROW|wxALL);
                wxStaticText* description=new wxStaticText(panels.back(),wxID_ANY,GetAttr(*mode,"long"));
                description->Wrap(200);
                desc_box_sizer->Add(description,0, wxALL|wxALIGN_LEFT,0);
                
                // add parameter box
                wxStaticBox* param_box = new wxStaticBox(panels.back(), wxID_ANY, _("Parameters"));
                wxBoxSizer* param_box_sizer = new wxStaticBoxSizer(param_box, wxVERTICAL);
                sizer_main->Add(sizer_left,0,wxGROW|wxALL,1);
                sizer_main->Add(param_box_sizer,0,wxGROW|wxALL,1);
                //and fill in parameters
                xmlpp::NodeSet parameterset = (*mode)->find("*");
                wxGridSizer* parameter_sizer = new wxGridSizer(0,2,3,10);
                param_box_sizer->Add(parameter_sizer,0,wxGROW|wxALL,1);
                if(parameterset.size()==0){
                    parameter_sizer->Add(new wxStaticText(panels.back(),wxID_ANY,_T("This component requires no parameters")),0, wxALL|wxALIGN_CENTER_VERTICAL,0);
                    }
                // go through all parameters of this node, and display the appropriate control
                for(xmlpp::NodeSet::iterator  parameter = parameterset.begin(); 
                parameter != parameterset.end(); ++parameter){
                    parameter_sizer->Add(new wxStaticText(panels.back(),wxID_ANY,GetAttr(*parameter,"desc")),0, wxALL|wxALIGN_LEFT,0);
                    std::string param_type(GetAttr(*parameter,"type"));
                    
                    //////////////// Treats type INTEGER /////////////////////
                    if(!param_type.compare("integer")){
                        int min, max;
                        
                        if(IsAttr(*parameter,"min")) min=GetIntAttr(*parameter,"min");
                        if(IsAttr(*parameter,"max")) max=GetIntAttr(*parameter,"max");
                        
                        if(min==-1) min=-10000;
                        if(max==-1) max=10000;
                        
                        wxString value("0");
                        int v=0;
                        if(IsDefined(parent->cfgRoot,(*parameter)->get_path().c_str()) && IsContent(parent->cfgRoot,(*parameter)->get_path().c_str())){                            
                            value=wxString(GetValByXPath(parent->cfgRoot,(*parameter)->get_path().c_str()));
                            v=GetIntValByXPath(parent->cfgRoot,(*parameter)->get_path().c_str());
                            }
                        else if(IsContent(nodeRoot,(*parameter)->get_path().c_str())){
                            value=wxString(GetVal(*parameter));
                            v=GetIntVal(*parameter);
                            }
                        
                        wxSpinCtrl* m_spinctrl = new wxSpinCtrl( panels.back(),actid++,value,wxDefaultPosition,wxDefaultSize,
                            wxSP_ARROW_KEYS, min,max,v);
                        idxpath.push_back(wxString((*parameter)->get_path().c_str()));
                        controls.push_back(m_spinctrl);
                        parameter_sizer->Add(m_spinctrl,0, wxALL|wxALIGN_LEFT,0);
						this->Connect(actid-1,wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(SettingsDialog::ChangeIntParam));
                        }
                    //////////////// Treats type DOUBLE //////////////////////
                    else if(!param_type.compare("double")){
                        wxString value("0.0");
                        if(IsDefined(parent->cfgRoot,(*parameter)->get_path().c_str()) && IsContent(parent->cfgRoot,(*parameter)->get_path().c_str())){ // if in cfg
                            value=wxString(GetValByXPath(parent->cfgRoot,(*parameter)->get_path().c_str()));
                            }
                        else if(IsContent(nodeRoot,(*parameter)->get_path().c_str())){ // else get from template
                            value=GetVal(*parameter);
                            }
                        wxTextCtrl* m_textctrl = new wxTextCtrl(panels.back(),actid++,value,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator);
                        idxpath.push_back(wxString((*parameter)->get_path().c_str()));
                        controls.push_back(m_textctrl);
                        parameter_sizer->Add(m_textctrl,0,wxALL|wxALIGN_LEFT,0);
                        this->Connect(actid-1,wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(SettingsDialog::ChangeParam));
                        }
                    //////////////// Treats type OPEN ///////////////////////
                    else if(!param_type.compare("open")){
                        wxString value("");
                        if(IsDefined(parent->cfgRoot,(*parameter)->get_path().c_str()) && IsContent(parent->cfgRoot,(*parameter)->get_path().c_str())){ // if in cfg
                            value=wxString(GetValByXPath(parent->cfgRoot,(*parameter)->get_path().c_str()));
                            }
                        else if(IsContent(nodeRoot,(*parameter)->get_path().c_str())){ // else get from template
                            value=GetVal(*parameter);
                            }
                        wxBoxSizer *sizer = new wxBoxSizer( wxHORIZONTAL );
                        wxTextCtrl* m_textctrl = new wxTextCtrl(panels.back(),actid++,value,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator);  
                        idxpath.push_back(wxString((*parameter)->get_path().c_str()));
                        controls.push_back(m_textctrl);
                        wxButton* m_button = new wxButton(panels.back(),actid++,_T("..."),wxDefaultPosition,wxSize(20,20));
                        idxpath.push_back(wxString((*parameter)->get_path().c_str()));
                        controls.push_back(m_button);
                        sizer->Add(m_textctrl,0,wxALL|wxALIGN_LEFT,0);
                        sizer->Add(m_button,0,wxALL|wxALIGN_RIGHT,0);
                        parameter_sizer->Add(sizer,0,wxALL|wxALIGN_LEFT,0);
                        this->Connect(actid-1,wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SettingsDialog::FileOpen));
                        }
                    //////////////// Treats type SAVE ///////////////////////
                    else if(!param_type.compare("save")){
                        wxString value("");
                       /* if(IsDefined(parent->cfgRoot,(*parameter)->get_path().c_str())){ // if in cfg
                            value=wxString(GetValByXPath(parent->cfgRoot,(*parameter)->get_path().c_str()));
                            }
                        else if(IsContent(nodeRoot,(*parameter)->get_path().c_str())){ // else get from template
                            value=GetVal(*parameter);
                            }*/
                        wxBoxSizer *sizer = new wxBoxSizer( wxHORIZONTAL );
                        wxTextCtrl* m_textctrl = new wxTextCtrl(panels.back(),actid++,value,wxDefaultPosition,wxDefaultSize,0,wxDefaultValidator);  
                        idxpath.push_back(wxString((*parameter)->get_path().c_str()));
                        controls.push_back(m_textctrl);
                        wxButton* m_button = new wxButton(panels.back(),actid++,_T("..."),wxDefaultPosition,wxSize(20,20));
                        idxpath.push_back(wxString((*parameter)->get_path().c_str()));
                        controls.push_back(m_button);
                        sizer->Add(m_textctrl,0,wxALL|wxALIGN_LEFT,0);
                        sizer->Add(m_button,0,wxALL|wxALIGN_RIGHT,0);
                        parameter_sizer->Add(sizer,0,wxALL|wxALIGN_LEFT,0);
                        this->Connect(actid-1,wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SettingsDialog::FileSave));
                        }
                    //////////////// Treats type LIST ///////////////////////
                    else if(!param_type.compare("list")){
                        wxArrayString listitems;       
                        xmlpp::Node::NodeList listset = (*parameter)->get_children("li");
                        if(listset.size()==0){
                            wxMessageDialog* dlg =
                                new wxMessageDialog(this,_T("Error in swistrack.exp: Expecting a list of parameters in the form of <li>...</li>"),(*parameter)->get_path().c_str(),wxOK);
                            dlg->ShowModal();
                            }
                        for(xmlpp::Node::NodeList::iterator listitem = listset.begin(); listitem != listset.end(); ++listitem){
                            listitems.Add(GetVal(*listitem));
                            }
                        wxListBox* listbox = new wxListBox(panels.back(),actid++, 
                            wxDefaultPosition, wxSize(120,20), listitems, wxLB_SINGLE | wxLB_HSCROLL | wxLB_NEEDED_SB);
                        parameter_sizer->Add(listbox,0,wxGROW|wxALL,5);
                        idxpath.push_back(wxString((*parameter)->get_path().c_str())); // store the location of the parameter related to this control
                        controls.push_back(listbox);               
                        }
                    /////////////// Catches unknown types ///////////////////
                    else{
                        parameter_sizer->Add(new wxStaticText(panels.back(),wxID_ANY,_T("Unknown Type/Type not implemented")),0, wxALL|wxALIGN_LEFT,0);
                        }
                    }
                
                panels.back()->SetSizer(sizer_main);
                sizer_main->Fit(panels.back());
                
                } // if current mode
                
            } // for each mode
            
            notebook->AddPage(panels.back(),GetAttr(*i,"desc"), false);
            
        }
        LayoutDialog();
    }