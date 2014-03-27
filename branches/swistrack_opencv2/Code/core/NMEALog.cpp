#include "NMEALog.h"
#define THISCLASS NMEALog

THISCLASS::NMEALog(const wxString &file):
		mLogFile() {

	if (file.length() == 0) {
	  mLogFile.Attach(wxFile::fd_stdout);
	} else {
	  mLogFile.Open(file, wxFile::write);
	}
}

THISCLASS::~NMEALog() {
}

void THISCLASS::OnNMEAProcessMessage(CommunicationMessage *m, bool withchecksum) {
	// We don't process any incoming messages.
}

void THISCLASS::OnNMEAProcessMessageChecksumError(CommunicationMessage *m) {
	// We don't process any incoming messages.
}

void THISCLASS::OnNMEAProcessUnrecognizedChar(unsigned char chr) {
	// We don't process any incoming messages.
}

void THISCLASS::OnNMEASend(const char *buffer, int len) {
	mLogFile.Write(buffer, len);
}

bool THISCLASS::Send(CommunicationMessage *m) {
	// Add the message to the log file (this will call OnNMEASend)
	NMEASendMessage(m);
	return true;
}

void THISCLASS::NMEASendMessage(CommunicationMessage *m) {
	char buffer[1024+5];
	int len;

	// Start of message
	buffer[0] = '$';
	len = 1;

	// Concatenate commands and arguments
	len += snprintf(buffer + len, 1024 - len, "%s", (const char*)(m->mCommand.mb_str(wxConvISO8859_1)));
	CommunicationMessage::tParameters::iterator it = m->mParameters.begin();
	while (it != m->mParameters.end()) {
		len += snprintf(buffer + len, 1024 - len, ",%s", (const char *)((*it).mb_str(wxConvISO8859_1)));
		it++;
	}

	// Calculate checksum
	int checksum = 0;
	for (int n = 1; n < len; n++) {
		checksum = checksum ^ buffer[n];
	}
	char ch1 = "0123456789ABCDEF"[(checksum >> 4) & 0xF];
	char ch2 = "0123456789ABCDEF"[checksum & 0xF];
	buffer[len++] = '*';
	buffer[len++] = ch1;
	buffer[len++] = ch2;
	buffer[len++] = '\r';
	buffer[len++] = '\n';

	// Send
	OnNMEASend(buffer, len);
	return;
}
