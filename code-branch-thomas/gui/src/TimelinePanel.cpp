#include "TimelinePanel.h"
#define THISCLASS TimelinePanel

#include <wx/image.h>
#include <highgui.h>
#include "ImageConversion.h"

BEGIN_EVENT_TABLE(THISCLASS, wxControl)
    EVT_PAINT(THISCLASS::OnPaint)
	EVT_ERASE_BACKGROUND(THISCLASS::OnEraseBackground)
	EVT_SIZE(THISCLASS::OnSize)
    EVT_LEFT_DOWN(THISCLASS::OnMouseLeftDown)
	EVT_LEFT_UP(THISCLASS::OnMouseLeftUp)
	EVT_RIGHT_DOWN(THISCLASS::OnMouseRightDown)
	EVT_MOTION(THISCLASS::OnMouseMove)
	EVT_MENU(sID_TriggerAuto10, THISCLASS::OnMenuTrigger)
	EVT_MENU(sID_TriggerAuto1, THISCLASS::OnMenuTrigger)
	EVT_MENU(sID_TriggerManual, THISCLASS::OnMenuTrigger)
	EVT_MENU(sID_ViewReset, THISCLASS::OnMenuViewReset)
	EVT_MENU(sID_SaveTimeline, THISCLASS::OnSaveTimeline)
END_EVENT_TABLE()

THISCLASS::TimelinePanel(wxWindow *parent, TimelinePanelPanel *cp):
		wxControl(parent, -1), mSwisTrack(0), mSelectedComponent(0), mPopupMenu(), mTimer(),
		mViewOffset(0), mViewScale(0), mMoveStarted(false), mMoveStartPoint(0) {

	SetWindowStyle(wxNO_BORDER);

	// Create the popup menu
	mPopupMenu.AppendCheckItem(sID_Trigger10, "Update every 10 seconds");
	mPopupMenu.AppendCheckItem(sID_Trigger1, "Update every second");
	mPopupMenu.AppendCheckItem(sID_TriggerManual, "Update manually (double click)");
	mPopupMenu.AppendSeparator();
	mPopupMenu.AppendCheckItem(sID_ViewReset, "Reset view");
	mPopupMenu.AppendSeparator();
	mPopupMenu.Append(sID_SaveTimeline, "Save timeline as ...");

	// Set non-bold font
	wxFont f=GetFont();
	f.SetWeight(wxNORMAL);

	// Set white background
	SetBackground(wxColour(0x00, 0x00, 0x00));

	// Start the timer
	mTimer.start(1000, false);
}

THISCLASS::~TimelinePanel() {
}

void THISCLASS::OnPaint(wxPaintEvent& WXUNUSED(event)) {
	wxPaintDC dc(this);
	bool ok=OnPaintImage(dc);

	// Display an error if no image can be displayed
	if (! ok) {
		dc.clear();
		dc.SetFont(GetFont());
		dc.DrawText("No timeline.", 4, 4);
	}
}

bool THISCLASS::Paint(wxPaintDC &dc) {
	// Get timeline
	SwisTrackCoreEventRecorder::Timeline *timeline=mSwisTrack->mSwisTrackCore->mTimeline->GetLastTimeline();
	if (timeline==0) {return false;}

	// Draw
	DrawTrigger(dc, timeline);
	DrawTicks(dc, timeline);
	DrawComponentSteps(dc, timeline);
	DrawStepLapTime(dc, timeline);
	DrawSteps(dc, timeline);
	DrawStartStop(dc, timeline);
	DrawBeginEnd(dc, timeline);

	return true;
}

void THISCLASS::DrawTrigger(wxPaintDC &dc, SwisTrackCoreEventRecorder::Timeline *timeline) {
	// Prepare
	wxSize dcsize=dc.GetSize();
	int dw=dcsize.GetWidth();
	int dh=dcsize.GetHeight();
	dc.SetBrush(wxBrush(wxColour(0xee, 0xee, 0xee)));
	dc.SetPen(wxBrush(wxColour(0xee, 0xee, 0xee)));

	// Draw
	double starttime=-1;
	SwisTrackCoreEventRecorder::Timeline::Eventlist::iterator it=timeline->mEvents.begin();
	while (it!=timeline->mEvents.end()) {
		if (it->mType == SwisTrackCoreEventRecorder::sType_TriggerStart) {
			starttime=SwisTrackCoreEventRecorder::CalculateDuration(&(timeline->mBegin), &(*it));;
		} else if (it->mType == SwisTrackCoreEventRecorder::sType_TriggerStop) {
			double time=SwisTrackCoreEventRecorder::CalculateDuration(&(timeline->mBegin), &(*it));
			DrawTrigger(dc, dw, dh, starttime, time)
			starttime=time;
		}

		it++;
	}

	// Draw last event
	if ((starttime>=0) || (timeline->mBeginState.mTriggerActive)) {
		double time=SwisTrackCoreEventRecorder::CalculateDuration(&(timeline->mBegin), &(timeline->mEnd));
		DrawTrigger(dc, dw, dh, starttime, time);
	}
}

void THISCLASS::DrawTrigger(wxPaintDC &dc, int dw, int dh, double starttime, double stoptime) {
	int startx=((starttime<0) ? -5 : (int)floor(starttime*mViewScale+mViewOffset+0.5));
	int stopx=((stoptime<0) ? dw+5 : (int)floor(stoptime*mViewScale+mViewOffset+0.5));
	dc.DrawRectangle(startx, 0, stopx, dh);
}

void THISCLASS::DrawTicks(wxPaintDC &dc) {
	wxSize dcsize=dc.GetSize();
	int dw=dcsize.GetWidth();
	int dh=dcsize.GetHeight();

	dc.SetPen(wxPen(wxColour(0xcc, 0xcc, 0xcc)));

	double mintickdistance=5;
	double mintickdistancetime=mintickdistance/mViewScale;
	double tickdistancetime=pow(10, ceil(log(mintickdistancetime)/log(10)));

	double endtime=SwisTrackCoreEventRecorder::CalculateDuration(&(timeline->mBegin), &(timeline->mEnd));
	double xtime=ceil(-mViewOffset/mViewScale/tickdistancetime)*tickdistancetime;
	while (true) {
		xtime+=tickdistancetime;
		if (xtime>=endtime) {break;}
		int x=(int)floor(xtime*mViewScale+mViewOffset);
		if (x>dw) {break;}
		dc.DrawLine(x, 0, x, 10);
		dc.DrawLine(x, dh-10, x, dh);
	}
}

void THISCLASS::DrawComponentStep(wxPaintDC &dc, SwisTrackCoreEventRecorder::Timeline *timeline) {
	// Prepare
	wxSize dcsize=dc.GetSize();
	int dw=dcsize.GetWidth();
	int dh=dcsize.GetHeight();

	// Draw
	double starttime=-1;
	double productive=timeline->mBeginState->mIsStartedInProductiveMode;
	SwisTrackCoreEventRecorder::Timeline::Eventlist::iterator it=timeline->mEvents.begin();
	while (it!=timeline->mEvents.end()) {
		if ((it->mType == SwisTrackCoreEventRecorder::sType_StepStart) && (it->mComponent)) {
			starttime=time;
		} else if ((it->mType == SwisTrackCoreEventRecorder::sType_StepStop) && (it->mComponent)) {
			double time=SwisTrackCoreEventRecorder::CalculateDuration(&(timeline->mBegin), &(*it));
			DrawComponentStep(dc, dw, dh, starttime, time, productive);
			starttime=time;
		}

		it++;
	}
}

void THISCLASS::DrawComponentStep(wxPaintDC &dc, int dw, int dh, double starttime, double stoptime, bool productive) {
	if (selected) {
		dc.SetBrush(wxBrush(wxColour(0x33, 0x33, 0xcc)));
		dc.SetPen(wxBrush(wxColour(0x22, 0x22, 0x88)));
	} else {
		dc.SetBrush(wxBrush(wxColour(0x99, 0xcc, 0x00)));
		dc.SetPen(wxBrush(wxColour(0x66, 0x88, 0x00)));
	}
	int startx=((starttime<0) ? -5 : (int)floor(starttime*mViewScale+mViewOffset+0.5));
	int stopx=((stoptime<0) ? dw+5 : (int)floor(stoptime*mViewScale+mViewOffset+0.5));
	dc.DrawRoundedRectangle(startx, dh-15, stopx, dh+5, 5);
}

void THISCLASS::DrawStepLapTime(wxPaintDC &dc, SwisTrackCoreEventRecorder::Timeline *timeline) {
	// Prepare
	wxSize dcsize=dc.GetSize();
	int dw=dcsize.GetWidth();
	int dh=dcsize.GetHeight();
	dc.SetBrush(wxBrush(wxColour(0x00, 0x00, 0x00)));
	dc.SetPen(wxBrush(wxColour(0x00, 0x00, 0x00)));

	// Draw
	double starttime=-1;
	double productive=timeline->mBeginState->mIsStartedInProductiveMode;
	SwisTrackCoreEventRecorder::Timeline::Eventlist::iterator it=timeline->mEvents.begin();
	while (it!=timeline->mEvents.end()) {
		if (it->mType == SwisTrackCoreEventRecorder::sType_StepLapTime) {
			double time=SwisTrackCoreEventRecorder::CalculateDuration(&(timeline->mBegin), &(*it));
			DrawStartStop(dc, dw, dh, starttime, time, productive);
		}

		it++;
	}
}

void THISCLASS::DrawStepLapTime(wxPaintDC &dc, int dw, int dh, double time) {
	int x=((starttime<0) ? -5 : (int)floor(time*mViewScale+mViewOffset+0.5));
	dc.DrawLine(x, dh-13, x, dh);
}

void THISCLASS::DrawStep(wxPaintDC &dc, SwisTrackCoreEventRecorder::Timeline *timeline) {
	// Prepare
	wxSize dcsize=dc.GetSize();
	int dw=dcsize.GetWidth();
	int dh=dcsize.GetHeight();
	dc.SetBrush(wxBrush(wxColour(0x00, 0xcc, 0x00)));
	dc.SetPen(wxBrush(wxColour(0x00, 0x88, 0x00)));

	// Draw
	double starttime=-1;
	double productive=timeline->mBeginState->mIsStartedInProductiveMode;
	SwisTrackCoreEventRecorder::Timeline::Eventlist::iterator it=timeline->mEvents.begin();
	while (it!=timeline->mEvents.end()) {
		if ((it->mType == SwisTrackCoreEventRecorder::sType_StepStart) && (it->mComponent==0)) {
			starttime=time;
		} else if ((it->mType == SwisTrackCoreEventRecorder::sType_StepStop) && (it->mComponent==0)) {
			double time=SwisTrackCoreEventRecorder::CalculateDuration(&(timeline->mBegin), &(*it));
			DrawStep(dc, dw, dh, starttime, time, productive);
			starttime=time;
		}

		it++;
	}
}

void THISCLASS::DrawStep(wxPaintDC &dc, int dw, int dh, double starttime, double stoptime, bool productive) {
	int startx=((starttime<0) ? -5 : (int)floor(starttime*mViewScale+mViewOffset+0.5));
	int stopx=((stoptime<0) ? dw+5 : (int)floor(stoptime*mViewScale+mViewOffset+0.5));
	dc.DrawRoundedRectangle(startx, dh-10, stopx, dh+5, 5);
}

void THISCLASS::DrawStartStop(wxPaintDC &dc, SwisTrackCoreEventRecorder::Timeline *timeline) {
	// Prepare
	wxSize dcsize=dc.GetSize();
	int dw=dcsize.GetWidth();
	int dh=dcsize.GetHeight();

	// Draw
	double starttime=-1;
	double productive=timeline->mBeginState->mIsStartedInProductiveMode;
	SwisTrackCoreEventRecorder::Timeline::Eventlist::iterator it=timeline->mEvents.begin();
	while (it!=timeline->mEvents.end()) {
		if (it->mType == SwisTrackCoreEventRecorder::sType_SetModeProductive) {
			productive=true;
		} else if (it->mType == SwisTrackCoreEventRecorder::sType_SetModeNormal) {
			productive=false;
		} else if (it->mType == SwisTrackCoreEventRecorder::sType_BeforeStart) {
			starttime=SwisTrackCoreEventRecorder::CalculateDuration(&(timeline->mBegin), &(*it));
		} else if (it->mType == SwisTrackCoreEventRecorder::sType_AfterStop) {
			double time=SwisTrackCoreEventRecorder::CalculateDuration(&(timeline->mBegin), &(*it));
			DrawStartStop(dc, dw, dh, starttime, time, productive);
			starttime=time;
		}

		it++;
	}

	// Draw last event
	if ((starttime>=0) || (timeline->mBeginState.mStarted)) {
		double time=SwisTrackCoreEventRecorder::CalculateDuration(&(timeline->mBegin), &(timeline->mEnd));
		DrawTrigger(dc, dw, dh, starttime, time);
	}
}

void THISCLASS::DrawStartStop(wxPaintDC &dc, int dw, int dh, double starttime, double stoptime, bool productive) {
	if (productive) {
		dc.SetBrush(wxBrush(wxColour(0x00, 0xcc, 0x00)));
		dc.SetPen(wxBrush(wxColour(0x00, 0x88, 0x00)));
	} else {
		dc.SetBrush(wxBrush(wxColour(0xcc, 0xcc, 0xcc)));
		dc.SetPen(wxBrush(wxColour(0x88, 0x88, 0x88)));
	}
	int startx=((starttime<0) ? -5 : (int)floor(starttime*mViewScale+mViewOffset+0.5));
	int stopx=((stoptime<0) ? dw+5 : (int)floor(stoptime*mViewScale+mViewOffset+0.5));
	dc.DrawRoundedRectangle(startx, dh-10, stopx, dh+5, 5);
}

void THISCLASS::DrawBeginEnd(wxPaintDC &dc, SwisTrackCoreEventRecorder::Timeline *timeline) {
	// Prepare
	wxSize dcsize=dc.GetSize();
	int dw=dcsize.GetWidth();
	int dh=dcsize.GetHeight();
	dc.SetBrush(wxBrush(wxColour(0x33, 0x33, 0x33)));
	dc.SetPen(wxBrush(wxColour(0x00, 0x00, 0x00)));

	// Calculate
	double totaltime=SwisTrackCoreEventRecorder::CalculateDuration(&(timeline->mBegin), &(timeline->mEnd));
	int beginx=(int)floor(mViewOffset+0.5));
	int endx=(int)floor(totaltime*mViewScale+mViewOffset+0.5));

	// Draw
	if (beginx>0) {
		dc.DrawRectangle(0, -1, beginx, dh);
	}
	if (endx<dw) {
		dc.DrawRectangle(endx, -1, dw, dh);
	}
}

void THISCLASS::OnMouseLeftDown(wxMouseEvent &event) {
	mMoveStartPoint=event.GetX();
	mMoveStarted=true;
}

void THISCLASS::OnMouseLeftUp(wxMouseEvent &event) {
	mMoveStarted=false;
}

void THISCLASS::OnMouseMove(wxMouseEvent &event) {
	if (! mMoveStarted) {return;}
	if (! event.LeftIsDown()) {mMoveStarted=false; return;}
	int nowpoint=event.GetX();

	// Move
	wxPoint oldviewoffset=mViewOffset;
	mViewOffset+=nowpoint-mMoveStartPoint;
	mMoveStartPoint=nowpoint;

	// Check bounds
	wxSize dcsize=this->GetSize();
	double time=SwisTrackCoreEventRecorder::CalculateDuration(&(timeline->mBegin), &(timeline->mEnd));
	int timelinewidth=(int)floor(time*mViewScale+mViewOffset+0.5);
	if (mViewOffset<dcsize.GetWidth()-timelinewidth) {mViewOffset=dcsize.GetWidth()-timelinewidth;}
	if (mViewOffset>0) {mViewOffset=0;}

	// Redraw if necessary
	if (mViewOffset==oldviewoffset) {return;}
	Refresh(true);
}

void THISCLASS::OnMouseRightDown(wxMouseEvent &event) {
	mPopupMenu.Check(sID_Trigger10, (mTimer->IsRunning() && (mTimer->GetInterval()==10000)));
	mPopupMenu.Check(sID_Trigger1, (mTimer->IsRunning() && (mTimer->GetInterval()==1000)));
	mPopupMenu.Check(sID_TriggerManual, (mTimer->IsRunning()==false));

	PopupMenu(&mPopupMenu);
}

void THISCLASS::OnSize(wxSizeEvent &event) {
	Refresh(true);
}

void THISCLASS::OnMenuViewReset(wxCommandEvent& event) {

}

void THISCLASS::OnMenuTrigger(wxCommandEvent& event) {
	if (mTimer.IsRunning()) {mTimer.stop();}
	if (event.GetId()==sID_Trigger10) {
		mTimer.start(10000, false);
	} else if (event.GetId()==sID_Trigger1) {
		mTimer.start(1000, false);
	} else {
		// Manual mode, do nothing
	}
	Refresh();
}

void THISCLASS::OnMenuSaveTimeline(wxCommandEvent& event) {
	// Get the timeline
	SwisTrackCoreEventRecorder::Timeline *timeline=mSwisTrack->mSwisTrackCore->mTimeline->GetLastTimeline();
	if (timeline==0) {return;}

	// Copy the current timeline (since the file dialog will allow other threads to run)
	SwisTrackCoreEventRecorder::Timeline timelinecopy=*timeline;

	// Show the file save dialog
	wxFileDialog *dlg = new wxFileDialog(this, "Save timeline", "", "", "Text (*.txt)|*.txt", wxSAVE, wxDefaultPosition);
	if (dlg->ShowModal() != wxID_OK) {return;}

	// Save the timeline
	wxString filename=dlg->GetPath();
	std::ofstream ofs(filename.c_str());
	if(! ofs.is_open()) {
		wxMessageDialog dlg(this, "The file could not be saved!", "Save timeline", wxOK);
		dlg.ShowModal();
		return;
	}

	// Write each event
	SwisTrackCoreEventRecorder::Timeline::Eventlist::iterator it=timelinecopy.mEvents.begin();
	while (it!=timelinecopy.mEvents.end()) {
		double time=SwisTrackCoreEventRecorder::CalculateDuration(&(timelinecopy.mStart), &(*it));
		ofs << time;

		std::string type=SwisTrackCoreEventRecorder::mTypeNames[it->mType];
		if (type=="") {
			ofs << "\t" << "(unknown)";
		} else {
			ofs << "\t" << type;
		}

		if (it->mComponent) {
			ofs << "\t" << it->mComponent->mName;
		}

		ofs << std::endl;
		it++;
	}
	
	// Close
	ofs.close();
}
