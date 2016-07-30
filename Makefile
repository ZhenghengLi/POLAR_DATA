
.PHONY : all preprocessing preprocessing_clean analysis analysis_clean \
	tools tools_clean

all: preprocessing analysis tools
	@echo " == All make successfully. == "

clean: preprocessing_clean analysis_clean tools_clean
	@echo " == All cleaned. == "

preprocessing:
	cd Preprocessing && $(MAKE)

preprocessing_clean:
	cd Preprocessing && $(MAKE) clean

analysis:
	cd Analysis && $(MAKE)

analysis_clean:
	cd Analysis && $(MAKE) clean

tools:
	cd Tools && $(MAKE)

tools_clean:
	cd Tools && $(MAKE) clean
