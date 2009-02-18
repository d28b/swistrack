# General
TARGET_FOLDER := $(SWISTRACK_ROOT)/SwisTrackEnvironment

# Toolchain
SHELL     := /bin/sh
CTAGS     := ctags
CC        := cc
CXX       := g++
AR        := ar

# OpenCV (activate the second set of lines in case you don't have pkg-config)
OPENCV_CPPFLAGS := `pkg-config --cflags opencv`
OPENCV_LDLIBS   := `pkg-config --libs opencv`
#OPENCV_CPPFLAGS := -I/usr/include/opencv
#OPENCV_LDLIBS   := -lcxcore -lcv -lhighgui -lcvaux -lml

# wxWidgets
WXWIDGETS_CPPFLAGS := `wx-config --cppflags`
WXWIDGETS_LDLIBS   := `wx-config --libs`

# GiGE Camera Component (activate the following lines to compile with this component)
#CPPFLAGS_ADD := -I/opt/pylon/include -I/opt/pylon/include/genicam -DUSE_CAMERA_PYLON_GIGE
#LDLIBS_ADD := -lpylon
