#include "ConfigurationParameterPointInteger.h"
#define THISCLASS ConfigurationParameterPointInteger

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <algorithm>
#include "SwisTrackCoreEditor.h"
#include "ComponentEditor.h"

BEGIN_EVENT_TABLE(THISCLASS, wxPanel)
  EVT_TEXT (wxID_ANY, THISCLASS::OnTextUpdated)
  EVT_TEXT_ENTER (wxID_ANY, THISCLASS::OnTextEnter)
END_EVENT_TABLE()

THISCLASS::ConfigurationParameterPointInteger(wxWindow* parent):
		ConfigurationParameter(parent),
		mValueMin(0), mValueMax(255), mValueDefault(0) {

}

THISCLASS::~ConfigurationParameterPointInteger() {
}

void THISCLASS::OnInitialize(ConfigurationXML *config, ErrorList *errorlist) {
	// Read specific configuration
	config->SelectRootNode();
	mValueMin=config->ReadInt("min", 0);
	mValueMax=config->ReadInt("max", 255);
	mValueDefault=config->ReadInt("default", 0);

	// Create the controls
	wxStaticText *label=new wxStaticText(this, -1, config->ReadString("label", ""), wxDefaultPosition, wxSize(75, -1), wxST_NO_AUTORESIZE);
	mTextX=new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(50, -1), wxTE_RIGHT|wxTE_PROCESS_ENTER);
	mTextY=new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(50, -1), wxTE_RIGHT|wxTE_PROCESS_ENTER);
	wxStaticText *labelx=new wxStaticText(this, -1, config->ReadString("labelx", "X"), wxDefaultPosition, wxSize(40, -1), wxST_NO_AUTORESIZE);
	wxStaticText *labely=new wxStaticText(this, -1, config->ReadString("labely", "Y"), wxDefaultPosition, wxSize(40, -1), wxST_NO_AUTORESIZE);

	wxBoxSizer *hs=new wxBoxSizer(wxHORIZONTAL);
	hs->Add(labelx, 0, wxALIGN_CENTER_VERTICAL, 0);
	hs->Add(mTextX, 0, wxALIGN_CENTER_VERTICAL, 0);
	hs->Add(labely, 0, wxALIGN_CENTER_VERTICAL, 0);
	hs->Add(mTextY, 0, wxALIGN_CENTER_VERTICAL, 0);

	wxBoxSizer *vs=new wxBoxSizer(wxVERTICAL);
	hs->Add(label, 0, 0, 0);
	hs->Add(hs, 0, 0, 0);
	SetSizer(vs);
}

void THISCLASS::OnUpdate() {
	int valuex=mComponent->GetConfigurationInt((mName+".x").c_str(), mValueDefaultX);
	int valuey=mComponent->GetConfigurationInt((mName+".y").c_str(), mValueDefaultY);
	mTextCtrl->SetValue(wxString::Format("%d", valuex));
	mTextCtrl->SetValue(wxString::Format("%d", valuey));
}

void THISCLASS::OnTextUpdated(wxCommandEvent& event) {
	long value=(long)mValueDefault;
	mTextCtrl->GetValue().ToLong(&value);
	if (value<mValueMin) {value=mValueMin;}
	if (value>mValueMax) {value=mValueMax;}
	int curvalue=mComponent->GetConfigurationInt(mName.c_str(), mValueDefault);
	if (curvalue==value) {return;}

	// Set the new configuration values
	ComponentEditor ce(mComponent);
	ce.SetConfigurationInt(mName.c_str(), value);

	// Commit these changes
	CommitChanges();
}

void THISCLASS::OnTextEnter(wxCommandEvent& event) {
	OnTextUpdated(event);
}

void THISCLASS::SetValue(int valuex, int valuey) {
	if (valuex<mValueMinX) {valuex=mValueMinX;}
	if (valuex>mValueMaxX) {valuex=mValueMaxX;}
	if (valuey<mValueMinY) {valuey=mValueMinY;}
	if (valuey>mValueMaxY) {valuey=mValueMaxY;}
	int curvaluex=mComponent->GetConfigurationInt((mName+".x").c_str(), mValueDefaultX);
	int curvaluey=mComponent->GetConfigurationInt((mName+".y").c_str(), mValueDefaultY);
	if ((curvaluex==valuex) && (curvaluey==valuey)) {return;}

	// Set the new configuration values
	ComponentEditor ce(mComponent);
	ce.SetConfigurationInt((mName+".x").c_str(), valuex);
	ce.SetConfigurationInt((mName+".y").c_str(), valuey);

	// Commit these changes
	CommitChanges();
}
