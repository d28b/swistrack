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
#include "Canvas.h"
#include "AviWriter.h"
#include "SocketServer.h"
#include "SettingsDialog.h"
#include "SwisTrackPanel.h"
#include "InterceptionPanel.h"
#include "GUIApp.h"
#include <string.h>

#include "wx/toolbar.h"
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


DECLARE_APP(GUIApp)

BEGIN_EVENT_TABLE(SwisTrack, wxFrame)
EVT_MENU(Gui_New, SwisTrack::OnMenuFileNew)
EVT_MENU(Gui_Open, SwisTrack::OnMenuFileOpen)
EVT_MENU(Gui_Save, SwisTrack::OnMenuFileSave)
EVT_MENU(Gui_Quit,  SwisTrack::OnMenuFileQuit)
EVT_MENU(Gui_About, SwisTrack::OnMenuHelpAbout)
EVT_MENU(Gui_Ctrl_Start, SwisTrack::OnMenuControlStart)
EVT_MENU(Gui_Ctrl_Pause, SwisTrack::OnMenuControlPause)
EVT_MENU(Gui_Ctrl_Continue, SwisTrack::OnMenuControlContinue)
EVT_MENU(Gui_Ctrl_Rewind, SwisTrack::OnMenuControlRewind)
EVT_MENU(Gui_Ctrl_Singlestepback, SwisTrack::OnMenuControlSinglestepback)
EVT_MENU(Gui_Ctrl_Singlestep, SwisTrack::OnMenuControlSinglestep)
EVT_MENU(Gui_Mode_Intercept, SwisTrack::OnMenuIntercept)
EVT_MENU(Gui_Ctrl_Stop, SwisTrack::OnMenuControlStop)
EVT_MENU(Gui_View_ShowTracker, SwisTrack::OnMenuViewShowTracker)
EVT_MENU(Gui_View_ShowParticleFilter, SwisTrack::OnMenuViewShowParticleFilter)
EVT_MENU(Gui_View_ShowSegmenter, SwisTrack::OnMenuViewShowSegmenter)
EVT_MENU(Gui_View_ShowInput, SwisTrack::OnMenuViewShowInput)
EVT_MENU_RANGE(Gui_View_TrajCross, Gui_View_ShowMask,SwisTrack::OnDrawingMode)
EVT_MENU(Gui_Tools_SetAviOutput, SwisTrack::OnSetAviOutput)
EVT_MENU(Gui_Tools_ShowCamera,SwisTrack::OnMenuToolsShow1394Camera)
EVT_MENU(Gui_Tools_AviOutput,SwisTrack::OnEnableAVI)
EVT_MENU(Gui_Tools_Screenshot,SwisTrack::MakeScreenShot)
EVT_COMMAND_SCROLL(wxID_DSPSPDSLIDER, SwisTrack::OnChangeDisplaySpeed)
EVT_MENU(Gui_Help,SwisTrack::OnHelp)
EVT_IDLE(SwisTrack::OnIdle)
END_EVENT_TABLE()

/** \brief Creates toolbar
*
* Adds toolbar items and wires them to menubar items by using the same identifer.
* Bitmaps are loaded from a windows ressource file.
*/
void SwisTrack::RecreateToolbar()
{
	wxToolBarBase *toolBar = GetToolBar();
	long style = wxTB_FLAT | wxTB_DOCKABLE | wxTB_TEXT;
	delete toolBar;
	SetToolBar(NULL);
	toolBar = CreateToolBar(style, wxID_TOOLBAR);
	wxBitmap toolBarBitmaps[9];
	toolBarBitmaps[0] = wxBITMAP(new);
	toolBarBitmaps[1] = wxBITMAP(open);
	toolBarBitmaps[2] = wxBITMAP(save);
	toolBarBitmaps[3] = wxBITMAP(finger);
	toolBarBitmaps[4] = wxBITMAP(play);
	toolBarBitmaps[5] = wxBITMAP(pause);
	//toolBarBitmaps[6] = wxBITMAP(rewind);
	//toolBarBitmaps[7] = wxBITMAP(singlestepback);
	toolBarBitmaps[8] = wxBITMAP(singlestep);
	toolBar->AddTool(Gui_New, _T("Config"), toolBarBitmaps[0], _T("Edit configuration"), wxITEM_CHECK);
	toolBar->AddTool(Gui_Open, _T("Open"), toolBarBitmaps[1], _T("Open configuration"), wxITEM_CHECK);
	toolBar->AddTool(Gui_Save, _T("Save"), toolBarBitmaps[2], _T("Save configuration"), wxITEM_CHECK);
	toolBar->AddSeparator();
	//toolBar->AddTool(Gui_Ctrl_Rewind, _T("Rewind"), toolBarBitmaps[6], _T("Rewind"), wxITEM_CHECK);    	
	//toolBar->AddTool(Gui_Ctrl_Singlestepback, _T("Back"), toolBarBitmaps[7], _T("One step back"));    
	toolBar->AddTool(Gui_Ctrl_Continue, _T("Cont"), toolBarBitmaps[4], _T("Continues Tracking"), wxITEM_CHECK);
	toolBar->AddTool(Gui_Ctrl_Singlestep, _T("Avance"), toolBarBitmaps[8], _T("One step"));
	toolBar->AddTool(Gui_Ctrl_Pause, _T("Pause"), toolBarBitmaps[5], _T("Pauses Tracking"), wxITEM_CHECK);	
	toolBar->AddSeparator();
	toolBar->AddTool(Gui_Mode_Intercept, _T("Intercept"), toolBarBitmaps[3], _T("Override data association"));
	toolBar->AddSeparator();

	wxSlider* m_slider = new wxSlider( toolBar, wxID_DSPSPDSLIDER, 0, 0,(int) fps,  wxDefaultPosition, wxSize(155,-1),
		wxSL_AUTOTICKS | wxSL_LABELS, wxGenericValidator(&display_speed));
	m_slider->SetTickFreq(5, 0);
	m_slider->SetToolTip(_T("Display refresh (Hz)"));
	m_slider->SetValue(display_speed);
	toolBar->AddControl(m_slider);


	TransferDataFromWindow();
	toolBar->Realize();
	toolBar->SetRows(1 ? 1 : 10 / 1);
}

/** \brief Constructor
*
* Creates menu bar and status bar.
*
* \param title    : Application name in the title bar
* \param pos	  : Position on screen
* \param size	  : Initial size
* \param style	  : Style (Icon, Always on top, etc.)
*/
SwisTrack::SwisTrack(const wxString& title, const wxPoint& pos, const wxSize& size, long style)
: wxFrame(NULL, -1, title, pos, size, style)
{


	show_coverage=0; // don't show coverage image
	display_speed=5; //initial display speed 5Hz
	fps=30; // initial guess for the FPS of our video

	trackingpanel = NULL; 
	segmenterpanel = NULL;
	inputpanel = NULL;
	particlefilterpanel = NULL;
	interceptionpanel = NULL;
	
	aviwriter = NULL;
	socketserver = NULL;
	parser = NULL;
	ot = NULL;

#ifdef MULTITHREAD
	criticalSection = NULL;
	objectTrackerThread = NULL;
#endif

	SetIcon(wxICON(gui)); // set the frame icon

	// create a menu bar
	menuBar = new wxMenuBar();
	// create menus
	menuFile    = new wxMenu;
	menuControl = new wxMenu;
	menuHelp    = new wxMenu;
	menuView    = new wxMenu;
	menuTools   = new wxMenu;

	menuFile->Append(Gui_New, _T("&Config\tCtrl-C"), _T("Edits profile"));
	menuFile->Append(Gui_Open, _T("&Open\tCtrl-O"), _T("Opens a tracking profile"));
	menuFile->Append(Gui_Save, _T("&Save\tCtrl-s"), _T("Saves the current tracking profile"));
	menuFile->AppendSeparator();
	menuFile->Append(Gui_Quit, _T("E&xit\tAlt-F4"), _T("Quit this program"));
	menuFile->Enable(Gui_Save,FALSE);

	menuControl->Append(Gui_Ctrl_Start,_T("&Go\tCtrl-G"),_T("Starts tracking"));
	menuControl->AppendSeparator();
	menuControl->Append(Gui_Ctrl_Pause,_T("&Pause\tCtrl-P"),_T("Pauses tracking"));
	menuControl->Append(Gui_Ctrl_Continue,_T("&Continue\tCtrl-C"),_T("Continues tracking"));
	menuControl->AppendSeparator();
	menuControl->Append(Gui_Ctrl_Stop,_T("&Stop\tCtrl-S"),_T("Stops tracking"));
	menuControl->Enable(Gui_Ctrl_Pause,FALSE);
	menuControl->Enable(Gui_Ctrl_Continue,FALSE);
	menuControl->Enable(Gui_Ctrl_Stop,FALSE);

	menuView->Append(Gui_View_ShowTracker,_T("Show Tracker Panel"),_T("Toggles window where tracking parameters can be changed"),TRUE);
	menuView->Check (Gui_View_ShowTracker, FALSE);
	menuView->Append(Gui_View_ShowParticleFilter,_T("Show Particle Filter Panel"),_T("Toggles window where particle filters parameters can be changed"),TRUE);
	menuView->Check (Gui_View_ShowParticleFilter, FALSE);
	menuView->Append(Gui_View_ShowSegmenter,_T("Show Segmenter Panel"),_T("Toggles window where segmenter parameters can be changed"),TRUE);
	menuView->Check(Gui_View_ShowSegmenter,FALSE);
	menuView->Append(Gui_View_ShowInput,_T("Show Input Panel"),_T("Toggles window where input parameters can be changed"),TRUE);
	menuView->Check(Gui_View_ShowInput,FALSE);
	menuView->AppendSeparator();
	menuView->AppendRadioItem(Gui_View_TrajCross, _T("cross only"));
	menuView->AppendRadioItem(Gui_View_TrajNoID, _T("trace and cross"));
	menuView->AppendRadioItem(Gui_View_TrajNoCross, _T("trace and ID"));
	menuView->AppendRadioItem(Gui_View_TrajFull, _T("trace, cross, and ID"));
	menuView->AppendRadioItem(Gui_View_Coverage,_T("coverage"));
	menuView->AppendSeparator();
	menuView->Append(Gui_View_ShowMask,_T("Show Mask"),_T("Displays the contours given by the mask image"),TRUE);
	menuView->Enable(Gui_View_ShowMask,FALSE);
	menuView->Check(Gui_View_ShowMask,FALSE);

	menuTools->Append(Gui_Tools_SetAviOutput,_T("Set AVI Output"),_T("Select video file for output"));
	menuTools->Append(Gui_Tools_AviOutput,_T("Enable AVI Output"),_T("Enables/Disables Video output"),TRUE);
	menuTools->Check(Gui_Tools_AviOutput, FALSE);
	menuTools->Enable(Gui_Tools_AviOutput,FALSE);
	menuTools->AppendSeparator();
	menuTools->Append(Gui_Tools_ShowCamera,_T("Display camera"),_T("Displays data from a IEEE1394 firewire camera"));
	menuTools->AppendSeparator();
	menuTools->Append(Gui_Tools_Screenshot,_T("Save Bitmap"),_T("Saves the current view to a file"));

#ifdef _1394
	if(theCamera.CheckLink() != CAM_SUCCESS)    
		menuTools->Enable(Gui_Tools_ShowCamera,FALSE);
	else
		menuTools->Enable(Gui_Tools_ShowCamera,TRUE);
#endif

	menuHelp->Append(Gui_Help,_T("&Manual"),_T("Opens the manual"));
	menuHelp->Enable(Gui_Help,FALSE);

	menuHelp->Append(Gui_About, _T("&About...\tF1"), _T("Show about dialog"));

	// now append the freshly created menu to the menu bar...
	menuBar->Append(menuFile, _T("&File"));
	menuBar->Append(menuControl, _T("&Control"));
	menuBar->Append(menuView,_T("&View"));
	menuBar->Append(menuTools,_T("&Tools"));
	menuBar->Append(menuHelp, _T("&Help"));
	// ... and attach this menu bar to the frame
	SetMenuBar(menuBar);
	menuBar->Check(Gui_View_TrajFull, TRUE);
	menuBar->Check(Gui_View_ShowMask,FALSE);

#if wxUSE_STATUSBAR
	CreateStatusBar(2);
	SetStatusText(_T("Welcome to SwisTrack!"));
#endif // wxUSE_STATUSBAR

	colorbmp = 0;
	canvas=0;
	colorbmp = new wxBitmap(640,480);
	canvas = new Canvas(this, wxPoint(0, 0), wxSize(-1,-1));
	RecreateToolbar();

	wxInitAllImageHandlers();

	/*
	// Help files need to be compiled in the directory stated below

	bool ret;
	help.SetTempDir(wxT("."));
	ret = help.AddBook(wxFileName(wxT("doc/html/index.hhp"), wxPATH_UNIX));
	if (! ret)
	wxMessageBox(wxT("Failed adding book doc/html/index.hhp"));
	*/

	/** \todo The SocketServer should go into ObjectTracker and the socket should be a parameter */
	socketserver = new SocketServer(this,3000);

	if(!wxFile::Exists("swistrack.exp")){
		DisplayModal("File swistrack.exp could not be found. Quitting.","Error");
		Close(TRUE);
	}
	if(!wxFile::Exists("default.cfg")){
		DisplayModal("File default.cfg could not be found. Quitting.","Error");
		Close(TRUE);
	}
	try{
		parser = new xmlpp::DomParser();
		parser->parse_file("default.cfg");
		document = parser->get_document();
		cfgRoot = parser->get_document()->get_root_node();
	}
	catch (...){
		throw "Problems encountered when opening default.cfg. Invalid syntax?";
	}
	try{
		expparser = new xmlpp::DomParser();
		expparser->parse_file("swistrack.exp");
		expdocument = expparser->get_document();
		expRoot = expparser->get_document()->get_root_node();
	}
	catch (...){
		throw "Problems encountered when opening swistrack.exp. Invalid syntax?";
	}
}

/** \brief Updates all graphical output, the status bar, and AVI output
Update() is called after each processed frame. Update() only updates the graphical
output and the AVI file if the current frame number is a multiple of the frame rate
divided by the display speed set in the toolbar. For example, for a frame rate of
15Hz and a display speed of 5Hz, Update() would ignore two frames out of three.
*/
void SwisTrack::Update(){
#ifdef MULTITHREAD
	wxCriticalSectionLocker locker(*criticalSection);
#endif
	double pos;
	double time;

	if(status==RUNNING){
		pos=ot->GetProgress(2);		
		time=ot->GetProgress(1)/1000.0;

		if(fmod(pos,10)){
			char progress[200];

			double prog=ot->GetProgress(3);

			if(prog>1) prog=1;
			sprintf(progress,"Time: %10.1fs Frame: %10.0f Progress: %2.2f%%",
				time,
				pos,
				100.0*prog);
			SetStatusText(progress,1);
		}
		if(display_speed && fmod(pos,(int) (fps/display_speed))==0){
			RefreshAllDisplays();
			if(aviwriter) aviwriter->WriteFrame();
		}
	}
}


/*! \brief Destructor

Deallocates all memory and closes the application.
*/
SwisTrack::~SwisTrack(){
	ShutDown();

	if(ot) delete ot;
	if(socketserver) delete socketserver;
	if(canvas) delete canvas;
	if(colorbmp) delete colorbmp;
	if(parser) delete parser;
	if(expparser) delete expparser;
#ifdef MULTITHREAD
	if (criticalSection) delete criticalSection;
#endif
	Close(TRUE);
}


/** \brief Event handler for the 'File->New' command
*
Opens the SettingsDialog, which allows specifying parameters as defined in the
file swistrack.exp.
*/ 
void SwisTrack::OnMenuFileNew(wxCommandEvent& WXUNUSED(event))
{
	SettingsDialog* cfgpan = new SettingsDialog(this);
	cfgpan->ShowModal();
}

/** \brief Event handler for the 'File->Open' command
*
Opens an open file dialog where the user can choose a configuration and
parses the file using xmlpp::DomParser().

\see xmlpp
*/
void SwisTrack::OnMenuFileOpen(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog *dlg = new wxFileDialog(this,"Open a configuration",
		"","","Configurations (*.cfg)|*.cfg",
		wxOPEN,wxDefaultPosition);
	if(dlg->ShowModal() == wxID_OK)
	{
		SetStatusText("Open " + dlg->GetPath(),1);
		FILE* f;
		f=fopen((char*) dlg->GetPath().GetData(),"r");
		if(f){
			fclose(f);

			try{
				if(parser) delete parser;
				parser = new xmlpp::DomParser();
				parser->parse_file((char*) dlg->GetPath().GetData());
				document = parser->get_document();
				cfgRoot = parser->get_document()->get_root_node();
			}
			catch (...){
				throw "Problems encountered when loading configuration. Invalid syntax?";
			}

			SettingsDialog* cfgpan = new SettingsDialog(this);
			dlg->Destroy();
			cfgpan->ShowModal();
			return;
		}
	}
	dlg->Destroy();
}

/** \brief Event handler for the 'File->Save' command
*
Opens an save file dialog where the user can save the current configuration,
and stores the configuration as formatted XML.

\todo This function should create a copy of the XML tree and prune all information
for modes that are not used. There should be a second function that instead
saves all modes into the default.cfg (Save as default).
*/
void SwisTrack::OnMenuFileSave(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog *dlg = new wxFileDialog(this,"Save a configuration",
		"","","Configurations (*.cfg)|*.cfg",
		wxSAVE,wxDefaultPosition);
	if(dlg->ShowModal() == wxID_OK && document)
	{
		SetStatusText("Save " + dlg->GetPath(),1);
		document->write_to_file_formatted((char*) dlg->GetPath().GetData());
	}
	dlg->Destroy();
}

/** \brief Event hanlder for the 'File->Exit' command

Closes the application
\see ~Swistrack
*/
void SwisTrack::OnMenuFileQuit(wxCommandEvent& WXUNUSED(event))
{
	Close(TRUE);
}

/** \brief Event handler for the 'File->Control->Start' command
*
* Inits and starts tracking by calling StartTracker()

\see StartTracker()
*/
void SwisTrack::OnMenuControlStart(wxCommandEvent& WXUNUSED(event))
{
	StartTracker();	
}

/** \brief Event handler for the 'File->Control->Pause' command
*
* Pauses tracking and toggles the `Pause' button in the GUI.
*/
void SwisTrack::OnMenuControlPause(wxCommandEvent& WXUNUSED(event))
{
	menuControl->Enable(Gui_Ctrl_Pause,FALSE);
	menuControl->Enable(Gui_Ctrl_Continue,TRUE);

	wxToolBarBase *tb = GetToolBar();
	for(int i=Gui_Ctrl_Rewind; i<= Gui_Ctrl_Pause; i++) tb->ToggleTool(i, 0 );
	tb->ToggleTool(Gui_Ctrl_Pause, 1 );
	status=PAUSED;
}


/** \brief Event handler for the 'File->Control->Continue' command
*
Continues tracking (after pause) and toggles the `Pause' button in
the GUI.
*/
void SwisTrack::OnMenuControlContinue(wxCommandEvent& WXUNUSED(event))
{
	menuControl->Enable(Gui_Ctrl_Pause,TRUE);
	menuControl->Enable(Gui_Ctrl_Continue,FALSE);

	wxToolBarBase *tb = GetToolBar();
	for(int i=Gui_Ctrl_Rewind; i<= Gui_Ctrl_Pause; i++) tb->ToggleTool(i, 0 );
	status=RUNNING;
	tb->ToggleTool(Gui_Ctrl_Continue, 1 );

}

/*! Event handler for the rewind button in the toolbar

\todo This function does not actually do anything as the rewind button is
currently disabled. Rewind should issue a command to ObjectTracker which sets
the AVI pointer 1s back.
*/
void SwisTrack::OnMenuControlRewind(wxCommandEvent& WXUNUSED(event))
{
	wxToolBarBase *tb = GetToolBar();
	for(int i=Gui_Ctrl_Rewind; i<= Gui_Ctrl_Pause; i++) tb->ToggleTool(i, 0 );
	tb->ToggleTool(Gui_Ctrl_Rewind, 1 );
}

/*! Event handler for the single step back button in the toolbar

\todo This function does not actually do anything as the single step back button
is currently disabled. This function should issue a command to ObjectTracker which sets
the AVI pointer 1 frame back.
*/
void SwisTrack::OnMenuControlSinglestepback(wxCommandEvent& WXUNUSED(event))
{
	wxToolBarBase *tb = GetToolBar();
	for(int i=Gui_Ctrl_Rewind; i<= Gui_Ctrl_Pause; i++) tb->ToggleTool(i, 0 );
	tb->ToggleTool(Gui_Ctrl_Pause, 1 );
}

/*! 

Performing a single step is equivalent to entering the PAUSE state (i.e. OnIdleEvent
does not process the video source), but a single step is executed by calling
ObjectTracker::Step() and SwisTrack::Update().
*/
void SwisTrack::Singlestep(){
	
	wxToolBarBase *tb = GetToolBar();
	for(int i=Gui_Ctrl_Rewind; i<= Gui_Ctrl_Pause; i++) tb->ToggleTool(i, 0 );
	tb->ToggleTool(Gui_Ctrl_Pause, 1 );
#ifdef MULTITHREAD
	criticalSection->Enter();
#endif
	((wxSlider*) tb->FindControl(wxID_DSPSPDSLIDER))->SetValue((int) ot->GetFPS());
	display_speed=(int) ot->GetFPS();
	ot->Step();
#ifdef MULTITHREAD
	criticalSection->Leave();
#endif
	Update();
	status=PAUSED;
	
}

/*! \brief Event handler for the single step button in the toolbar 

Calls SwisTrack::Singlestep().
*/
void SwisTrack::OnMenuControlSinglestep(wxCommandEvent& WXUNUSED(event))
{
	Singlestep();
}

/** \brief Event handler for the 'File->Control->Stop' command
*
* Calls SwisTrack::StopTracker().
*/
void SwisTrack::OnMenuControlStop(wxCommandEvent& WXUNUSED(event))
{
	StopTracker();
}

/** \brief Event handler for the 'File->Tools->Show 1394 Camera' command

Acquires an image from the camera and displays it right away.
* \todo This function works only for 640x480 yuv411 at 15 Hz, and shows only one static image.
Instead, a user dialog should offer the choices from SettingsDialog for the input, create a new of
ObjectTracker with this configuration, and a SwisTrackPanel without graphical output.
Also OnIdleEvent should be modified to either launch a step of tracking or displaying the input
stream.
*/
void SwisTrack::OnMenuToolsShow1394Camera(wxCommandEvent& WXUNUSED(event))
{
#ifdef _1394
	unsigned char *m_pBitmap;
	theCamera.InitCamera();

	theCamera.SetVideoFrameRate(4); // 15fps
	theCamera.SetVideoFormat(0);    // 640x480, yuv411
	theCamera.SetVideoMode(2);

	m_pBitmap = new unsigned char [theCamera.m_width * theCamera.m_height * 3];


	theCamera.StartImageAcquisition();
	for(int i=0; i<5; i++){

		theCamera.AcquireImage();
		theCamera.getRGB(m_pBitmap);

		wxImage* colorimg = new wxImage(640,480,m_pBitmap,TRUE);
		colorbmp= new wxBitmap(colorimg,3);
		canvas->Refresh();
		wxSleep(1);
	}
	theCamera.StopImageAcquisition();
	delete m_pBitmap;
#endif
}

/** \brief Event handler for the 'display speed' slider in the toolbar
*
* Calls the TransferDataFromWindow() method of the toolbar that calls
* all validators on the toolbar. Also, checks whether the user is at all
* interested in seeing the display. If not, a message is sent to the
* ObjectTracker class that disables some display processing and thus
* saves some time.
*/
void SwisTrack::OnChangeDisplaySpeed(wxScrollEvent& WXUNUSED(event))
{
	
	GetToolBar()->TransferDataFromWindow();
#ifdef MULTITHREAD
	criticalSection->Enter();
#endif
	if(!display_speed)
		ot->SetDisplay(0);
	else
		ot->SetDisplay(1);
#ifdef MULTITHREAD
	criticalSection->Leave();
#endif
}

/** \brief Event handler for the 'Help->About' command
*
* Opens a dialog showing copyright information
*/
void SwisTrack::OnMenuHelpAbout(wxCommandEvent& WXUNUSED(event))
{
	AboutDialog *dlg = new AboutDialog(this);
	dlg->SetText("(c) 2004-2006 Swarm-Intelligent Systems Group\nSwiss Federal Institute of Technology\nLausanne, Switzerland\n\nhttp://swistrack.sourceforge.net");
	dlg->ShowModal();
}



/** Event handler for the 'view->drawing mode' radio items
*
* Depending on which radio item has been chosen by the user, an 
* appropriate message is sent to the tracking class.
*/
void SwisTrack::OnDrawingMode(wxCommandEvent& event)
{
#ifdef MULTITHREAD
	wxCriticalSectionLocker lock(*criticalSection);
#endif
	switch ( event.GetId() )
	{
	case Gui_View_TrajCross:{
		show_coverage=0;
		ot->SetVisualisation(TRAJCROSS);
							}
							break;
	case Gui_View_TrajNoID:{
		show_coverage=0;
		ot->SetVisualisation(TRAJNONUMBER);}
						   break;
	case Gui_View_TrajFull:{
		show_coverage=0;
		ot->SetVisualisation(TRAJFULL);}
						   break;
	case Gui_View_TrajNoCross:{
		show_coverage=0;
		ot->SetVisualisation(TRAJNOCROSS);}
							  break;
	case Gui_View_Coverage:
		show_coverage=!show_coverage;
		break;
	case Gui_View_ShowMask:{
		ot->ToggleMaskDisplay();
		SetStatusText("Mask display toggled", 0);
						   }
						   break;
	}

}


/** \brief Event handler for the 'view->Show Tracker' checker
*
* If the item is checked, the tracking panel becomes visible.
*/
void SwisTrack::OnMenuViewShowTracker(wxCommandEvent& WXUNUSED(event))
{
	bool checked = GetMenuBar()->GetMenu(2)->IsChecked(Gui_View_ShowTracker);
	if(trackingpanel) trackingpanel->Show(checked);
}


/** \brief Event handler for the 'view->Show Segmenter' checker
*
* If the item is checked, the segmenter panel becomes visible.
*/
void SwisTrack::OnMenuViewShowSegmenter(wxCommandEvent& WXUNUSED(event))
{
	bool checked = GetMenuBar()->GetMenu(2)->IsChecked(Gui_View_ShowSegmenter);
	if(segmenterpanel) segmenterpanel->Show(checked);

}

/** \brief Event handler for the 'view->Show Input' checker
*
* If the item is checked, the input panel becomes visible.
*/
void SwisTrack::OnMenuViewShowInput(wxCommandEvent& WXUNUSED(event))
{
	bool checked = GetMenuBar()->GetMenu(2)->IsChecked(Gui_View_ShowInput);
	if(inputpanel) inputpanel->Show(checked);

}

/** \brief Event handler for the 'view->Show Particle Filter' checker
*
* If the item is checked, the particlefilter panel becomes visible.
*/
void SwisTrack::OnMenuViewShowParticleFilter(wxCommandEvent& WXUNUSED(event))
{
	bool checked = GetMenuBar()->GetMenu(2)->IsChecked(Gui_View_ShowParticleFilter);
	if(particlefilterpanel) particlefilterpanel->Show(checked);

}

/** \brief Stops tracking

ShutDown() calls ObjectTracker::Stop() and stops all tracking related panels
and closes a possibly open AVI File.
*/
void SwisTrack::ShutDown()
{

	wxBusyInfo info(_T("Shutting down, please wait..."), this);
	
#ifdef MULTITHREAD
	if (objectTrackerThread) {
		if (objectTrackerThread->IsAlive()) {
			objectTrackerThread->Delete();
		}
		objectTrackerThread = NULL;
	} 
#endif

#ifdef MULTITHREAD
	criticalSection->Enter();
#endif

	if(ot){
		ot->Stop();
	}

#ifdef MULTITHREAD
	criticalSection->Leave();
#endif
	
	status=STOPPED;

	menuView->Check(Gui_View_ShowTracker, FALSE);
	menuView->Check(Gui_View_ShowParticleFilter, FALSE);
	menuView->Check(Gui_View_ShowSegmenter,FALSE);
	menuView->Check(Gui_View_ShowInput,FALSE);

	if(trackingpanel){
		trackingpanel->Destroy();
		trackingpanel=NULL;
	}
	if(particlefilterpanel){
		particlefilterpanel->Destroy();
		particlefilterpanel=NULL;
	}
	if(segmenterpanel){
		segmenterpanel->Destroy();
		segmenterpanel=NULL;
	}
	if(inputpanel){
		inputpanel->Destroy();
		inputpanel=NULL;
	}
	if(interceptionpanel){
		interceptionpanel->Destroy();
		interceptionpanel=NULL;
	}
	if(aviwriter){
		delete(aviwriter);
		aviwriter=NULL;
		menuTools->Check(Gui_Tools_AviOutput,FALSE);
	}
}

/** \brief Refreshes all displays
*
This method is called during otUpdate() to refresh the
main canvas and those panels (segmenter, tracker, input) that
are activated. 
As the frames coming from Input are in BGR, they will be converted
into RGB.
*/
void SwisTrack::RefreshAllDisplays()
{
#ifdef MULTITHREAD
	wxCriticalSectionLocker locker(*criticalSection);
#endif
	if(ot->GetImagePointer()){
		IplImage* tmp;			
		if(!show_coverage)
		{
			tmp=cvCreateImage(cvSize((ot->GetImagePointer())->width,(ot->GetImagePointer())->height),IPL_DEPTH_8U,3);
			//Converting BGR data into RGB
			cvCvtColor(ot->GetImagePointer(),tmp,CV_BGR2RGB);
		}
		else
		{
			tmp=cvCreateImage(cvSize((ot->GetCoveragePointer())->width,(ot->GetCoveragePointer())->height),IPL_DEPTH_8U,3);
			//Converting BGR data into RGB
			cvCvtColor(ot->GetCoveragePointer(),tmp,CV_BGR2RGB);
			//tmp = cvCloneImage(ot->GetCoveragePointer());
		}
		cvFlip(tmp);
		wxImage* colorimg = new wxImage(tmp->width,tmp->height,(unsigned char*) tmp->imageData,TRUE);
		if(colorbmp) delete(colorbmp);
		colorbmp= new wxBitmap(colorimg,3);
		canvas->Refresh();
		delete(colorimg);
		cvReleaseImage(&tmp);
	}	

	if(ot->GetBinaryPointer() && GetMenuBar()->GetMenu(2)->IsChecked(Gui_View_ShowSegmenter)){
		IplImage* tmp = cvCreateImage(cvSize(ot->GetBinaryPointer()->width,ot->GetBinaryPointer()->height),
			ot->GetBinaryPointer()->depth,3);

		cvCvtPlaneToPix(ot->GetBinaryPointer(), ot->GetBinaryPointer(),
			ot->GetBinaryPointer(), NULL, tmp );

		cvFlip(tmp);

		wxImage* binaryimg = new wxImage(tmp->width,tmp->height,(unsigned char*) (tmp->imageData),TRUE);
		segmenterpanel->Clear();
		segmenterpanel->CreateBitmapfromImage((&(binaryimg->Rescale(tmp->width/3,tmp->height/3))),3); //
		segmenterpanel->Refresh();
		delete binaryimg;
		cvReleaseImage(&tmp);
	}

	if(ot->GetRawImagePointer() && GetMenuBar()->GetMenu(2)->IsChecked(Gui_View_ShowInput)){
		IplImage* tmp=cvCreateImage(cvSize((ot->GetRawImagePointer())->width,(ot->GetRawImagePointer())->height),IPL_DEPTH_8U,3);
		cvCvtColor(ot->GetRawImagePointer(),tmp,CV_BGR2RGB);
		wxImage* rawimg = new wxImage(tmp->width,tmp->height,(unsigned char*) (tmp->imageData),TRUE);
		inputpanel->Clear();
		inputpanel->CreateBitmapfromImage((&(rawimg->Rescale(tmp->width/3,tmp->height/3))),3); //
		inputpanel->Refresh();
		delete rawimg;
		cvReleaseImage(&tmp);
	}
}

/** \brief Eventhandler when tracking has finished
*
* Gets called after tracking has terminated (from otUpdate()), closes output files
*/
void SwisTrack::Finished()
{

	menuControl->Enable(Gui_Ctrl_Start,TRUE);
	menuControl->Enable(Gui_Ctrl_Pause,FALSE);
	menuControl->Enable(Gui_Ctrl_Continue,FALSE);
	menuControl->Enable(Gui_Ctrl_Stop,FALSE);

	
	if(interceptionpanel) interceptionpanel->Destroy();
	interceptionpanel=NULL;

	DisplayModal("Tracking has successfully terminated!");
}



CvPoint2D32f* SwisTrack::GetPos(int id)
{
#ifdef MULTITHREAD
	wxCriticalSectionLocker locker(*criticalSection);
#endif
	//	if(!stop)
	return ot->GetPos(id);
	//	else
	//		return(NULL);
}

CvPoint SwisTrack::GetUserEstimateFor(int id)
{
	
	GUIApp* app=&(wxGetApp());

	wxString msg;
	msg.Printf("Please identify object %d, right-click to abort",id);
	SetStatusText(msg,0);
	clicked=0;
	RefreshAllDisplays();
	while(!clicked){// && !stop){
		wxThread::Sleep(100);
		app->Yield();
	}
#ifdef MULTITHREAD
	criticalSection->Enter();
#endif
	int otStatus = ot->GetStatus();
#ifdef MULTITHREAD
	criticalSection->Leave();
#endif
	if(!(otStatus==STOPPED))
		if(clicked==1){
			return(cvPoint(mx,my));
		}
		else{
			SetStatusText(_T("Aborted"),0);
			return(cvPoint(-1,-1));
		}

	else{

		return(cvPoint(0,0));
	}
}

/*void SwisTrack::OnChangeMode(wxCommandEvent& WXUNUSED(event))
{
manual_mode=!manual_mode;
// if(!stop)
ot->SetManualMode(manual_mode);
}*/

/** \brief Eventhandler for the "Intercept" button
*
* \todo Opens a dialog that allows the user to pick a trajectory for manual assignment
*/
void SwisTrack::OnMenuIntercept(wxCommandEvent& WXUNUSED(event))
{

	// Same as pause:
	menuControl->Enable(Gui_Ctrl_Pause,FALSE);
	menuControl->Enable(Gui_Ctrl_Continue,TRUE);

	wxToolBarBase *tb = GetToolBar();
	for(int i=Gui_Ctrl_Rewind; i<= Gui_Ctrl_Pause; i++) tb->ToggleTool(i, 0 );
	tb->ToggleTool(Gui_Ctrl_Pause, 1 );
	if(interceptionpanel) interceptionpanel->Show(TRUE);

	status=PAUSED;
	//	otPause();


}




void SwisTrack::OnSetAviOutput(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog *dlg = new wxFileDialog(this,"Select an output file",
		"","","Video files (*.avi)|*.avi",
		wxSAVE,wxDefaultPosition);
	if(dlg->ShowModal() == wxID_OK)
	{
		avioutfname=(dlg->GetPath());
		SetStatusText(avioutfname);
		menuTools->Enable(Gui_Tools_AviOutput,TRUE);
	}


	dlg->Destroy();
}

void SwisTrack::OnEnableAVI(wxCommandEvent &event)
{
#ifdef MULTITHREAD
	criticalSection->Enter();
#endif
	if(!ot) {
		#ifdef MULTITHREAD
			criticalSection->Leave();
		#endif
		StartTracker();
	}
#ifdef MULTITHREAD
	criticalSection->Leave();
#endif
	if(!aviwriter){
		int type;
		int result = UserInputModal("Do you want to overlay the AVI output with trajectory information?","AVI Output");
		if(result==1)
			type=OVERLAY;
		else
			type=RAWIMAGE;

		// type=RAWIMAGE;
		aviwriter=new AviWriter(&avioutfname,this,type);
		SetStatusText("Create AVI Writer");}
	else{
		delete(aviwriter);
		aviwriter=NULL;
		SetStatusText("Deletes AVI Writer");
	}
}

int SwisTrack::GetDisplaySpeed()
{
	return(display_speed);
}

void SwisTrack::OnHelp(wxCommandEvent& WXUNUSED(event))
{
	//help.Display(wxT("Test HELPFILE"));
}


void SwisTrack::OnIdle(wxIdleEvent& WXUNUSED(event)){
	if(status==RUNNING){
		try{
#ifndef MULTITHREAD
			ot->Step();
#endif
			Update();
#ifdef MULTITHREAD
			criticalSection->Enter();
#endif
			status=ot->GetStatus();
#ifdef MULTITHREAD
			criticalSection->Leave();
#endif
			wxWakeUpIdle();
		}
		catch ( char * str ) {DisplayModal(str,"Error");}
	}
}

void SwisTrack::StartTracker()
{
	double alpha=0;

	try {

		if(trackingpanel) trackingpanel->Destroy(); // delete old copy that might exist from a previous run
		trackingpanel = new SwisTrackPanel(this,"Tracking panel","/CFG/TRACKER","/CFG/COMPONENTS/TRACKER",0); // ...and load new trackingpanel
	
		if(particlefilterpanel) particlefilterpanel->Destroy();
		particlefilterpanel = new SwisTrackPanel(this,"Particle Filter panel","/CFG/PARTICLEFILTER","/CFG/COMPONENTS/PARTICLEFILTER",0);

		if(segmenterpanel) segmenterpanel->Destroy();
		segmenterpanel = new SwisTrackPanel(this,"Segmenter panel","/CFG/SEGMENTER","/CFG/COMPONENTS/SEGMENTER",1);

		if(inputpanel) inputpanel->Destroy();
		inputpanel = new SwisTrackPanel(this,"Input panel","/CFG/INPUT","/CFG/COMPONENTS/INPUT",1);

		if(interceptionpanel) interceptionpanel->Destroy();
		interceptionpanel = new InterceptionPanel(this);

		trackingpanel->Show(FALSE);
		particlefilterpanel->Show(FALSE);
		segmenterpanel->Show(FALSE);
		inputpanel->Show(FALSE);

		interceptionpanel->Show(FALSE);

		wxToolBarBase *tb = GetToolBar();

		ot=new ObjectTracker(cfgRoot);

#ifdef MULTITHREAD
		criticalSection = new wxCriticalSection();
#endif

		fps=ot->GetFPS(); // read the real fps
		RecreateToolbar(); // ...and update the toolbar accordingly

		menuControl->Enable(Gui_Ctrl_Start,FALSE);
		menuControl->Enable(Gui_Ctrl_Pause,TRUE);
		menuControl->Enable(Gui_Ctrl_Continue,FALSE);
		menuControl->Enable(Gui_Ctrl_Stop,TRUE);
		menuBar->EnableTop(2,TRUE);

		SetStatusText(_T("Searching for objects in the first 30 frames"),0);
		status=ot->Start();                    
		SetStatusText(_T(""),0);
		status=ot->Step();

#ifdef MULTITHREAD
		// If thread does not exist
		if (objectTrackerThread == NULL) {
			objectTrackerThread = new ObjectTrackerThread(ot, criticalSection);
			objectTrackerThread->Create();
			objectTrackerThread->Run();
		}		
#endif 

	}             
	catch (char* str) {
		wxString msg(str);
		DisplayModal(msg,"Error");
	}

}

void SwisTrack::StopTracker()
{
	menuControl->Enable(Gui_Ctrl_Start,TRUE);
	menuControl->Enable(Gui_Ctrl_Pause,FALSE);
	menuControl->Enable(Gui_Ctrl_Continue,FALSE);
	menuControl->Enable(Gui_Ctrl_Stop,FALSE);
	menuBar->Check(Gui_View_ShowMask,FALSE);
	menuBar->EnableTop(2,FALSE);
	ShutDown();
}

void SwisTrack::MakeScreenShot(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog *dlg = new wxFileDialog(this,"Save screenshot",
		"","","Bitmap (*.bmp)|*.bmp",
		wxSAVE,wxDefaultPosition);
	if(dlg->ShowModal() == wxID_OK)
	{
		SetStatusText("Save " + dlg->GetPath(),1);
		wxImage colorimg=colorbmp->ConvertToImage();
		colorimg.Mirror(false);
		colorimg.SaveFile(dlg->GetPath().GetData(),wxBITMAP_TYPE_BMP);
	}


	dlg->Destroy();
}



void SwisTrack::DisplayModal(wxString msg, wxString title)
{

	wxMessageDialog* dlg =
		new wxMessageDialog(this,msg,title,wxOK);
	dlg->ShowModal();
}

int SwisTrack::UserInputModal(wxString msg, wxString title)
{
	wxMessageDialog *dlg = new wxMessageDialog(this,msg,title,wxYES_NO);
	return(dlg->ShowModal());
}
