#ifndef HEADER_SocketServer
#define HEADER_SocketServer

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

class SocketServer;
class SwisTrack;

#include <wx/socket.h>

class SocketServer: public wxEvtHandler, public CommunicationInterface {

public:
	SocketServer(SwisTrack* swistrack);
	~SocketServer();

	//! Sets the desired port and tries to start listening.
	void SetPort(int port);
	//! Returns the desired port (whether open or not).
	int GetPort() {return mPort;}
	//! Returns whether the server is listening or not.
	bool IsListening() {return (mServer!=0);}

	//! Handles server events.
	void OnServerEvent(wxSocketEvent& event);
	//! Called when a message is received.
	void OnProcessMessage(CommunicationMessage *m);

	// CommuncationInterface methods.
	bool SendMessage(CommunicationMessage *m);

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
	typedef std::list<SocketServerConnection*> tConnections;
	tConnections mConnections;

	//! Starts listening.
	void Open();
	//! Stops listening. This doesn't close already open connections.
	void Close();

	DECLARE_EVENT_TABLE()
};

#endif
