%module swistrack
%{
#include "SwisTrackCore.h"
#include "Component.h"
#include "DataStructureImage.h"
#include "ComponentBackgroundSubtractionCheungKamath.h"
#include "ComponentCategory.h"
#include "wx/wxPython/wxPython.h"
#include "wx/wxPython/pyclasses.h"

%}


%import my_typemaps.i


%include SwisTrackCore.h
%include Component.h
%include ComponentCategory.h
%include ComponentBackgroundSubtractionCheungKamath.h
%include DataStructureImage.h
