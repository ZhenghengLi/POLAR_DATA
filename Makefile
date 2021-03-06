
.PHONY : all clean bin_clean

DIRS:=$(wildcard *)
EXCL:=Makefile env.sh Documents README.md
GOAL:=$(filter-out $(EXCL),$(DIRS))

all:
	for x in $(GOAL); do\
		cd $$x && $(MAKE) && cd ..;\
	done
	@echo " == All make successfully. == "

clean: bin_clean
	for x in $(GOAL); do\
		cd $$x && $(MAKE) clean && cd ..;\
	done
	@echo " == All cleaned. == "

