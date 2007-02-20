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
	bool mFlipVertically;		//! Whether the image should be flipped vertically (TODO not implemented).
	bool mFlipHorizontally;		//! Whether the image should be flipped horizontally (TODO not implemented).

	// Constructor.
	Canvas(CanvasPanel *cp);
	// Destructor.
	~Canvas();

	//! This sets a new image. Note that this class cleans up and frees old image automatically.
	void SetImage(IplImage *img);
	//! This returns the current image.
	IplImage *GetImage() {return mImage;}

private:
	enum {
		sID_SaveImageAs
	};

	IplImage *mImage;			//!< The current image.
	wxString mDisplayError;	//!< The error displayed if no image can be drawn.
	wxMenu mPopupMenu;			//!< The popup menu.

	//! The corresponding GUI event handler.
	void OnMenuSaveImageAs(wxCommandEvent& event);
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
