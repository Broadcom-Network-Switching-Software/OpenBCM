# -*- Makefile -*-
# $Id: Makefile,v 1.1 2011/08/31 18:36:07 dkelley Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# API mode Makefile
#

ifneq (,$(findstring apimode,$(MAKECMDGOALS)))
LBUILD=1
endif

ifndef LBUILD

# Build as part of the SDK

LOCALDIR = src/appl/diag/api

include ${SDK}/make/Make.config

CFGFLAGS += -I$(SDK)/src/appl/cint
CFGFLAGS += -I$(SDK)/src/appl/diag/cint

lib = libdiagapi
include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend
else

# Build standalone interpreter application

CFLAGS := -g
CFLAGS += -DYYDEBUG
CFLAGS += -DAPIMODE_INCLUDE_MAIN=1
CFLAGS += -Wall
CFLAGS += -Werror 
CFLAGS += -Wformat
CFLAGS += -Wdeclaration-after-statement
CFLAGS += -Wuninitialized
CFLAGS += -Wunreachable-code
CFLAGS += -Winline
CFLAGS += -Wpointer-sign
CFLAGS += -Wextra
CFLAGS += -Wmissing-format-attribute
CFLAGS += -Wunused-parameter
CFLAGS += -Wstrict-prototypes
CFLAGS += -Wshadow
CFLAGS += -Wbad-function-cast
CFLAGS += -Wlogical-op
CFLAGS += -Waggregate-return
CFLAGS += -Wold-style-definition
CFLAGS += -Wmissing-prototypes
CFLAGS += -Wmissing-declarations
CFLAGS += -Wmissing-noreturn
CFLAGS += -Wnested-externs
CFLAGS += -ansi
CFLAGS += -I../../../../include
CFLAGS += -I../../../appl/cint
CFLAGS += -DVA_COPY_PTR
CFLAGS += -DCINT_CONFIG_INCLUDE_STDLIB=1
CFLAGS += -D_GNU_SOURCE

LDFLAGS := -g
CC := /tools/bin/gcc
BISON := /tools/bin/bison
YEAR := $(shell date +%Y)
TMP := tmp.h

apimode: main

gen: api_grammar.tab.c

main.o: api_mode.h api_grammar.tab.h

api_mode.o: api_mode.h api_grammar.tab.h

main: main.o api_grammar.tab.o api_mode.o

api_grammar.tab.h: api_grammar.tab.c

api_grammar.tab.c: api_grammar.y api_mode.h
	$(BISON) -g -d -v $<
	@echo '/* ' > $(TMP)
	@echo -n '   $$' >> $(TMP)
	@echo -n 'Id: ' >> $(TMP)
	@echo '$$' >> $(TMP)
	@echo '   $$Copyright: $(YEAR) Broadcom Corp.' >> $(TMP)
	@echo '   All Rights Reserved.$$' >> $(TMP)
	@echo '*/' >> $(TMP)
	@cat api_grammar.tab.h >> $(TMP)
	@rm api_grammar.tab.h
	@mv tmp.h api_grammar.tab.h

grammar: api_grammar.tab.c

clean:
	-rm -f api_grammar.output \
                api_grammar.tab.* *.o *.dot main


endif
