
.PHONY : all clean bin_clean

DIRS:=$(wildcard *)
EXCL:=Makefile bin script
GOAL:=$(filter-out $(EXCL),$(DIRS))

all:
	for x in $(GOAL); do\
		cd $$x && $(MAKE) && cd ..;\
	done

clean: bin_clean
	for x in $(GOAL); do\
		cd $$x && $(MAKE) clean && cd ..;\
	done

bin_clean:
	@echo "Cleaning binary files ..."
	@rm -f bin/*
