#include "NMEALogFile.h"
#define THISCLASS NMEALogFile

THISCLASS::NMEALogFile(const wxString &file):
		mLogFile(file, wxFile::write) {

}

THISCLASS::~NMEALogFile() {
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

bool THISCLASS::SendMessage(CommunicationMessage *m) {
	// Add the message to the log file (this will call OnNMEASend)
	NMEASendMessage(m);
	return true;
}
