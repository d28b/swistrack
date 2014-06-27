# General
TARGET_FOLDER := $(SWISTRACK_ROOT)/SwisTrackEnvironment

# Toolchain
SHELL     := /bin/sh
CTAGS     := ctags
CC        := cc
CXX       := g++
AR        := ar

# OpenCV (activate the second set of lines in case you don't have pkg-config)
OPENCV_CPPFLAGS    := `pkg-config --cflags opencv`
OPENCV_LDLIBS      := `pkg-config --libs opencv`

#OPENCV_CPPFLAGS    := -I/usr/include/opencv
#OPENCV_LDLIBS      := -lcxcore -lcv -lhighgui -lcvaux -lml

# wxWidgets
WXWIDGETS_CPPFLAGS := `wx-config --cppflags`
WXWIDGETS_LDLIBS   := `wx-config --libs`

#glib
GLIB_CPPFLAGS :=`pkg-config --cflags glib-2.0`
GLIB_LDLIBS    :=`pkg-config --libs glib-2.0`

CPPFLAGS_ADD += $(GLIB_CPPFLAGS)
LDLIBS_ADD   += $(GLIB_LDLIBS)

#switch GPU/CUDA code on
#CPPFLAGS_ADD += -DGPU_ENABLED

# GiGE PYLON Camera Component (activate to compile component)
# Defaults for Pylon 1
#PYLON_ROOT         := /opt/pylon
#GENICAM_ROOT_V1_1  := /opt/pylon
#CPPFLAGS_ADD       += -I$(PYLON_ROOT)/include -I$(GENICAM_ROOT_V1_1)/include/genicam -DUSE_CAMERA_PYLON_GIGE
#LDLIBS_ADD         += -L$(PYLON_ROOT)/lib -L$(GENICAM_ROOT_V1_1)/lib -lpylonbase

# GiGE PYLON Camera Component (activate to compile component)
# Defaults for Pylon 3 (replace Linux32_i86 with Linux64_x64 if compiling 64 bit)
#PYLON_ROOT         := /opt/pylon3
#GENICAM_ROOT_V2_3  := /opt/pylon3/genicam
#CPPFLAGS_ADD       += -isystem$(PYLON_ROOT)/include -isystem$(GENICAM_ROOT_V2_3)/library/CPP/include -DUSE_CAMERA_PYLON_GIGE -DUSE_GIGE -DUSE_PYLON3
#LDFLAGS_ADD        += -L$(PYLON_ROOT)/lib -L$(GENICAM_ROOT_V2_3)/library/CPP/lib/Linux32_i86 -L$(GENICAM_ROOT_V2_3)/bin/Linux32_i86 -L$(GENICAM_ROOT_V2_3)/bin/Linux32_i86/GenApi/Generic -Wl,-E
#LDLIBS_ADD         += -lpylonbase -lGenApi_gcc40_v2_3 -lGCBase_gcc40_v2_3 -lLog_gcc40_v2_3 -lMathParser_gcc40_v2_3 -lXerces-C_gcc40_v2_7 -llog4cpp_gcc40_v2_3

#ARV_ROOT := $(SWISTRACK_ROOT)/3rd_Party/aravis-0.2.2
#CPPFLAGS_ADD        +=  -DUSE_CAMERA_ARV_GIGE
#LDLIBS_ADD         += -L$(ARV_ROOT)/src  -laravis-0.2

# GiGE Proselica Camera Component (activate the following lines to compile with this component)
#CPPFLAGS_ADD       += -DUSE_CAMERA_PROSILICA_GIGE

# SQlite3 (activate the following lines to compile with components using this library)
#CPPFLAGS_ADD       += -DUSE_SQLITE3
#LDLIBS_ADD         += -lsqlite3

# XVID component (activate the following lines to compile with this component)
#CPPFLAGS_ADD       += -DUSE_XVID
#LDLIBS_ADD         += -lxvidcore

# Boost (activate the following lines to compile with components using the Boost library)
#CPPFLAGS_ADD       += -DUSE_BOOST
#LDLIBS_ADD         += -lboost_graph
