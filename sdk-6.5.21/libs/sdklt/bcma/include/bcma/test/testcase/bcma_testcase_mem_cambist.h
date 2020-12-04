/*! \file bcma_testcase_mem_cambist.h
 *
 * Test case for CAM built-in self testing.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_MEM_CAMBIST_H
#define BCMA_TESTCASE_MEM_CAMBIST_H

#include <bcmdrd/bcmdrd_types.h>
#include <bcma/test/bcma_testcase.h>

/*! Brief description for CAM built-in self test case */
#define BCMA_TESTCASE_MEM_CAMBIST_DESC \
        "Run BIST (Built-In Self Testing) on CAM memories.\n"

/*! Flags for CAM built-in self test case */
#define BCMA_TESTCASE_MEM_CAMBIST_FLAGS \
        (BCMA_TEST_F_DESTRUCTIVE | BCMA_TEST_F_DETACHED)

/*!
 * \brief Get CAM built-in self test case operation routines.
 *
 * \return Test case operation routines pointer.
 */
extern bcma_test_op_t *
bcma_testcase_mem_cambist_op_get(void);

#endif /* BCMA_TESTCASE_MEM_CAMBIST_H */

