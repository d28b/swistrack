#ifndef HEADER_TimelinePanel
#define HEADER_TimelinePanel

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

class TimelinePanel;

#include <wx/dcbuffer.h>
#include "SwisTrack.h"
#include "DisplayRenderer.h"
#include <cv.h>

/** \class TimelinePanel
* \brief Shows a snapshot of the execution timeline.
*/
class TimelinePanel: public wxControl {

public:
	SwisTrack *mSwisTrack;		//!< The associated SwisTrack object.

	// Constructor.
	TimelinePanel(wxWindow *parent, SwisTrack *st);
	// Destructor.
	~TimelinePanel();

private:
	enum eID {
		sID_TriggerAuto10,
		sID_TriggerAuto1,
		sID_TriggerManual,
		sID_ViewReset,
	};

	Component *mSelectedComponent;	//!< The selected component.
	wxMenu mPopupMenu;				//!< The popup menu.
	wxTimer mTimer;					//!< The timer.

	int mViewOffset;				//!< Offset of the view, in pixels.
	double mViewScale;				//!< Scale of the view, in pixels/s.
	bool mMoveStarted;				//!< True while we are in move mode.
	int mMoveStartPoint;			//!< The starting point when moving the image.

	//! Paints the timeline in the canvas.
	bool Paint(wxPaintDC &dc);
	//! Draws trigger activity.
	void DrawTrigger(wxPaintDC &dc, SwisTrackCoreEventRecorder::Timeline *timeline);
	//! Draws one trigger.
	void DrawTrigger(wxPaintDC &dc, int dw, int dh, double starttime, double stoptime);
	//! Draws the ticks.
	void DrawTicks(wxPaintDC &dc);
	//! Draws the component steps.
	void DrawComponentStep(wxPaintDC &dc, SwisTrackCoreEventRecorder::Timeline *timeline);
	//! Draws one component step.
	void DrawComponentStep(wxPaintDC &dc, int dw, int dh, double starttime, double stoptime, bool productive);
	//! Draws step lap times.
	void DrawStepLeap(wxPaintDC &dc, SwisTrackCoreEventRecorder::Timeline *timeline);
	//! Draws one step lap time.
	void DrawStepLeap(wxPaintDC &dc, int dw, int dh, double time);
	//! Draws steps.
	void DrawStep(wxPaintDC &dc, SwisTrackCoreEventRecorder::Timeline *timeline);
	//! Draws one step.
	void DrawStep(wxPaintDC &dc, int dw, int dh, double starttime, double stoptime, bool productive);
	//! Draws executions (from start to stop).
	void DrawStartStop(wxPaintDC &dc, SwisTrackCoreEventRecorder::Timeline *timeline);
	//! Draws one execution.
	void DrawStartStopEvent(wxPaintDC &dc, int dw, int dh, double starttime, double stoptime, bool productive);
	//! Draws the beginning and the end of the timeline.
	void DrawBeginEnd(wxPaintDC &dc, SwisTrackCoreEventRecorder::Timeline *timeline);

	//! The corresponding GUI event handler.
	void OnMouseLeftDown(wxMouseEvent &event);
	//! The corresponding GUI event handler.
	void OnMouseLeftUp(wxMouseEvent &event);
	//! The corresponding GUI event handler.
	void OnMouseRightDown(wxMouseEvent &event);
	//! The corresponding GUI event handler.
	void OnPaint(wxPaintEvent& WXUNUSED(event));
	//! The corresponding GUI event handler.
	void OnSize(wxSizeEvent &event);

	//! The corresponding GUI event handler.
	void OnMenuViewReset(wxCommandEvent& event);
	//! The corresponding GUI event handler.
	void OnMenuTrigger(wxCommandEvent& event);
	//! The corresponding GUI event handler.
	void OnMenuSaveTimeline(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()
};

#endif
