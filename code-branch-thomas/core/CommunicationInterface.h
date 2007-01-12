#ifndef HEADER_CommunicationInterface
#define HEADER_CommunicationInterface

class CommunicationInterface;

#include <string>
#include <list>
#include "CommunicationMessage.h"
#include "CommunicationCommandHandler.h"

class CommunicationInterface {

public:
	//! Command handler list type.
	typedef std::list<CommunicationCommandHandler*> tCommandHandlerList;
	//! The list of command handlers.
	tCommandHandlerList mCommandHandlerList;

	//! Constructor.
	CommunicationInterface(): mCommandHandlerList() {}
	//! Destructor.
	virtual ~CommunicationInterface() {}

	//! This is called to send a message. The return value should indicate whether the message has really been sent to at least one client.
	virtual bool SendMessage(CommunicationMessage *m) = 0;

	//! Registers a new command handler.
	void AddCommandHandler(CommunicationCommandHandler *ch);
	//! Removes a command handler.
	void RemoveCommandHandler(CommunicationCommandHandler *ch);

	//! Relays a message to all command handlers.
	bool OnCommunicationCommand(CommunicationMessage *m);
};

#endif
