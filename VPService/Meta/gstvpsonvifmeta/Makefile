include ../../Makefile.common

OUTFILE = $(OUTDIR)/libgstvpsonvifmeta.so

INPUT = gstvpsonvifmeta.h gstvpsonvifmeta.cpp
OUTPUT = -shared -o  $(OUTFILE)

all: setup-outdir
	$(CC) $(OUTPUT) $(FLAGS) $(INPUT) $(INCLUDE) $(GSTREAMER)

clean:
	@rm -f $(OUTFILE) 