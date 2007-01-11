#ifndef HEADER_CommunicationMessage
#define HEADER_CommunicationMessage

class CommunicationMessage;

#include <string>
#include <list>

class CommunicationMessage {

public:
	//! Parameter list type.
	typedef std::list<std::string> tParameters;
	//! The list of message parameters.
	tParameters mParameters;
	//! The message type (command).
	std::string mCommand;
	//! The associated request (if any)
	CommunicationMessage *mInReplyTo;

	//! Constructor.
	CommunicationMessage(const std::string &cmd = "", CommunicationMessage *inreplyto=0);
	//! Destructor.
	~CommunicationMessage() {}

	//! Returns a boolean from the parameter list.
	bool GetBool(bool defvalue);
	//! Returns an integer value from the parameter list.
	int GetInt(int defvalue);
	//! Returns a double from the parameter list.
	double GetDouble(double defvalue);
	//! Returns a string from the parameter list.
	std::string GetString(const std::string &defvalue);

	//! Adds a boolean value to the parameter list.
	bool AddBool(bool value);
	//! Adds an integer value to the parameter list.
	bool AddInt(int value);
	//! Adds a double to the parameter list.
	bool AddDouble(double value);
	//! Adds a string to the parameter list.
	bool AddString(const std::string &value);
	//! Adds a string to the parameter list.
	//bool AddString(const char *value);
};

#endif
