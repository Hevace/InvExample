SHELL = /bin/sh
.SUFFIXES:
.SUFFIXES: .c .cpp .o

mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir := $(dir $(mkfile_path))

########################################
# Inputs
########################################
SRCDIR := ../src
INCLUDEDIR := ../src
SOURCES := hellotest.cpp

########################################
# Intermediate directories
########################################
OBJDIR := ./obj
OBJECTS := $(addprefix $(OBJDIR)/, $(notdir $(SOURCES:.cpp=.o)))

########################################
# Output directories
########################################
BUILDDIR := ../build

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
	rm -f $@; \
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
	rm -r $(OBJDIR)
	rm -r $(BUILDDIR)

$(BUILDDIR)/InvExample: $(OBJECTS) | $(BUILDDIR)
	$(CXX) $(LDFLAGS) -o $@ $^

$(OBJECTS) $(OBJECTS:.o=.d): | $(OBJDIR)        # Create the object dir before creating object files or dependency files

$(OBJDIR):
	mkdir $(OBJDIR)            # Create the object dir if it doesn't exist

$(BUILDDIR):
	mkdir $(BUILDDIR)          # Create the build dir if it doesn't exist

include $(OBJECTS:.o=.d)    # include automatic header dependencies

# print variable value for makefile debugging
print-%:
	@echo '$*=$($*)'

