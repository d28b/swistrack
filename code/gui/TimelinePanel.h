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

	// Selects a component and highlights its step.
	void SelectComponent(Component *component);

private:
	//! An overwritten wxTimer.
class EventRecorderResetTimer: public wxTimer {
	public:
		TimelinePanel *mTimelinePanel;		//!< The associated timeline panel.

		//! Constructor.
		EventRecorderResetTimer(TimelinePanel *tp): wxTimer(), mTimelinePanel(tp) {}
		//! Destructor.
		~EventRecorderResetTimer() {}

		// wxTimer methods
		void Notify() {
			mTimelinePanel->StartRecording();
		}
	};

	//! Component IDs.
	enum eID {
		cID_TriggerAuto10=1,
		cID_TriggerAuto1,
		cID_TriggerManual,
		cID_ViewReset,
		cID_ViewZoomIn,
		cID_ViewZoomOut,
		cID_SaveTimeline
	};

	Component *mSelectedComponent;	//!< The selected component.
	wxMenu mPopupMenu;				//!< The popup menu.
	EventRecorderResetTimer mTimer;	//!< The timer.

	int mViewOffset;				//!< Offset of the view, in pixels.
	double mViewScale;				//!< Scale of the view, in pixels/s.
	bool mMoveStarted;				//!< True while we are in move mode.
	int mMoveStartPoint;			//!< The starting point when moving the image.

	//! Starts a new timeline recording.
	void StartRecording();
	//! Paints the timeline in the canvas.
	bool Paint(wxPaintDC &dc);
	//! Draws trigger activity.
	void DrawTrigger(wxPaintDC &dc, const SwisTrackCoreEventRecorder::Timeline *timeline);
	//! Draws one trigger.
	void DrawTrigger(wxPaintDC &dc, int dw, int dh, double starttime, double stoptime);
	//! Draws the ticks.
	void DrawTicks(wxPaintDC &dc, const SwisTrackCoreEventRecorder::Timeline *timeline);
	//! Draws the component steps.
	void DrawComponentSteps(wxPaintDC &dc, const SwisTrackCoreEventRecorder::Timeline *timeline);
	//! Draws one component step.
	void DrawComponentStep(wxPaintDC &dc, int dw, int dh, double starttime, double stoptime, bool selected);
	//! Draws step lap times.
	void DrawStepLapTimes(wxPaintDC &dc, const SwisTrackCoreEventRecorder::Timeline *timeline);
	//! Draws one step lap time.
	void DrawStepLapTime(wxPaintDC &dc, int dw, int dh, double time);
	//! Draws steps.
	void DrawSteps(wxPaintDC &dc, const SwisTrackCoreEventRecorder::Timeline *timeline);
	//! Draws one step.
	void DrawStep(wxPaintDC &dc, int dw, int dh, double starttime, double stoptime);
	//! Draws executions (from start to stop).
	void DrawStartStop(wxPaintDC &dc, const SwisTrackCoreEventRecorder::Timeline *timeline);
	//! Draws one execution.
	void DrawStartStop(wxPaintDC &dc, int dw, int dh, double starttime, double stoptime, bool production);
	//! Draws the beginning and the end of the timeline.
	void DrawBeginEnd(wxPaintDC &dc, const SwisTrackCoreEventRecorder::Timeline *timeline);
	//! Draws a red line at the point where the timeline run out of memory.
	void DrawTimelineOverflow(wxPaintDC &dc, const SwisTrackCoreEventRecorder::Timeline *timeline);

	//! The corresponding GUI event handler.
	void OnMouseLeftDoubleClick(wxMouseEvent &event);
	//! The corresponding GUI event handler.
	void OnMouseLeftDown(wxMouseEvent &event);
	//! The corresponding GUI event handler.
	void OnMouseLeftUp(wxMouseEvent &event);
	//! The corresponding GUI event handler.
	void OnMouseMove(wxMouseEvent &event);
	//! The corresponding GUI event handler.
	void OnMouseRightDown(wxMouseEvent &event);
	//! The corresponding GUI event handler.
	void OnMouseWheel(wxMouseEvent &event);
	//! The corresponding GUI event handler.
	void OnPaint(wxPaintEvent& WXUNUSED(event));
	//! The corresponding GUI event handler.
	void OnSize(wxSizeEvent &event);

	//! The corresponding GUI event handler.
	void OnMenuView(wxCommandEvent& event);
	//! The corresponding GUI event handler.
	void OnMenuTrigger(wxCommandEvent& event);
	//! The corresponding GUI event handler.
	void OnMenuSaveTimeline(wxCommandEvent& event);

	//! Sets a new view scale while keeping one point fixed and redraws.
	void SetViewScale(double newscale, int fixpoint);
	//! Sets a new view offset and redraws if necessary.
	void SetViewOffset(int newoffset);

	DECLARE_EVENT_TABLE()
};

#endif
