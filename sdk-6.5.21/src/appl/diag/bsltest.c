/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Broadcom System Log Test Suites
 *
 * This framework can launch test suites to validate BSL functionality
 * for correctness and backward compatibilty.
 *
 * Each test suite is composed of one or more reusable test cases.
 *
 * There are two main entries:
 *
 *   bsltest_run(int)   - run a test suite
 *   bsltest_list()     - list all test suite
 */

#include <sal/core/libc.h> 
#include <shared/bsl.h> 
#include <appl/diag/bslenable.h>
#include <appl/diag/bslsink.h>
#include <appl/diag/bsltest.h>

/* Test parameter */
volatile int bsltest_param = 12345678;

/****************************************
 * Helper functions
 */

STATIC void
bsltest_sink_prefix_set(bslsink_sink_t *sink, const char *prefix)
{
    sal_strncpy(sink->prefix_format, prefix, BSLSINK_PREFIX_FORMAT_MAX);
}

/****************************************
 * Test Cases
 */

STATIC void
bsltest_case_basic_macro(int unit)
{
    bsl_printf(">> bsltest_case_basic_macro\n");

    LOG_FATAL(BSL_LS_APPL_SYMTAB,
              (BSL_META("LOG_FATAL unit=%d\n"),
               unit));
    LOG_ERROR(BSL_LS_APPL_SYMTAB,
              (BSL_META("LOG_ERROR unit=%d\n"),
               unit));
    LOG_WARN(BSL_LS_APPL_SYMTAB,
             (BSL_META("LOG_WARN unit=%d\n"),
              unit));
    LOG_INFO(BSL_LS_APPL_SYMTAB,
             (BSL_META("LOG_INFO unit=%d\n"),
              unit));
    LOG_VERBOSE(BSL_LS_APPL_SYMTAB,
                (BSL_META("LOG_VERBOSE unit=%d\n"),
                 unit));
    LOG_DEBUG(BSL_LS_APPL_SYMTAB,
              (BSL_META("LOG_DEBUG unit=%d\n"),
               unit));
}

STATIC void
bsltest_case_option_macro(void)
{
    int unit = 0;
    int port = 1;
    int xtra = 2;

    bsl_printf(">> bsltest_case_option_macro\n");

    LOG_WARN(BSL_LS_APPL_SYMTAB,
             (BSL_META("LOG_WARN <nounit> (%d)\n"),
              bsltest_param));
    LOG_WARN(BSL_LS_APPL_SYMTAB,
             (BSL_META_U(unit,
                         "LOG_WARN unit (%d)\n"),
              bsltest_param));
    LOG_WARN(BSL_LS_APPL_SYMTAB,
             (BSL_META_UP(unit, port,
                          "LOG_WARN unit/port (%d)\n"),
              bsltest_param));
    LOG_WARN(BSL_LS_APPL_SYMTAB,
             (BSL_META_UPX(unit, port, xtra,
                           "LOG_WARN unit/port/xtra (%d)\n"),
              bsltest_param));
}

STATIC void
bsltest_case_incr_single(void)
{
    int idx;

    bsl_log_start(BSL_APPL_SHELL, bslSeverityWarn, BSL_UNIT_UNKNOWN,
                  "[bsl_log_start]");
    for (idx = 0; idx < 2; idx++) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityWarn, bslSinkIgnore, BSL_UNIT_UNKNOWN,
                    "[bsl_log_add #%d]", idx);
    }
    bsl_log_end(BSL_APPL_SHELL, bslSeverityWarn, bslSinkIgnore, BSL_UNIT_UNKNOWN,
                "[bsl_log_end]\n");
}

STATIC void
bsltest_case_incr_multi(void)
{
    int idx;

    bsl_log_start(BSL_APPL_SHELL, bslSeverityWarn, BSL_UNIT_UNKNOWN,
                  "[bsl_log_start]");
    for (idx = 0; idx < 7; idx++) {
        bsl_log_add(BSL_APPL_SHELL, bslSeverityWarn, bslSinkIgnore, BSL_UNIT_UNKNOWN,
                    "[bsl_log_add #%d]", idx);
        if ((idx & 0x1) == 1) {
            bsl_log_add(BSL_APPL_SHELL, bslSeverityWarn, bslSinkIgnore, BSL_UNIT_UNKNOWN,
                        "[newline]\n", idx);
        }
    }
    bsl_log_end(BSL_APPL_SHELL, bslSeverityWarn, bslSinkIgnore, BSL_UNIT_UNKNOWN,
                "[bsl_log_end]\n");
}

STATIC void
bsltest_case_incr_multi_opt(void)
{
    int idx;

    LOG_WARN(BSL_LS_APPL_SYMTAB,
             (BSL_META_O(BSL_META_OPT_START | BSL_META_OPT_PFX_NL,
                         "[OPT_START|OPT_PFX_NL]\n")));
    for (idx = 0; idx < 7; idx++) {
        LOG_WARN(BSL_LS_APPL_SYMTAB,
                 (BSL_META_O(0,
                             "[0 #%d]"), idx));
        if ((idx & 0x1) == 1) {
            LOG_WARN(BSL_LS_APPL_SYMTAB,
                     (BSL_META_O(0,
                                 "[0] newline\n")));
        }
    }
    LOG_WARN(BSL_LS_APPL_SYMTAB,
             (BSL_META_O(BSL_META_OPT_END,
                         "[OPT_END]\n")));
}

/****************************************
 * Test Suites
 */

STATIC void
bsltest_test_suite_0(void)
{
    bsltest_case_basic_macro(0);
}

STATIC void
bsltest_test_suite_1(void)
{
}

STATIC void
bsltest_test_suite_2(void)
{
    int sev, cur_sev;

    cur_sev = bslenable_get(bslLayerAppl, bslSourceSymtab);
    for (sev = 0; sev < bslSeverityCount; sev++) {
        bslenable_set(bslLayerAppl, bslSourceSymtab, sev);
        bsltest_case_basic_macro(0);
    }
    bslenable_set(bslLayerAppl, bslSourceSymtab, cur_sev);
}

STATIC void
bsltest_test_suite_3(void)
{
    bslsink_sink_t cursink, *sink = bslsink_sink_find_by_id(0);

    if (sink == NULL) {
        return;
    }

    sal_memcpy(&cursink, sink, sizeof(cursink));
    bsltest_sink_prefix_set(sink, "");
    sink->prefix_range.min = bslSeverityOff;
    sink->prefix_range.max = sink->prefix_range.min;

    bsltest_case_incr_single();

    sal_memcpy(sink, &cursink, sizeof(cursink));
}

STATIC void
bsltest_test_suite_4(void)
{
    bslsink_sink_t cursink, *sink = bslsink_sink_find_by_id(0);

    if (sink == NULL) {
        return;
    }

    sal_memcpy(&cursink, sink, sizeof(cursink));
    bsltest_sink_prefix_set(sink, "PREFIX:");
    sink->prefix_range.min = bslSeverityWarn;
    sink->prefix_range.max = sink->prefix_range.min;

    bsltest_case_incr_single();

    sal_memcpy(sink, &cursink, sizeof(cursink));
}

STATIC void
bsltest_test_suite_5(void)
{
    bslsink_sink_t cursink, *sink = bslsink_sink_find_by_id(0);

    if (sink == NULL) {
        return;
    }

    sal_memcpy(&cursink, sink, sizeof(cursink));
    bsltest_sink_prefix_set(sink, "");
    sink->prefix_range.min = bslSeverityOff;
    sink->prefix_range.max = sink->prefix_range.min;

    bsltest_case_incr_multi();

    sal_memcpy(sink, &cursink, sizeof(cursink));
}

STATIC void
bsltest_test_suite_6(void)
{
    bslsink_sink_t cursink, *sink = bslsink_sink_find_by_id(0);

    if (sink == NULL) {
        return;
    }

    sal_memcpy(&cursink, sink, sizeof(cursink));
    bsltest_sink_prefix_set(sink, "PREFIX:");
    sink->prefix_range.min = bslSeverityWarn;
    sink->prefix_range.max = sink->prefix_range.min;

    bsltest_case_incr_multi();

    sal_memcpy(sink, &cursink, sizeof(cursink));
}

STATIC void
bsltest_test_suite_7(void)
{
    bslsink_sink_t cursink, *sink = bslsink_sink_find_by_id(0);

    if (sink == NULL) {
        return;
    }

    sal_memcpy(&cursink, sink, sizeof(cursink));
    bsltest_sink_prefix_set(sink, "PREFIX:");
    sink->prefix_range.min = bslSeverityFatal;
    sink->prefix_range.max = bslSeverityError;

    bsltest_case_basic_macro(0);

    sal_memcpy(sink, &cursink, sizeof(cursink));
}

STATIC void
bsltest_test_suite_8(void)
{
    bsltest_case_option_macro();
}

STATIC void
bsltest_test_suite_9(void)
{
    bslsink_sink_t cursink, *sink = bslsink_sink_find_by_id(0);

    if (sink == NULL) {
        return;
    }

    sal_memcpy(&cursink, sink, sizeof(cursink));
    bsltest_sink_prefix_set(sink, "PREFIX(u=%u p=%p x=%x)");
    sink->prefix_range.min = bslSeverityWarn;
    sink->prefix_range.max = sink->prefix_range.min;

    bsltest_case_option_macro();

    sal_memcpy(sink, &cursink, sizeof(cursink));
}

STATIC void
bsltest_test_suite_10(void)
{
    bsltest_case_incr_multi_opt();
}

STATIC void
bsltest_test_suite_11(void)
{
    bslsink_sink_t cursink, *sink = bslsink_sink_find_by_id(0);

    if (sink == NULL) {
        return;
    }

    sal_memcpy(&cursink, sink, sizeof(cursink));
    bsltest_sink_prefix_set(sink, "PREFIX:");
    sink->prefix_range.min = bslSeverityWarn;
    sink->prefix_range.max = sink->prefix_range.min;

    bsltest_case_incr_multi_opt();

    sal_memcpy(sink, &cursink, sizeof(cursink));
}

STATIC void
bsltest_test_suite_12(void)
{
    bsltest_case_basic_macro(1);
}

STATIC void
bsltest_test_suite_13(void)
{
    bsltest_case_basic_macro(BSL_UNIT_UNKNOWN);
}

typedef void (*test_suite_f)(void);

typedef struct test_suite_s {
    test_suite_f func;
    const char *descr;
} test_suite_t;

static test_suite_t test_suites[] = {
    { bsltest_test_suite_0,  "Test all log macros with current settings" },
    { bsltest_test_suite_1,  NULL },
    { bsltest_test_suite_2,  "Test all log macros across all severities" },
    { bsltest_test_suite_3,  "Test incremental logging single line" },
    { bsltest_test_suite_4,  "Test incremental logging single line with prefix" },
    { bsltest_test_suite_5,  "Test incremental logging multiple lines" },
    { bsltest_test_suite_6,  "Test incremental logging multiple lines with prefix" },
    { bsltest_test_suite_7,  "Test current settings with prefixed Fatal/Error" },
    { bsltest_test_suite_8,  "Test all log macros with meta options" },
    { bsltest_test_suite_9,  "Test all log macros with meta options and prefix" },
    { bsltest_test_suite_10, "Test option logging multiple lines" },
    { bsltest_test_suite_11, "Test option logging multiple lines with prefix" },
    { bsltest_test_suite_12, "Test all log functions for unit 1" },
    { bsltest_test_suite_13, "Test all log functions for unknown unit (-1)" },
    { NULL, NULL }
};

STATIC void
bsltest_run_test_suite(int test_suite_no)
{
    test_suite_t *ts = &test_suites[test_suite_no];

    if (ts->func != NULL) {
        bsl_printf("** Starting test suite #%d (%s)\n",
                   test_suite_no, (ts->descr != NULL) ? ts->descr : "<nodesc>");
        test_suites[test_suite_no].func();
    } else {
        bsl_printf("** Test suite #%d undefined (skipping)\n", test_suite_no);
    }
}

/****************************************
 * Main Entry
 */

int
bsltest_run(int test_suite_no)
{
    int idx;

    if (test_suite_no < 0) {
        /* Run all */ 
        for (idx = 0; idx < COUNTOF(test_suites); idx++) {
            if (test_suites[idx].descr == NULL) {
                continue;
            }
            bsltest_run_test_suite(idx);
        }
        return 0;
    } else if (test_suite_no < COUNTOF(test_suites)) {
        bsltest_run_test_suite(test_suite_no);
        return 0;
    }
    bsl_printf("Invalid test suite #%d\n", test_suite_no);
    return -1;
}

void
bsltest_list(void)
{
    int idx;
    const char *descr;

    bsl_printf("Available test suites:\n");
    for (idx = 0; idx < COUNTOF(test_suites); idx++) {
        descr = test_suites[idx].descr;
        if (descr != NULL) {
            bsl_printf("%-3d: %s\n", idx, descr);
        }
    }
}
