##
##  Makefile for gui (as part of SwisTrack)
##  T. Lochmatter 2009-02-16
##

# Configuration
TARGET    := ../../SwisTrackEnvironment/SwisTrack-batch

# Toolchain (compiler, linker)
SHELL     := /bin/sh
CTAGS     := ctags
#CC        := cc
#AR        := ar

# Files
FILES_CPP := $(wildcard *.cpp)
FILES_H   := $(wildcard *.h)
FILES_O   := $(FILES_CPP:.cpp=.o)
FILES_D   := $(FILES_CPP:.cpp=.d)

# Flags
INCLUDE   := -I/usr/include -I/usr/include/opencv -I../core
CXXFLAGS  := -g -Wall `wx-config --cxxflags`
CPPFLAGS  := $(INCLUDE) `wx-config --cppflags`
LDLIBS    := -lm -lcv -lcxcore -lhighgui `wx-config --libs`
LDFLAGS   :=

# Build
.PHONY: all
all: $(TARGET)

$(TARGET): $(FILES_O)
	$(MAKE) -C ../libtsai
	$(MAKE) -C ../camshift_wrapper
	$(MAKE) -C ../core
	@echo "=== Building $@"
	$(CC) $(LDFLAGS) $(FILES_O) ../core/core.a ../libtsai/libtsai.a ../camshift_wrapper/camshift_wrapper.a $(LDLIBS) -o $@

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
