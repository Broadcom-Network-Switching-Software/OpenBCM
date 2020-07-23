/*! \file bcma_test.h
 *
 * Broadcom Test Component.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TEST_H
#define BCMA_TEST_H

#include <sal/sal_types.h>

#include <bcma/test/bcma_testdb.h>

/*!
 * \brief Get Test Database.
 *
 * This function gets the test database.
 *
 * \return test database pointer.
 */
extern bcma_test_db_t *
bcma_test_db_get(void);

/*!
 * \brief Initialize test component.
 *
 * This function initializes the test database and
 * adds the default set of test cases into test database.
 *
 * \param [in] tdb Test Database
 *
 * \retval SHR_E_NONE Add test case into test database successfully.
 * \retval SHR_E_PARAM Invalid parameters.
 * \retval SHR_E_MEMORY Memory Allocated fail.
 */
extern int
bcma_test_init(bcma_test_db_t *tdb);

/*!
 * \brief Cleanup test component.
 *
 * This function cleans up the test database.
 *
 * \param [in] tdb Test Database
 *
 * \retval SHR_E_NONE Add test case into test database successfully.
 * \retval SHR_E_PARAM Invalid parameters.
 * \retval SHR_E_MEMORY Memory Allocated fail.
 */
extern int
bcma_test_cleanup(bcma_test_db_t *tdb);

/*!
 * \brief Add test case into test database.
 *
 * This function adds test case into test database.
 *
 * \param [in] tdb Test Database
 * \param [in] tc Test Case
 * \param [in] feature Device Feature
 *
 * \retval SHR_E_NONE Add test case into test database successfully.
 * \retval SHR_E_PARAM Invalid parameters.
 * \retval SHR_E_MEMORY Memory Allocated fail.
 */
extern int
bcma_test_testcase_add(bcma_test_db_t *tdb, bcma_test_case_t *tc, int feature);

#endif /* BCMA_TEST_H */
