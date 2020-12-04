/*! \file bcma_testcase_traffic_stream.h
 *
 * Test case for streaming traffic.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_TRAFFIC_STREAM_H
#define BCMA_TESTCASE_TRAFFIC_STREAM_H

#include <bcma/test/bcma_testcase.h>

/*! Brief description for streaming traffic test case. */
#define BCMA_TESTCASE_TRAFFIC_STREAM_DESC \
    "Streaming traffic test to check TDM algorithm.\n"

/*! Flags for streaming traffic test case.
 *
 * No special flags are used in this test case.
 */
#define BCMA_TESTCASE_TRAFFIC_STREAM_FLAGS (0)

/*!
 * \brief Get streaming traffic test case operation routines.
 *
 * \return Test case operation routines pointer.
 */
extern bcma_test_op_t *
bcma_testcase_traffic_stream_op_get(void);


#endif /* BCMA_TESTCASE_TRAFFIC_STREAM_H */
