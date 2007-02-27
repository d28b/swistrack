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
#include <wx/filename.h>

class ConfigurationPanel: public wxPanel {

public:
	SwisTrack *mSwisTrack;			//!< The associated SwisTrack object.
	Component *mComponent;			//!< The associated component.
	ErrorList mErrorList;			//!< The list of errors.

	//! Constructor.
	ConfigurationPanel(wxWindow *parent, SwisTrack *st, Component *c);
	//! Destructor.
	~ConfigurationPanel();

private:
	wxStaticText *mLabelTitle;			//! The label holding the title of the component.

	//! Reads the *.swistrackcomponent file.
	void Read(const wxFileName &filename);
	//! Reads the configuration parameters for this component.
	void ReadConfiguration(wxXmlNode *configurationnode);
	//! Reads one configuration parameter.
	void ReadParameter(wxXmlNode *node);
	//! Returns a property of an XML node.
	wxString GetPropertyString(wxXmlNode *node, const wxString &name, const wxString &defvalue);

	DECLARE_EVENT_TABLE()
};

#endif
