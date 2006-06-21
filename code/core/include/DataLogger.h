#ifndef _datalogger_H
#define _datalogger_H

#include "Component.h"

/** \class DataLogger
*
* \brief Provides data logging functions
*/
class DataLogger : Component
	{
	public:
        DataLogger(char * fname, xmlpp::Element* cfgRoot);
	void WriteRow(int frame, double time);
	    ~DataLogger();
	

	private:
	FILE* rawcalib;
	FILE* rawuncalib;
    int ncols;
	};

#endif
