##########################################################################
# Makefile for regress
##########################################################################
# Author: Joseph Keshet jkeshet@gmail.com 
##########################################################################

# headers:
HDRS    = infra.h \
	  infra_vector.h \
	  infra_matrix.h \
	  infra_vv_funcs.h \
	  infra_vm_funcs.h \
	  infra_mm_funcs.h \
	  infra_binary.h \
	  infra_exception.h \
	  infra_refcount_darray.h

# sources:
SRCS    = infra_vv_funcs.cpp \
	  infra_vm_funcs.cpp \
	  infra_mm_funcs.cpp \
	  infra_exception.cpp \
	  infra_binary.cpp 


# inline sources:
INLN	= infra_matrix.imp \
	  infra_vector.imp \
	  infra_refcount_darray.imp

CC      = g++
CFLAGS =-O3 -DNDEBUG
CFLAGS_DEBUG  =-g
LFLAGS = -O3 -L.
ifeq ($(ATLAS),yes)
	CFLAGS += -D_USE_ATLAS_
  	LFLAGS += -lcblas -latlas
endif
	
OBJS    = $(SRCS:.cpp=.o)
OBJS_DEBUG  = $(SRCS:.cpp=.od)

all: libinfra.a libinfra_debug.a tester tester_debug tester_endian

tester_endian:  $(OBJS_DEBUG) tester_endian.od
	$(CC) $(OBJS_DEBUG) tester_endian.od $(LFLAGS_DEBUG) -o tester_endian

tester:  $(OBJS) tester.o
	$(CC) $(OBJS) tester.o $(LFLAGS) -o tester

tester_debug:  $(OBJS_DEBUG) tester.od
	$(CC) $(OBJS_DEBUG) tester.od $(LFLAGS_DEBUG) -o tester_debug

%.o: %.cpp $(INLN) $(HDRS)
	$(CC) $(CFLAGS) -c $<

%.od: %.cpp $(INLN) $(HDRS)
	$(CC) $(CFLAGS_DEBUG) -c $< -o $@


libinfra.a: $(OBJS)
	ar rv $@ $?
	ranlib $@

libinfra_debug.a: $(OBJS_DEBUG)
	ar rv $@ $?
	ranlib $@


clean:
	rm -f *.o *.od libinfra.a libinfra_debug.a tester tester_debug tester_endian

doc_html:    $(HDRS)
	mkdir html; kdoc -f html -d html -n Infra $(HDRS)

doc_pdf:    $(HDRS)
	mkdir pdf; kdoc -f latex -d pdf -n Infra $(HDRS); cd pdf; \
	mv main.tex main.bak; \
	perl -i -pe s\|_\|\\\\_\|g *.tex; \
	mv main.bak Infra.tex; \
	latex '\scrollmode\input Infra.tex'; \
	latex '\scrollmode\input Infra.tex'; \
	dvips -D 600 -P cmz -t letter Infra.dvi -o Infra.ps; \
	ps2pdf Infra.ps;
	cd .. 

doc_man:    $(HDRS)
	mkdir man; kdoc -f man -d man -n Infra $(HDRS) 

tar:	$(HDRS) $(SRCS) $(INLN)
	tar zcvf infra.tgz $(HDRS) $(SRCS) $(INLN) Makefile tester.cpp \
	LICENSE.txt
