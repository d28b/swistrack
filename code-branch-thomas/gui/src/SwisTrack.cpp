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
#include <wx/textdlg.h>
#include <wx/minifram.h>

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
	EVT_MENU(sID_Control_Run, THISCLASS::OnControlRun)
	EVT_MENU(sID_Control_Step, THISCLASS::OnControlStep)
	EVT_MENU(sID_Control_Reset, THISCLASS::OnControlReset)
	EVT_MENU(sID_View_ComponentList, THISCLASS::OnViewComponentList)
	EVT_MENU(sID_View_NewDisplay, THISCLASS::OnViewNewDisplay)
	EVT_MENU(sID_Tools_TCPServer, THISCLASS::OnToolsTCPServer)
	EVT_MENU(sID_Help, THISCLASS::OnHelp)
	EVT_MENU(sID_Test, THISCLASS::OnTest)
	EVT_MENU(sID_About, THISCLASS::OnHelpAbout)
	EVT_IDLE(THISCLASS::OnIdle)
END_EVENT_TABLE()

SwisTrack::SwisTrack(const wxString& title, const wxPoint& pos, const wxSize& size, long style):
		wxFrame(NULL, -1, title, pos, size, style),
		CommunicationCommandHandler(),
		mSwisTrackCore(0), mTCPServer(0), mFileName(""),
		mHorizontalSizer(0), mCanvasPanel(0), mComponentListPanel(0), mConfigurationPanel(0) {

#ifdef MULTITHREAD
	criticalSection = new wxCriticalSection();
#endif

	// General initialization
	SetIcon(wxICON(icon_gui));
	BuildMenuBar();
	BuildToolBar();
	BuildStatusBar();

	// The canvas panel
	mCanvasPanel=new CanvasPanel(this, this);

	// SwisTrackCore
	wxFileName filename(wxGetApp().mApplicationFolder, "", "");
	filename.AppendDir("Components");
	mSwisTrackCore=new SwisTrackCore(filename.GetFullPath().c_str());

	// TCP server
	mTCPServer=new TCPServer(this);
	mSwisTrackCore->mCommunicationInterface=mTCPServer;	
	mTCPServer->AddCommandHandler(this);

	// List
	mComponentListPanel=new ComponentListPanel(this, this);

	// Configuration panel
	mConfigurationPanel=new ConfigurationPanel(this, this, 0);

	// Timeline panel
	//mTimelinePanel=new TimelinePanel(this, this);

	// Setup frame contents
	mHorizontalSizer=new wxBoxSizer(wxHORIZONTAL);
	mHorizontalSizer->Add(mCanvasPanel, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 10);
	mHorizontalSizer->Add(mConfigurationPanel, 0, wxEXPAND, 0);

	wxBoxSizer *vs=new wxBoxSizer(wxVERTICAL);
	vs->Add(mHorizontalSizer, 3, wxEXPAND, 0);
	vs->Add(mComponentListPanel, 1, wxEXPAND, 0);
	SetSizer(vs);
}

SwisTrack::~SwisTrack(){
	Control_StopRunMode();
	Control_StopProductiveMode();

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
	wxMenu *menufile=new wxMenu;
	GetMenuBar()->Append(menufile, _T("&File"));
	menufile->Append(sID_New, _T("&New\tCtrl-C"), _T("Creates a new file"));
	menufile->Append(sID_Open, _T("&Open\tCtrl-O"), _T("Opens a file"));
	menufile->Append(sID_Save, _T("&Save\tCtrl-S"), _T("Saves the current file"));
	menufile->Append(sID_SaveAs, _T("&Save as ..."), _T("Saves the file with another name"));
	menufile->AppendSeparator();
	menufile->Append(sID_Quit, _T("E&xit\tAlt-F4"), _T("Quit this program"));
	menufile->Enable(sID_Save,FALSE);

	wxMenu *menuview=new wxMenu;
	GetMenuBar()->Append(menuview,_T("&View"));
	menuview->Append(sID_View_ComponentList, _T("Component list"), _T("Displays or hides the component list"), wxITEM_CHECK);
	//menuview->AppendSeparator();
	menuview->Append(sID_View_NewDisplay, _T("New display"), _T("Opens a display in a new window"));
	menuview->Check(sID_View_ComponentList, true);

	wxMenu *menuoutput=new wxMenu;
	GetMenuBar()->Append(menuoutput,_T("&Output"));
	menuoutput->Append(sID_Tools_TCPServer, _T("TCP Server ..."), _T("TCP server settings"));

	wxMenu *menuhelp=new wxMenu;
	GetMenuBar()->Append(menuhelp, _T("&Help"));
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
	toolbar->AddTool(sID_New, _T("New"), wxBITMAP(bitmap_new), _T("New"));
	toolbar->AddTool(sID_Open, _T("Open"), wxBITMAP(bitmap_open), _T("Open"));
	toolbar->AddTool(sID_Save, _T("Save"), wxBITMAP(bitmap_save), _T("Save"));
	toolbar->AddSeparator();
	toolbar->AddTool(sID_Control_ProductiveMode, _T("Productive"), wxBITMAP(bitmap_productive), _T("Run in productive mode"), wxITEM_CHECK);
	toolbar->AddTool(sID_Control_Run, _T("Run"), wxBITMAP(bitmap_play), _T("Run automatically"), wxITEM_CHECK);
	toolbar->AddSeparator();
	toolbar->AddTool(sID_Control_Step, _T("Step"), wxBITMAP(bitmap_singlestep), _T("Processes one image"));
	toolbar->AddTool(sID_Control_Reset, _T("Reset"), wxBITMAP(bitmap_singlestep), _T("Stops the execution. It will be started upon the next step."));
	toolbar->AddSeparator();

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

	// Destroy the old panel
	mConfigurationPanel->Destroy();

	// Create a new panel
	mConfigurationPanel=new ConfigurationPanel(this, this, c);
	mHorizontalSizer->Add(mConfigurationPanel, 0, wxEXPAND, 0);
	GetSizer()->Layout();

	// Switch to corresponding display
	if (mConfigurationPanel->mDisplayName!="") {
		Display *di=c->GetDisplayByName(mConfigurationPanel->mDisplayName.c_str());
		mCanvasPanel->SetDisplay(di);
	}

	// Show errors
	if (mConfigurationPanel->mErrorList.mList.empty()) {return;}
	if (mSwisTrackCore->IsStartedInProductiveMode()) {return;}
	ErrorListDialog eld(this, &(mConfigurationPanel->mErrorList), "Component configuration", "The following errors occurred while reading the component configuration description:");
	eld.ShowModal();
}

void THISCLASS::Control_Step() {
	// Start (if necessary) and perform a step, unless the automatic trigger is active
	if (mSwisTrackCore->IsTriggerActive()) {return;}
	mSwisTrackCore->Start(false);
	mSwisTrackCore->Step();
}

void THISCLASS::Control_ReloadConfiguration() {
	// In productive mode, we just call Control_ReloadConfiguration which may not update all configuration parameters.
	// Otherwise, we stop the simulation (it is automatically restarted upon the next step).
	if (mSwisTrackCore->IsStartedInProductiveMode()) {
		mSwisTrackCore->ReloadConfiguration();
	} else {
		mSwisTrackCore->Stop();
		mSwisTrackCore->Start(false);
	}
}

void THISCLASS::Control_StartProductiveMode() {
	// Stop and start in productive mode, unless we are in productive mode already
	if (mSwisTrackCore->IsStartedInProductiveMode()) {return;}
	mSwisTrackCore->Stop();
	GetToolBar()->ToggleTool(sID_Control_ProductiveMode, true);
	GetToolBar()->EnableTool(sID_Control_Reset, false);
	mSwisTrackCore->Start(true);
}

void THISCLASS::Control_StopProductiveMode() {
	// Stop productive mode (if we are running in productive mode) and start in normal mode
	if (! mSwisTrackCore->IsStartedInProductiveMode()) {return;}
	mSwisTrackCore->Stop();
	GetToolBar()->ToggleTool(sID_Control_ProductiveMode, false);
	GetToolBar()->EnableTool(sID_Control_Reset, true);
	mSwisTrackCore->Start(false);
}

void THISCLASS::Control_StartRunMode() {
	// Activate the automatic trigger
	GetToolBar()->ToggleTool(sID_Control_Run, true);
	GetToolBar()->EnableTool(sID_Control_Step, false);
	mSwisTrackCore->StartTrigger();
	mSwisTrackCore->Start(false);
}

void THISCLASS::Control_StopRunMode() {
	// Deactivate the automatic trigger
	GetToolBar()->ToggleTool(sID_Control_Run, false);
	GetToolBar()->EnableTool(sID_Control_Step, true);
	mSwisTrackCore->StopTrigger();
}

bool THISCLASS::OnCommunicationCommand(CommunicationMessage *m) {
	if (m->mCommand=="START") {
		// DEPRECATED: use RUN true
		Control_StartProductiveMode();
		return true;
	} else if (m->mCommand=="STOP") {
		// DEPRECATED: use RUN false
		Control_StopProductiveMode();
		return true;
	} else if (m->mCommand=="STEP") {
		Control_Step();
		return true;
	} else if (m->mCommand=="RELOADCONFIGURATION") {
		Control_ReloadConfiguration();
		return true;
	} else if (m->mCommand=="RUN") {
		bool state=m->GetBool(false);
		if (state) {
			Control_StartRunMode();
		} else {
			Control_StopRunMode();
		}
		return true;
	} else if (m->mCommand=="PRODUCTIVE") {
		bool state=m->GetBool(false);
		if (state) {
			Control_StartProductiveMode();
		} else {
			Control_StopProductiveMode();
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
	Control_StopProductiveMode();
	Control_StopRunMode();
	if (mChanged) {
		// TODO if necessary, ask the user whether he'd like to save the changes
		// return false;
	}

	// Set the new file name
	SetFileName(filename);

	// Read the components
	cr.ReadComponents(mSwisTrackCore);

	// Read the server settings
	cr.SelectRootNode();
	cr.SelectChildNode("server");
	mTCPServer->SetPort(cr.ReadInt("port", 3000));
	SetStatusText(wxString::Format("%d", mTCPServer->GetPort()), sStatusField_ServerPort);

	// Set the status text
	SetStatusText(filename+" opened!", sStatusField_Messages);

	// Show errors if there are any
	if (cr.mErrorList.mList.empty()) {return;}
	ErrorListDialog eld(this, &(cr.mErrorList), "Open File", wxString::Format("The following errors occurred while reading the file '%s':", filename));
	eld.ShowModal();
}

void THISCLASS::SetFileName(const wxString &filename) {
	mFileName=filename;
	if (filename=="") {
		// Set the frame title
		SetTitle("SwisTrack");

		// Change the CWD to the application folder
		wxFileName::SetCwd(wxGetApp().mApplicationFolder);
	} else {
		// Split the filename
		wxString path;
		wxString name;
		wxString extension;
		wxFileName::SplitPath(mFileName, &path, &name, &extension);
		
		// Set the frame title
		SetTitle(name+" - SwisTrack");

		// Change the CWD
		wxFileName::SetCwd(path);
	}
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
	/*wxFileName fn(filename);
	if (! fn.IsFileWritable()) {
		wxMessageDialog dlg(this, "Unable to write \n\n"+filename, "Save File", wxOK);
		dlg.ShowModal();
		return;
	}*/

	// Save the file
	ConfigurationWriterXML cw;
	cw.WriteComponents(mSwisTrackCore);

	// Save other settings
	cw.SelectRootNode();
	cw.SelectChildNode("server");
	cw.WriteInt("port", mTCPServer->GetPort());

	if (! cw.Save(filename.c_str())) {
		wxMessageDialog dlg(this, "There was an error writing to \n\n"+filename+"\n\nThe file may be incomplete.", "Save File", wxOK);
		dlg.ShowModal();
		return;
	}

	// Set the (new) filename
	SetFileName(filename);

	// Set the status text
	SetStatusText(filename+" saved!", sStatusField_Messages);
}

void SwisTrack::OnFileQuit(wxCommandEvent& WXUNUSED(event)) {
	Close(TRUE);
	Destroy();
}

void THISCLASS::OnControlProductiveMode(wxCommandEvent& WXUNUSED(event)) {
	if (mSwisTrackCore->IsStartedInProductiveMode()) {
		Control_StopProductiveMode();
	} else {
		Control_StartProductiveMode();
	}
}

void THISCLASS::OnControlRun(wxCommandEvent& WXUNUSED(event)) {
	if (mSwisTrackCore->IsTriggerActive()) {
		Control_StopRunMode();
	} else {
		Control_StartRunMode();
	}
}

void THISCLASS::OnControlStep(wxCommandEvent& WXUNUSED(event)) {
	Control_Step();
}

void THISCLASS::OnControlReset(wxCommandEvent& WXUNUSED(event)) {
	Control_ReloadConfiguration();
}

void SwisTrack::OnHelpAbout(wxCommandEvent& WXUNUSED(event)) {
	AboutDialog *dlg = new AboutDialog(this);
	dlg->ShowModal();
}

void THISCLASS::OnHelp(wxCommandEvent& WXUNUSED(event)) {
	wxLaunchDefaultBrowser("http://en.wikibooks.org/wiki/Swistrack");
}

void THISCLASS::OnTest(wxCommandEvent& WXUNUSED(event)) {
}

void THISCLASS::OnIdle(wxIdleEvent& event) {
	mSwisTrackCore->OnIdle();
}

void THISCLASS::OnToolsTCPServer(wxCommandEvent& WXUNUSED(event)) {
	wxTextEntryDialog dlg(this, "TCP Port:", "TCP Server", "");
	long port=(long)(mTCPServer->GetPort());
	dlg.SetValue(wxString::Format("%d", port));
	if (dlg.ShowModal() != wxID_OK) {return;}

	wxString str=dlg.GetValue();
	str.ToLong(&port);
	mTCPServer->SetPort((int)port);
}

void THISCLASS::OnViewComponentList(wxCommandEvent& event) {
	if (event.IsChecked()) {
		mComponentListPanel->Show();
		mConfigurationPanel->Show();
	} else {
		mComponentListPanel->Hide();
		mConfigurationPanel->Hide();	
	}
	GetSizer()->Layout();
}

void THISCLASS::OnViewNewDisplay(wxCommandEvent& WXUNUSED(event)) {
	wxMiniFrame *frame=new wxMiniFrame(this, -1,_("Display"), wxDefaultPosition, wxDefaultSize, wxCAPTION | wxRESIZE_BORDER | wxCLOSE_BOX | wxSYSTEM_MENU);
	new CanvasPanel(frame, this);
	frame->Show();
}
