
CURDIR := $(shell basename `pwd` )
SUBDIRS := learning_tools vot_predictor
# USE_ATLAS = yes 
USE_ATLAS = no
export USE_ATLAS

ifndef _ARCH
  _ARCH := $(shell uname -s)
  export _ARCH
endif
ifndef _CONFIGURATION
  _CONFIGURATION = Release
  export _CONFIGURATION
endif
OBJDIR := $(CURDIR)/_$(_ARCH)_$(_CONFIGURATION)
export OBJDIR
BINDIR := $(CURDIR)/bin

all:
	@echo ">>> Compiling all <<<"
	@for subdir in ${SUBDIRS} ; do ( cd $$subdir ; ${MAKE} ATLAS=$(USE_ATLAS) CONFIGURATION=$(_CONFIGURATION) BINDIR=$(BINDIR) $@) || exit 10 ; done
	@mkdir -p ../bin
	@cp vot_predictor/_$(_ARCH)_$(_CONFIGURATION)/VotFrontEnd ../bin
	@cp vot_predictor/_$(_ARCH)_$(_CONFIGURATION)/VotFrontEnd2 ../bin
	@cp vot_predictor/_$(_ARCH)_$(_CONFIGURATION)/InitialVotTrain ../bin
	@cp vot_predictor/_$(_ARCH)_$(_CONFIGURATION)/InitialVotDecode ../bin
	@echo ">> !!! Ok !!! <<<"
	
install:
	@echo ">>> Compiling all <<<"
	@for subdir in ${SUBDIRS} ; do ( cd $$subdir ; ${MAKE} ATLAS=$(USE_ATLAS) CONFIGURATION=$(_CONFIGURATION) BINDIR=$(BINDIR) $@) || exit 11 ; done
	@echo ">> !!! Ok !!! <<<"

clean:
	@echo ">>> Cleaning all <<<"
	@for subdir in ${SUBDIRS} ; do ( cd $$subdir ; ${MAKE} clean $@ ) || exit 10 ; done
	rm -fr ../bin/VotFrontEnd
	rm -fr ../bin/VotFrontEnd2
	rm -fr ../bin/InitialVotTrain
	rm -fr ../bin/InitialVotDecode
	@echo ">> !!! Ok !!! <<<"

