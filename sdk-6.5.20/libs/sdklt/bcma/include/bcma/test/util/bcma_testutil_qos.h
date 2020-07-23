/*! \file bcma_testutil_qos.h
 *
 * QOS LT operation utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTUTIL_QOS_H
#define BCMA_TESTUTIL_QOS_H

#include <shr/shr_types.h>
#include <bcmdrd/bcmdrd_types.h>


/*!
 * \brief Create one to one mapping for Packet PRI, CNG fields to INT_PRI.
 *
 * \param [in] unit Unit number.
 * \param [in] pbmp valid ports bitmap.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_drv_qos_one_to_one_map_create(int unit, bcmdrd_pbmp_t pbmp);

/*!
 * \brief Destroy one to one mapping for Packet PRI, CNG fields to INT_PRI.
 *
 * \param [in] unit Unit number.
 * \param [in] pbmp valid ports bitmap.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_drv_qos_one_to_one_map_destroy(int unit, bcmdrd_pbmp_t pbmp);

/*!
 * \brief Create one to one mapping for Packet PRI, CNG fields to INT_PRI.
 *
 * \param [in] unit Unit number.
 * \param [in] pbmp valid ports bitmap.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcma_testutil_drv_qos_one_to_one_map_create_f)(
                int unit, bcmdrd_pbmp_t pbmp);

/*!
 * \brief Destroy one to one mapping for Packet PRI, CNG fields to INT_PRI.
 *
 * \param [in] unit Unit number.
 * \param [in] pbmp valid ports bitmap.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcma_testutil_drv_qos_one_to_one_map_destroy_f)(
                int unit, bcmdrd_pbmp_t pbmp);

/*! Qos logical table operation functions. */
typedef struct bcma_testutil_qos_lt_op_s {

    /*! Packet priority to internal priority map create */
    bcma_testutil_drv_qos_one_to_one_map_create_f qos_one_to_one_map_create;

    /*! Packet priority to internal priority map destroy */
    bcma_testutil_drv_qos_one_to_one_map_destroy_f qos_one_to_one_map_destroy;

} bcma_testutil_qos_lt_op_t;

/*! XFS device qos operation functions on logical table. */
extern bcma_testutil_qos_lt_op_t *bcma_testutil_xfs_qos_lt_op_get(int unit);

/*! XGS device qos operation functions on logical table. */
extern bcma_testutil_qos_lt_op_t *bcma_testutil_xgs_qos_lt_op_get(int unit);

#endif /* BCMA_TESTUTIL_QOS_H */
