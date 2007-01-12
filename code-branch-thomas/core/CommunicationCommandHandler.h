#ifndef HEADER_CommunicationCommandHandler
#define HEADER_CommunicationCommandHandler

class CommunicationCommandHandler;

#include "CommunicationMessage.h"

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
