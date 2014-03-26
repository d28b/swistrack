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

	//! Sets the text.
	void SetText(const wxString &textleft, const wxString &textright);

private:
	//! The corresponding GUI event handler.
	void OnMouseLeftClick(wxMouseEvent &event);
	//! The corresponding GUI event handler.
	void OnMouseRightClick(wxMouseEvent &event);
	//! The corresponding GUI event handler.
	void OnMouseMove(wxMouseEvent &event);
	//! The corresponding GUI event handler.
	void OnPaint(wxPaintEvent& WXUNUSED(event));

	DECLARE_EVENT_TABLE()
};

#endif
