#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#

ifdef BCMA_CINT_ENABLE
# Build CINT as SDKLT LIB

CINT_CPPFLAGS += -DCINT_CONFIG_INCLUDE_SDKLT=1
CINT_CPPFLAGS += -DCINT_CONFIG_INCLUDE_PARSER=1
CINT_CPPFLAGS += -DCINT_CONFIG_INCLUDE_CINT_LOAD=0
TMP_SDK_CPPFLAGS := $(filter-out $(CINT_CPPFLAGS),$(SDK_CPPFLAGS))
TMP_SDK_CPPFLAGS += $(CINT_CPPFLAGS)
override SDK_CPPFLAGS := $(TMP_SDK_CPPFLAGS)

include $(SDK)/make/sublib.mk

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
CORE_DIR := core/

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
CFLAGS += -DCINT_CONFIG_INCLUDE_SDKLT=0
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

OBJS := $(SRCS:%.c=$(CORE_DIR)%.o)

DEPS := $(SRCS:%.c=$(CORE_DIR)%.d)

all: gen cint

gen: cint_c.lex.c cint_c.tab.c cint_wrappers.h

cint: $(OBJS)
	$(CC) -o $@ $+ -ldl -lrt -lreadline -lcurses

cint_c.tab.c: $(CORE_DIR)cint_grammar.y
	$(YACC) -dv -o $(CORE_DIR)$@ $<
# Avoid compiler warnings
	$(REPLACESTRING) -a "YYSTYPE yylval;" "YYSTYPE yylval = 0;" $(CORE_DIR)$@
	$(REPLACESTRING) -a "YYLTYPE yylloc;" "YYLTYPE yylloc = {0,0,0,0};" $(CORE_DIR)$@
# use CINT bindings via BISON interfaces
	$(REPLACESTRING) -a "yyps = (yypstate *) malloc (sizeof *yyps)" \
		"yyps = (yypstate *) YYMALLOC (sizeof *yyps)" $(CORE_DIR)$@
	$(REPLACESTRING) -a "free (yyps)" "YYFREE (yyps)" $(CORE_DIR)$@
# use standard BRCM id/copyright
	@echo '/* ' > $(TMP)
	@echo -n '   $$' >> $(TMP)
	@echo -n 'Id: ' >> $(TMP)
	@echo '$$' >> $(TMP)
	@echo '   $$Copyright: $(YEAR) Broadcom Corp.' >> $(TMP)
	@echo '   All Rights Reserved.$$' >> $(TMP)
	@echo '*/' >> $(TMP)
	@cat $(CORE_DIR)cint_c.tab.h >> $(TMP)
	@rm $(CORE_DIR)cint_c.tab.h
	@mv tmp.h $(CORE_DIR)cint_c.tab.h

cint_c.lex.c: $(CORE_DIR)cint_scanner.l $(CORE_DIR)lex.patch
	$(FLEX) --nounistd -o $(CORE_DIR)$@ $<
	$(PATCH) -p1 $(CORE_DIR)$@ < $(CORE_DIR)lex.patch

cint_wrappers.h: $(CORE_DIR)wrappergen.pl
	$(PERL) $< > $(CORE_DIR)$@

utest: all utest-a.h utest-b.h
	rm -f *.code *.actual *.expected
	$(PERL) $(CORE_DIR)utest.pl $(CORE_DIR)utests

utest-a.h:
	echo "print 1;" > $@

utest-b.h:
	echo "#include <utest-a.h>" > $@
	echo "print 121;" >> $@
	echo "#include <utest-a.h>" >> $@

clean:
	-rm -f cint $(OBJS) $(DEPS) $(CORE_DIR)cint_wrappers.h $(CORE_DIR)cint_c.*.[ch] $(CORE_DIR)cint_c.output utest-*.h

ifneq ($(MAKECMDGOALS),clean)
    include $(DEPS)
endif

# calculate C include dependencies
%.d: %.c
	@$(SHELL) -ec "$(CC) -M -MG $(CFLAGS) $< > $@; [ -s $@ ] || rm -f $@"

endif
