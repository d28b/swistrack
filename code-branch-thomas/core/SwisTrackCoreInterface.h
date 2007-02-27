#ifndef HEADER_SwisTrackCoreInterface
#define HEADER_SwisTrackCoreInterface

class SwisTrackCoreInterface;

#include <string>
#include "DisplayImage.h"

//! A class defining the events of SwisTrackCore.
class SwisTrackCoreInterface {

public:
	//! Constructor.
	SwisTrackCoreInterface() {}
	//! Destructor.
	~SwisTrackCoreInterface() {}

	//! Called before the components are started.
	virtual void OnBeforeStart(bool productivemode) = 0;
	//! Called after the components have been started.
	virtual void OnAfterStart(bool productivemode) = 0;

	//! Called before the components are stopped.
	virtual void OnBeforeStop() = 0;
	//! Called after the components have been stopped.
	virtual void OnAfterStop() = 0;

	//! Called before a step is performed.
	virtual void OnBeforeStep() = 0;
	//! Called when all components have performed the step, but before cleanup. Note that this is when all data structures hold the information that has been used for this step.
	virtual void OnStepReady() = 0;
	//! Called after a step including cleanup has been performed. Hence, the data structures may be empty.
	virtual void OnAfterStep() = 0;

	//! Called before the component list is modified.
	virtual void OnBeforeEdit() = 0;
	//! Called after the component list has been modified.
	virtual void OnAfterEdit() = 0;

	//! Called before a component is modified.
	virtual void OnBeforeEditComponent(Component *c) = 0;
	//! Called after a component has been modified.
	virtual void OnAfterEditComponent(Component *c) = 0;
};

#endif