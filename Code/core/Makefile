##
##  Makefile for core (as part of SwisTrack)
##

SWISTRACK_ROOT := ../..
include $(SWISTRACK_ROOT)/Makefile.inc

# Configuration
TARGET    := libSwisTrackCore.a

# Files
FILES_CPP := $(wildcard *.cpp)
FILES_H   := $(wildcard *.h)
FILES_O   := $(FILES_CPP:.cpp=.o)
FILES_D   := $(FILES_CPP:.cpp=.d)

# Flags
override CXXFLAGS  += -g -Wall $(CXXFLAGS_ADD)
override CPPFLAGS  += $(OPENCV_CPPFLAGS) $(WXWIDGETS_CPPFLAGS) -isystem/usr/include -isystem../../3rdParty/libtsai/include -isystem../../3rdParty/camshift_wrapper -isystem../../3rdParty/cvblobslib_v6p1 $(CPPFLAGS_ADD) -g

# Build
.PHONY: all
all: $(TARGET)

$(TARGET): $(FILES_O)
	@echo "=== Building $@"
	$(AR) -r $@ $(FILES_O)

$(FILES_O): %.o: %.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

# Cleanup
.PHONY: clean
clean:
	- rm $(FILES_O) $(FILES_D) $(TARGET)

# Tags
.PHONY: tags
tags: $(FILES_CPP) $(FILES_H)
	$(CTAGS) $(FILES_CPP) $(FILES_H)

# Dependency file creation
$(FILES_D): %.d: %.cpp
	$(CXX) -MM $(CPPFLAGS) $< > $@;

# Dependency file inclusion
ifneq ($(MAKECMDGOALS),clean)
include $(FILES_D)
endif
