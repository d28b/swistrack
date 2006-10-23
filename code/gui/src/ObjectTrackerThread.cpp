#include "ObjectTrackerThread.h"
#include "SwisTrack.h"
#include "Canvas.h"
#include "AviWriter.h"
#include "SocketServer.h"
#include "SettingsDialog.h"
#include "SwisTrackPanel.h"
#include "InterceptionPanel.h"
#include "GUIApp.h"
#include <string.h>

ObjectTrackerThread::ObjectTrackerThread(ObjectTracker* _ot, wxCriticalSection* _cs, SwisTrack* _st)
: wxThread(), ot(_ot), cs(_cs), st(_st) {

}

ObjectTrackerThread::ObjectTrackerThread(ObjectTrackerThread & that) : wxThread() {

}

ObjectTrackerThread::~ObjectTrackerThread() {
}


void* ObjectTrackerThread::Entry() {
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