/////////////////////////////////////////////////////////////////////////////
// Name:        client.cpp
// Purpose:     Client for SwisTrack demo
// Author:      Nikolaus Correll
// Modified from: wxClientDemo by Guillermo Rodriguez Garcia (c) 1999
/////////////////////////////////////////////////////////////////////////////

// ==========================================================================
// declarations
// ==========================================================================

// --------------------------------------------------------------------------
// headers
// --------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma implementation
    #pragma interface
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#  pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
#  include "wx/wx.h"
#endif

#include "wx/socket.h"
#include "wx/url.h"
#include "wx/wfstream.h"


// --------------------------------------------------------------------------
// classes
// --------------------------------------------------------------------------

// Define a new application type
class MyApp : public wxApp
{
public:
  virtual bool OnInit();
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
	void OnClientStop(wxCommandEvent& WXUNUSED(event));
  MyFrame();
  ~MyFrame();

  // event handlers for File menu
  void OnQuit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);

  // event handlers for Socket menu
  void OnOpenConnection(wxCommandEvent& event);
  void OnTest(wxCommandEvent& event);
  void OnCloseConnection(wxCommandEvent& event);
  void OnClientStart(wxCommandEvent& WXUNUSED(event));

  // socket event handler
  void OnSocketEvent(wxSocketEvent& event);

  // convenience functions
  void UpdateStatusBar();

private:
  wxSocketClient *m_sock;
  wxTextCtrl     *m_text;
  wxMenu         *m_menuFile;
  wxMenu         *m_menuSocket;
  wxMenu         *m_menuDatagramSocket;
  wxMenu         *m_menuProtocols;
  wxMenuBar      *m_menuBar;
  bool            m_busy;

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
  CLIENT_QUIT = wxID_EXIT,
  CLIENT_ABOUT = wxID_ABOUT,
  CLIENT_OPEN = 100,
  CLIENT_GETTRACK,
  CLIENT_CLOSE,
  CLIENT_START,
  CLIENT_STOP,
  // id for socket
  SOCKET_ID
};

// --------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// --------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_MENU(CLIENT_QUIT,     MyFrame::OnQuit)
  EVT_MENU(CLIENT_ABOUT,    MyFrame::OnAbout)
  EVT_MENU(CLIENT_OPEN,     MyFrame::OnOpenConnection)
  EVT_MENU(CLIENT_START,    MyFrame::OnClientStart)
  EVT_MENU(CLIENT_GETTRACK, MyFrame::OnTest)
  EVT_MENU(CLIENT_STOP,		MyFrame::OnClientStop)
  EVT_MENU(CLIENT_CLOSE,    MyFrame::OnCloseConnection)
  EVT_SOCKET(SOCKET_ID,     MyFrame::OnSocketEvent)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

// ==========================================================================
// implementation
// ==========================================================================

// --------------------------------------------------------------------------
// the application class
// --------------------------------------------------------------------------

bool MyApp::OnInit()
{
  // Create the main application window
  MyFrame *frame = new MyFrame();

  // Show it and tell the application that it's our main window
  frame->Show(true);
  SetTopWindow(frame);

  // success
  return true;
}

// --------------------------------------------------------------------------
// main frame
// --------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame() : wxFrame((wxFrame *)NULL, wxID_ANY,
                             _("SwisTrack demo client"),
                             wxDefaultPosition, wxSize(300, 200))
{
  // Give the frame an icon
  SetIcon(wxICON(connect));

  // Make menus
  m_menuFile = new wxMenu();
  m_menuFile->Append(CLIENT_ABOUT, _("&About...\tCtrl-A"), _("Show about dialog"));
  m_menuFile->AppendSeparator();
  m_menuFile->Append(CLIENT_QUIT, _("E&xit\tAlt-X"), _("Quit client"));

  m_menuSocket = new wxMenu();
  m_menuSocket->Append(CLIENT_OPEN, _("&Open session"), _("Connect to server"));
  m_menuSocket->AppendSeparator();
  m_menuSocket->Append(CLIENT_START, _("Start Tracker"),_("Starts tracking"));
  m_menuSocket->Append(CLIENT_GETTRACK, _("Test Trajectory"), _("Retrieves trajectory data"));
  m_menuSocket->Append(CLIENT_STOP,_("Stop Tracker"),_("Stops tracking"));
  m_menuSocket->AppendSeparator();
  m_menuSocket->Append(CLIENT_CLOSE, _("&Close session"), _("Close connection"));

 
  // Append menus to the menubar
  m_menuBar = new wxMenuBar();
  m_menuBar->Append(m_menuFile, _("&File"));
  m_menuBar->Append(m_menuSocket, _("&SocketClient"));
  SetMenuBar(m_menuBar);

#if wxUSE_STATUSBAR
  // Status bar
  CreateStatusBar(2);
#endif // wxUSE_STATUSBAR

  // Make a textctrl for logging
  m_text  = new wxTextCtrl(this, wxID_ANY,
                           _("Welcome to the SwisTrack client demo\n"),
                           wxDefaultPosition, wxDefaultSize,
                           wxTE_MULTILINE | wxTE_READONLY);

  // Create the socket
  m_sock = new wxSocketClient();

  // Setup the event handler and subscribe to most events
  m_sock->SetEventHandler(*this, SOCKET_ID);
  m_sock->SetNotify(wxSOCKET_CONNECTION_FLAG |
                    wxSOCKET_INPUT_FLAG |
                    wxSOCKET_LOST_FLAG);
  m_sock->Notify(true);

  m_busy = false;
  UpdateStatusBar();
}

MyFrame::~MyFrame()
{
  // No delayed deletion here, as the frame is dying anyway
  delete m_sock;
}

// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
  // true is to force the frame to close
  Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
  wxMessageBox(_("SwisTrack client demo\n(c) 2005 Nikolaus Correll\n"),
               _("About Client"),
               wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnOpenConnection(wxCommandEvent& WXUNUSED(event))
{
  wxIPV4address addr;

  m_menuSocket->Enable(CLIENT_OPEN, false);
  m_menuSocket->Enable(CLIENT_CLOSE, false);

  // Ask user for server address
  wxString hostname = wxGetTextFromUser(
    _("Enter the address of SwisTrack server:"),
    _("Connect ..."),
    _("localhost"));

  addr.Hostname(hostname);
  addr.Service(3000);

  // Mini-tutorial for Connect() :-)
  // ---------------------------
  //
  // There are two ways to use Connect(): blocking and non-blocking,
  // depending on the value passed as the 'wait' (2nd) parameter.
  //
  // Connect(addr, true) will wait until the connection completes,
  // returning true on success and false on failure. This call blocks
  // the GUI (this might be changed in future releases to honour the
  // wxSOCKET_BLOCK flag).
  //
  // Connect(addr, false) will issue a nonblocking connection request
  // and return immediately. If the return value is true, then the
  // connection has been already successfully established. If it is
  // false, you must wait for the request to complete, either with
  // WaitOnConnect() or by watching wxSOCKET_CONNECTION / LOST
  // events (please read the documentation).
  //
  // WaitOnConnect() itself never blocks the GUI (this might change
  // in the future to honour the wxSOCKET_BLOCK flag). This call will
  // return false on timeout, or true if the connection request
  // completes, which in turn might mean:
  //
  //   a) That the connection was successfully established
  //   b) That the connection request failed (for example, because
  //      it was refused by the peer.
  //
  // Use IsConnected() to distinguish between these two.
  //
  // So, in a brief, you should do one of the following things:
  //
  // For blocking Connect:
  //
  //   bool success = client->Connect(addr, true);
  //
  // For nonblocking Connect:
  //
  //   client->Connect(addr, false);
  //
  //   bool waitmore = true;
  //   while (! client->WaitOnConnect(seconds, millis) && waitmore )
  //   {
  //     // possibly give some feedback to the user,
  //     // update waitmore if needed.
  //   }
  //   bool success = client->IsConnected();
  //
  // And that's all :-)

  m_text->AppendText(_("\nTrying to connect (timeout = 10 sec) ...\n"));
  m_sock->Connect(addr, false);
  m_sock->WaitOnConnect(10);

  if (m_sock->IsConnected())
    m_text->AppendText(_("Succeeded ! Connection established\n"));
  else
  {
    m_sock->Close();
    m_text->AppendText(_("Failed ! Unable to connect\n"));
    wxMessageBox(_("Can't connect to the specified host"), _("Alert !"));
  }

  UpdateStatusBar();
}

void MyFrame::OnTest(wxCommandEvent& WXUNUSED(event))
{
 
  // Disable socket menu entries (exception: Close Session)
  m_busy = true;
  UpdateStatusBar();

  m_text->AppendText(_("Getting Trajectory data\n"));

  // Tell the server which test we are running
  unsigned char c = 'T';
  m_sock->Write(&c, 1);

  m_busy = false;
  UpdateStatusBar();
}


void MyFrame::OnCloseConnection(wxCommandEvent& WXUNUSED(event))
{
  m_sock->Close();
  UpdateStatusBar();
}


void MyFrame::OnSocketEvent(wxSocketEvent& event)
{
  wxString s = _("OnSocketEvent: ");

  switch(event.GetSocketEvent())
  {
    case wxSOCKET_INPUT : 	
		{
		int loopcond=1;
		while(loopcond)
		{
		 char buf[22];
		 wxString test;
		 m_sock->Read(buf,22);
		
		 if(m_sock->LastCount()==22)
		 {
 			float x,y;
			sscanf(buf,"%f %f",&x,&y);
			test.Printf("%f %f\n",x,y);
			s.Append(test);
		 }
		 else loopcond=0;
		} 
		}
		break;
	
    case wxSOCKET_LOST       : s.Append(_("wxSOCKET_LOST\n")); break;
    case wxSOCKET_CONNECTION : s.Append(_("wxSOCKET_CONNECTION\n")); break;
    default                  : s.Append(_("Unexpected event !\n")); break;
  }

  m_text->AppendText(s);
  UpdateStatusBar();
}

// convenience functions

void MyFrame::UpdateStatusBar()
{
  wxString s;

  if (!m_sock->IsConnected())
  {
    s.Printf(_("Not connected"));
  }
  else
  {
    wxIPV4address addr;

    m_sock->GetPeer(addr);
    s.Printf(_("%s : %d"), (addr.Hostname()).c_str(), addr.Service());
  }

#if wxUSE_STATUSBAR
  SetStatusText(s, 1);
#endif // wxUSE_STATUSBAR

  m_menuSocket->Enable(CLIENT_OPEN, !m_sock->IsConnected() && !m_busy);
  m_menuSocket->Enable(CLIENT_GETTRACK, m_sock->IsConnected() && !m_busy);
  m_menuSocket->Enable(CLIENT_CLOSE, m_sock->IsConnected());
}

void MyFrame::OnClientStart(wxCommandEvent& WXUNUSED(event))
{
  // Disable socket menu entries (exception: Close Session)
  m_busy = true;
  UpdateStatusBar();

  m_text->AppendText(_("Start Tracker...\n"));

  // Tell the server which test we are running
  unsigned char c = 'R';
  m_sock->Write(&c, 1);

  m_busy = false;
  UpdateStatusBar();
}

void MyFrame::OnClientStop(wxCommandEvent& WXUNUSED(event))
{
  // Disable socket menu entries (exception: Close Session)
  m_busy = true;
  UpdateStatusBar();

  m_text->AppendText(_("Stops Tracker...\n"));

  // Tell the server which test we are running
  unsigned char c = '!';
  m_sock->Write(&c, 1);

  m_busy = false;
  UpdateStatusBar();
}
