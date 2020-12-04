/*! \file bcmtm_imm_wred_cng_profile.c
 *
 * In-memory call back function for port service-pool configurations for WRED.
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
#include <bcmtm/wred/bcmtm_wred_internal.h>
/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM

/*******************************************************************************
 * Private functions
 */
static void
bcmtm_wred_cng_profile_parse(int unit,
                             const bcmltd_field_t *key,
                             const bcmltd_field_t *data,
                             bcmtm_wred_cng_profile_t *cng_profile)
{
    bcmltd_field_t *cur_key = (bcmltd_field_t *)key;
    cng_profile->action = BCMTM_WRED_CNG_ACTION_DEFAULT;

    while (cur_key) {
        switch (cur_key->id) {
            case TM_WRED_CNG_NOTIFICATION_PROFILEt_TM_WRED_CNG_NOTIFICATION_PROFILE_IDf:
                cng_profile->profile_id = cur_key->data;
                break;
            case TM_WRED_CNG_NOTIFICATION_PROFILEt_Q_AVGf:
                cng_profile->q_avg = cur_key->data;
                break;
            case TM_WRED_CNG_NOTIFICATION_PROFILEt_Q_MINf:
                cng_profile->q_min = cur_key->data;
                break;
            case TM_WRED_CNG_NOTIFICATION_PROFILEt_SERVICE_POOLf:
                cng_profile->sp = cur_key->data;
                break;
            default:
                break;
        }
        cur_key = cur_key->next;
    }
    if (data && data->id == TM_WRED_CNG_NOTIFICATION_PROFILEt_ACTIONf) {
        cng_profile->action = data->data;
    }
}

/*!
 * \brief WRED congestion profile physical table configurations.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in/out] cng_profile Congestion profile.
 * \param [in] set/get the profile from physical table.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error codes.
 */
static int
bcmtm_wred_cng_profile_pt_set(int unit,
                              bcmltd_sid_t ltid,
                              bcmtm_wred_cng_profile_t *cng_profile)
{
    bcmtm_drv_t *drv;
    bcmtm_wred_drv_t wred_drv;

    SHR_FUNC_ENTER(unit);
    sal_memset(&wred_drv, 0, sizeof(bcmtm_wred_drv_t));

    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->wred_drv_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_EXIT(drv->wred_drv_get(unit, &wred_drv));

    /* update physical table. */
    if (wred_drv.cng_profile_set) {
        SHR_IF_ERR_EXIT
           (wred_drv.cng_profile_set(unit, ltid, cng_profile));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM_WRED_CNG_NOTIFICATION_PROFILE logical table callback function for
 * staging.
 *
 * Handle TM_WRED_CNG_NOTIFICATION_PROFILE IMM table change events.
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
bcmtm_wred_cng_profile_stage(int unit,
        bcmltd_sid_t ltid,
        uint32_t trans_id,
        bcmimm_entry_event_t event_reason,
        const bcmltd_field_t *key,
        const bcmltd_field_t *data,
        void *context,
        bcmltd_fields_t *output_fields)
{
    bcmtm_wred_cng_profile_t wred_cng_profile;

    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }

    sal_memset(&wred_cng_profile, 0, sizeof(bcmtm_wred_cng_profile_t));
    bcmtm_wred_cng_profile_parse(unit, key, data, &wred_cng_profile);
    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_EXIT
                (bcmtm_wred_cng_profile_pt_set(unit, ltid, &wred_cng_profile));
            break;
        case BCMIMM_ENTRY_DELETE:
            wred_cng_profile.action = BCMTM_WRED_CNG_ACTION_CLEAR;
            SHR_IF_ERR_EXIT
                (bcmtm_wred_cng_profile_pt_set(unit, ltid, &wred_cng_profile));
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
bcmtm_wred_cng_profile_imm_register(int unit)
{
    /*!
     * Inmemory callback functions for TM_WRED_CNG_NOTIFICATION_PROFILE
     * logical table.
     */
    bcmimm_lt_cb_t bcmtm_wred_cng_profile_imm_cb = {
        /*! Staging function. */
        .stage = bcmtm_wred_cng_profile_stage,
    };
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);
    rv = bcmlrd_map_get(unit, TM_WRED_CNG_NOTIFICATION_PROFILEt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }
    /*! Registers callback functions for WRED congestion profile. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_WRED_CNG_NOTIFICATION_PROFILEt,
                             &bcmtm_wred_cng_profile_imm_cb, NULL));
exit:
    SHR_FUNC_EXIT();
}
