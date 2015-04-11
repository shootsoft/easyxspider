TOP_SRCDIR = ../..
include ${TOP_SRCDIR}/makefile.inc
SOURCES = $(shell ls *.cpp)

OBJECTS = $(SOURCES:.cpp=.o)

all: ${OBJECTS} ${TOP_SRCDIR}/include/hypar/HyXML.hpp

clean:
	rm -f ${OBJECTS}
