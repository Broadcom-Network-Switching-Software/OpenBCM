/*! \file bcmsec_subport_policy.c
 *
 * In-memory call back function for Subport policy related IMM LTs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <shr/shr_debug.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmsec/imm/bcmsec_imm_defines_internal.h>
#include <bcmsec/generated/bcmsec_ha.h>
#include <bcmsec/bcmsec_drv.h>
#include <bcmsec/bcmsec_utils.h>
#include <bcmsec/bcmsec_pt_internal.h>
#include <bcmsec/bcmsec_utils_internal.h>

#define BSL_LOG_MODULE  BSL_LS_BCMSEC_SUBPORT

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief  Decrypt Subport Policy physical table configuration
 * based on device type.
 *
 * \param [in] unit  Unit number.
 * \param [in] lport Logical port number.
 * \param [in] ltid Logical table ID.
 * \param [in] Obm port control configuration to be programmed.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmsec_subport_policy_pt_set(int unit, bcmltd_sid_t ltid,
                        bcmsec_pt_info_t *pt_dyn_info,
                        bool encrypt,
                        bcmltd_field_t *in)
{
    bcmsec_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmsec_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->subport_policy_set, SHR_E_UNAVAIL);

    /* update physical table. */
    SHR_IF_ERR_EXIT
        (drv->subport_policy_set(unit, ltid, pt_dyn_info, in));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief SEC_DECRYPT_SUBPORT_POLICY logical table staging function.
 *
 * Handle SEC_DECRYPT_SUBPORT_POLICY IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id LT Transaction Id
 * \param [in] event_reason Call back reason.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * \param [out] data. This is a linked list of extra added data fields (mostly
 * read only fields).
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LM fails to handle LT change events.
 */
static int
bcmsec_decrypt_subport_policy_stage(int unit,
                       bcmltd_sid_t ltid,
                       uint32_t trans_id,
                       bcmimm_entry_event_t event_reason,
                       const bcmltd_field_t *key,
                       const bcmltd_field_t *data,
                       void *context,
                       bcmltd_fields_t *output_fields)
{
    uint32_t blk_id;
    bcmltd_fid_t fid;
    uint64_t fval = 0;
    int index;
    bcmltd_field_t *entry_data;
    bcmsec_pt_info_t pt_dyn_info = {0};
    uint64_t flags;

    SHR_FUNC_ENTER(unit);
    if (output_fields) {
        output_fields->count = 0;
    }

    fid = SEC_DECRYPT_SUBPORT_POLICYt_SEC_BLOCK_IDf;
    SHR_IF_ERR_EXIT
        (bcmsec_fval_get_from_field_array(unit, fid, 0, key, &fval));
    blk_id = (uint32_t)fval;

    fid = SEC_DECRYPT_SUBPORT_POLICYt_SEC_DECRYPT_SUBPORT_POLICY_IDf;
    SHR_IF_ERR_EXIT
        (bcmsec_fval_get_from_field_array(unit, fid, 0, key, &fval));
    index = (uint32_t)fval;

    flags = 0;
    BCMSEC_PT_DYN_INFO(pt_dyn_info, index, blk_id, flags);

    entry_data = (bcmltd_field_t *)data;
    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
            flags =  BCMSEC_PT_DYN_FLAGS_UPDATE;
            BCMSEC_PT_DYN_INFO(pt_dyn_info, index, blk_id, flags);
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT
                (bcmsec_subport_policy_pt_set(unit, ltid, &pt_dyn_info,
                                         true, entry_data));
            break;
        default:
            break;
    }
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcmsec_decrypt_subport_policy_imm_register(int unit)
{
    /*!
     * Inmemory callback functions for SEC_DECRYPT_SUBPORT_POLICY
     * logical table.
     */
    bcmimm_lt_cb_t bcmsec_subport_policy_imm_cb = {
        /*! Staging function. */
        .stage = bcmsec_decrypt_subport_policy_stage,
    };
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /* SEC_DECRYPT_SUBPORT_POLICYt */
    rv = bcmlrd_map_get(unit, SEC_DECRYPT_SUBPORT_POLICYt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }
    /*! Registers callback functions for decrypt port settings. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, SEC_DECRYPT_SUBPORT_POLICYt,
                             &bcmsec_subport_policy_imm_cb, NULL));

exit:
    SHR_FUNC_EXIT();
}

