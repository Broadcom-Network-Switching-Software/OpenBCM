/*! \file bcma_testcase_pkt_rx.h
 *
 * Test case for packet I/O Rx performance.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_PKT_RX_H
#define BCMA_TESTCASE_PKT_RX_H

#include <bcma/test/bcma_testcase.h>

/*! Brief description for Rx performance test case. */
#define BCMA_TESTCASE_PKT_RX_DESC \
    "Packet I/O Rx performance test.\n"

/*! Flags of Rx performance test case. */
#define BCMA_TESTCASE_PKT_RX_FLAGS \
    (0)

/*!
 * \brief Get Rx performance test case operation routines.
 *
 * This function gets Rx performance test case operation routines.
 *
 * \retval Test operation routines pointer
 */
extern bcma_test_op_t *
bcma_testcase_pkt_rx_op_get(void);

#endif /* BCMA_TESTCASE_PKT_RX_H */

