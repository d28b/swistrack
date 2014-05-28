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
WXWIDGETS_CPPFLAGS := `wx-config --version=2.8 --cppflags`
WXWIDGETS_LDLIBS   := `wx-config --version=2.8 --libs`

#glib
GLIB_CPPFLAGS :=`pkg-config --cflags glib-2.0`
GLIB_LDLIBS    :=`pkg-config --libs glib-2.0`

CPPFLAGS_ADD += $(GLIB_CPPFLAGS)
LDLIBS_ADD   += $(GLIB_LDLIBS)

#switch GPU/CUDA code on
#CPPFLAGS_ADD += -DGPU_ENABLED

# GiGE PYLON Camera Component (activate the following lines to compile with this component)
#PYLON_ROOT         := /opt/pylon
#GENICAM_ROOT_V1_1  := /opt/pylon
#CPPFLAGS_ADD       += -I$(PYLON_ROOT)/include -I$(GENICAM_ROOT_V1_1)/include/genicam -DUSE_CAMERA_PYLON_GIGE
#LDLIBS_ADD         += -L$(PYLON_ROOT)/lib -L$(GENICAM_ROOT_V1_1)/lib -lpylonbase

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
