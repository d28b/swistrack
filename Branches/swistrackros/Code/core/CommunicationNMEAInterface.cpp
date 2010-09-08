#include "CommunicationNMEAInterface.h"
#define THISCLASS CommunicationNMEAInterface

#include <sstream>


#if defined(WIN32) && !defined(__CYGWIN__)
#define snprintf sprintf_s
#endif

void THISCLASS::NMEAProcessData(const char *data, int len) {
	for (int i = 0; i < len; i++) {
		char inchar = data[i];
		if (mState == 0) {
			if (inchar == '$') {
				mState = 1;
				mMessage = new CommunicationMessage();
				mBufferPos = 0;
				mChecksum = 0;
			} else {
				OnNMEAProcessUnrecognizedChar(inchar);
			}
		} else if (mState == 1) {
			if (inchar == ',') {
				mBuffer[mBufferPos] = 0;
				mMessage->AddParsedArgument(wxString(mBuffer, wxConvISO8859_1));
				mChecksum = mChecksum ^ inchar;
				mBufferPos = 0;
			} else if (inchar == '*') {
				mBuffer[mBufferPos] = 0;
				mMessage->AddParsedArgument(wxString(mBuffer, wxConvISO8859_1));
				mState = 2;
			} else if ((inchar == '\r') || (inchar == '\n')) {
				mBuffer[mBufferPos] = 0;
				mMessage->AddParsedArgument(wxString(mBuffer, wxConvISO8859_1));
				OnNMEAProcessMessage(mMessage, false);
				delete mMessage;
				mMessage = 0;
				mState = 0;
			} else {
				mBuffer[mBufferPos++] = inchar;
				mChecksum = mChecksum ^ inchar;
			}
		} else if (mState == 2) {
			char ch = "0123456789ABCDEF"[(mChecksum >> 4) & 0xF];
			if ((inchar == '\r') || (inchar == '\n')) {
				OnNMEAProcessMessageChecksumError(mMessage);
				delete mMessage;
				mMessage = 0;
				mState = 0;
			} else if (inchar == ch) {
				mState = 3;
			} else {
				OnNMEAProcessMessageChecksumError(mMessage);
				delete mMessage;
				mMessage = 0;
				mState = 4;
			}
		} else if (mState == 3) {
			char ch = "0123456789ABCDEF"[mChecksum & 0xF];
			if (inchar == ch) {
				OnNMEAProcessMessage(mMessage, true);
			} else {
				OnNMEAProcessMessageChecksumError(mMessage);
			}
			delete mMessage;
			mMessage = 0;
			mState = 0;
		} else if (mState == 4) {
			mState = 0;
		} else {
		}
	}
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
