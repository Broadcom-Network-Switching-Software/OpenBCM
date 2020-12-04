/*! \file bcmptm_rm_hash_ltpr.c
 *
 * Resource Manager for Hash Table.
 *
 * This file contains the routines that implement the ltpr table mgmt.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/bcmdrd_pt.h>
#include "bcmptm_rm_hash.h"
#include "bcmptm_rm_hash_robust_hash.h"
#include "bcmptm_rm_hash_pt_register.h"
#include "bcmptm_rm_hash_ltpr.h"

/*******************************************************************************
 * Defines
 */

#define BSL_LOG_MODULE  BSL_LS_BCMPTM_RMHASH

#define RM_HASH_LTPR_DATA_FIELD_START_BIT         0

#define RM_HASH_LTPR_DATA_FIELD_END_BIT          11


/*******************************************************************************
 * Typedefs
 */

/*******************************************************************************
 * Private variables
 */


/*******************************************************************************
 * Private Functions
 */

/*!
 * \brief Write a physical table in interactive path.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table SID.
 * \param [in] sid Physical table SID.
 * \param [in] index Physical table index.
 * \param [in] entry_data Buffer to contain the entry content.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
rm_hash_ireq_write(int unit,
                   bcmltd_sid_t lt_id,
                   bcmdrd_sid_t sid,
                   int index,
                   void *entry_data)
{
    bcmbd_pt_dyn_info_t dyn_info;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;

    SHR_FUNC_ENTER(unit);

    dyn_info.index = index;
    dyn_info.tbl_inst = -1;
    SHR_IF_ERR_EXIT
        (bcmptm_ireq_write(unit,
                           0,
                           sid,
                           &dyn_info,
                           NULL,
                           entry_data,
                           lt_id,
                           &rsp_ltid,
                           &rsp_flags));

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */

int
bcmptm_rm_hash_ltpr_info_config(int unit,
                                bool warm,
                                bool init,
                                bcmptm_lt_lookup_ltpr_info_t *ltpr_info,
                                bool clear_hit,
                                bcmptm_rm_hash_table_write write_cb,
                                bcmltd_sid_t ltid,
                                uint32_t trans_id)
{
    uint32_t entry = 0;
    uint32_t ltpr_val = 0;
    uint16_t lkp_id;
    bcmltd_op_arg_t op_arg = {0};
    const bcmltd_op_arg_t *op_arg_p = &op_arg;

    SHR_FUNC_ENTER(unit);

    lkp_id = ltpr_info->lkp_id;
    if (clear_hit == TRUE) {
        ltpr_val = 1 << lkp_id;
    }
    bcmdrd_field_set(&entry,
                     RM_HASH_LTPR_DATA_FIELD_START_BIT,
                     RM_HASH_LTPR_DATA_FIELD_END_BIT,
                     &ltpr_val);
    if ((init == TRUE) && (warm == FALSE)) {
        /* This is in initialization stage of cold boot. */
        SHR_IF_ERR_EXIT
            (rm_hash_ireq_write(unit,
                                DEVICE_EM_TILE_INFOt,
                                ltpr_info->ltpr_ptid,
                                ltpr_info->ltpr_index,
                                &entry));
    }
    if (init == FALSE) {
        if (write_cb == NULL) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        op_arg.trans_id = trans_id;
        SHR_IF_ERR_EXIT
            (write_cb(unit,
                      ltid,
                      op_arg_p,
                      ltpr_info->ltpr_ptid,
                      ltpr_info->ltpr_index,
                      &entry));
    }

exit:
    SHR_FUNC_EXIT();
}

