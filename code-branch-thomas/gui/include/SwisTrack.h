#ifndef HEADER_SwisTrack
#define HEADER_SwisTrack

//#define MULTITHREAD

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

#include <wx/timer.h>  // Spinbuttons 

//#include <wx/spinbutt.h>  // Spinbuttons 
//#include "wx/valgen.h"	  // Generic validators, used in almost every dialgo
//#include "wx/image.h"     // Necessary for image operations
//#include "wx/file.h"	  // File operations (here: FileExists)
//#include "wx/html/htmlwin.h"

class SwisTrack;

#include "SwisTrack.Constants.h"
#include "AboutDialog.h"
#include "ComponentListPanel.h"
#include "SocketServer.h"
#include "CanvasPanel.h"

/** \class SwisTrack
* \brief Main application window
*
* The class extends the wxFrame class and displays tracking progress using ObjectTracker which
* is triggered by idle events. Also, it implements all event handlers of the GUI.
*/ 
class SwisTrack: public wxFrame, public CommunicationCommandHandler {
public:
	// The free run timer.
	wxTimer mFreeRunTimer;
	// The free run interval in milliseconds.
	int mFreeRunInterval;

	//! The SwisTrackCore object.
	SwisTrackCore *mSwisTrackCore;
	//! The SocketServer object.
	SocketServer *mSocketServer;

	// Menu bar and items
	wxMenuBar *menuBar;
	wxMenu *menuFile;
	wxMenu *menuView;
	wxMenu *menuHelp;
	wxMenu *menuMode;
	wxMenu *menuTools;
	wxMenu *menuComponents;

	int mDisplaySpeed;

	// Main components
	CanvasPanel* mCanvasPanel;
	ComponentListPanel* mComponentListPanel;
	wxPanel* mPanelInformation;
	wxPanel* mPanelInformation1;

	//! Constructor.
	SwisTrack(const wxString& title, const wxPoint& pos, const wxSize& size, long style = wxDEFAULT_FRAME_STYLE);
	//! Destructor.
	~SwisTrack();

	//! Opens a file.
	void OpenFile(const wxString &filename, bool breakonerror, bool astemplate);
	//! Reads an XML file.
	void ConfigurationReadXML(xmlpp::Document *document, ErrorList *errorlist);
	//! Saves a file.
	void SaveFile(const wxString &filename);

	// CommuncationCommandHandler methods
	bool OnCommunicationCommand(CommunicationMessage *m);

    // event handlers (these functions should _not_ be virtual)
	void OnHelp(wxCommandEvent& WXUNUSED(event));
	void OnHelpAbout(wxCommandEvent& WXUNUSED(event));
	void OnTest(wxCommandEvent& WXUNUSED(event));
	void OnFileNew(wxCommandEvent& WXUNUSED(event));
	void OnFileOpen(wxCommandEvent& WXUNUSED(event));
	void OnFileSave(wxCommandEvent& WXUNUSED(event));
	void OnFileSaveAs(wxCommandEvent& WXUNUSED(event));
	void OnFileQuit(wxCommandEvent& WXUNUSED(event));
	void OnControlSeriousMode(wxCommandEvent& WXUNUSED(event));
	void OnControlFreeRun(wxCommandEvent& WXUNUSED(event));
	void OnControlSingleStep(wxCommandEvent& WXUNUSED(event));
	void OnChangeDisplaySpeed(wxScrollEvent& WXUNUSED(event));
	void OnMakeScreenShot(wxCommandEvent& WXUNUSED(event));

	//! Creates the menu.
	void BuildMenuBar();
	//! Creates the toolbar.
	void BuildToolBar();

	//! Starts the serious mode.
	void StartSeriousMode();
	//! Stops the serious mode.
	void StopSeriousMode();
	//! Performs a single step. If necessary, the core is started (in non-serious mode).
	void SingleStep();
	//! Reloads the configuration. If the core is started in serious mode, this only reloads a subset of the parameters. Otherwise, a complete core stop/start is done.
	void ReloadConfiguration();
	//! Sets the free run interval.
	void SetTriggerTimer(int interval);
	//! Sets the free run interval.
	void SetTriggerManual();

	//! The free run timer event.
	void OnFreeRunTimer(wxTimerEvent& WXUNUSED(event));
	//! The idle event.
	void OnIdle(wxIdleEvent& event);

	//! The current configuration file.
	wxString mFileName;
	bool mChanged;

#ifdef MULTITHREAD
	// Critical section used to synchronize the main and auxiliary threads
	wxCriticalSection* mCriticalSection; 
#endif

private:
	//void RefreshAllDisplays();
	double fps;       //!< FPS, read from the avi file
	int show_coverage;

protected:
	// IDs for the controls and the menu commands
	enum eConstants {
		sID_New,
		sID_Open,
		sID_Save,
		sID_SaveAs,
		sID_Quit,
		sID_Control_SeriousMode,
		sID_Control_FreeRun,
		sID_Control_SingleStep,
		sID_View_ShowTracker,
		sID_View_ShowParticleFilter,
		sID_View_ShowSegmenterPP,
		sID_View_ShowSegmenter,
		sID_View_ShowInput,
		sID_View_TrajCross,
		sID_View_TrajNoID,
		sID_View_TrajFull,
		sID_View_TrajNoCross,
		sID_View_Coverage,
		sID_View_ShowMask,
		sID_Mode_Auto,
		sID_Mode_Manual,
		sID_Mode_Intercept,
		sID_Tools_ShowCamera,
		sID_Tools_Screenshot,
		sID_DisplaySpeed,
		sID_Intercept_Done,
		sID_Intercept_Pick,
		sID_Help,
		sID_Test,
		sID_About = wxID_ABOUT   // this must be wxID_ABOUT to put it in the Mac OS X "Apple" menu
	};

    DECLARE_EVENT_TABLE()
};

#endif
