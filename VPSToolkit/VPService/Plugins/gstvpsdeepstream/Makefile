include ../../Makefile.common

OUTFILE = $(OUTDIR)/libgstvpsdeepstream.so
INPUT = gstvpsdeepstream.h gstvpsdeepstream.cpp
OUTPUT = -shared -o  $(OUTFILE)
INCLUDE = -I/opt/nvidia/deepstream/deepstream-4.0/sources/includes/
all: setup-outdir
	$(CC) $(OUTPUT) $(FLAGS) $(INPUT) $(INCLUDE) $(GSTREAMER)

clean:
	@rm -f $(OUTFILE)
