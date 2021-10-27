include ../Makefile.common

SRC = GenericByteData
OUTFILE = $(OUTDIR)/libvpsutilities.so
INPUT = OSHelper.h OSHelper.cpp $(SRC)/GenericByteData.h $(SRC)/GenericByteData.cpp $(SRC)/GenericByteDataInterface.h $(SRC)/GenericByteDataInterface.cpp
OUTPUT = -shared -o  $(OUTFILE)

all: setup-outdir
	$(CC) $(OUTPUT) $(FLAGS) $(INPUT)

clean:
	@rm -f $(OUTFILE)
