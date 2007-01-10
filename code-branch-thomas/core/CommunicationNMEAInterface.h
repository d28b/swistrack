#ifndef HEADER_CommunicationNMEAInterface
#define HEADER_CommunicationNMEAInterface

/*!
	NMEA Interface.
*/
class CommunicationNMEAInterface {

private:
	int mState;
	static const int mBufferSize=1024;
	unsigned char mBuffer[mBufferSize];
	int mBufferPos;
	int mChecksum;
	CommunicationMessage *mMessage;

public:
	//! Constructor.
	CommunicationNMEAInterface(): mState(0), mBufferPos(0), mChecksum(0), mMessage(0) {}

	// Overwritten DataInterface methods.
	virtual void OnProcessData(const unsigned char *data, int len);

	//! Called when a complete NMEA record has been received.
	virtual void OnProcessNMEA(const wxArrayString &args, bool withchecksum) = 0;
	//! Called when an NMEA record with a checksum error ist received.
	virtual void OnProcessNMEAChecksumError(const wxArrayString &args) = 0;
	//! Called to process a char that does not belong to an NMEA record.
	virtual void OnProcessUnrecognizedChar(unsigned char chr) = 0;

	//! Parses NMEA content and calls OnProcessNMEA or OnProcessNMEAChecksumError.
	//bool ProcessNMEA(const wxString &line);
	//! Sends a line of text.
	void SendNMEA(const wxArrayString &args);
};

#endif
