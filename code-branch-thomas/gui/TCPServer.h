#ifndef HEADER_TCPServer
#define HEADER_TCPServer

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

class TCPServer;
class SwisTrack;

#include <wx/socket.h>
#include <list>
#include "CommunicationInterface.h"
#include "TCPServerConnection.h"

class TCPServer: public wxEvtHandler, public CommunicationInterface {

public:
	TCPServer(SwisTrack* swistrack);
	~TCPServer();

	//! Sets the desired port and tries to start listening.
	void SetPort(int port);
	//! Returns the desired port (whether open or not).
	int GetPort() {return mPort;}
	//! Returns whether the server is listening or not.
	bool IsListening() {return (mServer!=0);}

	//! Handles server events.
	void OnServerEvent(wxSocketEvent& event);

	// CommuncationInterface methods.
	bool Send(CommunicationMessage *m);

protected:
	enum {
		SERVER_ID = 100
	};

private:
	//! The socket server.
	wxSocketServer *mServer;
	//! The desired port.
	int mPort;
	//! The SwisTrack object.
	SwisTrack* mSwisTrack;
	//! The list of all connected clients.
	typedef std::list<TCPServerConnection*> tConnections;
	tConnections mConnections;

	//! Starts listening.
	void Open();
	//! Stops listening. This doesn't close already open connections.
	void Close();

	//! Cleans the connection list.
	void CleanupConnections();

	DECLARE_EVENT_TABLE()
};

#endif
