#include "ConfigurationParameterPointDouble.h"
#define THISCLASS ConfigurationParameterPointDouble

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <algorithm>
#include "SwisTrackCoreEditor.h"
#include "ComponentEditor.h"

BEGIN_EVENT_TABLE(THISCLASS, wxPanel)
	EVT_TEXT (wxID_ANY, THISCLASS::OnTextUpdated)
	EVT_TEXT_ENTER (wxID_ANY, THISCLASS::OnTextEnter)
END_EVENT_TABLE()

THISCLASS::ConfigurationParameterPointDouble(wxWindow* parent):
		ConfigurationParameter(parent),
		mValueMinX(0), mValueMinY(0),
		mValueMaxX(1), mValueMaxY(1),
		mValueDefaultX(0), mValueDefaultY(0) {

}

THISCLASS::~ConfigurationParameterPointDouble() {
	mTextCtrlX->Disconnect(wxID_ANY, wxEVT_KILL_FOCUS, wxFocusEventHandler(THISCLASS::OnKillFocus), 0, this);
	mTextCtrlY->Disconnect(wxID_ANY, wxEVT_KILL_FOCUS, wxFocusEventHandler(THISCLASS::OnKillFocus), 0, this);
}

void THISCLASS::OnInitialize(ConfigurationXML *config, ErrorList *errorlist) {
	// Read specific configuration
	config->SelectRootNode();
	config->SelectChildNode(wxT("min"));
	mValueMinX = config->ReadDouble(wxT("x"), -DBL_MAX);
	mValueMinY = config->ReadDouble(wxT("y"), -DBL_MAX);
	config->SelectRootNode();
	config->SelectChildNode(wxT("max"));
	mValueMaxX = config->ReadDouble(wxT("x"), DBL_MAX);
	mValueMaxY = config->ReadDouble(wxT("y"), DBL_MAX);
	config->SelectRootNode();
	config->SelectChildNode(wxT("default"));
	mValueDefaultX = config->ReadDouble(wxT("x"), 0);
	mValueDefaultY = config->ReadDouble(wxT("y"), 0);

	// Create the controls
	config->SelectRootNode();
	wxStaticText *label = new wxStaticText(this, wxID_ANY, config->ReadString(wxT("label"), wxT("")), wxDefaultPosition, wxSize(scParameterWidth, -1), wxST_NO_AUTORESIZE);
	wxStaticText *dummyx = new wxStaticText(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(scIndentWidth, -1), wxST_NO_AUTORESIZE);
	wxStaticText *dummyy = new wxStaticText(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(scIndentWidth, -1), wxST_NO_AUTORESIZE);
	wxStaticText *labelx = new wxStaticText(this, wxID_ANY, config->ReadString(wxT("labelx"), wxT("X")), wxDefaultPosition, wxSize(scLabelWidth - scIndentWidth, -1), wxST_NO_AUTORESIZE);
	wxStaticText *labely = new wxStaticText(this, wxID_ANY, config->ReadString(wxT("labely"), wxT("Y")), wxDefaultPosition, wxSize(scLabelWidth - scIndentWidth, -1), wxST_NO_AUTORESIZE);
	mTextCtrlX = new wxTextCtrl(this, cID_X, wxT(""), wxDefaultPosition, wxSize(scTextBoxWidth, -1), wxTE_RIGHT | wxTE_PROCESS_ENTER);
	mTextCtrlX->Connect(wxID_ANY, wxEVT_KILL_FOCUS, wxFocusEventHandler(THISCLASS::OnKillFocus), 0, this);
	mTextCtrlY = new wxTextCtrl(this, cID_Y, wxT(""), wxDefaultPosition, wxSize(scTextBoxWidth, -1), wxTE_RIGHT | wxTE_PROCESS_ENTER);
	mTextCtrlY->Connect(wxID_ANY, wxEVT_KILL_FOCUS, wxFocusEventHandler(THISCLASS::OnKillFocus), 0, this);
	wxStaticText *unitx = new wxStaticText(this, wxID_ANY, wxT(" ") + config->ReadString(wxT("unitx"), wxT("")), wxDefaultPosition, wxSize(scUnitWidth, -1), wxST_NO_AUTORESIZE);
	wxStaticText *unity = new wxStaticText(this, wxID_ANY, wxT(" ") + config->ReadString(wxT("unity"), wxT("")), wxDefaultPosition, wxSize(scUnitWidth, -1), wxST_NO_AUTORESIZE);

	// Layout the controls
	wxBoxSizer *hsx = new wxBoxSizer(wxHORIZONTAL);
	hsx->Add(dummyx, 0, wxALIGN_CENTER_VERTICAL, 0);
	hsx->Add(labelx, 0, wxALIGN_CENTER_VERTICAL, 0);
	hsx->Add(mTextCtrlX, 0, wxALIGN_CENTER_VERTICAL, 0);
	hsx->Add(unitx, 0, wxALIGN_CENTER_VERTICAL, 0);
	wxBoxSizer *hsy = new wxBoxSizer(wxHORIZONTAL);
	hsy->Add(dummyy, 0, wxALIGN_CENTER_VERTICAL, 0);
	hsy->Add(labely, 0, wxALIGN_CENTER_VERTICAL, 0);
	hsy->Add(mTextCtrlY, 0, wxALIGN_CENTER_VERTICAL, 0);
	hsy->Add(unity, 0, wxALIGN_CENTER_VERTICAL, 0);

	wxBoxSizer *vs = new wxBoxSizer(wxVERTICAL);
	vs->Add(label, 0, wxBOTTOM, 2);
	vs->Add(hsx, 0, wxBOTTOM, 2);
	vs->Add(hsy, 0, 0, 0);
	SetSizer(vs);
}

void THISCLASS::OnUpdate(wxWindow *updateprotection) {
	double valuex = mComponent->GetConfigurationDouble(mName + wxT(".x"), mValueDefaultX);
	double valuey = mComponent->GetConfigurationDouble(mName + wxT(".y"), mValueDefaultY);
	if (updateprotection != mTextCtrlX) {
		mTextCtrlX->SetValue(wxString::Format(wxT("%f"), valuex));
	}
	if (updateprotection != mTextCtrlY) {
		mTextCtrlY->SetValue(wxString::Format(wxT("%f"), valuey));
	}
}

bool THISCLASS::ValidateNewValueX() {
	bool valid = true;

	// Check bounds
	if (mNewValueX < mValueMinX) {
		mNewValueX = mValueMinX;
		valid = false;
	}
	if (mNewValueX > mValueMaxX) {
		mNewValueX = mValueMaxX;
		valid = false;
	}

	return valid;
}

bool THISCLASS::ValidateNewValueY() {
	bool valid = true;

	// Check bounds
	if (mNewValueY < mValueMinY) {
		mNewValueY = mValueMinY;
		valid = false;
	}
	if (mNewValueY > mValueMaxY) {
		mNewValueY = mValueMaxY;
		valid = false;
	}

	return valid;
}

bool THISCLASS::CompareNewValue() {
	double valuex = mComponent->GetConfigurationDouble(mName + wxT(".x"), mValueDefaultX);
	double valuey = mComponent->GetConfigurationDouble(mName + wxT(".y"), mValueDefaultY);
	return ((valuex == mNewValueX) && (valuey == mNewValueY));
}

void THISCLASS::OnSetNewValue() {
	ComponentEditor ce(mComponent);
	ce.SetConfigurationDouble(mName + wxT(".x"), mNewValueX);
	ce.SetConfigurationDouble(mName + wxT(".y"), mNewValueY);
}

void THISCLASS::OnTextUpdated(wxCommandEvent& event) {
	mNewValueX = mValueDefaultX;
	mTextCtrlX->GetValue().ToDouble(&mNewValueX);
	if (ValidateNewValueX()) {
		mTextCtrlX->SetOwnForegroundColour(*wxBLACK);
	} else {
		mTextCtrlX->SetOwnForegroundColour(*wxRED);
	}
	mTextCtrlX->Refresh();

	mNewValueY = mValueDefaultY;
	mTextCtrlY->GetValue().ToDouble(&mNewValueY);
	if (ValidateNewValueY()) {
		mTextCtrlY->SetOwnForegroundColour(*wxBLACK);
	} else {
		mTextCtrlY->SetOwnForegroundColour(*wxRED);
	}
	mTextCtrlY->Refresh();

	if (CompareNewValue()) {
		return;
	}
	if (event.GetId() == cID_X) {
		SetNewValue(mTextCtrlX);
	} else {
		SetNewValue(mTextCtrlY);
	}
}

void THISCLASS::OnTextEnter(wxCommandEvent& event) {
	mNewValueX = mValueDefaultX;
	mTextCtrlX->GetValue().ToDouble(&mNewValueX);
	ValidateNewValueX();

	mNewValueY = mValueDefaultY;
	mTextCtrlY->GetValue().ToDouble(&mNewValueY);
	ValidateNewValueY();

	SetNewValue();
}

void THISCLASS::OnKillFocus(wxFocusEvent& event) {
	wxCommandEvent ev;
	OnTextEnter(ev);
}
