export TOP_SRCDIR = $(shell pwd)

include makefile.inc

SUB_DIRS = src test include

#TEST = $(foreach x, $(SUB_DIRS), $(x)/Makefile)

all:
	for d in $(SUB_DIRS); do cd $$d && make $@ && cd -; done

static:
	for d in $(SUB_DIRS); do cd $$d && make $@ && cd -; done

clean:
	for d in $(SUB_DIRS); do cd $$d && make $@ && cd -; done

install: all
	for d in $(SUB_DIRS); do cd $$d && make $@ && cd -; done

static_install: static
	cd test && make $@ && cd -

run: all
	cd test && make run
