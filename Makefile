
.PHONY : all sci sci_clean sci_25 sci_25_clean

all: sci sci_25 hk analysis

clean: sci_clean sci_25_clean hk_clean analysis_clean bin_clean

sci:
	cd SCI_Decode; make

sci_clean:
	cd SCI_Decode; make clean

sci_25:
	cd SCI_Decode_25; make

sci_25_clean:
	cd SCI_Decode_25; make clean

hk:
	cd HK_Decode; make

hk_clean:
	cd HK_Decode; make clean

analysis:
	cd Analysis; make

analysis_clean:
	cd Analysis; make clean

bin_clean:
	@echo "Cleaning binary files ..."
	@rm -f bin/*
