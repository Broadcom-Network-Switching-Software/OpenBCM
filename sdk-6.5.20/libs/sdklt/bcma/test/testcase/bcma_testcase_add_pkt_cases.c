/*! \file bcma_testcase_add_pkt_cases.c
 *
 * Add test cases for Packet I/O.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testdb.h>
#include <bcma/test/testcase/bcma_testcase_pkt.h>
#include <bcma/test/testcase/bcma_testcase_pkt_rx.h>
#include <bcma/test/testcase/bcma_testcase_pkt_tx.h>

static bcma_test_case_t testcase_pkt_tx = {
    "pkt",
    40,
    "Tx performance",
    BCMA_TESTCASE_PKT_TX_DESC,
    BCMA_TESTCASE_PKT_TX_FLAGS,
    1,
    NULL,
    bcma_testcase_pkt_tx_op_get,
};

static bcma_test_case_t testcase_pkt_rx = {
    "pkt",
    41,
    "Rx performance",
    BCMA_TESTCASE_PKT_RX_DESC,
    BCMA_TESTCASE_PKT_RX_FLAGS,
    1,
    NULL,
    bcma_testcase_pkt_rx_op_get,
};

void
bcma_testcase_add_pkt_cases(bcma_test_db_t *tdb)
{
    bcma_test_testcase_add(tdb, &testcase_pkt_tx, 0);
    bcma_test_testcase_add(tdb, &testcase_pkt_rx, 0);
}
