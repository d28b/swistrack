#ifndef HEADER_Canvas
#define HEADER_Canvas

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif

class Canvas;

#include "wx/dcbuffer.h"
#include "SwisTrack.h"
#include "cv.h"

/** \class Canvas
* \brief Creates a canvas to draw tracking progress
*
* This class implements an OnPaint method and an event handler
* for mouse clicks on the canvas.
*/
class Canvas: public wxControl {

public:
	//! The associated CanvasPanel.
	CanvasPanel* mCanvasPanel;
	//! The current image.
	IplImage *mImage;
	//! Whether the image should be flipped vertically (TODO not implemented).
	IplImage *mFlipVertically;
	//! Whether the image should be flipped horizontally (TODO not implemented).
	IplImage *mFlipHorizontally;

	// Constructor.
	Canvas(CanvasPanel *cp);
	// Destructor.
	~Canvas();

	//! Overwritten to avoid painting the background.
	void EraseBackground(wxEraseEvent& event);
	//! Handles left click.
	void OnMouseLeftClick(wxMouseEvent &event);
	//! Handles right click.
	void OnMouseRightClick(wxMouseEvent &event);
	//! Handles mouse move.
	void OnMouseMove(wxMouseEvent &event);
	//! Paints the canvas.
	void OnPaint(wxPaintEvent& WXUNUSED(event));

	//! This sets a new image. Note that this class cleans up and frees old image automatically.
	void SetImage(IplImage *img);

	DECLARE_EVENT_TABLE()
};

#endif
