#ifndef _Canvas_H
#define _Canvas_H

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif

#include "wx/dcbuffer.h"  // Allows to draw onto a dc buffer and hence to the screen

#include "SwisTrack.h"

/** \class Canvas
* \brief Creates a canvas to draw tracking progress
*
* This class implements an OnPaint method and an event handler
* for mouse clicks on the canvas.
*/
class Canvas: public  wxPanel // wxScrolledWindow
{
  public:
	  void OnMouseClick(wxMouseEvent &event);
    Canvas(SwisTrack *parent, const wxPoint& pos, const wxSize& size);
	void EraseBackground(wxEraseEvent& event);
  	
    void OnPaint(wxPaintEvent& WXUNUSED(event));
	 int default_width;
	 int default_height;

  private:
	 SwisTrack* parent;
	 int status;


	
  protected:
DECLARE_EVENT_TABLE()
};

#endif
