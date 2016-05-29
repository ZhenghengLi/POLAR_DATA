
.PHONY : sci_split sci_split_clean

all: sci_split

clean: sci_split_clean bin_clean

sci_split:
	cd sci_split && $(MAKE)

sci_split_clean:
	cd sci_split && $(MAKE) clean

bin_clean:
	@echo "Cleaning binary files ..."
	@rm -f bin/*
