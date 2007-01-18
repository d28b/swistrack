#include "Application.h"
#define THISCLASS Application

#include <wx/app.h>

/** \file Application.cpp
* \brief Mainfile for the project
*
* This is the projects main file, where execution starts.
* Code execution starts in GUIApp::OnInit() that instantiates SwisTrack that inherits from wxFrame 
* Menu calls for the functions provided by the ObjectTracker class are provided, also event handler that capture
* mouse clicks and display coordinates in image and world space (after calibration).
*
* \section visualstudio Project Settings for Visual Studio 2005
* This application was written using Microsoft Visual Studio 2005. In order to successfully link,
* the following project settings are necessary:
*
* \subsection codegen C++/Code Generation Tab
* Set to multithreaded DLL. wxWidgets as well as all other libraries are compiled using this 
* setting, using statical linking yields multiple object definitions.
* 
* \subsection libs Additional Libraries
* You need the following libraries: wxmsw26d_html.lib kernel32.lib wxmsw26d_core.lib wxbase26d.lib 
* wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib 
* gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib
* rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib odbc32.lib 
*
*/

IMPLEMENT_APP(Application)

bool THISCLASS::OnInit() {
	// Set some main application parameters.
	SetVendorName(wxT("Swarm-Intelligent Systems Group, EPFL, Lausanne, Switzerland"));
	SetAppName(wxT("SwisTrack")); 

	// Create the main application window
	SwisTrack *st = new SwisTrack(_T("SWISTrack 4.0.0"), wxPoint(-1, -1), wxSize(800, 600));

#if defined(__WIN16__) || defined(__WXMOTIF__)
	int width, height;
	frame->GetSize(& width, & height);
	frame->SetSize(-1, -1, width, height);
#endif

	// Show
    st->Show(TRUE);
	SetTopWindow(st);
    return TRUE;
}
