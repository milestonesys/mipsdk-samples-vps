include ../../Makefile.common

OUTFILE = $(OUTDIR)/libgstvpsboundingboxes.so
INPUT = gstvpsboundingboxes.h gstvpsboundingboxes.cpp
OUTPUT = -shared -o  $(OUTFILE)

all: setup-outdir
	$(CC) $(OUTPUT) $(FLAGS) $(INPUT) $(INCLUDE) $(GSTREAMER)

clean:
	@rm -f $(OUTFILE)
