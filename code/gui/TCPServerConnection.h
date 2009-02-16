#ifndef HEADER_TCPServerConnection
#define HEADER_TCPServerConnection

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/socket.h>
#include <list>

class TCPServerConnection;

#include "TCPServer.h"
#include "CommunicationNMEAInterface.h"

class TCPServerConnection: public wxEvtHandler, public CommunicationNMEAInterface {

public:
	TCPServerConnection(TCPServer* ss, wxSocketBase *sb);
	~TCPServerConnection();

	//! Handles socket events.
	void OnSocketEvent(wxSocketEvent& event);

	// CommuncationNMEAInterface methods.
	void OnNMEAProcessMessage(CommunicationMessage *m, bool withchecksum);
	void OnNMEAProcessMessageChecksumError(CommunicationMessage *m);
	void OnNMEAProcessUnrecognizedChar(unsigned char chr);
	void OnNMEASend(const char *buffer, int len);

	//! Sends a message.
	bool SendMessage(CommunicationMessage *m);

	//! Returns whether this object is still in use or not.
	bool IsActive() {
		return (mSocket != 0);
	}

protected:
	enum {
		SOCKET_ID = 100
	};

private:
	//! The socket object.
	wxSocketBase *mSocket;
	//! The socket server object.
	TCPServer* mTCPServer;

	//! The current request. This is 0 unless a request is being processed.
	CommunicationMessage* mCurrentRequest;

	//! The list of subscriptions type.
	typedef std::list<wxString> tSubscriptions;
	//! The list of subscriptions.
	tSubscriptions mSubscriptions;

	DECLARE_EVENT_TABLE()
};

#endif
