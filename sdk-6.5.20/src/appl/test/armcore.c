/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * tr145 - ARM core test
 * Test the ARM core(s) embedded in CMICM and IPROC switches
 */

#include <shared/bsl.h>

#include <sal/types.h>
#include <sal/core/boot.h>
#include <sal/appl/sal.h>
#include <soc/drv.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <shared/bsl.h>
#include <soc/types.h>
#include <soc/debug.h>
#include <soc/shared/mos_msg_common.h>
#if defined(BCM_CMICM_SUPPORT) || defined(BCM_IPROC_SUPPORT)
#include <soc/uc_msg.h>
#endif

#include <bcm/error.h>

#include "testlist.h"
#include <appl/diag/system.h>
#include <appl/diag/progress.h>

#if defined(BCM_CMICM_SUPPORT) || defined(BCM_IPROC_SUPPORT)

/* Return codes for TEST_RUN calls */
#define MOS_TEST_RC_OK                  0
#define MOS_TEST_RC_FAILURE             0xffffffff
#define MOS_TEST_RC_INVALID             0xfffffffe

typedef struct arm_core_work_s {
    int         unit;                   /* unit number */
    int         uc_num;                 /* core # to test */
    uint32      tests;                  /* bitmap of tests to perform */
} arm_core_work_t;

static uint32 _arm_core_uc_msg_timeout_usecs = 10 * 1000 * 1000; /* 10sec */

static arm_core_work_t *ac_work[SOC_MAX_NUM_DEVICES];

static
int
arm_core_test_msg(arm_core_work_t *ac, int subclass, uint32 data_in, uint32 *data_out)
{
    int                 rv;
    mos_msg_data_t      send, reply;

    /* Build the message */
    sal_memset(&send, 0, sizeof(send));
    sal_memset(&reply, 0, sizeof(reply));
    send.s.mclass = MOS_MSG_CLASS_TEST;
    send.s.subclass = subclass;
    send.s.data = soc_htonl(data_in);

    /* Send and receive */
    rv = soc_cmic_uc_msg_send_receive(ac->unit, ac->uc_num, &send, &reply,
                                      _arm_core_uc_msg_timeout_usecs);

    /* Check reply class, subclass */
    if (rv != SOC_E_NONE) {
        test_error(ac->unit, "soc_cmic_uc_msg_send_receive failed (%d).\n", rv);
        return rv;
    }
    if ((reply.s.mclass != send.s.mclass) ||
        (reply.s.subclass != send.s.subclass)) {
        test_error(ac->unit, "unexpected message reply.\n");
        return SOC_E_INTERNAL;
    }

    /* return data */
    *data_out = soc_ntohl(reply.s.data);
    return SOC_E_NONE;
}

int
arm_core_test(int u, args_t *args, void *pa)
{
    arm_core_work_t     *ac = (arm_core_work_t *)pa;
    uint32              count;
    int                 r;
    int                 rv;
    uint32              rc;

    /* Start the test application */
    rv = soc_cmic_uc_appl_init(u, ac->uc_num, MOS_MSG_CLASS_TEST,
                               _arm_core_uc_msg_timeout_usecs,
                               0, 0,
                               NULL, NULL);
    if (rv != SOC_E_NONE) {
        test_error(u, "Error starting test appl (%d).\n", rv);
        return -1;
    }

    rv = arm_core_test_msg(ac, MOS_MSG_SUBCLASS_TEST_COUNT, 0, &count);
    if (rv != SOC_E_NONE) {
        test_error(u, "Error communicating with test appl (%d).\n", rv);
        return -1;
    }

    /* If no tests, error */
    if (!count) {
        test_error(u, "uKernel image on core %d does not support test.\n",
                   ac->uc_num);
        return -1;
    }

    progress_init(count, 1, 0);
    for (r = 0; r < count; ) {
        rv = arm_core_test_msg(ac, MOS_MSG_SUBCLASS_TEST_RUN, r, &rc);
        if (rv != SOC_E_NONE) {
            test_error(u, "Error communicating with test appl (%d).\n", rv);
            return -1;
        }
        switch (rc) {
        case MOS_TEST_RC_OK:
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(u,
                                 "test %d PASS\n"), r));
            break;
        case MOS_TEST_RC_FAILURE:
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(u,
                                 "test %d FAIL\n"), r));
            break;
        case MOS_TEST_RC_INVALID:
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(u,
                                 "test %d SKIP\n"), r));
            break;
        }
        r += 1;
        progress_report(1);
    }
    progress_done();
    return 0;
}

int
arm_core_test_init(int u, args_t *args, void **pa)
{
    arm_core_work_t     *ac;
    parse_table_t       pt;

    if (!soc_feature(u, soc_feature_mcs)
        && !soc_feature(u, soc_feature_iproc)) {
        test_error(u, "ERROR: test only valid on mcs or iproc enabled devices\n");
        return -1;
    } 

    if (ac_work[u] == NULL) {
        ac_work[u] = sal_alloc(sizeof(arm_core_work_t), "arm_core_test");
        if (ac_work[u] == NULL) {
            test_error(u, "ERROR: cannot allocate memory\n");
            return -1;
        }
        sal_memset(ac_work[u], 0, sizeof(arm_core_work_t));
    }
    ac = ac_work[u];
    ac->unit = u;

    parse_table_init(u, &pt);
    parse_table_add(&pt, "uc", PQ_INT, (void *)0, &ac->uc_num,
                    NULL);
    parse_table_add(&pt, "tests", PQ_INT, (void *)0xffffffff, &ac->tests,
                    NULL);

    if (parse_arg_eq(args, &pt) < 0 || ARG_CNT(args) != 0) {
        test_error(u, "%s: Invalid Option: %s\n", ARG_CMD(args),
                   ARG_CUR(args) ? ARG_CUR(args) : "*");
    }
    parse_arg_eq_done(&pt);

    *pa = (void *)ac;

    return 0;
}

int
arm_core_test_done(int u, void *pa)
{
    arm_core_work_t     *ac = (arm_core_work_t *)pa;

    if (NULL == ac) {
        return(0);
    }

    sal_free(ac);
    ac_work[u] = NULL;
    return(0);
}

#endif
