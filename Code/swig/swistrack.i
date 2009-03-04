%module swistrack
%{
#include "SwisTrackCore.h"
#include "Component.h"
#include "DataStructureImage.h"
#include "ComponentBackgroundSubtractionCheungKamath.h"
#include "ComponentCategory.h"
%}

%include SwisTrackCore.h
%include Component.h
%include ComponentCategory.h
%include ComponentBackgroundSubtractionCheungKamath.h
%include DataStructureImage.h
