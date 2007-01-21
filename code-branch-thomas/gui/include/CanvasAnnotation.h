#ifndef HEADER_CanvasAnnotation
#define HEADER_CanvasAnnotation

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

class CanvasAnnotation;

#include <wx/dcbuffer.h>
#include "SwisTrack.h"
#include "cv.h"

//! The control below the main canvas, displaying information about the image.
class CanvasAnnotation: public wxControl {

public:
	//! The associated CanvasAnnotationPanel.
	CanvasAnnotationPanel* mCanvasAnnotationPanel;
	//! The text shown on the left side.
	wxString mTextLeft;
	//! The text shown on the right side.
	wxString mTextRight;

	// Constructor.
	CanvasAnnotation(CanvasAnnotationPanel *cp);
	// Destructor.
	~CanvasAnnotation();

	//! Handles left click.
	void OnMouseLeftClick(wxMouseEvent &event);
	//! Handles right click.
	void OnMouseRightClick(wxMouseEvent &event);
	//! Paints the canvas.
	void OnPaint(wxPaintEvent& WXUNUSED(event));

	//! This sets a new image. Note that this class cleans up and frees old image automatically.
	void SetImage(IplImage *img);

	DECLARE_EVENT_TABLE()
};

#endif
