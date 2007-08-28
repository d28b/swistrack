#ifndef _interceptionpanel_H
#define _interceptionpanel_H

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif

#include "wx/minifram.h"  // Imports the mini frame class (tracker and segmenter panel)
#include "wx/valgen.h"	  // Generic validators, used in almost every dialgo

#include "SwisTrack.h"


/** \class InterceptionPanel
* \brief Creates a miniframe allowing the user to pick a trajectory manually
*/
class InterceptionPanel : public wxMiniFrame
	{
	public:
		InterceptionPanel(SwisTrack *parent);

	protected:
		DECLARE_EVENT_TABLE()
	private:
		void OnSpinUpdate(wxSpinEvent &event );
		void OnInterceptDone( wxCommandEvent &event );
		void OnInterceptPick( wxCommandEvent &event );
		SwisTrack* parent;
		wxPanel* panel;
		int nobj;
		wxTextCtrl* spintext;
		wxString chosen_id;		
	};

#endif
