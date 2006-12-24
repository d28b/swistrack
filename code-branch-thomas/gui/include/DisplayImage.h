#ifndef HEADER_Display
#define HEADER_Display

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif

#include "wx/dcbuffer.h"  // Allows to draw onto a dc buffer and hence to the screen

#include "SwisTrack.h"

/** \class DisplayImage
* \brief Displays an image
*/
class DisplayImage: public Display {

public:
	Display(wxWindow *parent, SwisTrackCore *stc): mSwisTrackCore(stc) {}
	virtual ~Display() {}

	//! Sets the image to display.
	SetImage(DataStructureImage* dsi);

	// Display methods.
	void OnUpdate();

private:
	DataStructureImage *mDataStructureImage;


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

  

	
	DECLARE_EVENT_TABLE()
};

#endif
