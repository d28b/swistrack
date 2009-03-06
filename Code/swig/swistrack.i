%module swistrack
%{
#include "SwisTrackCore.h"
#include "ConfigurationXML.h"
#include "ConfigurationReaderXML.h"
#include "DataStructure.h"
#include "DataStructureInput.h"
#include "DataStructureImage.h"
#include "Component.h"
#include "ComponentBackgroundSubtractionCheungKamath.h"
#include "ComponentCategory.h"
#include "wx/wxPython/wxPython.h"
#include "wx/wxPython/pyclasses.h"
#include "ErrorList.h"
#include "ErrorListItem.h"
#include "StatusItem.h"
#include "Utility.h"
%}


%import typemapsWxPython.i
//%import typemapsOpenCv.i 
%include std_list.i

namespace std {
	  %template(tList) list<ErrorListItem>;
	  %template(tComponentList) list<Component*>;
	  %template(tStatusItemList) list<StatusItem>;
}

%include SwisTrackCore.h
%include Component.h
%include ComponentCategory.h
%include ComponentBackgroundSubtractionCheungKamath.h
%include DataStructure.h
%include DataStructureImage.h
%include DataStructureInput.h
%include ConfigurationXML.h
%include ConfigurationReaderXML.h
%include ErrorList.h
%include ErrorListItem.h
%include StatusItem.h
%include Utility.h
