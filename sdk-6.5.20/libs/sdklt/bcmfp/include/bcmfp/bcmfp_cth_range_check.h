/*! \file bcmfp_cth_range_check.h
 *
 * Defines, Enums, Structures and APIs corresponding to
 * FP range_check LT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_CTH_RANGE_CHECK_H
#define BCMFP_CTH_RANGE_CHECK_H

#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmfp/bcmfp_types_internal.h>

/* Maximum range check IDs */
#define BCMFP_MAX_RANGE_CHECK_ID       64

/* Maximum range check group IDs */
#define BCMFP_MAX_RANGE_CHECK_GROUP_ID 16

/*! Range Check Id bitmap structure */
typedef struct bcmfp_range_check_id_bmp_s {
    SHR_BITDCL w[SHRi_BITDCLSIZE(BCMFP_MAX_RANGE_CHECK_ID)];
} bcmfp_range_check_id_bmp_t;

/*! Configuration provided in FP range check group LT entry. */
typedef struct bcmfp_range_check_group_config_s {
    /*! Range check group ID. */
    uint32_t group_id;

    /*! Physical pipe line. */
    int pipe_id;

    /*
     * Bitmap of range check IDs that are part of
     * range check group.
     */
    bcmfp_range_check_id_bmp_t rcid_bitmap;
} bcmfp_range_check_group_config_t;

/*!
 * \brief  Insert the range check group.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transactio ID.
 * \param [in] tbl_id             Logical Table ID.
 * \param [in] key                Key consisting of all key field values.
 * \param [in] data               Data consisting of user specified data
 *                                field values.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_range_check_group_insert(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               bcmltd_sid_t tbl_id,
                               bcmfp_stage_id_t stage_id,
                               const bcmltd_field_t *key,
                               const bcmltd_field_t *data,
                               bcmltd_fields_t *output_fields);

/*!
 * \brief  Delete the range check group.
 *
 * \param [in] unit               Logical device id.
 * \param [in] trans_id           Transactio ID.
 * \param [in] tbl_id             Logical Table ID.
 * \param [in] key                Key consisting of all key field values.
 *
 * \retval SHR_E_NONE     Success
 * \retval SHR_E_PARAM    Invalid parameters.
 */
extern int
bcmfp_range_check_group_delete(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               bcmltd_sid_t tbl_id,
                               bcmfp_stage_id_t stage_id,
                               const bcmltd_field_t *key,
                               const bcmltd_field_t *data,
                               bcmltd_fields_t *output_fields);
#endif /* BCMFP_CTH_RANGE_CHECK_H */
