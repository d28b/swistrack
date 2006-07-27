#include "SwisTrack.h"
#include "Canvas.h"
#include "DataLogger.h"
#include "AviWriter.h"
#include "SocketServer.h"
#include "SettingsDialog.h"
#include "TrackingPanel.h"
#include "SegmenterPanel.h"
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
EVT_MENU(Gui_View_ShowSegmenter, SwisTrack::OnMenuViewShowSegmenter)
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
	// delete and recreate the toolbar
	wxToolBarBase *toolBar = GetToolBar();
	long style = wxTB_FLAT | wxTB_DOCKABLE | wxTB_TEXT;
	//style &= ~(wxTB_NOICONS | wxTB_TEXT);

	delete toolBar;

	SetToolBar(NULL);

	//  style &= ~(wxTB_HORIZONTAL | wxTB_VERTICAL);
	//  style |= m_horzToolbar ? wxTB_HORIZONTAL : wxTB_VERTICAL;

	toolBar = CreateToolBar(style, wxID_TOOLBAR);

	// Set up toolbar
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


	toolBar->AddTool(Gui_New, _T("New"), toolBarBitmaps[0], _T("New configuration"), wxITEM_CHECK);
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

	// after adding the buttons to the toolbar, must call Realize() to reflect
	// the changes
	toolBar->Realize();

	toolBar->SetRows(1 ? 1 : 10 / 1);
}

// frame constructor
/** \brief constructor
*
* Creates menu bar and status bar 
*
* \param title    : Application name in the title bar
* \param pos	  : Position on screen
* \param size	  : Initial size
* \param style	  : Style (Icon, Always on top, etc.)
*/
SwisTrack::SwisTrack(const wxString& title, const wxPoint& pos, const wxSize& size, long style)
: wxFrame(NULL, -1, title, pos, size, style),
help(wxHF_DEFAULT_STYLE | wxHF_OPEN_FILES)
{

	show_coverage=0; // don't show coverage image
	display_speed=5; //initial display speed 5Hz
	fps=30; // initial guess for the FPS of our video
	width=640; height=480; // initial guess for video resolution
	//manual_mode = 0;

	transform = NULL;
	trackingpanel = NULL;
	segmenterpanel = NULL;
	interceptionpanel = NULL;
	datalogger = NULL;
	aviwriter = NULL;
	socketserver = NULL;
	parser = NULL;
	ot = NULL;

	// set the frame icon
	SetIcon(wxICON(gui));

#if wxUSE_MENUS
	// create a menu bar
	menuBar = new wxMenuBar();
	// create menus
	menuFile    = new wxMenu;
	menuControl = new wxMenu;
	menuHelp    = new wxMenu;
	menuView    = new wxMenu;
	menuTools   = new wxMenu;

	menuFile->Append(Gui_New, _T("&New\tCtrl-N"), _T("Create a new tracking profile"));
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

	menuView->Append(Gui_View_ShowTracker,_T("Show Tracker Control"),_T("Toggles window where tracking parameters can be changed"),TRUE);
	menuView->Check (Gui_View_ShowTracker, FALSE);
	menuView->Append(Gui_View_ShowSegmenter,_T("Show Segmenter Control"),_T("Toggles window where segmenter parameters can be changed"),TRUE);
	menuView->Check(Gui_View_ShowSegmenter,FALSE);
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
	//menuBar->EnableTop(1,FALSE);
	//menuBar->EnableTop(2,FALSE);
	menuBar->Check(Gui_View_TrajFull, TRUE);
	menuBar->Check(Gui_View_ShowMask,FALSE);

#endif // wxUSE_MENUS

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
}

/** \brief Image displaying routing ProcessData(), called for every processed frame
*
*/
void SwisTrack::ProcessData(){
	double pos;
	double time;

	//	if(!stop){
	ot->SetSemaphor(TRUE);
	pos=ot->GetProgress(2);		
	time=ot->GetProgress(1)/1000.0;

	if(datalogger)
		datalogger->WriteRow((int) pos,time);

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
		// image routines
		if(1)//if(!stop)
		{	




			RefreshAllDisplays();
			if(aviwriter) aviwriter->WriteFrame();


		}
		else{
			//SetStatusText(_T("Mutual Exclusion prevented!"),1);
		}
		//SetStatusText(_T("Finish process data"),1);
	}
	//		}
	ot->SetSemaphor(FALSE);
}


/** \brief Wrapper for transporting tracking and segmenter parameters to the segmenter and tracking class respectively.
*
*/
void SwisTrack::SetTrackingSegmenterParameters(){
	/*	ot->SetParameters(cfg->GetIntParam("/CFG/PARTICLEFILTER/@0/MAXAREA"), 
	cfg->GetIntParam("/CFG/TRACKER/@0/MAXSPEED"),
	cfg->GetIntParam("/CFG/TRACKER/@0/THRESHOLD"),
	cfg->GetIntParam("/CFG/PARTICLEFILTER/@0/MINAREA"));
	*/
	/** \todo One needs to implement here a function that sends a message to all components to re-read their
	parameters from config*/
}

SwisTrack::~SwisTrack(){
	ShutDown();
	if(ot) delete ot;
	if(socketserver) delete socketserver;
	if(canvas) delete canvas;
	if(colorbmp) delete colorbmp;
	if(parser) delete parser;

	wxThread::Sleep(100);
	Close(TRUE);
}


// event handlers
/** \brief Event handler for the 'File->New' command
*
* Opens a dialog asking the user for video, background, and number of objects.
*/ 
void SwisTrack::OnMenuFileNew(wxCommandEvent& WXUNUSED(event))
{
	SettingsDialog* cfgpan = new SettingsDialog(this);
	cfgpan->ShowModal();
}

/** \brief Event handler for the 'File->Open' command
*
* Opens an open file dialog where the user can choose a configuration.
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
* Opens an save file dialog where the user can save the current configuration
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

/** \brief Event hanlder for the 'File->Exit' command and any attempt to close the window
* 
* Stops the tracking thread (blocking) and closes the main window after its termination.
*/
void SwisTrack::OnMenuFileQuit(wxCommandEvent& WXUNUSED(event))
{
	ShutDown();
	// TRUE is to force the frame to close
	Close(TRUE);
}

/** \brief Event handler for the 'File->Control->Start' command
*
* Inits and starts tracking.
*/
void SwisTrack::OnMenuControlStart(wxCommandEvent& WXUNUSED(event))
{
	StartTracker();	
}

/** \brief Event handler for the 'File->Control->Pause' command
*
* Pauses tracking
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
* Continues tracking (after pause)
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


void SwisTrack::OnMenuControlRewind(wxCommandEvent& WXUNUSED(event))
{
	wxToolBarBase *tb = GetToolBar();
	for(int i=Gui_Ctrl_Rewind; i<= Gui_Ctrl_Pause; i++) tb->ToggleTool(i, 0 );
	tb->ToggleTool(Gui_Ctrl_Rewind, 1 );
}

void SwisTrack::OnMenuControlSinglestepback(wxCommandEvent& WXUNUSED(event))
{
	wxToolBarBase *tb = GetToolBar();
	for(int i=Gui_Ctrl_Rewind; i<= Gui_Ctrl_Pause; i++) tb->ToggleTool(i, 0 );
	tb->ToggleTool(Gui_Ctrl_Pause, 1 );
}

void SwisTrack::Singlestep(){
	wxToolBarBase *tb = GetToolBar();
	for(int i=Gui_Ctrl_Rewind; i<= Gui_Ctrl_Pause; i++) tb->ToggleTool(i, 0 );
	tb->ToggleTool(Gui_Ctrl_Pause, 1 );
	((wxSlider*) tb->FindControl(wxID_DSPSPDSLIDER))->SetValue((int) ot->GetFPS());
	display_speed=(int) ot->GetFPS();
	ot->Step();
	ProcessData();
	status=PAUSED;
}

void SwisTrack::OnMenuControlSinglestep(wxCommandEvent& WXUNUSED(event))
{
	Singlestep();
}

/** \brief Event handler for the 'File->Control->Stop' command
*
* Stops the tracking thread.
*/
void SwisTrack::OnMenuControlStop(wxCommandEvent& WXUNUSED(event))
{
	StopTracker();
}

/** \brief Menu Handler Template
*
* \todo Comment this function
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
	if(!display_speed)
		ot->SetDisplay(0);
	else
		ot->SetDisplay(1);
}

/** \brief Event handler for the 'Help->About' command
*
* Opens a dialog showing copyright information
*/
void SwisTrack::OnMenuHelpAbout(wxCommandEvent& WXUNUSED(event))
{
	AboutDialog *dlg = new AboutDialog(this);
	dlg->SetText("(c) 2004, 2005 Swarm-Intelligent Systems Group\nSwiss Federal Institute of Technology\nLausanne, Switzerland\n");
	dlg->ShowModal();
}











/** Event handler for the 'view->drawing mode' radio items
*
* Depending on which radio item has been chosen by the user, an 
* appropriate message is sent to the tracking class.
*/
void SwisTrack::OnDrawingMode(wxCommandEvent& event)
{

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
	trackingpanel->Show(checked);
}


/** \brief Event handler for the 'view->Show Segmenter' checker
*
* If the item is checked, the segmenter panel becomes visible.
*/
void SwisTrack::OnMenuViewShowSegmenter(wxCommandEvent& WXUNUSED(event))
{
	bool checked = GetMenuBar()->GetMenu(2)->IsChecked(Gui_View_ShowSegmenter);
	segmenterpanel->Show(checked);

}

/** \brief Shuts down tracking, closes output files
*
* \todo Close output files here
*/
void SwisTrack::ShutDown()
{

	wxBusyInfo info(_T("Shutting down, please wait..."), this);


	if(datalogger){
		delete(datalogger);
		datalogger=NULL;
	}
	ot->Stop();
	status=STOPPED;

	menuView->Check (Gui_View_ShowTracker, FALSE);
	menuView->Check(Gui_View_ShowSegmenter,FALSE);

	if(trackingpanel){
		trackingpanel->Destroy();
		trackingpanel=NULL;
	}
	if(segmenterpanel){
		segmenterpanel->Destroy();
		segmenterpanel=NULL;
	}
	if(interceptionpanel){
		interceptionpanel->Destroy();
		interceptionpanel=NULL;
	}
	if(transform){
		delete(transform);
		transform=NULL;
	}
	if(aviwriter){
		delete(aviwriter);
		aviwriter=NULL;
		menuTools->Check(Gui_Tools_AviOutput,FALSE);
	}

}



/** \brief This function overrides otSearching() that is called within the ObjectTracker class during initial search
*
* During initialization, the Tracker class is looking for ExperimentCfg::nins objects to track. For every frame
* that is searched, otSearching() is called.
* In this implementation, we display segmenter progress here.
*/
/*void SwisTrack::otSearching()
{ 
wxString msg;
msg.Printf("Waiting for %s objects to be found",cfg->nins);
SetStatusText(msg,1);
RefreshAllDisplays();
}*/

/** \brief Refreshes all displays
*
* This method is called during otProcessData() and during otSearching() to refresh the
* main canvas and the segmenter panel (if activated).
* Note the use of otSetSemaphor(TRUE) and otSetSemaphor(FALSE) at the beginning and end of
* the method. In case of stopping the tracking thread, the destructor will wait until 
* the flag is false before deleting the memory taken by the progress images.
*/
void SwisTrack::RefreshAllDisplays()
{
	ot->SetSemaphor(TRUE);
	if(ot->GetImagePointer()){
		IplImage* tmp;			
		if(!show_coverage){
			tmp = cvCloneImage(ot->GetImagePointer());
		}
		else{
			tmp = cvCloneImage(ot->GetCoveragePointer());
		}
		cvFlip(tmp);
		wxImage* colorimg = new wxImage(width,height,(unsigned char*) tmp->imageData,TRUE);
		if(colorbmp){
			delete(colorbmp);
			colorbmp=NULL;
		}
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

		wxImage* binaryimg = new wxImage(width,height,(unsigned char*) (tmp->imageData),TRUE);
		if(segmenterpanel->binarybmp){
			delete segmenterpanel->binarybmp;
			segmenterpanel->binarybmp=NULL;
		}
		segmenterpanel->binarybmp=new wxBitmap((&(binaryimg->Rescale(width/3,height/3))),3); //
		segmenterpanel->canvas->Refresh();
		delete binaryimg;
		cvReleaseImage(&tmp);
	}
	ot->SetSemaphor(FALSE);
}

/** \brief Eventhandler when tracking has finished
*
* Gets called after tracking has terminated (from otProcessData()), closes output files
*/
void SwisTrack::Finished()
{

	menuControl->Enable(Gui_Ctrl_Start,TRUE);
	menuControl->Enable(Gui_Ctrl_Pause,FALSE);
	menuControl->Enable(Gui_Ctrl_Continue,FALSE);
	menuControl->Enable(Gui_Ctrl_Stop,FALSE);

	if(datalogger){
		delete(datalogger);
		datalogger=NULL;
	}

	if(interceptionpanel) interceptionpanel->Destroy();
	interceptionpanel=NULL;

	DisplayModal("Tracking has successfully terminated!");

}



CvPoint2D32f* SwisTrack::GetPos(int id)
{
	//	if(!stop)
	return(ot->GetPos(id));
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
	if(!(ot->GetStatus()==STOPPED))
		if(clicked==1){
			return(cvPoint(mx,my));
		}
		else{
			SetStatusText(_T("Aborted"),0);
			return(cvPoint(-1,-1));
		}

	else{

		return(cvPoint(width/2,height/2));
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
	help.Display(wxT("Test HELPFILE"));
}


void SwisTrack::OnIdle(wxIdleEvent& WXUNUSED(event)){
	if(status==RUNNING){
		try{
			ot->Step();
			ProcessData();
			status=ot->GetStatus();
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
		trackingpanel = new TrackingPanel(this); // ...and load new trackingpanel

		if(segmenterpanel) segmenterpanel->Destroy();
		segmenterpanel = new SegmenterPanel(this);

		if(interceptionpanel) interceptionpanel->Destroy();
		interceptionpanel = new InterceptionPanel(this);

		trackingpanel->Show(FALSE);
		segmenterpanel->Show(FALSE);
		interceptionpanel->Show(FALSE);

		wxToolBarBase *tb = GetToolBar();

		ot=new ObjectTracker(cfgRoot);

		fps=ot->GetFPS(); // read the real fps
		RecreateToolbar(); // ...and update the toolbar accordingly

		menuControl->Enable(Gui_Ctrl_Start,FALSE);
		menuControl->Enable(Gui_Ctrl_Pause,TRUE);
		menuControl->Enable(Gui_Ctrl_Continue,FALSE);
		menuControl->Enable(Gui_Ctrl_Stop,TRUE);
		menuBar->EnableTop(2,TRUE);

		/*        if(!cfg->IsEmpty("/CFG/OUTPUT/@0/OUTPUTFNAME"))
		datalogger = new DataLogger((char*) cfg->GetParam("/CFG/OUTPUT/@0/OUTPUTFNAME"),this);
		else{
		DisplayModal("You did not select an output filename, no data will be written!","Message");
		}*/

		SetStatusText(_T("Searching for the specified number of objects in the first 1000 frames"),0);
		status=ot->Start();                    
		status=ot->Step();


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
