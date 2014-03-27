##
##  Makefile for swig (as part of SwisTrack)
##

SWISTRACK_ROOT := ../..
include $(SWISTRACK_ROOT)/Makefile.inc


FILES_C   := $(wildcard *.c) 
FILES_I   := $(wildcard *.i) 
FILES_CPP := $(wildcard *.cpp)
FILES_H   := $(wildcard *.h)
FILES_O   := $(FILES_CPP:.cpp=.o)
FILES_D   := $(FILES_CPP:.cpp=.d)
FILES_COBJ:= $(FILES_C:.c=.o)
FILES_A   := ../core/libSwisTrackCore.a ../../3rdParty/cvblobslib_v6p1/libCVBlobs.a ../../3rdParty/libtsai/libtsai.a ../../3rdParty/camshift_wrapper/libCamshiftWrapper.a

override CPPFLAGS  += $(OPENCV_CPPFLAGS) $(WXWIDGETS_CPPFLAGS) -I/usr/include -I../../3rdParty/libtsai/include -I../../3rdParty/camshift_wrapper -I../../3rdParty/cvblobslib_v6p1 $(CPPFLAGS_ADD) -I$(SWISTRACK_ROOT)/Code/core
override LDFLAGS   += $(OPENCV_LDLIBS) $(WXWIDGETS_LDLIBS) -lm $(LDLIBS_ADD)$(LDFLAGS_ADD)

%_wrap.cxx: %.i $(FILES_I) $(FILES_H)
	swig -Wall -python -c++ $(CPPFLAGS) $<
%_wrap.o: %_wrap.cxx
	$(CXX) $(CPPFLAGS) $(CXXFLAGS)  -c $< `python2.5-config --cflags`


_%.so: %_wrap.o $(FILES_A)
ifeq ($(OS), Darwin)
	$(CXX) -Wall -dynamiclib  $< $(LDFLAGS) -L.   `python2.5-config --ldflags` -o $@ $(FILES_A)
else
	$(CXX) -Wall -shared  $< $(LDFLAGS) -L. `python2.5-config --ldflags` -o $@ $(FILES_A)
endif

.PHONY: all
.SECONDARY:

all: _swistrack.so

test:
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH) PYTHONPATH=. python swig_test.py

clean:
	- rm $(FILES_O) $(FILES_D) $(FILES_COBJ) 	$(TARGET) *_wrap.* *.so
