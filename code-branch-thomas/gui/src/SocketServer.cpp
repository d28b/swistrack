#include "SocketServer.h"
#define THISCLASS SocketServer

#include "SocketServer.h"

BEGIN_EVENT_TABLE(SocketServer, wxEvtHandler)
	EVT_SOCKET(SERVER_ID, SocketServer::OnServerEvent)
	EVT_SOCKET(SOCKET_ID, SocketServer::OnSocketEvent)
END_EVENT_TABLE()

THISCLASS::SocketServer(SwisTrack* swistrack): mSwisTrack(swistrack), mServer(0), mClientList() {
}

THISCLASS::~SocketServer() {
	mServer->Destroy();
}

void THISCLASS::SetPort(int port) {
	Close();
	mPort=port;
	Open();
}

void THISCLASS::Open() {
	if (mServer) {return;}
	if (mPort<1) {return;}

	// Create the socket
	wxIPV4address addr;
	addr.Service(mPort);
	addr.AnyAddress();
	mServer=new wxSocketServer(addr);

	// Check whether the server is listening.
	if (! mServer->Ok()) {
		wxString str;
		str.Printf("Could not listen at port %d.", mPort);
		mSwisTrack->SetStatusText(str);

		mServer->Destroy();
		mServer=0;
		return;
	}

	// Set a nice status 
	wxString str;
	str.Printf("Server listening at port %d.", mPort);
	mSwisTrack->SetStatusText(str);

	// Setup the event handler and subscribe to connection events
	mServer->SetEventHandler(*this, SERVER_ID);
	mServer->SetNotify(wxSOCKET_CONNECTION_FLAG);
	mServer->Notify(true);
}

void THISCLASS::Close() {
	if (! mServer) {return;}
	mServer->Destroy();
	mServer=0;
}

void THISCLASS::OnServerEvent(wxSocketEvent& event) {
	// Only treat connection events
	if (event.GetSocketEvent() != wxSOCKET_CONNECTION) {return;}

	// Accept the connection
	wxSocketBase *sock=mServer->Accept(false);
	if (! sock) {return;}

	// Setup the event handler
	sock->SetEventHandler(*this, SOCKET_ID);
	sock->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
	sock->Notify(true);

	// Add this connection to the list of clients
	mClientList->push_back(sock);

	// Notify the user with a status message
	wxIPV4address addr;
	sock->GetPeer(addr);
	wxString str;
	str.Printf("Connection with %s:%s established.", addr.Hostname().c_str(), addr.IPAddress().c_str());
	mSwisTrack->SetStatusText(str);
}

void THISCLASS::OnSocketEvent(wxSocketEvent& event) {
	wxString s = _("OnSocketEvent: ");
	wxSocketBase *sock = event.GetSocket();
	wxCommandEvent dummy;
#ifdef MULTITHREAD
	wxCriticalSectionLocker locker(*(parent->criticalSection));
#endif

	// Now we process the event
	switch(event.GetSocketEvent())
	{
	case wxSOCKET_INPUT:
		{
			// We disable input events, so that the test doesn't trigger
			// wxSocketEvent again.
			sock->SetNotify(wxSOCKET_LOST_FLAG);

			// Which test are we going to run?
			unsigned char c;
			sock->Read(&c, 1);

			switch (c)
			{
			case 'b': SendBlobs(sock); break;													// sends blobs asynch
			case 'B': {parent->Singlestep(); SendBlobs(sock);} break;							// send blobs synch
			case 'c': {calibration=0; parent->SetStatusText(_("Calibration is off"));} break;   // calibration off
			case 'C': {calibration=1; parent->SetStatusText(_("Calibration enabled"));} break;  // calibration on
			case 'i': {SendLargeInteger(sock,(int) parent->ot->GetProgress(2));} break;			// sends current frame nr
			case 'I': {SendSmallFloat(sock,100.0*(parent->ot->GetProgress(3)));} break;			// sends progress (%)
			case 'f': {SendSmallFloat(sock,parent->ot->GetFPS());} break;						// sends framerate (fps)
			case 'n': SendNumberofBlobs(sock); break;											// sends nr of blobs
			case 'N': SendNumberofTracks(sock); break;											// sends nr of tracks
			case 'p': {parent->SetStatus(PAUSED); parent->SetStatusText(_("Paused"));} break;   // pauses tracking
			case 'R': {parent->StartTracker();} break;											// starts tracker
			case 's': {parent->Singlestep();} break;											// performes single step
			case 'S': {parent->SetStatus(RUNNING); parent->SetStatusText(_("Running"));} break; // resumes tracking
			case '!': {parent->StopTracker();} break;											// stops tracker
			case 't': SendTracks(sock); break;													// sends tracks asynch
			case 'T': {parent->Singlestep(); SendTracks(sock);} break;							// sends tracks synch

			default: {
				wxString tmp;
				tmp.Printf("Unknown socket command (%c)\n",c);
				parent->SetStatusText(tmp);
					 }
			}

			// Enable input events again.
			sock->SetNotify(wxSOCKET_LOST_FLAG | wxSOCKET_INPUT_FLAG);
			break;
		}
	case wxSOCKET_LOST:
		{
			m_numClients--;

			// Destroy() should be used instead of delete wherever possible,
			// due to the fact that wxSocket uses 'delayed events' (see the
			// documentation for wxPostEvent) and we don't want an event to
			// arrive to the event handler (the frame, here) after the socket
			// has been deleted. Also, we might be doing some other thing with
			// the socket at the same time; for example, we might be in the
			// middle of a test or something. Destroy() takes care of all
			// this for us.

			parent->SetStatusText(_("Deleting socket.\n\n"));
			sock->Destroy();
			break;
		}
	default: parent->SetStatusText(_("Unknown socket event"));;
	}
}

void THISCLASS::SendBlobs(wxSocketBase *sock)
{
#ifdef MULTITHREAD
	wxCriticalSectionLocker locker(*(parent->criticalSection));
#endif
	unsigned int len,n,width;
	wxString buf;
	sock->SetFlags(wxSOCKET_WAITALL);
	n=parent->ot->GetNumberofParticles();
	buf.Printf("%#04d",n);
	width=(10+10+2);
	len=n*width+4;
	for(unsigned int i=0; i<n; i++){
		CvPoint2D32f* p;		

		if(calibration){
			p=parent->ot->GetPos(i);	
		}
		else
			p=parent->ot->GetParticlePos(i);

		buf.Printf("%s %+#010.4f %+#010.4f",buf.c_str(),p->x,p->y);
	}
	sock->Write(buf.GetData(),len);
	if(sock->Error()) parent->SetStatusText(_("Writing to socket failed"));  
}

void THISCLASS::SendTracks(wxSocketBase *sock)
{
#ifdef MULTITHREAD
  wxCriticalSectionLocker locker(*(parent->criticalSection));
#endif
	unsigned int len,n,width;
	wxString buf;
	sock->SetFlags(wxSOCKET_WAITALL);
	n=parent->ot->GetNumberofTracks();
	width=(10+10+2);
	len=n*width;
	for(unsigned int i=0; i<n; i++){
		CvPoint2D32f* p;		
		p=parent->ot->GetTargetPos(i);

		buf.Printf("%s %+#010.4f %+#010.4f",buf.c_str(),p->x,p->y);
	}
	sock->Write(buf.GetData(),len);
	if(sock->Error()) parent->SetStatusText(_("Writing to socket failed"));  
}

void THISCLASS::SendNumberofBlobs(wxSocketBase *sock)
{
#ifdef MULTITHREAD
	wxCriticalSectionLocker locker(*(parent->criticalSection));
#endif
	wxString buf;
	int n=parent->ot->GetNumberofParticles();
	buf.Printf("%04d",n);
	sock->Write(buf.GetData(),4);
	if(sock->Error()) parent->SetStatusText(_("Writing to socket failed"));
}

void THISCLASS::SendNumberofTracks(wxSocketBase *sock)
{
#ifdef MULTITHREAD
  wxCriticalSectionLocker locker(*(parent->criticalSection));
#endif
	wxString buf;
	int n=parent->ot->GetNumberofTracks();
	buf.Printf("%04d",n);
	sock->Write(buf.GetData(),4);
	if(sock->Error()) parent->SetStatusText(_("Writing to socket failed"));
}

void THISCLASS::SendSmallFloat(wxSocketBase *sock, double value)
{

	wxString buf;
	buf.Printf("%05.2f",value);
	sock->Write(buf.GetData(),5);
	if(sock->Error()) parent->SetStatusText(_("Writing to socket failed"));

}

void THISCLASS::SendLargeInteger(wxSocketBase *sock, int value)
{
	wxString buf;
	buf.Printf("%010d",value);
	sock->Write(buf.GetData(),10);
	if(sock->Error()) parent->SetStatusText(_("Writing to socket failed"));

}


