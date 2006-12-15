#ifndef _ObjectTrackerThread_h
#define _ObjectTrackerThread_h

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif
#include "wx/minifram.h"  // Imports the mini frame class (tracker and segmenter panel)
//#include <wx/html/helpctrl.h>
#include "wx/spinbutt.h"  // Spinbuttons 
#include "wx/valgen.h"	  // Generic validators, used in almost every dialgo
#include "wx/image.h"     // Necessary for image operations
#include "wx/file.h"	  // File operations (here: FileExists)
#include "wx/busyinfo.h"  // Imports the wxBusyInfo class (used on shutdown)
#include "wx/app.h"	      // Provides app functions (setvendorname...)
#include "wx/thread.h"	  // Provides threads

//#include "calibration.h"
#include "ObjectTracker.h"
#ifdef _1394
#include <1394camera.h>
#endif

class SwisTrack;
class Canvas;
class SwisTrackPanel;
class InterceptionPanel;
class AviWriter;
class SocketServer;
class NewExperimentDialog;
class GuiApp;


class ObjectTrackerThread: public wxThread {
public:
	ObjectTrackerThread(ObjectTracker* _ot, wxCriticalSection* _cs, SwisTrack* _st);
	virtual ~ObjectTrackerThread();
	virtual void* Entry();
protected:
	// void Update();
	// Object tracker pointer
	ObjectTracker* ot;
	// Critical section pointer
	wxCriticalSection* cs;
	SwisTrack* st;

private:
	// Forbids copy
	ObjectTrackerThread(ObjectTrackerThread & that);
};

#endif