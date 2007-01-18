#ifndef HEADER_CanvasPanel
#define HEADER_CanvasPanel

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif

class CanvasPanel;

#include "SwisTrack.h"
#include "DisplayImageSubscriberInterface.h"

/** \class CanvasPanel
* \brief The panel containing the canvas and the annotations
*/
class CanvasPanel: public wxPanel, public DisplayImageSubscriberInterface {

private:
	SwisTrack *mSwisTrack;
	Canvas *mCanvas;
	wxStaticText mTitle;
	wxStaticText mAnnotation1;
	wxStaticText mAnnotation2;
	wxStaticText mAnnotation3;
	wxMenu *mMenu;
  
public:
	CanvasPanel(SwisTrack *st);
	~CanvasPanel();

	void OnMouseLeftDown(wxMouseEvent &event);
	void OnMenu(wxCommandEvent& event);

	// DisplayImageSubscriberInterface methods
	void OnDisplayImageChanged(DisplayImage *di);

	DECLARE_EVENT_TABLE()
};

#endif
