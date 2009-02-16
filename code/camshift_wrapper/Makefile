##
##  Makefile for camshift_wrapper (as part of SwisTrack)
##  T. Lochmatter 2009-02-14
##

# Configuration
TARGET        = camshift_wrapper.a

# Toolchain (compiler, linker)
SHELL      = /bin/sh
CTAGS      = ctags
#CC         = cc
#AR         = ar

# Files
FILES_C   := $(wildcard *.c)
FILES_H   := $(wildcard *.h)
FILES_O   := $(FILES_C:.c=.o)
FILES_D   := $(FILES_C:.c=.d)

# Includes and libs
INCLUDE    = -I/usr/include -I/usr/include/opencv -I/opt/local/include/opencv -I/opt/local/include
LIB        = -lm -lcv -lcxcore -lhighgui

# Flags
CFLAGS    := -g -Wall
CPPFLAGS  := -DUNIX $(INCLUDE)

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
tags: $(FILES_C) $(FILES_H)
	$(CTAGS) $(FILES_C) $(FILES_H)

# Dependency file creation
$(FILES_D): %.d: %.c
	$(CC) -MM $(CPPFLAGS) $< > $@;

# Dependency file inclusion
ifneq ($(MAKECMDGOALS),clean)
include $(FILES_D)
endif
