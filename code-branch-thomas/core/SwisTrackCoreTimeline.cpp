#include "SwisTrackCoreEventRecorder.h"
#define THISCLASS SwisTrackCoreEventRecorder

#include "SwisTrackCore.h"

THISCLASS::SwisTrackCoreEventRecorder(SwisTrackCore *stc):
		mSwisTrackCore(stc), mList(), mCurrentTimeline(0), mLastTimeline(0) {

	// Store the frequency of the counter
#ifdef __WXMSW__
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	mFrequency=(double)frequency;
#else
	mFrequency=0;
#endif

	// Type names
	mTypeNames[sType_None]="None";
	mTypeNames[sType_SetModeProductive]="SetModeProductive";
	mTypeNames[sType_SetModeNormal]="SetModeNormal";
	mTypeNames[sType_BeforeStart]="BeforeStart";
	mTypeNames[sType_AfterStart]="AfterStart";
	mTypeNames[sType_BeforeStop]="BeforeStop";
	mTypeNames[sType_AfterStop]="AfterStop";
	mTypeNames[sType_TriggerStart]="TriggerStart";
	mTypeNames[sType_TriggerStop]="TriggerStop";
	mTypeNames[sType_StepStart]="StepStart";
	mTypeNames[sType_StepStop]="StepStop";
	mTypeNames[sType_StepLapTime]="StepLapTime";
}

THISCLASS::~SwisTrackCoreEventRecorder() {
}

void THISCLASS::StartRecording() {
	// Set the end time of the current timeline
	if (mCurrentTimeline) {
		if (mCurrentTimeline->mEvents.size() >= Timeline::mNumberOfEvents) {
			mCurrentTimeline->mEvents.back().mType=sType_TimelineOverflow;
			mCurrentTimeline->mEvents.back().mComponent=0;
		}
		LapTime(&(mCurrentTimeline->mEnd), sNone, 0);
	}

	// Swap current and old timeline
	Timeline temp=mLastTimeline;
	mLastTimeline=mCurrentTimeline;
	mCurrentTimeline=temp;

	// Initialize the current timeline
	if (! mCurrentTimeline) {
		mCurrentTimeline=new Timeline();
	}
	mCurrentTimeline->mEvents.clear();
	mCurrentTimeline->mEvents.reserve(Timeline::mNumberOfEvents);

	// Set the start state
	mCurrentTimeline->mStartState.mStarted.mIsStartedInProductiveMode=mSwisTrackCore->IsStartedInProductiveMode();
	mCurrentTimeline->mStartState.mStarted.mIsStarted=mSwisTrackCore->IsStarted();
	mCurrentTimeline->mStartState.mStarted.mIsTriggerActive=mSwisTrackCore->IsTriggerActive();
	
	// Set the start time of the new timeline
	LapTime(&(mCurrentTimeline->mBegin), sNone, 0);
}

void THISCLASS::LapTime(Item *it, eType type, Component *c) {
#ifdef __WXMSW__
	QueryPerformanceCounter(&(it.mTime));
#else
	it.mTime=0;
#endif
	it.mType=type;
	it.mComponent=c;
}

void THISCLASS::Add(Item *it) {
	if (! mCurrentTimeline) {return;}
	if (mCurrentTimeline->mEvents.size() >= Timeline::mNumberOfEvents) {return;}
	mEvents.push_back(*it);
}

void THISCLASS::Add(eType type, Component *c) {
	if (! mCurrentTimeline) {return;}
	if (mCurrentTimeline->mEvents.size() >= Timeline::mNumberOfEvents) {return;}

	Item it;
	LapTime(&it);
	Add(&it);
}

double THISCLASS::CalculateDuration(Item *it1, Item *it2) {
	double diff=(double)(it2->mTime.QuadPart-it1->mTime.QuadPart);
	return diff/frequency;
}

