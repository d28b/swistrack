#ifndef HEADER_ComponentTrigger
#define HEADER_ComponentTrigger

class ComponentTrigger;

#include "Component.h"

// The base class for all components.
class ComponentTrigger {

public:
	Component *mComponent;
	bool mIsReady;		//!< Whether the this component is ready to perform the next step.

	//! Constructor.
	ComponentTrigger(Component *c);

	//! This event is called on all trigger components to check if the trigger is ready.
	virtual bool IsReady() {return mIsReady;}
	//! Sets the ready flag.
	void SetReady(bool ready=true);
	//! Clears the ready flag.
	void ClearReady();

};

#endif
