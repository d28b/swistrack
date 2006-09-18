#ifndef _XMLCFG_H
#define _XMLCFG_H
#include <libxml++/libxml++.h>

/*! \file xmlcfg.h
\brief Contains macros for accessing libxml++ structures
*/

/*/////// Getters for Attributes //////////////////*/


/*! \brief Returns a string attribute
@param node A pointer to a node in an XML tree
@param desc The attribute's name

Example:

"node" points to <AVIFILE desc="AVI File" type="open" param="*.avi">file.avi</AVIFILE>.
Then, GetAttr(node,"param") would yield "*.avi*.
*/
#define GetAttr(node,desc)\
 ((dynamic_cast<const xmlpp::Element*> (node))->get_attribute(desc)->get_value().c_str())



/*! \brief Returns an integer attribute
@see GetAttr(node,desc)
\warning GetIntAttr uses atoi, and does not check whether the attribute is really an integer.
*/
#define GetIntAttr(node,desc)\
 (atoi((dynamic_cast<const xmlpp::Element*> (node))->get_attribute(desc)->get_value().c_str()))



/*! \brief Returns an integer attribute specified by an XPath

@param root A pointer to an XML tree
@param path An XPath to the node that has the attribute in question
@param desc The attribute's name
*/
#define GetIntAttrByXPath(root,path,desc)\
    (GetIntAttr(*(root->find(path).begin()),desc))


/*! \brief Returns a double attribute
@see GetAttr(node,desc)
\warning GetDoubleAttr uses atof, and does not check whether the attribute is really a double.
*/
#define GetDoubleAttr(node,desc)\
 (atof((dynamic_cast<const xmlpp::Element*> (node))->get_attribute(desc)->get_value().c_str()))



/*! \brief Returns a double attribute specified by its XPath
	\see GetAttrByXPath(root,path,desc)
	\see GetDoubleAttr(node,desc)
 */
#define GetDoubleAttrByXPath(root,path,desc)\
    (GetDoubleAttr(*(root->find(path).begin()),desc))




/*/////// Getters for Text fields //////////////////*/

/*! \brief Returns the child text of a node
@param node A pointer to a node of an XML tree
*/
#define GetVal(node)\
    ((dynamic_cast<const xmlpp::Element*> (node))->get_child_text()->get_content().c_str())

/*! \brief Retuns the child text of a node specified by an XPath
@param root A pointer to an XML tree
@param path An XPath to the node in question
*/
#define GetValByXPath(root,path)\
    (GetVal((*((root->find(path)).begin()))))

/*! \brief Returns the child integer of a node
\see GetVal(node)
\warning GetIntVal uses atoi, and does not check whether the child text is really an integer
*/
#define GetIntVal(node)\
    (atoi(GetVal(node)))

/*! \brief Returns the child integer of a node specified by an XPath
\see GetValByXPath(root,path)
*/
#define GetIntValByXPath(root,path)\
    (GetIntVal((*((root->find(path)).begin()))))


/*! \brief Returns the child double of a node
\see GetVal(node)
\warning GetIntVal uses atof, and does not check whether the child text is really an double
*/
#define GetDoubleVal(node)\
    (atof(GetVal(node)))

/*! \brief Returns the child double of a node specified by an XPath
\see GetValByXPath(root,path)
*/
#define GetDoubleValByXPath(root,path)\
    (GetDoubleVal((*((root->find(path)).begin()))))


/*/////// Setters for Attributes //////////////////*/
/*! \brief Sets an attribute to a specific value
@param node Pointer to a node, whose attribute will be modified
@param desc The attribute's name
@param value The new value (string) for the attribute
*/
#define SetAttr(node,desc,value)\
 ((dynamic_cast<xmlpp::Element*> (node))->get_attribute(desc)->set_value(value))

/*! \brief Sets an attribute to a specific value
@param root A pointer to an XML tree
@param path An XPath to the node containing the attribute in question
@param desc The attribute's name
@param value The new value (string) for the attribute
*/
#define SetAttrByXPath(root,path,desc,value)\
    (SetAttr(*((root)->find(path).begin()),desc,value))


/*//////// Setters for Textnodes //////////////////*/
/*! \brief Deletes the child text of a node
@param root A pointer to an XML tree
@param node Pointer to a node, whose child will be deleted
*/
#define ClearParam(root,node)\
	((root)->remove_child((dynamic_cast<xmlpp::Element*> (node))->get_child_text()))

/*! \brief Deletes the child text of a node
@param root A pointer to an XML tree
@param path An XPath to a node, whose child will be deleted
\see ClearParam(root,node)
*/
#define ClearParamByXPath(root,path)\
	root->remove_child((dynamic_cast<xmlpp::Element*> (*(root->find(path).begin())))->get_child_text())

/*! \brief Adds text to a node's child text
@param node Pointer to a node, whose child will be extended
@param value The value (string) that will be concatenated with the current child text
*/
#define ExtendParam(node,value)\
	((dynamic_cast<xmlpp::Element*> (node))->add_child_text(value))

/*! \brief Adds text to a node's child text
@param root A pointer to an XML tree
@param path An XPath to a node, whose child will be extended
@param value The value (string) that will be concatenated with the current child text
\see ExtendParam(node,value)
*/
#define ExtendParamByXPath(root,path,value)\
	(ExtendParam(*((root)->find(path).begin()),value))

/*! \brief Sets a node's child text
@param root A pointer to an XML tree
@param path An XPath to a node, whose child will be set
@param value The new value (string) for the node's child text
\see ClearParamByXPath(root,path)
\see ExtendParamByXPath(root,path,value)
*/
#define SetParamByXPath(root,path,value)\
		CreateExceptionIfEmpty(root,path);\
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
	if(!IsDefined(root,path)){ \
			char _str_[2000];\
			strcpy(_str_,"Path not found:\n");\
			strcat(_str_,path);\
            throw _str_; \
	}else \
	if(!IsContent(root,path)){ \
			char _str_[2000];\
			strcpy(_str_,"Empty field:\n");\
			strcat(_str_,path);\
            throw _str_; \
	}

#endif
