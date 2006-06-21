#ifndef _COMPONENT_H
#define _COMPONENT_H

#include <libxml++/libxml++.h>
#include "XMLCfg.h"

class Component
    {
    
    protected:
        xmlpp::Element* cfgRoot;
        int mode;
    };

#endif