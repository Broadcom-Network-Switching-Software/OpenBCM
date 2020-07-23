/*! \file bcma_testcase_add_pt_cases.c
 *
 * Add test cases for physical table.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testdb.h>
#include <bcma/test/testcase/bcma_testcase_pt.h>
#include <bcma/test/testcase/bcma_testcase_reg_reset.h>
#include <bcma/test/testcase/bcma_testcase_reg_wr.h>
#include <bcma/test/testcase/bcma_testcase_mem_fv.h>
#include <bcma/test/testcase/bcma_testcase_mem_rand.h>
#include <bcma/test/testcase/bcma_testcase_mem_rand_w.h>
#include <bcma/test/testcase/bcma_testcase_mem_dma.h>
#include <bcma/test/testcase/bcma_testcase_mem_cambist.h>
#include <bcma/test/testcase/bcma_testcase_mem_mbist.h>
#include <bcma/test/testcase/bcma_testcase_mem_reset.h>

static bcma_test_case_t testcase_reg_reset = {
    "pt",
    1,
    "Register reset value",
    BCMA_TESTCASE_REG_RESET_DESC,
    BCMA_TESTCASE_REG_RESET_FLAGS,
    1,
    NULL,
    bcma_testcase_reg_reset_op_get,
};

static bcma_test_case_t testcase_reg_wr = {
    "pt",
    3,
    "Register write/read",
    BCMA_TESTCASE_REG_WR_DESC,
    BCMA_TESTCASE_REG_WR_FLAGS,
    1,
    NULL,
    bcma_testcase_reg_wr_op_get,
};

static bcma_test_case_t testcase_mem_fv = {
    "pt",
    50,
    "Memory fill/verify",
    BCMA_TESTCASE_MEM_FV_DESC,
    BCMA_TESTCASE_MEM_FV_FLAGS,
    1,
    NULL,
    bcma_testcase_mem_fv_op_get,
};

static bcma_test_case_t testcase_mem_rand = {
    "pt",
    51,
    "Memory random addr",
    BCMA_TESTCASE_MEM_RAND_DESC,
    BCMA_TESTCASE_MEM_RAND_FLAGS,
    1,
    NULL,
    bcma_testcase_mem_rand_op_get,
};

static bcma_test_case_t testcase_mem_rand_w = {
    "pt",
    52,
    "Memory random addr group",
    BCMA_TESTCASE_MEM_RAND_W_DESC,
    BCMA_TESTCASE_MEM_RAND_W_FLAGS,
    1,
    NULL,
    bcma_testcase_mem_rand_w_op_get,
};

static bcma_test_case_t testcase_mem_dma = {
    "pt",
    71,
    "Memory DMA",
    BCMA_TESTCASE_MEM_DMA_DESC,
    BCMA_TESTCASE_MEM_DMA_FLAGS,
    1,
    NULL,
    bcma_testcase_mem_dma_op_get,
};

static bcma_test_case_t testcase_mem_mbist = {
    "pt",
    515,
    "Memory BIST",
    BCMA_TESTCASE_MEM_MBIST_DESC,
    BCMA_TESTCASE_MEM_MBIST_FLAGS,
    1,
    NULL,
    bcma_testcase_mem_mbist_op_get,
};

static bcma_test_case_t testcase_mem_cambist = {
    "pt",
    516,
    "CAM BIST",
    BCMA_TESTCASE_MEM_CAMBIST_DESC,
    BCMA_TESTCASE_MEM_CAMBIST_FLAGS,
    1,
    NULL,
    bcma_testcase_mem_cambist_op_get,
};

static bcma_test_case_t testcase_mem_reset = {
    "pt",
    54,
    "Memory reset value",
    BCMA_TESTCASE_MEM_RESET_DESC,
    BCMA_TESTCASE_MEM_RESET_FLAGS,
    1,
    NULL,
    bcma_testcase_mem_reset_op_get,
};

void
bcma_testcase_add_pt_cases(bcma_test_db_t *tdb)
{
    bcma_test_testcase_add(tdb, &testcase_reg_reset, 0);
    bcma_test_testcase_add(tdb, &testcase_reg_wr, 0);
    bcma_test_testcase_add(tdb, &testcase_mem_fv, 0);
    bcma_test_testcase_add(tdb, &testcase_mem_rand, 0);
    bcma_test_testcase_add(tdb, &testcase_mem_rand_w, 0);
    bcma_test_testcase_add(tdb, &testcase_mem_dma, 0);
    bcma_test_testcase_add(tdb, &testcase_mem_cambist, 0);
    bcma_test_testcase_add(tdb, &testcase_mem_mbist, 0);
    bcma_test_testcase_add(tdb, &testcase_mem_reset, 0);
}

