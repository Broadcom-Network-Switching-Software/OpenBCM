/*! \file bcmtm_imm_shaper_port.c
 *
 * In-memory call back function for shapers for Ports.
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
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/sched_shaper/bcmtm_shaper_internal.h>
#include <bcmtm/sched_shaper/bcmtm_scheduler_internal.h>
#include <bcmtm/bcmtm_shaper_encoding_internal.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>
/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM

/*******************************************************************************
 * Private functions
 */
static int
bcmtm_shaper_port_imm_update(int unit,
                             bcmltd_sid_t ltid,
                             bcmtm_lport_t lport,
                             bcmtm_shaper_cfg_t *shaper_cfg)
{
    bcmltd_fields_t in;
    bcmltd_fid_t fid;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 4, &in));

    in.count = 0;

    /* TM_SHAPER_PORTt_PORT_IDf */
    fid = TM_SHAPER_PORTt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, lport));

    /* TM_SHAPER_PORTt_OPERATIONAL_STATEf */
    fid = TM_SHAPER_PORTt_OPERATIONAL_STATEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, shaper_cfg->opcode));

    if (shaper_cfg->action != ACTION_PORT_DELETE_INT) {
        /* TM_SHAPER_PORTt_BANDWIDTH_KBPS_OPERf */
        fid = TM_SHAPER_PORTt_BANDWIDTH_KBPS_OPERf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, &in, fid, 0,
                                  shaper_cfg->max_bucket_encode.bandwidth));

        /* TM_SHAPER_PORTt_BURST_SIZE_KBITS_OPERf */
        fid = TM_SHAPER_PORTt_BURST_SIZE_KBITS_OPERf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, &in, fid, 0,
                                  shaper_cfg->max_bucket_encode.burst));
    }
    SHR_IF_ERR_EXIT
        (bcmimm_entry_update(unit, 0, ltid, &in));
exit:
    bcmtm_field_list_free(&in);
    SHR_FUNC_EXIT();
}

/*!
 * \brief  Populates shaper configuration using user defined values.
 *
 * \param [in] unit Unit number.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [out] shaper_cfg Shaper configuration populated.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static void
bcmtm_shaper_port_cfg_fill(int unit,
                           const bcmltd_field_t *data,
                           bcmtm_shaper_cfg_t *shaper_cfg)
{
    bcmltd_field_t *cur_data = (bcmltd_field_t *)data;
    /* enabled by default */
    shaper_cfg->burst_auto = 1;
    shaper_cfg->ifg_encap = 1;
    while (cur_data) {
        switch (cur_data->id) {
            case TM_SHAPER_PORTt_SHAPING_MODEf:
                shaper_cfg->shaping_mode = cur_data->data;
                break;
            case TM_SHAPER_PORTt_BANDWIDTH_KBPSf:
                shaper_cfg->max_bucket_encode.bandwidth = cur_data->data;
                break;
            case TM_SHAPER_PORTt_BURST_SIZE_KBITSf:
                shaper_cfg->max_bucket_encode.burst = cur_data->data;
                break;
            case TM_SHAPER_PORTt_BURST_SIZE_AUTOf:
                shaper_cfg->burst_auto = cur_data->data;
                break;
            case TM_SHAPER_PORTt_ETHERNET_AVf:
                shaper_cfg->ethernet_av = cur_data->data;
                break;
            case TM_SHAPER_PORTt_INTER_FRAME_GAP_BYTEf:
                shaper_cfg->byte_count_inter_frame_gap = cur_data->data;
                break;
            case TM_SHAPER_PORTt_INTER_FRAME_GAP_ENCAPf:
                shaper_cfg->ifg_encap = cur_data->data;
                break;
            case TM_SHAPER_PORTt_INTER_FRAME_GAP_HIGIG2_BYTEf:
                shaper_cfg->byte_count_inter_frame_gap_higig2 = cur_data->data;
                break;
            case TM_SHAPER_PORTt_HULL_MODEf:
                shaper_cfg->hull_mode = cur_data->data;
                break;
            case TM_SHAPER_PORTt_OPERATIONAL_STATEf:
                shaper_cfg->opcode = cur_data->data;
                break;
        }
        cur_data = cur_data->next;
    }
}

/*!
 * \brief  Shaper port physical table configuration based on device type.
 *
 * \param [in] unit  Unit number.
 * \param [in] lport Logical port number.
 * \param [in] ltid Logical table ID.
 * \param [in] shaper_cfg Shaper configuration to be programmed.
 * \param [in] enable Enables metering functinality.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_shaper_port_pt_set(int unit,
        bcmtm_lport_t lport,
        bcmltd_sid_t ltid,
        bcmtm_shaper_cfg_t *shaper_cfg,
        int enable)
{
    bcmtm_drv_t *drv;
    bcmtm_shaper_drv_t shaper_drv;

    SHR_FUNC_ENTER(unit);
    sal_memset(&shaper_drv, 0, sizeof(bcmtm_shaper_drv_t));

    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->shaper_drv_get, SHR_E_UNAVAIL);

    SHR_IF_ERR_EXIT(drv->shaper_drv_get(unit, &shaper_drv));
    /* Convert bandwidth and burst size to Physical Table readable format.*/
    if (shaper_cfg->action != ACTION_DELETE) {
        SHR_NULL_CHECK(shaper_drv.rate_to_bucket, SHR_E_INTERNAL);
        SHR_IF_ERR_EXIT
            (shaper_drv.rate_to_bucket(unit, lport,
                                       shaper_cfg->shaping_mode,
                                       shaper_cfg->burst_auto,
                                       &(shaper_cfg->max_bucket_encode)));
    }
    /* update physical table. */
    if (shaper_drv.port_set) {
        SHR_IF_ERR_EXIT
            (shaper_drv.port_set(unit, lport, ltid, shaper_cfg, enable));
    }

    /* Retrieve the hardware configure values */
    if (shaper_cfg->action != ACTION_DELETE) {
        SHR_NULL_CHECK(shaper_drv.bucket_to_rate, SHR_E_INTERNAL);
        SHR_IF_ERR_EXIT
            (shaper_drv.bucket_to_rate(unit, shaper_cfg->shaping_mode,
                                       &(shaper_cfg->max_bucket_encode)));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configures the physical table with reset values.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] shaper_cfg Shaper configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_shaper_port_reset(int unit,
                        bcmltd_sid_t ltid,
                        bcmtm_lport_t lport,
                        bcmtm_shaper_cfg_t *shaper_cfg)
{
    SHR_FUNC_ENTER(unit);

    /*
     * Physical table will be updated only if opcode is avlid.
     */
    if (shaper_cfg->opcode == VALID) {
        sal_memset(shaper_cfg, 0, sizeof(bcmtm_shaper_cfg_t));
        shaper_cfg->action = ACTION_DELETE;
        /* PT update */
        SHR_IF_ERR_EXIT
            (bcmtm_shaper_port_pt_set(unit, lport, ltid, shaper_cfg, 0));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configures the physical table with user defined values.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] shaper_cfg Shaper configuration.
 * \param [out] output_fields This is linked list of extra added data fields
 * (mostly read only fields.)
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_shaper_port_set(int unit,
                      bcmltd_sid_t ltid,
                      bcmtm_lport_t lport,
                      bcmtm_shaper_cfg_t *shaper_cfg,
                      bcmltd_fields_t *output_fields)
{
    bcmltd_fid_t fid;
    int mport;
    uint8_t action;

    SHR_FUNC_ENTER(unit);

    action =  shaper_cfg->action;
    switch (action) {

        case ACTION_INSERT:
            /* TM local port information.
             * Returns error if local port to physical port map is not available.
             * Updates operational state.
             */
            if (SHR_FAILURE(bcmtm_lport_mmu_local_port_get(unit, lport, &mport))) {
                shaper_cfg->opcode = PORT_INFO_UNAVAIL;
                fid = TM_SHAPER_PORTt_OPERATIONAL_STATEf;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, output_fields, fid,
                                          0, shaper_cfg->opcode));
                SHR_EXIT();
            }
            shaper_cfg->opcode = VALID;
            break;
        case ACTION_PORT_UPDATE_INT:
            shaper_cfg->opcode = VALID;
            break;
        case ACTION_PORT_DELETE_INT:
            SHR_IF_ERR_EXIT
                (bcmtm_shaper_port_reset(unit, ltid, lport, shaper_cfg));
            shaper_cfg->opcode = PORT_INFO_UNAVAIL;
            break;
        default:
            break;
    }

    if (shaper_cfg->opcode == VALID) {
        /* PT update */
        SHR_IF_ERR_EXIT
            (bcmtm_shaper_port_pt_set(unit, lport, ltid, shaper_cfg, 1));

    }

    if (action != ACTION_INSERT) {
        SHR_EXIT();
    }
    /* update the readonly fields.
     * 1. TM_SHAPER_PORTt_MAX_BANDWIDTH_KBPS_OPERf
     * 2. TM_SHAPER_PORTt_MAX_BURST_KBITS_OPERf
     * 3. TM_SHAPER_PORTt_OPERATIONAL_STATEf
     */
    fid = TM_SHAPER_PORTt_BANDWIDTH_KBPS_OPERf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, output_fields, fid, 0,
                              shaper_cfg->max_bucket_encode.bandwidth));

    fid = TM_SHAPER_PORTt_BURST_SIZE_KBITS_OPERf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, output_fields, fid, 0,
                              shaper_cfg->max_bucket_encode.burst));

    fid = TM_SHAPER_PORTt_OPERATIONAL_STATEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, output_fields, fid, 0, shaper_cfg->opcode));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM shaper port imm entry lookup.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port ID.
 * \param [out] shaper_cfg Shaper configuration.
 */
static int
bcmtm_shaper_port_imm_lkup(int unit,
                           bcmltd_sid_t ltid,
                           bcmtm_lport_t lport,
                           bcmtm_shaper_cfg_t *shaper_cfg)
{
    bcmltd_fields_t out, in;
    size_t num_fid;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, ltid, &num_fid));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &out));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 1, &in));
    in.count = 0;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, TM_SHAPER_PORTt_PORT_IDf, 0, lport));

    if (SHR_E_NONE != bcmimm_entry_lookup(unit, ltid, &in, &out)) {
        shaper_cfg->opcode = ENTRY_INVALID;
        SHR_EXIT();
    }
    /* update shaper port configuration based on imm lookup. */
    bcmtm_shaper_port_cfg_fill(unit, *(out.field), shaper_cfg);
exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}
/*!
 * \brief TM_SHAPER_PORT logical table callback function for staging.
 *
 * Handle TM_SHAPER_PORT IMM table change events.
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
bcmtm_shaper_port_stage(int unit,
        bcmltd_sid_t ltid,
        uint32_t trans_id,
        bcmimm_entry_event_t event_reason,
        const bcmltd_field_t *key,
        const bcmltd_field_t *data,
        void *context,
        bcmltd_fields_t *output_fields)
{
    bcmtm_lport_t lport;
    uint64_t fval;
    bcmltd_fid_t fid;
    bcmltd_field_t *cur_key = (bcmltd_field_t *)cur_key;
    bcmtm_shaper_cfg_t shaper_cfg;

    SHR_FUNC_ENTER(unit);

    fid = TM_SHAPER_PORTt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0 , key, &fval));
    lport = (uint32_t) fval;

    sal_memset(&shaper_cfg, 0, sizeof(bcmtm_shaper_cfg_t));

    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_EXIT
                (bcmtm_shaper_port_imm_lkup(unit, ltid, lport, &shaper_cfg));
        /* Fall through */
        case BCMIMM_ENTRY_INSERT:
            SHR_NULL_CHECK(output_fields, SHR_E_PARAM);
            output_fields->count = 0;
            bcmtm_shaper_port_cfg_fill(unit, data, &shaper_cfg);
            shaper_cfg.action = ACTION_INSERT;
            SHR_IF_ERR_EXIT
                (bcmtm_shaper_port_set(unit, ltid, lport, &shaper_cfg,
                                       output_fields));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT
                (bcmtm_shaper_port_imm_lkup(unit, ltid, lport, &shaper_cfg));
            SHR_IF_ERR_EXIT
                (bcmtm_shaper_port_reset(unit, ltid, lport, &shaper_cfg));
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
bcmtm_shaper_port_internal_update(int unit,
                                  bcmtm_lport_t lport,
                                  uint8_t action)
{
    bcmltd_sid_t ltid = TM_SHAPER_PORTt;
    bcmtm_shaper_cfg_t shaper_cfg;

    SHR_FUNC_ENTER(unit);

    sal_memset(&shaper_cfg, 0, sizeof(bcmtm_shaper_cfg_t));

    SHR_IF_ERR_EXIT
        (bcmtm_shaper_port_imm_lkup(unit, ltid, lport, &shaper_cfg));

    shaper_cfg.action = action;
    if (shaper_cfg.opcode != ENTRY_INVALID) {
        SHR_IF_ERR_EXIT
            (bcmtm_shaper_port_set(unit, ltid, lport, &shaper_cfg, NULL));
        SHR_IF_ERR_EXIT
            (bcmtm_shaper_port_imm_update(unit, ltid, lport, &shaper_cfg));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_shaper_port_imm_register(int unit)
{
    /*! Inmemory callback functions for TM_SHAPER_PORTt logical table. */
    static bcmimm_lt_cb_t bcmtm_shaper_port_imm_cb = {
        /*! Staging function. */
        .stage = bcmtm_shaper_port_stage,
    };
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);
    rv = bcmlrd_map_get(unit, TM_SHAPER_PORTt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }
    /*! Registers callback functions for Shaper port. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_SHAPER_PORTt,
                             &bcmtm_shaper_port_imm_cb, NULL));
exit:
    SHR_FUNC_EXIT();
}
