/*! \mainpage My Personal Index Page
\section intro_sec Introduction

\section prog_sec Getting started
You will need to install the following libraries 

- libxml2
- libxml++ (http://sourceforge.net/projects/libxmlplusplus)
- OpenCV (http://sourceforge.net/projects/opencvlibrary)
- WxWidgets (http://www.wxwidgets.org)

Windows only:

- CMU 1394 Firewire Driver

SwisTrack is divided in three projects Objecttracker, GUI, and UI. Objecttracker 
contains all methods for acquiring and processing images, whereas GUI and UI are a graphical and
a command line interface, respectively.

Hence, you will first need to build objecttracker.lib and then link it into either the GUI or
the UI project.

\subsection visual_studio Visual Studio 2005 Settings

Make sure you set the working directories for release and debug compilation to 'release', otherwise SwisTrack won't find
its configuration files 'swistrack.exp' and 'default.cfg' (Alt-F7, Configuration Properties, Debugging, Working directory).


\subsection samples Running the examples

When running the samples, make sure you have a recent version of DivX installed. If you have problems, try to open 
the video files with Virtualdub first. SwisTrack (and VirtualDub) use so called VfW drivers and not DirectMedia drivers,
usually installing a codec pack like the KazaaLite codec pack solves all of this problems.

If you are working with Firewire cameras make sure you replace the custom driver that comes with the camera with the CMU1394
driver.
*/

#include "SwisTrack.h"
#define THISCLASS SwisTrack

#include "CanvasPanel.h"
#include "TCPServer.h"
#include "ErrorListDialog.h"
#include "SwisTrackCoreEditor.h"
#include "ConfigurationReaderXML.h"
#include "ConfigurationWriterXML.h"

#include <algorithm>
#include <cctype>
#include <math.h>
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

/**
\todo

- does not start if tracks cannot be found
- threshold over 255 should be not allowed
- generic error handling routine needs to be implemented in the idleevent
- make sure video frame rate is stored correctly (not 30Hz if the camera was only 15), toolbar needs to be updated similarly

*/

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
		mSwisTrackCore(0), mTCPServer(0), mFileName(""), mTriggerFreeRunInterval(1000) {

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
	mComponentListPanel=new ComponentListPanel(this, mSwisTrackCore);

	// Panel
	mPanelInformation=new wxPanel(this, -1, wxDefaultPosition, wxSize(200, 20));
	mPanelInformation->SetBackgroundColour(*wxBLUE);
	mPanelInformation1=new wxPanel(this, -1, wxDefaultPosition, wxSize(200, 20));
	mPanelInformation1->SetBackgroundColour(*wxRED);
	mPanelInformation1->Hide();

	// Setup frame contents
	wxBoxSizer *hs=new wxBoxSizer(wxHORIZONTAL);
	hs->Add(mCanvasPanel, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 10);
	hs->Add(mPanelInformation, 0, wxEXPAND, 0);
	hs->Add(mPanelInformation1, 0, wxEXPAND, 0);

	wxBoxSizer *vs=new wxBoxSizer(wxVERTICAL);
	vs->Add(hs, 3, wxEXPAND, 0);
	vs->Add(mComponentListPanel, 1, wxEXPAND, 0);
	SetSizer(vs);

	// The timer used for the free run mode
	mTriggerFreeRunTimer=new wxTimer(this);
}

/*! \brief Destructor

Deallocates all memory and closes the application.
*/
SwisTrack::~SwisTrack(){
	SetTriggerManual();
	StopProductiveMode();

	delete mTCPServer;
	delete mSwisTrackCore;
	delete mTriggerFreeRunTimer;

#ifdef MULTITHREAD
	if (mCriticalSection) {delete mCriticalSection;}
#endif
}

void THISCLASS::BuildMenuBar() {
	// Create a menu bar
	SetMenuBar(new wxMenuBar());

	// Create menus
	mMenuFile = new wxMenu;
	GetMenuBar()->Append(mMenuFile, _T("&File"));

	mMenuTools = new wxMenu;
	GetMenuBar()->Append(mMenuTools,_T("&Tools"));

	mMenuHelp = new wxMenu;
	GetMenuBar()->Append(mMenuHelp, _T("&Help"));

	mMenuFile->Append(sID_New, _T("&New\tCtrl-C"), _T("Creates a new file"));
	mMenuFile->Append(sID_Open, _T("&Open\tCtrl-O"), _T("Opens a file"));
	mMenuFile->Append(sID_Save, _T("&Save\tCtrl-S"), _T("Saves the current file"));
	mMenuFile->Append(sID_SaveAs, _T("&Save as ..."), _T("Saves the file with another name"));
	mMenuFile->AppendSeparator();
	mMenuFile->Append(sID_Quit, _T("E&xit\tAlt-F4"), _T("Quit this program"));
	mMenuFile->Enable(sID_Save,FALSE);

	mMenuTools->Append(sID_Tools_Server, _T("TCP Server ..."), _T("TCP server settings"));

	mMenuHelp->Append(sID_Help, _T("&Manual"), _T("Opens the manual"));
	mMenuHelp->Append(sID_Test, _T("&Test"), _T("Test"));
	mMenuHelp->Append(sID_About, _T("&About ...\tF1"), _T("Show about dialog"));
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
	OpenFile("default.swistrack", false, true);
}

void THISCLASS::OnFileOpen(wxCommandEvent& WXUNUSED(event)) {
	wxFileDialog dlg(this, "Open Configuration", "", "", "SwisTrack Configurations (*.swistrack)|*.swistrack", wxOPEN, wxDefaultPosition);
	if (dlg.ShowModal() != wxID_OK) {return;}

	OpenFile(dlg.GetPath(), true, false);
}

void THISCLASS::OpenFile(const wxString &filename, bool breakonerror, bool astemplate) {
	// Check if file exists and is readable
	if (breakonerror) {
		//wxFileName fn(filename);
		//if (! fn.IsFileReadable()) {  // TODO: update to wxWidgets 2.8.0 and active this
		//	wxMessageDialog dlg(this, "Unable to read \n\n"+filename, "Open Configuration", wxOK);
		//	dlg.ShowModal();
		//	return;
		//}
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

	// Set the new file name
	if (astemplate) {
		mFileName="";
		SetTitle("SwisTrack");
	} else {
		mFileName=filename;
		SetTitle(mFileName+" - SwisTrack");
	}

	// Read the configuration
	StopProductiveMode();
	cr.ReadComponents(mSwisTrackCore);
	mTriggerFreeRunInterval=cr.ReadInt("trigger/freerun/interval", 1000);
	std::string type=cr.ReadString("trigger/mode", "manual");
	std::transform(type.begin(), type.end(), type.begin(), std::tolower);
	if (type=="freerun") {
		SetTriggerFreeRun();
	} else {
		SetTriggerManual();
	}

	// Read other settings
	mTCPServer->SetPort(cr.ReadInt("server/port", 3000));
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
	//wxFileName fn(filename);
	//if (! fn.IsFileWritable()) {  // TODO: update to 2.8.0 and active this
	//	wxMessageDialog dlg(this, "Unable to write \n\n"+filename, "Save File", wxOK).ShowModal();
	//	return;
	//}

	// Save the file
	ConfigurationWriterXML cw;
	cw.WriteComponents(mSwisTrackCore);

	// Save the trigger settings
	cw.SelectRootNode();
	cw.SelectNode("trigger");
	if (mTriggerFreeRunTimer->IsRunning()) {
		cw.WriteString("mode", "freerun");
	} else {
		cw.WriteString("mode", "manual");
	}
	cw.SelectNode("freerun");
	cw.WriteInt("interval", mTriggerFreeRunInterval);

	// Save other settings
	cw.SelectRootNode();
	cw.SelectNode("server");
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
	mPanelInformation->Hide();
	mPanelInformation1->Show();
	GetSizer()->Layout();
}

void THISCLASS::OnIdle(wxIdleEvent& WXUNUSED(event)) {
}

void SwisTrack::OnToolsServer(wxCommandEvent& WXUNUSED(event)) {
	
}
