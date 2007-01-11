#include "CommunicationInterface.h"
#include <algorithm>
#define THISCLASS CommunicationInterface

void THISCLASS::AddCommandHandler(CommunicationCommandHandler *ch) {
	mCommandHandlerList.push_back(ch);
}

void THISCLASS::RemoveCommandHandler(CommunicationCommandHandler *ch) {
	tCommandHandlerList::iterator it = find(mCommandHandlerList.begin(), mCommandHandlerList.end(), ch);
	if (it != mCommandHandlerList.end()) {mCommandHandlerList.erase(it);}
}

bool THISCLASS::OnCommand(CommunicationMessage *m) {
	bool ok=false;

	tCommandHandlerList::iterator it=mCommandHandlerList.begin();
	while (it!=mCommandHandlerList.end()) {
		bool res=(*it)->OnCommand(m);
		if (res) {ok=true;}
		it++;
	}
	
	return ok;
}
