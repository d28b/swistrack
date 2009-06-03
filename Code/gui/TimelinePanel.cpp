#include "TimelinePanel.h"
#define THISCLASS TimelinePanel

#include <wx/image.h>
#include <highgui.h>
#include <fstream>
#include <math.h>
#include "ImageConversion.h"

BEGIN_EVENT_TABLE(THISCLASS, wxControl)
	EVT_PAINT(THISCLASS::OnPaint)
	EVT_SIZE(THISCLASS::OnSize)
	EVT_LEFT_DCLICK(THISCLASS::OnMouseLeftDoubleClick)
	EVT_LEFT_DOWN(THISCLASS::OnMouseLeftDown)
	EVT_LEFT_UP(THISCLASS::OnMouseLeftUp)
	EVT_RIGHT_DOWN(THISCLASS::OnMouseRightDown)
	EVT_MOUSEWHEEL(THISCLASS::OnMouseWheel)
	EVT_MOTION(THISCLASS::OnMouseMove)
	EVT_MENU(cID_TriggerAuto10, THISCLASS::OnMenuTrigger)
	EVT_MENU(cID_TriggerAuto1, THISCLASS::OnMenuTrigger)
	EVT_MENU(cID_TriggerManual, THISCLASS::OnMenuTrigger)
	EVT_MENU(cID_ViewReset, THISCLASS::OnMenuView)
	EVT_MENU(cID_ViewZoomIn, THISCLASS::OnMenuView)
	EVT_MENU(cID_ViewZoomOut, THISCLASS::OnMenuView)
	EVT_MENU(cID_SaveTimeline, THISCLASS::OnMenuSaveTimeline)
END_EVENT_TABLE()

THISCLASS::TimelinePanel(wxWindow *parent, SwisTrack *st):
		wxControl(parent, -1), mSwisTrack(st), mSelectedComponent(0), mPopupMenu(), mTimer(this),
		mViewOffset(0), mViewScale(1000), mMoveStarted(false), mMoveStartPoint(0) {

	SetWindowStyle(wxNO_BORDER);
	SetSize(wxSize(20, 20));
	SetMinSize(wxSize(20, 20));

	// Create the popup menu
	mPopupMenu.AppendCheckItem(cID_TriggerAuto10, wxT("Update every 10 seconds"));
	mPopupMenu.AppendCheckItem(cID_TriggerAuto1, wxT("Update every second"));
	mPopupMenu.AppendCheckItem(cID_TriggerManual, wxT("Update manually (double click)"));
	mPopupMenu.AppendSeparator();
	mPopupMenu.AppendCheckItem(cID_ViewReset, wxT("Reset view"));
	mPopupMenu.AppendCheckItem(cID_ViewZoomIn, wxT("Zoom in"));
	mPopupMenu.AppendCheckItem(cID_ViewZoomOut, wxT("Zoom out"));
	mPopupMenu.AppendSeparator();
	mPopupMenu.Append(cID_SaveTimeline, wxT("Save timeline as ..."));

	// Set non-bold font
	wxFont f = GetFont();
	f.SetPointSize(8);
	f.SetWeight(wxNORMAL);

	// Start the timer
	mTimer.Start(1000, false);
}

THISCLASS::~TimelinePanel() {
}

void THISCLASS::SelectComponent(Component *component) {
	mSelectedComponent = component;
	Refresh(true);
}

void THISCLASS::StartRecording() {
	mSwisTrack->mSwisTrackCore->mEventRecorder->StartRecording();
	Refresh(true);

	double stepdistance = mSwisTrack->mSwisTrackCore->mEventRecorder->GetStepDistance();
	if (stepdistance == 0) {
		mSwisTrack->SetStatusText(wxT(""), SwisTrack::cStatusField_Timeline);
	} else {
		mSwisTrack->SetStatusText(wxString::Format(wxT("%0.2f fps (%0.0f ms)"), 1 / stepdistance, stepdistance*1000), SwisTrack::cStatusField_Timeline);
	}
}

void THISCLASS::OnPaint(wxPaintEvent& WXUNUSED(event)) {
	wxPaintDC dc(this);
	bool ok = Paint(dc);

	// Display an error if no image can be displayed
	if (! ok) {
		dc.SetBackground(wxBrush(wxColour(0x33, 0x33, 0x33)));
		dc.SetTextForeground(wxColour(0xff, 0xff, 0xff));
		dc.Clear();
		dc.SetFont(GetFont());
		dc.DrawText(wxT("No timeline."), 4, 4);
	}
}

bool THISCLASS::Paint(wxPaintDC &dc) {
	// Get timeline
	const SwisTrackCoreEventRecorder::Timeline *timeline = mSwisTrack->mSwisTrackCore->mEventRecorder->GetLastTimeline();
	if (timeline == 0) {
		return false;
	}

	// Draw background
	dc.SetBackground(wxBrush(wxColour(0xff, 0xff, 0xff)));
	dc.Clear();

	// Draw
	DrawTrigger(dc, timeline);
	DrawTicks(dc, timeline);
	DrawComponentSteps(dc, timeline);
	DrawStepLapTimes(dc, timeline);
	DrawSteps(dc, timeline);
	DrawStartStop(dc, timeline);
	DrawBeginEnd(dc, timeline);
	DrawTimelineOverflow(dc, timeline);

	return true;
}

void THISCLASS::DrawTrigger(wxPaintDC &dc, const SwisTrackCoreEventRecorder::Timeline *timeline) {
	// Prepare
	wxSize dcsize = dc.GetSize();
	int dw = dcsize.GetWidth();
	int dh = dcsize.GetHeight();
	dc.SetBrush(wxBrush(wxColour(0xee, 0xee, 0xee)));
	dc.SetPen(wxPen(wxColour(0xee, 0xee, 0xee)));

	// Draw
	double starttime = -1;
	bool active = timeline->mBeginState.mTriggerActive;
	SwisTrackCoreEventRecorder::Timeline::tEventList::const_iterator it = timeline->mEvents.begin();
	while (it != timeline->mEvents.end()) {
		if (it->mType == SwisTrackCoreEventRecorder::sType_BeforeTriggerStart) {
			starttime = mSwisTrack->mSwisTrackCore->mEventRecorder->CalculateDuration(&(timeline->mBegin), &(*it));;
			active = true;
		} else if (it->mType == SwisTrackCoreEventRecorder::sType_AfterTriggerStop) {
			double time = mSwisTrack->mSwisTrackCore->mEventRecorder->CalculateDuration(&(timeline->mBegin), &(*it));
			DrawTrigger(dc, dw, dh, starttime, time);
			starttime = time;
			active = false;
		}

		it++;
	}

	// Draw last event
	if (active) {
		DrawTrigger(dc, dw, dh, starttime, -1);
	}
}

void THISCLASS::DrawTrigger(wxPaintDC &dc, int dw, int dh, double starttime, double stoptime) {
	int startx = ((starttime < 0) ? -5 : (int)floor(starttime * mViewScale + mViewOffset + 0.5));
	int stopx = ((stoptime < 0) ? dw + 5 : (int)floor(stoptime * mViewScale + mViewOffset + 0.5));
	dc.DrawRectangle(startx, 0, stopx - startx, dh);
}

void THISCLASS::DrawTicks(wxPaintDC &dc, const SwisTrackCoreEventRecorder::Timeline *timeline) {
	wxSize dcsize = dc.GetSize();
	int dw = dcsize.GetWidth();
	//int dh=dcsize.GetHeight(); // Not needed

	dc.SetFont(GetFont());
	dc.SetPen(wxPen(wxColour(0xcc, 0xcc, 0xcc)));
	dc.SetTextForeground(wxColour(0xcc, 0xcc, 0xcc));

	// Calculate how many ticks to display
	double mintickdistance = 5;
	double mintickdistancetime = mintickdistance / mViewScale;
	double tickdistancetime = pow(10, ceil(log(mintickdistancetime) / log((double)10)));

	// Draw ticks
	double endtime = mSwisTrack->mSwisTrackCore->mEventRecorder->CalculateDuration(&(timeline->mBegin), &(timeline->mEnd));
	int ticknumber = (int)ceil(-mViewOffset / mViewScale / tickdistancetime);
	while (true) {
		double xtime = ticknumber * tickdistancetime;
		if (xtime >= endtime) {
			break;
		}
		int x = (int)floor(xtime * mViewScale + mViewOffset);
		if (x > dw) {
			break;
		}

		if (ticknumber % 10 == 0) {
			dc.DrawLine(x, 0, x, 4);
			wxString label = wxString::Format(wxT("%d"), (int)floor(xtime * 1000. + 0.5));
			int textwidth, textheight;
			GetTextExtent(label, &textwidth, &textheight) ;
			textwidth >>= 1;
			dc.DrawText(label, x - textwidth, 2);
		} else {
			dc.DrawLine(x, 0, x, 2);
		}

		ticknumber++;
	}
}

void THISCLASS::DrawComponentSteps(wxPaintDC &dc, const SwisTrackCoreEventRecorder::Timeline *timeline) {
	// Prepare
	wxSize dcsize = dc.GetSize();
	int dw = dcsize.GetWidth();
	int dh = dcsize.GetHeight();

	// Draw
	double starttime = -1;
	SwisTrackCoreEventRecorder::Timeline::tEventList::const_iterator it = timeline->mEvents.begin();
	while (it != timeline->mEvents.end()) {
		if ((it->mType == SwisTrackCoreEventRecorder::sType_StepStart) && (it->mComponent)) {
			starttime = mSwisTrack->mSwisTrackCore->mEventRecorder->CalculateDuration(&(timeline->mBegin), &(*it));
		} else if ((it->mType == SwisTrackCoreEventRecorder::sType_StepStop) && (it->mComponent)) {
			double time = mSwisTrack->mSwisTrackCore->mEventRecorder->CalculateDuration(&(timeline->mBegin), &(*it));
			DrawComponentStep(dc, dw, dh, starttime, time, (mSelectedComponent == it->mComponent));
			starttime = time;
		}

		it++;
	}
}

void THISCLASS::DrawComponentStep(wxPaintDC &dc, int dw, int dh, double starttime, double stoptime, bool selected) {
	if (selected) {
		dc.SetBrush(wxBrush(wxColour(0xff, 0x66, 0x66)));
		dc.SetPen(wxPen(wxColour(0xcc, 0x00, 0x00)));
	} else {
		dc.SetBrush(wxBrush(wxColour(0x66, 0x66, 0xff)));
		dc.SetPen(wxPen(wxColour(0x00, 0x00, 0xcc)));
	}
	int startx = ((starttime < 0) ? -5 : (int)floor(starttime * mViewScale + mViewOffset + 0.5));
	int stopx = ((stoptime < 0) ? dw + 5 : (int)floor(stoptime * mViewScale + mViewOffset + 0.5));
	dc.DrawRectangle(startx, dh - 15, stopx - startx, 20);
}

void THISCLASS::DrawStepLapTimes(wxPaintDC &dc, const SwisTrackCoreEventRecorder::Timeline *timeline) {
	// Prepare
	wxSize dcsize = dc.GetSize();
	int dw = dcsize.GetWidth();
	int dh = dcsize.GetHeight();
	dc.SetBrush(wxBrush(wxColour(0x00, 0x00, 0x00)));
	dc.SetPen(wxPen(wxColour(0x00, 0x00, 0x00)));

	// Draw
	SwisTrackCoreEventRecorder::Timeline::tEventList::const_iterator it = timeline->mEvents.begin();
	while (it != timeline->mEvents.end()) {
		if (it->mType == SwisTrackCoreEventRecorder::sType_StepLapTime) {
			double time = mSwisTrack->mSwisTrackCore->mEventRecorder->CalculateDuration(&(timeline->mBegin), &(*it));
			DrawStepLapTime(dc, dw, dh, time);
		}

		it++;
	}
}

void THISCLASS::DrawStepLapTime(wxPaintDC &dc, int dw, int dh, double time) {
	int x = ((time < 0) ? -5 : (int)floor(time * mViewScale + mViewOffset + 0.5));
	dc.DrawLine(x, dh - 13, x, dh);
}

void THISCLASS::DrawSteps(wxPaintDC &dc, const SwisTrackCoreEventRecorder::Timeline *timeline) {
	// Prepare
	wxSize dcsize = dc.GetSize();
	int dw = dcsize.GetWidth();
	int dh = dcsize.GetHeight();
	dc.SetBrush(wxBrush(wxColour(0x00, 0x00, 0xcc)));
	dc.SetPen(wxPen(wxColour(0x00, 0x00, 0xcc)));

	// Draw
	double starttime = -1;
	SwisTrackCoreEventRecorder::Timeline::tEventList::const_iterator it = timeline->mEvents.begin();
	while (it != timeline->mEvents.end()) {
		if ((it->mType == SwisTrackCoreEventRecorder::sType_StepStart) && (it->mComponent == 0)) {
			starttime = mSwisTrack->mSwisTrackCore->mEventRecorder->CalculateDuration(&(timeline->mBegin), &(*it));
		} else if ((it->mType == SwisTrackCoreEventRecorder::sType_StepStop) && (it->mComponent == 0)) {
			double time = mSwisTrack->mSwisTrackCore->mEventRecorder->CalculateDuration(&(timeline->mBegin), &(*it));
			DrawStep(dc, dw, dh, starttime, time);
			starttime = time;
		}

		it++;
	}
}

void THISCLASS::DrawStep(wxPaintDC &dc, int dw, int dh, double starttime, double stoptime) {
	int startx = ((starttime < 0) ? -5 : (int)floor(starttime * mViewScale + mViewOffset + 0.5));
	int stopx = ((stoptime < 0) ? dw + 5 : (int)floor(stoptime * mViewScale + mViewOffset + 0.5));
	dc.DrawRectangle(startx, dh - 8, stopx - startx, 15);
}

void THISCLASS::DrawStartStop(wxPaintDC &dc, const SwisTrackCoreEventRecorder::Timeline *timeline) {
	// Prepare
	wxSize dcsize = dc.GetSize();
	int dw = dcsize.GetWidth();
	int dh = dcsize.GetHeight();

	// Draw
	double starttime = -1;
	bool started = timeline->mBeginState.mStarted;
	bool production = timeline->mBeginState.mStartedInProductionMode;
	SwisTrackCoreEventRecorder::Timeline::tEventList::const_iterator it = timeline->mEvents.begin();
	while (it != timeline->mEvents.end()) {
		if (it->mType == SwisTrackCoreEventRecorder::sType_SetModeProduction) {
			production = true;
		} else if (it->mType == SwisTrackCoreEventRecorder::sType_SetModeNormal) {
			production = false;
		} else if (it->mType == SwisTrackCoreEventRecorder::sType_BeforeStart) {
			starttime = mSwisTrack->mSwisTrackCore->mEventRecorder->CalculateDuration(&(timeline->mBegin), &(*it));
			started = true;
		} else if (it->mType == SwisTrackCoreEventRecorder::sType_AfterStop) {
			double time = mSwisTrack->mSwisTrackCore->mEventRecorder->CalculateDuration(&(timeline->mBegin), &(*it));
			DrawStartStop(dc, dw, dh, starttime, time, production);
			starttime = time;
			started = false;
		}

		it++;
	}

	// Draw last event
	if (started) {
		//double time=mSwisTrack->mSwisTrackCore->mEventRecorder->CalculateDuration(&(timeline->mBegin), &(timeline->mEnd));
		DrawStartStop(dc, dw, dh, starttime, -1, production);
	}
}

void THISCLASS::DrawStartStop(wxPaintDC &dc, int dw, int dh, double starttime, double stoptime, bool production) {
	if (production) {
		dc.SetBrush(wxBrush(wxColour(0x00, 0xcc, 0x00)));
		dc.SetPen(wxPen(wxColour(0x00, 0x88, 0x00)));
	} else {
		dc.SetBrush(wxBrush(wxColour(0xcc, 0xcc, 0xcc)));
		dc.SetPen(wxPen(wxColour(0x88, 0x88, 0x88)));
	}
	int startx = ((starttime < 0) ? -5 : (int)floor(starttime * mViewScale + mViewOffset + 0.5));
	int stopx = ((stoptime < 0) ? dw + 5 : (int)floor(stoptime * mViewScale + mViewOffset + 0.5));
	dc.DrawRoundedRectangle(startx, dh - 6, stopx - startx, 15, 5);
}

void THISCLASS::DrawBeginEnd(wxPaintDC &dc, const SwisTrackCoreEventRecorder::Timeline *timeline) {
	// Prepare
	wxSize dcsize = dc.GetSize();
	int dw = dcsize.GetWidth();
	int dh = dcsize.GetHeight();
	dc.SetBrush(wxBrush(wxColour(0x33, 0x33, 0x33)));
	dc.SetPen(wxPen(wxColour(0x00, 0x00, 0x00)));

	// Calculate
	double totaltime = mSwisTrack->mSwisTrackCore->mEventRecorder->CalculateDuration(&(timeline->mBegin), &(timeline->mEnd));
	int beginx = (int)floor(mViewOffset + 0.5);
	int endx = (int)floor(totaltime * mViewScale + mViewOffset + 0.5);

	// Draw
	if (beginx > 0) {
		dc.DrawRectangle(0, -1, beginx, dh);
	}
	if (endx < dw) {
		dc.DrawRectangle(endx, -1, dw - endx + 1, dh + 1);
	}
}

void THISCLASS::DrawTimelineOverflow(wxPaintDC &dc, const SwisTrackCoreEventRecorder::Timeline *timeline) {
	// Get last event
	if (timeline->mEvents.size() == 0) {
		return;
	}
	SwisTrackCoreEventRecorder::Event ev = timeline->mEvents.back();
	if (ev.mType != SwisTrackCoreEventRecorder::sType_TimelineOverflow) {
		return;
	}

	// Prepare
	wxSize dcsize = dc.GetSize();
	//int dw=dcsize.GetWidth(); // Not needed
	int dh = dcsize.GetHeight();
	dc.SetBrush(wxBrush(wxColour(0x33, 0x33, 0x33)));
	dc.SetPen(wxPen(wxColour(0x00, 0x00, 0x00)));

	// Calculate
	double time = mSwisTrack->mSwisTrackCore->mEventRecorder->CalculateDuration(&(timeline->mBegin), &(ev));
	int x = (int)floor(time * mViewScale + mViewOffset + 0.5);

	// Draw
	dc.SetPen(wxPen(wxColour(255, 0, 0)));
	dc.SetTextForeground(wxColour(255, 0, 0));
	dc.DrawLine(x, -1, x, dh);
	dc.DrawText(wxT("Timeline memory exhausted."), x + 2, 2);
}

void THISCLASS::OnMouseLeftDoubleClick(wxMouseEvent &event) {
	if (mTimer.IsRunning()) {
		return;
	}
	StartRecording();
}

void THISCLASS::OnMouseLeftDown(wxMouseEvent &event) {
	mMoveStartPoint = event.GetX();
	mMoveStarted = true;
}

void THISCLASS::OnMouseLeftUp(wxMouseEvent &event) {
	mMoveStarted = false;
}

void THISCLASS::OnMouseMove(wxMouseEvent &event) {
	if (! mMoveStarted) {
		return;
	}
	if (! event.LeftIsDown()) {
		mMoveStarted = false;
		return;
	}

	// Move
	int nowpoint = event.GetX();
	SetViewOffset(mViewOffset + nowpoint - mMoveStartPoint);
	mMoveStartPoint = nowpoint;
}

void THISCLASS::OnMouseRightDown(wxMouseEvent &event) {
	// Set the checkmarks
	mPopupMenu.Check(cID_TriggerAuto10, (mTimer.IsRunning() && (mTimer.GetInterval() == 10000)));
	mPopupMenu.Check(cID_TriggerAuto1, (mTimer.IsRunning() && (mTimer.GetInterval() == 1000)));
	mPopupMenu.Check(cID_TriggerManual, (mTimer.IsRunning() == false));

	// Store the mouse position
	mMoveStartPoint = event.GetX();

	// Show the popup
	PopupMenu(&mPopupMenu);
}

void THISCLASS::OnMouseWheel(wxMouseEvent &event) {
	// For some reason (probably focus), this is never called.
	int rotation = event.GetWheelRotation();
	//SetViewScale(pow(1.01, rotation)*mViewScale, event.GetX());
	SetViewScale((rotation > 0 ? mViewScale*2 : mViewScale / 2), event.GetX());
}

void THISCLASS::OnSize(wxSizeEvent &event) {
	Refresh(true);
}

void THISCLASS::OnMenuView(wxCommandEvent& event) {
	if (event.GetId() == cID_ViewZoomOut) {
		SetViewScale(mViewScale / 2, mMoveStartPoint);
	} else if (event.GetId() == cID_ViewZoomIn) {
		SetViewScale(mViewScale*2, mMoveStartPoint);
	} else {
		// Reset
		mViewOffset = 0;
		mViewScale = 1000;
		Refresh(true);
	}
}

void THISCLASS::SetViewScale(double newscale, int fixpoint) {
	double xreal = ((double)fixpoint - mViewOffset) / mViewScale;
	mViewScale = newscale;
	SetViewOffset(fixpoint - (int)floor(xreal*mViewScale + 0.5));
	Refresh(true);
}

void THISCLASS::SetViewOffset(int newoffset) {
	// Get timeline
	const SwisTrackCoreEventRecorder::Timeline *timeline = mSwisTrack->mSwisTrackCore->mEventRecorder->GetLastTimeline();
	if (timeline == 0) {
		return;
	}

	// Check bounds
	wxSize dcsize = this->GetSize();
	double time = mSwisTrack->mSwisTrackCore->mEventRecorder->CalculateDuration(&(timeline->mBegin), &(timeline->mEnd));
	int timelinewidth = (int)floor(time * mViewScale + 0.5);
	if (newoffset < dcsize.GetWidth() - timelinewidth) {
		newoffset = dcsize.GetWidth() - timelinewidth;
	}
	if (newoffset > 0) {
		newoffset = 0;
	}

	// Redraw if necessary
	if (mViewOffset == newoffset) {
		return;
	}
	mViewOffset = newoffset;
	Refresh(true);
}

void THISCLASS::OnMenuTrigger(wxCommandEvent& event) {
	if (mTimer.IsRunning()) {
		mTimer.Stop();
	}
	if (event.GetId() == cID_TriggerAuto10) {
		mTimer.Start(10000, false);
	} else if (event.GetId() == cID_TriggerAuto1) {
		mTimer.Start(1000, false);
	} else {
		// Manual mode, do nothing
	}
	Refresh();
}

void THISCLASS::OnMenuSaveTimeline(wxCommandEvent& event) {
	// Get the timeline
	const SwisTrackCoreEventRecorder::Timeline *timeline = mSwisTrack->mSwisTrackCore->mEventRecorder->GetLastTimeline();
	if (timeline == 0) {
		return;
	}

	// Copy the current timeline (since the file dialog will allow other threads to run)
	SwisTrackCoreEventRecorder::Timeline timelinecopy = *timeline;

	// Show the file save dialog
	wxFileDialog dlg(this, wxT("Save timeline"), wxT(""), wxT(""), wxT("Text (*.txt)|*.txt|All files|*.*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);
	if (dlg.ShowModal() != wxID_OK) {
		return;
	}

	// Save the timeline
	wxString filename = dlg.GetPath();
	std::ofstream ofs(filename.mb_str(wxConvFile));
	if (! ofs.is_open()) {
		wxMessageDialog dlg(this, wxT("The file could not be saved!"), wxT("Save timeline"), wxOK);
		dlg.ShowModal();
		return;
	}

	// Write each event
	SwisTrackCoreEventRecorder::Timeline::tEventList::iterator it = timelinecopy.mEvents.begin();
	while (it != timelinecopy.mEvents.end()) {
		double time = mSwisTrack->mSwisTrackCore->mEventRecorder->CalculateDuration(&(timelinecopy.mBegin), &(*it));
		ofs << time;

		wxString type = mSwisTrack->mSwisTrackCore->mEventRecorder->mTypeNames[it->mType];
		if (type == wxT("")) {
			ofs << wxT("\t") << wxT("(unknown)");
		} else {
			ofs << wxT("\t") << type;
		}

		if (it->mComponent) {
			ofs << wxT("\t") << it->mComponent->mName;
		}

		ofs << std::endl;
		it++;
	}

	// Close
	ofs.close();
}
