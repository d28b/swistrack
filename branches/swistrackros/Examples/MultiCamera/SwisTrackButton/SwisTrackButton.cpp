#include "SwisTrackButton.h"
#define THISCLASS SwisTrackButton

#include "Application.h"

#include <wx/msgdlg.h>
#include <wx/image.h>
#include <wx/file.h>

#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__)
#include "icon_gui.xpm"
#endif

BEGIN_EVENT_TABLE(THISCLASS, wxFrame)
	EVT_BUTTON(cID_Button_StartProduction, THISCLASS::OnButtonStartProduction)
	EVT_BUTTON(cID_Button_StartTesting, THISCLASS::OnButtonStartTesting)
	EVT_BUTTON(cID_Button_Stop, THISCLASS::OnButtonStop)
END_EVENT_TABLE()

THISCLASS::SwisTrackButton(const wxString& title, const wxPoint& pos, const wxSize& size, long style):
		wxFrame(NULL, -1, title, pos, size, style), mSocketClientCount(0) {

	// General initialization
	//SetBackgroundColour(wxColour(128, 128, 128));
	SetIcon(wxICON(icon_gui));
	BuildStatusBar();

	// Sizer
	wxBoxSizer *vs = new wxBoxSizer(wxVERTICAL);
	this->SetSizer(vs);

	// Buttons
	wxButton *button;
	button=new wxButton(this, cID_Button_StartProduction, wxT("Start (production)"));
	button->SetMinSize(wxSize(40, 40));
	button->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
	vs->Add(button, 0, wxEXPAND, 0);

	button=new wxButton(this, cID_Button_StartTesting, wxT("Start (testing)"));
	button->SetMinSize(wxSize(40, 40));
	button->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	vs->Add(button, 0, wxEXPAND, 0);

	button=new wxButton(this, cID_Button_Stop, wxT("Stop"));
	button->SetMinSize(wxSize(40, 40));
	button->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	vs->Add(button, 0, wxEXPAND, 0);

	// Connections
	ReadConfiguration("SwisTrackButton.configuration");
	//AddConnection(wxT("grmapc11.epfl.ch"), 3000);
	//AddConnection(wxT("grmapc11.epfl.ch"), 3001);
	//AddConnection(wxT("grmapc12.epfl.ch"), 3002);
	//AddConnection(wxT("grmapc12.epfl.ch"), 3005);
	//AddConnection(wxT("grmapc13.epfl.ch"), 3003);
	//AddConnection(wxT("grmapc13.epfl.ch"), 3006);
	SetStatusText(wxString::Format(wxT("%d cameras"), mSocketClientCount), cStatusField_SwisTrack);
}

THISCLASS::~SwisTrackButton() {
}

void THISCLASS::BuildStatusBar() {
#if wxUSE_STATUSBAR
	int n = 3;
	int w[3] = { -1, 100, 100};

	CreateStatusBar(n);
	SetStatusWidths(n, w);

	SetStatusText(wxT(""), cStatusField_Message);
	SetStatusText(wxT(""), cStatusField_Status);
	SetStatusText(wxT(""), cStatusField_SwisTrack);
#endif // wxUSE_STATUSBAR
}

void THISCLASS::OnButtonStartProduction(wxCommandEvent& WXUNUSED(event)) {
	SetStatusText(wxT("Started"), cStatusField_Status);
	SendToAll(wxT("$START,production\n"));
	SendToAll(wxT("$RUN,true\n"));
}

void THISCLASS::OnButtonStartTesting(wxCommandEvent& WXUNUSED(event)) {
	SetStatusText(wxT("Started"), cStatusField_Status);
	SendToAll(wxT("$START,testing\n"));
	SendToAll(wxT("$RUN,true\n"));
}

void THISCLASS::OnButtonStop(wxCommandEvent& WXUNUSED(event)) {
	SetStatusText(wxT("Stopped"), cStatusField_Status);
	SendToAll(wxT("$RUN,false\n"));
	SendToAll(wxT("$STOP\n"));
}

void THISCLASS::AddConnection(const wxString &host, int port) {
	if (mSocketClientCount>=16) {
		wxMessageBox(wxT("Sorry, I can only connect to 16 SwisTrack instances at a time!\n"));
		return;
	}

	wxPrintf(wxT("Connecting to %s:%d!\n"), host.c_str(), port);
	wxIPV4address address;
	address.Hostname(host);
	address.Service(port);
	bool connected=mSocketClient[mSocketClientCount].Connect(address, true);
	if (! connected) {
		mSocketClient[mSocketClientCount].SetFlags(wxSOCKET_WAITALL);
		wxMessageBox(wxString::Format(wxT("Connection to %s:%d failed!\n"), host.c_str(), port));
		return;
	}

	mSocketClientCount++;
}

void THISCLASS::SendToAll(const wxString &message) {
	int i;
	for (i=0; i<mSocketClientCount; i++) {
		if (mSocketClient[i].IsConnected()) {
			mSocketClient[i].Write(message.mb_str(wxConvISO8859_1), message.length());
		}
	}
}

void THISCLASS::OnNMEASend(const char *buffer, int len) {
}

void THISCLASS::OnNMEAProcessMessage(CommunicationMessage *m, bool WXUNUSED(withchecksum)) {
	if ((m->mCommand == wxT("SWISTRACK")) && (m->mParameters.size() == 2)) {
		AddConnection(m->GetString(0, wxT("")), m->GetInt(1, 3000));
	}
}

void THISCLASS::ReadConfiguration(const wxString &filename_str) {
	// Open
	wxFile file(filename_str, wxFile::read);
	if (! file.IsOpened()) {
		wxMessageBox(wxT("Warning: Could not open configuration file \'") + filename_str + wxT("\'!\n"));
		return;
	}

	// Read and process the information
	char buffer[128];
	while (1) {
		int read = file.Read(buffer, 128);
		if (read == 0) break;
		NMEAProcessData(buffer, read);
	}
}