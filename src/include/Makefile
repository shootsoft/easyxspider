TOP_SRCDIR = ..
include ${TOP_SRCDIR}/makefile.inc
SUB_DIRS = hypar

all:
	@echo "Nothing to make in here ..."

static:
	/bin/true

clean:
	/bin/true

install:
	if [ ! -z ${INSTALL_INCDIR} -a -d ${INSTALL_INCDIR} ]; then cp -r ${SUB_DIRS} ${INSTALL_INCDIR}; fi
