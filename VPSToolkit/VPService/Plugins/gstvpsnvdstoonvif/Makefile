include ../../Makefile.common

OUTFILE = $(OUTDIR)/libgstvpsnvdstoonvif.so
INPUT = gstvpsnvdstoonvif.h gstvpsnvdstoonvif.cpp
OUTPUT = -shared -o  $(OUTFILE)
INCLUDE = -I/opt/nvidia/deepstream/deepstream-4.0/sources/includes/
LINKER = -L/opt/nvidia/deepstream/deepstream-4.0/lib/ -lnvdsgst_meta -lnvds_meta

all: setup-outdir
	$(CC) $(OUTPUT) $(FLAGS) $(INPUT) $(INCLUDE) $(LINKER) $(GSTREAMER)

clean:
	@rm -f $(OUTFILE)
