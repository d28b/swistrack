#ifndef HEADER_Canvas
#define HEADER_Canvas

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

class Canvas;

#include <wx/dcbuffer.h>
#include "SwisTrack.h"
#include <cv.h>

/** \class Canvas
* \brief Shows an intermediate step on the screen.
*
* This class implements an OnPaint method and an event handler
* for mouse clicks on the canvas.
*/
class Canvas: public wxControl {

public:
	CanvasPanel* mCanvasPanel;	//!< The associated CanvasPanel.
	wxSize mViewSize;			//!< The maximal size that the canvas should have.
	wxPoint mViewOffset;		//!< The current offset.

	// Constructor.
	Canvas(CanvasPanel *cp);
	// Destructor.
	~Canvas();

	//! Sets the display object.
	bool SetDisplay(Display *display);

private:
	enum {
		sID_SaveViewImageAs,
		sID_SaveOriginalImageAs,
		sID_FlipVertically,
		sID_FlipHorizontally,
		sID_Zoom200,
		sID_Zoom100,
		sID_Zoom50,
		sID_Zoom25,
		sID_Zoom10,
		sID_ZoomFit
	};

	wxMenu mPopupMenu;					//!< The popup menu.
	DisplayRenderer mDisplayRenderer;	//!< The display renderer. This renderer also handles scaling and flipping.

	wxPoint mMoveStartPoint;	//!< The starting point when moving the image.

	//! Paints the image in the canvas.
	bool OnPaintImage(wxPaintDC &dc);
	//! Returns the maximal size the image would take.
	wxSize THISCLASS::GetMaximumSize();
	//! Updates the view.
	void UpdateView();

	//! The corresponding GUI event handler.
	void OnMenuSaveViewImageAs(wxCommandEvent& event);
	//! The corresponding GUI event handler.
	void OnMenuSaveOriginalImageAs(wxCommandEvent& event);
	//! The corresponding GUI event handler.
	void OnMenuFlipVertically(wxCommandEvent& event);
	//! The corresponding GUI event handler.
	void OnMenuFlipHorizontally(wxCommandEvent& event);
	//! The corresponding GUI event handler.
	void OnMenuZoom(wxCommandEvent& event);

	//! Overwritten to avoid painting the background.
	void OnEraseBackground(wxEraseEvent& event);
	//! The corresponding GUI event handler.
	void OnMouseLeftDown(wxMouseEvent &event);
	//! The corresponding GUI event handler.
	void OnMouseRightDown(wxMouseEvent &event);
	//! The corresponding GUI event handler.
	void OnMouseMove(wxMouseEvent &event);
	//! The corresponding GUI event handler.
	void OnPaint(wxPaintEvent& WXUNUSED(event));
	//! The corresponding GUI event handler.
	void OnSize(wxSizeEvent &event);

	DECLARE_EVENT_TABLE()
};

#endif
