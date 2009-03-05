#include "NMEALog.h"
#define THISCLASS NMEALog

THISCLASS::NMEALog():
		CommunicationInterface(), CommunicationNMEAInterface(), mLogFile(STDOUT_FILENO) {

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
	// Write the message to the log file (STDOUT in this case). This will call OnNMEASend.
	NMEASendMessage(m);
	return true;
}
