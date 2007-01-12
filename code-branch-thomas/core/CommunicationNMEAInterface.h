#ifndef HEADER_CommunicationNMEAInterface
#define HEADER_CommunicationNMEAInterface

class CommunicationNMEAInterface;

#include "CommunicationMessage.h"

/*!
	NMEA Interface.
*/
class CommunicationNMEAInterface {

private:
	int mState;
	static const int mBufferSize=1024;
	char mBuffer[mBufferSize];
	int mBufferPos;
	int mChecksum;
	CommunicationMessage *mMessage;

public:
	//! Constructor.
	CommunicationNMEAInterface(): mState(0), mBufferPos(0), mChecksum(0), mMessage(0) {}

	//! Processes incoming data. This method should be called each time a piece of data is received. The data doesn't need to contain a complete NMEA record.
	void NMEAProcessData(const char *data, int len);
	//! This method can be called to send an NMEA message.
	void NMEASendMessage(CommunicationMessage *m);

	//! Called when a complete NMEA record has been received.
	virtual void OnNMEAProcessMessage(CommunicationMessage *m, bool withchecksum) = 0;
	//! Called when an NMEA record with a checksum error ist received.
	virtual void OnNMEAProcessMessageChecksumError(CommunicationMessage *m) = 0;
	//! Called to process a char that does not belong to an NMEA record.
	virtual void OnNMEAProcessUnrecognizedChar(unsigned char chr) = 0;
	//! Called to send text to the client.
	virtual void OnNMEASend(const std::string &str) = 0;
};

#endif
