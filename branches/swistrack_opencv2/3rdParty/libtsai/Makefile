##
##  Makefile for libtsai (as part of SwisTrack)
##

SWISTRACK_ROOT := ../..
include $(SWISTRACK_ROOT)/Makefile.inc

# Configuration
TARGET    := libtsai.a

# Files
FILES_C   := $(wildcard tsaisrc/*.c) $(wildcard mathsrc/*.c) $(wildcard mathsrc/minpack/*.c) $(wildcard include/*.c)
FILES_H   := $(wildcard tsaisrc/*.h) $(wildcard mathsrc/*.h) $(wildcard mathsrc/minpack/*.h) $(wildcard include/*.h)
FILES_O   := $(FILES_C:.c=.o)

# Flags
override CFLAGS    += -g -Wall
override CPPFLAGS  += -I/usr/include -Itsaisrc -Imathsrc -Imathsrc/minpack -Iinclude

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
