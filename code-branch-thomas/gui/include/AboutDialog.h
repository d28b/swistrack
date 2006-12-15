#ifndef _ABOUTDIALOG_H
#define _ABOUTDIALOG_H

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif
 
/** \class AboutDialog
*
* \brief Displays "about" dialog
*/
class AboutDialog : public wxDialog
{
public:
	AboutDialog(wxWindow *parent);
	virtual ~AboutDialog(){}
	void SetText(const wxString& text);

	private: 

	wxStaticText *m_pInfoText;
	wxButton *m_pOkButton;
};

#endif

