/*! \file bcma_testcase_add_cmicd_cases.c
 *
 * Add test cases for CMICd.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_feature.h>

#include <bcma/test/bcma_test.h>
#include <bcma/test/testcase/bcma_testcase_cmicd_intr.h>
#include <bcma/test/testcase/bcma_testcase_cmicd_sbusdma.h>
#include <bcma/test/testcase/bcma_testcase_cmicd_pktdma.h>
#include <bcma/test/testcase/bcma_testcase_cmicd_sbusdma_batch.h>
#include <bcma/test/testcase/bcma_testcase_cmicd_ccmdma.h>
#include <bcma/test/testcase/bcma_testcase_cmicd_fifodma.h>
#include <bcma/test/testcase/bcma_testcase_cmicd_schan.h>
#include <bcma/test/testcase/bcma_testcase_cmicd.h>

static bcma_test_case_t testcase_cmicd_schan = {
    "hmi",
    4,
    "SBUS access",
    BCMA_TESTCASE_CMICD_SCHAN_DESC,
    BCMA_TESTCASE_CMICD_SCHAN_FLAGS,
    1,
    NULL,
    bcma_testcase_cmicd_schan_op_get,
};

static bcma_test_case_t testcase_cmicd_intr = {
    "hmi",
    10,
    "Interrupt",
    BCMA_TESTCASE_CMICD_INTR_DESC,
    BCMA_TESTCASE_CMICD_INTR_FLAGS,
    1,
    NULL,
    bcma_testcase_cmicd_intr_op_get,
};

static bcma_test_case_t testcase_cmicd_sbusdma = {
    "hmi",
    11,
    "SBUS DMA",
    BCMA_TESTCASE_CMICD_SBUSDMA_DESC,
    BCMA_TESTCASE_CMICD_SBUSDMA_FLAGS,
    1,
    NULL,
    bcma_testcase_cmicd_sbusdma_op_get,
};

static bcma_test_case_t testcase_cmicd_pktdma = {
    "hmi",
    500,
    "Packet DMA",
    BCMA_TESTCASE_CMICD_PKTDMA_DESC,
    BCMA_TESTCASE_CMICD_PKTDMA_FLAGS,
    1,
    NULL,
    bcma_testcase_cmicd_pktdma_op_get,
};

static bcma_test_case_t testcase_cmicd_sbusdma_batch = {
    "hmi",
    502,
    "SBUS batch DMA",
    BCMA_TESTCASE_CMICD_SBUSDMA_BATCH_DESC,
    BCMA_TESTCASE_CMICD_SBUSDMA_BATCH_FLAGS,
    1,
    NULL,
    bcma_testcase_cmicd_sbusdma_batch_op_get,
};

static bcma_test_case_t testcase_cmicd_ccmdma = {
    "hmi",
    503,
    "CCM DMA",
    BCMA_TESTCASE_CMICD_CCMDMA_DESC,
    BCMA_TESTCASE_CMICD_CCMDMA_FLAGS,
    1,
    NULL,
    bcma_testcase_cmicd_ccmdma_op_get,
};

static bcma_test_case_t testcase_cmicd_fifodma = {
    "hmi",
    504,
    "FIFO DMA",
    BCMA_TESTCASE_CMICD_FIFODMA_DESC,
    BCMA_TESTCASE_CMICD_FIFODMA_FLAGS,
    1,
    NULL,
    bcma_testcase_cmicd_fifodma_op_get,
};

void
bcma_testcase_add_cmicd_cases(bcma_test_db_t *tdb)
{
    bcma_test_testcase_add(tdb, &testcase_cmicd_schan, BCMDRD_FT_CMICD);
    bcma_test_testcase_add(tdb, &testcase_cmicd_intr, BCMDRD_FT_CMICD);
    bcma_test_testcase_add(tdb, &testcase_cmicd_sbusdma, BCMDRD_FT_CMICD);
    bcma_test_testcase_add(tdb, &testcase_cmicd_pktdma, BCMDRD_FT_CMICD);
    bcma_test_testcase_add(tdb, &testcase_cmicd_sbusdma_batch, BCMDRD_FT_CMICD);
    bcma_test_testcase_add(tdb, &testcase_cmicd_ccmdma, BCMDRD_FT_CMICD);
    bcma_test_testcase_add(tdb, &testcase_cmicd_fifodma, BCMDRD_FT_CMICD);
}
