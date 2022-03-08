include ../../Makefile.common

OUTFILE = $(OUTDIR)/libgstvpsopenvinofaces.so
INPUT = gstvpsopenvinofaces.h gstvpsopenvinofaces.cpp
OUTPUT = -shared -o  $(OUTFILE)

all: setup-outdir
	$(CC) $(OUTPUT) $(FLAGS) $(INPUT) $(INCLUDE) $(GSTREAMER)

clean:
	@rm -f $(OUTFILE)
