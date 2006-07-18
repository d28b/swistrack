#ifndef _XMLCFG_H
#define _XMLCFG_H
#include <libxml++/libxml++.h>

/*/////// Getters for Attributes //////////////////*/
#define GetAttr(node,desc)\
 ((dynamic_cast<const xmlpp::Element*> (node))->get_attribute(desc)->get_value().c_str())
 
#define GetIntAttr(node,desc)\
 (atoi((dynamic_cast<const xmlpp::Element*> (node))->get_attribute(desc)->get_value().c_str()))

#define GetIntAttrByXPath(root,path,desc)\
    (GetIntAttr(*(root->find(path).begin()),desc))

#define GetDoubleAttr(node,desc)\
 (atof((dynamic_cast<const xmlpp::Element*> (node))->get_attribute(desc)->get_value().c_str()))

#define GetDoubleAttrByXPath(root,path,desc)\
    (GetDoubleAttr(*(root->find(path).begin()),desc))

/*/////// Getters for Text fields //////////////////*/
#define GetVal(node)\
    ((dynamic_cast<const xmlpp::Element*> (node))->get_child_text()->get_content().c_str())

#define GetValByXPath(root,path)\
    (GetVal((*((root->find(path)).begin()))))

#define GetIntVal(node)\
    (atoi(GetVal(node)))

#define GetIntValByXPath(root,path)\
    (GetIntVal((*((root->find(path)).begin()))))

/*/////// Setters for Attributes //////////////////*/
#define SetAttr(node,desc,value)\
 ((dynamic_cast<xmlpp::Element*> (node))->get_attribute(desc)->set_value(value))

#define SetAttrByXPath(root,path,desc,value)\
    (SetAttr(*((root)->find(path).begin()),desc,value))


/*/////// Test for existance of nodes and attributes //////////*/
#define IsAttr(node,desc)\
 ((dynamic_cast<const xmlpp::Element*> (node))->get_attribute(desc))

#define IsAttrByXPath(root,path,desc)\
 (IsAttr(*(root->find(path).begin()),desc))

#define IsDefined(root,path)\
    ((root)->find(path).size())

#define IsContent(root,path)\
    (dynamic_cast<const xmlpp::Element*>((*(root->find(path).begin())))->has_child_text())


#define CreateExceptionIfEmpty(root,path)\
       if(!IsDefined(root,path)) \
            throw "path: node not specified"; \
       else \
        if(!IsContent(root,path)) \
         throw "path: node without value"


#endif
