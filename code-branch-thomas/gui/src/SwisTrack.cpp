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
#include "SocketServer.h"
#include "ErrorListDialog.h"

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
	EVT_MENU(sID_Control_SeriousMode, THISCLASS::OnControlSeriousMode)
	EVT_MENU(sID_Control_FreeRun, THISCLASS::OnControlFreeRun)
	EVT_MENU(sID_Control_SingleStep, THISCLASS::OnControlSingleStep)
	EVT_MENU(sID_Tools_Screenshot, SwisTrack::OnMakeScreenShot)
	EVT_COMMAND_SCROLL(sID_DisplaySpeed, SwisTrack::OnChangeDisplaySpeed)
	EVT_MENU(sID_Help, SwisTrack::OnHelp)
	EVT_MENU(sID_Test, SwisTrack::OnTest)
	EVT_MENU(sID_About, THISCLASS::OnHelpAbout)
	EVT_TIMER(wxID_ANY, THISCLASS::OnFreeRunTimer)
	EVT_IDLE(SwisTrack::OnIdle)
END_EVENT_TABLE()

SwisTrack::SwisTrack(const wxString& title, const wxPoint& pos, const wxSize& size, long style):
		wxFrame(NULL, -1, title, pos, size, style),
		CommunicationCommandHandler(),
		mSwisTrackCore(0), mSocketServer(0), mFileName(""), mFreeRunInterval(1000) {

#ifdef MULTITHREAD
	criticalSection = new wxCriticalSection();
#endif

#if wxUSE_STATUSBAR
	CreateStatusBar(2);
	SetStatusText(_T("Welcome to SwisTrack!"));
#endif // wxUSE_STATUSBAR

	// General initialization
	SetIcon(wxICON(gui));
	BuildMenuBar();
	BuildToolBar();

	// The canvas panel
	mCanvasPanel=new CanvasPanel(this);

	// SwisTrackCore and SocketServer
	mSocketServer = new SocketServer(this);
	mSwisTrackCore=new SwisTrackCore();
	mSwisTrackCore->mCommunicationInterface=mSocketServer;	
	mSocketServer->SetPort(3001);

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
}

/*! \brief Destructor

Deallocates all memory and closes the application.
*/
SwisTrack::~SwisTrack(){
	SetTriggerManual();
	StopSeriousMode();

	if (mCanvasPanel) {delete mCanvasPanel;}
	if (mPanelInformation) {delete mPanelInformation;}
	if (mComponentListPanel) {delete mComponentListPanel;}
	if (mSocketServer) {delete mSocketServer;}
	if (mSwisTrackCore) {delete mSwisTrackCore;}

#ifdef MULTITHREAD
	if (mCriticalSection) {delete mCriticalSection;}
#endif

	Close(TRUE);
}

void THISCLASS::BuildMenuBar() {
	// Create a menu bar
	menuBar = new wxMenuBar();
	SetMenuBar(menuBar);

	// Create menus
	menuFile    = new wxMenu;
	menuBar->Append(menuFile, _T("&File"));

	menuView    = new wxMenu;
	menuBar->Append(menuView,_T("&View"));

	menuTools   = new wxMenu;
	menuBar->Append(menuTools,_T("&Tools"));

	menuHelp    = new wxMenu;
	menuBar->Append(menuHelp, _T("&Help"));

	menuFile->Append(sID_New, _T("&New\tCtrl-C"), _T("Creates a new file"));
	menuFile->Append(sID_Open, _T("&Open\tCtrl-O"), _T("Opens a file"));
	menuFile->Append(sID_Save, _T("&Save\tCtrl-S"), _T("Saves the current file"));
	menuFile->Append(sID_SaveAs, _T("&Save as ..."), _T("Saves the file with another name"));
	menuFile->AppendSeparator();
	menuFile->Append(sID_Quit, _T("E&xit\tAlt-F4"), _T("Quit this program"));
	menuFile->Enable(sID_Save,FALSE);

	menuView->Append(sID_View_ShowTracker, _T("Show Tracker Panel"), _T("Toggles window where tracking parameters can be changed"),TRUE);
	menuView->Check (sID_View_ShowTracker, FALSE);
	menuView->Append(sID_View_ShowParticleFilter, _T("Show Particle Filter Panel"), _T("Toggles window where particle filters parameters can be changed"),TRUE);
	menuView->Check (sID_View_ShowParticleFilter, FALSE);
	menuView->Append(sID_View_ShowSegmenterPP, _T("Show Segmenter Post-Processing Panel"), _T("Toggles window where parameters for segmentation post-processing can be changed"),TRUE);
	menuView->Check (sID_View_ShowSegmenterPP, FALSE);
	menuView->Append(sID_View_ShowSegmenter, _T("Show Segmenter Panel"), _T("Toggles window where segmenter parameters can be changed"),TRUE);
	menuView->Check(sID_View_ShowSegmenter ,FALSE);
	menuView->Append(sID_View_ShowInput, _T("Show Input Panel"), _T("Toggles window where input parameters can be changed"),TRUE);
	menuView->Check(sID_View_ShowInput, FALSE);
	menuView->AppendSeparator();
	menuView->AppendRadioItem(sID_View_TrajCross, _T("cross only"));
	menuView->AppendRadioItem(sID_View_TrajNoID, _T("trace and cross"));
	menuView->AppendRadioItem(sID_View_TrajNoCross, _T("trace and ID"));
	menuView->AppendRadioItem(sID_View_TrajFull, _T("trace, cross, and ID"));
	menuView->AppendRadioItem(sID_View_Coverage, _T("coverage"));
	menuView->AppendSeparator();
	menuView->Append(sID_View_ShowMask, _T("Show Mask"), _T("Displays the contours given by the mask image"),TRUE);
	menuView->Enable(sID_View_ShowMask, FALSE);
	menuView->Check(sID_View_ShowMask, FALSE);

	menuTools->Append(sID_Tools_ShowCamera, _T("Display camera"), _T("Displays data from a IEEE1394 firewire camera"));
	menuTools->AppendSeparator();
	menuTools->Append(sID_Tools_Screenshot, _T("Save Bitmap"), _T("Saves the current view to a file"));

#ifdef _1394
	if(theCamera.CheckLink() != CAM_SUCCESS)    
		menuTools->Enable(sID_Tools_ShowCamera, FALSE);
	else
		menuTools->Enable(sID_Tools_ShowCamera, TRUE);
#endif

	menuHelp->Append(sID_Help, _T("&Manual"), _T("Opens the manual"));
	menuHelp->Append(sID_Test, _T("&Test"), _T("Test"));
	menuHelp->Append(sID_About, _T("&About...\tF1"), _T("Show about dialog"));

	menuBar->Check(sID_View_TrajFull, TRUE);
	menuBar->Check(sID_View_ShowMask, FALSE);
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
	toolbar->AddTool(sID_Control_SeriousMode, _T("Serious"), wxBITMAP(play), _T("Serious"), wxITEM_CHECK);
	toolbar->AddSeparator();
	toolbar->AddTool(sID_Control_FreeRun, _T("Free-run"), wxBITMAP(play), _T("Free-run mode"), wxITEM_CHECK);
	toolbar->AddTool(sID_Control_SingleStep, _T("Step"), wxBITMAP(singlestep), _T("One step"));
	toolbar->AddSeparator();
	toolbar->AddTool(sID_Mode_Intercept, _T("Intercept"), wxBITMAP(finger), _T("Override data association"));
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

void THISCLASS::SingleStep() {
	// Start (if necessary) and perform a step
	mSwisTrackCore->Start(false);
	mSwisTrackCore->Step();
	mComponentListPanel->OnUpdate();
}

void THISCLASS::ReloadConfiguration() {
	// In serious mode, we just call ReloadConfiguration which may not update all configuration parameters.
	// Otherwise, we stop the simulation (it is automatically restarted upon the next step).
	if (mSwisTrackCore->IsStartedInSeriousMode()) {
		mSwisTrackCore->ReloadConfiguration();
		mComponentListPanel->OnUpdate();
	} else {
		mSwisTrackCore->Stop();
		mComponentListPanel->OnUpdate();
	}
}

void THISCLASS::StartSeriousMode() {
	if (mSwisTrackCore->IsStartedInSeriousMode()) {return;}
	mSwisTrackCore->Stop();
	GetToolBar()->ToggleTool(sID_Control_SeriousMode, true);
	mSwisTrackCore->Start(true);
	mComponentListPanel->OnUpdate();
}

void THISCLASS::StopSeriousMode() {
	if (! mSwisTrackCore->IsStartedInSeriousMode()) {return;}
	mSwisTrackCore->Stop();
	GetToolBar()->ToggleTool(sID_Control_SeriousMode, false);
	mComponentListPanel->OnUpdate();
}

void THISCLASS::SetTriggerManual() {
	GetToolBar()->ToggleTool(sID_Control_FreeRun, false);
	mFreeRunTimer.Stop();
}

void THISCLASS::SetTriggerTimer(int ms) {
	GetToolBar()->ToggleTool(sID_Control_FreeRun, true);
	mFreeRunInterval=ms;
	if (! mFreeRunTimer.IsRunning()) {mFreeRunTimer.Start(mFreeRunInterval);}
}

void THISCLASS::OnFreeRunTimer(wxTimerEvent& WXUNUSED(event)) {
	SingleStep();
	if (mFreeRunTimer.GetInterval()!=mFreeRunInterval) {
		mFreeRunTimer.Stop();
		mFreeRunTimer.Start(mFreeRunInterval);
	}
}

bool THISCLASS::OnCommunicationCommand(CommunicationMessage *m) {
	if (m->mCommand=="START") {
		StartSeriousMode();
		return true;
	} else if (m->mCommand=="STOP") {
		StopSeriousMode();
		return true;
	} else if (m->mCommand=="STEP") {
		SingleStep();
		return true;
	} else if (m->mCommand=="RELOADCONFIGURATION") {
		ReloadConfiguration();
		return true;
	} else if (m->mCommand=="TRIGGER") {
		std::string type=m->GetString("MANUAL");
		if (type=="TIMER") {
			double interval=m->GetDouble(1);
			SetTriggerTimer(interval);
		} else {
			SetTriggerManual();
		}
		return true;
	}

	return false;
}

void SwisTrack::OnFileNew(wxCommandEvent& WXUNUSED(event)) {
	StopSeriousMode();
	OpenFile("default.swistrack", false, true);
}

void THISCLASS::OnFileOpen(wxCommandEvent& WXUNUSED(event)) {
	wxFileDialog dlg(this, "Open Configuration", "", "", "SwisTrack Configurations (*.swistrack)|*.swistrack", wxOPEN, wxDefaultPosition);
	if (dlg.ShowModal() != wxID_OK) {return;}

	StopSeriousMode();
	OpenFile(dlg.GetPath(), true, false);
}

void THISCLASS::OpenFile(const wxString &filename, bool breakonerror, bool astemplate) {
	SwisTrackCoreEditor stce;
	if (! stce.IsEditable()) {return;}

	// Check if file is readable
	if (breakonerror) {
		//wxFileName fn(filename);
		//if (! fn.IsFileReadable()) {  // TODO: update to wxWidgets 2.8.0 and active this
		//	wxMessageDialog dlg(this, "Unable to read \n\n"+filename, "Open Configuration", wxOK);
		//	dlg.ShowModal();
		//	return;
		//}
	}

	// Read the new file
	xmlpp::DomParser parser;
	xmlpp::Document *document=0;
	try {
		parser.parse_file(filename.c_str());
		if (parser==true) {
			document=parser.get_document();
		}
	} catch (...) {
		document=0;
	}

	if ((breakonerror) && (document==0)) {
		wxMessageDialog dlg(this, "The file \n\n"+filename+" \n\ncould not be loaded. Syntax error?", "Open Configuration", wxOK);
		dlg.ShowModal();
		return;
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
	} else {
		mFileName=filename;
	}

	// Read the configuration
	ErrorList errorlist;
	ConfigurationReadXML(&stce, document, &errorlist);
	mComponentListPanel->OnUpdate();
	if (errorlist.mList.empty()) {return;}

	ErrorListDialog eld(this, &errorlist, "Open File", wxString::Format("The following errors occurred while reading the file '%s':", filename));
	eld.ShowModal();
}

void THISCLASS::ConfigurationReadXML(SwisTrackCoreEditor *stce, xmlpp::Document *document, ErrorList *errorlist) {
	// If this method is called without a document, set up an empty component list
	if (document==0) {
		stce->ConfigurationReadXML(0, errorlist);
		return;
	}

	// Get the root node
	xmlpp::Element *rootnode=document->get_root_node();
	if (! rootnode) {
		stce->ConfigurationReadXML(0, errorlist);
		return;
	}

	// Get the list of components
	xmlpp::Node::NodeList nodes_components=rootnode->get_children("Components");
	if (nodes_components.empty()) {
		stce->ConfigurationReadXML(0, errorlist);
		return;
	}

	// Read the component list
	xmlpp::Element *components=dynamic_cast<xmlpp::Element *>(nodes_components.front());
	stce->ConfigurationReadXML(components, errorlist);
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

	// Create an XML document
	xmlpp::Document document;
	xmlpp::Element *rootnode=document.create_root_node("swistrack");
	xmlpp::Element *components=rootnode->add_child("components");

	// Add all components to this document
	ErrorList errorlist;
	mSwisTrackCore->ConfigurationWriteXML(components, &errorlist);

	// Save
	try {
		document.write_to_file_formatted(filename.c_str());
	} catch (...) {
		wxMessageDialog dlg(this, "There was an error writing to \n\n"+filename+"\n\nThe file may be incomplete.", "Save File", wxOK);
		dlg.ShowModal();
		return;
	}

	SetStatusText(filename+" saved!", 1);
}

void SwisTrack::OnFileQuit(wxCommandEvent& WXUNUSED(event)) {
	Close(TRUE);
}

void THISCLASS::OnControlSeriousMode(wxCommandEvent& WXUNUSED(event)) {
	if (mSwisTrackCore->IsStartedInSeriousMode()) {
		StartSeriousMode();
	} else {
		StopSeriousMode();
	}
}

void THISCLASS::OnControlFreeRun(wxCommandEvent& WXUNUSED(event)) {
	if (mFreeRunTimer.IsRunning()) {
		SetTriggerManual();
	} else {
		SetTriggerTimer(mFreeRunInterval);
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

	double px[10];
	double py[10];
	double po[10];
	for (int i=0; i<10; i++) {
		px[i]=i*0.1;
		py[i]=i*0.05;
		po[i]=i*0.1;
	}

	for (int r=0; r<3000; r++) {
		CommunicationMessage mbegin("BEGINFRAME");
		mbegin.AddInt(r);
		mSocketServer->SendMessage(&mbegin);

		for (int i=0; i<10; i++) {
			if ((px[i]<=0.2) && (py[i]>=0)) {
				CommunicationMessage m("PARTICLE");
				m.AddInt(i);
				m.AddDouble(px[i]);
				m.AddDouble(py[i]);
				m.AddDouble(po[i]);
				mSocketServer->SendMessage(&m);
			}
		}

		CommunicationMessage mend("ENDFRAME");
		mSocketServer->SendMessage(&mend);

		for (int i=0; i<10; i++) {
			px[i]+=sin(po[i])*0.02;
			py[i]+=cos(po[i])*0.02;
			po[i]-=i*0.01;
		}

		Sleep(100);
	}
}

void THISCLASS::OnIdle(wxIdleEvent& WXUNUSED(event)) {
}

void SwisTrack::OnMakeScreenShot(wxCommandEvent& WXUNUSED(event)) {
}
