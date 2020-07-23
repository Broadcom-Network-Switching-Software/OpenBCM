/*! \file bcmecmp_util.c
 *
 * This file contains ECMP custom table handler utility functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmissu/issu_api.h>
#include <bcmecmp/bcmecmp_util.h>
#include <bcmecmp/bcmecmp_types.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_ECMP

/*******************************************************************************
 * Private functions
 */
/*******************************************************************************
 * Public functions
 */

/*!
 * \brief Compares two integer values and returns the status.
 *
 * \param [in] a First integer value to compare.
 * \param [in] b Second integer value to compare with.
 *
 * \returns BCMECMP_CMP_GREATER When first value is greater than second.
 * \returns BCMECMP_CMP_EQUAL When both values are equal.
 * \returns BCMECMP_CMP_LESS When first value is lesser than second.
 */
int
bcmecmp_cmp_int(const void *a, const void *b)
{
    int first;  /* Fist integer value to compare. */
    int second; /* Second integer value to compare. */

    first = *(int *)a;
    second = *(int *)b;

    if (first < second) {
        return (BCMECMP_CMP_LESS);
    } else if (first > second) {
        return (BCMECMP_CMP_GREATER);
    }
    return (BCMECMP_CMP_EQUAL);
}

/*!
 * \brief Compares member table start indices of the defragmentation blocks.
 *
 * \param [in] a First defragmentation block to compare.
 * \param [in] b Second defragmentation block to compare with.
 *
 * \returns BCMECMP_CMP_GREATER When first value is greater than second.
 * \returns BCMECMP_CMP_EQUAL When both values are equal.
 * \returns BCMECMP_CMP_LESS When first value is lesser than second.
 */
int
bcmecmp_defrag_grps_cmp(const void *a, const void *b)
{
    bcmecmp_grp_defrag_t *first;  /* Fist defragmentation block. */
    bcmecmp_grp_defrag_t *second; /* Second defragmentation block. */

    first = (bcmecmp_grp_defrag_t *)a;
    second = (bcmecmp_grp_defrag_t *)b;

    if (first->mstart_idx < second->mstart_idx) {
        return (BCMECMP_CMP_LESS);
    } else if (first->mstart_idx > second->mstart_idx) {
        return (BCMECMP_CMP_GREATER);
    }
    return (BCMECMP_CMP_EQUAL);
}
