
.PHONY : sci_l1convert sci_l1convert_clean \
	hk_l1convert hk_l1convert_clean bin_clean

all: sci_l1convert hk_l1convert

clean: sci_l1convert_clean hk_l1convert_clean bin_clean

sci_l1convert:
	cd sci_l1convert && $(MAKE)

sci_l1convert_clean:
	cd sci_l1convert && $(MAKE) clean

hk_l1convert:
	cd hk_l1convert && $(MAKE)

hk_l1convert_clean:
	cd hk_l1convert && $(MAKE) clean

bin_clean:
	@echo "Cleaning binary files ..."
	@rm -f bin/*
