/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        test.h
 * Purpose:     Defines and types required for building test lists.
 */

#ifndef _DIAG_TEST_H
#define _DIAG_TEST_H

#include <soc/types.h>

#include <appl/diag/parse.h>

/* File loaded when RC initialization required */

typedef int     (*test_ifunc_t)(int, args_t *, void **);
typedef int     (*test_dfunc_t)(int, void *);
typedef int     (*test_func_t)(int, args_t *, void *);

typedef struct test_s {
    char        *t_name;                 /* Test Name */
    uint32      t_flags;                 /* Flags for test */
#   define      T_F_SEL_ALL     (1U << 0) /* Test selected for any chip */
#   define      T_F_SEL_CHIP    (1U << 1) /* Test selected for supported chip */
#   define      T_F_SELECT      (T_F_SEL_ALL | T_F_SEL_CHIP)
#   define      T_F_ACTIVE      (1U << 2) /* Currently Active */
#   define      T_F_STOP        (1U << 3) /* Stop-on-error processed */
#   define      T_F_ERROR       (1U << 4) /* Error reported this run */
#   define      T_F_RC          (1U << 5) /* Std init script should be run */

/* Following flags are used in testlist, but are part of flags */

#   define      TSEL            T_F_SEL_CHIP

/*#if defined (BCM_DFE_SUPPORT)|| defined (BCM_PETRA_SUPPORT)*/
#   define      T88650            (1U << 20)
#   define      T88675            (T88650) /* Jericho */ 
#   define      T88750            (1U << 1)
#   define      DPP_ALL           (T88650 | T88675)
#   define      DFE_ALL           (T88750)
#   define      DPP_DFE           (DPP_ALL | DFE_ALL)
/*#endif*/

#   define      T56850          (1U << 18)
#   define      T5675           (1U << 19)
#   define      T56960          (1U << 20)
#   define      T56504          (1U << 24)
#   define      T56218          (1U << 25)
#   define      T56624          (1U << 26)
#   define      T56634          (T56624)
#   define      T56334          (T56624)
#   define      T56142          (T56624)
#   define      T56150          (T56142)
#   define      T56160          (T56142)
#   define      T53400          (T56142)
#   define      T53570          (T56142)
#   define      T56070          (T56142)
#   define      T56840          (T56624)
#   define      T56640          (1U << 21) /* reusing a depricated device id */
#   define      T56340          (T56640)  /* Helix4 - replicate TR3 case for now */
#   define      T56224          (1U << 27)
#   define      T56700          (1U << 28)
#   define      T56820          (1U << 29)
#   define      T88732          (T56820)
#   define      T53314          (1U << 30)
#   define      T56440          (T56624)
#   define      T56450          (1U << 31)
#   define      T56860          (T56850)
#   define      T56560          (T56860)
#   define      T56670          (T56860)
#   define      T56260          (T56440 | T56450)
#   define      T56870          (T56960)
#   define      T56873          (T56960)
#   define      T56770          (T56870)
#   define      T56771          (T56870)
#   define      T56370          (T56870)
#   define      T56275          (T56370)
#   define      T56470          (T56370)
#   define      T56270          (T56624)
#   define      T56970          (T56960)
#   define      T56980          (T56960)
#   define      TXGS3SW         (T56504 | T56218 | T56224 | T56624 | \
                                 T56700 | T53314 | T56820 | T56640 | \
                                 T56850 | T56960 | T56450 | T56870 | \
                                 T56970 | T56770 | T56370 | T56970 | \
                                 T56275 | T56980 | T56470)
#   define      TXGS12SW        (T5675)
#   define      TXGSSW          (TXGS12SW | TXGS3SW)
#   define      TALL            (TXGSSW)

#   define      TRC             (T_F_RC)

/*
 * Major problems are lack of bits for additional devices and code complexity due to multiple ifdefs.
 * So we introduce new sand_testlist and new TR Flags serving sand_testlist[],
 * where all above are serving common_testlist,
 * First 6 bits [0-5] are general ones and used by both by common_testlist and sand_testlist
 * We are assigning bits for previous families as well. although they will not be in use until we change
 * list assignment in get_test_list(appl/diag/test.c)
 * All TS bits will be used by sand_testlist only and these list will use only these bits
 */
#   define      TS88650            (1U << 6)
#   define      TS_DPP             TS88650  /* Effectively used by all devices under BCM_PETRA_SUPPORT */
#   define      TS88750            (1U << 7)
#   define      TS_DFE             TS88750  /* Effectively used by all devices under BCM_PETRA_SUPPORT */
#   define      TS88690            (1U << 8) /* Jericho 2 only, */
#   define      TS_DNX             TS88690  /* Effectively used by all devices under BCM_DNX_SUPPORT */
#   define      TS88790            (1U << 9) /* Ramon only */
#   define      TS_DNXF            TS88790  /* Effectively used by all devices under BCM_DNXF_SUPPORT */
#   define      TS_SAND            TS_DPP | TS_DFE | TS_DNX | TS_DNXF /* All Dune chip families */
#   define      TS_DNXC            TS_DNX | TS_DNXF /* jericho 2 / Ramon */

    int         t_test;                 /* Test Number */
    int         t_loops;                /* Default # iterations */
    test_ifunc_t t_init_f;              /* Initialization routine */
    test_dfunc_t t_done_f;              /* Completion cleanup */
    test_func_t t_test_f;               /* Test function */

    char        *t_default_string;      /* Possible init string */
    char        *t_override_string;     /* Possible override init string */

    /* Counters of run statistics */

    int         t_runs;                 /* Total Number of runs */
    int         t_success;              /* # Successful completions */
    int         t_fail;                 /* # failures */
} test_t;

/*
 * Macro:       TEST
 * Purpose:     Macro to help build a test entry.
 * Parameters:  _nu - (integer) Test Number
 *                      If negative, test is not selected by default
 *              _na - (string) Test Name
 *              _f  - (integer) Test Flags (chips and RC)
 *              _fi - (function) Initialization function.
 *              _fd - (function) Termination function.
 *              _ft - (function) Actual test Function
 *              _l  - (integer) loop count, # times test
 *                      function called in one run.
 *              _arg- (string) Default argument string.
 */
#define TEST_DECL(_nu, _na, _flg, _fi, _fd, _ft, _l, _arg)      \
    {(_na), (_flg), (_nu), (_l), (_fi), \
     (_fd), (_ft), (_arg), 0, 0, 0, 0}

#define TEST(_nu, _na, _f, _fi, _fd, _ft, _l, _arg)     \
    TEST_DECL((_nu), (_na) , (_f), (_fi), (_fd), (_ft), (_l), (_arg)),

# define TUNIMP(_nu, _na, _f, _fi, _fd, _ft, _l, _arg)


extern void     test_exit(int u, int status);
extern void     test_error(int u, const char *fmt, ...)  /* May not return */
     COMPILER_ATTRIBUTE ((format (printf, 2, 3)));
extern void     test_msg(const char *fmt, ...)
     COMPILER_ATTRIBUTE ((format (printf, 1, 2)));
extern int      test_get_last_test_status(int unit);
extern int      test_is_no_reinit_mode(int unit);

/* testlist.c */

extern  test_t  common_test_list[];
extern  int     common_test_cnt;

#if defined(BCM_SAND_SUPPORT)
/* sand_test_list is defined for ALL dune chip families but used currently only for
 * devices under BCM_DNX_SUPPORT and BCM_DNXF_SUPPORT
 * We may consider to switch to this list for all
 * see get_test_list(appl/diag/test.c)
 */
extern  test_t  sand_test_list[];
extern  int     sand_test_cnt;
#endif

#endif /* _DIAG_TEST_H */
