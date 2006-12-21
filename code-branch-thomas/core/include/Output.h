#ifndef _Output_H
#define _Output_H

#include "Component.h"
#include "cv.h"
#include "ObjectTracker.h"

/** \class Output
*
* \brief Provides data logging functions
*/
class Output : OldComponent
	{
	public:
        Output(ObjectTracker* parent, xmlpp::Element* cfgRoot);
	void WriteRow();
	    ~Output();
	

	private:
	FILE* output;	
    int ncols;
	ObjectTracker* parent;
	};

#endif
