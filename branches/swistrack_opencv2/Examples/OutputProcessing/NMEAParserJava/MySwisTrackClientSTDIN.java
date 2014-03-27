import java.io.IOException;

// This is the main class of our sample program. It reads data from STDIN and passes them through the NMEA parser.
class MySwisTrackClientSTDIN extends CommunicationNMEAInterface {

	public static void main(String argv[]) {
		MySwisTrackClientSTDIN client = new MySwisTrackClientSTDIN();
		client.Run();
	}

	// Just as an example, we count the number of PARTICLE messages per frame
	private int particleMessagesCount;
	// Keeps track of the current frame number
	private int frameNumber;

	// Constructor
	MySwisTrackClientSTDIN() {
		this.particleMessagesCount = 0;
		this.frameNumber = 0;
	}

	// CommunicationNMEAInterface methods
	@Override
	void OnNMEAProcessMessage(CommunicationMessage m, boolean withchecksum) {
		/*
		 * This method is called each time a complete message was received (with
		 * either a valid or no checksum). A message consists of a command
		 * string and arguments. The command string can be accessed via
		 *     m.command
		 * while the arguments can be read (and converted) with
		 *     boolean myvar = m.getBoolean(int i, boolean defaultvalue);
		 *     int myvar = m.getInteger(int i, int defaultvalue);
		 *     double myvar = m.getDouble(int i, double defaultvalue);
		 *     String myvar = m.getString(int i, String defaultvalue);
		 * or
		 *     bool myvar = m.popBoolean(boolean defaultvalue);
		 *     int myvar = m.popInteger(int defaultvalue);
		 *     double myvar = m.popDouble(double defaultvalue);
		 *     String myvar = m.popString(String defaultvalue); 
		 * The get* methods return the argument at a specific index, or the
		 * default value if that argument doesn't exist. The pop* methods return
		 * the next argument each time they are called.
		 */

		if (m.command.equals("STEP_START")) {
			// This messages indicates the beginning of a new frame
			this.frameNumber = 0;
			this.particleMessagesCount = 0;
		} else if (m.command.equals("FRAMENUMBER")) {
			// This message contains the frame number
			this.frameNumber = m.popInteger(0);
		} else if (m.command.equals("PARTICLE")) {
			// This messages transmits the position of a particle for the
			// current frame
			this.particleMessagesCount++;
		} else if (m.command.equals("STEP_STOP")) {
			// This messages indicates the end of the current frame
			System.out.println("Got " + this.particleMessagesCount
					+ " particle(s) for frame " + this.frameNumber);
		} else {
			System.out.println("Unknown message '" + m.command + "' received!");
		}
	}

	@Override
	void OnNMEAProcessMessageChecksumError(CommunicationMessage m) {
		/*
		 * We don't really expect messages with checksum errors here. Hence, we
		 * just discard such messages.
		 * 
		 * In case you are working with lossy channels, you could do something
		 * more fancy here.
		 */
	}

	@Override
	void OnNMEAProcessUnrecognizedChar(char c) {
		/*
		 * Unrecognized chars are bytes that are transmitted between valid NMEA
		 * messages. An NMEA stream usually only consists of NMEA messages, but
		 * people sometimes put human readable comments in between. On lossy
		 * channels, messages missing the $ sign (start-of-message) would also
		 * fall in here.
		 * 
		 * This method is called for each char that does not belong to an NMEA
		 * message. But unless you have a good reason, leave this method empty.
		 */
	}

	@Override
	void OnNMEASend(String str) {
		/*
		 * Whenever you send a message using
		 * NMEASendMessage(CommunicationMessage *m), this method is called to
		 * actually transmit the bytes.
		 */

		// This is just useless cosmetics, as we never send stuff back to
		// SwisTrack in this simple example
		System.out.println(str);
	}

	// Main loop.
	void Run() {
		System.out.println("Go ahead and give me some NMEA messages, e.g.:");
		System.out.println("  $STEP_START");
		System.out.println("  $FRAMENUMBER,34");
		System.out.println("  $PARTICLE,0,0,0,0");
		System.out.println("  $STEP_STOP");
		System.out.println("");

		// As long as we can
		byte[] buffer = new byte[128];
		while (true) {
			// Read available bytes
			int len = 0;
			try {
				len = System.in.read(buffer);
			} catch (IOException e) {
				System.err.println("STDIN closed ...");
				break;
			}

			// Pass it to the NMEA parser (this will subsequently call our
			// OnNMEA... methods)
			NMEAProcessData(buffer, 0, len);
		}
	}
};
