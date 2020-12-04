/*! \file bcmtm_imm_obm_pc_pm_max_usage_mode.c
 *
 * In-memory call back function for obm policy table.
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
#include <shr/shr_debug.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>
#include <bcmtm/obm/bcmtm_obm_internal.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM
/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Configures the physical table with user defined values.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] key  This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [out] output_fields This is linked list of extra added data fields
 * (mostly read only fields.)
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_obm_pc_pm_max_usage_mode_set(int unit,
                      bcmltd_sid_t ltid,
                      uint8_t pmid,
                      uint8_t fval)
{
    bcmtm_drv_t *drv;
    bcmtm_obm_drv_t obm_drv;

    SHR_FUNC_ENTER(unit);
    sal_memset(&obm_drv, 0, sizeof(bcmtm_obm_drv_t));

    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->obm_drv_get, SHR_E_UNAVAIL);

    SHR_IF_ERR_EXIT(drv->obm_drv_get(unit, &obm_drv));

    if (obm_drv.max_usage_mode_set) {
        SHR_IF_ERR_EXIT
            (obm_drv.max_usage_mode_set(unit, ltid, pmid, fval));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM_OBM_PC_PM_MAX_USAGE_MODE logical table callback function for staging.
 *
 * Handle TM_OBM_PC_PM_MAX_USAGE_MODE table change events.
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
bcmtm_obm_pc_pm_max_usage_mode_stage(int unit,
        bcmltd_sid_t ltid,
        uint32_t trans_id,
        bcmimm_entry_event_t event_reason,
        const bcmltd_field_t *key,
        const bcmltd_field_t *data,
        void *context,
        bcmltd_fields_t *output_fields)
{
    uint64_t fval;
    uint8_t pmid, mode = 0;
    bcmdrd_fid_t fid;
    SHR_FUNC_ENTER(unit);

    fid = TM_OBM_PC_PM_MAX_USAGE_MODEt_PC_PM_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    pmid = (uint8_t)fval - 1;

    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
        case BCMIMM_ENTRY_INSERT:
            if (data) {
                fid = TM_OBM_PC_PM_MAX_USAGE_MODEt_MAX_USAGE_MODEf;
                SHR_IF_ERR_EXIT
                    (bcmtm_fval_get_from_field_array(unit, fid, 0, data, &fval));
                mode = (uint8_t)fval;
            }

            SHR_NULL_CHECK(output_fields, SHR_E_INTERNAL);
            output_fields->count = 0 ;
            SHR_IF_ERR_EXIT
                (bcmtm_obm_pc_pm_max_usage_mode_set(unit, ltid, pmid, mode));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT
                (bcmtm_obm_pc_pm_max_usage_mode_set(unit, ltid, pmid, 0));
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
bcmtm_obm_pc_pm_max_usage_mode_imm_register(int unit)
{
    /*! Inmemory callback functions for TM_OBM_PC_PM_MAX_USAGE_MODE LT. */
    bcmimm_lt_cb_t bcmtm_obm_pc_pm_max_usage_mode_imm_cb = {
        /*! Staging function. */
        .stage = bcmtm_obm_pc_pm_max_usage_mode_stage,
    };
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);
    rv = bcmlrd_map_get(unit, TM_OBM_PC_PM_MAX_USAGE_MODEt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }
    /*! Registers callback functions for obm flow control. */
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmimm_lt_event_reg(unit, TM_OBM_PC_PM_MAX_USAGE_MODEt,
                             &bcmtm_obm_pc_pm_max_usage_mode_imm_cb, NULL),
         SHR_E_NOT_FOUND);
exit:
    SHR_FUNC_EXIT();
}

