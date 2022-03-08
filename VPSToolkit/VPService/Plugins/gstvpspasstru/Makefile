include ../../Makefile.common

OUTFILE = $(OUTDIR)/libgstvpspasstru.so
INPUT = gstvpspasstru.h gstvpspasstru.cpp
OUTPUT = -shared -o  $(OUTFILE)

all: setup-outdir
	$(CC) $(OUTPUT) $(FLAGS) $(INPUT) $(INCLUDE) $(GSTREAMER)

clean:
	@rm -f $(OUTFILE)
