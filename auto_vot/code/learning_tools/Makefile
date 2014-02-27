SUBDIRS := infra2 cmdline htk_utils
CURRENTDIR := $(shell basename `pwd` )

all:
	@echo "[make] Compiling ${CURRENTDIR}."
	@for subdir in ${SUBDIRS} ; do ( cd $$subdir ; ${MAKE} $@) || exit 10 ; done
	@echo "[make] Compiling ${CURRENTDIR} completed."

clean:
	@echo "[make] Cleaning ${CURRENTDIR}."
	@for subdir in ${SUBDIRS} ; do ( cd $$subdir ; ${MAKE} clean $@ ) || exit 10 ; done
	@echo "[make] Cleaning ${CURRENTDIR} completed."

