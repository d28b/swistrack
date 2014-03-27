##
##  Makefile for SwisTrackButton
##

SWISTRACK_ROOT := ../../../
include $(SWISTRACK_ROOT)/Makefile.inc

# Configuration
TARGET    := SwisTrackButton

# Files
FILES_CPP := $(wildcard *.cpp)
FILES_H   := $(wildcard *.h)
FILES_O   := $(FILES_CPP:.cpp=.o)
FILES_D   := $(FILES_CPP:.cpp=.d)

# Flags
override CXXFLAGS  += -g -Wall $(CXXFLAGS_ADD)
override CPPFLAGS  += $(WXWIDGETS_CPPFLAGS) -I/usr/include -I../core $(CPPFLAGS_ADD)
override LDLIBS    += $(WXWIDGETS_LDLIBS) -lm $(LDLIBS_ADD)
override LDFLAGS   += $(LDFLAGS_ADD)

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
