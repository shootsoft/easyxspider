TOP_SRCDIR = ..
include ${TOP_SRCDIR}/makefile.inc

#TEST = $(foreach x, $(SUB_DIRS), $(x)/Makefile)
EXESOURCES = $(shell ls *.cpp)
EXES = $(EXESOURCES:.cpp=.exe)
EXES_STATIC = $(EXESOURCES:.cpp=.static.exe)

.SUFFIXES: .cpp .exe .static.exe

.cpp.exe:
	${CXX} ${CPPFLAGS} ${INCLUDES} ${LIBDIRS} ${LIBS} $< -o $@

.cpp.static.exe:
	${CXX} ${CPPFLAGS_STATIC} ${INCLUDES} ${LIBS_STATIC} $< -o $@

static: ${EXES_STATIC}

all: ${EXES}

clean:
	rm -f ${EXES} ${EXES_STATIC}

install: all
	if [ ! -z ${INSTALL_BINDIR} -a -d ${INSTALL_BINDIR} ]; then cp -f ${EXES} ${INSTALL_BINDIR}; fi

static_install: static
	if [ ! -z ${INSTALL_BINDIR} -a -d ${INSTALL_BINDIR} ]; then cp -f ${EXES_STATIC} ${INSTALL_BINDIR}; fi
