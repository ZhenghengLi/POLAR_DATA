
.PHONY : sci_split sci_split_clean hk_split hk_split_clean bin_clean

all: sci_split hk_split

clean: sci_split_clean hk_split_clean bin_clean

sci_split:
	cd SCI_Split && $(MAKE)

sci_split_clean:
	cd SCI_Split && $(MAKE) clean

hk_split:
	cd HK_Split && $(MAKE)

hk_split_clean:
	cd HK_Split && $(MAKE) clean

bin_clean:
	@echo "Cleaning binary files ..."
	@rm -f bin/*
