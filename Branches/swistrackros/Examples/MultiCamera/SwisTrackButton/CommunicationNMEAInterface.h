#ifndef HEADER_CommunicationNMEAInterface
#define HEADER_CommunicationNMEAInterface

class CommunicationNMEAInterface;

#include "CommunicationMessage.h"

//! NMEA parser.
class CommunicationNMEAInterface {

private:
	int mState;							//!< The current parser state.
	static const int mBufferSize = 1024;	//!< The size of the input buffer in number of chars. Messages longer than this will be cut.
	char mBuffer[mBufferSize];			//!< The buffer holding unprocessed characters.
	int mBufferPos;						//!< The current position in mBuffer.
	int mChecksum;						//!< The current checksum.
	CommunicationMessage *mMessage;		//!< The associated CommunicationMessage.

public:
	//! Constructor.
	CommunicationNMEAInterface(): mState(0), mBufferPos(0), mChecksum(0), mMessage(0) {}
	//! Destructor.
	virtual ~CommunicationNMEAInterface() {}

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
	virtual void OnNMEASend(const char *buffer, int len) = 0;
};

#endif
