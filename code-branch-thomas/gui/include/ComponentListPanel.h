#ifndef HEADER_ComponentListCtrl
#define HEADER_ComponentListCtrl

class ComponentListCtrl;

#include "SwisTrackCore.h"

class ComponentListCtrl: public wxListCtrl {

public:
	//! The associated SwisTrackCore object.
	SwisTrackCore *mSwisTrackCore;

	//! Constructor.
	ComponentListCtrl(wxWindow *parent, SwisTrackCore *stc);
	//! Destructor.
	~Component() {}

	void OnUpdate();
};

#endif
