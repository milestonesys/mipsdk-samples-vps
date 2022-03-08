include ../../Makefile.common

OUTFILE = $(OUTDIR)/libgstvpsxprotectmeta.so

INPUT = gstvpsxprotectmeta.h gstvpsxprotectmeta.cpp
OUTPUT = -shared -o  $(OUTFILE)

all: setup-outdir
	$(CC) $(OUTPUT) $(FLAGS) $(INPUT) $(INCLUDE) $(GSTREAMER)

clean:
	@rm -f $(OUTFILE) 