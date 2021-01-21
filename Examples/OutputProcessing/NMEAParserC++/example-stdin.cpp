#include <string>
#include <iostream>

#include "CommunicationNMEAInterface.h"

//! This is the main class of our sample program. It reads data from STDIN and passes them through the NMEA parser.
class MySwisTrackClientSTDIN: public CommunicationNMEAInterface {
public:
	//! Constructor
	MySwisTrackClientSTDIN();

	//! Main loop.
	void Run();

	// CommunicationNMEAInterface methods
	void OnNMEAProcessMessage(CommunicationMessage *m, bool withchecksum);
	void OnNMEAProcessMessageChecksumError(CommunicationMessage *m);
	void OnNMEAProcessUnrecognizedChar(char c);
	void OnNMEASend(const std::string &str);

	//! Just as an example, we count the number of PARTICLE messages per frame
	int mParticleMessagesCount;
	//! Keeps track of the current frame number
	int mFrameNumber;
};

MySwisTrackClientSTDIN::MySwisTrackClientSTDIN(): CommunicationNMEAInterface() {
	mParticleMessagesCount = 0;
	mFrameNumber = 0;
}

void MySwisTrackClientSTDIN::Run() {
	char buffer[128];

	std::cout << "Go ahead and give me some NMEA messages, e.g.:" << std::endl;
	std::cout << "  $STEP_START" << std::endl;
	std::cout << "  $FRAMENUMBER,34" << std::endl;
	std::cout << "  $PARTICLE,0,0,0,0" << std::endl;
	std::cout << "  $STEP_STOP" << std::endl;
	std::cout << std::endl;

	// As long as we can
	while (1) {
		// Read available bytes
		int len = read(0, buffer, 128);
		if (len < 0) {
			std::cout << "STDIN closed ..." << std::endl;
			break;
		}

		// Pass it to the NMEA parser (this will subsequently call our OnNMEA... methods)
		NMEAProcessData(buffer, len);
	}
}

void MySwisTrackClientSTDIN::OnNMEAProcessMessage(CommunicationMessage *m, bool withchecksum) {
	/* This method is called each time a complete message was received (with either a valid or no checksum).
	 * A message consists of a command string and arguments. The command string can be accessed via
	 *     m->mCommand
	 * while the arguments can be read (and converted) with
	 *     bool myvar = m->GetBool(int i, bool defaultvalue);
	 *     int myvar = m->GetInt(int i, int defaultvalue);
	 *     double myvar = m->GetDouble(int i, double defaultvalue);
	 *     std::string myvar = m->GetString(int i, const std::string defaultvalue);
	 * or
	 *     bool myvar = m->PopBool(bool defaultvalue);
	 *     int myvar = m->PopInt(int defaultvalue);
	 *     double myvar = m->PopDouble(double defaultvalue);
	 *     std::string myvar = m->PopString(const std::string defaultvalue);
	 * The Get* methods return the argument at a specific index, or the default value if that argument doesn't exist.
	 * The Pop* methods return the next argument each time they are called.
	 */

	if (m->mCommand == "STEP_START") {
		// This message indicates the beginning of a new frame
		mFrameNumber = 0;
		mParticleMessagesCount = 0;
	} else if (m->mCommand == "FRAMENUMBER") {
		// This message contains the frame number
		mFrameNumber = m->PopInt(0);
	} else if (m->mCommand == "PARTICLE") {
		// This message transmits the position of a particle for the current frame
		mParticleMessagesCount++;
	} else if (m->mCommand == "STEP_STOP") {
		// This message indicates the end of the current frame
		std::cout << "Got " << mParticleMessagesCount << " particle(s) for frame " << mFrameNumber << std::endl;
	} else {
		std::cout << "Unknown message '" << m->mCommand << "' received!" << std::endl;
	}
}

void MySwisTrackClientSTDIN::OnNMEAProcessMessageChecksumError(CommunicationMessage *m) {
	/* We don't really expect messages with checksum errors here.
	 * Hence, we just discard such messages.
	 *
	 * In case you are working with lossy channels, you could do something more fancy here.
	 */
}

void MySwisTrackClientSTDIN::OnNMEAProcessUnrecognizedChar(char c) {
	/* Unrecognized chars are bytes that are transmitted between valid NMEA messages.
	 * An NMEA stream usually only consists of NMEA messages, but people sometimes put human readable comments in between.
	 * On lossy channels, messages missing the $ sign (start-of-message) would also fall in here.
	 *
	 * This method is called for each char that does not belong to an NMEA message.
	 * But unless you have a good reason, leave this method empty.
	 */
}

void MySwisTrackClientSTDIN::OnNMEASend(const std::string &str) {
	/* Whenever you send a message using NMEASendMessage(CommunicationMessage *m),
	 * this method is called to actually transmit the bytes.
	 */

	// This is just useless cosmetics, as we never send stuff back to SwisTrack in this simple example
	std::cout << str;
}


int main(int argc, char *argv) {
	MySwisTrackClientSTDIN client;
	client.Run();
	return 0;
}
