#ifndef HEADER_CommunicationMessage
#define HEADER_CommunicationMessage

class CommunicationMessage;

#include <string>
#include <vector>

//! A communication message tailored for NMEA message.
class CommunicationMessage {

private:
	unsigned int mPopIndex;				//!< The index of the next element to pop from the list.

public:
	//! Parameter list type.
	typedef std::vector<std::string> tParameters;

	tParameters mParameters;			//!< The list of message parameters.
	std::string mCommand;				//!< The message type (command).
	CommunicationMessage *mInReplyTo;	//!< The associated request (if any).

	//! Constructor.
	CommunicationMessage(const std::string &cmd = "", CommunicationMessage *inreplyto = 0);
	//! Destructor.
	~CommunicationMessage() {}

	//! Returns a boolean from the parameter list.
	bool GetBool(unsigned int i, bool defvalue);
	//! Returns an integer value from the parameter list.
	int GetInt(unsigned int i, int defvalue);
	//! Returns a double from the parameter list.
	double GetDouble(unsigned int i, double defvalue);
	//! Returns a string from the parameter list.
	std::string GetString(unsigned int i, const std::string &defvalue);

	//! Returns the next parameter as boolean value from the list.
	bool PopBool(bool defvalue) {
		return GetBool(mPopIndex++, defvalue);
	}
	//! Returns the next parameter as integer value from the list.
	int PopInt(int defvalue) {
		return GetInt(mPopIndex++, defvalue);
	}
	//! Returns the next parameter as double value from the list.
	double PopDouble(double defvalue) {
		return GetDouble(mPopIndex++, defvalue);
	}
	//! Returns the next parameter as string from the list.
	std::string PopString(const std::string &defvalue) {
		return GetString(mPopIndex++, defvalue);
	}

	//! Adds a boolean value to the parameter list.
	bool AddBool(bool value);
	//! Adds an integer value to the parameter list.
	bool AddInt(int value);
	//! Adds a double to the parameter list.
	bool AddDouble(double value);
	//! Adds a string to the parameter list.
	bool AddString(const std::string &value);
	//! Sets the command or adds a string to the parameter list.
	bool AddParsedArgument(const std::string &value);
};

#endif
