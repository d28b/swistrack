#include "SwisTrackCoreEventRecorder.h"
#define THISCLASS SwisTrackCoreEventRecorder

#include "SwisTrackCore.h"

THISCLASS::SwisTrackCoreEventRecorder(SwisTrackCore *stc):
		mSwisTrackCore(stc), mFrequency(0), mCurrentTimeline(0), mLastTimeline(0), mStepDistance(0), mPreviousStep() {

	// Store the frequency of the counter
#ifdef __WXMSW__
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	mFrequency=(double)frequency.LowPart;
#endif

	// Set the previous step to sType_None, indicating that there was no such step
	mPreviousStep.mType=sType_None;

	// Type names
	mTypeNames[sType_None]="None";
	mTypeNames[sType_SetModeProductive]="SetModeProductive";
	mTypeNames[sType_SetModeNormal]="SetModeNormal";
	mTypeNames[sType_BeforeStart]="BeforeStart";
	mTypeNames[sType_AfterStart]="AfterStart";
	mTypeNames[sType_BeforeStop]="BeforeStop";
	mTypeNames[sType_AfterStop]="AfterStop";
	mTypeNames[sType_BeforeTriggerStart]="BeforeTriggerStart";
	mTypeNames[sType_AfterTriggerStart]="AfterTriggerStart";
	mTypeNames[sType_BeforeTriggerStop]="BeforeTriggerStop";
	mTypeNames[sType_AfterTriggerStop]="AfterTriggerStop";
	mTypeNames[sType_StepStart]="StepStart";
	mTypeNames[sType_StepStop]="StepStop";
	mTypeNames[sType_StepLapTime]="StepLapTime";
}

THISCLASS::~SwisTrackCoreEventRecorder() {
	delete mCurrentTimeline;
	delete mLastTimeline;
}

void THISCLASS::StartRecording() {
	// Set the end time of the current timeline
	if (mCurrentTimeline) {
		if (mCurrentTimeline->mEvents.size() >= Timeline::mNumberOfEvents) {
			mCurrentTimeline->mEvents.back().mType=sType_TimelineOverflow;
			mCurrentTimeline->mEvents.back().mComponent=0;
		}
		LapTime(&(mCurrentTimeline->mEnd), sType_None, 0);
	}

	// Swap current and old timeline
	Timeline *temp=mLastTimeline;
	mLastTimeline=mCurrentTimeline;
	mCurrentTimeline=temp;

	// Initialize the current timeline
	if (! mCurrentTimeline) {
		mCurrentTimeline=new Timeline();
	}
	mCurrentTimeline->mEvents.clear();
	mCurrentTimeline->mEvents.reserve(Timeline::mNumberOfEvents);

	// Set the start state
	mCurrentTimeline->mBeginState.mStartedInProductiveMode=mSwisTrackCore->IsStartedInProductiveMode();
	mCurrentTimeline->mBeginState.mStarted=mSwisTrackCore->IsStarted();
	mCurrentTimeline->mBeginState.mTriggerActive=mSwisTrackCore->IsTriggerActive();
	
	// Set the start time of the new timeline
	LapTime(&(mCurrentTimeline->mBegin), sType_None, 0);
}

void THISCLASS::LapTime(Event *it, eType type, Component *c) {
#ifdef __WXMSW__
	QueryPerformanceCounter(&(it->mTime));
#else
	it.mTime=0;
#endif
	it->mType=type;
	it->mComponent=c;
}

void THISCLASS::Add(const Event *it) {
	if (! mCurrentTimeline) {return;}
	if (mCurrentTimeline->mEvents.size() >= Timeline::mNumberOfEvents) {return;}
	mCurrentTimeline->mEvents.push_back(*it);
}

void THISCLASS::Add(eType type, Component *component) {
	if (! mCurrentTimeline) {return;}
	if (mCurrentTimeline->mEvents.size() >= Timeline::mNumberOfEvents) {return;}

	Event it;
	LapTime(&it, type, component);
	Add(&it);
}

void THISCLASS::AddStepStart() {
	// Take a measurement
	Event it;
	LapTime(&it, sType_StepStart, 0);
	Add(&it);

	// Update the frames per second
	if (mPreviousStep.mType==sType_StepStart) {
		double currentstepdistance=CalculateDuration(&mPreviousStep, &it);
		double w=0.5;
		mStepDistance=mStepDistance*w+currentstepdistance*(1-w);
	}

	// Set the previous step
	mPreviousStep=it;
}

double THISCLASS::CalculateDuration(const Event *it1, const Event *it2) const {
	double diff=(double)(it2->mTime.QuadPart-it1->mTime.QuadPart);
	return diff/mFrequency;
}