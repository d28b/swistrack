#include "Canvas.h"

BEGIN_EVENT_TABLE(Canvas, wxPanel)
    EVT_PAINT(Canvas::OnPaint)
	EVT_ERASE_BACKGROUND(Canvas::EraseBackground)
    EVT_LEFT_DOWN(Canvas::OnMouseClick)
	EVT_RIGHT_DOWN(Canvas::OnMouseClick)
END_EVENT_TABLE()

/** \brief Constructor
* 
* Creates a canvas where the tracking progress can be drawn.
*/
Canvas::Canvas(SwisTrack *parent, const wxPoint& pos, const wxSize& size):
wxPanel(parent, -1, pos, size)
{
 this->parent=parent;
 default_width=size.GetWidth();
 default_height=size.GetHeight();
}

/** \brief Interception of the EVT_ERASE_BACKGROUND
*
* This method is empty on purpose and helps to reduce flickering by intercepting the
* erase background event (EVT_ERASE_BACKGROUND).
*/
void Canvas::EraseBackground(wxEraseEvent& event){
	}

/** \brief Event handler for the canvas' OnPaint event
*
* This function is called whenever the canvas needs to be redrawn (for 
* instance if the window is moved. Also, when the Canvas::Refresh() method
* is called (as from SwisTrack::otUpdate().
*/
void Canvas::OnPaint(wxPaintEvent& WXUNUSED(event))
{
  wxBufferedPaintDC dc(this);
   
	  //wxASSERT((parent->colorbmp));
  if(parent->colorbmp && parent->colorbmp->Ok()){
		  dc.BeginDrawing();
		  dc.DrawBitmap(*(parent->colorbmp),0,0,FALSE);
		  dc.EndDrawing();
	 }

 }

/** \brief Event handler for mouse clicks on the main canvas 
* 
* Calculates the coordinates in worldspace based on the position within 
* the canvas and prints both into the status bar.
*/
void Canvas::OnMouseClick(wxMouseEvent &event)
{
	int u = event.GetX();
	int v = event.GetY();
	
	CvPoint p = GetPixel(u,v);
	u=p.x;
	v=p.y;
   	parent->mx=u;
	parent->my=v;
	parent->clicked=event.GetButton();
	
    if(event.GetButton()==wxMOUSE_BTN_LEFT){
		// set statusbar text
		wxString msg;
		/*if(parent->transform)
		{
			CvPoint2D32f p1=cvPoint2D32f(u-parent->width/2,v-parent->height/2);
			CvPoint2D32f p2;
			p2=parent->transform->ImageToWorld(p1);
			msg.Printf("(%dpx, %dpx) -> (%0.3fm, %0.3fm)",u,v,p2.x,p2.y);
		}
		else*/
		/** \todo Insert calibration into ot interface and allow for conversion of points */
			msg.Printf(_("(%d, %d)"), u, v);
		(parent)->SetStatusText(msg, 0);
	}
	else if(event.GetButton()==wxMOUSE_BTN_RIGHT){
#ifdef MULTITHREAD
		wxCriticalSectionLocker locker(*(parent->criticalSection));
#endif
		parent->ot->ClearCoverageImage();
	}		
}
