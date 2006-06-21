# Makefile
# ==============================================================================
# 07.02.2002 cmc :  Created based on sample from Kevin Nickels.
# 07.15.2004 cmc :  Updated and applied to sn_sim project.
# 08.05.2004 cmc :  Added tags to removed files in distclean, added "doc" target
# 12.16.2004 cmc :  Remove output files with distclean.
# ------------------------------------------------------------------------------
#  TODO: Figure out why LD won't link C++ object files with standard args.
# ==============================================================================

PROG   = blob
OUTPUT = $(wildcard $(PROG)*.m)

# ==============================================================================
# Local variables
# ------------------------------------------------------------------------------
#  first, find all the pertinent filenames
CSRCS = $(wildcard *.c)
CXXSRCS = $(wildcard *.cc) $(wildcard *.cpp) $(wildcard *.C)
SRCS = $(CSRCS) $(CXXSRCS)
HDRS = $(wildcard *.h) $(wildcard *.hh) $(wildcard *.hpp)
OBJS = $(addsuffix .o,$(basename $(SRCS)))
DEPS = $(addsuffix .d,$(basename $(SRCS)))

CTAGS    = ctags
CC       = cc
CXX      = c++
CFLAGS   = -g -Wall
CXXFLAGS = -g -Wall
CPPFLAGS = -I/usr/include 

LDFLAGS  = -g # -L/usr/lib32
LDLIBS   = -lm -lstdc++ #-largtable2

ifeq ($(MAKECMDGOALS),debug)
CXXFLAGS += -DDEBUG -DVERBOSITY=0
endif


# ==============================================================================
# Dependencies & rules
# ------------------------------------------------------------------------------
all: lib solib tags #$(PROG) doc

.PHONY: debug
debug: all

lib: $(OBJS) $(HDRS)
	ar rcs lib$(PROG).a $(OBJS)

solib: $(OBJS) $(HDRS)
	$(CC) $(CFLAGS) -shared -fPIC $(OBJS) -o lib$(PROG).so

$(PROG): $(OBJS)
	$(CXX) $(LDFLAGS) $(LDLIBS) $(OBJS) -o $(PROG)
#	$(LD) $(LDFLAGS) $(LDLIBS) $(OBJS) -o $(PROG)
# LD is behaving wierdly; something wrong with c/c++ name mangling...

# ==============================================================================
# Clean up directory
# ------------------------------------------------------------------------------
.PHONY:	clean
clean:	
	- $(RM) $(OBJS) *~ core.* *.rpo

.PHONY: distclean
distclean: clean
	- $(RM) $(DEPS) tags *.a *.so $(OUTPUT)
# ==============================================================================


# ==============================================================================
# generate doxygen documentation
# ------------------------------------------------------------------------------
doc:	$(SRCS) $(HDRS)
	doxygen Doxyfile
# ==============================================================================


# ==============================================================================
# make tags files for vim
# ------------------------------------------------------------------------------
tags:	$(SRCS) $(HDRS)
	$(CTAGS) $(SRCS) $(HDRS)
# ==============================================================================


# ==============================================================================
# a default rule for building dependency files
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


# ==============================================================================
# include the source code dependencies
# ------------------------------------------------------------------------------
ifneq ($(MAKECMDGOALS),clean)
include $(DEPS)
endif
# ==============================================================================


# ==============================================================================
# end of Makefile
