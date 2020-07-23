/*! \file bcma_sys_conf_test.c
 *
 * Initialize test module component.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/sys/bcma_sys_conf.h>
#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testcmd.h>
#include <bcma/test/testcase/bcma_testcase_cmicd.h>
#include <bcma/test/testcase/bcma_testcase_cmicx.h>
#include <bcma/test/testcase/bcma_testcase_pt.h>
#include <bcma/test/testcase/bcma_testcase_lb.h>
#include <bcma/test/testcase/bcma_testcase_counter.h>
#include <bcma/test/testcase/bcma_testcase_pkt.h>
#include <bcma/test/testcase/bcma_testcase_ser.h>
#include <bcma/test/testcase/bcma_testcase_traffic.h>
#include <bcma/test/testcase/bcma_testcase_benchmark.h>

static bcma_test_db_t testdb = {0};

/*******************************************************************************
 * Public functions
 */

int
bcma_sys_conf_test_init(bcma_sys_conf_t *sc)
{
    bcma_test_db_t *tdb = &testdb;

    /* Check for valid sys_conf structure */
    if (sc == NULL) {
        return -1;
    }

    bcma_test_init(tdb);

    bcma_testcase_add_cmicd_cases(tdb);
    bcma_testcase_add_cmicx_cases(tdb);
    bcma_testcase_add_pt_cases(tdb);
    bcma_testcase_add_lb_cases(tdb);
    bcma_testcase_add_counter_cases(tdb);
    bcma_testcase_add_pkt_cases(tdb);
    bcma_testcase_add_ser_cases(tdb);
    bcma_testcase_add_traffic_cases(tdb);
    bcma_testcase_add_benchmark_cases(tdb);
    bcma_testcmd_add_cmds(sc->dsh);

    return 0;
}

int
bcma_sys_conf_test_cleanup(bcma_sys_conf_t *sc)
{
    bcma_test_db_t *tdb = &testdb;

    bcma_test_cleanup(tdb);

    return 0;
}

int
bcma_sys_conf_test_testcase_add(bcma_test_case_t *tc, int feature)
{
    bcma_test_db_t *tdb = &testdb;

    return bcma_testdb_testcase_add(tdb, tc, feature);
}
