#ifndef HEADER_SwisTrackCoreEventRecorder
#define HEADER_SwisTrackCoreEventRecorder

class SwisTrackCoreEventRecorder;
class SwisTrackCore;
#include "Component.h";

//! Records event timings. An object of this class is directly attached to the SwisTrackCore object. This class uses the QueryPerformaceCounter function of the Windows API and is therefore not portable.
class SwisTrackCoreEventRecorder {

public:
#ifdef __WXMSW__
	typedef LARGE_INTEGER tTime;
#else
	typedef int tTime;
#endif

	//! Types of timeline items.
	enum eType {
		sType_None,
		sType_TimelineOverflow,
		sType_SetModeProductive,
		sType_SetModeNormal,
		sType_BeforeStart,
		sType_AfterStart,
		sType_BeforeStop,
		sType_AfterStop,
		sType_TriggerStart,
		sType_TriggerStop,
		sType_StepStart,
		sType_StepStop,
		sType_StepLapTime,
	};

	static std::map<eType, std::tring> mTypeNames;		//!< String names of the event types.

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
		bool mStartedInProductiveMode;		//!< Whether started in productive mode.
		bool mStarted;						//!< Whether started at all.
		bool mTriggerActive;				//!< Whether the automatic trigger is active.
	}

	//! A timeline of finite length (the mEvents vector will not grow over a certain length).
	class Timeline {
	public:
		//! An event list type.
		typedef std::vector<Event> tEventList;

		static const mNumberOfEvents = 1000;	//!< The number of events stored per timeline.
		Event mBegin;							//!< Holds the time at which the timeline begins.
		Event mEnd;								//!< Holds the time at which the timeline end.
		State mStartState;						//!< The state of the SwisTrackCore object at time of starting.
		tEventList mEvents;						//!< The recorded events. This vector is initialized to a certain size 
	}

	SwisTrackCore *mSwisTrackCore;				//!< The associated SwisTrackCore object.

	//! Constructor.
	SwisTrackCoreEventRecorder(SwisTrackCore *stc);
	//! Destructor.
	~SwisTrackCoreEventRecorder();

	//! Starts a new recording and makes the old recording available through GetLastTimeline().
	void StartRecording();
	//! Adds a new event to the current timeline.
	Item Add(const eType type, Component *c=0);
	//! Adds a new event to the current timeline.
	void Add(Item *it);
	//! Stores an event in an Item object.
	static void LapTime(Item *it, const eType type, Component *c) const;
	//! Returns the duration (in seconds) between two timeline items.
	static double CalculateDuration(Item *it1, Item *it2) const;

	//! Returns the last timeline.
	const TimeLine *GetLastTimeline() const {return mLastTimeLine;}

private:
	double mFrequency;				//!< The frequency of the performance counter.
	TimeLine *mCurrentTimeLine;		//!< The timeline which currently records events.
	TimeLine *mLastTimeLine;		//!< The previously recorded timeline.

};

#endif
