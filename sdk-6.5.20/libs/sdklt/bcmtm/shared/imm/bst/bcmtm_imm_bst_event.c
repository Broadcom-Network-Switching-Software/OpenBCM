/*! \file bcmtm_imm_bst_event.c
 *
 * BCMTM BST events interface to in-memory table.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmbd/bcmbd.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bst/bcmtm_bst_internal.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>


/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief BST event control IMM table change callback function for staging.
 *
 * Handle BST event control RO IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] action This is the desired action for the entry.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * \param [out] output_fields Read only fields update to imm.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL BST fails to handle LT change events.
 */
static int
bcmtm_bst_event_control_imm_stage_callback(int unit,
                                           bcmltd_sid_t sid,
                                           uint32_t trans_id,
                                           bcmimm_entry_event_t event_reason,
                                           const bcmltd_field_t *key,
                                           const bcmltd_field_t *data,
                                           void *context,
                                           bcmltd_fields_t *output_fields)
{
    bcmtm_bst_event_cfg_t event_cfg;
    bcmtm_bst_drv_t bst_drv;
    bcmtm_bst_dev_info_t *bst_dev_info;
    bcmtm_drv_t *drv;
    bcmltd_fid_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);
    sal_memset(&bst_drv, 0, sizeof(bcmtm_bst_drv_t));

    bcmtm_bst_dev_info_get(unit, &bst_dev_info);
    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->bst_drv_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_EXIT(drv->bst_drv_get(unit, &bst_drv));

    if (output_fields) {
        output_fields->count = 0;
    }

    fid = TM_BST_EVENT_STATE_CONTROLt_BUFFER_POOLf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    event_cfg.buffer_pool = (uint8_t)fval;

    fid = TM_BST_EVENT_STATE_CONTROLt_CLASSf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    event_cfg.class = (bcmtm_bst_event_class_t)fval;

    /* Trigger state determines the user defined state control table state. */
    bst_dev_info->trigger_state[event_cfg.buffer_pool][event_cfg.class] = 0;
    if (data) {
        fid = TM_BST_EVENT_STATE_CONTROLt_STATEf;
        if (SHR_SUCCESS
                (bcmtm_fval_get_from_field_array(unit, fid, 0, data, &fval))) {
            event_cfg.state = (bcmtm_bst_event_state_t)fval;
            if (event_cfg.state == BST_EVENT_STATE_ARMED ||
                event_cfg.state == BST_EVENT_STATE_TRIGGERED) {
                bst_dev_info->trigger_state[event_cfg.buffer_pool][event_cfg.class] = 1;
            }
        }
    }

    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_UPDATE:
            break;
        case BCMIMM_ENTRY_DELETE:
            event_cfg.state = BST_EVENT_STATE_OFF;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Updates the hardware for events. */
    if (bst_drv.bst_event_control_set) {
        SHR_IF_ERR_EXIT
            (bst_drv.bst_event_control_set(unit, sid, &event_cfg));
    }
    /* Updates TM_BST_EVENT_STATE logical table for the event states.*/
    SHR_IF_ERR_EXIT
        (bcmtm_bst_event_state_update(unit, &event_cfg));
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
/*!
 * \brief BST init time routine
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code.
 */
int
bcmtm_imm_bst_event_register(int unit)
{
    const bcmlrd_map_t *map = NULL;
    int rv;
    /*
     * This structure contains callback functions that will be conresponding
     * to TM_BST_EVENT_STATE_CONTROL logical table entry commit stages.
     */
    static bcmimm_lt_cb_t bst_event_control_imm_callback = {
        /*! Staging function. */
        .stage = bcmtm_bst_event_control_imm_stage_callback,
    };

    SHR_FUNC_ENTER(unit);

    /*
     * To register callback for BST EVENT CONTROL LT.
     */
    rv = bcmlrd_map_get(unit, TM_BST_EVENT_STATE_CONTROLt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 TM_BST_EVENT_STATE_CONTROLt,
                                 &bst_event_control_imm_callback,
                                 NULL));
    }
exit:
    SHR_FUNC_EXIT();
}
