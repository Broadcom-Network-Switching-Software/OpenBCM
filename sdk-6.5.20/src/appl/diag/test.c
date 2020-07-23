/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        test.c
 * Purpose:     entry point for all tr related routines
 */

#ifdef INCLUDE_TEST

#include <shared/bsl.h>

#include <sal/types.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>
#include <soc/debug.h>
#include <soc/drv.h>

#include <appl/diag/system.h>
#include <appl/diag/test.h>
#include <appl/diag/progress.h>

#ifndef NO_CTRL_C
#include <setjmp.h>
#endif

#ifdef __KERNEL__
#define atoi _shr_ctoi
#endif

/*
 * Variable:    test_XXX_usage
 * Purpose:     usage strings for various tests.
 */

#define TEST_ID_USAGE   \
    "\"Test ID\" is a test #, test name, or \"*\"\n"

char test_list_usage[] = 
    "Parameters: [Test ID] ...\n\t"
    "Display the tests specified by [Test ID] in table format,\n\t"
    "A=active, S=selected, # Runs = number times test dispatched, \n\t"
    "# Pass = number times test completed successfully, \n\t"
    "# Fail = number times test completed with a failure\n\t"
    TEST_ID_USAGE;

static  test_t  *test_active = NULL;    /* Pointer to active test */
static  sal_thread_t test_thread = NULL; /* Pointer to test thread */

/*
 * variables:   test_active_jmp
 * Purpose:     Long jump buffer used to return from a running
 *              test such as "test_exit" call.
 */
#ifndef NO_CTRL_C
static  jmp_buf test_active_jmp;                /* test_exit Long jump */
#endif
/*
 * Defines:     TEST_XXX
 * Purpose:     Return values from longjmp indicating action to 
 *              take (also return value from some of the "run"
 *              routines.
 *
 *      TEST_RUNNING - (0), normal return from longjmp
 *      TEST_ABORT   - (1), abort current test, continue running 
 *                      test sequence.
 *      TEST_STOP    - (2), abort current test, AND test sequence.
 *      TEST_CONTINUE- (3), continue running test and sequence
 *      TEST_INTR    - (4), test aborted by CTRL-C
 */
#define TEST_RUNNING    0
#define TEST_ABORT      1
#define TEST_STOP       2
#define TEST_CONTINUE   3
#define TEST_INTR       4

#define         TEST_O_SOE      0x01    /* Stop on Error */
#define         TEST_O_AOE      0x02    /* Abort on Error */
#define         TEST_O_BATCH    0x04    /* Batch Mode */
#define         TEST_O_RANDOM   0x08    /* Random Mode */
#define         TEST_O_PROGRESS 0x10    /* Print test progress */
#define         TEST_O_SILENT   0x20    /* Don't print test error msgs */
#define         TEST_O_RUN      0x40    /* Print msg when test run */
#define         TEST_O_OVERRIDE 0x80    /* Override Chips */
#define         TEST_O_QUIET    0x100   /* Don't print messages */
#define         TEST_O_NO_REINIT 0x200   /* No reinit */

static  uint32  test_options = 0;       /* Run options */
static  int last_test_status[SOC_MAX_NUM_DEVICES] = {0};       /* Store the last test runs status */

/*
 * Approach changed from 1 global testlist to dynamic list assignment for whatever reason one chose
 * It may be SOC_IS based as in case of Jericho2/Ramon, or any other one
 * So this routine allows caller to assigns test_list and test_cnt local values instead of global ones
 */
static test_t *get_test_list(int unit, int *test_cnt_p)
{
#if defined(BCM_SAND_SUPPORT)
    /* For now only Jericho 2 and Ramon will utilize sand_test_list, but we may consider to moving all the dune
     * chip families due to lack of bits in common_test_list
     */
    if(SOC_IS_DNX(unit) || SOC_IS_DNXF(unit))
    {
        *test_cnt_p = sand_test_cnt;
        return sand_test_list;
    }
    else
#endif
    {
        *test_cnt_p = common_test_cnt;
        return common_test_list;
    }
}

int
test_is_no_reinit_mode(int unit) 
{
    return ((test_options & TEST_O_NO_REINIT) > 0);
}

int
test_get_last_test_status(int unit) 
{
    assert((unit < SOC_MAX_NUM_DEVICES) && (unit >= 0));
    return last_test_status[unit];
}

STATIC uint32
_test_chip(int unit)
/*
 * Function:    _test_chip
 * Purpose:     Return current T56xx mask based on unit #.
 * Parameters:  Unit - Strata Swtich unit #.
 * Returns:     Mask.
 */
{
    if (!soc_attached(unit)) {
        return(0);
    }
    switch (SOC_CHIP_GROUP(unit)) {
#if defined (BCM_SAND_SUPPORT)
    case SOC_CHIP_BCM88650:     return(T88650);
    case SOC_CHIP_BCM88660:     return(T88650);
    case SOC_CHIP_BCM88675:     return(T88675);
    case SOC_CHIP_BCM88375:     return(T88675);
    case SOC_CHIP_BCM88680:     return(T88675);
    case SOC_CHIP_BCM88470:     return(T88675);
    case SOC_CHIP_BCM83208:     return(T88675);
    case SOC_CHIP_BCM88202:     return(T88650);
    case SOC_CHIP_BCM88750:     return(T88750);
    case SOC_CHIP_BCM88270:     return(T88675);
	case SOC_CHIP_BCM88950:		return(T88750);
    case SOC_CHIP_BCM88952:     return(T88750);
    case SOC_CHIP_BCM88752:     return(T88750);
/* Starting from Jericho 2 and Ramon there is another test_list with new bitmap TSxxxxx instead of Txxxxx */
    case SOC_CHIP_BCM8869X:     return(TS88690);
    case SOC_CHIP_BCM8879X:     return(TS88790);
    case SOC_CHIP_BCM8880X:     return(TS88690);
    case SOC_CHIP_BCM8848X:     return(TS88690);
    case SOC_CHIP_BCM8885X:     return(TS88690);
#endif
#if defined(BCM_ESW_SUPPORT)
    case SOC_CHIP_BCM5675:      return(T5675);
    case SOC_CHIP_BCM56102:     return(T56504);
    case SOC_CHIP_BCM56304:     return(T56504);
    case SOC_CHIP_BCM56112:     return(T56504);
    case SOC_CHIP_BCM56314:     return(T56504);
    case SOC_CHIP_BCM56504:     return(T56504);
    case SOC_CHIP_BCM56514:     return(T56504);
    case SOC_CHIP_BCM56800:
	if (SOC_IS_HUMV(unit)) {
	    return(T56700);
	} else { 
	    return(T56504);
	}
    case SOC_CHIP_BCM56820:     return(T56820);
    case SOC_CHIP_BCM56725:     return(T56700);
    case SOC_CHIP_BCM56624:     return(T56624);
    case SOC_CHIP_BCM56680:     return(T56624);
    case SOC_CHIP_BCM56218:     return(T56218);
    case SOC_CHIP_BCM56224:     return(T56224);
    case SOC_CHIP_BCM53314:     return(T53314);
    case SOC_CHIP_BCM56634:     return(T56634);
    case SOC_CHIP_BCM56524:     return(T56634);
    case SOC_CHIP_BCM56685:     return(T56634);
    case SOC_CHIP_BCM56334:     return(T56334);
    case SOC_CHIP_BCM56142:     return(T56142);
    case SOC_CHIP_BCM56150:     return(T56150);
    case SOC_CHIP_BCM56160:     return(T56160);
    case SOC_CHIP_BCM53540:     return(T56160);
    case SOC_CHIP_BCM53400:     return(T53400);
    case SOC_CHIP_BCM88732:     return(T88732);
    case SOC_CHIP_BCM56840:     return(T56840);
    case SOC_CHIP_BCM56640:     return(T56640);
    case SOC_CHIP_BCM56340:     return(T56340);
    case SOC_CHIP_BCM56440:     return(T56440);
    case SOC_CHIP_BCM56450:     return(T56450);
    case SOC_CHIP_BCM56260:     return(T56260);
    case SOC_CHIP_BCM56270:     return(T56270);
    case SOC_CHIP_BCM56850:     return(T56850);
    case SOC_CHIP_BCM56860:     return(T56860);
    case SOC_CHIP_BCM56960:     return(T56960);
    case SOC_CHIP_BCM56965:     return(T56960);
    case SOC_CHIP_BCM56980:     return(T56980);
    case SOC_CHIP_BCM56560:     return(T56560);
    case SOC_CHIP_BCM56870:     return(T56870);
    case SOC_CHIP_BCM53570:     return(T53570);
    case SOC_CHIP_BCM56070:     return(T56070);
    case SOC_CHIP_BCM56970:     return(T56970);
    case SOC_CHIP_BCM56770:     return(T56770);
    case SOC_CHIP_BCM56370:     return(T56370);
    case SOC_CHIP_BCM56275:     return(T56275);
    case SOC_CHIP_BCM56670:     return(T56670);
    case SOC_CHIP_BCM56470:     return(T56470);
#endif
    default:                    return(0);
    }
}

static test_t *
test_find(char *key, test_t *test_list, int test_cnt)
/*
 * Function:    test_find
 * Purpose:     Location a matching entry in the TEST test list.
 * Parameters:  key - key to lookup name on
 * Returns:     Pointer to entry if found, NULL if no match
 * Notes:       If key is all digits (0-9), it is assumed top be a test
 *              number otherwise it is a test name.
 */
{
    test_t      *test = NULL;
    int         ki, kl;                 /* Key integer/length */
    int         i;

    if (isint(key)) {                   /* Locate on test number */
        ki = atoi(key);
        for (i = 0; !test && (i < test_cnt); i++) {
            if (test_list[i].t_test == ki) {
                test = &test_list[i];
            }
        }
    } else {                            /* Locate Based on name */
        kl = strlen(key);
        for (i = 0; !test && (i < test_cnt); i++) {
            if (!sal_strncasecmp(key, test_list[i].t_name, kl)) {
                test = &test_list[i];
            }
        }
    }
    return(test);
}

void
test_done(int u, test_t *test, int status)
/*
 * Function:    test_done
 * Purpose:     Process the completion of a test
 * Parameters:  u - unit number.
 *              test - pointer to test completed.
 *              status - exit status of test.
 * Returns:     Nothing.
 */
{
    if (status || (test->t_flags & T_F_ERROR)) {
        last_test_status[u] = -1;
        test->t_fail++;
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(u,
                             "Test %d (%s) Completed with error (%d)\n"), 
                  test->t_test, test->t_name, status));
        /*
         * If Stop-on-error not processed yet, give user a chance
         * now.
         */
        if ((status == TEST_ABORT) || (status == TEST_STOP)) {
            cli_out("Test %d (%s) Aborted\n", test->t_test, test->t_name);
        } else if ((test_options & TEST_O_SOE) && 
                   !(test->t_flags & T_F_STOP)) {
            test_error(u, 
                       "Stop-on-error: Test %d (%s) completed with error\n",
                       test->t_test, test->t_name);
        }
    } else {
        last_test_status[u] = 0;
        test->t_success++;
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(u,
                             "Test %d (%s) Completed successfully\n"), 
                  test->t_test, test->t_name));
    }
}

void
test_exit(int u, int status)
/*
 * Function:    test_exit
 * Purpose:     To exit the currently running test.
 * Parameters:  status - the exit status, 0 for success, -1 for failure.
 * Returns:     Does not return
 */
{
    test_done(u, test_active, status);
#ifndef NO_CTRL_C
    longjmp(test_active_jmp, 1);
#endif
}

int
test_test_init(int u, test_t *test, args_t *a, void **fp)
/*
 * Function:    test_test_init
 * Purpose:     Run initialization scripts and routines for a specified test.
 * Parameters:  u - unit #
 *              test - pointer to test description.
 *              a - pointer to arguments.
 *              fp - cookie passed back to test routine. 
 * Returns:     0 - success
 *              -1 - failed.
 */
{
    int rv = 0;

    *fp = NULL;
    if (sh_set_rctest && (test->t_flags & T_F_RC)) {
        var_set_integer("testinit", test->t_test, 0, 0);
#if defined(BCM_ESW_SUPPORT)
#ifndef NO_SAL_APPL
        if (SOC_IS_XGS(u)) {
            if (diag_rc_load(u) != CMD_OK) {
                cli_out("Test: ERROR: RC init script for test %d (%s) failed\n",
                        test->t_test, test->t_name);
                rv = -1;
            }
        }
#endif
#endif
        var_unset("testinit", 0, 1, 0);
        if (rv) {
            return(rv);
        }
    }
    if (test->t_init_f) {               /* Call init function */
         rv = test->t_init_f(u, a, fp);
        /* check if return value is != BCM_E_UNAVAIL because the memory test on the internal
         * memories will return BCM_E_UNAVAIL if External TCAM is present. Those memories shouldn't
         * be tested instead should be skipped. */
        if (rv && (rv != BCM_E_UNAVAIL)) {
            cli_out("Test: ERROR: Init function for test %d (%s) failed\n",
                    test->t_test, test->t_name);
            rv = -1;
        }
    }
    return(rv);
}

/*
 * Function:    test_test_done
 * Purpose:     Run done scripts and routines for a specified test.
 * Parameters:  u - unit #
 *              test - pointer to test description.
 *              fp   - value returned from init function.
 * Returns:     0 - success
 *              -1 - failed.
 */
int
test_test_done(int u, test_t *test, void *fp)
{
    int rv = 0;

    if (test->t_done_f) {               /* Call done function */
        if (test->t_done_f(u, fp)) {
            cli_out("Test: ERROR: Done function for test %d (%s) failed\n",
                    test->t_test, test->t_name);
            rv = -1;
        }
    }
    test->t_flags &= ~(T_F_ACTIVE|T_F_STOP|T_F_ERROR);
    test_active = NULL;
    test_thread = NULL;
    if (test_options & TEST_O_RUN) {
        cli_out("Test %d (%s) Completed\n", test->t_test, test->t_name);
    }

    return(rv);
}

int
test_dispatch(int u, test_t *test, int loops, args_t *a)
/*
 * Function:    test_dispatch
 * Purpose:     Run a selected test.
 * Parameters:  test - Pointer to test entry to run.
 *              loops - # of iterations, if -1 count taken from test default.
 *              a - arguments to pass to test.
 * Returns:     TEST_XXX
 */
{
    jmp_buf     jmp;                    /* ^C out of tests must be trapped */
    int         i;                      /* Loop counter */
    int         error;                  /* No error */
    int         arg_saved;
    volatile int vloops = loops;        /* "loops"/setjmp clobber war */
    args_t      * volatile av = a;
    args_t      * volatile args = 0;
    void        * volatile fp;          /* Test function pointer parameter */
    volatile    int cleanup = FALSE;
    int rv;
    /* Check if test supported */

    if (!(test->t_flags & _test_chip(u))) {
        if (test_options & TEST_O_OVERRIDE) {
            cli_out("Warning: Running test %d (%s) not supported on %s\n", 
                    test->t_test, test->t_name, SOC_UNIT_GROUP(u));
        }
    }

    if (test_options & TEST_O_RUN) {
        cli_out("Test %d (%s) Started\n", test->t_test, test->t_name);
    }

    COMPILER_REFERENCE(cleanup);
    error = TEST_RUNNING;

#ifndef NO_CTRL_C
    if (TEST_RUNNING != (error = setjmp(jmp))) { /* Control C handler */
        if (test_active) {
            if (cleanup) {
                cli_out("Warning: Cleanup aborted - "
                        "continue at your own risk\n");
            } else {
                cleanup = TRUE;
                cli_out("Warning: cleaning up active test: %s\n", 
                        test_active->t_name);
                test->t_flags |= T_F_STOP;      /* Say processed stop */
                test_done(u, test_active, error);
                test_test_done(u, test_active, fp);
                if (args) {
                    sal_free(args);
                }
            }
            sh_pop_ctrl_c();
            test_thread = NULL;
            test_active = NULL;
            return(TEST_INTR);
        }
    }
#endif

    sh_push_ctrl_c(&jmp);
    test_thread = sal_thread_self();

    /* If no arguments passed in, use default args */

    error = TEST_ABORT;
    if (!av) {
        if ((av = args = sal_alloc(sizeof(args_t), "test_args")) == NULL) {
            goto return_no_free;
        }

        if (diag_parse_args(test->t_override_string ? 
                       test->t_override_string : test->t_default_string, 
                       NULL,
                       av)) {
            test->t_fail++;
            last_test_status[u] = -1;
            goto return_with_free;
        }
    }
    fp = NULL;

    if (-1 == vloops) {                 /* Assign AFTER setjmp */
        vloops = test->t_loops;         /* Use default */
    } 

    test_active = test;
    test->t_flags |= T_F_ACTIVE;
    arg_saved = av->a_arg;              /* Some may be consumed */
   
    rv = test_test_init(u, test, av, (void **)&fp);
    /* check if return value is != BCM_E_UNAVAIL because the memory test on the internal
     * memories will return BCM_E_UNAVAIL if External TCAM is present. Those memories shouldn't
     * be tested instead should be skipped. */
    if ((rv != BCM_E_UNAVAIL) && (rv)){
        test->t_runs++;                 /* Increment RUN/FAIL count */
        test->t_fail++;
        last_test_status[u] = -1;
        test->t_flags &= ~T_F_ACTIVE;
        test_active = NULL;
        test_thread = NULL;
        goto return_with_free;
    }

#ifndef NO_CTRL_C
    if (TEST_RUNNING != (error = setjmp(test_active_jmp))) {
        if (cleanup) {
            cli_out("Warning: Cleanup aborted - "
                    "continue at your own risk\n");
        } else {
            cleanup = TRUE;
            test_done(u, test, error);
        }
    } else {
#endif
        /* If progress reporting requested, set it up ... */
        if (test_options & TEST_O_PROGRESS) {
            progress_init(vloops, 0, FALSE);
            progress_status(test->t_name);
        }
        for (i = 0; i < vloops; i++) {
            if (vloops > 1) {
                LOG_VERBOSE(BSL_LS_APPL_TESTS,
                            (BSL_META_U(u,
                                        "Test %d: %s.  Starting interation %d.\n"),
                             test->t_test, test->t_name, i+1));
            }
            test->t_runs++;             /* Let-em know we ran */
            av->a_arg = arg_saved;      /* Reset ARG pointer */
            test->t_flags &= ~(T_F_STOP|T_F_ERROR);
            /* Skip if rv is BCM_E_UNAVAIL as we mentioned above */
            if (rv != BCM_E_UNAVAIL) {
                error = test->t_test_f(u, av, fp);/* Run Test */
                /*
                 * Store the result of the last test in the local, BCM shell, variable $last_tr_failed
                 */
                (void)var_set_integer("last_tr_failed", error, TRUE, FALSE);
            } else {
                error = 0;
            }
            if (error == 0 && test->t_flags & T_F_ERROR) {
                /* Some tests call test_error but fail to return error */
                error = -1;
            }
            test_done(u, test, error);
            if (test_options & TEST_O_PROGRESS) {
                progress_report(1);
            }
        }
        if (test_options & TEST_O_PROGRESS) {
            progress_done();
        }
#ifndef NO_CTRL_C
    }
#endif

            /* Skip if rv is BCM_E_UNAVAIL as we mentioned above */
            if (rv != BCM_E_UNAVAIL) {
               (void)test_test_done(u, test, fp);  /* Run done routines/scripts */ 
            } else {
                  /* If Return Value is BCM_E_UNAVAIL it is assumed that the internal
                   * memories are being tested for External TCAm devices and the test has
                   * not supported these memories. Even though the test is not executed
                   * increasing the test success count in order to show up in the final test
                   * results as not failed.
 */
                   test_active = NULL;
                   test_thread = NULL;
                   test->t_success++;
              }
 return_with_free:
    if (args != NULL) {
        sh_block_ctrl_c(TRUE);
        sal_free(args);
        args = 0;
        sh_block_ctrl_c(FALSE);
    }

 return_no_free:
    sh_pop_ctrl_c();

    if (test_options & TEST_O_RUN) {
        cli_out("Completed test (%d) %s\n", test->t_test, test->t_name);
    }

    if (a != NULL)
        ARG_DISCARD(a);

    return(error);
}

void
test_msg(const char *fmt, ...)
/*
 * Function:    test_msg
 * Purpose:     Print an informational message from a test.
 * Parameters:  Printf style string.
 * Returns:     Nothing
 */
{
    va_list     varg;

    va_start(varg, fmt);

    if (!(test_options & TEST_O_QUIET)) {
        (void)bsl_vprintf(fmt, varg);
    }

    va_end(varg); 
}

void
test_error(int u, const char *fmt, ...)
/*
 * Function:    test_error
 * Purpose:     Print an Error Message for test failure, stopping if
 *              Stop-on-Error is requested.
 * Parameters:  Printf style string.
 * Returns:     Nothing
 */
{
    char        pfx[64];
    va_list     varg;
    int         o_spl;

    va_start(varg, fmt);

    if (sal_thread_self() != test_thread) {
        /*
         * If this is not the test thread itself, then longjump
         * will not work - so only option is to continue.       
         */
        cli_out("\n"
                "****************************************\n"
                "TEST ERROR: Warning: Not in test thread, "
                "continuing ...\n");
        (void)bsl_vprintf(fmt, varg);
        cli_out("****************************************\n");
        va_end(varg); 

        return;
    }

    o_spl = sal_splhi();
    test_active->t_flags |= T_F_ERROR;  /* Indicate error seen */
    sal_spl(o_spl);

    if (!(test_options & TEST_O_SILENT)) {
        cli_out("\n"
                "****************************************\n"
                "TEST ERROR %s:\n", test_active->t_name);
        (void)bsl_vprintf(fmt, varg);
        cli_out("****************************************\n");
    }

    if (test_options & TEST_O_SOE) {    /* Stop on error? */
        o_spl = sal_splhi();
        test_active->t_flags |= T_F_STOP; /* Indicate stopped once */
        sal_spl(o_spl);
        while (1) {
            char *c, b[64];
            int bl;
            cli_out("\n\n *** Stop-on-Error *** \n"
                    "\t\"bcm\" to enter recursive BCM shell\n"
                    "\t\"continue\" to continue running current test\n\n"
                    "\t\"stop\" to stop all tests\n"
                    "\t\"abort\" to abort current test and start next test\n");
            c = sal_readline("Stop-on-Error action? ", b, sizeof(b), "bcm");
            if (c) {
                b[sizeof(b) - 1] = '\0';        /* Be sure null terminated */
                bl = strlen(c);
                if (!sal_strncasecmp("bcm", b, bl)) { 
                    cli_out("\nType \"exit\" to return from sh\n\n");
                    sal_sprintf(pfx, "FAILED(%s)", test_active->t_name);
                    (void)sh_process(u, pfx, TRUE);
                } else if (!sal_strncasecmp("stop", b, bl)) {
#ifndef NO_CTRL_C
                    longjmp(test_active_jmp, TEST_STOP);
#endif
                } else if (!sal_strncasecmp("abort", b, bl)) {
#ifndef NO_CTRL_C
                    longjmp(test_active_jmp, TEST_ABORT);
#endif
                } else if (!sal_strncasecmp("continue", b, bl)) {
                    va_end(varg);     
                    return;
                }
            }
        }
    } else if (test_options & TEST_O_AOE) { /* Abort on error */
        cli_out("\nAborting test\n");
#ifndef NO_CTRL_C       
        longjmp(test_active_jmp, TEST_ABORT);
#endif
    }
        
    va_end(varg); 
    return;                             /* Continue */
}

void
test_print_header(int status_report)
/*
 * Function:    test_print_header
 * Purpose:     Print header string for output.
 * Parameters:  None.
 * Returns:     Nothing
 */
{
    if (status_report) {
        cli_out("-----+----+--- TEST STATUS REPORT ----+"
                "-----+-----+-----+-----+-----------\n");
    }

    cli_out("U/A/S|Test|            Test           |Loop | Run |Pass |Fail |"
            "  Arguments\n");
    cli_out("     | #  |            Name           |Count|Count|Count|Count|"
            "\n");
}

void
test_print_separator(void)
/*
 * Function:    test_print_header
 * Purpose:     Print header string for output.
 * Parameters:  None.
 * Returns:     Nothing
 */
{
    cli_out("-----+----+---------------------------+-----+-----+-----+-----+"
            "-----------\n");
}

void
test_print_summary(int loops, int runs, int successes, int failures)
/*
 * Function:    test_print_summary
 * Purpose:     Print summary values
 * Parameters:  Summary values
 * Returns:     Nothing
 */
{
    cli_out("     |    | %-26s|%5d|%5d|%5d|%5d|\n",
            "TOTAL", loops, runs, successes, failures);
}

void
test_print_entry(int unit, test_t *test)
/*
 * Function:    test_print_entry
 * Purpose:     Print a single test entry in a pretty formatted way.
 * Parameters:  unit - Strata Switch unit #
 *              test - pointer to test list entry to print
 * Returns:     Nothing
 */
{
    int         selected;
    uint32      chip = _test_chip(unit);

    selected = (test->t_flags & T_F_SEL_ALL) || 
               ((test->t_flags & T_F_SEL_CHIP) && (test->t_flags & chip));
    cli_out(" %c%c%c |%4d| %-26s|%5d|%5d|%5d|%5d| %s\n", 
            test->t_flags & _test_chip(unit) ? ' ' : 'U',
            test->t_flags & T_F_ACTIVE ? 'A' : ' ',
            selected ? 'S' : ' ',
            test->t_test, test->t_name, test->t_loops, test->t_runs, 
            test->t_success, test->t_fail, 
            test->t_override_string ? 
            test->t_override_string : 
            test->t_default_string ? test->t_default_string : "(none)");
}

cmd_result_t
test_print_list(int u, args_t *a)
/*
 * Function:    test_print_list
 * Purpose:     Print some/all of the tests and status.
 * Parameters:  u - unit #
 *              "*" for all tests
 *              List of tests
 *              Nothing for all (same as *)
 * Returns:     CMD_OK - success
 *              CMD_FAIL - failed to find test or if a test failed.
 */
{
    test_t      *test;
    char        *s;
    int         header = FALSE;
    int         i, supported, all, fail;
    cmd_result_t result = CMD_OK;
    uint32      chip = _test_chip(u);
    int         num_fail = 0;

    test_t     *test_list;
    int        test_cnt;
    test_list = get_test_list(u, &test_cnt);

    COMPILER_REFERENCE(u);

    /* Check for print all */

    if (ARG_CNT(a) > 0) {
        supported = (0 == strcmp(_ARG_CUR(a), "*"));
        all       = (0 == sal_strcasecmp(_ARG_CUR(a), "all"));
        fail = (0 == sal_strcasecmp(_ARG_CUR(a), "fail"));
        if (supported || all || fail) {         /* Consume Argument  */
            ARG_NEXT(a);
        }
    } else if (ARG_CNT(a) == 0) {
        supported = TRUE;
        all       = FALSE;
        fail   = FALSE;
    } else {
        supported = FALSE;
        all       = FALSE;
        fail   = FALSE;
    }   

    /* If "all" or supported, consume argument and list tests */

        if (fail) {
            for (i = 0; i < test_cnt; i++) {
                if (test_list[i].t_fail) {
                    cli_out("%d  ", test_list[i].t_test);
                    num_fail++;
                }
            }
            if (num_fail == 0) {
                    cli_out("All tests passed");
            }
            cli_out("\n");
            return(result);
        }

    if (all || supported) {
        int t_loops = 0, t_runs = 0, t_success = 0, t_fail = 0;
        if (chip == 0) {                /* No chip - print all */
            all = TRUE;
        }
        test_print_header(TRUE);
        test_print_separator();
        for (i = 0; i < test_cnt; i++) {
            if (all || 
                (supported && (chip & test_list[i].t_flags))
                || (test_list[i].t_flags & T_F_SEL_ALL)) {
                test_print_entry(u, &test_list[i]);
                t_loops += test_list[i].t_loops;
                t_runs += test_list[i].t_runs;
                t_success += test_list[i].t_success;
                t_fail += test_list[i].t_fail;
            }
        }
        test_print_summary(t_loops, t_runs, t_success, t_fail);
        test_print_separator();

        /*
          In order for the automated tests to automatically diagnose
          that a test failed, we need this function to return 
          something when an individual test fails.
         */
        if (t_runs == 0) {
            /*
             * We want return CMD_FAIL if no tests were run 
             */
            result = CMD_FAIL;
        } else if (t_fail > 0) {
            /*
             * We also want to return CMD_FAIL if any tests failed.
             */
            result = CMD_FAIL;
        }
        return(result);
    } 

    while ((s = ARG_GET(a)) != NULL) {
        test = test_find(s, test_list, test_cnt);
        if (!test) {
            cli_out("%s: Unable to locate test: %s\n", ARG_CMD(a), s);
            result = CMD_FAIL;
            continue;
        } 
        if (!header) {
            test_print_header(FALSE);
            test_print_separator();
            header = TRUE;
        }
        test_print_entry(u, test);
    }
    return(result);
}

/*
 * Function:    test_run_selected
 * Purpose:     Run selected tests.
 * Parameters:  u - unit number to run tests on.
 * Returns:     Nothing.
 */
static int
test_run_selected(int u, test_t *test_list, int test_cnt)
{
    int         i;

    if (test_options & TEST_O_RANDOM) {
        cli_out("Test: Random mode not supported yet\n");
        return(TEST_STOP);
    }

    for (i = 0; i < test_cnt; i++) {
        if ((test_list[i].t_flags & T_F_SEL_ALL) ||
            ((test_list[i].t_flags & T_F_SEL_CHIP) &&
             (test_list[i].t_flags & _test_chip(u))))
        {
            int         rv;
            switch((rv = test_dispatch(u, &test_list[i], -1, NULL))) {
            case TEST_STOP:
            case TEST_INTR:
                return(rv);
            }
        }
    }
    return(TEST_RUNNING);
}

char test_run_usage[] = 
    "Parameters: [Test ID] [arguments]\n\t"
    "   With no parameters runs all selected tests with the set\n\t"
    "   loop counts and set parameters.  Otherwise, runs a specific\n\t"
    "   test with the specified loop count and arguments.  You may\n\t"
    "   want to use \"set RCTest=true\" so that some tests (like\n\t"
    "   loopback) will properly initialize using rc.soc).\n";

cmd_result_t
test_run(int u, args_t *a)
/*
 * Function:    test_run
 * Purpose:     Perform "run" command for test.
 * Parameters:  none - Run all selected tests with loaded parameters.
 *              "test [options]"
 * Returns:     Test result.
 */
{
    test_t      *test;
    int         rv;
    int         loops = -1;

    test_t     *test_list;
    int        test_cnt;
    test_list = get_test_list(u, &test_cnt);

    if (test_active && !SOC_IS_SAND(u)) {
        cli_out("%s: Already running: %s\n", ARG_CMD(a), test_active->t_name);
        return(CMD_FAIL);
    } else if (!sh_check_attached(ARG_CMD(a), u)) {
        return(CMD_FAIL);
    }

    if (!ARG_CNT(a)) {
        rv = test_run_selected(u, test_list, test_cnt);
    } else {
        if (NULL == (test = test_find(ARG_CUR(a), test_list, test_cnt))) {
            cli_out("%s: Unknown test: %s\n", ARG_CMD(a), ARG_CUR(a));
            return(CMD_FAIL);
        }

        if (!(test->t_flags & _test_chip(u)) && 
            !(test_options & TEST_O_OVERRIDE)) {
            cli_out("Error: Test %d (%s) not supported on %s\n"
                    "Error: use \"testmode +override\" to override and run\n",
                    test->t_test, test->t_name, SOC_UNIT_GROUP(u));
            return(CMD_FAIL);
        }           
        ARG_NEXT(a);                    /* Move arg pointer */
        /* If there is an argument which is an integer, it's a loop count */
        if (ARG_CUR(a) && isint(ARG_CUR(a))) {
            loops = strtoul(_ARG_GET(a), NULL, 10);
        }
        rv = test_dispatch(u, test, loops, ARG_CNT(a) ? a : NULL);
    }
    return(rv == TEST_INTR ? CMD_INTR : rv);
}

STATIC int
_test_select(int u, test_t *t, int only_if_available)
{
    uint32      chip = _test_chip(u);

    if (only_if_available) {            /* Only select available tests */
        if (t->t_flags & chip) {
            t->t_flags |= T_F_SEL_CHIP;
        }
    } else {
        if (!(t->t_flags & chip)) {
            if (test_options & TEST_O_OVERRIDE) {
                cli_out("Warning: Selecting test %d (%s) "
                        "not supported on %s\n", 
                        t->t_test, t->t_name, SOC_UNIT_GROUP(u));
            } else {
                cli_out("Error: Test %d (%s) not supported on %s\n"
                        "use \"testmode +override\" to override and run\n",
                        t->t_test, t->t_name, SOC_UNIT_GROUP(u));
                return(CMD_FAIL);
            }
        }
        t->t_flags |= T_F_SEL_ALL;
    }
    return(CMD_OK);
}

STATIC void
_test_unselect(int u, test_t *t)
{
    uint32      chip = _test_chip(u);

    t->t_flags &= ~T_F_SEL_ALL;
    if (t->t_flags & chip) {
        t->t_flags &= ~T_F_SEL_CHIP;
    }
}    

STATIC int
_test_select_check(int u, test_t *t)
{
    uint32      chip = _test_chip(u);

    return((t->t_flags & T_F_SEL_ALL) || 
           ((t->t_flags & T_F_SEL_CHIP) && (t->t_flags & chip)));
}

char test_select_usage[] = 
    "Parameters: [+|-][Test ID] ... - Select specified tests\n\t"
    "    If + specified before a test it is added to the\n\t"
    "    run list. If - is specified it is removed from\n\t"
    "    the run list. If neither + or - is specified then\n\t"
    "    the current select status for a test is inverted\n\t"
    TEST_ID_USAGE;

cmd_result_t
test_select(int u, args_t *a)
/*
 * Function:    test_select
 * Purpose:     Select a set of tests, prefix with + to add, - to remove.
 * Parameters:  u - unit #
 *              a - list of tests to select, deselect (* means all)
 * Returns:     0 - success, -1 failed.
 */
{
    enum {ADD, DEL, XOR} option;
    int         silent;
    cmd_result_t error = CMD_OK;
    char        *s;
    test_t      *ts, *te;               /* Test start/end pointers */

    test_t     *test_list;
    int        test_cnt;
    test_list = get_test_list(u, &test_cnt);

    COMPILER_REFERENCE(u);

    while ((s = ARG_GET(a)) != NULL) {
        if ('+' == *s) {
            option = ADD;
            s++;
        } else if ('-' == *s) {
            option = DEL;
            s++;
        } else {
            option = XOR;
        }
            
        if (!strcmp("*", s)) {          /* Do it to all of the tests */
            ts = &test_list[0];
            te = &test_list[test_cnt-1];
            silent = TRUE;
        } else {
            silent = FALSE;
            ts = te = test_find(s, test_list, test_cnt);
            if (!ts) {
                cli_out("%s: Unable to locate test: %s\n", ARG_CMD(a), s);
                return(CMD_FAIL);
            }
                
        }
        while (ts <= te) {
            switch(option) {
            case ADD:
                error = _test_select(u, ts, silent);
                break;
            case DEL:
                _test_unselect(u, ts);
                break;
            case XOR:
                if (_test_select_check(u, ts)) {
                    _test_unselect(u, ts);
                } else {
                    error = _test_select(u, ts, silent);
                }
                break;
            }
            ts++;
        }
    }
    return(error);
}

char test_mode_usage[] = 
    "Parameters: [+/-][StopOnError|Random|Progress|SIlent|Run|Override] ... \n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "\t   - StopOnError requests a diagnostic shell be invoked when\n\t"
    "   a test fails. This allows the user to inspect SOC state\n\t"
    "   using the diagshell.\n\t"
    "   - Abortonerror requests a test be aborted on failure, and \n\t"
    "   execution continues with next test.\n\t"
    "   - Random requests tests be run in pseudo random order\n\t"
    "   - Progress requests a simple progress report be displayed\n\t"
    "   as tests are run\n\t"
    "   - Silent requests test error messages not be printed, but\n\t"
    "   errors are counted\n\t"
    "   - Run requests a message is printed when a test is dispatched\n\t"
    "   Prefixing the flag with + turns on the option, - turns\n\t"
    "   off the option, no +/- toggles the current option\n\t"
    "   - Override allows tests not supported on the specified chip to\n\t"
    "   be selected and run. WARNING: these is a high probablility the\n\t"
    "   system will crash when running unsupported chips\n"
#endif
    ;

cmd_result_t
test_mode(int u, args_t *a)
/*
 * Function:    test_mode
 * Purpose:     Set test test mode
 * Parameters:  [+/-] "soe" or "stoponerror"
 * Returns:     0 - success, 
 *              -1 - failed.
 */
{
    static      parse_pm_t options[] = {
        {"@SOE",        TEST_O_SOE},
        {"@AOE",        TEST_O_AOE},
        {"Stoponerror", TEST_O_SOE},
        {"Abortonerror",TEST_O_AOE},
        {"Quiet",       TEST_O_QUIET},
        {"Progress",    TEST_O_PROGRESS},
        {"RANDom",      TEST_O_RANDOM},
        {"SIlent",      TEST_O_SILENT},
        {"Run",         TEST_O_RUN},
        {"Override",    TEST_O_OVERRIDE},
        {"NoReinit",    TEST_O_NO_REINIT},
        {NULL,          0}
    };
    char        *c;

    COMPILER_REFERENCE(u);

    if (0 == ARG_CNT(a)) {
        cli_out("Test: Options: ");
        parse_mask_format(80, options, test_options);
        cli_out("\n");
        return(CMD_OK);
    }

    while ((c = ARG_GET(a)) != NULL) {
        if (parse_mask(c, options, &test_options)) {
            cli_out("%s: Error: invalid option ignored: %s\n", ARG_CMD(a), c);
        }
    }
    return(CMD_OK);
}

char test_parameters_usage[] =
    "Parameters: [Test Name | Test #] [\"Parameter String\"] ...\n\t"
    "    Override the default parameters for the test specified\n\t"
    "    by [Test ID]. Multiple tests may be listed and the parameter\n\t"
    "    string must be quoted if more that 1 argument is specified (\").\n\t"
    "    To clear a previously set parameter list, pass a NULL string\n\t"
    "    as an argument (\"\").\n";

cmd_result_t
test_parameters(int u, args_t *a)
/*
 * Function:    test_parameters
 * Purpose:     Set the parameters for the specified tests.
 * Parameters:  u - unit number
 *              a - arguments, expects tuples of the form:
 *                      [Test name/#] Arguments.
 *              if arguments is null string ("") then previously
 *              set arguments are discarded and defaults used.
 * Returns:     cmd_result_t.
 */
{
    char        *tid, *p;               /* Testid, parameters */
    test_t      *t;                     /* Test pointer */

    test_t     *test_list;
    int        test_cnt;
    test_list = get_test_list(u, &test_cnt);

    COMPILER_REFERENCE(u);

    if (0 == ARG_CNT(a)) {
        return(CMD_USAGE);
    }
    
    while (NULL != (tid = ARG_GET(a))) {
        p = ARG_GET(a);                 /* Parameter */
        t = test_find(tid, test_list, test_cnt);
        if (NULL == t) {
            cli_out("%s: Error: Unable to find test: %s\n", ARG_CMD(a), tid);
            return(CMD_FAIL);
        }
        if (!p || !*p) {                /* Clear parameter */
            if (!t->t_override_string) {
                cli_out("%s: Warning: No arguments to clear for test: %s\n",
                        ARG_CMD(a), t->t_name);
                continue;
            }
            sal_free(t->t_override_string);
            t->t_override_string = NULL;
        } else {                        /* Set Arguments */
            if (t->t_override_string) {
                sal_free(t->t_override_string);
            }
            if (!(t->t_override_string = 
                  (char *)sal_alloc(strlen(p) + 1, "test_parm"))) {
                cli_out("%s: Error: Out of memory\n", ARG_CMD(a));
                return(CMD_FAIL);
            }
            sal_strncpy(t->t_override_string, p, sal_strlen(p) + 1);
            cli_out("%s: Parameter set: %s(\"%s\")\n", ARG_CMD(a), t->t_name, 
                    t->t_override_string);
        }
    }
    return(CMD_OK);
}

char test_clear_usage[] =
    "Parameters: [Test Name | Test #]\n\t"
    "Clear the run, success, and failure statistics for the test\n\t"
    "specified by [Test ID]. Multiple tests may be listed, and \"*\"\n\t"
    "may be used to represent all tests.\n";

cmd_result_t
test_clear(int u, args_t *a)
/*
 * Function:    test_clear
 * Purpose:     Clear previous test results for a test.
 * Parameters:  u - unit number
 *              a - arguments, expects testname/#.
 * Returns:     cmd_result_t
 */
{
    char        *tid;
    test_t      *t;

    test_t     *test_list;
    int        test_cnt;
    test_list = get_test_list(u, &test_cnt);

    COMPILER_REFERENCE(u);

    if (0 == ARG_CNT(a)) {
        return(CMD_USAGE);
    }

    while (NULL != (tid = ARG_GET(a))) {
        if (!strcmp(tid, "*")) {        /* Clear all tests */
            for (t = test_list; t < &test_list[test_cnt]; t++) {
                t->t_runs = 0;
                t->t_success = 0;
                t->t_fail = 0;
            }
        } else {                        /* Clear specific test */
            t = test_find(tid, test_list, test_cnt);
            if (NULL == t) {
                cli_out("%s: Error: Unable to find test: %s\n", 
                        ARG_CMD(a), tid);
                return(CMD_FAIL);
            }
            t->t_runs = 0;
            t->t_success = 0;
            t->t_fail = 0;
        }
    }
    return(CMD_OK);
}

#endif	/* INCLUDE_TEST */

typedef int _diag_test_not_empty; /* Make ISO compilers happy. */
