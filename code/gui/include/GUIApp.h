#ifndef _GUIApp_H
#define _GUIApp_H
#include "SwisTrack.h"


/** \class AppRoot
* \brief Main application holding the application window
* Implements the OnInit() method of the wxApp class it inherits from.
*/ 
class GUIApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
	SwisTrack* frame;

};
#endif
