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
	void OnNMEAProcessUnrecognizedChar(unsigned char chr);
	void OnNMEASend(const std::string &str);

	//! Just as an example, we count the number of PARTICLE messages per frame
	int mParticleMessagesCount;
};

MySwisTrackClientSTDIN::MySwisTrackClientSTDIN(): CommunicationNMEAInterface() {
	mParticleMessagesCount=0;
}

void MySwisTrackClientSTDIN::Run() {
	char buffer[128];

	std::cout << "Go ahead and give me some NMEA messages, e.g.:" << std::endl;
	std::cout << "  $BEGINFRAME,34" << std::endl;
	std::cout << "  $PARTICLE,0,0,0" << std::endl;
	std::cout << "  $ENDFRAME" << std::endl;
	std::cout << std::endl;

	// As long as we can
	while (1) {
		// Read available bytes
		int len=read(0, buffer, 128);
		if (len<0) {
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
	 *     bool myvar = m->GetBool(bool defaultvalue);
	 *     int myvar = m->GetInt(int defaultvalue);
	 *     double myvar = m->GetDouble(double defaultvalue);
	 *     std::string myvar = m->GetString(const std::string defaultvalue);
	 * Each of these methods pops the next argument of the lists, converts it and returns the result.
	 */

	if (m->mCommand=="BEGINFRAME") {
		// This messages indicates the beginning of a new frame
		int framenumber=m->GetInt(0);
		std::cout << "Getting data for frame number " << framenumber << std::endl;
		mParticleMessagesCount=0;
	} else if (m->mCommand=="PARTICLE") {
		// This messages transmits the position of a particle for the current frame
		mParticleMessagesCount++;
	} else if (m->mCommand=="ENDFRAME") {
		// This messages indicates the end of the current frame
		std::cout << "  -- Got " << mParticleMessagesCount << " particle(s) for that frame." << std::endl;
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

void MySwisTrackClientSTDIN::OnNMEAProcessUnrecognizedChar(unsigned char chr) {
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
