#ifndef HEADER_NMEALog
#define HEADER_NMEALog

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/file.h>

class NMEALog;

#include "CommunicationInterface.h"
#include "CommunicationNMEAInterface.h"

class NMEALog: public CommunicationInterface, public CommunicationNMEAInterface {

public:
	NMEALog();
	~NMEALog();

	// CommuncationInterface methods.
	bool Send(CommunicationMessage *m);

	// CommuncationNMEAInterface methods.
	void OnNMEAProcessMessage(CommunicationMessage *m, bool withchecksum);
	void OnNMEAProcessMessageChecksumError(CommunicationMessage *m);
	void OnNMEAProcessUnrecognizedChar(unsigned char chr);
	void OnNMEASend(const char *buffer, int len);

private:
	wxFile mLogFile;	//!< The log file.

};

#endif
