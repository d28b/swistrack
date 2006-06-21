#ifndef _segmenterpanelcanvas_H
#define _segmenterpanelcanvas_H

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif

#include "wx/dcbuffer.h"  // Allows to draw onto a dc buffer and hence to the screen

#include "SegmenterPanel.h"

//class SegmenterPanel;

/** \class SegmenterPanelCanvas
* \brief Creates a canvas showing the segmented image, allowing for finetuning the segmenter parameters.
*/ 
class SegmenterPanelCanvas: public  wxPanel // wxScrolledWindow
{
  public:
  SegmenterPanelCanvas(SegmenterPanel *parent, wxPanel *loc,const wxPoint& pos, const wxSize& size);
    
  void OnPaint(wxPaintEvent& WXUNUSED(event));


  private:
	 SegmenterPanel* parent;


	
  protected:
DECLARE_EVENT_TABLE()
};

#endif
