#include "TCPServerConnection.h"
#define THISCLASS TCPServerConnection

BEGIN_EVENT_TABLE(TCPServerConnection, wxEvtHandler)
	EVT_SOCKET(SOCKET_ID, TCPServerConnection::OnSocketEvent)
END_EVENT_TABLE()

THISCLASS::TCPServerConnection(TCPServer* ss, wxSocketBase *sb):
		mSocket(sb), mTCPServer(ss), mCurrentRequest(0), mSubscriptions() {

	// Configure the socket to be non-blocking
	mSocket->SetFlags(wxSOCKET_NOWAIT);

	// Setup the event handler
	mSocket->SetEventHandler(*this, SOCKET_ID);
	mSocket->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
	mSocket->Notify(true);
}

THISCLASS::~TCPServerConnection() {
	if (! mSocket) {
		return;
	}
	mSocket->Destroy();
}

void THISCLASS::OnSocketEvent(wxSocketEvent& event) {
	if (! mSocket) {
		return;
	}
	switch (event.GetSocketEvent()) {

		// We've received something
	case wxSOCKET_INPUT:
		char buffer[128];
		while (1) {
			mSocket->Read(buffer, 128);
			int read = mSocket->LastCount();
			if (read == 0) {
				break;
			}
			NMEAProcessData(buffer, read);
		}
		break;

		// The connection was closed
	case wxSOCKET_LOST:
		mSocket->Destroy();
		mSocket = 0;
		break;

		// Something else
	default:
		break;
	}
}

void THISCLASS::OnNMEAProcessMessage(CommunicationMessage *m, bool withchecksum) {
	if (! mSocket) {
		return;
	}

	// Treat special messages
	if (m->mCommand == wxT("SUBSCRIBE")) {
		// The user wants to subscribe to a subset of the messages
		mSubscriptions.clear();

		int cc = 0;
		CommunicationMessage::tParameters::iterator it = m->mParameters.begin();
		while (it != m->mParameters.end()) {
			mSubscriptions.push_back(*it);
			cc++;
			if (cc > 31) {
				break;
			}
			it++;
		}

		return;
	} else if (m->mCommand == wxT("BROADCAST")) {
		// Broadcast the content of this message to all clients

		// The first element becomes the command and is removed
		m->mCommand = m->GetString(0, wxT("BROADCAST_EMPTY_MESSAGE"));
		if (m->mParameters.begin() != m->mParameters.end()) {
			m->mParameters.erase(m->mParameters.begin());
		}

		mTCPServer->Send(m);
		return;
	}

	// Otherwise, give the message to the TCPServer for further processing
	mCurrentRequest = m;
	mTCPServer->OnCommunicationCommand(m);
	mCurrentRequest = 0;
}

void THISCLASS::OnNMEAProcessMessageChecksumError(CommunicationMessage *m) {
	// Don't do anything for now
	// TODO: such incidents should be logged or at least counted somewhere
}

void THISCLASS::OnNMEAProcessUnrecognizedChar(unsigned char chr) {
	// Don't do anything, just ignore such chars
}

void THISCLASS::OnNMEASend(const char *buffer, int len) {
	if (! mSocket) {
		return;
	}
	mSocket->Write(buffer, len);
}

bool THISCLASS::SendMessage(CommunicationMessage *m) {
	if (! mSocket) {
		return false;
	}

	// If the message is sent in reply to another message, only send it to the sender of the original message
	if (m->mInReplyTo) {
		if (m->mInReplyTo == mCurrentRequest) {
			NMEASendMessage(m);
			return true;
		}

		return false;
	}

	// If the sender didn't subscribe for any messages, send all
	if (mSubscriptions.size()==0) {
		NMEASendMessage(m);
		return true;
	}

	// Otherwise, only send if if the sender has subscribed for it
	tSubscriptions::iterator it = mSubscriptions.begin();
	while (it != mSubscriptions.end()) {
		if ((*it) == m->mCommand) {
			NMEASendMessage(m);
			return true;
		}
		it++;
	}

	return false;
}
