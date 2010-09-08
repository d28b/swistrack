/*
 NMEA Interface.
 */
abstract class CommunicationNMEAInterface {

	private enum State {
		Argument, Checksum1, Checksum2, Checksum2Discard, None
	};

	private int checksum;

	private StringBuffer currentArgument;

	private CommunicationMessage message;

	private State state;

	// Constructor.
	CommunicationNMEAInterface() {
		this.state = State.None;
		this.checksum = 0;
		this.message = null;
		this.currentArgument = null;
	}

	// Processes incoming data. This method should be called each time a piece
	// of data is received. The data doesn't need to contain a complete NMEA
	// record.
	void NMEAProcessData(byte buffer[], int offset, int len) {
		for (int i = offset; i < len; i++) {
			char inchar = (char) buffer[i];
			if (this.state == State.None) {
				if (inchar == '$') {
					this.message = new CommunicationMessage();
					this.currentArgument = new StringBuffer();
					this.checksum = 0;
					this.state = State.Argument;
				} else {
					OnNMEAProcessUnrecognizedChar(inchar);
				}
			} else if (this.state == State.Argument) {
				if (inchar == ',') {
					this.message.addParsedArgument(this.currentArgument
							.toString());
					this.currentArgument = new StringBuffer();
					this.checksum = this.checksum ^ inchar;
				} else if (inchar == '*') {
					this.message.addParsedArgument(this.currentArgument
							.toString());
					this.currentArgument = null;
					this.state = State.Checksum1;
				} else if ((inchar == '\n') || (inchar == '\r')) {
					this.message.addParsedArgument(this.currentArgument
							.toString());
					this.currentArgument = null;
					OnNMEAProcessMessage(this.message, false);
					this.message = null;
					this.state = State.None;
				} else {
					this.currentArgument.append(inchar);
					this.checksum = this.checksum ^ inchar;
				}
			} else if (this.state == State.Checksum1) {
				char checksum_expected = Integer
						.toHexString(this.checksum >> 4).charAt(0);
				if (inchar == '\n') {
					OnNMEAProcessMessageChecksumError(this.message);
					this.message = null;
					this.state = State.None;
				} else if (inchar == checksum_expected) {
					this.state = State.Checksum2;
				} else {
					OnNMEAProcessMessageChecksumError(this.message);
					this.message = null;
					this.state = State.Checksum2Discard;
				}
			} else if (this.state == State.Checksum2) {
				char checksum_expected = Integer.toHexString(
						this.checksum & 0xF).charAt(0);
				if (inchar == checksum_expected) {
					OnNMEAProcessMessage(this.message, true);
				} else {
					OnNMEAProcessMessageChecksumError(this.message);
				}
				this.message = null;
				this.state = State.None;
			} else if (this.state == State.Checksum2Discard) {
				this.state = State.None;
			} else {
			}
		}
	}

	// This method can be called to send an NMEA message.
	void NMEASendMessage(CommunicationMessage m) {
		// Concatenate
		StringBuffer line = new StringBuffer("$");
		line.append(m.command);

		for (String str : m.parameters) {
			line.append(',');
			line.append(str);
		}

		// Calculate checksum
		int lineChecksum = 0;
		for (int i = 0; i < line.length(); i++) {
			lineChecksum = lineChecksum ^ line.charAt(i);
		}
		char ch1 = Integer.toHexString(lineChecksum >> 4).charAt(0);
		char ch2 = Integer.toHexString(lineChecksum & 0xF).charAt(0);
		line.append('*');
		line.append(ch1);
		line.append(ch2);

		// Send
		line.append("\r\n");
		OnNMEASend(line.toString());
		return;
	}

	// Called when a complete NMEA record has been received.
	abstract void OnNMEAProcessMessage(CommunicationMessage m,
			boolean withchecksum);

	// Called when an NMEA record with a checksum error ist received.
	abstract void OnNMEAProcessMessageChecksumError(CommunicationMessage m);

	// Called to process a char that does not belong to an NMEA record.
	abstract void OnNMEAProcessUnrecognizedChar(char c);

	// Called to send text to the client.
	abstract void OnNMEASend(String str);
};
