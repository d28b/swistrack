
#include "AboutDialog.h"

AboutDialog::AboutDialog(wxWindow *parent) : wxDialog(parent,-1,"About SwisTrack",wxDefaultPosition,
													   wxSize(400,150), wxDEFAULT_DIALOG_STYLE)
	{
		SetAutoLayout(TRUE);
		
		wxLayoutConstraints *layout = new wxLayoutConstraints();

		//layout->top.SameAs(this, wxTop, 10);
		layout->centreX.SameAs(this,wxCentreX);
		layout->centreY.PercentOf(this,wxCentreY,60);
		layout->width.AsIs();
		layout->height.AsIs();

		m_pInfoText = new wxStaticText(this, -1, "", wxPoint(5,5),
			wxSize(100,100), wxALIGN_CENTRE);
		
		m_pInfoText->SetConstraints(layout);

		layout = new wxLayoutConstraints();
		layout->top.Below(m_pInfoText,10);
		layout->centreX.SameAs(this,wxCentreX);
		layout->width.PercentOf(this, wxWidth, 20);
		layout->height.AsIs();
		
		m_pOkButton = new wxButton(this, wxID_OK, "Ok", wxPoint(5,40));
		m_pOkButton->SetConstraints(layout);

		Layout();
	}

void AboutDialog::SetText(const	wxString &text)
	{
		m_pInfoText->SetLabel(text);
		Layout();
	}
