#ifndef HEADER_ConfigurationPanel
#define HEADER_ConfigurationPanel

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

class ConfigurationPanel;

#include "SwisTrack.h"
#include "SwisTrackCore.h"
#include "ComponentsDialog.h"
#include <wx/panel.h>
#include <wx/listctrl.h>
#include <wx/button.h>

class ConfigurationPanel: public wxPanel, public SwisTrackCoreInterface {

public:
	SwisTrack *mSwisTrack;				//! The associated SwisTrack object.
	SwisTrackCore *mSwisTrackCore;		//! The associated SwisTrackCore object.
	Component *mComponent;				//! The associated component.
	wxString mComponentDisplayName;		//! The name of the component.
	wxString mComponentDescription;		//! The description of the component.
	wxString mComponentDefaultDisplay;	//! The default display.

	//! Constructor.
	ConfigurationPanel(wxWindow *parent, SwisTrackCore *stc);
	//! Destructor.
	~ConfigurationPanel();

	// SwisTrackCoreInterface methods
	void OnBeforeStart(bool productivemode) {};
	void OnAfterStart(bool productivemode) {};
	void OnBeforeStop() {};
	void OnAfterStop() {};
	void OnBeforeStep() {};
	void OnStepReady() {};
	void OnAfterStep() {};
	void OnBeforeEdit() {};
	void OnAfterEdit() {};

private:
	//! Reads the configuration parameters for this component.
	void ReadConfiguration(wxXmlNode *configurationnode);
	//! Reads one configuration parameter.
	void ReadParameter(wxXmlNode *node);
	//! Returns a property of an XML node.
	wxString GetPropertyString(wxXmlNode *node, const wxString &name, const wxString &defvalue);

	DECLARE_EVENT_TABLE()
};

#endif
