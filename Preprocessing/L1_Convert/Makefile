
.PHONY : sci_l1convert sci_l1convert_clean \
	hk_l1convert hk_l1convert_clean bin_clean

all: sci_l1convert hk_l1convert

clean: sci_l1convert_clean hk_l1convert_clean bin_clean

sci_l1convert:
	cd SCI_L1Convert && $(MAKE)

sci_l1convert_clean:
	cd SCI_L1Convert && $(MAKE) clean

hk_l1convert:
	cd HK_L1Convert && $(MAKE)

hk_l1convert_clean:
	cd HK_L1Convert && $(MAKE) clean

bin_clean:
	@echo "Cleaning binary files ..."
	@rm -f bin/*
