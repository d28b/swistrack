#ifndef HEADER_Canvas
#define HEADER_Canvas

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif

class Canvas;

#include "wx/dcbuffer.h"
#include "SwisTrack.h"

/** \class Canvas
* \brief Creates a canvas to draw tracking progress
*
* This class implements an OnPaint method and an event handler
* for mouse clicks on the canvas.
*/
class Canvas: public wxControl {

private:
	 SwisTrack* parent;
	 int status;
	 int dragging; 
  
public:
	Canvas(SwisTrack *parent, const wxPoint& pos, const wxSize& size);

	void EraseBackground(wxEraseEvent& event);
	void OnMouseClick(wxMouseEvent &event);
	void OnMouseMove(wxMouseEvent &event);

	void OnPaint(wxPaintEvent& WXUNUSED(event));

	DECLARE_EVENT_TABLE()
};

#endif
