#ifndef _datalogger_H
#define _datalogger_H

#include "Component.h"
#include "cv.h"
#include "ObjectTracker.h"

/** \class DataLogger
*
* \brief Provides data logging functions
*/
class DataLogger : Component
	{
	public:
        DataLogger(ObjectTracker* parent, xmlpp::Element* cfgRoot);
	void WriteRow();
	    ~DataLogger();
	

	private:
	FILE* output;	
    int ncols;
	ObjectTracker* parent;
	};

#endif
