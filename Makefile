
.PHONY : all sci sci_clean sci_25 sci_25_clean hk hk_clean \
	utc utc_clean tools tools_clean analysis analysis_clean \
	bin_clean

all: sci sci_25 hk utc tools analysis

clean: sci_clean sci_25_clean hk_clean utc_clean tools_clean \
	analysis_clean bin_clean

sci:
	cd SCI_Decode && $(MAKE)

sci_clean:
	cd SCI_Decode && $(MAKE) clean

sci_25:
	cd SCI_Decode_25 && $(MAKE)

sci_25_clean:
	cd SCI_Decode_25 && $(MAKE) clean

hk:
	cd HK_Decode && $(MAKE)

hk_clean:
	cd HK_Decode && $(MAKE) clean

utc:
	cd UTC_Calculate && $(MAKE)

utc_clean:
	cd UTC_Calculate && $(MAKE) clean

tools:
	cd tools && $(MAKE)

tools_clean:
	cd tools && $(MAKE) clean

analysis:
	cd Analysis && $(MAKE)

analysis_clean:
	cd Analysis && $(MAKE) clean

bin_clean:
	@echo "Cleaning binary files ..."
	@rm -f bin/*
