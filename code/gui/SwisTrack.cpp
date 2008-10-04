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

#if defined(__WXGTK__) || defined(__WXMOTIF__)
#include "bitmaps/icon_gui.xpm"
#include "bitmaps/bitmap_new.xpm"
#include "bitmaps/bitmap_open.xpm"
#include "bitmaps/bitmap_save.xpm"
#include "bitmaps/bitmap_play.xpm"
//#include "bitmaps/bitmap_finger.xpm"
//#include "bitmaps/bitmap_pause.xpm"
#include "bitmaps/bitmap_singlestep.xpm"
#include "bitmaps/bitmap_production.xpm"
#endif

BEGIN_EVENT_TABLE(THISCLASS, wxFrame)
	EVT_MENU(sID_New, THISCLASS::OnFileNew)
	EVT_MENU(sID_Open, THISCLASS::OnFileOpen)
	EVT_MENU(sID_Save, THISCLASS::OnFileSave)
	EVT_MENU(sID_SaveAs, THISCLASS::OnFileSaveAs)
	EVT_MENU(sID_Quit,  THISCLASS::OnFileQuit)
	EVT_MENU(sID_Control_ProductionMode, THISCLASS::OnControlProductionMode)
	EVT_MENU(sID_Control_Run, THISCLASS::OnControlRun)
	EVT_MENU(sID_Control_Step, THISCLASS::OnControlStep)
	EVT_MENU(sID_Control_Reset, THISCLASS::OnControlReset)
	EVT_MENU(sID_View_ComponentList, THISCLASS::OnViewComponentList)
	EVT_MENU(sID_View_NewDisplay, THISCLASS::OnViewNewDisplay)
	EVT_MENU(sID_Tools_TCPServer, THISCLASS::OnToolsTCPServer)
	EVT_MENU(sID_Help, THISCLASS::OnHelp)
	EVT_MENU(sID_About, THISCLASS::OnHelpAbout)
	EVT_MENU(sID_DeveloperUtilityTest, THISCLASS::OnDeveloperUtilityTest)
	EVT_MENU(sID_DeveloperUtilityExportComponentsTable, THISCLASS::OnDeveloperUtilityExportComponentsTable)

	EVT_IDLE(THISCLASS::OnIdle)
END_EVENT_TABLE()

SwisTrack::SwisTrack(const wxString& title, const wxPoint& pos, const wxSize& size, long style):
		wxFrame(NULL, -1, title, pos, size, style),
		CommunicationCommandHandler(),
		mSwisTrackCore(0), mTCPServer(0), mFileName(wxT("")),
		mCanvasPanel(0), mComponentListPanel(0),
		mConfigurationPanel(0), mTimelinePanel(0), mHorizontalSizer(0) {

#ifdef MULTITHREAD
	criticalSection = new wxCriticalSection();
#endif

	// General initialization
	SetBackgroundColour(wxColour(128, 128, 128));
	SetIcon(wxICON(icon_gui));
	BuildMenuBar();
	BuildToolBar();
	BuildStatusBar();

	// The canvas panel
	mCanvasPanel = new CanvasPanel(this, this);

	// SwisTrackCore
	wxFileName filename(wxGetApp().mApplicationFolder, wxT(""), wxT(""));
	filename.AppendDir(wxT("Components"));
	mSwisTrackCore = new SwisTrackCore(filename.GetFullPath());

	// TCP server
	mTCPServer = new TCPServer(this);
	mSwisTrackCore->mCommunicationInterface = mTCPServer;
	mTCPServer->AddCommandHandler(this);

	// List
	mComponentListPanel = new ComponentListPanel(this, this);

	// Configuration panel
	mConfigurationPanel = new ConfigurationPanel(this, this, 0);

	// Timeline panel
	mTimelinePanel = new TimelinePanel(this, this);

	// Setup frame contents
	mHorizontalSizer = new wxBoxSizer(wxHORIZONTAL);
	mHorizontalSizer->Add(mCanvasPanel, 1, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL, 10);
	mHorizontalSizer->Add(mConfigurationPanel, 0, wxEXPAND, 0);

	wxBoxSizer *vs = new wxBoxSizer(wxVERTICAL);
	vs->Add(mHorizontalSizer, 3, wxEXPAND, 0);
	vs->Add(mComponentListPanel, 1, wxEXPAND, 0);
	vs->Add(mTimelinePanel, 0, wxEXPAND, 0);
	SetSizer(vs);
}

SwisTrack::~SwisTrack(){
	Control_StopRunMode();
	Control_StopProductionMode();

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
	GetMenuBar()->Append(menufile, _T("&File"));
	menufile->Append(sID_New, _T("&New\tCtrl-C"), _T("Creates a new file"));
	menufile->Append(sID_Open, _T("&Open\tCtrl-O"), _T("Opens a file"));
	menufile->Append(sID_Save, _T("&Save\tCtrl-S"), _T("Saves the current file"));
	menufile->Append(sID_SaveAs, _T("&Save as ..."), _T("Saves the file with another name"));
	menufile->AppendSeparator();
	menufile->Append(sID_Quit, _T("E&xit\tAlt-F4"), _T("Quit this program"));
	menufile->Enable(sID_Save, FALSE);

	wxMenu *menuview = new wxMenu;
	GetMenuBar()->Append(menuview, _T("&View"));
	menuview->Append(sID_View_ComponentList, _T("Component list"), _T("Displays or hides the component list"), wxITEM_CHECK);
	//menuview->AppendSeparator();
	menuview->Append(sID_View_NewDisplay, _T("New display"), _T("Opens a display in a new window"));
	menuview->Check(sID_View_ComponentList, true);

	wxMenu *menuoutput = new wxMenu;
	GetMenuBar()->Append(menuoutput, _T("&Output"));
	menuoutput->Append(sID_Tools_TCPServer, _T("TCP Server ..."), _T("TCP server settings"));

	wxMenu *menudeveloperutilities = new wxMenu;
	menudeveloperutilities->Append(sID_DeveloperUtilityTest, _T("&Test"), _T("A free menu item for developers to do quick tests while developing"));
	menudeveloperutilities->Append(sID_DeveloperUtilityExportComponentsTable, _T("&Export components table"), _T("Exports the components in wiki format"));

	wxMenu *menuhelp = new wxMenu;
	GetMenuBar()->Append(menuhelp, _T("&Help"));
	menuhelp->Append(sID_Help, _T("&Manual"), _T("Opens the online manual"));
	menuhelp->Append(sID_About, _T("&About ...\tF1"), _T("Shows the about dialog"));
	menuhelp->AppendSeparator();
	menuhelp->Append(new wxMenuItem(menuhelp, wxID_ANY, _T("&Developer utilities"), _T("Developer utilities"), wxITEM_NORMAL, menudeveloperutilities));
}

void THISCLASS::BuildToolBar() {
	// Delete the old toolbar (if any)
	wxToolBarBase *toolbar = GetToolBar();
	delete toolbar;
	SetToolBar(NULL);

	// Create toolbar
	toolbar = CreateToolBar(wxTB_FLAT | wxTB_DOCKABLE | wxTB_TEXT);
	toolbar->AddTool(sID_New, _T("New"), wxBITMAP(bitmap_new), _T("New"));
	toolbar->AddTool(sID_Open, _T("Open"), wxBITMAP(bitmap_open), _T("Open"));
	toolbar->AddTool(sID_Save, _T("Save"), wxBITMAP(bitmap_save), _T("Save"));
	toolbar->AddSeparator();
	toolbar->AddTool(sID_Control_ProductionMode, _T("Production"), wxBITMAP(bitmap_production), _T("Run in production mode"), wxITEM_CHECK);
	toolbar->AddTool(sID_Control_Run, _T("Run"), wxBITMAP(bitmap_play), _T("Run automatically"), wxITEM_CHECK);
	toolbar->AddSeparator();
	toolbar->AddTool(sID_Control_Step, _T("Step"), wxBITMAP(bitmap_singlestep), _T("Processes one image"));
	toolbar->AddTool(sID_Control_Reset, _T("Reset"), wxBITMAP(bitmap_singlestep), _T("Stops the execution. It will be started upon the next step."));
	toolbar->AddSeparator();

	toolbar->Realize();
	toolbar->SetRows(1 ? 1 : 10 / 1);
}

void THISCLASS::BuildStatusBar() {
#if wxUSE_STATUSBAR
	int n = 3;
	int w[3] = { -1, 120, 120};

	CreateStatusBar(n);
	SetStatusWidths(n, w);

	SetStatusText(_T("Welcome to SwisTrack!"), sStatusField_Messages);
	SetStatusText(_T("Closed"), sStatusField_ServerPort);
	SetStatusText(_T(""), sStatusField_Timeline);
#endif // wxUSE_STATUSBAR
}

void THISCLASS::SetConfigurationPanel(Component *c) {
	if (mConfigurationPanel->mComponent == c) {
		return;
	}

	// Destroy the old panel
	mConfigurationPanel->Destroy();

	// Create a new panel
	mConfigurationPanel = new ConfigurationPanel(this, this, c);
	mHorizontalSizer->Add(mConfigurationPanel, 0, wxEXPAND, 0);
	GetSizer()->Layout();

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

void THISCLASS::Control_Step() {
	// Start (if necessary) and perform a step, unless the automatic trigger is active
	if (mSwisTrackCore->IsTriggerActive()) {
		return;
	}
	mSwisTrackCore->Start(false);
	mSwisTrackCore->Step();
}

void THISCLASS::Control_ReloadConfiguration() {
	// In production mode, we just call Control_ReloadConfiguration which may not update all configuration parameters.
	// Otherwise, we stop the simulation (it is automatically restarted upon the next step).
	if (mSwisTrackCore->IsStartedInProductionMode()) {
		mSwisTrackCore->ReloadConfiguration();
	} else {
		mSwisTrackCore->Stop();
		mSwisTrackCore->Start(false);
	}
}

void THISCLASS::Control_StartProductionMode() {
	// Stop and start in production mode, unless we are in production mode already
	if (mSwisTrackCore->IsStartedInProductionMode()) {
		return;
	}
	mSwisTrackCore->Stop();
	GetToolBar()->ToggleTool(sID_Control_ProductionMode, true);
	GetToolBar()->EnableTool(sID_Control_Reset, false);
	mSwisTrackCore->Start(true);
}

void THISCLASS::Control_StopProductionMode() {
	// Stop production mode (if we are running in production mode) and start in normal mode
	if (! mSwisTrackCore->IsStartedInProductionMode()) {
		return;
	}
	mSwisTrackCore->Stop();
	GetToolBar()->ToggleTool(sID_Control_ProductionMode, false);
	GetToolBar()->EnableTool(sID_Control_Reset, true);
	mSwisTrackCore->Start(false);
}

void THISCLASS::Control_StartRunMode() {
	// Activate the automatic trigger
	GetToolBar()->ToggleTool(sID_Control_Run, true);
	GetToolBar()->EnableTool(sID_Control_Step, false);
	mSwisTrackCore->TriggerStart();
	mSwisTrackCore->Start(false);
}

void THISCLASS::Control_StopRunMode() {
	// Deactivate the automatic trigger
	GetToolBar()->ToggleTool(sID_Control_Run, false);
	GetToolBar()->EnableTool(sID_Control_Step, true);
	mSwisTrackCore->TriggerStop();
}

bool THISCLASS::OnCommunicationCommand(CommunicationMessage *m) {
	if (m->mCommand == wxT("STEP")) {
		Control_Step();
		return true;
	} else if (m->mCommand == wxT("RELOADCONFIGURATION")) {
		Control_ReloadConfiguration();
		return true;
	} else if (m->mCommand == wxT("RUN")) {
		bool state = m->PopBool(false);
		if (state) {
			Control_StartRunMode();
		} else {
			Control_StopRunMode();
		}
		return true;
	} else if (m->mCommand == wxT("MODE")) {
		wxString str = m->PopString(wxT("DEFAULT"));
		wxString strlc = str.Lower();
		if (strlc == wxT("production")) {
			Control_StartProductionMode();
		} else {
			Control_StopProductionMode();
		}
		return true;
	}

	return false;
}

void SwisTrack::OnFileNew(wxCommandEvent& WXUNUSED(event)) {
	OpenFile(wxGetApp().mApplicationFolder + wxT("/default.swistrack"), false, true);
}

void THISCLASS::OnFileOpen(wxCommandEvent& WXUNUSED(event)) {
	wxFileDialog dlg(this, wxT("Open Configuration"), wxT(""), wxT(""), wxT("SwisTrack Configurations (*.swistrack)|*.swistrack"), wxFD_OPEN);
	if (dlg.ShowModal() != wxID_OK) {
		return;
	}

	OpenFile(dlg.GetPath(), true, false);
}

void THISCLASS::OpenFile(const wxString &filename, bool breakonerror, bool astemplate) {
	// Check if file exists and is readable
	if (breakonerror) {
		wxFileName fn(filename);
		if (! fn.IsFileReadable()) {
			wxMessageDialog dlg(this, wxT("Unable to read \n\n") + filename, wxT("Open Configuration"), wxOK);
			dlg.ShowModal();
			return;
		}
	}

	// Open the file
	ConfigurationReaderXML cr;
	if (! cr.Open(filename)) {
		if (breakonerror) {
			wxMessageDialog dlg(this, wxT("The file \n\n") + filename + wxT(" \n\ncould not be loaded. Syntax error?"), wxT("Open Configuration"), wxOK);
			dlg.ShowModal();
			return;
		}
	}

	// At this point, we consider the SwisTrack configuration to be valid. The next few lines close the current configuration and read the configuration.

	// Close the current configuration
	Control_StopProductionMode();
	Control_StopRunMode();
	SetConfigurationPanel(0);
	if (mChanged) {
		// TODO if necessary, ask the user whether he'd like to save the changes
		// return false;
	}

	// Set the new file name
	SetFileName(filename);

	// Read the components
	cr.ReadComponents(mSwisTrackCore);

	// Read the server settings
	cr.SelectRootNode();
	cr.SelectChildNode(wxT("server"));
	mTCPServer->SetPort(cr.ReadInt(wxT("port"), 3000));
	SetStatusText(wxString::Format(wxT("TCP: %d"), mTCPServer->GetPort()), sStatusField_ServerPort);

	// Set the status text
	SetStatusText(filename + wxT(" opened!"), sStatusField_Messages);

	// Show errors if there are any
	if (cr.mErrorList.mList.empty()) {
		return;
	}
	ErrorListDialog eld(this, &(cr.mErrorList), wxT("Open File"), wxString::Format(wxT("The following errors occurred while reading the file '%s':"), filename.c_str()));
	eld.ShowModal();
}

void THISCLASS::SetFileName(const wxString &filename) {
	mFileName = filename;
	if (filename == wxT("")) {
		// Set the frame title
		SetTitle(wxT("SwisTrack"));

		// Change the CWD to the application folder
		wxFileName::SetCwd(wxGetApp().mApplicationFolder);
	} else {
		// Split the filename
		wxString path;
		wxString name;
		wxString extension;
		wxFileName::SplitPath(mFileName, &path, &name, &extension);

		// Set the frame title
		SetTitle(name + wxT(" - SwisTrack"));

		// Change the CWD
		wxFileName::SetCwd(path);
	}
}

void THISCLASS::OnFileSaveAs(wxCommandEvent& WXUNUSED(event)) {
	wxFileDialog dlg(this, wxT("Save a configuration"), wxT(""), wxT(""), wxT("Configurations (*.swistrack)|*.swistrack"), wxSAVE, wxDefaultPosition);
	if (dlg.ShowModal() != wxID_OK) {
		return;
	}

	mFileName = dlg.GetPath();
	SaveFile(mFileName);
}

void THISCLASS::OnFileSave(wxCommandEvent& event) {
	if (mFileName == wxT("")) {
		OnFileSaveAs(event);
		return;
	}

	SaveFile(mFileName);
}

void THISCLASS::SaveFile(const wxString &filename) {
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
		wxMessageDialog dlg(this, wxT("There was an error writing to \n\n") + filename + wxT("\n\nThe file may be incomplete."), wxT("Save File"), wxOK);
		dlg.ShowModal();
		return;
	}

	// Set the (new) filename
	SetFileName(filename);

	// Set the status text
	SetStatusText(filename + wxT(" saved!"), sStatusField_Messages);
}

void SwisTrack::OnFileQuit(wxCommandEvent& WXUNUSED(event)) {
	Close(TRUE);
	Destroy();
}

void THISCLASS::OnControlProductionMode(wxCommandEvent& WXUNUSED(event)) {
	if (mSwisTrackCore->IsStartedInProductionMode()) {
		Control_StopProductionMode();
	} else {
		Control_StartProductionMode();
	}
}

void THISCLASS::OnControlRun(wxCommandEvent& WXUNUSED(event)) {
	if (mSwisTrackCore->IsTriggerActive()) {
		Control_StopRunMode();
	} else {
		Control_StartRunMode();
	}
}

void THISCLASS::OnControlStep(wxCommandEvent& WXUNUSED(event)) {
	Control_Step();
}

void THISCLASS::OnControlReset(wxCommandEvent& WXUNUSED(event)) {
	Control_ReloadConfiguration();
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

void SwisTrack::OnHelpAbout(wxCommandEvent& WXUNUSED(event)) {
	AboutDialog *dlg = new AboutDialog(this);
	dlg->ShowModal();
}

void THISCLASS::OnDeveloperUtilityTest(wxCommandEvent& WXUNUSED(event)) {
}

void THISCLASS::OnDeveloperUtilityExportComponentsTable(wxCommandEvent& WXUNUSED(event)) {
	// Show the file save dialog
	wxFileDialog dlg(this, wxT("Export components table"), wxT(""), wxT(""), wxT("Text (*.txt)|*.txt"), wxSAVE, wxDefaultPosition);
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
