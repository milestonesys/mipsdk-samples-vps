include ../../Makefile.common

OUTFILE = $(OUTDIR)/libgstvpsmetafromroi.so
INPUT = gstvpsmetafromroi.h gstvpsmetafromroi.cpp
OUTPUT = -shared -o  $(OUTFILE)

all: setup-outdir
	$(CC) $(OUTPUT) $(FLAGS) $(INPUT) $(INCLUDE) $(GSTREAMER)

clean:
	@rm -f $(OUTFILE)
