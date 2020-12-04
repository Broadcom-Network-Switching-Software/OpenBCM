/*! \file bcmtm_imm_scheduler_shaper_cpu_node.c
 *
 * In-memory call back function for scheduler shaper for scheduling nodes of
 * cpu port.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <shr/shr_debug.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_utils_internal.h>
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
bcmtm_scheduler_shaper_cpu_node_cfg_fill(int unit,
        const bcmltd_field_t *data,
        bcmtm_shaper_cfg_t *shaper_cfg,
        bcmtm_scheduler_cfg_t *scheduler_cfg)
{
    bcmltd_field_t *cur_data = (bcmltd_field_t *)data;
    while (cur_data) {
        switch (cur_data->id) {
            case TM_SCHEDULER_SHAPER_CPU_NODEt_SCHED_MODEf:
                scheduler_cfg->sched_mode = cur_data->data;
                break;
            case TM_SCHEDULER_SHAPER_CPU_NODEt_WEIGHTf:
                scheduler_cfg->weight = cur_data->data;
                break;
            case TM_SCHEDULER_SHAPER_CPU_NODEt_PARENT_TM_SCHEDULER_SHAPER_CPU_NODE_IDf:
                scheduler_cfg->parent_node = cur_data->data;
                break;
            case TM_SCHEDULER_SHAPER_CPU_NODEt_BURST_SIZE_AUTOf:
                shaper_cfg->burst_auto = cur_data->data;
                break;
            case TM_SCHEDULER_SHAPER_CPU_NODEt_MAX_BANDWIDTH_KBPSf:
                shaper_cfg->max_bucket_encode.bandwidth = cur_data->data;
                break;
            case TM_SCHEDULER_SHAPER_CPU_NODEt_MIN_BANDWIDTH_KBPSf:
                shaper_cfg->min_bucket_encode.bandwidth = cur_data->data;
                break;
            case TM_SCHEDULER_SHAPER_CPU_NODEt_MAX_BURST_SIZE_KBITSf:
                shaper_cfg->max_bucket_encode.burst = cur_data->data;
                break;
            case TM_SCHEDULER_SHAPER_CPU_NODEt_MIN_BURST_SIZE_KBITSf:
                shaper_cfg->min_bucket_encode.burst = cur_data->data;
                break;
            case TM_SCHEDULER_SHAPER_CPU_NODEt_SHAPING_MODEf:
                shaper_cfg->shaping_mode = cur_data->data;
                break;
            case TM_SCHEDULER_SHAPER_CPU_NODEt_ETHERNET_AVf:
                shaper_cfg->ethernet_av = cur_data->data;
                break;
            default:
                break;
        }
        cur_data = cur_data->next;
    }
}

/*!
 * \brief  Scheduler shaper cpu node physical table configuration based on
 * device type.
 *
 * \param [in] unit  Unit number.
 * \param [in] sched_node_id Scheduler node ID.
 * \param [in] sched_level Scheduler level.
 * \param [in] ltid Logical table ID.
 * \param [in] shaper_cfg Shaper configuration to be programmed.
 * \param [in] scheduler_cfg Scheduler configuration to be programmed.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_scheduler_shaper_cpu_node_pt_set(int unit,
                                       uint32_t sched_node_id,
                                       uint32_t sched_level,
                                       bcmltd_sid_t ltid,
                                       bcmtm_shaper_cfg_t *shaper_cfg,
                                       bcmtm_scheduler_cfg_t *scheduler_cfg)
{
    bcmtm_lport_t lport = 0;
    bcmtm_drv_t *drv;
    bcmtm_scheduler_drv_t sched_drv;
    bcmtm_shaper_drv_t shaper_drv;


    SHR_FUNC_ENTER(unit);

    sal_memset(&sched_drv, 0, sizeof(bcmtm_scheduler_drv_t));
    sal_memset(&shaper_drv, 0, sizeof(bcmtm_shaper_drv_t));

    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->scheduler_drv_get, SHR_E_UNAVAIL);
    SHR_NULL_CHECK(drv->shaper_drv_get, SHR_E_UNAVAIL);

    SHR_IF_ERR_EXIT(drv->scheduler_drv_get(unit, &sched_drv));
    SHR_IF_ERR_EXIT(drv->shaper_drv_get(unit, &shaper_drv));

    /* convert bandwidth and burst size to Physical table readable format. */
    if (shaper_cfg->action != ACTION_DELETE) {
        SHR_NULL_CHECK(shaper_drv.rate_to_bucket, SHR_E_INTERNAL);

        SHR_IF_ERR_EXIT
            (shaper_drv.rate_to_bucket(unit, lport,
                                       shaper_cfg->shaping_mode,
                                       shaper_cfg->burst_auto,
                                       &(shaper_cfg->min_bucket_encode)));

        SHR_IF_ERR_EXIT
            (shaper_drv.rate_to_bucket(unit, lport,
                                         shaper_cfg->shaping_mode,
                                         shaper_cfg->burst_auto,
                                         &(shaper_cfg->max_bucket_encode)));

    }
    /* update physical table. */
    if (shaper_drv.cpu_node_set) {
        SHR_IF_ERR_EXIT
            (shaper_drv.cpu_node_set(unit, sched_node_id,
                                    sched_level, ltid, shaper_cfg));
    }
    if (sched_drv.cpu_node_set) {
        SHR_IF_ERR_EXIT
            (sched_drv.cpu_node_set(unit, sched_node_id,
                                     sched_level, ltid, scheduler_cfg));
    }

    if (shaper_cfg->action != ACTION_DELETE) {
        SHR_NULL_CHECK(shaper_drv.bucket_to_rate, SHR_E_INTERNAL);
        SHR_IF_ERR_EXIT
            (shaper_drv.bucket_to_rate(unit,
                                       shaper_cfg->shaping_mode,
                                       &(shaper_cfg->max_bucket_encode)));

        SHR_IF_ERR_EXIT
            (shaper_drv.bucket_to_rate(unit,
                                       shaper_cfg->shaping_mode,
                                       &(shaper_cfg->min_bucket_encode)));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief  Resets the physical table to default values.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] key  This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_scheduler_shaper_cpu_node_reset(int unit,
        bcmltd_sid_t ltid,
        const bcmltd_field_t *key,
        bcmtm_shaper_cfg_t *shaper_cfg,
        bcmtm_scheduler_cfg_t *scheduler_cfg)
{
    uint32_t sched_node_id, sched_level;
    bcmltd_fid_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    /* TM_SCHEDULER_SHAPER_CPU_NODEt_SCHED_NODEf*/
    fid = TM_SCHEDULER_SHAPER_CPU_NODEt_SCHED_NODEf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    sched_level = (uint32_t)fval;

    /* TM_SCHEDULER_SHAPER_CPU_NODEt_TM_SCHEDULER_SHAPER_CPU_NODE_IDf */
    fid = TM_SCHEDULER_SHAPER_CPU_NODEt_TM_SCHEDULER_SHAPER_CPU_NODE_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    sched_node_id = (uint32_t)fval;

    shaper_cfg->action = ACTION_DELETE;

    /* Physical table update */
    SHR_IF_ERR_EXIT
        (bcmtm_scheduler_shaper_cpu_node_pt_set(unit, sched_node_id, sched_level,
                                         ltid, shaper_cfg, scheduler_cfg));
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Configures the physical table with user defined values.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] key  This is a linked list of the key fields identifying
 * the entry.
 * \param [in] shaper_cfg Shaper configurations.
 * \param [in] scheduler_cfg Scheduler configurations.
 * \param [out] output_fields This is linked list of extra added data fields
 * (mostly read only fields.)
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_scheduler_shaper_cpu_node_set(int unit,
        bcmltd_sid_t ltid,
        const bcmltd_field_t *key,
        bcmtm_shaper_cfg_t *shaper_cfg,
        bcmtm_scheduler_cfg_t *scheduler_cfg,
        bcmltd_fields_t *output_fields)
{
    bcmltd_fid_t fid;
    uint32_t sched_node_id, sched_level;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    /* TM_SCHEDULER_SHAPER_CPU_NODEt_SCHED_NODEf*/
    fid = TM_SCHEDULER_SHAPER_CPU_NODEt_SCHED_NODEf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    sched_level = (uint32_t)fval;

    /* TM_SCHEDULER_SHAPER_CPU_NODEt_TM_SCHEDULER_SHAPER_CPU_NODE_IDf */
    fid = TM_SCHEDULER_SHAPER_CPU_NODEt_TM_SCHEDULER_SHAPER_CPU_NODE_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    sched_node_id = (uint32_t)fval;

    /*
     * CPU node has only multicast queue. It does not support unicast queue.
     * Parent node is applicable only for L1 sched multicast queue.
     */
    if ((sched_level == L0_SCHED_COS) && (scheduler_cfg->parent_node != 0xff)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* PT update */
    SHR_IF_ERR_EXIT
        (bcmtm_scheduler_shaper_cpu_node_pt_set(unit, sched_node_id, sched_level,
                                                ltid, shaper_cfg, scheduler_cfg));

    /* update the readonly fields.
     * 1. TM_SCHEDULER_SHAPER_CPU_NODEt_MAX_BANDWIDTH_KBPS_OPERf
     * 2. TM_SCHEDULER_SHAPER_CPU_NODEt_MAX_BURST_KBITS_OPERf
     * 3. TM_SCHEDULER_SHAPER_CPU_NODEt_MIN_BANDWIDTH_KBPS_OPERf
     * 4. TM_SCHEDULER_SHAPER_CPU_NODEt_MIN_BURST_KBITS_OPERf
     * 5. TM_SCHEDULER_SHAPER_CPU_NODEt_OPERATIONAL_STATEf
     */
    fid = TM_SCHEDULER_SHAPER_CPU_NODEt_MAX_BANDWIDTH_KBPS_OPERf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, output_fields, fid, 0,
                         shaper_cfg->max_bucket_encode.bandwidth));

    fid = TM_SCHEDULER_SHAPER_CPU_NODEt_MAX_BURST_SIZE_KBITS_OPERf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, output_fields, fid, 0,
                         shaper_cfg->max_bucket_encode.burst));

    fid = TM_SCHEDULER_SHAPER_CPU_NODEt_MIN_BANDWIDTH_KBPS_OPERf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, output_fields, fid, 0,
                         shaper_cfg->min_bucket_encode.bandwidth));

    fid = TM_SCHEDULER_SHAPER_CPU_NODEt_MIN_BURST_SIZE_KBITS_OPERf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, output_fields, fid, 0,
                         shaper_cfg->min_bucket_encode.burst));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM_SHAPER_NODE logical table callback function for staging.
 *
 * Handle TM_SCHEDULER_NODE IMM table change events.
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
bcmtm_scheduler_shaper_cpu_node_stage(int unit,
        bcmltd_sid_t ltid,
        uint32_t trans_id,
        bcmimm_entry_event_t event_reason,
        const bcmltd_field_t *key,
        const bcmltd_field_t *data,
        void *context,
        bcmltd_fields_t *output_fields)
{
    bcmltd_fields_t out, in;
    bcmltd_field_t *cur_key = (bcmltd_field_t *)key;
    size_t num_fid;
    bcmtm_shaper_cfg_t shaper_cfg;
    bcmtm_scheduler_cfg_t scheduler_cfg;

    SHR_FUNC_ENTER(unit);

    sal_memset(&shaper_cfg, 0, sizeof(bcmtm_shaper_cfg_t));
    sal_memset(&scheduler_cfg, 0, sizeof(bcmtm_scheduler_cfg_t));

    /* Updating default values for scheduler configuration. */
    if (event_reason != BCMIMM_ENTRY_DELETE) {
        scheduler_cfg.sched_mode = DEFAULT_SCHED_MODE;
        scheduler_cfg.weight = DEFAULT_WEIGHT;
        /* Reset to invalid value to prevent programming for invalid
         * scheduler node levels.
         */
        scheduler_cfg.parent_node = 0xff;
    }
    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
            sal_memset(&in, 0, sizeof(bcmltd_fields_t));
            sal_memset(&out, 0, sizeof(bcmltd_fields_t));
            SHR_IF_ERR_EXIT(bcmlrd_field_count_get(unit, ltid, &num_fid));
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_alloc(unit, num_fid, &out));
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_alloc(unit, 2, &in));
            in.count = 0;
            while (cur_key) {
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, &in, cur_key->id, 0, cur_key->data));
                cur_key = cur_key->next;
            }
            SHR_IF_ERR_EXIT
                (bcmimm_entry_lookup(unit, ltid, &in, &out));
            bcmtm_scheduler_shaper_cpu_node_cfg_fill(unit, *(out.field),
                    &shaper_cfg, &scheduler_cfg);
            /* Fall through */
        case BCMIMM_ENTRY_INSERT:
            SHR_NULL_CHECK(output_fields, SHR_E_INTERNAL);
            output_fields->count = 0;
            bcmtm_scheduler_shaper_cpu_node_cfg_fill(unit, data,
                    &shaper_cfg, &scheduler_cfg);
            shaper_cfg.action = ACTION_INSERT;
            SHR_IF_ERR_EXIT
                (bcmtm_scheduler_shaper_cpu_node_set(unit, ltid, key,
                                      &shaper_cfg, &scheduler_cfg, output_fields));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT
                (bcmtm_scheduler_shaper_cpu_node_reset(unit, ltid, key,
                                                       &shaper_cfg, &scheduler_cfg));
            break;
        default:
            break;
    }
exit:
    if (event_reason == BCMIMM_ENTRY_UPDATE) {
        bcmtm_field_list_free(&in);
        bcmtm_field_list_free(&out);
    }
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int
bcmtm_scheduler_shaper_cpu_node_imm_register(int unit)
{
    /*!
     * Inmemory callback functions for TM_SCHEDULER_SHAPER_CPU_NODEt logical
     * table.
     */
    const bcmlrd_map_t *map = NULL;
    static bcmimm_lt_cb_t bcmtm_scheduler_shaper_cpu_node_imm_cb = {
        /*! Staging function. */
        .stage = bcmtm_scheduler_shaper_cpu_node_stage,
    };
    int rv;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, TM_SCHEDULER_SHAPER_CPU_NODEt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    /*! Registers callback functions for Shaper node. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_SCHEDULER_SHAPER_CPU_NODEt,
                             &bcmtm_scheduler_shaper_cpu_node_imm_cb, NULL));
exit:
    SHR_FUNC_EXIT();
}
