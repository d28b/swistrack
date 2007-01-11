#include "SocketServerConnection.h"
#define THISCLASS SocketServerConnection

#include "SocketServerConnection.h"

BEGIN_EVENT_TABLE(SocketServerConnection, wxEvtHandler)
	EVT_SOCKET(SOCKET_ID, SocketServerConnection::OnSocketEvent)
END_EVENT_TABLE()

THISCLASS::SocketServerConnection(SocketServer* ss, wxSocketBase *sb): mSocketServer(ss), mSocket(sb), mCurrentRequest(0), mSubscriptions() {
	// Configure the socket to be non-blocking
	mSocket->SetFlags(wxSOCKET_NOWAIT);
	
	// Setup the event handler
	mSocket->SetEventHandler(*this, SOCKET_ID);
	mSocket->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
	mSocket->Notify(true);
}

THISCLASS::~SocketServerConnection() {
	mSocket->Destroy();
}

void THISCLASS::OnSocketEvent(wxSocketEvent& event) {
	switch (event.GetSocketEvent()) {

	// We've received something
	case wxSOCKET_INPUT:
		unsigned char buffer[128];
		while (1) {
			mSocket->Read(buffer, 128);
			int read=mSocket->LastCount();
			if (read==0) {break;}
			NMEAProcessData(buffer, read);
		}
		break;

	// The connection was closed
	case wxSOCKET_LOST:
		mSocket->Destroy();
		mSocket=0;
		break;
	}
}

void THISCLASS::OnNMEAProcessMessage(CommunicationMessage *m, bool withchecksum) {
	// If the used wants to subscribe, we process the message by ourselves.
	if (m->mCommand=="SUBSCRIBE") {
		mSubscriptions.clear();
		
		int cc=0;
		CommunicationMessage::tParameters::iterator it=m->mParameters.begin();
		while (it!=m->mParameters.end()) {
			mSubscriptions.push_back(*it);
			cc++;
			if (cc>31) {break;}
			it++;
		}

		return;
	}
	
	// Otherwise, give the message to the SocketServer for further processing
	mCurrentRequest=m;
	mSocketServer->OnCommand(m);
	mCurrentRequest=0;
}

void THISCLASS::OnNMEAProcessMessageChecksumError(CommunicationMessage *m) {
	// Don't do anything for now
	// TODO: such incidents should be logged or at least counted somewhere
}

void THISCLASS::OnNMEAProcessUnrecognizedChar(unsigned char chr) {
	// Don't do anything, just ignore such chars
}

void THISCLASS::OnNMEASend(const std::string &str) {
	mSocket->Write(str.c_str(), str.length());
}

bool THISCLASS::SendMessage(CommunicationMessage *m) {
	// If the message is sent in reply to another message, only send it to the sender of the original message
	if (m->mInReplyTo) {
		if (m->mInReplyTo==mCurrentRequest) {
			NMEASendMessage(m);
			return true;
		}

		return false;
	}
	
	// If it's a normal message, only send if if the sender subscribed for it
	tSubscriptions::iterator it=mSubscriptions.begin();
	while (it!=mSubscriptions.end()) {
		if (((*it)=="*") || ((*it)==m->mCommand)) {
			NMEASendMessage(m);
			return true;
		}
		it++;
	}
	
	return false;
}
