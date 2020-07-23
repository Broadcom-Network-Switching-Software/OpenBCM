/*! \file bcma_testcase_cmicx_pktdma.h
 *
 * Broadcom Packet Dma test case for CMICx.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_CMICX_PKTDMA_H
#define BCMA_TESTCASE_CMICX_PKTDMA_H

#include <bcma/test/bcma_testcase.h>

/*! Brief description for CMICx pktdma test case. */
#define BCMA_TESTCASE_CMICX_PKTDMA_DESC \
    "Test packet DMA access by desciptors.\n"

/*! Flags of CMICx pktdma test case. */
#define BCMA_TESTCASE_CMICX_PKTDMA_FLAGS \
    (0)

/*!
 * \brief Get CMICx pktdma test case operation routines.
 *
 * This function gets CMICx pktdma test case operation routines.
 *
 * \retval Test operation routines pointer
 */
extern bcma_test_op_t *
bcma_testcase_cmicx_pktdma_op_get(void);

#endif /* BCMA_TESTCASE_CMICX_PKTDMA_H */
