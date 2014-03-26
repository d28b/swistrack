#ifndef HEADER_SwisTrackCoreInterface
#define HEADER_SwisTrackCoreInterface

class SwisTrackCoreInterface;

#include <wx/string.h>
class Component;

//! A class defining the events of SwisTrackCore.
class SwisTrackCoreInterface {

public:
	//! Constructor.
	SwisTrackCoreInterface() {}
	//! Destructor.
	virtual ~SwisTrackCoreInterface() {}

	//! Called before the components are started.
	virtual void OnBeforeStart(bool productionmode) {}
	//! Called after the components have been started.
	virtual void OnAfterStart(bool productionmode) {}

	//! Called before the components are stopped.
	virtual void OnBeforeStop() {}
	//! Called after the components have been stopped.
	virtual void OnAfterStop() {}

	//! Called before a step is performed.
	virtual void OnBeforeStep() {}
	//! Called when all components have performed the step, but before cleanup. Note that this is when all data structures hold the information that has been used for this step.
	virtual void OnStepReady() {}
	//! Called after a step including cleanup has been performed. Hence, the data structures may be empty.
	virtual void OnAfterStep() {}

	//! Called before the component list is modified.
	virtual void OnBeforeEdit() {}
	//! Called after the component list has been modified.
	virtual void OnAfterEdit() {}

	//! Called after a component has been added to the list.
	virtual void OnAfterAddComponent(Component *c) {}
	//! Called before a component is modified.
	virtual void OnBeforeEditComponent(Component *c) {}
	//! Called after a component has been modified.
	virtual void OnAfterEditComponent(Component *c) {}
	//! Called before a component is removed from the list.
	virtual void OnBeforeRemoveComponent(Component *c) {}

	//! Called before the trigger is started.
	virtual void OnBeforeTriggerStart() {}
	//! Called after the trigger has been started.
	virtual void OnAfterTriggerStart() {}

	//! Called before the trigger is stopped.
	virtual void OnBeforeTriggerStop() {}
	//! Called after the trigger has been stopped.
	virtual void OnAfterTriggerStop() {}

};

#endif
