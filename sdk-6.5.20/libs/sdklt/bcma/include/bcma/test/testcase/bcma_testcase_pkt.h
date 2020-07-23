/*! \file bcma_testcase_pkt.h
 *
 * Add Packet I/O test cases into Test Database.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_PKT_H
#define BCMA_TESTCASE_PKT_H

#include <bcma/test/bcma_testdb.h>

/*!
 * \brief Add Packet I/O test case into TDB.
 *
 * This function adds Packet I/O test case into test database.
 *
 * \param [in] tdb Test Database
 */
extern void
bcma_testcase_add_pkt_cases(bcma_test_db_t *tdb);

#endif /* BCMA_TESTCASE_PKT_H */
