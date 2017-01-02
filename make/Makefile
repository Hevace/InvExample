SHELL = /bin/sh
.SUFFIXES:
.SUFFIXES: .c .cpp .o

MKFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
MKFILE_DIR := $(dir $(MKFILE_PATH))

########################################
# Inputs
########################################
SRCDIR := $(MKFILE_DIR)../src
INCLUDEDIR := $(MKFILE_DIR)../src
SOURCES := hellotest.cpp

########################################
# Intermediate directories
########################################
OBJDIR := $(MKFILE_DIR)obj
OBJECTS := $(addprefix $(OBJDIR)/, $(notdir $(SOURCES:.cpp=.o)))

########################################
# Output directories
########################################
BUILDDIR := $(MKFILE_DIR)../build

VPATH := $(SRCDIR) $(INCLUDEDIR) $(OBJDIR) $(BUILDDIR)

########################################
# Compiler options
########################################
CDEBUG := -g
CXXFLAGS := -I$(INCLUDEDIR) -std=c++11 -O2 -Wall $(CDEBUG)
LDFLAGS := -g

########################################
# Automatic generation of dependency files
########################################
$(OBJDIR)/%.d:%.cpp
	@echo 'Dependencies $(<F) ==> $(@F)'
	@rm -f $@; \
	$(CXX) -MM $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

########################################
# Compilation rules
########################################
$(OBJDIR)/%.o:%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c $^

########################################
# Build rules
########################################
.PHONY: all clean
all: $(BUILDDIR)/InvExample

clean:
	rm -rf $(OBJDIR)
	rm -rf $(BUILDDIR)

$(BUILDDIR)/InvExample: $(OBJECTS) | $(BUILDDIR)
	$(CXX) $(LDFLAGS) -o $@ $^

# Create the object dir before creating object files or dependency files
$(OBJECTS) $(OBJECTS:.o=.d): | $(OBJDIR)

# Create the object dir if it doesn't exist
$(OBJDIR):
	mkdir $(OBJDIR)

# Create the build dir if it doesn't exist
$(BUILDDIR):
	mkdir $(BUILDDIR)

# include automatic header dependencies
include $(OBJECTS:.o=.d)

# print variable value for makefile debugging
print-%:
	@echo '$*=$($*)'

