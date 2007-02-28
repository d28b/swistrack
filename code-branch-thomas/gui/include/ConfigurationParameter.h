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
	SwisTrack *mSwisTrack;			//!< The associated SwisTrack object.
	Component *mComponent;			//!< The component.

	//! Constructor.
	ConfigurationParameter(wxWindow *parent);
	//! Destructor.
	~ConfigurationParameter();

	//! Initializes the parameter.
	void Initialize(SwisTrack *st, Component *c, ConfigurationXML *config, ErrorList *errorlist);

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
	void OnBeforeEditComponent(Component *c) {};
	void OnAfterEditComponent(Component *c);

protected:
	wxString mName;			//!< The (internal) name of the parameter as it is written in component settings.
	wxString mLabel;		//!< The main label of the parameter.
	wxString mDisplay;		//!< The display to be selected when the parameter is changed.
	bool mReloadable;		//!< If true, the parameters can be applied by reloading the parameters of this component (without restarting the components).

	// Update control variables
	bool mUpdating;			//!< This variable is set to true during OnUpdate in order to avoid an update causing another update. This is also set to true until the parameter object is initialized.
	wxWindow *mFocusWindow;	//!< The window that currently has the focus. Such windows (especially text boxes) should in general not be updated since the user is about to do that.

	//! This method is called (by the method Initialize) to initialize the parameter. This method must be implemented by the subclass and is called exactly once (immediately after creating the parameter object).
	virtual void OnInitialize(ConfigurationXML *config, ErrorList *errorlist) = 0;
	//! This method is called after the component configuration has been modified.
	virtual void OnUpdate() = 0;

	//! Handles the corresponding GUI event.
	void OnSetFocus(wxFocusEvent &event);
	//! Handles the corresponding GUI event.
	void OnKillFocus(wxFocusEvent &event);

	DECLARE_EVENT_TABLE()
};

#endif
