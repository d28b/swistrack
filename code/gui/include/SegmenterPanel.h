#ifndef _segmenterpanel_H
#define _segmenterpanel_H

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif

#include "SwistrackPanel.h"
#include "SegmenterPanelCanvas.h"
class SegmenterPanelCanvas;

/** \class SegmenterPanel 
* \brief Creates a miniframe allowing the user to change segmenter parameters.
*/
class SegmenterPanel : public SwisTrackPanel
	{
	public:
		void OnPaint(wxPaintEvent& WXUNUSED(event));

		SegmenterPanel(SwisTrack *parent);

		wxBitmap* binarybmp;
		SegmenterPanelCanvas* canvas;

	private:
		void ChangeParam(wxCommandEvent& event);
		void ChangeIntParam(wxScrollEvent& event);
	
	};
#endif
