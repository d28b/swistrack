##
##  Makefile for batch (as part of SwisTrack)
##

SWISTRACK_ROOT := ../..
include $(SWISTRACK_ROOT)/Makefile.inc

# Configuration
TARGET    := $(TARGET_FOLDER)/SwisTrack-batch

# Files
FILES_CPP := $(wildcard *.cpp)
FILES_H   := $(wildcard *.h)
FILES_O   := $(FILES_CPP:.cpp=.o)
FILES_D   := $(FILES_CPP:.cpp=.d)
FILES_A   := ../core/libSwisTrackCore.a ../../3rdParty/cvblobslib_v6p1/libCVBlobs.a ../../3rdParty/libtsai/libtsai.a ../../3rdParty/camshift_wrapper/libCamshiftWrapper.a

# Flags
CXXFLAGS  := -g -Wall $(CXXFLAGS_ADD)
CPPFLAGS  := $(OPENCV_CPPFLAGS) $(WXWIDGETS_CPPFLAGS) -I/usr/include -I../core $(CPPFLAGS_ADD)
LDLIBS    := $(OPENCV_LDLIBS) $(WXWIDGETS_LDLIBS) -lm $(LDLIBS_ADD)
LDFLAGS   := $(LDFLAGS_ADD)

# Build
.PHONY: all
all: $(TARGET)

$(TARGET): $(FILES_O) $(FILES_A)
	@echo "=== Building $@"
	$(CXX) $(LDFLAGS) $(FILES_O) $(FILES_A) $(LDLIBS) -o $@

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
