include ../../Makefile.common

OUTFILE = $(OUTDIR)/libgstvpsjpegtranscoder.so
INPUT = gstvpsjpegtranscoder.h gstvpsjpegtranscoder.cpp
OUTPUT = -shared -o  $(OUTFILE)

all: setup-outdir
	$(CC) $(OUTPUT) $(FLAGS) $(INPUT) $(INCLUDE) $(GSTREAMER)

clean:
	@rm -f $(OUTFILE)
