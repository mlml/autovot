##########################################################################
# Makefile for active_set
##########################################################################
# Author: Shai Shalev-Shwartz
##########################################################################

# headers:
HDRS    = active_set.h 

#inlines
INLN = active_set.imp

# sources:
SRCS    = tester.cc

CC      = g++
CFLAGS_DEBUG  = -Wall -g
CFLAGS  = -Wall -O3 -DNDEBUG -D_USE_ATLAS_
LFLAGS=-lcblas -latlas  

OBJS    = $(SRCS:.cc=.o)
OBJS_DEBUG    = $(SRCS:.cc=.od)

all: tester 

tester:  $(OBJS) 
	$(CC) $(OBJS) $(LFLAGS) -o tester

tester_debug:  $(OBJS_DEBUG) 
	$(CC) $(OBJS_DEBUG) $(LFLAGS_DEBUG) -o tester_debug

%.o: %.cc $(INLN) $(HDRS)
	$(CC) $(CFLAGS) -c $<

%.od: %.cc $(INLN) $(HDRS)
	$(CC) $(CFLAGS_DEBUG) -c $< -o $@


clean:
	rm -f *.o *~ *.od 

doc_html:    $(HDRS)
	rm -fr html; mkdir html; kdoc -f html -d html -n active_set $(HDRS)

tar:	$(HDRS) $(SRCS) $(INLN)
	tar zcvf active_set.tgz $(HDRS) $(SRCS) $(INLN) Makefile tester.cc \
	LICENSE.txt
