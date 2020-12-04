/*! \file bcma_testcase_counter_snmp.h
 *
 * Broadcom test cases for verifying the SNMP statistics for each port.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_COUNTER_SNMP_H
#define BCMA_TESTCASE_COUNTER_SNMP_H

#include <bcma/test/bcma_testcase.h>

/*! Brief description for counter snmp statistics test case. */
#define BCMA_TESTCASE_COUNTER_SNMP_DESC \
    "Verify the SNMP statistics for each port.\n"

/*! Flags of counter snmp statistics test case. */
#define BCMA_TESTCASE_COUNTER_SNMP_FLAGS \
    (0)

/*!
 * \brief Get counter snmp statistics test case operation routines.
 *
 * This function gets counter snmp statistics test case operation routines.
 *
 * \retval Test operation routines pointer
 */
extern bcma_test_op_t *
bcma_testcase_counter_snmp_op_get(void);

#endif /* BCMA_TESTCASE_COUNTER_SNMP_H */
