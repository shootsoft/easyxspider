TOP_SRCDIR = ../..
include ${TOP_SRCDIR}/makefile.inc
SOURCES = $(shell ls *.cpp)

OBJECTS = $(SOURCES:.cpp=.o)

all: ${OBJECTS}

clean:
	rm -f ${OBJECTS}
