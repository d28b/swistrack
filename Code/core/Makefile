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
override CPPFLAGS  += -Wall $(OPENCV_CPPFLAGS) $(WXWIDGETS_CPPFLAGS) -isystem../../3rdParty/libtsai/include $(CPPFLAGS_ADD)

# Build
.PHONY: all
all: $(TARGET)

$(TARGET): $(FILES_O)
	@echo "=== Building $@"
	$(AR) -r $@ $(FILES_O)

$(FILES_O): %.o: %.cpp
	$(CXX) -c $(CPPFLAGS) $< -o $@

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

test: test.o
	$(CXX) $(LDFLAGS_ADD) test.o $(OPENCV_LDLIBS) $(WXWIDGETS_LDLIBS) -lm $(LDLIBS_ADD) -o $@
