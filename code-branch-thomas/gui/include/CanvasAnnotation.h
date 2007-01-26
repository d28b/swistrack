#ifndef HEADER_CanvasAnnotation
#define HEADER_CanvasAnnotation

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

class CanvasAnnotation;

#include <wx/dcbuffer.h>
#include <cv.h>
#include "SwisTrack.h"
#include "CanvasPanel.h"

//! The control below the main canvas, displaying information about the image.
class CanvasAnnotation: public wxControl {

public:
	//! The associated CanvasPanel.
	CanvasPanel* mCanvasPanel;
	//! The text shown on the left side.
	wxString mTextLeft;
	//! The text shown on the right side.
	wxString mTextRight;

	// Constructor.
	CanvasAnnotation(CanvasPanel *cp);
	// Destructor.
	~CanvasAnnotation();

	//! Handles left click.
	void OnMouseLeftClick(wxMouseEvent &event);
	//! Handles right click.
	void OnMouseRightClick(wxMouseEvent &event);
	//! Handles mouse move.
	void OnMouseMove(wxMouseEvent &event);
	//! Paints the canvas.
	void OnPaint(wxPaintEvent& WXUNUSED(event));

	DECLARE_EVENT_TABLE()
};

#endif
