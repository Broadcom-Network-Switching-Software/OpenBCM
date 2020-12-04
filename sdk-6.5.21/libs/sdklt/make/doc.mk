#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# SDK default documentation target
#

# Default output directory
DOC_DSTDIR ?= doxygen$(DOXY_EXT)

# Default doxygen tool
ifndef DOXYGEN
DOXYGEN = doxygen
endif

# Minimum version for doxygen binary
DOXYVER = 1.8.9.1

# Default ditaa tool
ifndef DITAA
DITAA := $(shell which ditaa 2>/dev/null)
endif

# Generate BMAP figure from ASCII file
DOCDIR := DOC
FIGDIR := DOC/generated
FIGS := $(patsubst %.fig, ${FIGDIR}/%.png, $(notdir $(wildcard ${DOCDIR}/*.fig)))

${FIGDIR}/%.png: ${DOCDIR}/%.fig
	if [ -z "$(DITAA)" ] || [ ! -x $(DITAA) ]; then \
		echo "Error: Image generation requires java-based ditaa tool" \
		     "(ditaa.sourceforge.net)" 1>&2; \
		exit 1; \
	fi
	mkdir -p $(dir $@)
	$(eval TMPFILE := $(shell mktemp ditaa.XXXXXX))
	cp $< $(TMPFILE).fig
	echo "Creating image $@"
	unix2dos $(TMPFILE).fig 2>/dev/null
	if [ ! -e "$(TMPFILE).fig" ]; then \
		echo "Error: Image source file $(TMPFILE) does not exist" 1>&2; \
		exit 1; \
	fi
	$(DITAA) -E -o $(TMPFILE).fig >/dev/null
	mv -f $(TMPFILE).png $@
	rm -f $(TMPFILE)*

# The construct below overrides the Doxyfile OUTPUT_DIRECTORY
doc: doxychk figs
	@echo Create documentation in $(DOC_DSTDIR)
	( cat Doxyfile$(DOXY_EXT) ; echo "OUTPUT_DIRECTORY=$(DOC_DSTDIR)" ) | $(DOXYGEN) - > /dev/null

# Need separate target for figures
figs:
ifneq (,$(FIGS))
	for tgt in $(FIGS); do $(MAKE) $$tgt; done
endif

clean::
	@echo Cleaning documentation output in $(DOC_DSTDIR)
	rm -rf $(DOC_DSTDIR)

cleanall::
	@echo Cleaning documentation image in $(FIGDIR)
#	rm -rf $(FIGDIR)

# Check doxygen version
doxychk:
	$(SDK)/make/vercomp.pl `$(DOXYGEN) --version` ge $(DOXYVER) \
		|| (make doxyver; exit 1)

# Print doxygen version
doxyver:
	@printf "Current doxygen version: %s\n" `$(DOXYGEN) --version`
	@printf "Must be version %s or newer\n" $(DOXYVER)

.PHONY: doc figs clean doxychk doxyver
