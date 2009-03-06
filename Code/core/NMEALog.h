#ifndef HEADER_NMEALog
#define HEADER_NMEALog

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/file.h>

class NMEALog;

#include "CommunicationInterface.h"

class NMEALog: public CommunicationInterface {

public:
	NMEALog(const wxString &file);
	~NMEALog();

	// CommuncationNMEAInterface methods.
	void OnNMEAProcessMessage(CommunicationMessage *m, bool withchecksum);
	void OnNMEAProcessMessageChecksumError(CommunicationMessage *m);
	void OnNMEAProcessUnrecognizedChar(unsigned char chr);
	void OnNMEASend(const char *buffer, int len);

	//! Sends a message.
	bool Send(CommunicationMessage *m);
	void NMEASendMessage(CommunicationMessage *m);

private:
	wxFile mLogFile;	//!< The log file.

};

#endif
