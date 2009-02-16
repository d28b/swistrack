##
##  Makefile for libtsai (as part of SwisTrack)
##  T. Lochmatter 2009-02-16
##

# Configuration
TARGET        = libtsai.a

# Toolchain (compiler, linker)
SHELL      = /bin/sh
CTAGS      = ctags
#CC         = cc
#AR         = ar

# Files
FILES_C   := $(wildcard tsaisrc/*.c) $(wildcard mathsrc/*.c) $(wildcard mathsrc/minpack/*.c) $(wildcard include/*.c)
FILES_H   := $(wildcard tsaisrc/*.h) $(wildcard mathsrc/*.h) $(wildcard mathsrc/minpack/*.h) $(wildcard include/*.h)
FILES_O   := $(FILES_C:.c=.o)

# Flags
INCLUDE    = -I/usr/include -Itsaisrc -Imathsrc -Imathsrc/minpack -Iinclude
CFLAGS    := -g -Wall
CPPFLAGS  := -DUNIX $(INCLUDE)

# Build
.PHONY: all
all: $(TARGET)

$(TARGET): $(FILES_O)
	@echo "=== Building $@"
	$(AR) -r $@ $(FILES_O)

$(FILES_O): %.o: %.c $(FILES_H)
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

# Cleanup
.PHONY: clean
clean:
	- rm $(FILES_O) $(TARGET)

# Tags
.PHONY: tags
tags: $(FILES_C) $(FILES_H)
	$(CTAGS) $(FILES_C) $(FILES_H)
