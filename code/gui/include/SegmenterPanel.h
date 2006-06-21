#ifndef _segmenterpanel_H
#define _segmenterpanel_H

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif

#include "wx/minifram.h"  // Imports the mini frame class (tracker and segmenter panel)
#include "wx/valgen.h"	  // Generic validators, used in almost every dialgo

#include "constants.h"
#include "SwisTrack.h"

#include "SegmenterPanelCanvas.h"
class SegmenterPanelCanvas;

/** \class SegmenterPanel 
* \brief Creates a miniframe allowing the user to change segmenter parameters.
*/
class SegmenterPanel : public wxMiniFrame
	{
	public:
		void OnChangeMinArea(wxScrollEvent& WXUNUSED(event));
		void OnChangeThreshold(wxScrollEvent& WXUNUSED(event));
		void OnPaint(wxPaintEvent& WXUNUSED(event));

		SegmenterPanel(SwisTrack *parent);

		wxBitmap* binarybmp;
		SegmenterPanelCanvas* canvas;
	
	protected:
		DECLARE_EVENT_TABLE()
	private:
		int minarea;
		int threshold;
		SwisTrack* parent;
		wxPanel* panel;
	
	};
#endif
