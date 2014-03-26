#ifndef HEADER_SwisTrackCoreEventRecorder
#define HEADER_SwisTrackCoreEventRecorder

// We need to include wx.h here, although LARGE_INTEGER is defined in windows.h. First of all, this automatically works on all platforms, but second, windows.h defines an ugly marco #define GetClassInfo GetClassInfoA (and doesn undef it) which causes troubles all over the place - a nice jest of the skilled programmers in Redmond.
#include <wx/wx.h>

class SwisTrackCoreEventRecorder;
class SwisTrackCore;
#include "Component.h"

//! Records event timings. An object of this class is directly attached to the SwisTrackCore object. This class uses the QueryPerformaceCounter function of the Windows API and is therefore not portable.
class SwisTrackCoreEventRecorder {

public:
#ifdef __WXMSW__
	typedef LARGE_INTEGER tTime;
#else
	typedef struct timeval tTime;
#endif

	//! Types of timeline items.
	enum eType {
		sType_None,
		sType_TimelineOverflow,
		sType_SetModeProduction,
		sType_SetModeNormal,
		sType_BeforeStart,
		sType_AfterStart,
		sType_BeforeStop,
		sType_AfterStop,
		sType_BeforeReloadConfiguration,
		sType_AfterReloadConfiguration,
		sType_BeforeTriggerStart,
		sType_AfterTriggerStart,
		sType_BeforeTriggerStop,
		sType_AfterTriggerStop,
		sType_StepStart,
		sType_StepStop,
		sType_StepLapTime,
	};

	//! A map type to map event types to names.
	typedef std::map<eType, wxString> tTypeNameMap;

	//! A timeline item.
	class Event {
	public:
		tTime mTime;				//!< The time of the event.
		eType mType;				//!< The event type.
		Component *mComponent;		//!< The associated component (if any).
	};

	//! A class holding the state of SwisTrackCore at a specific time instance.
	class State {
	public:
		bool mStartedInProductionMode;		//!< Whether started in production mode.
		bool mStarted;						//!< Whether started at all.
		bool mTriggerActive;				//!< Whether the automatic trigger is active.
	};

	//! A timeline of finite length (the mEvents vector will not grow over a certain length).
	class Timeline {
	public:
		//! An event list type.
		typedef std::vector<Event> tEventList;

		static const unsigned int mNumberOfEvents = 1000;	//!< The number of events stored per timeline.
		Event mBegin;										//!< Holds the time at which the timeline begins.
		Event mEnd;											//!< Holds the time at which the timeline end.
		State mBeginState;									//!< The state of the SwisTrackCore object at time of starting.
		tEventList mEvents;									//!< The recorded events. This vector is initialized to a certain size
	};

	SwisTrackCore *mSwisTrackCore;				//!< The associated SwisTrackCore object.
	tTypeNameMap mTypeNames;					//!< String names of the event types.

	//! Constructor.
	SwisTrackCoreEventRecorder(SwisTrackCore *stc);
	//! Destructor.
	~SwisTrackCoreEventRecorder();

	//! Starts a new recording and makes the old recording available through GetLastTimeline().
	void StartRecording();
	//! Adds a new event to the current timeline.
	void Add(const eType type, Component *component = 0);
	//! Adds a new event to the current timeline.
	void Add(const Event *it);
	//! Adds a sType_StepStart event and keeps track of the FPS.
	void AddStepStart();
	//! Stores an event in an Item object.
	static void LapTime(Event *it, const eType type, Component *component = 0);
	//! Returns the duration (in seconds) between two timeline items.
	double CalculateDuration(const Event *it1, const Event *it2) const;

	//! Returns the last timeline.
	const Timeline *GetLastTimeline() const {
		return mLastTimeline;
	}
	//! Returns the frames per seconds.
	double GetFramesPerSecond() const {
		return 1 / mStepDistance;
	}
	//! Returns the average distance between steps.
	double GetStepDistance() const {
		return mStepDistance;
	}

private:
#ifdef __WXMSW__
	double mFrequency;				//!< The frequency of the performance counter.
#endif
	Timeline *mCurrentTimeline;		//!< The timeline which currently records events.
	Timeline *mLastTimeline;		//!< The previously recorded timeline.
	Event mPreviousStep;			//!< Timestamp of the previous step.
	double mStepDistance;			//!< The distance (time) between two steps.

};

#endif
