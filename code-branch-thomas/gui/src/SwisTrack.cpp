#include "SwisTrack.h"
#define THISCLASS SwisTrack

#include "Application.h"
#include "CanvasPanel.h"
#include "TCPServer.h"
#include "ErrorListDialog.h"
#include "SwisTrackCoreEditor.h"
#include "ConfigurationReaderXML.h"
#include "ConfigurationWriterXML.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <highgui.h>

#include <wx/toolbar.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/image.h>
#include <wx/valgen.h>

#if defined(__WXGTK__) || defined(__WXMOTIF__)
#include "bitmaps/gui.xpm"
#include "bitmaps/new.xpm"
#include "bitmaps/open.xpm"
#include "bitmaps/save.xpm"
#include "bitmaps/finger.xpm"
#include "bitmaps/play.xpm"
#include "bitmaps/pause.xpm"
#include "bitmaps/singlestep.xpm"
#endif

BEGIN_EVENT_TABLE(THISCLASS, wxFrame)
	EVT_MENU(sID_New, THISCLASS::OnFileNew)
	EVT_MENU(sID_Open, THISCLASS::OnFileOpen)
	EVT_MENU(sID_Save, THISCLASS::OnFileSave)
	EVT_MENU(sID_SaveAs, THISCLASS::OnFileSaveAs)
	EVT_MENU(sID_Quit,  THISCLASS::OnFileQuit)
	EVT_MENU(sID_Control_ProductiveMode, THISCLASS::OnControlProductiveMode)
	EVT_MENU(sID_Control_FreeRun, THISCLASS::OnControlFreeRun)
	EVT_MENU(sID_Control_SingleStep, THISCLASS::OnControlSingleStep)
	EVT_MENU(sID_Tools_Server, THISCLASS::OnToolsServer)
	EVT_COMMAND_SCROLL(sID_DisplaySpeed, THISCLASS::OnChangeDisplaySpeed)
	EVT_MENU(sID_Help, THISCLASS::OnHelp)
	EVT_MENU(sID_Test, THISCLASS::OnTest)
	EVT_MENU(sID_About, THISCLASS::OnHelpAbout)
	EVT_TIMER(wxID_ANY, THISCLASS::OnFreeRunTimer)
	EVT_IDLE(THISCLASS::OnIdle)
END_EVENT_TABLE()

SwisTrack::SwisTrack(const wxString& title, const wxPoint& pos, const wxSize& size, long style):
		wxFrame(NULL, -1, title, pos, size, style),
		CommunicationCommandHandler(),
		mSwisTrackCore(0), mTCPServer(0), mFileName(""), mTriggerFreeRunInterval(1000),
		mHorizontalSizer(0), mCanvasPanel(0), mComponentListPanel(0), mConfigurationPanel(0) {

#ifdef MULTITHREAD
	criticalSection = new wxCriticalSection();
#endif

	// General initialization
	SetIcon(wxICON(gui));
	BuildMenuBar();
	BuildToolBar();
	BuildStatusBar();

	// The canvas panel
	mCanvasPanel=new CanvasPanel(this);

	// SwisTrackCore and TCPServer
	mSwisTrackCore=new SwisTrackCore();
	mTCPServer=new TCPServer(this);
	mSwisTrackCore->mCommunicationInterface=mTCPServer;	
	mTCPServer->AddCommandHandler(this);

	// List
	mComponentListPanel=new ComponentListPanel(this, this);

	// Configuration panel
	mConfigurationPanel=new ConfigurationPanel(this, this, 0);

	// Setup frame contents
	mHorizontalSizer=new wxBoxSizer(wxHORIZONTAL);
	mHorizontalSizer->Add(mCanvasPanel, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 10);
	mHorizontalSizer->Add(mConfigurationPanel, 0, wxEXPAND, 0);

	wxBoxSizer *vs=new wxBoxSizer(wxVERTICAL);
	vs->Add(mHorizontalSizer, 3, wxEXPAND, 0);
	vs->Add(mComponentListPanel, 1, wxEXPAND, 0);
	SetSizer(vs);

	// The timer used for the free run mode
	mTriggerFreeRunTimer=new wxTimer(this);
}

SwisTrack::~SwisTrack(){
	SetTriggerManual();
	StopProductiveMode();

	// Delete the trigger timer
	delete mTriggerFreeRunTimer;

	// Delete the components that rely on SwisTrackCore
	mComponentListPanel->Destroy();
	mConfigurationPanel->Destroy();

	// Delete the TCP server and the SwisTrackCore object
	delete mTCPServer;
	delete mSwisTrackCore;
}

void THISCLASS::BuildMenuBar() {
	// Create a menu bar
	SetMenuBar(new wxMenuBar());

	// Create menus
	wxMenu *menufile = new wxMenu;
	GetMenuBar()->Append(menufile, _T("&File"));

	wxMenu *menutools = new wxMenu;
	GetMenuBar()->Append(menutools,_T("&Tools"));

	wxMenu *menuhelp = new wxMenu;
	GetMenuBar()->Append(menuhelp, _T("&Help"));

	menufile->Append(sID_New, _T("&New\tCtrl-C"), _T("Creates a new file"));
	menufile->Append(sID_Open, _T("&Open\tCtrl-O"), _T("Opens a file"));
	menufile->Append(sID_Save, _T("&Save\tCtrl-S"), _T("Saves the current file"));
	menufile->Append(sID_SaveAs, _T("&Save as ..."), _T("Saves the file with another name"));
	menufile->AppendSeparator();
	menufile->Append(sID_Quit, _T("E&xit\tAlt-F4"), _T("Quit this program"));
	menufile->Enable(sID_Save,FALSE);

	menutools->Append(sID_Tools_Server, _T("TCP Server ..."), _T("TCP server settings"));

	menuhelp->Append(sID_Help, _T("&Manual"), _T("Opens the manual"));
	menuhelp->Append(sID_Test, _T("&Test"), _T("Test"));
	menuhelp->Append(sID_About, _T("&About ...\tF1"), _T("Show about dialog"));
}

void THISCLASS::BuildToolBar() {
	// Delete the old toolbar (if any)
	wxToolBarBase *toolbar = GetToolBar();
	delete toolbar;
	SetToolBar(NULL);

	// Create toolbar
	toolbar = CreateToolBar(wxTB_FLAT|wxTB_DOCKABLE|wxTB_TEXT);
	toolbar->AddTool(sID_New, _T("New"), wxBITMAP(new), _T("New"));
	toolbar->AddTool(sID_Open, _T("Open"), wxBITMAP(open), _T("Open"));
	toolbar->AddTool(sID_Save, _T("Save"), wxBITMAP(save), _T("Save"));
	toolbar->AddSeparator();
	toolbar->AddTool(sID_Control_ProductiveMode, _T("Productive"), wxBITMAP(play), _T("Productive"), wxITEM_CHECK);
	toolbar->AddSeparator();
	toolbar->AddTool(sID_Control_FreeRun, _T("Free-run"), wxBITMAP(play), _T("Free-run mode"), wxITEM_CHECK);
	toolbar->AddTool(sID_Control_SingleStep, _T("Step"), wxBITMAP(singlestep), _T("One step"));
	toolbar->AddSeparator();

	int fps=30;
	wxSlider *slider = new wxSlider(toolbar, sID_DisplaySpeed, 0, 0,(int) fps, wxDefaultPosition, wxSize(155,-1), wxSL_AUTOTICKS|wxSL_LABELS, wxGenericValidator(&mDisplaySpeed));
	slider->SetTickFreq(5, 0);
	slider->SetToolTip(_T("Display refresh (Hz)"));
	slider->SetValue(mDisplaySpeed);
	toolbar->AddControl(slider);

	toolbar->Realize();
	toolbar->SetRows(1 ? 1 : 10 / 1);
}

void THISCLASS::BuildStatusBar() {
#if wxUSE_STATUSBAR
	int n=3;
	int w[3]={-1, 50, 120};

	CreateStatusBar(n);
	SetStatusWidths(n, w);

	SetStatusText(_T("Welcome to SwisTrack!"), sStatusField_Messages);
	SetStatusText(_T("Closed"), sStatusField_ServerPort);
	SetStatusText(_T("Manual"), sStatusField_Trigger);
#endif // wxUSE_STATUSBAR
}

void THISCLASS::SetConfigurationPanel(Component *c) {
	if (mConfigurationPanel->mComponent==c) {return;}

	// Destroy the hold panel
	mConfigurationPanel->Destroy();

	// Create a new panel
	mConfigurationPanel=new ConfigurationPanel(this, this, c);
	mHorizontalSizer->Add(mConfigurationPanel, 0, wxEXPAND, 0);
	GetSizer()->Layout();

	// Show errors
	if (mConfigurationPanel->mErrorList.mList.empty()) {return;}
	if (mSwisTrackCore->IsStartedInProductiveMode()) {return;}
	ErrorListDialog eld(this, &(mConfigurationPanel->mErrorList), "Component configuration", "The following errors occurred while reading the component configuration description:");
	eld.ShowModal();
}

void THISCLASS::SingleStep() {
	// Start (if necessary) and perform a step
	mSwisTrackCore->Start(false);
	mSwisTrackCore->Step();
}

void THISCLASS::ReloadConfiguration() {
	// In productive mode, we just call ReloadConfiguration which may not update all configuration parameters.
	// Otherwise, we stop the simulation (it is automatically restarted upon the next step).
	if (mSwisTrackCore->IsStartedInProductiveMode()) {
		mSwisTrackCore->ReloadConfiguration();
	} else {
		mSwisTrackCore->Stop();
	}
}

void THISCLASS::StartProductiveMode() {
	if (mSwisTrackCore->IsStartedInProductiveMode()) {return;}
	mSwisTrackCore->Stop();
	GetToolBar()->ToggleTool(sID_Control_ProductiveMode, true);
	mSwisTrackCore->Start(true);
}

void THISCLASS::StopProductiveMode() {
	if (! mSwisTrackCore->IsStartedInProductiveMode()) {return;}
	mSwisTrackCore->Stop();
	GetToolBar()->ToggleTool(sID_Control_ProductiveMode, false);
}

void THISCLASS::SetTriggerManual() {
	GetToolBar()->ToggleTool(sID_Control_FreeRun, false);
	SetStatusText("Manual", sStatusField_Trigger);
	mTriggerFreeRunTimer->Stop();
}

void THISCLASS::SetTriggerFreeRun() {
	GetToolBar()->ToggleTool(sID_Control_FreeRun, true);
	if (! mTriggerFreeRunTimer->IsRunning()) {
		mTriggerFreeRunTimer->Start(mTriggerFreeRunInterval);
	}
}

void THISCLASS::OnFreeRunTimer(wxTimerEvent& WXUNUSED(event)) {
	SingleStep();
	if (mTriggerFreeRunTimer->GetInterval()!=mTriggerFreeRunInterval) {
		mTriggerFreeRunTimer->Stop();
		mTriggerFreeRunTimer->Start(mTriggerFreeRunInterval);
		SetStatusText(wxString::Format("%d ms / %.1f FPS", mTriggerFreeRunInterval, 1/(double)mTriggerFreeRunInterval), sStatusField_Trigger);
	}
}

bool THISCLASS::OnCommunicationCommand(CommunicationMessage *m) {
	if (m->mCommand=="START") {
		StartProductiveMode();
		return true;
	} else if (m->mCommand=="STOP") {
		StopProductiveMode();
		return true;
	} else if (m->mCommand=="STEP") {
		SingleStep();
		return true;
	} else if (m->mCommand=="RELOADCONFIGURATION") {
		ReloadConfiguration();
		return true;
	} else if (m->mCommand=="TRIGGER") {
		std::string type=m->GetString("MANUAL");
		std::transform(type.begin(), type.end(), type.begin(), std::toupper);
		if (type=="FREERUN") {
			mTriggerFreeRunInterval=m->GetDouble(1);
			SetTriggerFreeRun();
		} else {
			SetTriggerManual();
		}
		return true;
	}

	return false;
}

void SwisTrack::OnFileNew(wxCommandEvent& WXUNUSED(event)) {
	OpenFile(wxGetApp().mApplicationFolder+"/default.swistrack", false, true);
}

void THISCLASS::OnFileOpen(wxCommandEvent& WXUNUSED(event)) {
	wxFileDialog dlg(this, "Open Configuration", "", "", "SwisTrack Configurations (*.swistrack)|*.swistrack", wxFD_OPEN);
	if (dlg.ShowModal() != wxID_OK) {return;}

	OpenFile(dlg.GetPath(), true, false);
}

void THISCLASS::OpenFile(const wxString &filename, bool breakonerror, bool astemplate) {
	// Check if file exists and is readable
	if (breakonerror) {
		wxFileName fn(filename);
		if (! fn.IsFileReadable()) {
			wxMessageDialog dlg(this, "Unable to read \n\n"+filename, "Open Configuration", wxOK);
			dlg.ShowModal();
			return;
		}
	}

	// Open the file
	ConfigurationReaderXML cr;
	if (! cr.Open(filename.c_str())) {
		if (breakonerror) {
			wxMessageDialog dlg(this, "The file \n\n"+filename+" \n\ncould not be loaded. Syntax error?", "Open Configuration", wxOK);
			dlg.ShowModal();
			return;
		}
	}

	// At this point, we consider the SwisTrack configuration to be valid. The next few lines close the current configuration and read the configuration.

	// Close the current configuration
	if (mChanged) {
		// TODO if necessary, ask the user whether he'd like to save the changes
		// return false;
	}

	// Split the filename
	wxString path;
	wxString name;
	wxString extension;
	wxFileName::SplitPath(mFileName, &path, &name, &extension);

	// Change the CWD
	wxFileName::SetCwd(path);

	// Set the new file name
	if (astemplate) {
		mFileName="";
		SetTitle("SwisTrack");
	} else {
		mFileName=filename;
		SetTitle(mFileName+" - SwisTrack");
	}

	// Read the components
	StopProductiveMode();
	cr.ReadComponents(mSwisTrackCore);

	// Read the trigger settings
	cr.SelectRootNode();
	cr.SelectChildNode("trigger");
	wxString type=cr.ReadString("mode", "manual");
	cr.SelectChildNode("freerun");
	mTriggerFreeRunInterval=cr.ReadInt("interval", 1000);
	type.MakeLower();
	if (type=="freerun") {
		SetTriggerFreeRun();
	} else {
		SetTriggerManual();
	}

	// Read the server settings
	cr.SelectRootNode();
	cr.SelectChildNode("server");
	mTCPServer->SetPort(cr.ReadInt("port", 3000));
	SetStatusText(wxString::Format("%d", mTCPServer->GetPort()), sStatusField_ServerPort);

	// Show errors if there are any
	if (cr.mErrorList.mList.empty()) {return;}
	ErrorListDialog eld(this, &(cr.mErrorList), "Open File", wxString::Format("The following errors occurred while reading the file '%s':", filename));
	eld.ShowModal();
}

void THISCLASS::OnFileSaveAs(wxCommandEvent& WXUNUSED(event)) {
	wxFileDialog dlg(this, "Save a configuration", "", "", "Configurations (*.swistrack)|*.swistrack", wxSAVE, wxDefaultPosition);
	if (dlg.ShowModal() != wxID_OK) {return;}
	
	mFileName=dlg.GetPath();
	SaveFile(mFileName);
}

void THISCLASS::OnFileSave(wxCommandEvent& event) {
	if (mFileName=="") {
		OnFileSaveAs(event);
		return;
	}

	SaveFile(mFileName);
}

void THISCLASS::SaveFile(const wxString &filename) {
	// Check if can write to that file
	wxFileName fn(filename);
	if (! fn.IsFileWritable()) {
		wxMessageDialog dlg(this, "Unable to write \n\n"+filename, "Save File", wxOK);
		dlg.ShowModal();
		return;
	}

	// Save the file
	ConfigurationWriterXML cw;
	cw.WriteComponents(mSwisTrackCore);

	// Save the trigger settings
	cw.SelectRootNode();
	cw.SelectChildNode("trigger");
	if (mTriggerFreeRunTimer->IsRunning()) {
		cw.WriteString("mode", "freerun");
	} else {
		cw.WriteString("mode", "manual");
	}
	cw.SelectChildNode("freerun");
	cw.WriteInt("interval", mTriggerFreeRunInterval);

	// Save other settings
	cw.SelectRootNode();
	cw.SelectChildNode("server");
	cw.WriteInt("port", mTCPServer->GetPort());

	if (! cw.Save(filename.c_str())) {
		wxMessageDialog dlg(this, "There was an error writing to \n\n"+filename+"\n\nThe file may be incomplete.", "Save File", wxOK);
		dlg.ShowModal();
		return;
	}

	SetStatusText(filename+" saved!", sStatusField_Messages);
}

void SwisTrack::OnFileQuit(wxCommandEvent& WXUNUSED(event)) {
	Close(TRUE);
	Destroy();
}

void THISCLASS::OnControlProductiveMode(wxCommandEvent& WXUNUSED(event)) {
	if (mSwisTrackCore->IsStartedInProductiveMode()) {
		StopProductiveMode();
	} else {
		StartProductiveMode();
	}
}

void THISCLASS::OnControlFreeRun(wxCommandEvent& WXUNUSED(event)) {
	if (mTriggerFreeRunTimer->IsRunning()) {
		SetTriggerManual();
	} else {
		SetTriggerFreeRun();
	}
}

void THISCLASS::OnControlSingleStep(wxCommandEvent& WXUNUSED(event)) {
	SingleStep();
}

void THISCLASS::OnChangeDisplaySpeed(wxScrollEvent& WXUNUSED(event)) {
	GetToolBar()->TransferDataFromWindow();
	// TODO
}

void SwisTrack::OnHelpAbout(wxCommandEvent& WXUNUSED(event)) {
	AboutDialog *dlg = new AboutDialog(this);
	dlg->ShowModal();
}

void THISCLASS::OnHelp(wxCommandEvent& WXUNUSED(event)) {
	wxLaunchDefaultBrowser("http://en.wikibooks.org/wiki/Swistrack");
}

void THISCLASS::OnTest(wxCommandEvent& WXUNUSED(event)) {
	//mPanelInformation->Hide();
	//mPanelInformation1->Show();
	//GetSizer()->Layout();
}

void THISCLASS::OnIdle(wxIdleEvent& WXUNUSED(event)) {
}

void SwisTrack::OnToolsServer(wxCommandEvent& WXUNUSED(event)) {
	
}
