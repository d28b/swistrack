##
##  Makefile for SwisTrack
## 	C. Cianci 2007-09-27
##
##  $Date$
##  $Revision$
##  $Author$
##  $HeadURL$
##


TARGET      = SwisTrack
PROJROOT    = ..
SUBDIRS     = $(shell find $(PROJROOT) -type d | fgrep -v .svn | fgrep -v /ui \
			  | fgrep -v /doc | fgrep -v /Make | fgrep -v /MSVisualStudio \
			  | fgrep -v /test )
OUTPUTDIR   = ../../SwisTrackEnvironment

# ==============================================================================
# Local variables
# ------------------------------------------------------------------------------
CSRCS   = $(wildcard $(patsubst %,%/*.c,  $(SUBDIRS)))
CXXSRCS = $(wildcard $(patsubst %,%/*.cpp,$(SUBDIRS)))
SRCS = $(CSRCS) $(CXXSRCS)
OBJS = $(addsuffix .o,$(basename $(SRCS)))
DEPS = $(addsuffix .d,$(basename $(SRCS)))

INCL_DIRS  = /usr/include /usr/local/include/opencv $(SUBDIRS)
INCL_CMD   = $(patsubst %,-I%,$(INCL_DIRS))
LD_LIBS    = m stdc++ cv cxcore highgui
LD_CMD     = $(patsubst %,-l%,$(LD_LIBS))

CTAGS    = ctags
CC       = cc
CXX      = c++
CFLAGS   = -g -Wall
CXXFLAGS = -g -Wall
CPPFLAGS = $(INCL_CMD) `wx-config --cxxflags`
LDFLAGS  = -g $(LD_CMD) `wx-config --libs`

ifeq ($(MAKECMDGOALS),debug)
CXXFLAGS += -DDEBUG -DVERBOSITY=0
endif

# ==============================================================================
# Dependencies & rules
# ------------------------------------------------------------------------------
.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $(LDLIBS) $(OBJS) -o $(OUTPUTDIR)/$(TARGET)

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
# make tags files for vim
# ------------------------------------------------------------------------------
tags: $(SRCS) $(HDRS)
	$(CTAGS) $(SRCS) $(HDRS)

# ==============================================================================
# default rules for building dependency files
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
# include the source code dependencies
# ------------------------------------------------------------------------------
ifneq ($(MAKECMDGOALS),clean)
include $(DEPS)
endif

# ==============================================================================
# end of Makefile
