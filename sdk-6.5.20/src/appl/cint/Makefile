# $Id: Makefile,v 1.30 2013/05/10 20:55:15 dkelley Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# Makefile for CINT library
#

ifneq (,$(findstring cint,$(MAKECMDGOALS)))
LBUILD=1
endif

ifneq (,$(findstring utest,$(MAKECMDGOALS)))
LBUILD=1
endif

ifndef LBUILD

# Build as part of the SDK
LOCALDIR = src/appl/cint
include ${SDK}/make/Make.config
lib = libcint
include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend

else

# Build standalone interpreter application

CC := /tools/bin/gcc
PERL := perl
FLEX := /projects/ntsw-tools/bin/flex-2.5.31
YACC := /tools/bin/yacc
PATCH := patch
REPLACESTRING := /projects/ntsw-tools/bin/replacestring
YEAR := $(shell date +%Y)
TMP := tmp.h

SRCS := cint_ast.c
SRCS += cint_ast_debug.c
SRCS += cint_builtins.c
SRCS += cint_datatypes.c
SRCS += cint_debug.c
SRCS += cint_error.c
SRCS += cint_eval_ast_cint.c
SRCS += cint_eval_ast_print.c
SRCS += cint_eval_asts.c
SRCS += cint_internal.c
SRCS += cint_interpreter.c
SRCS += cint_operators.c
SRCS += cint_parser.c
SRCS += cint_test_data.c
SRCS += cint_variables.c
SRCS += main.c
SRCS += cint_stubs.c
# generated
SRCS += cint_c.lex.c
SRCS += cint_c.tab.c

CFLAGS := -g -O0

# portability interfaces - pick one
CFLAGS += -DCINT_CONFIG_INCLUDE_STDLIB=1
CFLAGS += -DCINT_CONFIG_INCLUDE_SDK_SAL=0
CFLAGS += -DCINT_CONFIG_INCLUDE_STUBS=0

# test features
CFLAGS += -DCINT_CONFIG_INCLUDE_LOAD=1
CFLAGS += -DCINT_CONFIG_INCLUDE_MAIN=1
CFLAGS += -DCINT_CONFIG_INCLUDE_TEST_DATA=1

# FILE IO and READLINE
#CFLAGS += -DCINT_CONFIG_FILE_IO=0
#CFLAGS += -DCINT_CONFIG_INCLUDE_XINCLUDE=0
#CFLAGS += -DCINT_CONFIG_INCLUDE_PARSER_READLINE=0
#CFLAGS += -DCINT_CONFIG_INCLUDE_PARSER_ADD_HISTORY=0

# other configs
#CFLAGS += -DCINT_CONFIG_INCLUDE_PARSER=0
#CFLAGS += -DCINT_CONFIG_INCLUDE_DOUBLES=0
#CFLAGS += -DCINT_CONFIG_INCLUDE_LONGLONGS=0
#CFLAGS += -DCINT_CONFIG_INCLUDE_CINT_LOAD=0
#CFLAGS += -DCINT_CONFIG_INCLUDE_DTRACE=0
#CFLAGS += -DCINT_CONFIG_INCLUDE_DEBUG=0
#CFLAGS += -DCINT_CONFIG_INCLUDE_POSIX_TIMER=0
#CFLAGS += -DCINT_CONFIG_YAPI=1

# unused configs
#CFLAGS += -DCINT_CONFIG_PARSER_STDIN=0
#CFLAGS += -DCINT_CONFIG_INCLUDE_CINT_SOURCE=1

CFLAGS += -Wall
CFLAGS += -Werror
CFLAGS += -DYYDEBUG

OBJS := $(SRCS:%.c=%.o)

DEPS := $(SRCS:%.c=%.d)

all: cint

gen: cint_c.lex.c cint_c.tab.c cint_wrappers.h 

cint: $(OBJS)
	$(CC) -o $@ $+ -ldl -lrt -lreadline -lcurses

cint_c.tab.c: cint_grammar.y
	$(YACC) -dv -o $@ $<
# Avoid compiler warnings
	$(REPLACESTRING) -a "YYSTYPE yylval;" "YYSTYPE yylval = 0;" $@
	$(REPLACESTRING) -a "YYLTYPE yylloc;" "YYLTYPE yylloc = {0,0,0,0};" $@
# use CINT bindings via BISON interfaces
	$(REPLACESTRING) -a "yyps = (yypstate *) malloc (sizeof *yyps)" \
		"yyps = (yypstate *) YYMALLOC (sizeof *yyps)" $@
	$(REPLACESTRING) -a "free (yyps)" "YYFREE (yyps)" $@
# use standard BRCM id/copyright
	@echo '/* ' > $(TMP)
	@echo -n '   $$' >> $(TMP)
	@echo -n 'Id: ' >> $(TMP)
	@echo '$$' >> $(TMP)
	@echo '   $$Copyright: $(YEAR) Broadcom Corp.' >> $(TMP)
	@echo '   All Rights Reserved.$$' >> $(TMP)
	@echo '*/' >> $(TMP)
	@cat cint_c.tab.h >> $(TMP)
	@rm cint_c.tab.h
	@mv tmp.h cint_c.tab.h

cint_c.lex.c: cint_scanner.l lex.patch
	$(FLEX) --nounistd -o $@ $<
	$(PATCH) < lex.patch

cint_wrappers.h: wrappergen.pl
	$(PERL) $< > $@

utest: cint utest-a.h utest-b.h
	rm -f *.code *.actual *.expected
	$(PERL) utest.pl utests

utest-a.h:
	echo "print 1;" > $@

utest-b.h:
	echo "#include <utest-a.h>" > $@
	echo "print 121;" >> $@
	echo "#include <utest-a.h>" >> $@


clean:
	-rm -f cint $(OBJS) $(DEPS) cint_wrappers.h cint_c.*.[ch] utest-*.h

ifneq ($(MAKECMDGOALS),clean)
    include $(DEPS)
endif

# calculate C include dependencies
%.d: %.c
	@$(SHELL) -ec "$(CC) -M -MG $(CFLAGS) $< > $@; [ -s $@ ] || rm -f $@"

endif



