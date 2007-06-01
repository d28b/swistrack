#include "TimelinePanel.h"
#define THISCLASS TimelinePanel

#include <wx/image.h>
#include <highgui.h>
#include <fstream>
#include "ImageConversion.h"

BEGIN_EVENT_TABLE(THISCLASS, wxControl)
    EVT_PAINT(THISCLASS::OnPaint)
	EVT_ERASE_BACKGROUND(THISCLASS::OnEraseBackground)
	EVT_SIZE(THISCLASS::OnSize)
	EVT_LEFT_DCLICK(THISCLASS::OnMouseLeftDoubleClick)
    EVT_LEFT_DOWN(THISCLASS::OnMouseLeftDown)
	EVT_LEFT_UP(THISCLASS::OnMouseLeftUp)
	EVT_RIGHT_DOWN(THISCLASS::OnMouseRightDown)
	EVT_MOTION(THISCLASS::OnMouseMove)
	EVT_MENU(sID_TriggerAuto10, THISCLASS::OnMenuTrigger)
	EVT_MENU(sID_TriggerAuto1, THISCLASS::OnMenuTrigger)
	EVT_MENU(sID_TriggerManual, THISCLASS::OnMenuTrigger)
	EVT_MENU(sID_ViewReset, THISCLASS::OnMenuView)
	EVT_MENU(sID_ViewZoomIn, THISCLASS::OnMenuView)
	EVT_MENU(sID_ViewZoomOut, THISCLASS::OnMenuView)
	EVT_MENU(sID_SaveTimeline, THISCLASS::OnMenuSaveTimeline)
END_EVENT_TABLE()

THISCLASS::TimelinePanel(wxWindow *parent, SwisTrack *st):
		wxControl(parent, -1), mSwisTrack(st), mSelectedComponent(0), mPopupMenu(), mTimer(this),
		mViewOffset(0), mViewScale(1000), mMoveStarted(false), mMoveStartPoint(0) {

	SetWindowStyle(wxNO_BORDER);
	SetSize(wxSize(20, 20));
	SetMinSize(wxSize(20, 20));

	// Create the popup menu
	mPopupMenu.AppendCheckItem(sID_TriggerAuto10, "Update every 10 seconds");
	mPopupMenu.AppendCheckItem(sID_TriggerAuto1, "Update every second");
	mPopupMenu.AppendCheckItem(sID_TriggerManual, "Update manually (double click)");
	mPopupMenu.AppendSeparator();
	mPopupMenu.AppendCheckItem(sID_ViewReset, "Reset view");
	mPopupMenu.AppendCheckItem(sID_ViewZoomIn, "Zoom in");
	mPopupMenu.AppendCheckItem(sID_ViewZoomOut, "Zoom out");
	mPopupMenu.AppendSeparator();
	mPopupMenu.Append(sID_SaveTimeline, "Save timeline as ...");

	// Set non-bold font
	wxFont f=GetFont();
	f.SetPointSize(8);
	f.SetWeight(wxNORMAL);

	// Start the timer
	mTimer.Start(1000, false);
}

THISCLASS::~TimelinePanel() {
}

void THISCLASS::StartRecording() {
	mSwisTrack->mSwisTrackCore->mEventRecorder->StartRecording();
	Refresh(true);
}

void THISCLASS::OnPaint(wxPaintEvent& WXUNUSED(event)) {
	wxPaintDC dc(this);
	bool ok=Paint(dc);

	// Display an error if no image can be displayed
	if (! ok) {
		dc.SetBackground(wxBrush(wxColour(0x33, 0x33, 0x33)));
		dc.SetTextForeground(wxColour(0xff, 0xff, 0xff));
		dc.Clear();
		dc.SetFont(GetFont());
		dc.DrawText("No timeline.", 4, 4);
	}
}

bool THISCLASS::Paint(wxPaintDC &dc) {
	// Get timeline
	const SwisTrackCoreEventRecorder::Timeline *timeline=mSwisTrack->mSwisTrackCore->mEventRecorder->GetLastTimeline();
	if (timeline==0) {return false;}

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

	return true;
}

void THISCLASS::DrawTrigger(wxPaintDC &dc, const SwisTrackCoreEventRecorder::Timeline *timeline) {
	// Prepare
	wxSize dcsize=dc.GetSize();
	int dw=dcsize.GetWidth();
	int dh=dcsize.GetHeight();
	dc.SetBrush(wxBrush(wxColour(0xee, 0xee, 0xee)));
	dc.SetPen(wxPen(wxColour(0xee, 0xee, 0xee)));

	// Draw
	double starttime=-1;
	bool active=timeline->mBeginState.mTriggerActive;
	SwisTrackCoreEventRecorder::Timeline::tEventList::const_iterator it=timeline->mEvents.begin();
	while (it!=timeline->mEvents.end()) {
		if (it->mType == SwisTrackCoreEventRecorder::sType_BeforeTriggerStart) {
			starttime=mSwisTrack->mSwisTrackCore->mEventRecorder->CalculateDuration(&(timeline->mBegin), &(*it));;
			active=true;
		} else if (it->mType == SwisTrackCoreEventRecorder::sType_AfterTriggerStop) {
			double time=mSwisTrack->mSwisTrackCore->mEventRecorder->CalculateDuration(&(timeline->mBegin), &(*it));
			DrawTrigger(dc, dw, dh, starttime, time);
			starttime=time;
			active=false;
		}

		it++;
	}

	// Draw last event
	if (active) {
		DrawTrigger(dc, dw, dh, starttime, -1);
	}
}

void THISCLASS::DrawTrigger(wxPaintDC &dc, int dw, int dh, double starttime, double stoptime) {
	int startx=((starttime<0) ? -5 : (int)floor(starttime*mViewScale+mViewOffset+0.5));
	int stopx=((stoptime<0) ? dw+5 : (int)floor(stoptime*mViewScale+mViewOffset+0.5));
	dc.DrawRectangle(startx, 0, stopx-startx, dh);
}

void THISCLASS::DrawTicks(wxPaintDC &dc, const SwisTrackCoreEventRecorder::Timeline *timeline) {
	wxSize dcsize=dc.GetSize();
	int dw=dcsize.GetWidth();
	int dh=dcsize.GetHeight();

	dc.SetFont(GetFont());
	dc.SetPen(wxPen(wxColour(0xcc, 0xcc, 0xcc)));
	dc.SetTextForeground(wxColour(0xcc, 0xcc, 0xcc));

	// Calculate how many ticks to display
	double mintickdistance=5;
	double mintickdistancetime=mintickdistance/mViewScale;
	double tickdistancetime=pow(10, ceil(log(mintickdistancetime)/log((double)10)));

	// Draw ticks
	double endtime=mSwisTrack->mSwisTrackCore->mEventRecorder->CalculateDuration(&(timeline->mBegin), &(timeline->mEnd));
	int ticknumber=(int)ceil(-mViewOffset/mViewScale/tickdistancetime);
	while (true) {
		double xtime=ticknumber*tickdistancetime;
		if (xtime>=endtime) {break;}
		int x=(int)floor(xtime*mViewScale+mViewOffset);
		if (x>dw) {break;}

		if (ticknumber % 10 == 0) {
			dc.DrawLine(x, 0, x, 4);
			wxString label=wxString::Format("%d", (int)(xtime*1000));
			int textwidth, textheight;
			GetTextExtent(label, &textwidth, &textheight) ;
			textwidth >>= 1;
			dc.DrawText(label, x-textwidth, 2);
		} else {
			dc.DrawLine(x, 0, x, 2);
		}

		ticknumber++;
	}
}

void THISCLASS::DrawComponentSteps(wxPaintDC &dc, const SwisTrackCoreEventRecorder::Timeline *timeline) {
	// Prepare
	wxSize dcsize=dc.GetSize();
	int dw=dcsize.GetWidth();
	int dh=dcsize.GetHeight();

	// Draw
	double starttime=-1;
	SwisTrackCoreEventRecorder::Timeline::tEventList::const_iterator it=timeline->mEvents.begin();
	while (it!=timeline->mEvents.end()) {
		if ((it->mType == SwisTrackCoreEventRecorder::sType_StepStart) && (it->mComponent)) {
			starttime=mSwisTrack->mSwisTrackCore->mEventRecorder->CalculateDuration(&(timeline->mBegin), &(*it));
		} else if ((it->mType == SwisTrackCoreEventRecorder::sType_StepStop) && (it->mComponent)) {
			double time=mSwisTrack->mSwisTrackCore->mEventRecorder->CalculateDuration(&(timeline->mBegin), &(*it));
			DrawComponentStep(dc, dw, dh, starttime, time, false);
			starttime=time;
		}

		it++;
	}
}

void THISCLASS::DrawComponentStep(wxPaintDC &dc, int dw, int dh, double starttime, double stoptime, bool selected) {
	if (selected) {
		dc.SetBrush(wxBrush(wxColour(0x33, 0x33, 0xcc)));
		dc.SetPen(wxPen(wxColour(0x22, 0x22, 0x88)));
	} else {
		dc.SetBrush(wxBrush(wxColour(0x99, 0xcc, 0x00)));
		dc.SetPen(wxPen(wxColour(0x66, 0x88, 0x00)));
	}
	int startx=((starttime<0) ? -5 : (int)floor(starttime*mViewScale+mViewOffset+0.5));
	int stopx=((stoptime<0) ? dw+5 : (int)floor(stoptime*mViewScale+mViewOffset+0.5));
	dc.DrawRectangle(startx, dh-15, stopx-startx, 20);
}

void THISCLASS::DrawStepLapTimes(wxPaintDC &dc, const SwisTrackCoreEventRecorder::Timeline *timeline) {
	// Prepare
	wxSize dcsize=dc.GetSize();
	int dw=dcsize.GetWidth();
	int dh=dcsize.GetHeight();
	dc.SetBrush(wxBrush(wxColour(0x00, 0x00, 0x00)));
	dc.SetPen(wxPen(wxColour(0x00, 0x00, 0x00)));

	// Draw
	double starttime=-1;
	SwisTrackCoreEventRecorder::Timeline::tEventList::const_iterator it=timeline->mEvents.begin();
	while (it!=timeline->mEvents.end()) {
		if (it->mType == SwisTrackCoreEventRecorder::sType_StepLapTime) {
			double time=mSwisTrack->mSwisTrackCore->mEventRecorder->CalculateDuration(&(timeline->mBegin), &(*it));
			DrawStepLapTime(dc, dw, dh, time);
		}

		it++;
	}
}

void THISCLASS::DrawStepLapTime(wxPaintDC &dc, int dw, int dh, double time) {
	int x=((time<0) ? -5 : (int)floor(time*mViewScale+mViewOffset+0.5));
	dc.DrawLine(x, dh-13, x, dh);
}

void THISCLASS::DrawSteps(wxPaintDC &dc, const SwisTrackCoreEventRecorder::Timeline *timeline) {
	// Prepare
	wxSize dcsize=dc.GetSize();
	int dw=dcsize.GetWidth();
	int dh=dcsize.GetHeight();
	dc.SetBrush(wxBrush(wxColour(0x00, 0xcc, 0x00)));
	dc.SetPen(wxPen(wxColour(0x00, 0x88, 0x00)));

	// Draw
	double starttime=-1;
	SwisTrackCoreEventRecorder::Timeline::tEventList::const_iterator it=timeline->mEvents.begin();
	while (it!=timeline->mEvents.end()) {
		if ((it->mType == SwisTrackCoreEventRecorder::sType_StepStart) && (it->mComponent==0)) {
			starttime=mSwisTrack->mSwisTrackCore->mEventRecorder->CalculateDuration(&(timeline->mBegin), &(*it));
		} else if ((it->mType == SwisTrackCoreEventRecorder::sType_StepStop) && (it->mComponent==0)) {
			double time=mSwisTrack->mSwisTrackCore->mEventRecorder->CalculateDuration(&(timeline->mBegin), &(*it));
			DrawStep(dc, dw, dh, starttime, time);
			starttime=time;
		}

		it++;
	}
}

void THISCLASS::DrawStep(wxPaintDC &dc, int dw, int dh, double starttime, double stoptime) {
	int startx=((starttime<0) ? -5 : (int)floor(starttime*mViewScale+mViewOffset+0.5));
	int stopx=((stoptime<0) ? dw+5 : (int)floor(stoptime*mViewScale+mViewOffset+0.5));
	dc.DrawRectangle(startx, dh-8, stopx-startx, 15);
}

void THISCLASS::DrawStartStop(wxPaintDC &dc, const SwisTrackCoreEventRecorder::Timeline *timeline) {
	// Prepare
	wxSize dcsize=dc.GetSize();
	int dw=dcsize.GetWidth();
	int dh=dcsize.GetHeight();

	// Draw
	double starttime=-1;
	bool started=timeline->mBeginState.mStarted;
	bool productive=timeline->mBeginState.mStartedInProductiveMode;
	SwisTrackCoreEventRecorder::Timeline::tEventList::const_iterator it=timeline->mEvents.begin();
	while (it!=timeline->mEvents.end()) {
		if (it->mType == SwisTrackCoreEventRecorder::sType_SetModeProductive) {
			productive=true;
		} else if (it->mType == SwisTrackCoreEventRecorder::sType_SetModeNormal) {
			productive=false;
		} else if (it->mType == SwisTrackCoreEventRecorder::sType_BeforeStart) {
			starttime=mSwisTrack->mSwisTrackCore->mEventRecorder->CalculateDuration(&(timeline->mBegin), &(*it));
			started=true;
		} else if (it->mType == SwisTrackCoreEventRecorder::sType_AfterStop) {
			double time=mSwisTrack->mSwisTrackCore->mEventRecorder->CalculateDuration(&(timeline->mBegin), &(*it));
			DrawStartStop(dc, dw, dh, starttime, time, productive);
			starttime=time;
			started=false;
		}

		it++;
	}

	// Draw last event
	if (started) {
		//double time=mSwisTrack->mSwisTrackCore->mEventRecorder->CalculateDuration(&(timeline->mBegin), &(timeline->mEnd));
		DrawStartStop(dc, dw, dh, starttime, -1, productive);
	}
}

void THISCLASS::DrawStartStop(wxPaintDC &dc, int dw, int dh, double starttime, double stoptime, bool productive) {
	if (productive) {
		dc.SetBrush(wxBrush(wxColour(0x00, 0xcc, 0x00)));
		dc.SetPen(wxPen(wxColour(0x00, 0x88, 0x00)));
	} else {
		dc.SetBrush(wxBrush(wxColour(0xcc, 0xcc, 0xcc)));
		dc.SetPen(wxPen(wxColour(0x88, 0x88, 0x88)));
	}
	int startx=((starttime<0) ? -5 : (int)floor(starttime*mViewScale+mViewOffset+0.5));
	int stopx=((stoptime<0) ? dw+5 : (int)floor(stoptime*mViewScale+mViewOffset+0.5));
	dc.DrawRoundedRectangle(startx, dh-6, stopx-startx, 15, 5);
}

void THISCLASS::DrawBeginEnd(wxPaintDC &dc, const SwisTrackCoreEventRecorder::Timeline *timeline) {
	// Prepare
	wxSize dcsize=dc.GetSize();
	int dw=dcsize.GetWidth();
	int dh=dcsize.GetHeight();
	dc.SetBrush(wxBrush(wxColour(0x33, 0x33, 0x33)));
	dc.SetPen(wxPen(wxColour(0x00, 0x00, 0x00)));

	// Calculate
	double totaltime=mSwisTrack->mSwisTrackCore->mEventRecorder->CalculateDuration(&(timeline->mBegin), &(timeline->mEnd));
	int beginx=(int)floor(mViewOffset+0.5);
	int endx=(int)floor(totaltime*mViewScale+mViewOffset+0.5);

	// Draw
	if (beginx>0) {
		dc.DrawRectangle(0, -1, beginx, dh);
	}
	if (endx<dw) {
		dc.DrawRectangle(endx, -1, dw-endx+1, dh+1);
	}
}

void THISCLASS::OnMouseLeftDoubleClick(wxMouseEvent &event) {
	if (mTimer.IsRunning()) {return;}
	StartRecording();
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
	int oldviewoffset=mViewOffset;
	mViewOffset+=nowpoint-mMoveStartPoint;
	mMoveStartPoint=nowpoint;

	// Check bounds
	//wxSize dcsize=this->GetSize();
	//double time=mSwisTrack->mSwisTrackCore->mEventRecorder->CalculateDuration(&(timeline->mBegin), &(timeline->mEnd));
	//int timelinewidth=(int)floor(time*mViewScale+mViewOffset+0.5);
	//if (mViewOffset<dcsize.GetWidth()-timelinewidth) {mViewOffset=dcsize.GetWidth()-timelinewidth;}
	if (mViewOffset>0) {mViewOffset=0;}

	// Redraw if necessary
	if (mViewOffset==oldviewoffset) {return;}
	Refresh(true);
}

void THISCLASS::OnMouseRightDown(wxMouseEvent &event) {
	// Set the checkmarks
	mPopupMenu.Check(sID_TriggerAuto10, (mTimer.IsRunning() && (mTimer.GetInterval()==10000)));
	mPopupMenu.Check(sID_TriggerAuto1, (mTimer.IsRunning() && (mTimer.GetInterval()==1000)));
	mPopupMenu.Check(sID_TriggerManual, (mTimer.IsRunning()==false));

	// Store the mouse position
	mMoveStartPoint=event.GetX();

	// Show the popup
	PopupMenu(&mPopupMenu);
}

void THISCLASS::OnSize(wxSizeEvent &event) {
	Refresh(true);
}

void THISCLASS::OnMenuView(wxCommandEvent& event) {
	if (event.GetId()==sID_ViewZoomOut) {
		mViewOffset=0;
		mViewScale/=10;
	} else if (event.GetId()==sID_ViewZoomIn) {
		mViewOffset=0;
		mViewScale*=10;
	} else {
		// Reset
		mViewOffset=0;
		mViewScale=1000;
	}
	Refresh(true);
}

void THISCLASS::OnMenuTrigger(wxCommandEvent& event) {
	if (mTimer.IsRunning()) {mTimer.Stop();}
	if (event.GetId()==sID_TriggerAuto10) {
		mTimer.Start(10000, false);
	} else if (event.GetId()==sID_TriggerAuto1) {
		mTimer.Start(1000, false);
	} else {
		// Manual mode, do nothing
	}
	Refresh();
}

void THISCLASS::OnMenuSaveTimeline(wxCommandEvent& event) {
	// Get the timeline
	const SwisTrackCoreEventRecorder::Timeline *timeline=mSwisTrack->mSwisTrackCore->mEventRecorder->GetLastTimeline();
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
	SwisTrackCoreEventRecorder::Timeline::tEventList::iterator it=timelinecopy.mEvents.begin();
	while (it!=timelinecopy.mEvents.end()) {
		double time=mSwisTrack->mSwisTrackCore->mEventRecorder->CalculateDuration(&(timelinecopy.mBegin), &(*it));
		ofs << time;

		std::string type=mSwisTrack->mSwisTrackCore->mEventRecorder->mTypeNames[it->mType];
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
