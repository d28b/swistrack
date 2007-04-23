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
* \brief Creates a canvas to draw tracking progress
*
* This class implements an OnPaint method and an event handler
* for mouse clicks on the canvas.
*/
class Canvas: public wxControl {

public:
	CanvasPanel* mCanvasPanel;	//! The associated CanvasPanel.
	bool mViewFlipVertically;	//! Whether the image should be flipped vertically (TODO not implemented).
	bool mViewFlipHorizontally;	//! Whether the image should be flipped horizontally (TODO not implemented).
	wxSize mViewSize;			//! The maximal size that the canvas should have.
	float mViewScalingFactor;	//! The scaling factor.
	wxPoint mViewOffset;		//! The offset.

	// Constructor.
	Canvas(CanvasPanel *cp);
	// Destructor.
	~Canvas();

	//! Paints the image in the canvas.
	bool SetDisplay(Display *di);

private:
	enum {
		sID_SaveViewImageAs,
		sID_SaveOriginalImageAs,
		sID_FlipVertically,
		sID_FlipHorizontally,
		sID_Zoom200,
		sID_Zoom100,
		sID_Zoom50,
		sID_ZoomFit
	};

	IplImage *mImage;			//!< The current image.
	wxMenu mPopupMenu;			//!< The popup menu.
	DisplayRenderer *mDisplayRenderer;	//!< The display renderer.

	wxPoint mMoveStartPoint;	//!< The starting point when moving the image.

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
