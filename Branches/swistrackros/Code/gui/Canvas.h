#ifndef HEADER_Canvas
#define HEADER_Canvas

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class Canvas;

#include <wx/dcbuffer.h>
#include "SwisTrack.h"
#include "DisplayRenderer.h"
#include "WriteAvi.h"
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
	void SetDisplay(Display *display);
	//! Updates the display.
	void OnDisplayChanged();
	//! Returns the maximal size the image would take.
	wxSize GetMaximumSize();

private:
	enum eID {
		cID_SaveViewImageAs = 1,
		cID_SaveOriginalImageAs,
		cID_FlipVertically,
		cID_FlipHorizontally,
		cID_Zoom200,
		cID_Zoom100,
		cID_Zoom50,
		cID_Zoom25,
		cID_Zoom10,
		cID_ZoomFit,
		cID_UpdateRate0,
		cID_UpdateRate1,
		cID_UpdateRate2,
		cID_UpdateRate4,
		cID_UpdateRate8,
		cID_UpdateRate16,
		cID_UpdateRate32,
		cID_UpdateRate64,
		cID_UpdateRate128,
	};

	wxMenu mPopupMenu;					//!< The popup menu.
	DisplayRenderer mDisplayRenderer;	//!< The display renderer. This renderer also handles scaling and flipping.

	bool mMoveStarted;					//!< True while we are in move mode.
	wxPoint mMoveStartPoint;			//!< The starting point when moving the image.

	WriteAvi mWriter;

	//! Paints the image in the canvas.
	bool OnPaintImage(wxPaintDC &dc);
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
	//! The corresponding GUI event handler.
	void OnMenuUpdateRate(wxCommandEvent& event);

	//! Overwritten to avoid painting the background.
	void OnEraseBackground(wxEraseEvent& event);
	//! The corresponding GUI event handler.
	void OnMouseLeftDown(wxMouseEvent &event);
	//! The corresponding GUI event handler.
	void OnMouseLeftUp(wxMouseEvent &event);
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
