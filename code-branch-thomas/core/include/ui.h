// ui.h: interface for the ui class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UI_H__248741CA_ACD7_47BC_A7B5_6A4C783FB10A__INCLUDED_)
#define AFX_UI_H__248741CA_ACD7_47BC_A7B5_6A4C783FB10A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ObjectTracker.h"
#include "Component.h"

class ui : public Component
{
public:
	ui();
	virtual ~ui();

private:
	void ErrorHandler(int error);
	ObjectTracker* objecttracker;
	int status;
};

#endif // !defined(AFX_UI_H__248741CA_ACD7_47BC_A7B5_6A4C783FB10A__INCLUDED_)
