#include "ErrorList.h"
#define THISCLASS ErrorList

void THISCLASS::Clear() {
	mList.clear();
}

void THISCLASS::Add(const wxString & msg, int linenumber) {
	ErrorListItem e;
	e.mMessage = msg;
	e.mLineNumber = linenumber;
	mList.push_back(e);
}
