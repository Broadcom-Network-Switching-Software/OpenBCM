/*! \file bcm56880_a0_tm_port_mc_q_to_service_pool.c
 *
 * IMM handlers for TM ingress mapping LTs.
 *
 * This file contains callbcak handlers for LT TM_PORT_MC_Q_TO_SERVICE_POOL.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/chip/bcm56880_a0_enum.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmtm/chip/bcm56880_a0_tm_thd_internal.h>
#include <bcmtm/chip/bcm56880_a0_tm.h>

#include "bcm56880_a0_tm_imm_internal.h"

#define TD4_MAX_NUM_MC_Q 6

/* TM_PORT_MC_Q_ENTRY_STATE_T */
#define VALID                   0
#define PORT_INFO_UNAVAILABLE   1
#define MC_Q_INVALID            2

#define NUM_KEY_FIELDS          2
#define NUM_OPER_FIELDS         1

#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM_CHIP

static int
tm_port_mc_q_to_service_pool_populate_entry(int unit, int lport, int queue,
                                            int up, soc_mmu_cfg_buf_t *buf)
{
    bcmltd_fields_t entry;
    bcmltd_sid_t sid = TM_PORT_MC_Q_TO_SERVICE_POOLt;
    uint32_t oper_state;
    size_t num_fid;
    int num_q;
    soc_mmu_cfg_buf_queue_t *buf_queue;

    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(bcmltd_fields_t));
    buf_queue = &buf->ports[lport].queues[queue];

    /*
     * Populate default entry (value fields) with Arch default values.
     * Key fields must be set.
     */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, sid, &num_fid));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &entry));
    SHR_IF_ERR_EXIT
        (bcmtm_default_entry_array_construct(unit, sid, &entry));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
                         TM_PORT_MC_Q_TO_SERVICE_POOLt_PORT_IDf, 0, lport));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
                         TM_PORT_MC_Q_TO_SERVICE_POOLt_TM_MC_Q_IDf, 0, queue));

    oper_state = PORT_INFO_UNAVAILABLE;
    /* Check for invalid queue (number of MC queue for the port). */
    if (!bcmtm_lport_is_cpu(unit, lport)) {
        num_q = bcm56880_a0_tm_num_mc_q_non_cpu_port_get(unit);
    } else {
        num_q = SOC_TD4_NUM_CPU_QUEUES;
    }
    if (queue >= num_q) {
        oper_state = MC_Q_INVALID;
    } else {
        /* Populate user-configured fields during init. */
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, &entry,
                TM_PORT_MC_Q_TO_SERVICE_POOLt_TM_EGR_SERVICE_POOL_IDf, 0,
                buf_queue->pool_idx));
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, &entry,
                TM_PORT_MC_Q_TO_SERVICE_POOLt_USE_QGROUP_MINf, 0,
                buf_queue->qgroup_min_enable));
    }
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_PORT_MC_Q_TO_SERVICE_POOLt_OPERATIONAL_STATEf, 0, oper_state));

    SHR_IF_ERR_EXIT
        (bcmimm_entry_insert(unit, sid, &entry));

exit:
    bcmtm_field_list_free(&entry);
    SHR_FUNC_EXIT();
}

static int
tm_port_mc_q_to_service_pool_update(int unit,
                                    bcmltd_sid_t sid,
                                    int lport,
                                    int queue,
                                    const bcmltd_field_t *data_fields,
                                    bcmltd_fields_t *output_fields)
{
    const bcmltd_field_t *data_ptr;
    bcmdrd_sid_t ptid;
    bcmtm_pt_info_t pt_dyn_info = {0};
    uint32_t ltmbuf[BCMTM_MAX_THD_ENTRY_WSIZE] = {0};
    uint32_t fval;
    int pport, num_q, midx, base, mmu_queue, num_uc_q;

   bcmdrd_fid_t field_qspid[12] = {
            QUEUE0_SPIDf, QUEUE1_SPIDf, QUEUE2_SPIDf,
            QUEUE3_SPIDf, QUEUE4_SPIDf, QUEUE5_SPIDf,
            QUEUE6_SPIDf, QUEUE7_SPIDf, QUEUE8_SPIDf,
            QUEUE9_SPIDf, QUEUE10_SPIDf, QUEUE11_SPIDf};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_size_check(unit, output_fields, NUM_OPER_FIELDS));
    output_fields->count = 0;

    /* Check for invalid port (port is not added). */
    if (SHR_FAILURE(bcmtm_lport_pport_get(unit, lport, &pport))) {
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, output_fields,
                             TM_PORT_MC_Q_TO_SERVICE_POOLt_OPERATIONAL_STATEf,
                             0, PORT_INFO_UNAVAILABLE));
        SHR_EXIT();
    }
    /* Check for invalid queue (number of MC queue for the port). */
    SHR_IF_ERR_EXIT
        (bcmtm_lport_num_mcq_get(unit, lport, &num_q));
    if (queue >= num_q) {
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, output_fields,
                             TM_PORT_MC_Q_TO_SERVICE_POOLt_OPERATIONAL_STATEf,
                             0, MC_Q_INVALID));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmtm_lport_mcq_base_get(unit, lport, &base));
    SHR_IF_ERR_EXIT
        (bcmtm_lport_num_ucq_get(unit, lport, &num_uc_q));
    mmu_queue = num_uc_q + queue;

    /* Insert/update command with user configured value. */
    data_ptr = data_fields;
    while (data_ptr) {
        fval = data_ptr->data;
        switch (data_ptr->id) {
            case TM_PORT_MC_Q_TO_SERVICE_POOLt_TM_EGR_SERVICE_POOL_IDf:

                if (!bcmtm_lport_is_cpu(unit, lport)) {
                    /* CMIC has 48 queues... so not applicalble for cmic port */
                    /* Update MMU_THDO_PORT_QUEUE_SERVICE_POOL. */
                    ptid = MMU_THDO_PORT_QUEUE_SERVICE_POOLm;
                    midx = td4_piped_mem_index(unit, lport, ptid, 0);
                    BCMTM_PT_DYN_INFO(pt_dyn_info, midx, 0);
                    SHR_IF_ERR_EXIT
                        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
                    SHR_IF_ERR_EXIT
                        (bcmtm_field_set(unit, ptid, field_qspid[mmu_queue], ltmbuf, &fval));
                    SHR_IF_ERR_EXIT
                        (bcmtm_pt_indexed_write(unit, ptid, sid, &pt_dyn_info, ltmbuf));
                }

                /* Update MMU_THDO_QUEUE_CONFIG. */
                ptid = MMU_THDO_QUEUE_CONFIGm;
                BCMTM_PT_DYN_INFO(pt_dyn_info, base + queue, 0);
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, ptid, SPIDf, ltmbuf, &fval));
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_write(unit, ptid, sid, &pt_dyn_info, ltmbuf));
                ptid = MMU_THDO_QUEUE_CONFIG1m;
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_write(unit, ptid, sid, &pt_dyn_info, ltmbuf));

                /* Update MMU_THDO_Q_TO_QGRP_MAPD. */
                ptid = MMU_THDO_Q_TO_QGRP_MAPD0m;
                BCMTM_PT_DYN_INFO(pt_dyn_info, base + queue, 0);
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, ptid, SPIDf, ltmbuf, &fval));
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_write(unit, ptid, sid, &pt_dyn_info, ltmbuf));
                break;
            case TM_PORT_MC_Q_TO_SERVICE_POOLt_USE_QGROUP_MINf:

                /* Update MMU_THDO_QUEUE_CONFIG. */
                ptid = MMU_THDO_QUEUE_CONFIGm;
                BCMTM_PT_DYN_INFO(pt_dyn_info, base + queue, 0);
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, ptid, USE_QGROUP_MINf, ltmbuf, &fval));
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_write(unit, ptid, sid, &pt_dyn_info, ltmbuf));
                ptid = MMU_THDO_QUEUE_CONFIG1m;
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_write(unit, ptid, sid, &pt_dyn_info, ltmbuf));

                /* Update MMU_THDO_Q_TO_QGRP_MAPD. */
                ptid = MMU_THDO_Q_TO_QGRP_MAPD0m;
                BCMTM_PT_DYN_INFO(pt_dyn_info, base + queue, 0);
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, ptid, USE_QGROUP_MINf, ltmbuf, &fval));
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_write(unit, ptid, sid, &pt_dyn_info, ltmbuf));
                break;
        }
        data_ptr = data_ptr->next;
    }
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, output_fields,
                         TM_PORT_MC_Q_TO_SERVICE_POOLt_OPERATIONAL_STATEf,
                         0, VALID));

exit:
    SHR_FUNC_EXIT();
}

static int
tm_port_mc_q_to_service_pool_delete(int unit, bcmltd_sid_t sid, int lport, int queue)
{
    /* Should reset all fields to HW default when delete. */
    bcmltd_field_t *data_fields = NULL;
    bcmltd_fields_t dummy_fields;
    size_t num_fid;

    SHR_FUNC_ENTER(unit);

    sal_memset(&dummy_fields, 0, sizeof(bcmltd_fields_t));

    /* Create data_fields with default field values. */
    SHR_IF_ERR_EXIT
        (bcmtm_default_entry_ll_construct(unit, sid, &data_fields));

    /* Reuse update callback to write HW. */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, sid, &num_fid));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &dummy_fields));

    SHR_IF_ERR_EXIT
        (tm_port_mc_q_to_service_pool_update(unit, sid, lport, queue, data_fields,
                                             &dummy_fields));

exit:
    SHR_FREE(data_fields);
    bcmtm_field_list_free(&dummy_fields);
    SHR_FUNC_EXIT();
}

static int
tm_port_mc_q_to_service_pool_stage(int unit,
                          bcmltd_sid_t sid,
                          uint32_t trans_id,
                          bcmimm_entry_event_t event_reason,
                          const bcmltd_field_t *key_fields,
                          const bcmltd_field_t *data_fields,
                          void *context,
                          bcmltd_fields_t *output_fields)
{
    const bcmltd_field_t *key_ptr;
    int lport = -1;
    int queue = -1;

    SHR_FUNC_ENTER(unit);

    key_ptr = key_fields;
    while (key_ptr) {
        switch (key_ptr->id) {
            case TM_PORT_MC_Q_TO_SERVICE_POOLt_PORT_IDf:
                lport = key_ptr->data;
                break;
            case TM_PORT_MC_Q_TO_SERVICE_POOLt_TM_MC_Q_IDf:
                queue = key_ptr->data;
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        key_ptr = key_ptr->next;
    }

    if (lport < 0 || queue < 0) {
        return SHR_E_PARAM;
    }

    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
            /* Insert shares the same callback with update. */
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_EXIT
                (tm_port_mc_q_to_service_pool_update(unit, sid, lport, queue,
                                                     data_fields, output_fields));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT
                (tm_port_mc_q_to_service_pool_delete(unit, sid, lport, queue));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */

int
bcm56880_a0_tm_port_mc_q_to_service_pool_register(int unit)
{
    bcmimm_lt_cb_t tm_thd_cb_handler;
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /*
     * Assign callback functions to IMM event callback handler.
     * commit and abort functions can be added if necessary.
     */
    sal_memset(&tm_thd_cb_handler, 0, sizeof(tm_thd_cb_handler));
    tm_thd_cb_handler.stage    = tm_port_mc_q_to_service_pool_stage;

    rv = bcmlrd_map_get(unit, TM_PORT_MC_Q_TO_SERVICE_POOLt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    /* Register callback handler for the LT. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_PORT_MC_Q_TO_SERVICE_POOLt, &tm_thd_cb_handler, NULL));

exit:
    SHR_FUNC_EXIT();
}

int
bcm56880_a0_tm_port_mc_q_to_service_pool_populate(int unit)
{
    int lport, queue, max_num_mcq, rv;
    const bcmlrd_map_t *map = NULL;
    bcmtm_drv_info_t *drv_info;
    soc_mmu_cfg_buf_t *buf;
    soc_mmu_device_info_t devcfg;

    SHR_FUNC_ENTER(unit);

    bcmtm_drv_info_get(unit, &drv_info);
    buf = soc_mmu_buf_cfg_alloc(unit);
    if (!buf) {
        return SHR_E_MEMORY;
    }
    bcmtm_drv_info_get(unit, &drv_info);
    td4_mmu_init_dev_config(unit, &devcfg, drv_info->lossless);
    rv = bcmlrd_map_get(unit, TM_PORT_MC_Q_TO_SERVICE_POOLt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    for (lport = 0; lport < SOC_MMU_CFG_PORT_MAX; lport++) {
        td4_mmu_config_thdo_set(unit, lport, buf, &devcfg, drv_info->lossless);
        if (bcmtm_lport_is_cpu(unit, lport)) {
            max_num_mcq = SOC_TD4_NUM_CPU_QUEUES;
        } else {
            max_num_mcq = TD4_MAX_NUM_MC_Q;
        }
        for (queue = 0; queue < max_num_mcq; queue++) {
            SHR_IF_ERR_EXIT
                (tm_port_mc_q_to_service_pool_populate_entry(unit, lport, queue,
                                                             1, buf));
        }
    }

exit:
    sal_free(buf);
    SHR_FUNC_EXIT();
}

int
bcm56880_a0_tm_port_mc_q_to_service_pool_update(int unit, int pport, int up)
{
    int lport, queue, max_num_mcq, rv;
    const bcmlrd_map_t *map = NULL;
    bcmltd_fields_t lkup_fields, keys, out_fields;
    size_t num_fid;
    bcmltd_sid_t sid = TM_PORT_MC_Q_TO_SERVICE_POOLt;
    bcmltd_field_t *data_fields = NULL;
    int j;

    SHR_FUNC_ENTER(unit);

    sal_memset(&lkup_fields, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out_fields, 0, sizeof(bcmltd_fields_t));
    sal_memset(&keys, 0, sizeof(bcmltd_fields_t));
    rv = bcmlrd_map_get(unit, TM_PORT_MC_Q_TO_SERVICE_POOLt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, NUM_KEY_FIELDS, &keys));

    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, sid, &num_fid));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &lkup_fields));
    /* Create data_fields with default field values. */
    SHR_IF_ERR_EXIT
        (bcmtm_default_entry_ll_construct(unit, sid, &data_fields));

    SHR_IF_ERR_EXIT
        (bcmtm_pport_lport_get(unit, pport, &lport));
    if (bcmtm_lport_is_cpu(unit, lport)) {
        max_num_mcq = SOC_TD4_NUM_CPU_QUEUES;
    } else {
        max_num_mcq = TD4_MAX_NUM_MC_Q;
    }
    for (queue = 0; queue < max_num_mcq; queue++) {
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_alloc(unit, num_fid, &out_fields));
        keys.field[0]->id   = TM_PORT_MC_Q_TO_SERVICE_POOLt_PORT_IDf;
        keys.field[0]->data = lport;
        keys.field[1]->id   = TM_PORT_MC_Q_TO_SERVICE_POOLt_TM_MC_Q_IDf;
        keys.field[1]->data = queue;
        if (SHR_SUCCESS(bcmimm_entry_lookup(unit, sid, &keys, &lkup_fields))) {
            for (j = 0; j < (int)lkup_fields.count; j++) {
               data_fields[j] = *(lkup_fields).field[j];
            }

            SHR_IF_ERR_EXIT
                (tm_port_mc_q_to_service_pool_update(unit, sid, lport, queue,
                                               data_fields, &out_fields));
            if (up == 0) {
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, &out_fields,
                                     TM_PORT_MC_Q_TO_SERVICE_POOLt_OPERATIONAL_STATEf,
                                     0, PORT_INFO_UNAVAILABLE));
            }
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, &out_fields,
                                 TM_PORT_MC_Q_TO_SERVICE_POOLt_PORT_IDf, 0,
                                 lport));
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, &out_fields,
                                 TM_PORT_MC_Q_TO_SERVICE_POOLt_TM_MC_Q_IDf, 0,
                                 queue));
            SHR_IF_ERR_EXIT
                (bcmimm_entry_update(unit, FALSE, sid, &out_fields));
        }
        bcmtm_field_list_free(&out_fields);
    }

exit:
    SHR_FREE(data_fields);
    bcmtm_field_list_free(&lkup_fields);
    bcmtm_field_list_free(&keys);
    SHR_FUNC_EXIT();
}
