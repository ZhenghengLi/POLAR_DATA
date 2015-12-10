
.PHONY : all sci sci_clean sci_25 sci_25_clean

all: sci sci_25

clean: sci_clean sci_25_clean bin_clean

sci:
	cd SCI_Decode; make

sci_clean:
	cd SCI_Decode; make clean

sci_25:
	cd SCI_Decode_25; make

sci_25_clean:
	cd SCI_Decode_25; make clean

bin_clean:
	@echo "Cleaning binary files ..."
	@rm -f bin/*
