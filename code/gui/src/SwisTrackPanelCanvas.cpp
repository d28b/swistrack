#include "SwisTrackPanel.h"
#include "SwisTrackPanelCanvas.h"
#include "Canvas.h"

BEGIN_EVENT_TABLE(SwisTrackPanelCanvas, wxPanel)
    EVT_PAINT(SwisTrackPanelCanvas::OnPaint)
	EVT_ERASE_BACKGROUND(Canvas::EraseBackground)
END_EVENT_TABLE()

/** \brief Constructor
* 
* Creates a canvas where the segmented image can be drawn (within the SwisTrack panel).
*/
SwisTrackPanelCanvas::SwisTrackPanelCanvas(SwisTrack *parent, wxPanel *loc, const wxPoint& pos, const wxSize& size):
	wxPanel(loc, -1, pos, size)
{
 this->parent=parent;
 bmp=new wxBitmap(640/4,480/3,3);
 /** \todo get the right values for width and height here */
}

/** \brief Event handler for the canvas' OnPaint event
*
* This function is called whenever the canvas needs to be redrawn (for 
* instance if the window is moved. Also, when the Canvas::Refresh() method
* is called (as from SwisTrack::otUpdate().
*/
void SwisTrackPanelCanvas::OnPaint(wxPaintEvent& WXUNUSED(event))
{
  wxBufferedPaintDC dc(this);
   
	  //wxASSERT((parent->colorbmp));
  if(bmp){
		  dc.BeginDrawing();
		  dc.DrawBitmap(*(bmp),0,0,FALSE);
		  dc.EndDrawing();
	 }

 }

void SwisTrackPanelCanvas::Clear()
{
	if(bmp){
			delete bmp;
			bmp=NULL;
		}
}

void SwisTrackPanelCanvas::CreateBitmapfromImage(const wxImage& img, int depth){
	bmp=new wxBitmap(img,depth);
}

SwisTrackPanelCanvas::~SwisTrackPanelCanvas()
{
	if(bmp) delete bmp;
}