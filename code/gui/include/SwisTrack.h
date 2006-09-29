#ifndef _SwisTrack_H
#define _SwisTrack_H

#define MULTITHREAD

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif
#include "wx/minifram.h"  // Imports the mini frame class (tracker and segmenter panel)
//#include <wx/html/helpctrl.h>
#include "wx/spinbutt.h"  // Spinbuttons 
#include "wx/valgen.h"	  // Generic validators, used in almost every dialgo
#include "wx/image.h"     // Necessary for image operations
#include "wx/file.h"	  // File operations (here: FileExists)
#include "wx/busyinfo.h"  // Imports the wxBusyInfo class (used on shutdown)
#include "wx/app.h"	      // Provides app functions (setvendorname...)


//#include "calibration.h"
#include "ObjectTracker.h"
// For the auxiliary thread
#ifdef MULTITHREAD
#include "ObjectTrackerThread.h"
#endif

#ifdef _1394
#include <1394camera.h>
#endif

#include "constants.h"
#include "AboutDialog.h"
#include "XMLCfg.h"

class Canvas;
class SwisTrackPanel;
class InterceptionPanel;
class AviWriter;
class SocketServer;
class NewExperimentDialog;
class GuiApp;

/** \class SwisTrack
* \brief Main application window
*
* The class extends the wxFrame class and displays tracking progress using ObjectTracker which
* is triggered by idle events. Also, it implements all event handlers of the GUI.
*/ 
class SwisTrack : public wxFrame//, public ObjectTracker
{
public:
	int UserInputModal(wxString msg,wxString title);
	void DisplayModal(wxString msg, wxString title=_T("Message"));
	void MakeScreenShot(wxCommandEvent& WXUNUSED(event));
	void StopTracker();
	void StartTracker();
	void SetStatus(int status){ this->status=status;}
	int GetDisplaySpeed();
	void OnEnableAVI(wxCommandEvent& event);
	CvPoint GetUserEstimateFor(int id);
	void Finished();
	void ShutDown();
   // ctor(s)
	SwisTrack(const wxString& title, const wxPoint& pos, const wxSize& size,
		long style  = wxDEFAULT_FRAME_STYLE);
	
	// menu bar and items
    wxMenuBar *menuBar;
  	wxMenu *menuFile;
	wxMenu *menuView;
	wxMenu *menuControl;
	wxMenu *menuHelp;
	wxMenu *menuMode;
	wxMenu *menuTools;

	Canvas* canvas;
  
	// bitmap
	wxBitmap* colorbmp;
	//int width, height;

    // event handlers (these functions should _not_ be virtual)
	void OnHelp(wxCommandEvent& WXUNUSED(event));
    void OnSetAviOutput(wxCommandEvent& WXUNUSED(event));
	void OnMenuFileNew(wxCommandEvent& WXUNUSED(event));
	void OnMenuFileOpen(wxCommandEvent& WXUNUSED(event));
	void OnMenuFileSave(wxCommandEvent& WXUNUSED(event));
	void OnMenuFileQuit(wxCommandEvent& WXUNUSED(event));
	void OnMenuControlStart(wxCommandEvent& WXUNUSED(event));
	void OnMenuControlPause(wxCommandEvent& WXUNUSED(event));
	void OnMenuControlContinue(wxCommandEvent& WXUNUSED(event));
	void OnMenuControlStop(wxCommandEvent& WXUNUSED(event));
	void OnMenuControlSinglestep(wxCommandEvent& WXUNUSED(event));
	void Singlestep();
	void OnMenuControlSinglestepback(wxCommandEvent& WXUNUSED(event));
	void OnMenuControlRewind(wxCommandEvent& WXUNUSED(event));

	void OnMenuToolsShow1394Camera(wxCommandEvent& WXUNUSED(event));
    void OnMenuHelpAbout(wxCommandEvent& WXUNUSED(event));
	void OnChangeDisplaySpeed(wxScrollEvent& WXUNUSED(event));
	
	void OnMenuViewShowTracker(wxCommandEvent& WXUNUSED(event));
	void OnMenuViewShowSegmenter(wxCommandEvent& WXUNUSED(event));
	void OnMenuViewShowInput(wxCommandEvent& WXUNUSED(event));
	void OnMenuViewShowParticleFilter(wxCommandEvent& WXUNUSED(event));

	void OnDrawingMode(wxCommandEvent& event);
 	void OnMenuIntercept(wxCommandEvent& WXUNUSED(event));
	
    //control panels
	SwisTrackPanel* trackingpanel;
	SwisTrackPanel* segmenterpanel;
	SwisTrackPanel* inputpanel;
	SwisTrackPanel* particlefilterpanel;
	InterceptionPanel* interceptionpanel;

	//toolbar
	void RecreateToolbar();

	void Update();

	
	CvPoint2D32f* GetPos(int id);
	int clicked; // keep track of mouse
	int mx,my;
	
#ifdef _1394
	C1394Camera theCamera;  //!< Camera handle (CMU 1394 Camera Driver)
#endif
	ObjectTracker* ot;		//!< The tracking core (ObjectTracker)

	// Tool classes
	AviWriter* aviwriter;
	SocketServer* socketserver;

	~SwisTrack();

	 void OnIdle(wxIdleEvent& event);

	 xmlpp::DomParser* parser;
     xmlpp::Document* document;
     xmlpp::Element* cfgRoot;
    
	 xmlpp::DomParser* expparser;
     xmlpp::Document* expdocument;
     xmlpp::Element* expRoot;

#ifdef MULTITHREAD
	 // Critical section used to synchronize the main and auxiliary threads
	 wxCriticalSection* criticalSection; 
#endif

protected:
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()

private:
	void RefreshAllDisplays();
	int display_speed; //!< Display speed that can be changed in the toolbar
	double fps;       //!< FPS, read from the avi file
	wxString avioutfname; //!< File for video output
	//wxHtmlHelpController help; //!< Help controller
	int show_coverage;
	int status;

#ifdef MULTITHREAD
	ObjectTrackerThread* objectTrackerThread; // Object tracker auxiliary thread
#endif
	
};

#endif
