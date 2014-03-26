#ifndef HEADER_CommunicationCommandHandler
#define HEADER_CommunicationCommandHandler

class CommunicationCommandHandler;

#include "CommunicationMessage.h"

//! An interface class for command handlers. A command handler can receive commands in form CommunicationMessage objects and may reply to them. In the current implementation of SwisTrack, these commands come from a TCP client.
class CommunicationCommandHandler {

public:
	//! Constructor.
	CommunicationCommandHandler() {}
	//! Destructor.
	virtual ~CommunicationCommandHandler() {}

	//! This event is called when a message arrives.
	virtual bool OnCommunicationCommand(CommunicationMessage *m) = 0;
};

#endif
