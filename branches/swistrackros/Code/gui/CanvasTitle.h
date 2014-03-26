#ifndef HEADER_CanvasTitle
#define HEADER_CanvasTitle

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class CanvasTitle;

#include <wx/dcbuffer.h>
#include "SwisTrack.h"
#include "CanvasPanel.h"
#include "Display.h"

//! The control above the main canvas, showing the title of the image.
class CanvasTitle: public wxControl {

public:
	//! The associated CanvasPanel.
	CanvasPanel* mCanvasPanel;
	//! The title text (displayed on the left).
	wxString mTitle;
	//! The text on the right.
	wxString mTextRight;
	//! Whether the title is highlighted (during mouse over) or not.
	bool mHighlight;
	//! The popup menu.
	wxMenu *mMenu;

	// Constructor.
	CanvasTitle(CanvasPanel *cp);
	// Destructor.
	~CanvasTitle();

	//! Sets the text.
	void SetText(const wxString &title, const wxString &textright);

private:
	//! The corresponding GUI event handler.
	void OnMouseEnter(wxMouseEvent &event);
	//! The corresponding GUI event handler.
	void OnMouseLeave(wxMouseEvent &event);
	//! The corresponding GUI event handler.
	void OnMouseLeftDown(wxMouseEvent &event);
	//! The corresponding GUI event handler.
	void OnMouseRightDown(wxMouseEvent &event);
	//! The corresponding GUI event handler.
	void OnPaint(wxPaintEvent& WXUNUSED(event));
	//! The corresponding GUI event handler.
	void OnMenu(wxCommandEvent& event);

	//! Returns the Display associated with the index.
	Display *GetDisplay(int selid);

	DECLARE_EVENT_TABLE()
};

#endif
