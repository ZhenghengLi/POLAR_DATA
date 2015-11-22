
.PHONY : all sci sci_clean

all: sci

sci:
	cd SCI_Decode; make

sci_clean:
	cd SCI_Decode; make clean
