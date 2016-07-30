
.PHONY : all decode decode_clean time time_clean \
	orbit_split orbit_split_clean l1_convert l1_convert_clean \
	bin_clean

all: decode time orbit_split l1_convert

clean: decode_clean  time_clean orbit_split_clean \
	l1_convert_clean bin_clean

decode:
	cd Decode && $(MAKE)

decode_clean:
	cd Decode && $(MAKE) clean

time:
	cd Time_Calculate && $(MAKE)

time_clean:
	cd Time_Calculate && $(MAKE) clean

orbit_split:
	cd Orbit_Split && $(MAKE)

orbit_split_clean:
	cd Orbit_Split && $(MAKE) clean

l1_convert:
	cd L1_Convert && $(MAKE)

l1_convert_clean:
	cd L1_Convert && $(MAKE) clean

bin_clean:
	@echo "Cleaning binary files ..."
	@rm -f bin/*
