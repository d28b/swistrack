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
#include <wx/scrolwin.h>
#include <wx/listctrl.h>
#include <wx/button.h>
#include <wx/filename.h>

class ConfigurationPanel: public wxPanel {

public:
	SwisTrack *mSwisTrack;					//!< The associated SwisTrack object.
	Component *mComponent;					//!< The associated component.
	ErrorList mErrorList;					//!< The list of errors.
	wxString mHelpURL;						//!< The URL to open when to user asks for help.
	wxString mDisplayName;					//!< The name of the default display to show.

	//! Constructor.
	ConfigurationPanel(wxWindow *parent, SwisTrack *st, Component *c);
	//! Destructor.
	~ConfigurationPanel();

protected:
	wxScrolledWindow *mPanel;				//!< The main panel with the parameters.
	wxBoxSizer *mPanelSizer;				//!< The main sizer of the panel (where the parameter components go in).
	wxStaticText *mLabelTitle;				//!< The label holding the title of the component.
	static const int scParameterWidth = 200;	//!< The width of a parameter (or another element).

	//! Reads the *.swistrackcomponent file.
	void Read(const wxFileName &filename);
	//! Reads the configuration parameters for this component.
	void ReadConfiguration(wxXmlNode *configurationnode);
	//! Reads one configuration node.
	void ReadConfigurationNode(wxXmlNode *node);
	//! Reads one configuration parameter.
	void ReadParameter(wxXmlNode *node);
	//! Returns a property of an XML node.
	wxString GetPropertyString(wxXmlNode *node, const wxString &name, const wxString &defvalue);

	//! Handles the corresponding GUI event.
	void OnHelpURLClick(wxMouseEvent& event);
};

#endif
