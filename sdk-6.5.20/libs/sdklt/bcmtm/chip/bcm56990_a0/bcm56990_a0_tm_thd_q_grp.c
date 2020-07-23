/*! \file bcm56990_a0_tm_thd_q_grp.c
 *
 * IMM handlers for threshold LTs.
 *
 * This file contains callbcak handlers for LT TM_THD_Q_GRP.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/chip/bcm56990_a0_enum.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmtm/chip/bcm56990_a0_tm_thd_internal.h>
#include <bcmcth/bcmcth_imm_util.h>

#include "bcm56990_a0_tm_imm_internal.h"

/* TM_THD_Q_GRP_ENTRY_STATE_T */
#define VALID                   0
#define PORT_INFO_UNAVAILABLE   1
#define INCORRECT_MIN_GUARANTEE 2

#define NUM_KEY_FIELDS          1
#define NUM_OPER_FIELDS         3

#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM_CHIP
static int
tm_thd_q_grp_populate_entry(int unit, int lport, int up,
                            soc_mmu_cfg_buf_qgroup_t *queue_grp)
{
    bcmltd_fields_t entry;
    bcmltd_sid_t sid = TM_THD_Q_GRPt;
    size_t num_fid;

    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(bcmltd_fields_t));

    /*
     * Populate default entry (value fields) with HW default values.
     * Key fields must be set.
     */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, sid, &num_fid));
    if (bcmtm_is_flexport(unit)) {
        /* Only update operational fields during flexport operation. */
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_alloc(unit, NUM_KEY_FIELDS + NUM_OPER_FIELDS, &entry));
        entry.count = 0;
    } else {
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_alloc(unit, num_fid, &entry));
        SHR_IF_ERR_EXIT
            (bcmtm_default_entry_array_construct(unit, sid, &entry));
    }
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry, TM_THD_Q_GRPt_PORT_IDf, 0, lport));


    /* Write to field list. */
    /* Populate user-configured fields during init. */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_THD_Q_GRPt_UC_Q_GRP_MIN_GUARANTEE_CELLSf, 0, queue_grp->guarantee));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_THD_Q_GRPt_MC_Q_GRP_MIN_GUARANTEE_CELLSf, 0, queue_grp->guarantee_mc));
    /* Operational fields. */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_THD_Q_GRPt_OPERATIONAL_STATEf, 0, PORT_INFO_UNAVAILABLE));

    SHR_IF_ERR_EXIT
        (bcmimm_entry_insert(unit, sid, &entry));

exit:
    bcmtm_field_list_free(&entry);
    SHR_FUNC_EXIT();
}

static int
tm_thd_q_grp_update(int unit,
                    bcmltd_sid_t sid,
                    const bcmltd_op_arg_t *op_arg,
                    int lport,
                    const bcmltd_field_t *data_fields,
                    bcmltd_fields_t *output_fields, uint8_t action)
{
    const bcmltd_field_t *data_ptr;
    int pport, mmu_chip_port;
    long int delta;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_sid_t ptid;
    uint32_t uc_q_grp_min, mc_q_grp_min, fval;
    uint32_t ltmbuf[BCMTM_MAX_THD_ENTRY_WSIZE] = {0};
    bool write_flag;
    soc_mmu_cfg_buf_qgroup_t queue_grp;
    uint64_t flags = 0;

    SHR_FUNC_ENTER(unit);
    if (op_arg) {
        flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    }

    th4_mmu_config_buf_qgroup_get(unit, lport, &queue_grp);

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_size_check(unit, output_fields, NUM_OPER_FIELDS));
    output_fields->count = 0;

    /* Check for invalid port (port is not added). */
    if (SHR_FAILURE(bcmtm_lport_pport_get(unit, lport, &pport))) {
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, output_fields,
                             TM_THD_Q_GRPt_OPERATIONAL_STATEf,
                             0, PORT_INFO_UNAVAILABLE));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmtm_lport_mchip_port_get(unit, lport, &mmu_chip_port));
    BCMTM_PT_DYN_INFO(pt_dyn_info, mmu_chip_port, 0);
    pt_dyn_info.flags = flags;

    data_ptr = data_fields;
    while (data_ptr) {
        write_flag = TRUE;
        fval = data_ptr->data;
        switch (data_ptr->id) {
            case TM_THD_Q_GRPt_UC_Q_GRP_MIN_GUARANTEE_CELLSf:
                ptid = MMU_THDO_CONFIG_UC_QGROUP0m;
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info,
                                           ltmbuf));
                SHR_IF_ERR_EXIT
                    (bcmtm_field_get(unit, ptid, MIN_LIMITf, ltmbuf,
                                     &uc_q_grp_min));
                if (action == PORT_ADD) {
                    uc_q_grp_min = queue_grp.guarantee;
                } else if (action == PORT_DELETE) {
                    fval = queue_grp.guarantee;
                }

                /*
                 * Calculate delta of available shared size.
                 * delta of shared_size always equals to -(delta of reserved_size).
                 */
                delta = - ((long int) fval - uc_q_grp_min);
                /* Check if both ITM have enough buffer size to allocate the delta. */
                if (SHR_SUCCESS
                        (bcm56990_a0_tm_shared_buffer_update(unit, 0, -1, delta,
                                                             RESERVED_BUFFER_UPDATE,
                                                             TRUE)) &&
                    SHR_SUCCESS
                        (bcm56990_a0_tm_shared_buffer_update(unit, 1, -1, delta,
                                                             RESERVED_BUFFER_UPDATE,
                                                             TRUE))) {
                    /* Update the service pool size in both ITM. */
                    SHR_IF_ERR_EXIT
                        (bcm56990_a0_tm_shared_buffer_update(unit, 0, -1, delta,
                                                             RESERVED_BUFFER_UPDATE,
                                                             FALSE));
                    SHR_IF_ERR_EXIT
                        (bcm56990_a0_tm_shared_buffer_update(unit, 1, -1, delta,
                                                             RESERVED_BUFFER_UPDATE,
                                                             FALSE));
                    /* Update operational fields to the new limit. */
                    SHR_IF_ERR_EXIT
                        (bcmtm_field_list_set(unit, output_fields,
                                         TM_THD_Q_GRPt_UC_Q_GRP_MIN_GUARANTEE_CELLS_OPERf,
                                         0, fval));
                    SHR_IF_ERR_EXIT
                        (bcmtm_field_list_set(unit, output_fields,
                                         TM_THD_Q_GRPt_OPERATIONAL_STATEf,
                                         0, VALID));
                } else {
                    write_flag = FALSE;
                    /* Update operational fields to reflect the error. */
                    SHR_IF_ERR_EXIT
                        (bcmtm_field_list_set(unit, output_fields, TM_THD_Q_GRPt_OPERATIONAL_STATEf,
                                         0, INCORRECT_MIN_GUARANTEE));
                }
                break;
            case TM_THD_Q_GRPt_MC_Q_GRP_MIN_GUARANTEE_CELLSf:
                ptid = MMU_THDO_CONFIG_MC_QGROUPm;
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
                SHR_IF_ERR_EXIT
                    (bcmtm_field_get(unit, ptid, MIN_LIMITf, ltmbuf, &mc_q_grp_min));
                if (action == PORT_ADD) {
                    mc_q_grp_min = queue_grp.guarantee_mc;
                } else if (action == PORT_DELETE) {
                    fval = queue_grp.guarantee_mc;
                }
                /*
                 * Calculate delta of available shared size.
                 * delta of shared_size always equals to -(delta of reserved_size).
                 */
                delta = - ((long int) fval - mc_q_grp_min);
                /* Check if both ITM have enough buffer size to allocate the delta. */
                if (SHR_SUCCESS
                        (bcm56990_a0_tm_shared_buffer_update(unit, 0, -1, delta,
                                                             RESERVED_BUFFER_UPDATE,
                                                             TRUE)) &&
                    SHR_SUCCESS
                        (bcm56990_a0_tm_shared_buffer_update(unit, 1, -1, delta,
                                                             RESERVED_BUFFER_UPDATE,
                                                             TRUE))) {
                    /* Update the service pool size in both ITM. */
                    SHR_IF_ERR_EXIT
                        (bcm56990_a0_tm_shared_buffer_update(unit, 0, -1, delta,
                                                             RESERVED_BUFFER_UPDATE,
                                                             FALSE));
                    SHR_IF_ERR_EXIT
                        (bcm56990_a0_tm_shared_buffer_update(unit, 1, -1, delta,
                                                             RESERVED_BUFFER_UPDATE,
                                                             FALSE));
                    /* Update operational fields to the new limit. */
                    SHR_IF_ERR_EXIT
                        (bcmtm_field_list_set(unit, output_fields,
                                         TM_THD_Q_GRPt_MC_Q_GRP_MIN_GUARANTEE_CELLS_OPERf,
                                         0, fval));
                    SHR_IF_ERR_EXIT
                        (bcmtm_field_list_set(unit, output_fields,
                                         TM_THD_Q_GRPt_OPERATIONAL_STATEf,
                                         0, VALID));
                } else {
                    write_flag = FALSE;
                    /* Update operational fields to reflect the error. */
                    SHR_IF_ERR_EXIT
                        (bcmtm_field_list_set(unit, output_fields,
                                            TM_THD_Q_GRPt_OPERATIONAL_STATEf,
                                            0, INCORRECT_MIN_GUARANTEE));
                }
                break;
            default:
                write_flag = FALSE;
                break;
        }
        if (write_flag) {
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, ptid, MIN_LIMITf, ltmbuf, &fval));
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, ptid, sid, &pt_dyn_info, ltmbuf));

        }
        data_ptr = data_ptr->next;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
tm_thd_q_grp_delete(int unit, bcmltd_sid_t sid,
                    const bcmltd_op_arg_t *op_arg, int lport)
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
        (tm_thd_q_grp_update(unit, sid, op_arg, lport, data_fields,
                             &dummy_fields, FALSE));

exit:
    SHR_FREE(data_fields);
    bcmtm_field_list_free(&dummy_fields);
    SHR_FUNC_EXIT();
}

static int
tm_thd_q_grp_stage(int unit,
                   bcmltd_sid_t sid,
                   const bcmltd_op_arg_t *op_arg,
                   bcmimm_entry_event_t event_reason,
                   const bcmltd_field_t *key_fields,
                   const bcmltd_field_t *data_fields,
                   void *context,
                   bcmltd_fields_t *output_fields)
{
    const bcmltd_field_t *key_ptr;
    int lport = -1;

    SHR_FUNC_ENTER(unit);

    key_ptr = key_fields;
    while (key_ptr) {
        switch (key_ptr->id) {
            case TM_THD_Q_GRPt_PORT_IDf:
                lport = key_ptr->data;
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        key_ptr = key_ptr->next;
    }

    if (lport < 0) {
        return SHR_E_PARAM;
    }

    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
            /* Insert shares the same callback with update. */
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_EXIT
                (tm_thd_q_grp_update(unit, sid, op_arg, lport, data_fields,
                                     output_fields, UPDATE_ONLY));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT
                (tm_thd_q_grp_delete(unit, sid, op_arg, lport));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
tm_thd_q_grp_lookup(int unit,
                    bcmltd_sid_t sid,
                    const bcmltd_op_arg_t *op_arg,
                    void *context,
                    bcmimm_lookup_type_t lkup_type,
                    const bcmltd_fields_t *in,
                    bcmltd_fields_t *out)
{
    const bcmltd_fields_t *key;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_sid_t ptid;
    uint32_t fid = 0, fval = 0;
    uint64_t idx = 0, req_flags;
    int lport, pport, mmu_chip_port;
    uint32_t ltmbuf[BCMTM_MAX_THD_ENTRY_WSIZE] = {0};

    SHR_FUNC_ENTER(unit);

    if (!(op_arg->attrib & BCMLT_ENT_ATTR_GET_FROM_HW)) {
        SHR_EXIT();
    }
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    key = (lkup_type == BCMIMM_TRAVERSE) ? out : in;
    fid = TM_THD_Q_GRPt_PORT_IDf;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_imm_fields_get(unit, key, fid, &idx));
    lport = idx;
    /* Check for invalid port (port is not added). */
    if (SHR_FAILURE(bcmtm_lport_pport_get(unit, lport, &pport))) {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

    SHR_IF_ERR_EXIT
        (bcmtm_lport_mchip_port_get(unit, lport, &mmu_chip_port));

    ptid = MMU_THDO_CONFIG_UC_QGROUP0m;
    BCMTM_PT_DYN_INFO(pt_dyn_info, mmu_chip_port, 0);
    pt_dyn_info.flags = req_flags;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT(
        bcmtm_field_get(unit, ptid, MIN_LIMITf, ltmbuf, &fval));
    SHR_IF_ERR_EXIT(
        bcmcth_imm_field_update(unit, out,
                                TM_THD_Q_GRPt_UC_Q_GRP_MIN_GUARANTEE_CELLSf,
                                fval));

    ptid = MMU_THDO_CONFIG_MC_QGROUPm;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT(
        bcmtm_field_get(unit, ptid, MIN_LIMITf, ltmbuf, &fval));
    SHR_IF_ERR_EXIT(
        bcmcth_imm_field_update(unit, out,
                                TM_THD_Q_GRPt_MC_Q_GRP_MIN_GUARANTEE_CELLSf,
                                fval));

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */

int
bcm56990_a0_tm_thd_q_grp_register(int unit)
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
    tm_thd_cb_handler.op_stage = tm_thd_q_grp_stage;
    tm_thd_cb_handler.op_lookup = tm_thd_q_grp_lookup;

    rv = bcmlrd_map_get(unit, TM_THD_Q_GRPt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }
    /* Register callback handler for the LT. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_THD_Q_GRPt, &tm_thd_cb_handler, NULL));

exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_a0_tm_thd_q_grp_populate(int unit)
{
    int lport, rv;
    const bcmlrd_map_t *map = NULL;
    soc_mmu_cfg_buf_qgroup_t queue_grp;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, TM_THD_Q_GRPt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    for (lport = 0; lport < SOC_MMU_CFG_PORT_MAX; lport++) {
        th4_mmu_config_buf_qgroup_get(unit, lport, &queue_grp);
        SHR_IF_ERR_EXIT
            (tm_thd_q_grp_populate_entry(unit, lport, 1, &queue_grp));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_a0_tm_thd_q_grp_update(int unit, int pport, int up)
{
    int lport, rv;
    const bcmlrd_map_t *map = NULL;
    bcmltd_fields_t lkup_fields, keys, out_fields;
    size_t num_fid;
    bcmltd_sid_t sid = TM_THD_Q_GRPt;
    bcmltd_field_t *data_fields = NULL;
    int j;

    SHR_FUNC_ENTER(unit);
    sal_memset(&lkup_fields, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out_fields, 0, sizeof(bcmltd_fields_t));
    sal_memset(&keys, 0, sizeof(bcmltd_fields_t));

    rv = bcmlrd_map_get(unit, TM_THD_Q_GRPt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL)|| (!map)) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, NUM_KEY_FIELDS, &keys));

    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, sid, &num_fid));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &out_fields));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &lkup_fields));
    /* Create data_fields with default field values. */
    SHR_IF_ERR_EXIT
        (bcmtm_default_entry_ll_construct(unit, sid, &data_fields));

    SHR_IF_ERR_EXIT
        (bcmtm_pport_lport_get(unit, pport, &lport));
    keys.field[0]->id   = TM_THD_Q_GRPt_PORT_IDf;
    keys.field[0]->data = lport;
    if (SHR_SUCCESS(bcmimm_entry_lookup(unit, sid, &keys, &lkup_fields))) {
        for (j = 0; j < (int)lkup_fields.count; j++) {
           data_fields[j] = *(lkup_fields).field[j];
        }

        SHR_IF_ERR_EXIT
            (tm_thd_q_grp_update(unit, sid, NULL, lport, data_fields, &out_fields,
                                 up));
        if (up == 0) {
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, &out_fields,
                                 TM_THD_Q_GRPt_OPERATIONAL_STATEf,
                                 0, PORT_INFO_UNAVAILABLE));
        }
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, &out_fields,
                              TM_THD_Q_GRPt_PORT_IDf,
                              0, lport));
        SHR_IF_ERR_EXIT
            (bcmimm_entry_update(unit, FALSE, sid, &out_fields));
    }

exit:
    bcmtm_field_list_free(&out_fields);
    SHR_FREE(data_fields);
    bcmtm_field_list_free(&lkup_fields);
    bcmtm_field_list_free(&keys);
    SHR_FUNC_EXIT();
}
