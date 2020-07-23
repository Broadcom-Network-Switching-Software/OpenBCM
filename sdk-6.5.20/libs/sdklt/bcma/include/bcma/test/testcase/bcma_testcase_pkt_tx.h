/*! \file bcma_testcase_pkt_tx.h
 *
 * Test case for packet I/O Tx performance.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_PKT_TX_H
#define BCMA_TESTCASE_PKT_TX_H

#include <bcma/test/bcma_testcase.h>

/*! Brief description for Tx performance test case. */
#define BCMA_TESTCASE_PKT_TX_DESC \
    "Packet I/O Tx performance test.\n"

/*! Flags of Tx performance test case. */
#define BCMA_TESTCASE_PKT_TX_FLAGS \
    (0)

/*!
 * \brief Get Tx performance test case operation routines.
 *
 * This function gets Tx performance test case operation routines.
 *
 * \retval Test operation routines pointer
 */
extern bcma_test_op_t *
bcma_testcase_pkt_tx_op_get(void);

#endif /* BCMA_TESTCASE_PKT_TX_H */

