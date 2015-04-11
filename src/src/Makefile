TOP_SRCDIR = ..
include ${TOP_SRCDIR}/makefile.inc
SUB_DIRS = textproc utils hypar html

#TEST = $(foreach x, $(SUB_DIRS), $(x)/Makefile)

HYPARLIB = libhypar2.so

all: copyObjs ${HYPARLIB}

static: copyObjs

copyObjs: subdirmake
	for d in $(SUB_DIRS); do cp $$d/*.o .obj/; done

${HYPARLIB}: copyObjs
	${CXX} ${LINKER_FLAGS} -o $@ .obj/*.o

subdirmake:
	if [ ! -d .obj ]; then mkdir .obj; fi
	for d in $(SUB_DIRS); do cd $$d && make all && cd -; done

clean:
	for d in $(SUB_DIRS); do cd $$d && make $@ && cd -; done
	rm -f ${HYPARLIB} .obj/*.o

install: all
	if [ ! -z ${INSTALL_LIBDIR} -a -d ${INSTALL_LIBDIR} ]; then cp -f ${HYPARLIB} ${INSTALL_LIBDIR}; fi

run: all
	cd test && make run
