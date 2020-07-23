/*! \file bcma_testcase_cmicx_intr_internal.h
 *
 * common functions for CMICx interrupt testing.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTCASE_CMICX_INTR_INTERNAL_H
#define BCMA_TESTCASE_CMICX_INTR_INTERNAL_H

#include <sal/sal_types.h>

/*
 * Interrupt and trigger function for software
 * interrupt testing.
 *
 * host: interrupt name string for matching.
 */
typedef struct bcma_testcase_swi_test_s {
    char *host;
    void (*swi_intr)(int unit, uint32_t intr_param);
    void (*swi_trig)(int unit);
} bcma_testcase_swi_test_t;

extern int bcma_testcase_cmicx_intr_cnt;
extern int bcma_testcase_cmicx_intr_num;

/*!
 * \brief retrieve function set for CMICx interrupt testing.
 *
 * \param [in] unit Unit number.
 * \param [out] pointer of CMICx interrupt function set.
 * \param [out] array size of function set.
 * \param [out] pointer of default host string.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
int
bcma_testcase_cmicx_intr_func_get(int unit, bcma_testcase_swi_test_t **swi_test,
                                  int *count, char **def_host);

/*!
 * \brief retrieve function set for CMICx2 interrupt testing.
 *
 * \param [in] unit Unit number.
 * \param [out] pointer of CMICx2 interrupt function set.
 * \param [out] array size of function set.
 * \param [out] pointer of default host string.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
int
bcma_testcase_cmicx2_intr_func_get(int unit, bcma_testcase_swi_test_t **swi_test,
                                   int *count, char **def_host);

#endif /* BCMA_TESTCASE_CMICX_INTR_INTERNAL_H */
