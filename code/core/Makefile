##
##  Makefile for core (as part of SwisTrack)
##  T. Lochmatter 2009-02-16
##

# Configuration
TARGET    := core.a

# Toolchain (compiler, linker)
SHELL     := /bin/sh
CTAGS     := ctags
#CXX       := c++
#AR        := ar

# Files
FILES_CPP := $(wildcard *.cpp)
FILES_H   := $(wildcard *.h)
FILES_O   := $(FILES_CPP:.cpp=.o)
FILES_D   := $(FILES_CPP:.cpp=.d)

# Flags
INCLUDE   := -I/usr/include -I/usr/include/opencv `wx-config --cppflags` -I../libtsai/include -I../camshift_wrapper
CXXFLAGS  := -g -Wall
CPPFLAGS  := -DUNIX $(INCLUDE)

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
