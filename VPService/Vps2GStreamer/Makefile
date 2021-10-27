include ../Makefile.common

OUTFILE = $(OUTDIR)/vps2gstreamer.dll
INPUT = vps2gstreamer.h vps2gstreamer.cpp PInvoke.h PInvoke.cpp MediaData.h MediaData.cpp MetaData.h MetaData.cpp IData.h
OUTPUT = -shared -o  $(OUTFILE)

all: setup-outdir
	$(CC) $(OUTPUT) $(FLAGS) $(INPUT) $(GSTREAMER)

clean:
	@rm -f $(OUTFILE)
