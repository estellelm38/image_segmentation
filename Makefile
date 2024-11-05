#######################################
# Makefile PBM                        #
#                                     #
# E.B.                                #
#######################################

C = gcc

CFLAGS = -Wall -I.

LDFLAGS = -lm

SOURCES = main.c Util.c

OBJECTS = main.o Util.o

EXEC = output

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

# Rule to build main.o
main.o: main.c Util.h
	$(CC) $(CFLAGS) -c main.c

# Rule to build imageFormationUtils.o
Util.o: Util.c Util.h
	$(CC) $(CFLAGS) -c Util.c

# Clean rule to remove build files
clean:
	rm -f $(OBJECTS) $(EXEC)
	rm -f *.pgm
#include "imageFormationUtils.h"

