#ifndef HEADER_Display
#define HEADER_Display

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif

#include "wx/panel.h"
#include "wx/dcbuffer.h"  // Allows to draw onto a dc buffer and hence to the screen

#include "SwisTrackCore.h"

/** \class Display
* \brief This is the base class for all display panels.
*/
class Display: public wxPanel {

public:
	//! The associated SwisTrack GUI.
	SwisTrack *mSwisTrack;
	//! The associated SwisTrackCore object.
	SwisTrackCore *mSwisTrackCore;
	//! The internal name of the object.
	wxString mName;
	//! The name displayed to the user.
	wxString mDisplayName;
	//! Whether the image shall be flipped horizontally.
	bool mFlipHorizontal;
	//! Whether the image shall be flipped vertically.
	bool mFlipVertical;

	Display(wxWindow *parent, SwisTrack *st);
	virtual ~Display() {}

	//! This function is called if an update is desired.
	virtual void OnUpdate() = 0;

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

 };

#endif
