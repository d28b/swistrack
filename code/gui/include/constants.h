#ifndef _CONSTANTS_H
#define _CONSTANTS_H

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Gui_New,
	Gui_Open,
	Gui_Save,
	Gui_Quit,
	Gui_Ctrl_Start,
	Gui_Ctrl_Rewind,
	Gui_Ctrl_Continue,
	Gui_Ctrl_Stop,
	Gui_Ctrl_Singlestepback,
	Gui_Ctrl_Singlestep,
	Gui_Ctrl_Pause,
	Gui_View_ShowTracker,
	Gui_View_ShowParticleFilter,
	Gui_View_ShowSegmenterPP,
	Gui_View_ShowSegmenter,
	Gui_View_ShowInput,
	Gui_View_TrajCross,
    Gui_View_TrajNoID,
    Gui_View_TrajFull,
	Gui_View_TrajNoCross,
	Gui_View_Coverage,
	Gui_View_ShowMask,
	Gui_Mode_Auto,
	Gui_Mode_Manual,
	Gui_Mode_Intercept,
	Gui_Tools_SetAviOutput,
	Gui_Tools_AviOutput,
	Gui_Tools_ShowCamera,
	Gui_Tools_Screenshot,
	Gui_Tools_FlipScreen,
	Gui_Tools_FitCanvas,
	Gui_Intercept_Done,
	Gui_Intercept_Pick,
	wxID_OPENAVI,
	wxID_OPENBMP,
	wxID_SPINNINS,
	wxID_CFGNEXT,
	wxID_CFGOK,
	wxID_OPENCALIB,
	wxID_OPENCALIBBMP,
	wxID_OPENOUTPUT,
	wxID_CHOICE_INPUT,
	wxID_CHOICE_SEGMODE,
	wxID_OPENMASK,
	ID_MASK_AREACOUNT,
	ID_MASK_CONTOUR,
	wxID_NOTEBOOK,
	wxID_TOOLBAR,
	wxID_DSPSPDSLIDER,
	wxID_TPSPEED,
	wxID_SPMINAREA,
	wxID_SPTHRESHOLD,
    wxID_MODE_SELECTION,
	Gui_Help,
    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    Gui_About = wxID_ABOUT
};
#endif

