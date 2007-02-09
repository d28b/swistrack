#ifndef HEADER_SwisTrack
#define HEADER_SwisTrack

//#define MULTITHREAD

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

class SwisTrack;

#include "AboutDialog.h"
#include "ComponentListPanel.h"
#include "SocketServer.h"
#include "CanvasPanel.h"
#include <wx/timer.h>

//! Main application window.
/*!
	Description ...
*/
class SwisTrack: public wxFrame, public CommunicationCommandHandler {

public:
	wxTimer *mTriggerTimer;			//!< The timer that triggers the steps.
	int mTriggerInterval;			//!< The trigger timer interval in milliseconds.

	SwisTrackCore *mSwisTrackCore;	//!< The SwisTrackCore object.
	SocketServer *mSocketServer;	//!< The SocketServer object.

	wxString mFileName;				//!< The current configuration file.
	bool mChanged;					//!< Whether the file has been modified. This flag is currently ignored.

	// Menu bar and items
	wxMenu *mMenuFile;
	wxMenu *mMenuTools;
	wxMenu *mMenuHelp;

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
	//! Saves a file.
	void SaveFile(const wxString &filename);

	// CommuncationCommandHandler methods
	bool OnCommunicationCommand(CommunicationMessage *m);

    // Event handlers
	void OnFileNew(wxCommandEvent& WXUNUSED(event));
	void OnFileOpen(wxCommandEvent& WXUNUSED(event));
	void OnFileSave(wxCommandEvent& WXUNUSED(event));
	void OnFileSaveAs(wxCommandEvent& WXUNUSED(event));
	void OnFileQuit(wxCommandEvent& WXUNUSED(event));
	void OnControlSeriousMode(wxCommandEvent& WXUNUSED(event));
	void OnControlFreeRun(wxCommandEvent& WXUNUSED(event));
	void OnControlSingleStep(wxCommandEvent& WXUNUSED(event));
	void OnChangeDisplaySpeed(wxScrollEvent& WXUNUSED(event));
	void OnToolsServer(wxCommandEvent& WXUNUSED(event));
	void OnHelp(wxCommandEvent& WXUNUSED(event));
	void OnTest(wxCommandEvent& WXUNUSED(event));
	void OnHelpAbout(wxCommandEvent& WXUNUSED(event));

	//! Creates the menu.
	void BuildMenuBar();
	//! Creates the toolbar.
	void BuildToolBar();
	//! Creates the status bar.
	void BuildStatusBar();

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

#ifdef MULTITHREAD
	// Critical section used to synchronize the main and auxiliary threads
	wxCriticalSection* mCriticalSection; 
#endif

protected:
	// IDs for the controls and the menu commands
	enum eIDs {
		sID_New,
		sID_Open,
		sID_Save,
		sID_SaveAs,
		sID_Quit,
		sID_Control_SeriousMode,
		sID_Control_FreeRun,
		sID_Control_SingleStep,
		sID_Tools_Server,
		sID_DisplaySpeed,
		sID_Help,
		sID_Test,
		sID_About = wxID_ABOUT   // this must be wxID_ABOUT to put it in the Mac OS X "Apple" menu
	};

	enum eStatusFields {
		sStatusField_Messages=0,
		sStatusField_ServerPort=1,
		sStatusField_Trigger=2,
	};

    DECLARE_EVENT_TABLE()
};

#endif
