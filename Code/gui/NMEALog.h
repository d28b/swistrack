#ifndef HEADER_NMEALog
#define HEADER_NMEALog

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/file.h>

class NMEALog;

#include "CommunicationNMEAInterface.h"

class NMEALog: public CommunicationNMEAInterface {

public:
	NMEALog(const wxString &file);
	~NMEALog();

	// CommuncationNMEAInterface methods.
	void OnNMEAProcessMessage(CommunicationMessage *m, bool withchecksum);
	void OnNMEAProcessMessageChecksumError(CommunicationMessage *m);
	void OnNMEAProcessUnrecognizedChar(unsigned char chr);
	void OnNMEASend(const char *buffer, int len);

	//! Sends a message.
	bool SendMessage(CommunicationMessage *m);

private:
	wxFile mLogFile;	//!< The log file.

};

#endif
