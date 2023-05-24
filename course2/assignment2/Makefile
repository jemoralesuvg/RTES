INCLUDE_DIRS = 
LIB_DIRS = 
CC=gcc

CDEFS=
CFLAGS= -O0 -g $(INCLUDE_DIRS) $(CDEFS)
LIBS= 

HFILES= seqgen.h
CFILES= seqgen3.c

SRCS= ${HFILES} ${CFILES}
OBJS= ${CFILES:.c=.o}

all:	seqgen3

clean:
	-rm -f *.o *.d frames/*.pgm frames/*.ppm
	-rm -f seqgen3 

seqgen3: seqgen3.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $@.o -lpthread -lrt


depend:

.c.o:
	$(CC) $(CFLAGS) -c $<
