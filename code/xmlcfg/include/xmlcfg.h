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

#define GetDoubleVal(node)\
    (atof(GetVal(node)))

#define GetDoubleValByXPath(root,path)\
    (GetDoubleVal((*((root->find(path)).begin()))))

/*/////// Setters for Attributes //////////////////*/
#define SetAttr(node,desc,value)\
 ((dynamic_cast<xmlpp::Element*> (node))->get_attribute(desc)->set_value(value))

#define SetAttrByXPath(root,path,desc,value)\
    (SetAttr(*((root)->find(path).begin()),desc,value))

/*//////// Setters for Textnodes //////////////////*/
			

#define ClearParam(root,node)\
	((root)->remove_child((dynamic_cast<xmlpp::Element*> (node))->get_child_text()))

#define ClearParamByXPath(root,path)\
	root->remove_child((dynamic_cast<xmlpp::Element*> (*(parent->cfgRoot->find(path).begin())))->get_child_text())

#define ExtendParam(node,value)\
	((dynamic_cast<xmlpp::Element*> (node))->add_child_text(value))

#define ExtendParamByXPath(root,path,value)\
	(ExtendParam(*((root)->find(path).begin()),value))

#define SetParamByXPath(root,path,value)\
		ClearParamByXPath(root,path);\
		ExtendParamByXPath(root,path,value);



/*/////// Test for existance of nodes and attributes //////////*/
#define IsAttr(node,desc)\
 ((dynamic_cast<const xmlpp::Element*> (node))->get_attribute(desc))

#define IsAttrByXPath(root,path,desc)\
 (IsAttr(*(root->find(path).begin()),desc))

#define CreateExceptionIfUnspecified(node,desc)\
		if(!IsAttr(node,desc)) throw "attribute desc not specified"; 

#define CreateExceptionIfUnspecifiedByXPath(root,path,desc)\
		if(!IsAttrByXPath(root,path,desc)) throw "path: attribute desc not specified"; 

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
