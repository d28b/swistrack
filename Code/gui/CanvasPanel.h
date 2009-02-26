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
#include "DisplaySubscriberInterface.h"

/** \class CanvasPanel
* \brief The panel containing the canvas and the annotations.
*/
class CanvasPanel: public wxPanel, public DisplaySubscriberInterface {

public:
	SwisTrack *mSwisTrack;			//!< The associated SwisTrack object.
	Display *mCurrentDisplay;		//!< The Display currently subscribed to.
	wxSize mAvailableSpace;			//!< The available space for the image.
	int mUpdateRate;				//!< The update rate (only each mUpdateRate image is displayed). If mUpdateRate==0, visualization is disabled.

	//! Constructor.
	CanvasPanel(wxWindow *parent, SwisTrack *st);
	//! Destructor.
	~CanvasPanel();

	//! Sets a new Display object.
	void SetDisplay(Display *di);

	// DisplaySubscriberInterface methods
	void OnDisplaySubscribe(Display *di);
	void OnDisplayUnsubscribe(Display *di);
	void OnDisplayBeforeStep(Display *di);
	void OnDisplayChanged(Display *di);

	//! Recalculates and sets the size.
	void UpdateSize();

private:
	Canvas *mCanvas;						//!< The canvas.
	CanvasTitle *mCanvasTitle;				//!< The title box.
	CanvasAnnotation *mCanvasAnnotation;	//!< The annotation box.

	int mUpdateStepCounter;			//!< The update rate counter in number of steps.

	//! The corresponding GUI event handler.
	void OnSize(wxSizeEvent &event);

	DECLARE_EVENT_TABLE()
};

#endif
