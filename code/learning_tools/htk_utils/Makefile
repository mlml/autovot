

ifeq (,$(filter _%,$(notdir $(CURDIR))))
  include target.mk
else
#----- End Boilerplate

# This section is for the architecture-specific target directory
VPATH =  $(SRCDIR)

# Additional headers and libraries directories
INFRA_PATH = ../../../learning_tools/infra2
LEARNING_PATH = ../../../learning_tools

CC = g++
CXXFLAGS = -Wall -I$(INFRA_PATH) -I$(LEARNING_PATH)
LDLIBS = -L$(INFRA_PATH) -L$(LEARNING_PATH)/cmdline

# Check if the configuration is Release or Debug
ifeq ($(CONFIGURATION),Debug)
	CXXFLAGS += -g
	LDLIBS += -g -linfra_debug -lcmdline
else
	CXXFLAGS += -O3 -DNDEBUG
	LDLIBS += -O3 -linfra -lcmdline
	CONFIGURATION = Release
endif

ifeq ($(ATLAS),yes)
	CXXFLAGS += -D_USE_ATLAS_
  	LDLIBS += -lcblas -latlas
endif


# Targets
PROGRAMS = htk_shuffle htk_stats htk2db htk2txt txt2htk htk_ceps_dist htk_split htkcat htk_contract htk_split_from_phn

all: $(PROGRAMS)

htk_shuffle: htk_shuffle.o HtkFile.o
htk_stats: htk_stats.o HtkFile.o
htk_split: htk_split.o HtkFile.o
htk2db: htk2db.o HtkFile.o
htk2txt: htk2txt.o HtkFile.o
txt2htk: txt2htk.o HtkFile.o
htk_ceps_dist: htk_ceps_dist.o HtkFile.o
htkcat: htkcat.o HtkFile.o
htk_contract: htk_contract.o HtkFile.o
htk_split_from_phn: htk_split_from_phn.o HtkFile.o

install: all
	if test "$(BINDIR)" = "" ; then \
		echo "BINDIR not set. Cannot install executables"; \
		exit 1; \
		fi
	for prog in $(PROGRAMS); do \
		install $(PWD)/_$(_ARCH)_$(CONFIGURATION)/$$prog $(BINDIR); \
		done

#----- Begin Boilerplate
endif
