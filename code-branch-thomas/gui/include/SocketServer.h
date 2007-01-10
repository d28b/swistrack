#ifndef _SOCKETSERVER_H
#define _SOCKETSERVER_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

class SocketServer;
class SwisTrack;

#include <wx/socket.h>

class SocketServer: public wxEvtHandler, public CommunicationInterface {

public:
	void SendLargeInteger(wxSocketBase *sock, int value);
	void SendSmallFloat(wxSocketBase *sock, double value);
	void SendNumberofTracks(wxSocketBase *sock);
	void SendNumberofBlobs(wxSocketBase *sock);
	void SendTracks(wxSocketBase *sock);
	SocketServer(SwisTrack* parent, int socketnumber);
	~SocketServer();

	//! Sets the desired port and tries to start listening.
	void SetPort(int port);
	//! Returns the desired port (whether open or not).
	int GetPort() {return mPort;}
	//! Returns whether the server is listening or not.
	bool IsListening() {return (mServer!=0);}

	//! Handles server events.
	void OnServerEvent(wxSocketEvent& event);
	//! Handles socket events.
	void OnSocketEvent(wxSocketEvent& event);

	void SendBlobs(wxSocketBase *sock);
	void SendTrajectories(wxSocketBase *sock);

	// CommuncationInterface methods.
	void SendRecord(const wxStringArray *arr);

protected:
	enum {
		SERVER_ID = 100,
		SOCKET_ID = 101
	};

private:
	//! The socket server.
	wxSocketServer *mServer;
	//! The desired port.
	int mPort;
	//! The SwisTrack object.
	SwisTrack* mSwisTrack;
	//! The list of all connected clients.
	typedef std::list<wxSocketBase*> tClientList;
	tClientList mClientList;

	//! Starts listening.
	void Open();
	//! Stops listening.
	void Close();

	DECLARE_EVENT_TABLE()
};

#endif
