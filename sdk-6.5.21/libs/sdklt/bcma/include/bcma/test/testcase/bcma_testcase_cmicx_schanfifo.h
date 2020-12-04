/*! \file bcma_testcase_cmicx_schanfifo.h
 *
 * Broadcom SCHAN FIFO test case for CMICx.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_CMICX_SCHANFIFO_H
#define BCMA_TESTCASE_CMICX_SCHANFIFO_H

#include <bcma/test/bcma_testcase.h>

/*! Brief description for CMICx SCHANFIFO test case. */
#define BCMA_TESTCASE_CMICX_SCHANFIFO_DESC \
    "Test S-Channel FIFO access for memory read/write operations.\n"

/*! Flags of CMICx SCHANFIFO test case. */
#define BCMA_TESTCASE_CMICX_SCHANFIFO_FLAGS \
    (BCMA_TEST_F_DETACHED | BCMA_TEST_F_DESTRUCTIVE)

/*!
 * \brief Get CMICx SCHANFIFO test case operation routines.
 *
 * This function gets CMICx SCHANFIFO test case operation routines.
 *
 * \retval Test operation routines pointer
 */
extern bcma_test_op_t *
bcma_testcase_cmicx_schanfifo_op_get(void);

#endif /* BCMA_TESTCASE_CMICX_SCHANFIFO_H */
