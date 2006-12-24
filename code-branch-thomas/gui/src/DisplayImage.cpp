#include "DisplayImage.h"
#define THISCLASS DisplayImage

BEGIN_EVENT_TABLE(DisplayImage, wxPanel)
    EVT_PAINT(DisplayImage::OnPaint)
	EVT_ERASE_BACKGROUND(DisplayImage::EraseBackground)
    EVT_LEFT_DOWN(DisplayImage::OnMouseClick)
	EVT_RIGHT_DOWN(DisplayImage::OnMouseClick)
	EVT_MOTION(DisplayImage::OnMouseMove)
END_EVENT_TABLE()

THISCLASS::DisplayImage(wxWindow *parent, SwisTrackCore *stc):
		Display(parent, stc) {

}

void THISCLASS::SetImage(DataStructureImage* dsi) {
	mDataStructureImage=dsi;
}


/** \brief Interception of the EVT_ERASE_BACKGROUND
*
* This method is empty on purpose and helps to reduce flickering by intercepting the
* erase background event (EVT_ERASE_BACKGROUND).
*/
void THISCLASS::EraseBackground(wxEraseEvent& event) {
}

/** \brief Event handler for the OnPaint event
*
* This function is called whenever the canvas needs to be redrawn (for 
* instance if the window is moved. Also, when the Canvas::Refresh() method
* is called (as from SwisTrack::otUpdate().
*/
void THISCLASS::OnPaint(wxPaintEvent& WXUNUSED(event)) {
	wxBufferedPaintDC dc(this);

	// Fetch image to draw
	IplImage *inputimage=0;
	if (mDataStructureImage) {
		inputimage=mDataStructureImage->mImage;
	}

	// Begin drawing to buffer
	dc.BeginDrawing();
	int width=dc.GetClientWidth();
	int height=dc.GetClientHeight();

	if (inputimage==0) {
		// If no image is to be drawn, display a white surface
		dc.SetBrush(wxNONE);
		dc.SetFillColor(*wxWHITE);
		dc.Rect(0, 0, width, height);
		dc.SetFontColor(*wxBLACK);
		dc.SetFontSize(12);
		dc.DrawText("No image.");
	} else {
		// Otherwise, display the image
		double widthratio=(double)width/(double)(inputimage->width());
		double heightratio=(double)height/(double)(inputimage->height());
		double ratio=1;
		if (widthratio<ratio) {ratio=widthratio;}
		if (heightratio<ratio) {ratio=heightratio;}

		if (ratio<1) {
			// Scale image
			int scaledwidth=(int)floor(inputimage->width()*ratio);
			int scaledheight=(int)floor(inputimage->height()*ratio);
			IplImage *img = cvCreateImage(cvSize(scaledwidth, scaledheight), inputimage->depth, inputimage->channels);
			cvScaleImage(inputimage, img, ratio, ratio);
			TODO
		}

		//if (mFlipHorizontal) cvFlip(inputimage);
		//if (mFlipVertical) cvFlip(inputimage);

		wxImage* colorimg = new wxImage(inputimage->width, inputimage->height, (unsigned char*) inputimage->imageData, TRUE);
		colorbmp= new wxBitmap(colorimg, 3);
		dc.DrawBitmap(*(parent->colorbmp), 0, 0, FALSE);
	}
	
	// End drawing
	dc.EndDrawing();
}

/** \brief Event handler for mouse clicks on the main canvas 
* 
* Calculates the coordinates in worldspace based on the position within 
* the canvas and prints both into the status bar.
*/
void Canvas::OnMouseClick(wxMouseEvent &event) {
#ifdef MULTITHREAD
	wxCriticalSectionLocker locker(*(parent->criticalSection));
#endif

	CvPoint p = GetPixel(event.GetX(), event.GetY());
	if (parent->GetMenuBar()->GetMenu(3)->IsChecked(Gui_Tools_FlipScreen)) p.y=default_height-p.y;
   	parent->mx=p.x;
	parent->my=p.y;
	parent->clicked=event.GetButton();

    if (event.GetButton()==wxMOUSE_BTN_LEFT) {
		// set statusbar text
		wxString msg;
		/** \todo Insert calibration into ot interface and allow for conversion of points */
		msg.Printf(_("(%d, %d)"), p.x, p.y);

		mSwisTrack->SetStatusText(msg, 0);

		if (parent->ot && parent->GetStatus()==RUNNING)
			dragging=parent->ot->GetIdByPos(&cvPoint2D32f(p.x,p.y)); // set 'dragging' to the id of the trajectory the user is clicking on (-1) if none
		else 
			dragging=-1;
	} else if (event.GetButton()==wxMOUSE_BTN_RIGHT) {
		parent->ot->ClearCoverageImage();
	}		
}

/** \brief Event handler for mouse movements over the main canvas 
* 
*/
void Canvas::OnMouseMove(wxMouseEvent &event) {
#ifdef MULTITHREAD
	wxCriticalSectionLocker locker(*(parent->criticalSection));
#endif
	wxString msg;
	
	if (event.LeftIsDown() && dragging>-1) {
    	CvPoint p = GetPixel(event.GetX(),event.GetY());
		if(parent->GetMenuBar()->GetMenu(3)->IsChecked(Gui_Tools_FlipScreen)) p.y=default_height-p.y;
        parent->ot->SetPos(dragging,&cvPoint2D32f(p.x,p.y));  
		wxString msg;
		msg.Printf("Dragging Object %d",dragging);
		(parent)->SetStatusText(msg, 0);
	} else {
		dragging=-1;
	}
}
