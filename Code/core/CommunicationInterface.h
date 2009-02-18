#ifndef HEADER_CommunicationInterface
#define HEADER_CommunicationInterface

class CommunicationInterface;

#include <wx/string.h>
#include <list>
#include "CommunicationMessage.h"
#include "CommunicationCommandHandler.h"

//! The interface class for communication messages. In the current SwisTrack, this interface is implemented as a TCP server which forwards the messages to TCP clients.
class CommunicationInterface {

public:
	//! Command handler list type.
	typedef std::list<CommunicationCommandHandler*> tCommandHandlerList;

	tCommandHandlerList mCommandHandlerList; //!< The list of command handlers.

	//! Constructor.
	CommunicationInterface(): mCommandHandlerList() {}
	//! Destructor.
	virtual ~CommunicationInterface() {}

	//! This is called to send a message. The return value should indicate whether the message has really been sent to at least one client.
	virtual bool Send(CommunicationMessage *m) = 0;

	//! Registers a new command handler.
	void AddCommandHandler(CommunicationCommandHandler *ch);
	//! Removes a command handler.
	void RemoveCommandHandler(CommunicationCommandHandler *ch);

	//! Relays a message to all command handlers.
	bool OnCommunicationCommand(CommunicationMessage *m);
};

#endif
