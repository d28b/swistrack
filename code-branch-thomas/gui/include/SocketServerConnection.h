#ifndef HEADER_SocketServerConnection
#define HEADER_SocketServerConnection

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

#include "SocketServer.h"
#include <wx/socket.h>

class SocketServerConnection: public wxEvtHandler, public CommunicationNMEAInterface {

public:
	SocketServerConnection(SocketServer* ss, wxSocketBase *sb);
	~SocketServerConnection();

	//! Handles socket events.
	void OnSocketEvent(wxSocketEvent& event);

	// CommuncationNMEAInterface methods.
	void OnNMEAProcessMessage(CommunicationMessage *m, bool withchecksum);
	void OnNMEAProcessMessageChecksumError(CommunicationMessage *m);
	void OnNMEAProcessUnrecognizedChar(unsigned char chr);
	void OnNMEASend(const std::string &str);

	//! Sends a message.
	bool SendMessage(CommunicationMessage *m);
	
protected:
	enum {
		SOCKET_ID = 100
	};

private:
	//! The socket object.
	wxSocketBase *mSocket;
	//! The socket server object.
	SocketServer* mSocketServer;

	//! The current request. This is 0 unless a request is being processed.
	CommunicationMessage* mCurrentRequest;

	//! The list of subscriptions type.
	std::list<std::string> tSubscriptions;
	//! The list of subscriptions.
	tSubscriptions mSubscriptions;

	DECLARE_EVENT_TABLE()
};

#endif
