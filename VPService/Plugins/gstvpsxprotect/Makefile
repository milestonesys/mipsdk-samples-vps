include ../../Makefile.common

OUTFILE = $(OUTDIR)/libgstvpsxprotect.so
INPUT = gstvpstoxprotectconverter.h gstvpstoxprotectconverter.cpp gstvpsfromxprotectconverter.h gstvpsfromxprotectconverter.cpp gstvpsxprotectjoin.cpp gstvpsxprotectjoin.h gstvpsxprotect.cpp gstvpsxprotect.h
OUTPUT = -shared -o  $(OUTFILE)

all: setup-outdir
	$(CC) $(OUTPUT) $(FLAGS) $(INPUT) $(INCLUDE) $(GSTREAMER)

clean:
	@rm -f $(OUTFILE)
