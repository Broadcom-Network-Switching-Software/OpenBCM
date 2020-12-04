/*! \file bcma_testcase_pt.h
 *
 * Add test cases for Physical Table.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_PT_H
#define BCMA_TESTCASE_PT_H

#include <bcma/test/bcma_testdb.h>

/*!
 * \brief Add physical table test cases into TDB.
 *
 * \param [in] tdb Test Database.
 */
extern void
bcma_testcase_add_pt_cases(bcma_test_db_t *tdb);

#endif /* BCMA_TESTCASE_PT_H */

