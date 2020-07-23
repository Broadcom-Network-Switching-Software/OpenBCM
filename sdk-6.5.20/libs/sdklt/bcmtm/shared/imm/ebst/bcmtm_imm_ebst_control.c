/*! \file bcmtm_imm_ebst_control.c
 *
 * BCMTM EBST control logical table.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/ebst/bcmtm_ebst_internal.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>

#define BSL_LOG_MODULE  BSL_LS_BCMTM_BST

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Parse the key and data fields
 *
 * \param [in] unit Unit number.
 * \param [in] data Array of Data fields
 * \param [out] ebst_cfg EBST control configuration.
 */
static void
bcmtm_ebst_control_cfg_parse(int unit,
                             const bcmltd_field_t *data,
                             bcmtm_ebst_control_cfg_t *ebst_cfg)
{
    const bcmltd_field_t *field;
    uint32_t fid;
    uint64_t fval;

    /* parsing data fields. */
    field = data;
    while (field) {
        fid = field->id;
        fval = field->data;
        switch (fid) {
            case TM_EBST_CONTROLt_SCAN_ROUNDf:
                ebst_cfg->scan_round = fval;
                break;
            case TM_EBST_CONTROLt_SCAN_MODEf:
                ebst_cfg->scan_mode = fval;
                break;
            case TM_EBST_CONTROLt_EBSTf:
                ebst_cfg->enable_mode = fval;
                break;
            case TM_EBST_CONTROLt_SCAN_THDf:
                ebst_cfg->scan_thd = fval;
                break;
            default:
                break;
        }
        field = field->next;
    }
}

/*!
 * \brief BCMTM EBST control physical table programming.
 *
 * \param [in] unit Logical unit.
 * \param [in] ltid Logical table ID.
 * \param [in] ebst_cfg EBST configuration.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_INIT Error in driver initialization.
 * \retval SHR_E_NOT_FOUND Fields not found.
 */
static int
bcmtm_ebst_control_install(int unit,
                           bcmltd_sid_t ltid,
                           bcmtm_ebst_control_cfg_t *ebst_cfg)
{
    bcmtm_ebst_enable_mode_t prev_mode;
    bcmtm_ebst_dev_info_t *ebst_info;
    bcmtm_ebst_drv_t ebst_drv;
    bcmtm_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    bcmtm_ebst_dev_info_get(unit, &ebst_info);
    prev_mode = ebst_info->enable_mode;

    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->ebst_drv_get, SHR_E_INIT);
    SHR_IF_ERR_EXIT
        (drv->ebst_drv_get(unit, &ebst_drv));

    /* When EBST mode is set to EBST_ENABLE_MODE_START. */
    if (ebst_cfg->enable_mode == EBST_ENABLE_MODE_START) {
        if (prev_mode == EBST_ENABLE_MODE_OFF) {
            /* Previous state is EBST_ENABLE_MODE_OFF, signifies EBST is off.
             * Configures the EBST thread for data collection.
             */
            /* Enable the EBST. */
            SHR_IF_ERR_EXIT(bcmtm_ebst_start(unit));
            /* Enable EBST. */
            if (ebst_drv.ebst_enable) {
                SHR_IF_ERR_EXIT(ebst_drv.ebst_enable(unit, ltid, true));
            }
        } else if (prev_mode == EBST_ENABLE_MODE_FULL) {
            /* EBST_FIFO_FULL interrupt is set. Clear the interrupt and enable
             * EBST fifo processing.
             */
            if (ebst_drv.ebst_enable) {
                SHR_IF_ERR_EXIT(ebst_drv.ebst_enable(unit, ltid, true));
            }
        }
    }

    /* Configure PT for EBST control configurations. */
    if (ebst_drv.ebst_scan_config_set) {
        SHR_IF_ERR_EXIT
            (ebst_drv.ebst_scan_config_set(unit, ltid, ebst_cfg));
    }

    /* Disabling EBST after setting disable to EBST PT. */
    if (prev_mode != EBST_ENABLE_MODE_OFF &&
            ebst_cfg->enable_mode == EBST_ENABLE_MODE_OFF) {
        /*! When set to OFF free all the resources mapped. */
        bcmtm_ebst_fifodma_stop(unit, true);
        /*! Disable EBST. */
        if (ebst_drv.ebst_enable) {
            SHR_IF_ERR_EXIT(ebst_drv.ebst_enable(unit, ltid, false));
        }
    } else if ((prev_mode == EBST_ENABLE_MODE_START ||
                prev_mode == EBST_ENABLE_MODE_FULL) &&
            ebst_cfg->enable_mode == EBST_ENABLE_MODE_STOP) {
        /*
         * When set to STOP only fifodma is stopped. Mapped resource
         * will not be freed.
         */
        bcmtm_ebst_fifodma_stop(unit, false);
        /*! Disable EBST. */
        if (ebst_drv.ebst_enable) {
            SHR_IF_ERR_EXIT(ebst_drv.ebst_enable(unit, ltid, false));
        }
    }
    /* update the ebst_info. */
    ebst_info->enable_mode = ebst_cfg->enable_mode;
    ebst_info->scan_mode = ebst_cfg->scan_mode;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief EBST control IMM table change callback function for staging.
 *
 * Handle EBST control IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid This is the logical table ID.
 * \param [in] event_reason This is the desired action for the entry.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * \param [out] output_fields Read only fields update to imm.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Unsupported event reason.
 * \retval SHR_E_FAIL BST fails to handle LT change events.
 */
static int
bcmtm_ebst_control_imm_stage_callback(int unit,
                                      bcmltd_sid_t ltid,
                                      uint32_t trans_id,
                                      bcmimm_entry_event_t event_reason,
                                      const bcmltd_field_t *key,
                                      const bcmltd_field_t *data,
                                      void *context,
                                      bcmltd_fields_t *output_fields)
{
    bcmtm_ebst_control_cfg_t ebst_cfg;
    int ebst_full_state[MAX_BUFFER_POOL] = {0};
    SHR_FUNC_ENTER(unit);

    sal_memset(&ebst_cfg, 0, sizeof(bcmtm_ebst_control_cfg_t));
    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_EXIT
                (bcmtm_ebst_control_imm_lkup(unit, &ebst_cfg));
            /* Fall-through */
        case BCMIMM_ENTRY_INSERT:
            bcmtm_ebst_control_cfg_parse(unit, data, &ebst_cfg);
            /* EBST_ENABLE_MODE_FULL is to be enabled by system only. */
            if (ebst_cfg.enable_mode == EBST_ENABLE_MODE_FULL) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            break;
        case BCMIMM_ENTRY_DELETE:
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /*! EBST control install to the physical table. */
    SHR_IF_ERR_EXIT
        (bcmtm_ebst_control_install(unit, ltid, &ebst_cfg));

    /* Update the status for TM_EBST_STATUS */
    SHR_IF_ERR_EXIT
        (bcmtm_ebst_status_update(unit, ebst_cfg.enable_mode, ebst_full_state));
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int
bcmtm_imm_ebst_control_register (int unit)
{
    const bcmlrd_map_t *map = NULL;
    int rv;

    /*
     * This structure contains callback functions that will be corresponding
     * to TM_EBST_CONTROL logical table entry commit stages.
     */
    static bcmimm_lt_cb_t ebst_control_imm_callback = {
        /*! Staging function. */
        .stage = bcmtm_ebst_control_imm_stage_callback,
    };

    SHR_FUNC_ENTER(unit);

    /*
     * To register callback for EBST CONTROL LT.
     */
    rv = bcmlrd_map_get(unit, TM_EBST_CONTROLt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 TM_EBST_CONTROLt,
                                 &ebst_control_imm_callback,
                                 NULL));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_ebst_control_imm_lkup(int unit,
                            bcmtm_ebst_control_cfg_t *ebst_cfg)
{
    bcmltd_fields_t out;
    bcmltd_sid_t ltid = TM_EBST_CONTROLt;

    SHR_FUNC_ENTER(unit);

    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, TM_EBST_CONTROLt_FIELD_COUNT, &out));

    if (SHR_SUCCESS(bcmimm_entry_lookup(unit, ltid, &out, &out))) {
        /* update ebst control configuration. */
        bcmtm_ebst_control_cfg_parse(unit, *(out.field), ebst_cfg);
    }
exit:
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}
