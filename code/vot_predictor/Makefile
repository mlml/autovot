

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
CXXFLAGS = -Wall -I$(INFRA_PATH) -I$(LEARNING_PATH) -I../../audiofile-0.3.4/libaudiofile -I..
LDLIBS = -L$(INFRA_PATH) -L$(LEARNING_PATH)/cmdline #-L../../audiofile-0.3.4/libaudiofile/.libs

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
all: VotFrontEnd VotFrontEnd2 InitialVotTrain InitialVotDecode
VotFrontEnd: VotFrontEnd.o Dataset.o infra_dsp.o FFTReal/FFTReal.cpp get_f0s.o sigproc.o ../../audiofile-0.3.4/libaudiofile/.libs/libaudiofile.a 
VotFrontEnd2: VotFrontEnd2.o Dataset.o infra_dsp.o FFTReal/FFTReal.cpp get_f0s.o sigproc.o ../../audiofile-0.3.4/libaudiofile/.libs/libaudiofile.a
InitialVotTrain: InitialVotTrain.o Classifier.o Dataset.o KernelExpansion.o
InitialVotDecode: InitialVotDecode.o Classifier.o Dataset.o KernelExpansion.o

#----- Begin Boilerplate
endif
