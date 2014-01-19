# This is a sample target.mk file, used to jump from a source directory to a
# target directory.  This version allows for both an optional _common and an
# architecture specific target directory.
#

.SUFFIXES:


# Find the target directory(ies).
#
ifndef _ARCH
  _ARCH := $(shell uname -s)
  export _ARCH
endif

ifndef CONFIGURATION
  CONFIGURATION = Release
  export _ARCH
endif

OBJDIR := _$(_ARCH)_$(CONFIGURATION)

EXTRATARGETS := $(wildcard _common)

# Define the rules to build in the target subdirectories.
#
MAKETARGET = $(MAKE) --no-print-directory -C $@ -f $(CURDIR)/Makefile \
		SRCDIR=$(CURDIR) $(MAKECMDGOALS)

.PHONY: $(OBJDIR) $(EXTRATARGETS)
$(OBJDIR) $(EXTRATARGETS):
	+@[ -d $@ ] || mkdir -p $@
	+@$(MAKETARGET)

$(OBJDIR) : $(EXTRATARGETS)


# These rules keep make from trying to use the match-anything rule below to
# rebuild the makefiles--ouch!  Obviously, if you don't follow my convention
# of using a `.mk' suffix on all non-standard makefiles you'll need to change
# the pattern rule.
#
Makefile : ;
%.mk :: ;


# Anything we don't know how to build will use this rule.  The command is a
# do-nothing command, but the prerequisites ensure that the appropriate
# recursive invocations of make will occur.
#
% :: $(EXTRATARGETS) $(OBJDIR) ;


# The clean rule is best handled from the source directory: since we're
# rigorous about keeping the target directories containing only target files
# and the source directory containing only source files, `clean' is as trivial
# as removing the target directories!
#
.PHONY: clean
clean:
	$(if $(EXTRATARGETS),rm -f $(EXTRATARGETS)/*)
	rm -rf $(OBJDIR)
