##
##  Makefile for gui (as part of SwisTrack)
##  T. Lochmatter 2009-02-16
##

# Configuration
TARGET_FOLDER := ../../SwisTrackEnvironment
TARGET        := $(TARGET_FOLDER)/SwisTrack

include ../../Makefile.inc

# Files
FILES_CPP := $(wildcard *.cpp)
FILES_H   := $(wildcard *.h)
FILES_O   := $(FILES_CPP:.cpp=.o)
FILES_D   := $(FILES_CPP:.cpp=.d)
FILES_A   := ../core/libSwistrackCore.a ../../3rdParty/cvblobslib_v6p1/libCVBlobs.a ../../3rdParty/libtsai/libtsai.a ../../3rdParty/camshift_wrapper/libCamshiftWrapper.a

# Flags
INCLUDE   := -I/usr/include `pkg-config --cflags opencv` -I../core
CXXFLAGS  := -g -Wall `wx-config --cxxflags`
CPPFLAGS  := $(INCLUDE) `wx-config --cppflags`
LDLIBS    := `pkg-config --libs opencv` `wx-config --libs` --lstdc++
LDFLAGS   :=

# Build
.PHONY: all
all: $(TARGET)

$(TARGET): $(FILES_O) $(FILES_A)
	@echo "=== Building $@"
	$(CXX) $(LDFLAGS) $(FILES_O) $(FILES_A) $(LDLIBS) -o $@

.PHONY: osxbundle
osxbundle: $(TARGET)
	cd $(TARGET_FOLDER) && tar xfz ../AppleXCode/SwisTrack.app.skeleton.tar.gz
	cp $(TARGET) $(TARGET_FOLDER)/SwisTrack.app/Contents/MacOS/

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
