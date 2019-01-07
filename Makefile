# Folders
SRCDIR=src
TARGETDIR=bin
BUILDDIR=$(TARGETDIR)/obj
RESDIR=res
VENDORINCLUDE=$(SRCDIR)/vendor

# Standard
CXX=g++
CXXFLAGS:=-g -std=c++11 -Wall -O0 -I$(VENDORINCLUDE) -Iinclude `wx-config --cxxflags` -c
LD=g++
LIBS=`wx-config --libs`
LDFLAGS:=$(LIBS)

# User defined
TARGET=software_renderer.out
SOURCES=$(shell find $(SRCDIR) -type f -name *.cpp)
OBJECTS=$(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.cpp=.o))

# Commands
MKDIR_P=mkdir -p

all: resources $(TARGETDIR)/$(TARGET)
	@echo Done!

#Copy Resources from Resources Directory to Target Directory
resources: directories
	@echo Copting Resources to Target Directory
	@cp -r $(RESDIR)/* $(TARGETDIR)/

#Make the Directories
directories:
	@echo Creating Directories
	@$(MKDIR_P) $(TARGETDIR)
	@$(MKDIR_P) $(BUILDDIR)

$(TARGETDIR)/$(TARGET): $(BUILDDIR)/pch.h.gch $(OBJECTS)
	@echo Linking
	@$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@echo "Compiling: $(CXX): $< -> $@"
	@$(CXX) $< $(CXXFLAGS) -include $(SRCDIR)/pch.h -o $@

$(BUILDDIR)/pch.h.gch: $(SRCDIR)/pch.h
	@echo Precompiled header
	@$(CXX) $< $(CXXFLAGS) -o $@

# Clean objects and executables
cleanall: clean
	@echo Cleaning Executable Files
	@$(RM) -r $(TARGETDIR)

#Clean only Objects
clean:
	@echo Cleaning Intermediate Files
	@$(RM) -r $(BUILDDIR)

#Non-File Targets
.PHONY: all clean cleanall
