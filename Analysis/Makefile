
.PHONY : lib lib_clean \
	ped_calc ped_calc_clean \
	xtalk_calc xtalk_calc_clean
	bin_clean

all: lib ped_calc xtalk_calc

clean: lib_clean ped_calc_clean xtalk_calc_clean bin_clean

lib:
	cd lib && $(MAKE)

lib_clean:
	cd lib && $(MAKE) clean

ped_calc:
	cd ped_calc && $(MAKE)

ped_calc_clean:
	cd ped_calc && $(MAKE) clean

xtalk_calc:
	cd xtalk_calc && $(MAKE)

xtalk_calc_clean:
	cd xtalk_calc && $(MAKE) clean

bin_clean:
	@echo "Cleaning binary files ..."
	@rm -f bin/*
