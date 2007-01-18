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

private:
	 CanvasPanel* mCanvasPanel;
	 IplImage *mImage;
  
public:
	Canvas(CanvasPanel *cp);

	void EraseBackground(wxEraseEvent& event);
	void OnMouseClick(wxMouseEvent &event);
	void OnMouseMove(wxMouseEvent &event);

	void OnPaint(wxPaintEvent& WXUNUSED(event));

	//! This sets a new image. Note that this class cleans up and frees old image automatically.
	void SetImage(IplImage *img);

	DECLARE_EVENT_TABLE()
};

#endif
