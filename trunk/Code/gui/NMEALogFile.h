#ifndef HEADER_NMEALogFile
#define HEADER_NMEALogFile

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/file.h>

class NMEALogFile;

#include "CommunicationNMEAInterface.h"

class NMEALogFile: public CommunicationNMEAInterface {

public:
	NMEALogFile(const wxString &file);
	~NMEALogFile();

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
