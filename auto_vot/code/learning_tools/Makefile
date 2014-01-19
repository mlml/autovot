SUBDIRS := infra2 cmdline htk_utils
CURRENTDIR := $(shell basename `pwd` )

all:
	@echo ">>> Compiling ${CURRENTDIR} <<<"
	@for subdir in ${SUBDIRS} ; do ( cd $$subdir ; ${MAKE} $@) || exit 10 ; done
	@echo ">> !!! Ok !!! <<<"

clean:
	@echo ">>> Cleaning ${CURRENTDIR} <<<"
	@for subdir in ${SUBDIRS} ; do ( cd $$subdir ; ${MAKE} clean $@ ) || exit 10 ; done
	@echo ">> !!! Ok !!! <<<"

