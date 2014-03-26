#include "ConfigurationParameterInputFile.h"
#define THISCLASS ConfigurationParameterInputFile

#include <wx/sizer.h>
#include <wx/stattext.h>
#include <algorithm>
#include "SwisTrackCoreEditor.h"
#include "ComponentEditor.h"

BEGIN_EVENT_TABLE(THISCLASS, wxPanel)
	EVT_TEXT_ENTER (wxID_ANY, THISCLASS::OnTextEnter)
	EVT_BUTTON (wxID_ANY, THISCLASS::OnButtonClicked)
END_EVENT_TABLE()

THISCLASS::ConfigurationParameterInputFile(wxWindow* parent):
		ConfigurationParameter(parent),
		mTextCtrl(0), mButton(0),
		mValueDefault(wxT("")) {

}

THISCLASS::~ConfigurationParameterInputFile() {
}

void THISCLASS::OnInitialize(ConfigurationXML *config, ErrorList *errorlist) {
	// Read specific configuration
	config->SelectRootNode();
	mValueDefault = config->ReadString(wxT("default"), wxT(""));

	// Prepare file filter
	wxString filetype = config->ReadString(wxT("filetype"), wxT(""));
	if (filetype == wxT("image")) {
		mFileFilter = wxT("All known formats|*.bmp;*.dib;*.jpeg;*.jpg;*.jpe;*.png;*.pmb;*.pgm;*.ppm;*.sr;*.ras;*.tiff;*.tif;*.exr;*.jp2|Windows bitmaps|*.bmp;*.dib|JPEG files|*.jpeg;*.jpg;*.jpe|PNG|*.png|Portable image format|*.pmb;*.pgm;*.ppm|Sun raster format|*.sr;*.ras|TIFF files|*.tiff;*.tif|OpenEXR HDR images|*.exr|JPEG 2000 images|*.jp2|All files|*.*");
	} else {
		mFileFilter = wxT("All files|*.*");
	}
	mFileFilter = config->ReadString(wxT("filefilter"), mFileFilter);

	// Create the controls
	wxStaticText *label = new wxStaticText(this, wxID_ANY, config->ReadString(wxT("label"), wxT("")), wxDefaultPosition, wxSize(scLabelWidth, -1), wxST_NO_AUTORESIZE);
	mTextCtrl = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(scParameterWidth - 25, -1), wxTE_PROCESS_ENTER);
	mTextCtrl->Connect(wxID_ANY, wxEVT_KILL_FOCUS, wxFocusEventHandler(THISCLASS::OnKillFocus), 0, this);
	mButton = new wxButton(this, wxID_ANY, wxT("..."), wxDefaultPosition, wxSize(25, -1), wxST_NO_AUTORESIZE);

	// Layout the controls
	wxBoxSizer *hs = new wxBoxSizer(wxHORIZONTAL);
	hs->Add(mTextCtrl, 0, wxALIGN_CENTER_VERTICAL, 0);
	hs->Add(mButton, 0, wxALIGN_CENTER_VERTICAL, 0);

	wxBoxSizer *vs = new wxBoxSizer(wxVERTICAL);
	vs->Add(label, 0, wxBOTTOM, 2);
	vs->Add(hs, 0, 0, 0);
	SetSizer(vs);
}

void THISCLASS::OnUpdate(wxWindow *updateprotection) {
	if (updateprotection == mTextCtrl) {
		return;
	}
	wxString value = mComponent->GetConfigurationString(mName, mValueDefault);
	mTextCtrl->SetValue(value);
}

bool THISCLASS::ValidateNewValue() {
	return true;
}

bool THISCLASS::CompareNewValue() {
	wxString value = mComponent->GetConfigurationString(mName, mValueDefault);
	return (value == mNewValue);
}

void THISCLASS::OnSetNewValue() {
	ComponentEditor ce(mComponent);
	ce.SetConfigurationString(mName, mNewValue);
}

void THISCLASS::OnButtonClicked(wxCommandEvent& event) {
	// Normalize the current path
	wxFileName current_filename = mSwisTrack->mSwisTrackCore->GetProjectFileName(mTextCtrl->GetValue());
	wxString current_fullpath;
	if (current_filename.IsOk()) {
		current_fullpath = current_filename.GetFullPath();
	}

	// Show the file open window
	long dialogStyle = wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW;
	wxFileDialog dlg(this, wxT("Select file"), wxT(""), current_fullpath, mFileFilter, dialogStyle);
	if (dlg.ShowModal() != wxID_OK) {
		return;
	}

	// Make the path relative to the project folder or the home directory, if possible
	wxFileName filename(dlg.GetPath());
	wxFileName filename_relative_to_project(filename);
	filename_relative_to_project.MakeRelativeTo(mSwisTrack->mSwisTrackCore->GetFileName().GetPath());
	wxArrayString filename_relative_to_project_dirs = filename_relative_to_project.GetDirs();
	wxFileName filename_relative_to_home(filename);
	filename_relative_to_home.MakeRelativeTo(wxFileName::GetHomeDir());
	wxArrayString filename_relative_to_home_dirs = filename_relative_to_home.GetDirs();
	if ((filename_relative_to_project_dirs.GetCount() == 0) || (filename_relative_to_project_dirs [0] != wxT(".."))) {
		mNewValue = filename_relative_to_project.GetFullPath();
	} else if ((filename_relative_to_home_dirs.GetCount() == 0) || (filename_relative_to_home_dirs[0] != wxT(".."))) {
		filename_relative_to_home.PrependDir(wxT("~"));
		mNewValue = filename_relative_to_home.GetFullPath();
	} else {
		mNewValue = filename.GetFullPath();
	}

	// Set the new value
	ValidateNewValue();
	SetNewValue();
}

void THISCLASS::OnTextEnter(wxCommandEvent& event) {
	mNewValue = mTextCtrl->GetValue();
	ValidateNewValue();
	SetNewValue();
}

void THISCLASS::OnKillFocus(wxFocusEvent& event) {
	wxCommandEvent ev;
	OnTextEnter(ev);
}
