#ifndef HEADER_CanvasPanel
#define HEADER_CanvasPanel

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

class CanvasPanel;

#include <wx/panel.h>
#include <wx/stattext.h>
#include "SwisTrack.h"
#include "Canvas.h"
#include "CanvasTitle.h"
#include "CanvasAnnotation.h"
#include "DisplayImageSubscriberInterface.h"

/** \class CanvasPanel
* \brief The panel containing the canvas and the annotations.
*/
class CanvasPanel: public wxPanel, public DisplayImageSubscriberInterface {

public:
	SwisTrack *mSwisTrack;			//!< The associated SwisTrack object.

	//! Constructor.
	CanvasPanel(wxWindow *parent, SwisTrack *st);
	//! Destructor.
	~CanvasPanel();

	//! Sets a new DisplayImage object.
	void SetDisplayImage(DisplayImage *di);

	// DisplayImageSubscriberInterface methods
	void OnDisplayImageSubscribe(DisplayImage *di);
	void OnDisplayImageChanged(DisplayImage *di);
	void OnDisplayImageUnsubscribe(DisplayImage *di);

private:
	Canvas *mCanvas;						//!< The canvas.
	CanvasTitle *mCanvasTitle;				//!< The title box.
	CanvasAnnotation *mCanvasAnnotation;	//!< The annotation box.
	DisplayImage *mCurrentDisplayImage;		//!< The DisplayImage currently subscribed to.

	int mUpdateRate;				//!< The update rate (only each mUpdateRate image is displayed). If mUpdateRate==0, visualization is disabled.
	int mUpdateCounter;				//!< The update rate counter.
	wxSize mAvailableSpace;			//!< The available space for the image.

	//! The corresponding GUI event handler.
	void OnMouseLeftDown(wxMouseEvent &event);
	//! The corresponding GUI event handler.
	void OnSize(wxSizeEvent &event);

	DECLARE_EVENT_TABLE()
};

#endif
