

ifeq (,$(filter _%,$(notdir $(CURDIR))))
  include target.mk
else
#----- End Boilerplate

# This section is for the architecture-specific target directory
VPATH =  $(SRCDIR)

# Additional headers and libraries directories
INFRA_PATH = ../../infra2.1
LEARNING_PATH = ../..

CC = g++
CXXFLAGS = -Wall -I$(INFRA_PATH) -I$(LEARNING_PATH)/cmdline
LDLIBS = -L$(INFRA_PATH) -L$(LEARNING_PATH)/cmdline

# on IDIAP machine the atlas library defined by a shell variable
ifdef ATLAS_HOME
	LDLIBS  += -L$(ATLAS_HOME)/lib
endif

# Check if the configuration is Release or Debug
ifeq ($(CONFIGURATION),Debug)
	CXXFLAGS += -g
	LDLIBS += -g -linfra_debug -lcmdline
else
	CXXFLAGS += -O3 -DNDEBUG
	LDLIBS += -O3 -linfra -lcblas -latlas -lcmdline
endif

# If the machine is PowerPC swap endians (shell variable)
ifeq ($(shell uname -p),powerpc) 
		CXXFLAGS += -D_ENDIAN_SWAP_
endif

# Targets
all: db2txt txt2db infra2txt db_append db_average_norm_l2 \
	db_extract db_normalize db_remove_labels db_shuffle db_zero_size infra_split

db2txt:  db2txt.o
txt2db:  txt2db.o
infra2txt:  infra2txt.o
db_append:  db_append.o
db_average_norm_l2:  db_average_norm_l2.o
db_extract:  db_extract.o
db_normalize:  db_normalize.o
db_remove_labels:  db_remove_labels.o
db_shuffle:  db_shuffle.o
db_zero_size:  db_zero_size.o
infra_split: infra_split.o

#----- Begin Boilerplate
endif
