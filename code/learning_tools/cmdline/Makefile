
# headers
HDRS=cmd_option.h cmd_line.h

# sources
SRCS=cmd_line.cpp tester.cpp

CC = g++
CFLAGS  = -Wall -O3 
LFLAGS  = -lcmdline

%.o: %.cpp $(HDRS)
	$(CC) $(CFLAGS) -c $<

# all: tester libcmdline.a libcmdline_mpi.a
all: tester libcmdline.a

tester: tester.o libcmdline.a 

libcmdline.a: cmd_line.o
	ar rv $@ $?
	ranlib $@

clean:
	rm -f *.o tester libcmdline.a 

doc:    $(HDRS)
	mkdir html; kdoc -f html -d html -n cmd_line $(HDRS)
