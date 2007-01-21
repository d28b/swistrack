#ifndef HEADER_CanvasTitle
#define HEADER_CanvasTitle

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

class CanvasTitle;

#include <wx/dcbuffer.h>
#include "SwisTrack.h"
#include "cv.h"

//! The control above the main canvas, showing the title of the image.
class CanvasTitle: public wxControl {

public:
	//! The associated CanvasTitlePanel.
	CanvasTitlePanel* mCanvasTitlePanel;
	//! The title text.
	wxString mTitle;
	//! Whether the title is highlighted (during mouse over) or not.
	bool mHighlight;
	//! The popup menu.
	wxMenu *mMenu;

	// Constructor.
	CanvasTitle(CanvasTitlePanel *cp);
	// Destructor.
	~CanvasTitle();

private:
	//! The corresponding GUI event handler.
	void OnMouseLeftDown(wxMouseEvent &event);
	//! The corresponding GUI event handler.
	void OnMouseRightDown(wxMouseEvent &event);
	//! The corresponding GUI event handler.
	void OnPaint(wxPaintEvent& WXUNUSED(event));
	//! The corresponding GUI event handler.
	void OnMenu(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()
};

#endif
