/*! \file bcma_testcase_pkt_multi.h
 *
 * Test case for packet I/O performance with multiple channels.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_PKT_MULTI_H
#define BCMA_TESTCASE_PKT_MULTI_H

#include <bcma/test/bcma_testcase.h>

/*! Brief description for performance test case. */
#define BCMA_TESTCASE_PKT_MULTI_DESC \
    "Packet I/O performance test with multiple channels.\n"

/*! Flags of performance test case. */
#define BCMA_TESTCASE_PKT_MULTI_FLAGS \
    (0)

/*!
 * \brief Get performance test case operation routines.
 *
 * This function gets performance test case operation routines.
 *
 * \retval Test operation routines pointer
 */
extern bcma_test_op_t *
bcma_testcase_pkt_multi_op_get(void);

#endif /* BCMA_TESTCASE_PKT_MULTI_H */
