/*! \file bcma_testcase_lb.h
 *
 * Add test cases for Loopback.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_LB_H
#define BCMA_TESTCASE_LB_H

#include <bcma/test/bcma_testdb.h>

/*!
 * \brief Add loopback test cases into TDB.
 *
 * \param [in] tdb Test Database.
 */
void
bcma_testcase_add_lb_cases(bcma_test_db_t *tdb);

#endif /* BCMA_TESTCASE_LB_H */
