##
##  Makefile for camshift_wrapper (as part of SwisTrack)
##

SWISTRACK_ROOT := ../..
include $(SWISTRACK_ROOT)/Makefile.inc

# Configuration
TARGET    := libCamshiftWrapper.a

# Files
FILES_C   := $(wildcard *.c)
FILES_H   := $(wildcard *.h)
FILES_O   := $(FILES_C:.c=.o)
FILES_D   := $(FILES_C:.c=.d)

# Flags
override CFLAGS    += -g -Wall -Wno-unused-function
override CPPFLAGS  += $(OPENCV_CPPFLAGS) -I/usr/lib/include/opencv

# Build
.PHONY: all
all: $(TARGET)

$(TARGET): $(FILES_O)
	@echo "=== Building $@"
	$(AR) -r $@ $(FILES_O)

$(FILES_O): %.o: %.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

# Cleanup
.PHONY: clean
clean:
	- rm $(FILES_O) $(FILES_D) $(TARGET)

# Tags
.PHONY: tags
tags: $(FILES_C) $(FILES_H)
	$(CTAGS) $(FILES_C) $(FILES_H)

# Dependency file creation
$(FILES_D): %.d: %.c
	$(CC) -MM $(CPPFLAGS) $< > $@;

# Dependency file inclusion
ifneq ($(MAKECMDGOALS),clean)
include $(FILES_D)
endif
