#include "TCPServer.h"
#define THISCLASS TCPServer

#include "SwisTrack.h"
#include "TCPServer.h"

BEGIN_EVENT_TABLE(TCPServer, wxEvtHandler)
	EVT_SOCKET(SERVER_ID, TCPServer::OnServerEvent)
END_EVENT_TABLE()

THISCLASS::TCPServer(SwisTrack* swistrack):
		mServer(0), mPort(0), mSwisTrack(swistrack), mConnections() {
}

THISCLASS::~TCPServer() {
	if (mServer) {
		mServer->Destroy();
	}
}

void THISCLASS::SetPort(int port) {
	Close();
	mPort = port;
	Open();
}

void THISCLASS::Open() {
	if (mServer) {
		return;
	}
	if (mPort < 1) {
		return;
	}

	// Create the socket
	wxIPV4address addr;
	addr.Service(mPort);
	addr.AnyAddress();
	mServer = new wxSocketServer(addr, wxSOCKET_NOWAIT);

	// Check whether the server is listening.
	if (! mServer->Ok()) {
		wxString str;
		str.Printf(wxT("Could not listen on port %d."), mPort);
		mSwisTrack->SetStatusText(str);

		mServer->Destroy();
		mServer = 0;
		return;
	}

	// Setup the event handler and subscribe to connection events
	mServer->SetEventHandler(*this, SERVER_ID);
	mServer->SetNotify(wxSOCKET_CONNECTION_FLAG);
	mServer->Notify(true);
}

void THISCLASS::Close() {
	if (! mServer) {
		return;
	}
	mServer->Destroy();
	mServer = 0;
}

void THISCLASS::OnServerEvent(wxSocketEvent& event) {
	// Only treat connection events
	if (event.GetSocketEvent() != wxSOCKET_CONNECTION) {
		return;
	}

	// Accept the connection
	wxSocketBase *sock = mServer->Accept(false);
	if (! sock) {
		return;
	}

	// Cleanup connections that have been closed in between
	CleanupConnections();

	// Create a new connection object
	TCPServerConnection *ssc = new TCPServerConnection(this, sock);

	// Add this connection to the list of clients
	mConnections.push_back(ssc);

	// Notify the user with a status message
	wxIPV4address addr;
	sock->GetPeer(addr);
	wxString str = wxString::Format(wxT("Connection with %s:%s established."), addr.Hostname().c_str(), addr.IPAddress().c_str());
	mSwisTrack->SetStatusText(str);
}

void THISCLASS::CleanupConnections() {
	tConnections::iterator it = mConnections.begin();
	while (it != mConnections.end()) {
		tConnections::iterator next = it;
		next++;

		// If the socket is 0, this means that connection has been closed in between
		if (! (*it)->IsActive()) {
			delete *it;
			mConnections.erase(it);
		}

		it = next;
	}
}

bool THISCLASS::Send(CommunicationMessage *m) {
	bool sent = false;
	tConnections::iterator it = mConnections.begin();
	while (it != mConnections.end()) {
		bool cursent = (*it)->SendMessage(m);
		if (cursent) {
			sent = true;
		}
		it++;
	}
	return sent;
}
