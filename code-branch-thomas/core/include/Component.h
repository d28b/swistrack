#ifndef _COMPONENT_H
#define _COMPONENT_H

#include <libxml++/libxml++.h>
#include "XMLCfg.h"

class OldComponent
    {
    
    protected:
        xmlpp::Element* cfgRoot;
        int mode;

	public:
		//virtual void SetParameters() = 0;
    };

#endif