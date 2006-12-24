#include "Display.h"
#define THISCLASS Display

BEGIN_EVENT_TABLE(Canvas, wxPanel)
    EVT_PAINT(Canvas::OnPaint)
	EVT_ERASE_BACKGROUND(Canvas::EraseBackground)
    EVT_LEFT_DOWN(Canvas::OnMouseClick)
	EVT_RIGHT_DOWN(Canvas::OnMouseClick)
	EVT_MOTION(Canvas::OnMouseMove)
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
#ifdef MULTITHREAD
		wxCriticalSectionLocker locker(*(parent->criticalSection));
#endif
	
	CvPoint p = GetPixel(event.GetX(),event.GetY());
	if(parent->GetMenuBar()->GetMenu(3)->IsChecked(Gui_Tools_FlipScreen)) p.y=default_height-p.y;
   	parent->mx=p.x;
	parent->my=p.y;
	parent->clicked=event.GetButton();
	
    if(event.GetButton()==wxMOUSE_BTN_LEFT){
		// set statusbar text
		wxString msg;
		/** \todo Insert calibration into ot interface and allow for conversion of points */
			msg.Printf(_("(%d, %d)"), p.x, p.y);

		(parent)->SetStatusText(msg, 0);

		if(parent->ot && parent->GetStatus()==RUNNING)
			dragging=parent->ot->GetIdByPos(&cvPoint2D32f(p.x,p.y)); // set 'dragging' to the id of the trajectory the user is clicking on (-1) if none
		else 
			dragging=-1;
	}
	else if(event.GetButton()==wxMOUSE_BTN_RIGHT){
		parent->ot->ClearCoverageImage();
	}		
}

/** \brief Event handler for mouse movements over the main canvas 
* 
*/
void Canvas::OnMouseMove(wxMouseEvent &event)
{
#ifdef MULTITHREAD
	wxCriticalSectionLocker locker(*(parent->criticalSection));
#endif
	wxString msg;
	
	if(event.LeftIsDown() && dragging>-1){
    	CvPoint p = GetPixel(event.GetX(),event.GetY());
		if(parent->GetMenuBar()->GetMenu(3)->IsChecked(Gui_Tools_FlipScreen)) p.y=default_height-p.y;
        parent->ot->SetPos(dragging,&cvPoint2D32f(p.x,p.y));  
		wxString msg;
		msg.Printf("Dragging Object %d",dragging);
		(parent)->SetStatusText(msg, 0);
	}
	else
		dragging=-1;
}
