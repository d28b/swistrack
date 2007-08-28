#ifndef HEADER_RunThread
#define HEADER_RunThread

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif

#include <wx/thread.h>
#include "SwisTrack.h"

class RunThread: public wxThread {

public:
	RunThread(SwisTrack* st);
	virtual ~RunThread();
	virtual void* Entry();

protected:
	//! The associated SwisTrack object.
	SwisTrack* mSwisTrack;
};

#endif