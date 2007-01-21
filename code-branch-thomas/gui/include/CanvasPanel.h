#ifndef HEADER_CanvasPanel
#define HEADER_CanvasPanel

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

class CanvasPanel;

#include <wx/stattext.h>
#include "SwisTrack.h"
#include "Canvas.h"
#include "DisplayImageSubscriberInterface.h"

/** \class CanvasPanel
* \brief The panel containing the canvas and the annotations.
*/
class CanvasPanel: public wxPanel, public DisplayImageSubscriberInterface {

public:
	//! The associated SwisTrack object.
	SwisTrack *mSwisTrack;
	//! The canvas.
	Canvas *mCanvas;
	//! The title text.
	wxStaticText *mTitle;
	//! The update rate (only each mUpdateRate image is displayed). If mUpdateRate==0, visualization is disabled.
	int mUpdateRate;
	//! The update rate counter.
	int mUpdateCounter;

	//! Constructor.
	CanvasPanel(SwisTrack *st);
	//! Destructor.
	~CanvasPanel();

	//! The corresponding GUI event handler.
	void OnMouseLeftDown(wxMouseEvent &event);

	// DisplayImageSubscriberInterface methods
	void OnDisplayImageChanged(DisplayImage *di);

	DECLARE_EVENT_TABLE()
};

#endif
