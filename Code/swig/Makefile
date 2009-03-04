##
##  Makefile for swig (as part of SwisTrack)
##

SWISTRACK_ROOT := ../..
include $(SWISTRACK_ROOT)/Makefile.inc


FILES_C   := $(wildcard *.c) 
FILES_CPP := $(wildcard *.cpp)
FILES_H   := $(wildcard *.h)
FILES_O   := $(FILES_CPP:.cpp=.o)
FILES_D   := $(FILES_CPP:.cpp=.d)
FILES_COBJ:= $(FILES_C:.c=.o)

CPPFLAGS  := $(OPENCV_CPPFLAGS) $(WXWIDGETS_CPPFLAGS) -I/usr/include -I../../3rdParty/libtsai/include -I../../3rdParty/camshift_wrapper -I../../3rdParty/cvblobslib_v6p1 $(CPPFLAGS_ADD) -I$(SWISTRACK_ROOT)/Code/core
LDFLAGS   := $(LDFLAGS_ADD)

%_wrap.cxx: %.i $(FILES_H)
	swig -Wall -python -c++ $(CPPFLAGS) $<
%_wrap.o: %_wrap.cxx
	$(CXX) $(CPPFLAGS) $(CXXFLAGS)  -c $< `python2.5-config --cflags`


_%.so: %_wrap.o
ifeq ($(OS), Darwin)
	$(CXX) -Wall -dynamiclib  $< $(LDFLAGS) -L.  -lStefieSquintReader `python2.5-config --ldflags` -o $@
else
	$(CXX) -Wall -shared  $< $(LDFLAGS) -L.  -lStefieSquintReader `python2.5-config --ldflags` -o $@
endif

.PHONY: all
.SECONDARY:

all: _swistrack.so