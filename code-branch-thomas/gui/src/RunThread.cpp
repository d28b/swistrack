#include "RunThread.h"
#define THISCLASS RunThread

THISCLASS::RunThread(SwisTrack *st):
		wxThread(), mSwisTrack(st) {

}

THISCLASS::~RunThread() {
}


void* THISCLASS::Entry() {
	while (true) {
	
		if (TestDestroy() == true) {
			return NULL;
		}
		else {
			{
				wxCriticalSectionLocker locker(*cs);	
				int otStatus = ot->GetStatus();
				int stStatus = st->GetStatus();
				if (stStatus == RUNNING && otStatus == RUNNING) {
					ot->Step();
				}
			}
		}
		Sleep(10);
		
		if (TestDestroy() == true) {
			return NULL;
		}
	}
	return NULL;
}