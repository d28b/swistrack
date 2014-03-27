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
#include <fstream>
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
#include <wx/splitter.h>

#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__)
#include "bitmaps/icon_gui.xpm"
#include "bitmaps/bitmap_new.xpm"
#include "bitmaps/bitmap_open.xpm"
#include "bitmaps/bitmap_save.xpm"
#include "bitmaps/bitmap_play.xpm"
#include "bitmaps/bitmap_stop.xpm"
//#include "bitmaps/bitmap_finger.xpm"
//#include "bitmaps/bitmap_pause.xpm"
#include "bitmaps/bitmap_singlestep.xpm"
#include "bitmaps/bitmap_production.xpm"
#endif

BEGIN_EVENT_TABLE(THISCLASS, wxFrame)
	EVT_MENU(cID_New, THISCLASS::OnFileNew)
	EVT_MENU(cID_Open, THISCLASS::OnFileOpen)
	EVT_MENU(cID_Save, THISCLASS::OnFileSave)
	EVT_MENU(cID_SaveAs, THISCLASS::OnFileSaveAs)
	EVT_MENU(cID_Quit,  THISCLASS::OnFileQuit)
	EVT_MENU(cID_Control_ProductionMode, THISCLASS::OnControlProductionMode)
	EVT_MENU(cID_Control_Run, THISCLASS::OnControlRun)
	EVT_MENU(cID_Control_Stop, THISCLASS::OnControlRun)
	EVT_MENU(cID_Control_Step, THISCLASS::OnControlStep)
	EVT_MENU(cID_Control_Reset, THISCLASS::OnControlReset)
	EVT_MENU(cID_View_ComponentList, THISCLASS::OnViewComponentList)
	EVT_MENU(cID_View_NewDisplay, THISCLASS::OnViewNewDisplay)
	EVT_MENU(cID_Tools_TCPServer, THISCLASS::OnToolsTCPServer)
	EVT_MENU(cID_Help, THISCLASS::OnHelp)
	EVT_MENU(cID_About, THISCLASS::OnHelpAbout)
	EVT_MENU(cID_DeveloperUtilityTest, THISCLASS::OnDeveloperUtilityTest)
	EVT_MENU(cID_DeveloperUtilityExportComponentsTable, THISCLASS::OnDeveloperUtilityExportComponentsTable)

	EVT_IDLE(THISCLASS::OnIdle)
END_EVENT_TABLE()

THISCLASS::SwisTrack(const wxString& title, const wxPoint& pos, const wxSize& size, long style):
		wxFrame(NULL, -1, title, pos, size, style),
		CommunicationCommandHandler(),
		SwisTrackCoreInterface(),
		mSwisTrackCore(0), mTCPServer(0), mChanged(false),
		mCanvasPanel(0), mComponentListPanel(0),
		mConfigurationPanel(0), mTimelinePanel(0), mHorizontalSizer(0) {

	// General initialization
	SetBackgroundColour(wxColour(128, 128, 128));
	SetIcon(wxICON(icon_gui));
	BuildMenuBar();
	BuildToolBar();
	BuildStatusBar();

	// Splitter window
	wxSplitterWindow *splitter_window = new wxSplitterWindow(this);
	panel_top = new wxPanel(splitter_window);
	wxPanel *panel_bottom = new wxPanel(splitter_window);
	splitter_window->SplitHorizontally(panel_top, panel_bottom);
	splitter_window->SetSashPosition(450);

	// The canvas panel
	mCanvasPanel = new CanvasPanel(panel_top, this);

	// SwisTrackCore
	wxFileName filename(wxGetApp().mApplicationFolder, wxT(""), wxT(""));
	filename.AppendDir(wxT("Components"));
	mSwisTrackCore = new SwisTrackCore(filename.GetFullPath());

	// TCP server
	mTCPServer = new TCPServer(this);
	mSwisTrackCore->mCommunicationInterface = mTCPServer;
	mTCPServer->AddCommandHandler(this);

	// Top part: horizontal separation
	mHorizontalSizer = new wxBoxSizer(wxHORIZONTAL);
	panel_top->SetSizer(mHorizontalSizer);

	// Canvas (main display)
	mHorizontalSizer->Add(mCanvasPanel, 1, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL, 0);

	// Configuration panel
	mConfigurationPanel = new ConfigurationPanel(panel_top, this, 0);
	mHorizontalSizer->Add(mConfigurationPanel, 0, wxEXPAND, 0);//change during execution

	// Bottom part: vertical separation
	wxBoxSizer *vsBottom = new wxBoxSizer(wxVERTICAL);
	panel_bottom->SetSizer(vsBottom);

	// List
	mComponentListPanel = new ComponentListPanel(panel_bottom, this);
	vsBottom->Add(mComponentListPanel, 1, wxEXPAND, 0);

	// Timeline panel
	mTimelinePanel = new TimelinePanel(panel_bottom, this);
	vsBottom->Add(mTimelinePanel, 0, wxEXPAND, 0);

	wxBoxSizer *vs = new wxBoxSizer(wxVERTICAL);
	vs->Add(splitter_window, 1, wxEXPAND);
	SetSizer(vs);

	// Add SwisTrackCoreInterface
	mSwisTrackCore->AddInterface(this);
}

THISCLASS::~SwisTrack() {
	mSwisTrackCore->TriggerStop();
	mSwisTrackCore->Stop();

	// Delete the components that rely on SwisTrackCore
	mComponentListPanel->Destroy();
	mConfigurationPanel->Destroy();

	// Delete the SwisTrackCore object and then the TCP server object (in that order)
	delete mSwisTrackCore;
	delete mTCPServer;
}

void THISCLASS::BuildMenuBar() {
	// Create a menu bar
	SetMenuBar(new wxMenuBar());

	// Create menus
	wxMenu *menufile = new wxMenu;
	GetMenuBar()->Append(menufile, wxT("&File"));
	menufile->Append(cID_New, wxT("&New\tCtrl-C"), wxT("Creates a new file"));
	menufile->Append(cID_Open, wxT("&Open\tCtrl-O"), wxT("Opens a file"));
	menufile->Append(cID_Save, wxT("&Save\tCtrl-S"), wxT("Saves the current file"));
	menufile->Append(cID_SaveAs, wxT("&Save as ..."), wxT("Saves the file with another name"));
	menufile->AppendSeparator();
	menufile->Append(cID_Quit, wxT("E&xit\tAlt-F4"), wxT("Quit this program"));
	menufile->Enable(cID_Save, FALSE);

	wxMenu *menuview = new wxMenu;
	GetMenuBar()->Append(menuview, wxT("&View"));
	menuview->Append(cID_View_ComponentList, wxT("Component list"), wxT("Displays or hides the component list"), wxITEM_CHECK);
	//menuview->AppendSeparator();
	menuview->Append(cID_View_NewDisplay, wxT("New display"), wxT("Opens a display in a new window"));
	menuview->Check(cID_View_ComponentList, true);

	wxMenu *menuoutput = new wxMenu;
	GetMenuBar()->Append(menuoutput, wxT("&Output"));
	menuoutput->Append(cID_Tools_TCPServer, wxT("TCP Server ..."), wxT("TCP server settings"));

	wxMenu *menudeveloperutilities = new wxMenu;
	menudeveloperutilities->Append(cID_DeveloperUtilityTest, wxT("&Test"), wxT("A free menu item for developers to do quick tests while developing"));
	menudeveloperutilities->Append(cID_DeveloperUtilityExportComponentsTable, wxT("&Export components table"), wxT("Exports the components in wiki format"));

	wxMenu *menuhelp = new wxMenu;
	GetMenuBar()->Append(menuhelp, wxT("&Help"));
	menuhelp->Append(cID_Help, wxT("&Manual"), wxT("Opens the online manual"));
	menuhelp->Append(cID_About, wxT("&About ...\tF1"), wxT("Shows the about dialog"));
	menuhelp->AppendSeparator();
	menuhelp->Append(new wxMenuItem(menuhelp, wxID_ANY, wxT("&Developer utilities"), wxT("Developer utilities"), wxITEM_NORMAL, menudeveloperutilities));
}

void THISCLASS::BuildToolBar() {
	// Delete the old toolbar (if any)
	wxToolBarBase *toolbar = GetToolBar();
	delete toolbar;
	SetToolBar(NULL);

	// Create toolbar
	mHiddenStartStopTool = NULL;
	toolbar = CreateToolBar(wxTB_FLAT | wxTB_TEXT);
	toolbar->AddTool(cID_New, wxT("New"), wxBITMAP(bitmap_new), wxT("New"));
	toolbar->AddTool(cID_Open, wxT("Open"), wxBITMAP(bitmap_open), wxT("Open"));
	toolbar->AddTool(cID_Save, wxT("Save"), wxBITMAP(bitmap_save), wxT("Save"));
	toolbar->AddSeparator();
	toolbar->AddTool(cID_Control_ProductionMode, wxT("Production"), wxBITMAP(bitmap_production), wxT("Run in production mode"), wxITEM_CHECK);
	// We are going to add both 'start' and 'stop' tools here, then immediately remove the 'stop' one.
	// This is just to construct the 'stop' tool. That will be hidden: removed from the toolbar and stored in mHiddenStartStopTool.
	toolbar->AddTool(cID_Control_Run, wxT("Run"), wxBITMAP(bitmap_play), wxT("Run automatically"));
	toolbar->AddTool(cID_Control_Stop, wxT("Stop"), wxBITMAP(bitmap_stop), wxT("Stop running"));
	toolbar->AddSeparator();
	toolbar->AddTool(cID_Control_Step, wxT("Step"), wxBITMAP(bitmap_singlestep), wxT("Processes one image"));
	toolbar->AddTool(cID_Control_Reset, wxT("Reset"), wxBITMAP(bitmap_singlestep), wxT("Stops the execution. It will be started upon the next step."));
	toolbar->AddSeparator();

	toolbar->Realize();
	mHiddenStartStopTool = GetToolBar()->RemoveTool(cID_Control_Stop);
	//toolbar->Realize();

	toolbar->SetRows(1 ? 1 : 10 / 1);
}

void THISCLASS::BuildStatusBar() {
#if wxUSE_STATUSBAR
	int n = 4;
	int w[4] = { -1, 200, 120, 120};

	CreateStatusBar(n);
	SetStatusWidths(n, w);

	SetStatusText(wxT("No file"), cStatusField_FileFullPath);
	SetStatusText(wxT(""), cStatusField_RunTitle);
	SetStatusText(wxT("Closed"), cStatusField_ServerPort);
	SetStatusText(wxT(""), cStatusField_Timeline);
#endif // wxUSE_STATUSBAR
}

void THISCLASS::SetConfigurationPanel(Component *c) {
	if (mConfigurationPanel->mComponent == c) {
		return;
	}

	// Destroy the old panel
	mConfigurationPanel->Destroy();

	// Create a new panel
	mConfigurationPanel = new ConfigurationPanel(panel_top, this, c);
	mHorizontalSizer->Add(mConfigurationPanel, 0, wxEXPAND, 0);
	mHorizontalSizer->Layout();

	// Switch to corresponding display
	if (mConfigurationPanel->mDisplayName != wxT("")) {
		Display *di = c->GetDisplayByName(mConfigurationPanel->mDisplayName);
		mCanvasPanel->SetDisplay(di);
	}

	// Show errors
	if (mConfigurationPanel->mErrorList.mList.empty()) {
		return;
	}
	if (mSwisTrackCore->IsStartedInProductionMode()) {
		return;
	}
	ErrorListDialog eld(this, &(mConfigurationPanel->mErrorList), wxT("Component configuration"), wxT("The following errors occurred while reading the component configuration description:"));
	eld.ShowModal();
}

bool THISCLASS::OnCommunicationCommand(CommunicationMessage *m) {
	if (m->mCommand == wxT("STEP")) {
		mSwisTrackCore->Step();
		return true;
	} else if (m->mCommand == wxT("RELOADCONFIGURATION")) {
		mSwisTrackCore->ReloadConfiguration();
		return true;
	} else if (m->mCommand == wxT("RUN")) {
		bool state = m->PopBool(false);
		if (state) {
			mSwisTrackCore->TriggerStart();
		} else {
			mSwisTrackCore->TriggerStop();
		}
		return true;
	} else if (m->mCommand == wxT("START")) {
		wxString str = m->PopString(wxT("DEFAULT"));
		wxString strlc = str.Lower();
		mSwisTrackCore->Stop();
		mSwisTrackCore->Start(strlc == wxT("production"));
		return true;
	} else if (m->mCommand == wxT("STOP")) {
		mSwisTrackCore->Stop();
		return true;
	}

	return false;
}

void THISCLASS::OnBeforeStart(bool productionmode) {
	// In production mode, save the current configuration as configuration.swistrack in the run folder
	if (productionmode) {
		SaveFile(mSwisTrackCore->GetRunFileName(wxT("configuration.swistrack")), false, true);
	}

	// Update toolbar and status text
	GetToolBar()->ToggleTool(cID_Control_ProductionMode, productionmode);
	GetToolBar()->EnableTool(cID_Control_Reset, ! productionmode);
	SetStatusText(mSwisTrackCore->GetRunTitle(), cStatusField_RunTitle);
}

void THISCLASS::OnAfterStop() {
	// Update toolbar and status text
	GetToolBar()->ToggleTool(cID_Control_ProductionMode, false);
	GetToolBar()->EnableTool(cID_Control_Reset, true);
	SetStatusText(wxT(""), cStatusField_RunTitle);
}

void THISCLASS::OnBeforeTriggerStart() {
	// Switch start to stop button (by removing one and adding the other one) and store removed tool in mHiddenStartStopTool
	int pos = GetToolBar()->GetToolPos(cID_Control_Run);
	if ( pos != wxNOT_FOUND) {
		wxToolBarToolBase *temp = GetToolBar()->RemoveTool(cID_Control_Run);
		GetToolBar()->InsertTool(pos, mHiddenStartStopTool);
		mHiddenStartStopTool = temp;
		GetToolBar()->Realize();
	}

	// Update the toolbar
	GetToolBar()->EnableTool(cID_Control_Step, false);
}

void THISCLASS::OnAfterTriggerStop() {
	// Switch stop to start button (by removing one and adding the other one) and store removed tool in mHiddenStartStopTool
	int pos = GetToolBar()->GetToolPos(cID_Control_Stop);
	if ( pos != wxNOT_FOUND) { // need to check because starting SwisTrack will trigger a call to this method, even though app is in 'stopped' mode
		wxToolBarToolBase *temp = GetToolBar()->RemoveTool(cID_Control_Stop);
		GetToolBar()->InsertTool(pos, mHiddenStartStopTool);
		mHiddenStartStopTool = temp;
		GetToolBar()->Realize();
	}

	// Update the toolbar
	GetToolBar()->EnableTool(cID_Control_Step, true);
}

void THISCLASS::OnFileNew(wxCommandEvent& WXUNUSED(event)) {
	wxMessageDialog dlg(this, wxT("This will destroy your current session. Are you sure?"), wxT("Destroy current session?"), wxOK | wxCANCEL | wxICON_ERROR);
	if (dlg.ShowModal() != wxID_OK) {
		return;
	}

	NewFile();
}

void THISCLASS::OnFileOpen(wxCommandEvent& WXUNUSED(event)) {
	wxFileDialog dlg(this, wxT("Open Configuration"), wxT(""), wxT(""), wxT("SwisTrack Configurations (*.swistrack)|*.swistrack|All files|*.*"), wxFD_OPEN);
	if (dlg.ShowModal() != wxID_OK) {
		return;
	}

	OpenFile(dlg.GetPath(), true, false);
}

void THISCLASS::NewFile() {
	wxFileName file_template(wxGetApp().mApplicationFolder + wxT("/default.swistrack"));
	OpenFile(file_template, false, true);
}

void THISCLASS::OpenFile(const wxFileName &filename, bool breakonerror, bool astemplate) {
	// Check if file exists and is readable
	if (breakonerror) {
		if (! filename.IsFileReadable()) {
			wxMessageDialog dlg(this, wxT("Unable to read \n\n") + filename.GetFullPath(), wxT("Open Configuration"), wxOK);
			dlg.ShowModal();
			return;
		}
	}

	// Open the file
	ConfigurationReaderXML cr;
	if (! cr.Open(filename)) {
		if (breakonerror) {
			wxMessageDialog dlg(this, wxT("The file \n\n") + filename.GetFullPath() + wxT(" \n\ncould not be loaded. Syntax error?"), wxT("Open Configuration"), wxOK);
			dlg.ShowModal();
			return;
		}
	}

	// At this point, we consider the SwisTrack configuration to be valid and we will attempt to close the old file and use the new one. The user may still cancel the operation.

	// TODO: If necessary, ask the user whether he'd like to save the changes
	//if (mChanged) {
	// return false;
	//}

	// Close the current configuration
	mSwisTrackCore->Stop();
	mSwisTrackCore->TriggerStop();
	SetConfigurationPanel(0);

	// At this point, we definitively switch to the new file.

	// Set the new file and update the GUI
	if (astemplate) {
		SetFileName(wxFileName());
	} else {
		SetFileName(filename);
	}

	// Read the components
	cr.ReadComponents(mSwisTrackCore);

	// Read the server settings
	cr.SelectRootNode();
	cr.SelectChildNode(wxT("server"));
	mTCPServer->SetPort(cr.ReadInt(wxT("port"), 3000));

	// Show errors if there are any
	if (cr.mErrorList.mList.empty()) {
		return;
	}
	ErrorListDialog eld(this, &(cr.mErrorList), wxT("Open File"), wxString::Format(wxT("The following errors occurred while reading the file '%s':"), filename.GetFullPath().c_str()));
	eld.ShowModal();
}

void THISCLASS::SetFileName(const wxFileName &filename) {
	mSwisTrackCore->SetFileName(wxFileName(filename));

	wxFileName new_filename = mSwisTrackCore->GetFileName();
	if (new_filename.IsOk()) {
		SetTitle(new_filename.GetName() + wxT(" - SwisTrack"));
		SetStatusText(new_filename.GetFullPath(), cStatusField_FileFullPath);
	} else {
		SetTitle(wxT("SwisTrack"));
		SetStatusText(wxT("New file - no output will be written until you save it!"), cStatusField_FileFullPath);
	}

	SetStatusText(mSwisTrackCore->GetRunTitle(), cStatusField_RunTitle);
}

void THISCLASS::OnFileSaveAs(wxCommandEvent& WXUNUSED(event)) {
	wxFileDialog dlg(this, wxT("Save a configuration"), wxT(""), wxT(""), wxT("Configurations (*.swistrack)|*.swistrack|All files|*.*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);
	if (dlg.ShowModal() != wxID_OK) {
		return;
	}

	SaveFile(dlg.GetPath(), true, false);
}

void THISCLASS::OnFileSave(wxCommandEvent& event) {
	wxFileName filename = mSwisTrackCore->GetFileName();
	if (filename.IsOk()) {
		SaveFile(filename, true, false);
	} else {
		OnFileSaveAs(event);
	}
}

void THISCLASS::SaveFile(const wxFileName &filename, bool breakonerror, bool ascopy) {
	// Check if can write to that file
	/*wxFileName fn(filename);
	if (! fn.IsFileWritable()) {
		wxMessageDialog dlg(this, wxT("Unable to write \n\n")+filename, wxT("Save File"), wxOK);
		dlg.ShowModal();
		return;
	}*/

	// Save the file
	ConfigurationWriterXML cw;
	cw.WriteComponents(mSwisTrackCore);

	// Save other settings
	cw.SelectRootNode();
	cw.SelectChildNode(wxT("server"));
	cw.WriteInt(wxT("port"), mTCPServer->GetPort());

	if (! cw.Save(filename)) {
		if (breakonerror) {
			wxMessageDialog dlg(this, wxT("There was an error writing to \n\n") + filename.GetFullPath() + wxT("\n\nThe file may be incomplete."), wxT("Save File"), wxOK);
			dlg.ShowModal();
			return;
		}
	}

	// Set the (new) filename
	if (! ascopy) {
		SetFileName(wxFileName(filename));
	}
}

void THISCLASS::OnFileQuit(wxCommandEvent& WXUNUSED(event)) {
	Close(TRUE);
	Destroy();
}

void THISCLASS::OnControlProductionMode(wxCommandEvent& WXUNUSED(event)) {
	if (mSwisTrackCore->IsStartedInProductionMode()) {
		mSwisTrackCore->Stop();
		if (mSwisTrackCore->IsTriggerActive()) {
			mSwisTrackCore->Start(false);
		}
	} else {
		mSwisTrackCore->Stop();
		mSwisTrackCore->Start(true);
	}
}

void THISCLASS::OnControlRun(wxCommandEvent& WXUNUSED(event)) {
	if (mSwisTrackCore->IsTriggerActive()) {
		mSwisTrackCore->TriggerStop();
	} else {
		mSwisTrackCore->Start(false);
		mSwisTrackCore->TriggerStart();
	}
}

void THISCLASS::OnControlStep(wxCommandEvent& WXUNUSED(event)) {
	// Start (if necessary) and perform a step, unless the automatic trigger is active
	if (mSwisTrackCore->IsTriggerActive()) {
		return;
	}
	mSwisTrackCore->Start(false);
	mSwisTrackCore->Step();
}

void THISCLASS::OnControlReset(wxCommandEvent& WXUNUSED(event)) {
	mSwisTrackCore->Stop();
	mSwisTrackCore->Start(false);
}

void THISCLASS::OnIdle(wxIdleEvent& event) {
	mSwisTrackCore->OnIdle();
}

void THISCLASS::OnToolsTCPServer(wxCommandEvent& WXUNUSED(event)) {
	wxTextEntryDialog dlg(this, wxT("TCP Port:"), wxT("TCP Server"), wxT(""));
	long port = (long)(mTCPServer->GetPort());
	dlg.SetValue(wxString::Format(wxT("%ld"), port));
	if (dlg.ShowModal() != wxID_OK) {
		return;
	}

	wxString str = dlg.GetValue();
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
	wxMiniFrame *frame = new wxMiniFrame(this, -1, wxT("Display"), wxDefaultPosition, wxDefaultSize, wxCAPTION | wxRESIZE_BORDER | wxCLOSE_BOX | wxSYSTEM_MENU);
	new CanvasPanel(frame, this);
	frame->Show();
}

void THISCLASS::OnHelp(wxCommandEvent& WXUNUSED(event)) {
	wxLaunchDefaultBrowser(wxT("http://en.wikibooks.org/wiki/Swistrack"));
}

void THISCLASS::OnHelpAbout(wxCommandEvent& WXUNUSED(event)) {
	AboutDialog *dlg = new AboutDialog(this);
	dlg->ShowModal();
}

void THISCLASS::OnDeveloperUtilityTest(wxCommandEvent& WXUNUSED(event)) {
}

void THISCLASS::OnDeveloperUtilityExportComponentsTable(wxCommandEvent& WXUNUSED(event)) {
	// Show the file save dialog
	wxFileDialog dlg(this, wxT("Export components table"), wxT(""), wxT(""), wxT("Text (*.txt)|*.txt|All files|*.*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);
	if (dlg.ShowModal() != wxID_OK) {
		return;
	}

	// Open the file
	wxString filename = dlg.GetPath();
	std::ofstream ofs(filename.mb_str(wxConvFile));
	if (! ofs.is_open()) {
		wxMessageDialog dlg(this, wxT("Unable to open the file!"), wxT("Export components table"), wxOK);
		dlg.ShowModal();
		return;
	}

	// Write header
	ofs << wxT("{| border=\"1\" cellspacing=\"0\" cellpadding=\"5\"") << std::endl;
	ofs << wxT("! rowspan=2 align=\"left\" | Component name") << std::endl;
	ofs << wxT("! rowspan=2 align=\"left\" | Category") << std::endl;
	ofs << wxT("! rowspan=2 | Trigger?") << std::endl;
	ofs << wxT("! colspan=") << mSwisTrackCore->mDataStructures.size() << wxT(" | Processing channels (data structures)") << std::endl;
	ofs << wxT("|-") << std::endl;

	// Data structures
	SwisTrackCore::tDataStructureList::iterator itds = mSwisTrackCore->mDataStructures.begin();
	while (itds != mSwisTrackCore->mDataStructures.end()) {
		ofs << wxT("! ") << (*itds)->mDisplayName << std::endl;
		itds++;
	}

	// Add each component
	bool category_shaded = true;
	SwisTrackCore::tComponentList::iterator it = mSwisTrackCore->mAvailableComponents.begin();
	ComponentCategory *curcategory = 0;
	while (it != mSwisTrackCore->mAvailableComponents.end()) {
		ComponentCategory *category = (*it)->mCategory;
		if (category) {
			if ((! curcategory) || (category != curcategory)) {
				category_shaded = !category_shaded;
				curcategory = category;
			}

			// Name, category and trigger
			ofs << wxT("|- style=\"background-color:") << (category_shaded ? wxT("#eeeeee") : wxT("#ffffff")) << wxT("\" |") << std::endl;
			ofs << wxT("| [[SwisTrack/Components/") << (*it)->mName << wxT("|") << (*it)->mDisplayName << wxT("]]") << std::endl;
			ofs << wxT("| [[SwisTrack/ComponentCategories/") << curcategory->mName  << wxT("|") << curcategory->mDisplayName << wxT("]]") << std::endl;
			ofs << wxT("| align=\"center\" | ") << ((*it)->mTrigger ? wxT("T") : wxT("&nbsp;")) << std::endl;

			// Data structures
			SwisTrackCore::tDataStructureList::iterator itds = mSwisTrackCore->mDataStructures.begin();
			while (itds != mSwisTrackCore->mDataStructures.end()) {
				bool read = (*it)->HasDataStructureRead(*itds);
				bool write = (*it)->HasDataStructureWrite(*itds);
				if (read && write) {
					ofs << wxT("| align=\"center\" | E") << std::endl;
				} else if (read) {
					ofs << wxT("| align=\"center\" | R") << std::endl;
				} else if (write) {
					ofs << wxT("| align=\"center\" | W") << std::endl;
				} else {
					ofs << wxT("| align=\"center\" | &nbsp;") << std::endl;
				}

				itds++;
			}

		}
		it++;
	}

	// Footer
	ofs << wxT("|}") << std::endl;

	// Close
	ofs.close();
}
