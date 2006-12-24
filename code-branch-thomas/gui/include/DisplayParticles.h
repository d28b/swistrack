#ifndef HEADER_Display
#define HEADER_Display

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
private:
	 int default_width;
	 int default_height;
	 SwisTrack* parent;
	 int status;
	 int dragging; 
  
public:
	void OnMouseClick(wxMouseEvent &event);
	void OnMouseMove(wxMouseEvent &event);
    Canvas(SwisTrack *parent, const wxPoint& pos, const wxSize& size);
	void EraseBackground(wxEraseEvent& event);
  	
    void OnPaint(wxPaintEvent& WXUNUSED(event));
	wxSize GetDefaultSize(){return(wxSize(default_width,default_height));}
	void SetDefaultSize(wxSize size){default_width=size.GetWidth(); default_height=size.GetHeight();}
	void SetSizeToDefault(){SetSize(default_width,default_height);};
	CvPoint GetPixel(int x,int y){return(cvPoint((double) x/GetSize().GetWidth()*default_width,(double) y/GetSize().GetHeight()*default_height));};
	//CvPoint GetPixel(int x,int y){return(cvPoint(GetSize().GetWidth(),GetSize().GetHeight()));};

  

	
  protected:
DECLARE_EVENT_TABLE()
};

#endif
