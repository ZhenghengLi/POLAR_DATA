
.PHONY : all \
	HK_RawSplit HK_RawSplit_clean \
	HK_RawTime HK_RawTime_clean \
	SCI_RawSplit SCI_RawSplit_clean \
	SCI_RawTime SCI_RawTime_clean

all: SCI_RawSplit HK_RawSplit SCI_RawTime HK_RawTime

clean: SCI_RawSplit_clean HK_RawSplit_clean SCI_RawTime_clean HK_RawTime_clean

HK_RawSplit:
	cd HK_RawSplit && $(MAKE)

HK_RawSplit_clean:
	cd HK_RawSplit && $(MAKE) clean

HK_RawTime:
	cd HK_RawTime && $(MAKE)

HK_RawTime_clean:
	cd HK_RawTime && $(MAKE) clean

SCI_RawSplit:
	cd SCI_RawSplit && $(MAKE)

SCI_RawSplit_clean:
	cd SCI_RawSplit && $(MAKE) clean

SCI_RawTime:
	cd SCI_RawTime && $(MAKE)

SCI_RawTime_clean:
	cd SCI_RawTime && $(MAKE) clean
