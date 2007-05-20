#ifndef HEADER_SwisTrack
#define HEADER_SwisTrack

//#define MULTITHREAD

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

class SwisTrack;

#include "AboutDialog.h"
#include "TCPServer.h"
#include "CanvasPanel.h"
#include "ComponentListPanel.h"
#include "ConfigurationPanel.h"
#include <wx/timer.h>

//! Main application window.
/*!
	Description ...
*/
class SwisTrack: public wxFrame, public CommunicationCommandHandler {

public:
	SwisTrackCore *mSwisTrackCore;	//!< The SwisTrackCore object.
	TCPServer *mTCPServer;			//!< The TCPServer object.

	wxString mFileName;				//!< The current configuration file.
	bool mChanged;					//!< Whether the file has been modified. This flag is currently ignored.

	//! Constructor.
	SwisTrack(const wxString& title, const wxPoint& pos, const wxSize& size, long style = wxDEFAULT_FRAME_STYLE);
	//! Destructor.
	~SwisTrack();

	//! Opens a file.
	void OpenFile(const wxString &filename, bool breakonerror, bool astemplate);
	//! Saves a file.
	void SaveFile(const wxString &filename);

	//! Sets the configuration panel on the right side.
	void SetConfigurationPanel(Component *c);

	//! Starts in productive mode.
	void Control_StartProductiveMode();
	//! Stops productive mode.
	void Control_StopProductiveMode();
	//! Performs a single step. If necessary, the core is started (in non-productive mode).
	void Control_Step();
	//! Reloads the configuration. If the core is started in productive mode, this only reloads a subset of the parameters. Otherwise, a complete core stop/start is performed.
	void Control_ReloadConfiguration();
	//! Starts the automatic trigger.
	void Control_StartRunMode();
	//! Stops the automatic trigger and switches to manual trigger.
	void Control_StopRunMode();

#ifdef MULTITHREAD
	// Critical section used to synchronize the main and auxiliary threads
	wxCriticalSection* mCriticalSection; 
#endif

protected:
	wxBoxSizer *mHorizontalSizer;				//!< The horizonal sizer containing the canvas panel and the component configuration panel.
	CanvasPanel *mCanvasPanel;					//!< The panel in the center displaying the current image.
	ComponentListPanel *mComponentListPanel;	//!< The list of components in the bottom of the window.
	ConfigurationPanel *mConfigurationPanel;	//!< The component configuration panel shown on the right.

	//! IDs for the controls and the menu commands
	enum eIDs {
		sID_New,
		sID_Open,
		sID_Save,
		sID_SaveAs,
		sID_Quit,
		sID_Control_ProductiveMode,
		sID_Control_Run,
		sID_Control_Step,
		sID_Control_Reset,
		sID_View_ComponentList,
		sID_View_NewDisplay,
		sID_Tools_TCPServer,
		sID_Help,
		sID_Test,
		sID_About = wxID_ABOUT   // this must be wxID_ABOUT to put it in the Mac OS X "Apple" menu
	};

	//! Status field IDs
	enum eStatusFields {
		sStatusField_Messages=0,
		sStatusField_ServerPort=1,
		sStatusField_Trigger=2,
	};

	//! Creates the menu.
	void BuildMenuBar();
	//! Creates the toolbar.
	void BuildToolBar();
	//! Creates the status bar.
	void BuildStatusBar();
	//! Sets the filename, updates the window title and sets the CWD.
	void SetFileName(const wxString &filename);

	// CommuncationCommandHandler methods
	bool OnCommunicationCommand(CommunicationMessage *m);

	// Event handlers
	void OnFileNew(wxCommandEvent& WXUNUSED(event));
	void OnFileOpen(wxCommandEvent& WXUNUSED(event));
	void OnFileSave(wxCommandEvent& WXUNUSED(event));
	void OnFileSaveAs(wxCommandEvent& WXUNUSED(event));
	void OnFileQuit(wxCommandEvent& WXUNUSED(event));
	void OnControlProductiveMode(wxCommandEvent& WXUNUSED(event));
	void OnControlRun(wxCommandEvent& WXUNUSED(event));
	void OnControlStep(wxCommandEvent& WXUNUSED(event));
	void OnControlReset(wxCommandEvent& WXUNUSED(event));
	void OnViewComponentList(wxCommandEvent& WXUNUSED(event));
	void OnViewNewDisplay(wxCommandEvent& WXUNUSED(event));
	void OnToolsTCPServer(wxCommandEvent& WXUNUSED(event));
	void OnHelp(wxCommandEvent& WXUNUSED(event));
	void OnTest(wxCommandEvent& WXUNUSED(event));
	void OnHelpAbout(wxCommandEvent& WXUNUSED(event));

	//! The idle event.
	void OnIdle(wxIdleEvent& event);

    DECLARE_EVENT_TABLE()
};

#endif
