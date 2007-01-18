#ifndef HEADER_CanvasPanel
#define HEADER_CanvasPanel

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif

class CanvasPanel;

#include "SwisTrack.h"

/** \class CanvasPanel
* \brief The panel containing the canvas and the annotations
*/
class CanvasPanel: public wxPanel {

private:
	SwisTrack* mSwisTrack;
	Canvas *mCanvas;
	wxStaticText mAnnotation1;
	wxStaticText mAnnotation2;
	wxStaticText mAnnotation3;
  
public:
	CanvasPanel(SwisTrack *st);
	~CanvasPanel();

	void OnMouseLeftDown(wxMouseEvent &event);

	DECLARE_EVENT_TABLE()
};

#endif
