#ifndef HEADER_ComponentTriggerCounter
#define HEADER_ComponentTriggerCounter

#include "Component.h"

//! A trigger component which generates only a certain amount of triggers.
class ComponentTriggerCounter: public Component {

public:

	//! Constructor.
	ComponentTriggerCounter(SwisTrackCore *stc);
	//! Destructor.
	~ComponentTriggerCounter();

	// Overwritten Component methods
	void OnStart();
	void OnReloadConfiguration();
	void OnStep();
	void OnStepCleanup();
	void OnStop();
	Component *Create() {
		return new ComponentTriggerCounter(mCore);
	}

private:
	int mCount;				//!< (configuration) The number of steps to perform.
	int mTodo;				//!< The number of steps performed.

};

#endif
