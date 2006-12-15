#ifndef _SOCKETSERVER_H
#define _SOCKETSERVER_H

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif

#include "wx/socket.h"

class SwisTrack;

class SocketServer : public wxEvtHandler
{
public:
	void SendLargeInteger(wxSocketBase *sock, int value);
	void SendSmallFloat(wxSocketBase *sock, double value);
	void SendNumberofTracks(wxSocketBase *sock);
	void SendNumberofBlobs(wxSocketBase *sock);
	void SendTracks(wxSocketBase *sock);
  SocketServer(SwisTrack* parent,int socketnumber);
  ~SocketServer();

  // event handlers (these functions should _not_ be virtual)
  void OnServerEvent(wxSocketEvent& event);
  void OnSocketEvent(wxSocketEvent& event);

  void SendBlobs(wxSocketBase *sock);
  void SendTrajectories(wxSocketBase *sock);
  
private:
  wxSocketServer *m_server;
  bool            m_busy;
  int             m_numClients;
  SwisTrack*	  parent;
  int			  calibration;

  // any class wishing to process wxWidgets events must use this macro
  DECLARE_EVENT_TABLE()
};

// --------------------------------------------------------------------------
// constants
// --------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
  // menu items
  SERVER_QUIT = wxID_EXIT,
  SERVER_ABOUT = wxID_ABOUT,

  // id for sockets
  SERVER_ID = 100,
  SOCKET_ID
};

#endif
