#include "ConfigurationParameterInteger.h"
#define THISCLASS ConfigurationParameterInteger

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <algorithm>
#include "SwisTrackCoreEditor.h"

BEGIN_EVENT_TABLE(THISCLASS, wxPanel)
  EVT_BUTTON (eID_ButtonAdd, THISCLASS::OnButtonAddClick)
  EVT_BUTTON (eID_ButtonRemove, THISCLASS::OnButtonRemoveClick)
  EVT_BUTTON (eID_ButtonUp, THISCLASS::OnButtonUpClick)
  EVT_BUTTON (eID_ButtonDown, THISCLASS::OnButtonDownClick)
  EVT_LIST_ITEM_SELECTED (eID_List, THISCLASS::OnListItemSelected)
END_EVENT_TABLE()

THISCLASS::ConfigurationParameterInteger(wxWindow* parent, wxXmlNode *node):
		ConfigurationParameter(parent, node) {

	// Read the configuration
	
	
	// Create the controls
	wxStaticText *label=new wxStaticText(this, -1, GetString("name", ""), wxDefaultPosition, wxSize(100, 25), wxST_NO_AUTORESIZE);
	mTextCtrl=new wxStaticText(this, -1, "", wxDefaultPosition, wxSize(50, 25), wxTE_RIGHT);
	wxStaticText *unitlabel=new wxStaticText(this, -1, GetString("unit"), wxDefaultPosition, wxSize(50, 25), wxST_NO_AUTORESIZE);

	wxBoxSizer *hs=new wxBoxSizer(wxHORIZONTAL);
	hs->Add(label, 1, 0, 0);
	hs->Add(mTextCtrl, 0, 0, 0);
	hs->Add(unitlabel, 0, 0, 0);
	SetSizer(hs);
}

THISCLASS::~ConfigurationParameterInteger() {
}

void THISCLASS::OnUpdate() {

}

void THISCLASS::OnListItemSelected(wxListEvent& event) {
	mSelectedComponent=(Component*)(event.GetData());

	// TODO: show configuration panel of this component
}
