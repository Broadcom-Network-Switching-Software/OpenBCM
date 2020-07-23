/*! \file bcma_testcase_cmicx_schan.h
 *
 * Broadcom PCI S-Channel test case for CMICx.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_CMICX_SCHAN_H
#define BCMA_TESTCASE_CMICX_SCHAN_H

#include <bcma/test/bcma_testcase.h>

/*! Brief description for CMICx schan test case. */
#define BCMA_TESTCASE_CMICX_SCHAN_DESC "Test CMIC S-Channel access.\n"

/*! Flags of CMICx schan test case. */
#define BCMA_TESTCASE_CMICX_SCHAN_FLAGS (BCMA_TEST_F_DETACHED)

/*!
 * \brief Get CMICx schan test case operation routines.
 *
 * This function gets CMICx schan test case operation routines.
 *
 * \retval Test operation routines pointer
 */
extern bcma_test_op_t *
bcma_testcase_cmicx_schan_op_get(void);

#endif /* BCMA_TESTCASE_CMICX_SCHAN_H */
