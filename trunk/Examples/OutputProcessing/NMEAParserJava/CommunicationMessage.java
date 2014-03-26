// A communication message tailored for NMEA message.
class CommunicationMessage {

	// The message type (command).
	String command;

	// The associated request (if any).
	CommunicationMessage inReplyTo;

	// The list of message parameters.
	public java.util.Vector<String> parameters;

	// The index of the next element to pop from the list.
	private int popIndex;

	// Constructor.
	CommunicationMessage() {
		this.parameters = new java.util.Vector<String>();
		this.command = "";
		this.inReplyTo = null;
	}

	// Constructor.
	CommunicationMessage(String cmd) {
		this.parameters = new java.util.Vector<String>();
		this.command = cmd;
		this.inReplyTo = null;
	}

	// Constructor.
	CommunicationMessage(String cmd, CommunicationMessage inreplyto) {
		this.parameters = new java.util.Vector<String>();
		this.command = cmd;
		this.inReplyTo = inreplyto;
	}

	// Adds a boolean value to the parameter list.
	boolean addBoolean(boolean value) {
		if (value) {
			this.parameters.add("true");
		} else {
			this.parameters.add("false");
		}
		return true;
	}

	// Adds a double to the parameter list.
	boolean addDouble(double value) {
		this.parameters.add(new Double(value).toString());
		return true;
	}

	// Adds an integer value to the parameter list.
	boolean addInt(int value) {
		this.parameters.add(new Integer(value).toString());
		return true;
	}

	// Sets the command or adds a string to the parameter list.
	boolean addParsedArgument(String value) {
		if (this.command == "") {
			this.command = value;
		} else {
			this.parameters.add(value);
		}
		return true;
	}

	// Adds a string to the parameter list.
	boolean addString(String value) {
		this.parameters.add(value);
		return true;
	}

	// Returns a boolean from the parameter list.
	boolean getBoolean(int i, boolean defvalue) {
		if (this.parameters.size() < 1) {
			return defvalue;
		}

		String str = this.parameters.elementAt(i).toLowerCase();
		if (str == "true") {
			return true;
		}
		if (str == "false") {
			return false;
		}

		Boolean val = new Boolean(defvalue);
		try {
			val = new Boolean(str);
		} catch (NumberFormatException e) {
		}
		return val.booleanValue();
	}

	// Returns a double from the parameter list.
	double getDouble(int i, double defvalue) {
		if (this.parameters.size() < 1) {
			return defvalue;
		}

		String str = this.parameters.elementAt(i);
		Double val = new Double(defvalue);
		try {
			val = new Double(str);
		} catch (NumberFormatException e) {
		}
		return val.doubleValue();
	}

	// Returns an integer value from the parameter list.
	int getInteger(int i, int defvalue) {
		if (this.parameters.size() < 1) {
			return defvalue;
		}

		String str = this.parameters.elementAt(i);
		Integer val = new Integer(defvalue);
		try {
			val = new Integer(str);
		} catch (NumberFormatException e) {
		}
		return val.intValue();
	}

	// Returns a string from the parameter list.
	String getString(int i, String defvalue) {
		if (this.parameters.size() < 1) {
			return defvalue;
		}
		return this.parameters.elementAt(i);
	}

	// Returns the next parameter as boolean value from the list.
	boolean popBoolean(boolean defvalue) {
		return getBoolean(this.popIndex++, defvalue);
	}

	// Returns the next parameter as double value from the list.
	double popDouble(double defvalue) {
		return getDouble(this.popIndex++, defvalue);
	}

	// Returns the next parameter as integer value from the list.
	int popInteger(int defvalue) {
		return getInteger(this.popIndex++, defvalue);
	}

	// Returns the next parameter as string from the list.
	String popString(String defvalue) {
		return getString(this.popIndex++, defvalue);
	}
};
