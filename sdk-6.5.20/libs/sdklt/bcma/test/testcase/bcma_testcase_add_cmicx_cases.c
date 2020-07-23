/*! \file bcma_testcase_add_cmicx_cases.c
 *
 * Add test cases for CMICx.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_feature.h>

#include <bcma/test/bcma_test.h>
#include <bcma/test/testcase/bcma_testcase_cmicx_intr.h>
#include <bcma/test/testcase/bcma_testcase_cmicx_sbusdma.h>
#include <bcma/test/testcase/bcma_testcase_cmicx_schanfifo.h>
#include <bcma/test/testcase/bcma_testcase_cmicx_pktdma.h>
#include <bcma/test/testcase/bcma_testcase_cmicx_sbusdma_batch.h>
#include <bcma/test/testcase/bcma_testcase_cmicx_ccmdma.h>
#include <bcma/test/testcase/bcma_testcase_cmicx_fifodma.h>
#include <bcma/test/testcase/bcma_testcase_cmicx_schan.h>
#include <bcma/test/testcase/bcma_testcase_cmicx.h>

static bcma_test_case_t testcase_cmicx_schan = {
    "hmi",
    4,
    "SBUS access",
    BCMA_TESTCASE_CMICX_SCHAN_DESC,
    BCMA_TESTCASE_CMICX_SCHAN_FLAGS,
    1,
    NULL,
    bcma_testcase_cmicx_schan_op_get,
};

static bcma_test_case_t testcase_cmicx_intr = {
    "hmi",
    10,
    "Interrupt",
    BCMA_TESTCASE_CMICX_INTR_DESC,
    BCMA_TESTCASE_CMICX_INTR_FLAGS,
    1,
    NULL,
    bcma_testcase_cmicx_intr_op_get,
};

static bcma_test_case_t testcase_cmicx_sbusdma = {
    "hmi",
    11,
    "SBUS DMA",
    BCMA_TESTCASE_CMICX_SBUSDMA_DESC,
    BCMA_TESTCASE_CMICX_SBUSDMA_FLAGS,
    1,
    NULL,
    bcma_testcase_cmicx_sbusdma_op_get,
};

static bcma_test_case_t testcase_cmicx_schanfifo = {
    "hmi",
    12,
    "SBUS FIFO",
    BCMA_TESTCASE_CMICX_SCHANFIFO_DESC,
    BCMA_TESTCASE_CMICX_SCHANFIFO_FLAGS,
    1,
    NULL,
    bcma_testcase_cmicx_schanfifo_op_get,
};

static bcma_test_case_t testcase_cmicx_pktdma = {
    "hmi",
    500,
    "Packet DMA",
    BCMA_TESTCASE_CMICX_PKTDMA_DESC,
    BCMA_TESTCASE_CMICX_PKTDMA_FLAGS,
    1,
    NULL,
    bcma_testcase_cmicx_pktdma_op_get,
};

static bcma_test_case_t testcase_cmicx_sbusdma_batch = {
    "hmi",
    502,
    "SBUS batch DMA",
    BCMA_TESTCASE_CMICX_SBUSDMA_BATCH_DESC,
    BCMA_TESTCASE_CMICX_SBUSDMA_BATCH_FLAGS,
    1,
    NULL,
    bcma_testcase_cmicx_sbusdma_batch_op_get,
};

static bcma_test_case_t testcase_cmicx_ccmdma = {
    "hmi",
    503,
    "CCM DMA",
    BCMA_TESTCASE_CMICX_CCMDMA_DESC,
    BCMA_TESTCASE_CMICX_CCMDMA_FLAGS,
    1,
    NULL,
    bcma_testcase_cmicx_ccmdma_op_get,
};

static bcma_test_case_t testcase_cmicx_fifodma = {
    "hmi",
    504,
    "FIFO DMA",
    BCMA_TESTCASE_CMICX_FIFODMA_DESC,
    BCMA_TESTCASE_CMICX_FIFODMA_FLAGS,
    1,
    NULL,
    bcma_testcase_cmicx_fifodma_op_get,
};

void
bcma_testcase_add_cmicx_cases(bcma_test_db_t *tdb)
{
    bcma_test_testcase_add(tdb, &testcase_cmicx_schan, BCMDRD_FT_CMICX);
    bcma_test_testcase_add(tdb, &testcase_cmicx_intr, BCMDRD_FT_CMICX);
    bcma_test_testcase_add(tdb, &testcase_cmicx_sbusdma, BCMDRD_FT_CMICX);
    bcma_test_testcase_add(tdb, &testcase_cmicx_schanfifo, BCMDRD_FT_CMICX);
    bcma_test_testcase_add(tdb, &testcase_cmicx_pktdma, BCMDRD_FT_CMICX);
    bcma_test_testcase_add(tdb, &testcase_cmicx_sbusdma_batch, BCMDRD_FT_CMICX);
    bcma_test_testcase_add(tdb, &testcase_cmicx_ccmdma, BCMDRD_FT_CMICX);
    bcma_test_testcase_add(tdb, &testcase_cmicx_fifodma, BCMDRD_FT_CMICX);
}
