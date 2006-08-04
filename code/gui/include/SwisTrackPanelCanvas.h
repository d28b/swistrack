#ifndef _SwisTrackpanelcanvas_H
#define _SwisTrackpanelcanvas_H

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif

#include "wx/dcbuffer.h"  // Allows to draw onto a dc buffer and hence to the screen
#include "wx/panel.h"

class SwisTrackPanel;

/** \class SwisTrackPanelCanvas
* \brief Creates a canvas showing the segmented image, allowing for finetuning the SwisTrack parameters.
*/ 
class SwisTrackPanelCanvas: public wxPanel // wxScrolledWindow
{
  public:
  SwisTrackPanelCanvas(SwisTrack* parent, wxPanel *loc,const wxPoint& pos, const wxSize& size);
  void Clear();
  void CreateBitmapfromImage(const wxImage& img, int depth = -1);    
  void OnPaint(wxPaintEvent& WXUNUSED(event));

private:
	 SwisTrack* parent;
	 wxBitmap* bmp;


	
  protected:
DECLARE_EVENT_TABLE()
};

#endif
