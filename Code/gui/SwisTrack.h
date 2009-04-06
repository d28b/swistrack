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
#include "TimelinePanel.h"
#include "CommunicationCommandHandler.h"
#include "SwisTrackCoreInterface.h"
#include <wx/timer.h>
#include <wx/filename.h>

//! Main application window.
/*!
	This class implementes the main window of SwisTrack.
*/
class SwisTrack: public wxFrame, public CommunicationCommandHandler, public SwisTrackCoreInterface {

public:
	//! Status field IDs
	enum eStatusField {
		cStatusField_FileFullPath = 0,
		cStatusField_RunTitle = 1,
		cStatusField_ServerPort = 2,
		cStatusField_Timeline = 3,
	};

	SwisTrackCore *mSwisTrackCore;	//!< The SwisTrackCore object.
	TCPServer *mTCPServer;			//!< The TCPServer object.

	bool mChanged;					//!< Whether the file has been modified. This flag is currently ignored.

	CanvasPanel *mCanvasPanel;					//!< The panel in the center displaying the current image.
	ComponentListPanel *mComponentListPanel;	//!< The list of components in the bottom of the window.
	ConfigurationPanel *mConfigurationPanel;	//!< The component configuration panel shown on the right.
	TimelinePanel *mTimelinePanel;				//!< The timeline panel shown on the bottom.

	//! Constructor.
	SwisTrack(const wxString& title, const wxPoint& pos, const wxSize& size, long style = wxDEFAULT_FRAME_STYLE);
	//! Destructor.
	~SwisTrack();

	//! Creates a new file.
	void NewFile();
	//! Opens a file.
	void OpenFile(const wxFileName &filename, bool breakonerror, bool astemplate);
	//! Saves a file.
	void SaveFile(const wxFileName &filename, bool breakonerror, bool ascopy);

	//! Sets the configuration panel on the right side.
	void SetConfigurationPanel(Component *c);

protected:
	//! IDs for the controls and the menu commands
	enum eID {
		cID_New = 1,
		cID_Open,
		cID_Save,
		cID_SaveAs,
		cID_Quit,
		cID_Control_ProductionMode,
		cID_Control_Run,
		cID_Control_Stop,
		cID_Control_Step,
		cID_Control_Reset,
		cID_View_ComponentList,
		cID_View_NewDisplay,
		cID_Tools_TCPServer,
		cID_Help,
		cID_DeveloperUtilityTest,
		cID_DeveloperUtilityExportComponentsTable,
		cID_About = wxID_ABOUT   // this must be wxID_ABOUT to put it in the Mac OS X "Apple" menu
	};

	wxBoxSizer *mHorizontalSizer;				//!< The horizonal sizer containing the canvas panel and the component configuration panel.
	wxToolBarToolBase *mHiddenStartStopTool; 	//!< the hidden tool, either start or stop. (I.e if start is shown, this is the stop.)
	wxPanel *panel_top;							//!< Pointer to the top panel of the wxSplitterWindow

	//! Creates the menu.
	void BuildMenuBar();
	//! Creates the toolbar.
	void BuildToolBar();
	//! Creates the status bar.
	void BuildStatusBar();
	//! Sets the filename, updates the window title and sets the CWD.
	void SetFileName(const wxFileName &filename);

	// CommuncationCommandHandler methods
	bool OnCommunicationCommand(CommunicationMessage *m);

	// SwisTrackCoreInterface methods
	void OnBeforeStart(bool productionmode);
	void OnAfterStop();
	void OnBeforeTriggerStart();
	void OnAfterTriggerStop();

	// Event handlers
	void OnFileNew(wxCommandEvent& WXUNUSED(event));
	void OnFileOpen(wxCommandEvent& WXUNUSED(event));
	void OnFileSave(wxCommandEvent& WXUNUSED(event));
	void OnFileSaveAs(wxCommandEvent& WXUNUSED(event));
	void OnFileQuit(wxCommandEvent& WXUNUSED(event));
	void OnControlProductionMode(wxCommandEvent& WXUNUSED(event));
	void OnControlRun(wxCommandEvent& WXUNUSED(event));
	void OnControlStep(wxCommandEvent& WXUNUSED(event));
	void OnControlReset(wxCommandEvent& WXUNUSED(event));
	void OnViewComponentList(wxCommandEvent& WXUNUSED(event));
	void OnViewNewDisplay(wxCommandEvent& WXUNUSED(event));
	void OnToolsTCPServer(wxCommandEvent& WXUNUSED(event));
	void OnHelp(wxCommandEvent& WXUNUSED(event));
	void OnHelpAbout(wxCommandEvent& WXUNUSED(event));
	void OnDeveloperUtilityTest(wxCommandEvent& WXUNUSED(event));
	void OnDeveloperUtilityExportComponentsTable(wxCommandEvent& WXUNUSED(event));

	//! The idle event.
	void OnIdle(wxIdleEvent& event);

	DECLARE_EVENT_TABLE()
};

#endif
