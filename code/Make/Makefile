##
##  Makefile for SwisTrack
## 	C. Cianci 2007-09-27
##      T. Lochmatter 2008-03-25
##

TARGET     = SwisTrack
PROJROOT   = ..
SUBDIRS    = $(PROJROOT)/core \
	$(PROJROOT)/libtsai \
	$(PROJROOT)/libtsai/tsaisrc \
	$(PROJROOT)/libtsai/mathsrc \
	$(PROJROOT)/libtsai/mathsrc/minpack \
	$(PROJROOT)/libtsai/include \
	$(PROJROOT)/gui
OUTPUTDIR  = ../../SwisTrackEnvironment

# Comment this out if you want to compile without XVID support
XVID_PREFIX = /usr/local

# ==============================================================================
# Local variables
# ------------------------------------------------------------------------------
CSRCS      = $(wildcard $(patsubst %,%/*.c,   $(SUBDIRS)))
CXXSRCS    = $(wildcard $(patsubst %,%/*.cpp, $(SUBDIRS)))
SRCS       = $(CSRCS) $(CXXSRCS)
OBJS       = $(addsuffix .o, $(basename $(SRCS)))
DEPS       = $(addsuffix .d, $(basename $(SRCS)))

INCL_DIRS  = /usr/include /usr/include/opencv /opt/local/include/opencv $(SUBDIRS)
LD_LIBS    = m stdc++ cv cxcore highgui

ifneq ($(XVID_PREFIX),)
INCL_DIRS += $(XVID_PREFIX)/include
#LD_LIBS   += xvidcore

endif

CTAGS      = ctags
CC         = cc
CXX        = c++
CFLAGS     = -g -Wall
CXXFLAGS   = -g -Wall
INCL_CMD   = $(patsubst %,-I%,$(INCL_DIRS))
CPPFLAGS   = $(INCL_CMD) `wx-config --cxxflags`
LD_CMD     = $(patsubst %,-l%,$(LD_LIBS))
LDFLAGS    = -g $(LD_CMD) `wx-config --libs` -L/opt/local/lib

ifeq ($(MAKECMDGOALS),debug)
CXXFLAGS  += -DDEBUG -DVERBOSITY=0
endif

# ==============================================================================
# Dependencies & rules
# ------------------------------------------------------------------------------
.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) $(LDLIBS) -o $(OUTPUTDIR)/$(TARGET)

.PHONY: debug
debug: all

.PHONY: echo
echo: 
	@echo SUBDIRS = $(SUBDIRS)
	@echo SRCS    = $(SRCS)
	@echo DEPS    = $(DEPS)
	@echo OBJS    = $(OBJS)

lib: $(OBJS) $(HDRS)
	ar rcs lib$(PROG).a $(OBJS)

solib: $(OBJS) $(HDRS)
	$(CC) $(CFLAGS) -shared -fPIC $(OBJS) -o lib$(PROG).so

$(PROG): $(OBJS)
	$(CXX) $(LDFLAGS) $(LDLIBS) $(OBJS) -o $(PROG)

# ==============================================================================
# Clean up directory
# ------------------------------------------------------------------------------
.PHONY: clean
clean:
	- $(RM) $(OBJS) $(shell find $(PROJROOT) -name \*~)

.PHONY: distclean
distclean: clean
	- $(RM) $(DEPS) $(OUTPUTDIR)/$(TARGET)

# ==============================================================================
# Make tags files for vim
# ------------------------------------------------------------------------------
tags: $(SRCS) $(HDRS)
	$(CTAGS) $(SRCS) $(HDRS)

# ==============================================================================
# Default rules for building dependency files
# ------------------------------------------------------------------------------
%.d: %.c
	@ $(SHELL) -ec '$(CXX) -MM $(CPPFLAGS) $< > $@'
%.d: %.cc
	@ $(SHELL) -ec '$(CXX) -MM $(CPPFLAGS) $< > $@'
%.d: %.C
	@ $(SHELL) -ec '$(CXX) -MM $(CPPFLAGS) $< > $@'
%.d: %.cpp
	@ $(SHELL) -ec '$(CXX) -MM $(CPPFLAGS) $< > $@'

# ==============================================================================
# Include the source code dependencies
# ------------------------------------------------------------------------------
ifneq ($(MAKECMDGOALS),clean)
include $(DEPS)
endif

# ==============================================================================
# End of Makefile
