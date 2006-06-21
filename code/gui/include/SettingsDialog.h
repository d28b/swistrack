#ifndef _SETTINGSDIALOG_H
#define _SETTINGSDIALOG_H

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include "wx/propdlg.h"
#include "wx/listbox.h"
#include "wx/spinctrl.h"
#include "wx/event.h"
#include "constants.h"

#include "SwisTrack.h"
#include <libxml++/libxml++.h>
#include <vector>
#include "XMLCfg.h"



/** \class NewExperimentDialog
* \brief Create a new/altering a configuration
*
*/
class SettingsDialog : public wxPropertySheetDialog
    {
    DECLARE_CLASS(SettingsDialog)
    private:
        xmlpp::DomParser parser;
        xmlpp::Document* document;
        xmlpp::Element* nodeRoot;
        xmlpp::Element* current;

        wxBookCtrlBase* notebook;

        int actid; // used to keep track of control id's
        vector<wxString> idxpath; // keeps track of the x-path of each control
        vector<wxControl*> controls; // keeps track of all controls
        void CreateDialog();

        void FileOpen(wxCommandEvent& event);
        void FileSave(wxCommandEvent& event);
        void ChangeMode(wxCommandEvent& event);

    public:
        SettingsDialog(SwisTrack *parent);
        ~SettingsDialog();
        SwisTrack *parent;
        
    protected:
        // any class wishing to process wxWindows events must use this macro
        DECLARE_EVENT_TABLE()
    };
#endif
