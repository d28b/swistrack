#ifndef HEADER_ComponentTriggerTimer
#define HEADER_ComponentTriggerTimer

#include "Component.h"
#include "Display.h"
#include <wx/timer.h>

//! A timer trigger.
class ComponentTriggerTimer: public Component {

public:
	//! Constructor.
	ComponentTriggerTimer(SwisTrackCore *stc);
	//! Destructor.
	~ComponentTriggerTimer();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentTriggerTimer(mCore);
	}

private:
	//! An overwritten wxTimer.
class Timer: public wxTimer {
	public:
		ComponentTriggerTimer *mComponentTriggerTimer;		//!< The associated component.

		//! Constructor.
		Timer(ComponentTriggerTimer *ctt): wxTimer(), mComponentTriggerTimer(ctt) {}
		//! Destructor.
		~Timer() {}

		// wxTimer methods
		void Notify() {
			mComponentTriggerTimer->mTrigger->SetReady();
		}
	};

	Timer *mTimer;		//!< The associated timer.
};

#endif
