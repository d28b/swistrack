
#include "SocketServer.h"
#include "SwisTrack.h"

BEGIN_EVENT_TABLE(SocketServer, wxEvtHandler)
  EVT_SOCKET(SERVER_ID,  SocketServer::OnServerEvent)
  EVT_SOCKET(SOCKET_ID,  SocketServer::OnSocketEvent)
END_EVENT_TABLE()


SocketServer::SocketServer(SwisTrack* parent,int socketnumber)
{
  this->parent=parent;
 
  calibration=0;
 
  // Create the address - defaults to localhost:0 initially
  wxIPV4address addr;
  addr.Service(socketnumber);

  // Create the socket
  m_server = new wxSocketServer(addr);

  // We use Ok() here to see if the server is really listening
  if (! m_server->Ok())
  {
    parent->SetStatusText("Could not listen at the specified port !\n\n");
    return;
  }
  else
  {
	wxString temp;
	temp.Printf("Server listening at port %d",socketnumber);
	parent->SetStatusText(temp);
  }

  // Setup the event handler and subscribe to connection events
  m_server->SetEventHandler(*this, SERVER_ID);
  m_server->SetNotify(wxSOCKET_CONNECTION_FLAG);
  m_server->Notify(true);

  m_busy = false;
  m_numClients = 0;
}

SocketServer::~SocketServer()
{
  // No delayed deletion here, as the frame is dying anyway
  m_server->Destroy();
  delete m_server;
}

void SocketServer::OnServerEvent(wxSocketEvent& event)
{
  wxIPV4address addr;
  wxString s = _("");
  wxSocketBase *sock;

  switch(event.GetSocketEvent())
  {
    case wxSOCKET_CONNECTION : s.Append(_("Incoming socket connection\n")); break;
    default                  : s.Append(_("Unexpected socket event !\n")); break;
  }

  parent->SetStatusText(s);
 
  // Accept new connection if there is one in the pending
  // connections queue, else exit. We use Accept(false) for
  // non-blocking accept (although if we got here, there
  // should ALWAYS be a pending connection).

  sock = m_server->Accept(false);

  if (sock)
  {
	wxString temp;
	sock->GetPeer(addr);
	temp.Printf("Connection with %s (%s) established",(addr.Hostname()).c_str(),(addr.IPAddress()).c_str());
    parent->SetStatusText(temp);
  }
  else
  {
    parent->SetStatusText(_("Error: couldn't accept a new connection\n\n"));
    return;
  }

  sock->SetEventHandler(*this, SOCKET_ID);
  sock->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
  sock->Notify(true);

  m_numClients++;
 }


void SocketServer::OnSocketEvent(wxSocketEvent& event)
{
  wxString s = _("OnSocketEvent: ");
  wxSocketBase *sock = event.GetSocket();
  wxCommandEvent dummy;
  
  
	

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

void SocketServer::SendBlobs(wxSocketBase *sock)
{
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

void SocketServer::SendTracks(wxSocketBase *sock)
{
  unsigned int len,n,width;
  wxString buf;
  sock->SetFlags(wxSOCKET_WAITALL);
  n=parent->ot->GetNumberofTracks();
  width=(10+10+2);
  len=n*width;
  for(unsigned int i=0; i<n; i++){
	CvPoint2D32f* p;		
<<<<<<< .mine
=======
//	CvPoint2D32f pc;
>>>>>>> .r99

	p=parent->ot->GetTargetPos(i);
		
	buf.Printf("%s %+#010.4f %+#010.4f",buf.c_str(),p->x,p->y);
 }
  sock->Write(buf.GetData(),len);
  if(sock->Error()) parent->SetStatusText(_("Writing to socket failed"));  
}

void SocketServer::SendNumberofBlobs(wxSocketBase *sock)
{
	wxString buf;
	int n=parent->ot->GetNumberofParticles();
	buf.Printf("%04d",n);
	sock->Write(buf.GetData(),4);
	if(sock->Error()) parent->SetStatusText(_("Writing to socket failed"));
}

void SocketServer::SendNumberofTracks(wxSocketBase *sock)
{
	wxString buf;
	int n=parent->ot->GetNumberofTracks();
	buf.Printf("%04d",n);
	sock->Write(buf.GetData(),4);
	if(sock->Error()) parent->SetStatusText(_("Writing to socket failed"));
}

void SocketServer::SendSmallFloat(wxSocketBase *sock, double value)
{
	wxString buf;
	buf.Printf("%05.2f",value);
	sock->Write(buf.GetData(),5);
	if(sock->Error()) parent->SetStatusText(_("Writing to socket failed"));

}

void SocketServer::SendLargeInteger(wxSocketBase *sock, int value)
{
	wxString buf;
	buf.Printf("%010d",value);
	sock->Write(buf.GetData(),10);
	if(sock->Error()) parent->SetStatusText(_("Writing to socket failed"));

}


