#include "SegmenterPanelCanvas.h"
#include "Canvas.h"

BEGIN_EVENT_TABLE(SegmenterPanelCanvas, wxPanel)
    EVT_PAINT(SegmenterPanelCanvas::OnPaint)
	EVT_ERASE_BACKGROUND(Canvas::EraseBackground)
END_EVENT_TABLE()

/** \brief Constructor
* 
* Creates a canvas where the segmented image can be drawn (within the segmenter panel).
*/
SegmenterPanelCanvas::SegmenterPanelCanvas(SegmenterPanel *parent, wxPanel *loc, const wxPoint& pos, const wxSize& size):
	wxPanel(loc, -1, pos, size)
{
 this->parent=parent;
}

/** \brief Event handler for the canvas' OnPaint event
*
* This function is called whenever the canvas needs to be redrawn (for 
* instance if the window is moved. Also, when the Canvas::Refresh() method
* is called (as from SwisTrack::otProcessData().
*/
void SegmenterPanelCanvas::OnPaint(wxPaintEvent& WXUNUSED(event))
{
  wxBufferedPaintDC dc(this);
   
	  //wxASSERT((parent->colorbmp));
  if(parent->binarybmp){
		  dc.BeginDrawing();
		  dc.DrawBitmap(*(parent->binarybmp),0,0,FALSE);
		  dc.EndDrawing();
	 }

 }
