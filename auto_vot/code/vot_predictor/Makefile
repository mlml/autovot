

ifeq (,$(filter _%,$(notdir $(CURDIR))))
  include target.mk
else
#----- End Boilerplate

# This section is for the architecture-specific target directory
VPATH =  $(SRCDIR)

# Additional headers and libraries directories
INFRA_PATH = ../../learning_tools/infra2 
LEARNING_PATH = ../../learning_tools

CC = g++
CXXFLAGS = -Wall -I$(INFRA_PATH) -I$(LEARNING_PATH) -I..
LDLIBS = -L$(INFRA_PATH) -L$(LEARNING_PATH)/cmdline 

# Check if the configuration is Release or Debug
ifeq ($(CONFIGURATION),Debug)
	CXXFLAGS += -g
	LDLIBS += -g -linfra_debug -lcmdline #-laudiofile
else
	CXXFLAGS += -O3 -DNDEBUG
	LDLIBS += -O3 -linfra -lcmdline #-laudiofile
	CONFIGURATION = Release
endif

ifeq ($(ATLAS),yes)
	CXXFLAGS += -D_USE_ATLAS_
  	LDLIBS += -lcblas 
endif


# Targets
all:  VotFrontEnd2 VotTrain VotDecode
VotFrontEnd2: VotFrontEnd2.o Dataset.o infra_dsp.o FFTReal/FFTReal.cpp get_f0s.o sigproc.o WavFile.o
VotTrain: VotTrain.o Classifier.o Dataset.o KernelExpansion.o
VotDecode: VotDecode.o Classifier.o Dataset.o KernelExpansion.o

#----- Begin Boilerplate
endif
