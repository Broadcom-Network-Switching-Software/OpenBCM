/*! \file bcma_test.c
 *
 * Broadcom Test component
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/test/bcma_test.h>
#include <bcma/test/bcma_testdb.h>

static bcma_test_db_t *test_db = NULL;

bcma_test_db_t *
bcma_test_db_get(void)
{
    return test_db;
}

int
bcma_test_init(bcma_test_db_t *tdb)
{
    test_db = tdb;
    return bcma_testdb_init(tdb);
}

int
bcma_test_cleanup(bcma_test_db_t *tdb)
{
    return bcma_testdb_cleanup(tdb);
}

int
bcma_test_testcase_add(bcma_test_db_t *tdb, bcma_test_case_t *tc, int feature)
{
    return bcma_testdb_testcase_add(tdb, tc, feature);
}
