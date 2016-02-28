
.PHONY : lib lib_clean pn_subtract pn_subtract_clean

all: lib pn_subtract

clean: lib_clean pn_subtract_clean bin_clean

lib:
	cd lib && $(MAKE)

lib_clean:
	cd lib && $(MAKE) clean

pn_subtract:
	cd pn_subtract && $(MAKE)

pn_subtract_clean:
	cd pn_subtract && $(MAKE) clean

bin_clean:
	@echo "Cleaning binary files ..."
	@rm -f bin/*
