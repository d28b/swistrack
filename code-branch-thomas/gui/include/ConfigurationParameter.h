#ifndef HEADER_ConfigurationParameter
#define HEADER_ConfigurationParameter

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

class ConfigurationParameter;

#include "SwisTrackCore.h"
#include "SwisTrack.h"
#include "ConfigurationXML.h"
#include <wx/panel.h>
#include <wx/listctrl.h>
#include <wx/button.h>

class ConfigurationParameter: public wxPanel, public SwisTrackCoreInterface {

public:
	SwisTrackCore *mSwisTrackCore;	//!< The associated SwisTrackCore object.
	SwisTrack *mSwisTrack;			//!< The associated SwisTrack object.
	Component *mComponent;			//!< The component.

	//! Constructor.
	ConfigurationParameter(wxWindow *parent);
	//! Destructor.
	~ConfigurationParameter();

	//! Initializes the parameter.
	void Initialize(SwisTrack *st, ConfigurationXML *config);
	//! This method is called (by the method Initialize) to initialize the parameter. This method must be implemented by the subclass and is called exactly once (immediately after creating the parameter object).
	virtual void OnInitialize(ConfigurationXML *config) = 0;
	//! This method is called after the component configuration has been modified.
	virtual void OnUpdate() = 0;

	//! Restarts or reloads the components to commit the changes made in the configuration.
	void CommitChanges();

	// SwisTrackCoreInterface methods
	void OnBeforeStart(bool productivemode);
	void OnAfterStart(bool productivemode) {};
	void OnBeforeStop() {};
	void OnAfterStop();
	void OnBeforeStep() {};
	void OnStepReady() {};
	void OnAfterStep() {};
	void OnBeforeEdit() {};
	void OnAfterEdit() {};

protected:
	wxString mName;			//!< The name of the parameter.
	wxString mDisplay;		//!< The display to be selected when the parameter is changed.
	bool mReloadable;		//!< If true, the parameters can be applied by reloading the parameters of this component (without restarting the components).

};

#endif
