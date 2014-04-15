
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
	@echo "[make] Compiling all."
	@for subdir in ${SUBDIRS} ; do ( cd $$subdir ; ${MAKE} ATLAS=$(USE_ATLAS) CONFIGURATION=$(_CONFIGURATION) BINDIR=$(BINDIR) $@) || exit 10 ; done
	@mkdir -p ../bin
	@cp vot_predictor/_$(_ARCH)_$(_CONFIGURATION)/VotFrontEnd2 ../bin
	@cp vot_predictor/_$(_ARCH)_$(_CONFIGURATION)/VotTrain ../bin
	@cp vot_predictor/_$(_ARCH)_$(_CONFIGURATION)/VotDecode ../bin
	@echo "[make] Compiling completed."
	
install:
	@echo "[make] Not implemented yet."

clean:
	@echo "[make] Cleaning all."
	@for subdir in ${SUBDIRS} ; do ( cd $$subdir ; ${MAKE} clean $@ ) || exit 10 ; done
	rm -fr ../bin/VotFrontEnd2
	rm -fr ../bin/VotTrain
	rm -fr ../bin/VotDecode
	@echo "[make] Cleaning completed."

