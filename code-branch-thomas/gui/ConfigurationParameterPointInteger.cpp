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
	mSpinCtrlX->Disconnect(wxID_ANY, wxEVT_KILL_FOCUS, wxFocusEventHandler(THISCLASS::OnKillFocus), 0, this);
	mSpinCtrlY->Disconnect(wxID_ANY, wxEVT_KILL_FOCUS, wxFocusEventHandler(THISCLASS::OnKillFocus), 0, this);
}

void THISCLASS::OnInitialize(ConfigurationXML *config, ErrorList *errorlist) {
	// Read specific configuration
	config->SelectRootNode();
	config->SelectChildNode("min");
	mValueMinX=config->ReadInt("x", INT_MIN);
	mValueMinY=config->ReadInt("y", INT_MIN);
	config->SelectRootNode();
	config->SelectChildNode("max");
	mValueMaxX=config->ReadInt("x", INT_MAX);
	mValueMaxY=config->ReadInt("y", INT_MAX);
	config->SelectRootNode();
	config->SelectChildNode("default");
	mValueDefaultX=config->ReadInt("x", 0);
	mValueDefaultY=config->ReadInt("y", 0);

	// Create the controls
	config->SelectRootNode();
	wxStaticText *label=new wxStaticText(this, wxID_ANY, config->ReadString("label", ""), wxDefaultPosition, wxSize(scParameterWidth, -1), wxST_NO_AUTORESIZE);
	wxStaticText *dummyx=new wxStaticText(this, wxID_ANY, "", wxDefaultPosition, wxSize(scIndentWidth, -1), wxST_NO_AUTORESIZE);
	wxStaticText *dummyy=new wxStaticText(this, wxID_ANY, "", wxDefaultPosition, wxSize(scIndentWidth, -1), wxST_NO_AUTORESIZE);
	wxStaticText *labelx=new wxStaticText(this, wxID_ANY, config->ReadString("labelx", "X"), wxDefaultPosition, wxSize(scLabelWidth-scIndentWidth, -1), wxST_NO_AUTORESIZE);
	wxStaticText *labely=new wxStaticText(this, wxID_ANY, config->ReadString("labely", "Y"), wxDefaultPosition, wxSize(scLabelWidth-scIndentWidth, -1), wxST_NO_AUTORESIZE);
	mSpinCtrlX=new wxSpinCtrl(this, sID_X, "", wxDefaultPosition, wxSize(scTextBoxWidth, -1), wxTE_RIGHT|wxTE_PROCESS_ENTER, mValueMinX, mValueMaxX, mValueDefaultX);
	mSpinCtrlX->Connect(wxID_ANY, wxEVT_KILL_FOCUS, wxFocusEventHandler(THISCLASS::OnKillFocus), 0, this);
	mSpinCtrlY=new wxSpinCtrl(this, sID_Y, "", wxDefaultPosition, wxSize(scTextBoxWidth, -1), wxTE_RIGHT|wxTE_PROCESS_ENTER, mValueMinY, mValueMaxY, mValueDefaultY);
	mSpinCtrlY->Connect(wxID_ANY, wxEVT_KILL_FOCUS, wxFocusEventHandler(THISCLASS::OnKillFocus), 0, this);
	wxStaticText *unitx=new wxStaticText(this, wxID_ANY, " "+config->ReadString("unitx", "X"), wxDefaultPosition, wxSize(scUnitWidth, -1), wxST_NO_AUTORESIZE);
	wxStaticText *unity=new wxStaticText(this, wxID_ANY, " "+config->ReadString("unity", "Y"), wxDefaultPosition, wxSize(scUnitWidth, -1), wxST_NO_AUTORESIZE);

	// Layout the controls
	wxBoxSizer *hsx=new wxBoxSizer(wxHORIZONTAL);
	hsx->Add(dummyx, 0, wxALIGN_CENTER_VERTICAL, 0);
	hsx->Add(labelx, 0, wxALIGN_CENTER_VERTICAL, 0);
	hsx->Add(mSpinCtrlX, 0, wxALIGN_CENTER_VERTICAL, 0);
	hsx->Add(unitx, 0, wxALIGN_CENTER_VERTICAL, 0);
	wxBoxSizer *hsy=new wxBoxSizer(wxHORIZONTAL);
	hsy->Add(dummyy, 0, wxALIGN_CENTER_VERTICAL, 0);
	hsy->Add(labely, 0, wxALIGN_CENTER_VERTICAL, 0);
	hsy->Add(mSpinCtrlY, 0, wxALIGN_CENTER_VERTICAL, 0);
	hsy->Add(unity, 0, wxALIGN_CENTER_VERTICAL, 0);

	wxBoxSizer *vs=new wxBoxSizer(wxVERTICAL);
	vs->Add(label, 0, wxBOTTOM, 2);
	vs->Add(hsx, 0, 0, 0);
	vs->Add(hsy, 0, 0, 0);
	SetSizer(vs);
}

void THISCLASS::OnUpdate(wxWindow *updateprotection) {
	double valuex=mComponent->GetConfigurationDouble((mName+".x").c_str(), mValueDefaultX);
	double valuey=mComponent->GetConfigurationDouble((mName+".y").c_str(), mValueDefaultY);
	if (updateprotection!=mSpinCtrlX) {mSpinCtrlX->SetValue(valuex);}
	if (updateprotection!=mSpinCtrlY) {mSpinCtrlY->SetValue(valuey);}
}

bool THISCLASS::ValidateNewValueX() {
	bool valid=true;

	// Check bounds
	if (mNewValueX<mValueMinX) {mNewValueX=mValueMinX; valid=false;}
	if (mNewValueX>mValueMaxX) {mNewValueX=mValueMaxX; valid=false;}

	return valid;
}

bool THISCLASS::ValidateNewValueY() {
	bool valid=true;

	// Check bounds
	if (mNewValueY<mValueMinY) {mNewValueY=mValueMinY; valid=false;}
	if (mNewValueY>mValueMaxY) {mNewValueY=mValueMaxY; valid=false;}

	return valid;
}

bool THISCLASS::CompareNewValue() {
	int valuex=mComponent->GetConfigurationInt((mName+".x").c_str(), mValueDefaultX);
	int valuey=mComponent->GetConfigurationInt((mName+".y").c_str(), mValueDefaultY);
	return ((valuex==mNewValueX) && (valuey==mNewValueY));
}

void THISCLASS::OnSetNewValue() {
	ComponentEditor ce(mComponent);
	ce.SetConfigurationInt((mName+".x").c_str(), mNewValueX);
	ce.SetConfigurationInt((mName+".y").c_str(), mNewValueY);
}

void THISCLASS::OnTextUpdated(wxCommandEvent& event) {
	mNewValueX=mSpinCtrlX->GetValue();
	if (ValidateNewValueX()) {
		mSpinCtrlX->SetOwnForegroundColour(*wxBLACK);
	} else {
		mSpinCtrlX->SetOwnForegroundColour(*wxRED);
	}
	mSpinCtrlX->Refresh();

	mNewValueY=mSpinCtrlY->GetValue();
	if (ValidateNewValueY()) {
		mSpinCtrlX->SetOwnForegroundColour(*wxBLACK);
	} else {
		mSpinCtrlX->SetOwnForegroundColour(*wxRED);
	}
	mSpinCtrlX->Refresh();

	if (CompareNewValue()) {return;}
	if (event.GetId()==sID_X) {
		SetNewValue(mSpinCtrlX);
	} else {
		SetNewValue(mSpinCtrlX);
	}
}

void THISCLASS::OnTextEnter(wxCommandEvent& event) {
	mNewValueX=mSpinCtrlX->GetValue();
	ValidateNewValueX();

	mNewValueY=mSpinCtrlY->GetValue();
	ValidateNewValueY();

	SetNewValue();
}

void THISCLASS::OnSpin(wxSpinEvent& event) {
	mNewValueX=mSpinCtrlX->GetValue();
	ValidateNewValueX();

	mNewValueY=mSpinCtrlY->GetValue();
	ValidateNewValueY();

	SetNewValue();
}

void THISCLASS::OnKillFocus(wxFocusEvent& event) {
	OnTextEnter(wxCommandEvent());
}