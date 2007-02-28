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
  EVT_SPINCTRL (wxID_ANY, THISCLASS::OnSpin)
END_EVENT_TABLE()

THISCLASS::ConfigurationParameterPointInteger(wxWindow* parent):
		ConfigurationParameter(parent),
		mValueMinX(0), mValueMaxX(640), mValueDefaultX(0),
		mValueMinY(0), mValueMaxY(480), mValueDefaultY(0) {

}

THISCLASS::~ConfigurationParameterPointInteger() {
}

void THISCLASS::OnInitialize(ConfigurationXML *config, ErrorList *errorlist) {
	// Read specific configuration
	config->SelectRootNode();
	config->SelectChildNode("min");
	mValueMinX=config->ReadInt("x", 0);
	mValueMinY=config->ReadInt("y", 0);
	config->SelectRootNode();
	config->SelectChildNode("max");
	mValueMaxX=config->ReadInt("x", 640);
	mValueMaxY=config->ReadInt("y", 480);
	config->SelectRootNode();
	config->SelectChildNode("default");
	mValueDefaultX=config->ReadInt("x", 0);
	mValueDefaultY=config->ReadInt("y", 0);

	// Create the controls
	config->SelectRootNode();
	wxStaticText *label=new wxStaticText(this, -1, config->ReadString("label", ""), wxDefaultPosition, wxSize(75, -1), wxST_NO_AUTORESIZE);
	mSpinCtrlX=new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(50, -1), wxTE_RIGHT|wxTE_PROCESS_ENTER);
	mSpinCtrlY=new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(50, -1), wxTE_RIGHT|wxTE_PROCESS_ENTER);
	wxStaticText *labelx=new wxStaticText(this, -1, config->ReadString("labelx", "X")+" ", wxDefaultPosition, wxSize(40, -1), wxST_NO_AUTORESIZE|wxALIGN_RIGHT);
	wxStaticText *labely=new wxStaticText(this, -1, config->ReadString("labely", "Y")+" ", wxDefaultPosition, wxSize(40, -1), wxST_NO_AUTORESIZE|wxALIGN_RIGHT);

	// Layout the controls
	wxBoxSizer *hs=new wxBoxSizer(wxHORIZONTAL);
	hs->Add(labelx, 0, wxALIGN_CENTER_VERTICAL, 0);
	hs->Add(mSpinCtrlX, 0, wxALIGN_CENTER_VERTICAL, 0);
	hs->Add(labely, 0, wxALIGN_CENTER_VERTICAL, 0);
	hs->Add(mSpinCtrlY, 0, wxALIGN_CENTER_VERTICAL, 0);

	wxBoxSizer *vs=new wxBoxSizer(wxVERTICAL);
	vs->Add(label, 0, 0, 0);
	vs->Add(hs, 0, 0, 0);
	SetSizer(vs);
}

void THISCLASS::OnUpdate() {
	int valuex=mComponent->GetConfigurationInt((mName+".x").c_str(), mValueDefaultX);
	int valuey=mComponent->GetConfigurationInt((mName+".y").c_str(), mValueDefaultY);
	if (mFocusWindow!=mSpinCtrlX) {mSpinCtrlX->SetValue(wxString::Format("%d", valuex));}
	if (mFocusWindow!=mSpinCtrlX) {mSpinCtrlY->SetValue(wxString::Format("%d", valuey));}
}

void THISCLASS::OnTextUpdated(wxCommandEvent& event) {
	long valuex=(long)mValueDefaultX;
	mSpinCtrlX->GetValue().ToLong(&valuex);
	long valuey=(long)mValueDefaultY;
	mSpinCtrlY->GetValue().ToLong(&valuey);
	SetValue(valuex, valuey);
}

void THISCLASS::OnTextEnter(wxCommandEvent& event) {
	OnTextUpdated(event);
}

void THISCLASS::OnSpin(wxSpinEvent& event) {
	SetValue(mSpinCtrl->GetValue());
}

void THISCLASS::SetValue(int valuex, int valuey) {
	// If we are in OnUpdate(), do nothing
	if (mUpdating) {return;}

	// Check bounds
	if (valuex<mValueMinX) {valuex=mValueMinX;}
	if (valuex>mValueMaxX) {valuex=mValueMaxX;}
	if (valuey<mValueMinY) {valuey=mValueMinY;}
	if (valuey>mValueMaxY) {valuey=mValueMaxY;}

	// Check if the same value is set already
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
