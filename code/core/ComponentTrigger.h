#ifndef HEADER_ComponentTrigger
#define HEADER_ComponentTrigger

class ComponentTrigger;

#include "Component.h"

//! The component trigger class. All components that provide a trigger should use this class.
class ComponentTrigger {

public:
	Component *mComponent;
	bool mIsReady;		//!< Whether the this component is ready to perform the next step.

	//! Constructor.
	ComponentTrigger(Component *c);
	//! Destructor.
	virtual ~ComponentTrigger() {}

	//! This event is called on all trigger components to check if the trigger is ready.
	virtual bool IsReady() {
		return mIsReady;
	}
	//! Sets the ready flag.
	void SetReady(bool ready = true);
	//! Clears the ready flag.
	void ClearReady();

};

#endif
