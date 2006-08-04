#include "SwisTrackPanel.h"

SwisTrackPanel::SwisTrackPanel(SwisTrack* parent, wxString title) : wxMiniFrame(parent, -1,
																title,
																wxDefaultPosition,
																wxDefaultSize,

																wxCAPTION | wxSIMPLE_BORDER),
																actid(wxID_HIGHEST+1), parent(parent)
{
}
