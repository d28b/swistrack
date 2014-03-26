#ifndef HEADER_SwisTrackButton
#define HEADER_SwisTrackButton

//#define MULTITHREAD

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class SwisTrackButton;

#include "CommunicationNMEAInterface.h"
#include <wx/socket.h>

//! Main application window.
/*!
	This class implementes the main window of SwisTrackButton.
*/
class SwisTrackButton: public wxFrame, public CommunicationNMEAInterface {

public:
	//! Status field IDs
	enum eStatusField {
		cStatusField_Message = 0,
		cStatusField_Status = 1,
		cStatusField_SwisTrack = 2,
	};

	//! Constructor.
	SwisTrackButton(const wxString& title, const wxPoint& pos, const wxSize& size, long style = wxDEFAULT_FRAME_STYLE);
	//! Destructor.
	~SwisTrackButton();

protected:
	//! IDs for the controls and the menu commands
	enum eID {
		cID_Button_StartProduction = 1,
		cID_Button_StartTesting,
		cID_Button_Stop,
		cID_About = wxID_ABOUT
	};

	int mSocketClientCount;				//!< Number of connections.
	wxSocketClient mSocketClient[16];	//!< List of connections to SwisTrack.

	//! Creates the status bar.
	void BuildStatusBar();

	// Event handlers
	void OnButtonStartProduction(wxCommandEvent& WXUNUSED(event));
	void OnButtonStartTesting(wxCommandEvent& WXUNUSED(event));
	void OnButtonStop(wxCommandEvent& WXUNUSED(event));

	// CommunicationInterface
	void OnNMEAProcessMessage(CommunicationMessage *m, bool withchecksum);
	void OnNMEAProcessMessageChecksumError(CommunicationMessage *m) {}
	void OnNMEAProcessUnrecognizedChar(unsigned char chr) {}
	void OnNMEASend(const char *buffer, int len);

	//! Adds a SwisTrack connection.
	void AddConnection(const wxString &host, int port);
	//! Sends a message to all open connections.
	void SendToAll(const wxString &message);
	//! Reads a configuration file.
	void ReadConfiguration(const wxString &filename_str);

	DECLARE_EVENT_TABLE()
};

#endif
