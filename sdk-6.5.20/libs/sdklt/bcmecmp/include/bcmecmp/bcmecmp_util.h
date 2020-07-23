/*! \file bcmecmp_util.h
 *
 * This file contains ECMP custom table handler utility functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMECMP_UTIL_H
#define BCMECMP_UTIL_H

#include <sal/sal_types.h>
#include <bcmecmp/bcmecmp_db_internal.h>

/*! First integer value greather than Second integer value.  */
#define BCMECMP_CMP_GREATER       (1)

/*! First integer value equal to the Second integer value.  */
#define BCMECMP_CMP_EQUAL         (0)

/*! First integer value less than the Second integer value.  */
#define BCMECMP_CMP_LESS         (-1)

/*!
 * \brief Compares two integer values and returns the status.
 *
 * \param [in] a Pointer to the First integer value to compare.
 * \param [in] b Pointer to the Second integer value to compare with.
 *
 * \returns BCMECMP_CMP_GREATER When first value is greater than second.
 * \returns BCMECMP_CMP_EQUAL When both values are equal.
 * \returns BCMECMP_CMP_LESS When first value is lesser than second.
 */
extern int
bcmecmp_cmp_int(const void *a,
                const void *b);

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
extern int
bcmecmp_defrag_grps_cmp(const void *a,
                        const void *b);

#endif /* BCMECMP_UTIL_H */
