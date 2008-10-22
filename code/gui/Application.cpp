#include "Application.h"
#define THISCLASS Application
using namespace std;
#include <iostream>
#include <wx/app.h>
#include <wx/image.h>
#include <wx/filefn.h>

#include "ConfigurationReaderXML.h"
#include "SwisTrackCore.h"
#include "Random.h"

IMPLEMENT_APP_NO_MAIN(Application)

#include <stdio.h>
void usage() {
  cout << "Our option parser sucks.  Either zero, one, or two arguments." <<endl;
  cout << "  ./SwisTrack" << endl;
  cout << "  ./SwisTrack something.swistrack" << endl;
  cout << "  ./SwisTrack --batch something.swistrack" << endl;
}
int runBatch(char * filename) {
  cout << "Starting batch processing " << endl;
  SwisTrackCore * core = new SwisTrackCore(wxT("./Components"));
  wxFileName fn(wxString::FromAscii(filename));
  wxFileName::SetCwd(wxT(".."));
  if (! fn.IsFileReadable()) {
    cout << "Could not read " << fn.GetFullPath().ToAscii() << endl;
    return -1;
  }
  ConfigurationReaderXML cr;
  if (! cr.Open(fn.GetFullPath())) {
    cout << "Could not read " << fn.GetFullPath().ToAscii() << " Syntax error? " << endl;
    return -1;
  }
  cr.ReadComponents(core);
  ErrorList & errors = cr.mErrorList;
  for (ErrorList::tList::iterator it = errors.mList.begin(); it != errors.mList.end(); it++) {
    cout << "Error " << it->mLineNumber << ": " << it->mMessage << endl; 
  }
  if (errors.mList.size() != 0) {
    return -1;
  }
  for (std::list<Component*>::const_iterator it = core->GetDeployedComponents()->begin();
       it != core->GetDeployedComponents()->end(); it++) {
    Component * component = *it;
    cout << "Comp: " <<  component->mDisplayName.ToAscii() << endl;
    if (component->mStatusHasError) {
      cout << "Has an error" << endl;
      return -1;
    }
    if (component->mStatusHasWarning) {
      cout << "Has a warning" << endl;
      return -1;
    }
  }
  if (core->GetDeployedComponents()->size() == 0) {
    cout << "No available components." << endl;
    return -1;
  }
  cout << "Running with " << core->GetDeployedComponents()->size() << " components." << endl;
  core->TriggerStart();
  core->Start(false);
  for (std::list<Component*>::const_iterator it = core->GetDeployedComponents()->begin();
       it != core->GetDeployedComponents()->end(); it++) {
    Component * component = *it;
    if (!component->mStarted) {
      cout << component->mDisplayName.ToAscii() << " not started." << endl;
    }
    
    for (std::list<StatusItem>::iterator it = component->mStatus.begin(); 
	 it != component->mStatus.end(); it++) {
      StatusItem & item = *it;
      cout <<"Error: " << item.mMessage.ToAscii() << endl;
    }
    if (component->mStatusHasError) {
      cout << "Has an error" << endl;
      return -1;
    }
    if (component->mStatusHasWarning) {
      cout << "Has a warning" << endl;
    }
  }
  
  while (core->IsTriggerActive()) {
    core->Step();
  }
  core->Stop();
  exit(0);
}
int main( int argc, char **argv) {
  if (argc <= 2) {
    return wxEntry(argc, argv);
  } else if ((argc == 3) && 
	     (wxString::FromAscii(argv[1]) == wxT("--batch"))) {
    runBatch(argv[2]);
  } else {
    usage();
    return -1;
  }
  }

bool THISCLASS::OnInit() {
	// Set some main application parameters.
	SetVendorName(wxT("Swarm-Intelligent Systems Group, EPFL, Lausanne, Switzerland"));
	SetAppName(wxT("SwisTrack"));

	// Take the current directory as the application folder
	mApplicationFolder = wxGetCwd();

	// Initialize the random number generator
	Random::Initialize();

	// Initialize all available image handlers
	wxInitAllImageHandlers();

	// Create the main application window
	mSwisTrack = new SwisTrack(_T("SwisTrack"), wxPoint(-1, -1), wxSize(1024, 700));

	// Open a file
	if (argc > 1) {
		mSwisTrack->OpenFile(argv[1], true, false);
	} else {
		mSwisTrack->OpenFile(mApplicationFolder + wxT("/default.swistrack"), false, true);
	}

	// Show
	mSwisTrack->Show(TRUE);
	SetTopWindow(mSwisTrack);



#if defined(__WIN16__) || defined(__WXMOTIF__)
	int width, height;
	frame->GetSize(&width, &height);
	frame->SetSize(-1, -1, width, height);
#endif


	return TRUE;
}

int THISCLASS::OnExit() {
	// Uninitialize the random number generator
	Random::Uninitialize();

	return 0;
}
