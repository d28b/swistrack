#ifndef _trackingpanel_H
#define _trackingpanel_H

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/minifram.h"  // Imports the mini frame class (tracker and segmenter panel)
#include "wx/valgen.h"	  // Generic validators, used in almost every dialgo

#include "constants.h"
#include "SwisTrack.h"


/** \class TrackingPanel
* \brief Creates a miniframe allowing the user to change tracking parameters.
*
*/
class TrackingPanel : public wxMiniFrame
	{
	public:
		void OnChangeMaxSpeed(wxScrollEvent& WXUNUSED(event));
		TrackingPanel(SwisTrack *parent);
	protected:
		DECLARE_EVENT_TABLE()
	private:
		SwisTrack* parent;
		int maxspeed;
	};

#endif
