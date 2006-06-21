// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

/**

\todo One step back:

- pop_back all trajectories
- move the file pointers back one line (data, calib, mask)
- go back in the video by one frame

*/

#define USE_HTML_HELP 1

#include "wx/app.h"	      // Provides app functions (setvendorname...)
#include "GUIApp.h"

/** \file gui.cpp
* \brief Mainfile for the project
*
* This is the projects main file, where execution starts.
* Code execution starts in GUIApp::OnInit() that instantiates SwisTrack that inherits from wxFrame 
* Menu calls for the functions provided by the ObjectTracker class are provided, also event handler that capture
* mouse clicks and display coordinates in image and world space (after calibration).
*
* \section visualstudio Project Settings for Visual Studio 6.0
* This application was written using Microsoft Visual Studio 6.0. In order to successfully link,
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
* The librares objecttracker.lib and calibration.lib have to be compiled using their own work-
* spaces.
*/


// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. GUIApp and
// not wxApp)
IMPLEMENT_APP(GUIApp)

// 'Main program' equivalent: the program execution "starts" here
/** \brief Application root - program execution starts here
*
* Instantiantes SwisTrack and displays the main window.
*/
bool GUIApp::OnInit()
{
   // create the main application window
    frame = new SwisTrack(_T("SWISTrack v3.0.0"),
                                 wxPoint(-1, -1), wxSize(660, 480));

	SetVendorName(wxT("Swarm-Intelligent Systems Group"));
    SetAppName(wxT("SwisTrack")); 

#if defined(__WIN16__) || defined(__WXMOTIF__)
    int width, height;
    frame->GetSize(& width, & height);
    frame->SetSize(-1, -1, width, height);
#endif

 // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(TRUE);
	SetTopWindow(frame);
    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned FALSE here, the
    // application would exit immediately.
    return TRUE;
}
