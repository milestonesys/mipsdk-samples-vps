include ../Makefile.common

DIRS = gstvpsonvifmeta gstvpsxprotectmeta

all: setup-outdir
	$(foreach var, $(DIRS), make -C $(var)  OUTDIR=$(OUTDIR);)

clean:
	$(foreach var, $(DIRS), make clean -C $(var);)