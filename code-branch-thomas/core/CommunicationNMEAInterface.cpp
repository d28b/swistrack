#include "CommunicationNMEAInterface.h"
#define THISCLASS CommunicationNMEAInterface

#include <sstream>

void THISCLASS::NMEAProcessData(const char *data, int len) {
	for (int i=0; i<len; i++) {
		char inchar=data[i];
		if (mState==0) {
			if (inchar=='$') {
				mState=1;
				mMessage=new CommunicationMessage();
				mBufferPos=0;
				mChecksum=0;
			} else {
				OnNMEAProcessUnrecognizedChar(inchar);
			}
		} else if (mState==1) {
			if (inchar==',') {
				mBuffer[mBufferPos]=0;
				mMessage->AddParsedArgument(mBuffer);
				mChecksum=mChecksum ^ inchar;
				mBufferPos=0;
			} else if (inchar=='*') {
				mBuffer[mBufferPos]=0;
				mMessage->AddParsedArgument(mBuffer);
				mState=2;
			} else if (inchar=='\n') {
				mBuffer[mBufferPos]=0;
				mMessage->AddParsedArgument(mBuffer);
				OnNMEAProcessMessage(mMessage, false);
				delete mMessage;
				mMessage=0;
				mState=0;
			} else {
				mBuffer[mBufferPos++]=inchar;
				mChecksum=mChecksum ^ inchar;
			}
		} else if (mState==2) {
			char ch="0123456789ABCDEF"[(mChecksum >> 4) & 0xF];
			if (inchar=='\n') {
				OnNMEAProcessMessageChecksumError(mMessage);
				delete mMessage;
				mMessage=0;
				mState=0;
			} else if (inchar==ch) {
				mState=3;
			} else {
				OnNMEAProcessMessageChecksumError(mMessage);
				delete mMessage;
				mMessage=0;
				mState=4;
			}
		} else if (mState==3) {
			char ch="0123456789ABCDEF"[mChecksum & 0xF];
			if (inchar==ch) {
				OnNMEAProcessMessage(mMessage, true);
			} else {
				OnNMEAProcessMessageChecksumError(mMessage);
			}
			delete mMessage;
			mMessage=0;
			mState=0;
		} else if (mState==4) {
			mState=0;
		} else {
		}
	}
}

void THISCLASS::NMEASendMessage(CommunicationMessage *m) {
	// Concatenate
	std::ostringstream line;
	line << "$" << m->mCommand;
	CommunicationMessage::tParameters::iterator it=m->mParameters.begin();
	while (it != m->mParameters.end()) {
		line << "," << *it;
		it++;
	}

	// Calculate checksum
	int checksum=0;
	std::string linestr=line.str();
	int len=linestr.length();
	const char *buffer=linestr.c_str();
	for (int n=1; n<len; n++) {
		checksum=checksum ^ buffer[n];
	}
	char ch1="0123456789ABCDEF"[(checksum >> 4) & 0xF];
	char ch2="0123456789ABCDEF"[checksum & 0xF];
	line << "*" << ch1 << ch2;

	// Send
	line << "\r\n";
	OnNMEASend(line.str());
	return;
}
