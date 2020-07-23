/*! \file bcma_testcase_ser.h
 *
 * Add test cases for SER.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_SER_H
#define BCMA_TESTCASE_SER_H

#include <bcma/test/bcma_testdb.h>

/*!
 * \brief Add SER test cases into TDB.
 *
 * \param [in] tdb Test Database.
 *
 * \return NONE.
 */
extern void
bcma_testcase_add_ser_cases(bcma_test_db_t *tdb);

#endif /* BCMA_TESTCASE_SER_H */
