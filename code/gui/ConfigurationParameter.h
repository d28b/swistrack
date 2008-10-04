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
	SwisTrack *mSwisTrack;					//!< The associated SwisTrack object.
	Component *mComponent;					//!< The component.

	//! Constructor.
	ConfigurationParameter(wxWindow *parent);
	//! Destructor.
	~ConfigurationParameter();

	//! Initializes the parameter.
	void Initialize(SwisTrack *st, Component *c, ConfigurationXML *config, ErrorList *errorlist);

	//! Restarts or reloads the components to commit the changes made in the configuration.
	void CommitChanges();

	// SwisTrackCoreInterface methods
	void OnBeforeStart(bool productionmode);
	void OnAfterStart(bool productionmode) {};
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
	static const int scParameterWidth = 200;	//!< The width of a parameter.
	static const int scLabelWidth = 75;		//!< The width of a label.
	static const int scIndentWidth = 25;		//!< The indent width.
	static const int scTextBoxWidth = 75;		//!< The width of a text box.
	static const int scUnitWidth = 50;		//!< The width of a unit label.

private:
	// Update control variables
	bool mUpdating;					//!< This variable is set to true during OnUpdate in order to avoid an update causing another update. This is also set to true until the parameter object is initialized.
	wxWindow *mUpdateProtection;	//!< The window to protect against updates. This is usually the window having the focus (since the user is editing it, it is not nice to reset values).

protected:
	//! This method is called (by the method Initialize) to initialize the parameter. This method must be implemented by the subclass and is called exactly once (immediately after creating the parameter object).
	virtual void OnInitialize(ConfigurationXML *config, ErrorList *errorlist) = 0;
	//! This method is called after the component configuration has been modified. The updateprotection window (e.g. text box) should not be updated.
	virtual void OnUpdate(wxWindow *updateprotection) = 0;
	//! This method is called by SetNewValue() to set the new value in the .
	virtual void OnSetNewValue() = 0;

	//! Sets the new value. The provided window (e.g. text box) will be protected against updates caused by this call.
	void SetNewValue(wxWindow *updateprotection = 0);
};

#endif
