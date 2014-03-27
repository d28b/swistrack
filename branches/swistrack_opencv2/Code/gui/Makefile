##
##  Makefile for gui (as part of SwisTrack)
##

SWISTRACK_ROOT := ../..
include $(SWISTRACK_ROOT)/Makefile.inc

# Configuration
TARGET    := $(TARGET_FOLDER)/SwisTrack

# Files
FILES_CPP := $(wildcard *.cpp)
FILES_H   := $(wildcard *.h)
FILES_O   := $(FILES_CPP:.cpp=.o)
FILES_D   := $(FILES_CPP:.cpp=.d)
FILES_A   := ../core/libSwisTrackCore.a ../../3rdParty/cvblobslib_v6p1/libCVBlobs.a ../../3rdParty/libtsai/libtsai.a ../../3rdParty/camshift_wrapper/libCamshiftWrapper.a 

# Flags
override CXXFLAGS  += -g -Wall $(CXXFLAGS_ADD)
override CPPFLAGS  += $(OPENCV_CPPFLAGS) $(WXWIDGETS_CPPFLAGS) -I/usr/include -I../core $(CPPFLAGS_ADD)
override LDLIBS    += $(OPENCV_LDLIBS) $(WXWIDGETS_LDLIBS) -lm $(LDLIBS_ADD)
override LDFLAGS   += $(LDFLAGS_ADD)

# Build
.PHONY: all
all: $(TARGET)

$(TARGET): $(FILES_O) $(FILES_A)
	@echo "=== Building $@"
	$(CXX) $(LDFLAGS) $(FILES_O) $(FILES_A) $(LDLIBS) -o $@
#	$(CXX) $(LDFLAGS) $(FILES_O) $(FILES_A)/usr/lib/libefence.a  $(LDLIBS) -o $@

$(FILES_O): %.o: %.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

# Build with cygwin
.PHONY: cygwin
cygwin: $(FILES_O) $(FILES_A)
	@echo "=== Building SwisTrack (cygwin)"
	windres MSVisualStudio/SwisTrack-GUI.rc -O coff -o cygwinRC.o
	$(CXX) $(LDFLAGS) $(FILES_O) $(FILES_A) cygwinRC.o  $(LDLIBS) -o $(TARGET)

# Build on Mac OS X
.PHONY: osxbundle
osxbundle: $(TARGET)
	cd $(TARGET_FOLDER) && tar xfz ../Code/AppleXCode/SwisTrack.app.skeleton.tar.gz
	cp $(TARGET) $(TARGET_FOLDER)/SwisTrack.app/Contents/MacOS/

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
