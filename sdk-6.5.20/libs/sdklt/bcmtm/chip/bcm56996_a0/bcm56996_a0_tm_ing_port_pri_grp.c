/*! \file bcm56996_a0_tm_ing_port_pri_grp.c
 *
 * IMM handlers for threshold LTs.
 *
 * This file contains callbcak handlers for LT TM_ING_PORT_PRI_GRP.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/chip/bcm56996_a0_enum.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmtm/chip/bcm56996_a0_tm_thd_internal.h>
#include <bcmcth/bcmcth_imm_util.h>

#include "bcm56996_a0_tm_imm_internal.h"

/* TM_PORT_ENTRY_STATE_T */
#define VALID                   0
#define PORT_INFO_UNAVAILABLE   1

#define NUM_KEY_FIELDS          2
#define NUM_OPER_FIELDS         1
#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM_CHIP

static int
tm_ing_port_pri_grp_populate_entry(int unit, int lport, int pg, int up,
                                   soc_mmu_cfg_buf_t *buf)
{
    bcmltd_fields_t entry;
    bcmltd_sid_t sid = TM_ING_PORT_PRI_GRPt;
    size_t num_fid;
    soc_mmu_cfg_buf_prigroup_t *buf_prigroup;

    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(bcmltd_fields_t));
    buf_prigroup = &buf->ports[lport].prigroups[pg];

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
        (bcmtm_field_list_set(unit, &entry,
                              TM_ING_PORT_PRI_GRPt_PORT_IDf, 0, lport));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
                              TM_ING_PORT_PRI_GRPt_TM_PRI_GRP_IDf, 0, pg));

    /* Write to field list. */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_ING_PORT_PRI_GRPt_ING_MIN_MODEf, 0,
            !buf_prigroup->port_guarantee_enable));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_ING_PORT_PRI_GRPt_LOSSLESSf, 0, buf_prigroup->lossless));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_ING_PORT_PRI_GRPt_OPERATIONAL_STATEf, 0, PORT_INFO_UNAVAILABLE));

    /* Insert/update the LT. (Update is needed during flexport operation.) */
    SHR_IF_ERR_EXIT
        (bcmimm_entry_insert(unit, sid, &entry));

exit:
    bcmtm_field_list_free(&entry);
    SHR_FUNC_EXIT();
}

static int
tm_ing_port_pri_grp_update(int unit,
                           bcmltd_sid_t sid,
                           const bcmltd_op_arg_t *op_arg,
                           int lport,
                           int pg,
                           const bcmltd_field_t *data_fields,
                           bcmltd_fields_t *output_fields)
{
    const bcmltd_field_t *data_ptr;
    int pport, pipe, midx, mmu_port;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_sid_t ptid;
    uint32_t fval;
    uint32_t ltmbuf[BCMTM_MAX_THD_ENTRY_WSIZE] = {0};
    SHR_BITDCL lossless_bmp[1] = {0};
    uint64_t flags = 0;

    bcmdrd_fid_t field_use_portsp[SOC_MMU_CFG_PRI_GROUP_MAX] = {
            PG0_USE_PORTSP_MINf, PG1_USE_PORTSP_MINf, PG2_USE_PORTSP_MINf,
            PG3_USE_PORTSP_MINf, PG4_USE_PORTSP_MINf, PG5_USE_PORTSP_MINf,
            PG6_USE_PORTSP_MINf, PG7_USE_PORTSP_MINf};

    SHR_FUNC_ENTER(unit);

    if (op_arg) {
        flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    }

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_size_check(unit, output_fields, NUM_OPER_FIELDS));
    output_fields->count = 0;

    /* Check for invalid port (port is not added). */
    if (SHR_FAILURE(bcmtm_lport_pport_get(unit, lport, &pport))) {
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, output_fields,
                             TM_ING_PORT_PRI_GRPt_OPERATIONAL_STATEf,
                             0, PORT_INFO_UNAVAILABLE));
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT
        (bcmtm_lport_pipe_get(unit, lport, &pipe));

    data_ptr = data_fields;
    while (data_ptr) {
        fval = data_ptr->data;
        switch (data_ptr->id) {
            case TM_ING_PORT_PRI_GRPt_ING_MIN_MODEf:
                ptid = MMU_THDI_PORT_PG_MIN_CONFIGm;
                midx = th4g_piped_mem_index(unit, lport, ptid, 0);
                BCMTM_PT_DYN_INFO(pt_dyn_info, midx, pipe);
                pt_dyn_info.flags = flags;
                sal_memset(ltmbuf, 0, sizeof(ltmbuf));
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, ptid, field_use_portsp[pg], ltmbuf, &fval));
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_write(unit, ptid, sid, &pt_dyn_info, ltmbuf));
                break;
            case TM_ING_PORT_PRI_GRPt_LOSSLESSf:
                ptid = MMU_THDI_ING_PORT_CONFIGr;
                SHR_IF_ERR_EXIT
                    (bcmtm_lport_mport_get(unit, lport, &mmu_port));
                BCMTM_PT_DYN_INFO(pt_dyn_info, 0, mmu_port);
                pt_dyn_info.flags = flags;
                sal_memset(ltmbuf, 0, sizeof(ltmbuf));
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
                SHR_IF_ERR_EXIT
                    (bcmtm_field_get(unit, ptid, PG_IS_LOSSLESSf, ltmbuf, lossless_bmp));
                if (fval) {
                    SHR_BITSET(lossless_bmp, pg);
                } else {
                    SHR_BITCLR(lossless_bmp, pg);
                }
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, ptid, PG_IS_LOSSLESSf, ltmbuf, lossless_bmp));
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_write(unit, ptid, sid, &pt_dyn_info, ltmbuf));
                break;
            case TM_ING_PORT_PRI_GRPt_PFCf:
                ptid = MMU_THDI_ING_PORT_CONFIGr;
                SHR_IF_ERR_EXIT
                    (bcmtm_lport_mport_get(unit, lport, &mmu_port));
                BCMTM_PT_DYN_INFO(pt_dyn_info, 0, mmu_port);
                sal_memset(ltmbuf, 0, sizeof(ltmbuf));
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
                SHR_IF_ERR_EXIT
                    (bcmtm_field_get(unit, ptid, PFC_PG_ENABLEf, ltmbuf, lossless_bmp));
                if (fval) {
                    SHR_BITSET(lossless_bmp, pg);
                } else {
                    SHR_BITCLR(lossless_bmp, pg);
                }
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, ptid, PFC_PG_ENABLEf, ltmbuf, lossless_bmp));
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_write(unit, ptid, sid, &pt_dyn_info, ltmbuf));
            default:
                break;
        }
        data_ptr = data_ptr->next;
    }
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, output_fields,
                              TM_ING_PORT_PRI_GRPt_OPERATIONAL_STATEf,
                              0, VALID));

exit:
    SHR_FUNC_EXIT();
}

static int
tm_ing_port_pri_grp_delete(int unit, bcmltd_sid_t sid,
                           const bcmltd_op_arg_t *op_arg, int lport, int pg)
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
        (tm_ing_port_pri_grp_update(unit, sid, op_arg, lport, pg, data_fields,
                                    &dummy_fields));

exit:
    SHR_FREE(data_fields);
    bcmtm_field_list_free(&dummy_fields);
    SHR_FUNC_EXIT();
}

static int
tm_ing_port_pri_grp_stage(int unit,
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
    int pg = -1;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);

    key_ptr = key_fields;
    while (key_ptr) {
        switch (key_ptr->id) {
            case TM_ING_PORT_PRI_GRPt_PORT_IDf:
                lport = key_ptr->data;
                break;
            case TM_ING_PORT_PRI_GRPt_TM_PRI_GRP_IDf:
                pg = key_ptr->data;
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        key_ptr = key_ptr->next;
    }

    if (lport < 0 || pg < 0) {
        return SHR_E_PARAM;
    }

    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
            /* Insert shares the same callback with update. */
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_EXIT
                (tm_ing_port_pri_grp_update(unit, sid, op_arg, lport, pg,
                                            data_fields, output_fields));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT
                (tm_ing_port_pri_grp_delete(unit, sid, op_arg, lport, pg));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
tm_ing_port_pri_grp_lookup(int unit,
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
    int lport, pport, pg, pipe, midx, mmu_port;
    SHR_BITDCL lossless_bmp[1] = {0};
    uint32_t ltmbuf[BCMTM_MAX_THD_ENTRY_WSIZE] = {0};
    bcmdrd_fid_t field_use_portsp[SOC_MMU_CFG_PRI_GROUP_MAX] = {
            PG0_USE_PORTSP_MINf, PG1_USE_PORTSP_MINf, PG2_USE_PORTSP_MINf,
            PG3_USE_PORTSP_MINf, PG4_USE_PORTSP_MINf, PG5_USE_PORTSP_MINf,
            PG6_USE_PORTSP_MINf, PG7_USE_PORTSP_MINf};

    SHR_FUNC_ENTER(unit);

    if (!(op_arg->attrib & BCMLT_ENT_ATTR_GET_FROM_HW)) {
        SHR_EXIT();
    }
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    key = (lkup_type == BCMIMM_TRAVERSE) ? out : in;
    fid = TM_ING_PORT_PRI_GRPt_PORT_IDf;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_imm_fields_get(unit, key, fid, &idx));
    lport = idx;
    fid = TM_ING_PORT_PRI_GRPt_TM_PRI_GRP_IDf;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_imm_fields_get(unit, key, fid, &idx));
    pg = idx;

    /* Check for invalid port (port is not added). */
    if (SHR_FAILURE(bcmtm_lport_pport_get(unit, lport, &pport))) {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

    SHR_IF_ERR_EXIT
        (bcmtm_lport_pipe_get(unit, lport, &pipe));

    ptid = MMU_THDI_PORT_PG_MIN_CONFIGm;
    midx = th4g_piped_mem_index(unit, lport, ptid, 0);
    BCMTM_PT_DYN_INFO(pt_dyn_info, midx, 0);
    pt_dyn_info.flags = req_flags;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));

    SHR_IF_ERR_EXIT(
        bcmtm_field_get(unit, ptid, field_use_portsp[pg], ltmbuf, &fval));
    SHR_IF_ERR_EXIT(
        bcmcth_imm_field_update(unit, out,
                                TM_ING_PORT_PRI_GRPt_ING_MIN_MODEf,
                                fval));

    ptid = MMU_THDI_ING_PORT_CONFIGr;
    SHR_IF_ERR_EXIT
        (bcmtm_lport_mport_get(unit, lport, &mmu_port));
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, mmu_port);
    pt_dyn_info.flags = req_flags;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));

    SHR_IF_ERR_EXIT(
        bcmtm_field_get(unit, ptid, PG_IS_LOSSLESSf, ltmbuf, lossless_bmp));
    SHR_IF_ERR_EXIT(
        bcmcth_imm_field_update(unit, out,
                                TM_ING_PORT_PRI_GRPt_LOSSLESSf,
                                SHR_BITGET(lossless_bmp, pg) ? 1 : 0));

    SHR_IF_ERR_EXIT(
        bcmtm_field_get(unit, ptid, PFC_PG_ENABLEf, ltmbuf, lossless_bmp));
    SHR_IF_ERR_EXIT(
        bcmcth_imm_field_update(unit, out,
                                TM_ING_PORT_PRI_GRPt_PFCf,
                                SHR_BITGET(lossless_bmp, pg) ? 1 : 0));
exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */

int
bcm56996_a0_tm_ing_port_pri_grp_register(int unit)
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
    tm_thd_cb_handler.op_stage = tm_ing_port_pri_grp_stage;
    tm_thd_cb_handler.op_lookup = tm_ing_port_pri_grp_lookup;

    rv = bcmlrd_map_get(unit, TM_ING_PORT_PRI_GRPt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    /* Register callback handler for the LT. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_ING_PORT_PRI_GRPt, &tm_thd_cb_handler, NULL));

exit:
    SHR_FUNC_EXIT();
}

int
bcm56996_a0_tm_ing_port_pri_grp_populate(int unit)
{
    int lport, rv;
    int pg;
    const bcmlrd_map_t *map = NULL;
    bcmtm_drv_info_t *drv_info;
    soc_mmu_cfg_buf_t *buf;
    soc_mmu_device_info_t devcfg;

    SHR_FUNC_ENTER(unit);

    bcmtm_drv_info_get(unit, &drv_info);
    buf = th4g_soc_mmu_buf_cfg_alloc(unit);
    if (!buf) {
        return SHR_E_MEMORY;
    }
    bcmtm_drv_info_get(unit, &drv_info);
    th4g_mmu_init_dev_config(unit, &devcfg, drv_info->lossless);
    rv = bcmlrd_map_get(unit, TM_ING_PORT_PRI_GRPt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    for (lport = 0; lport < SOC_MMU_CFG_PORT_MAX; lport++) {
        th4g_mmu_config_thdi_set(unit, lport, buf, &devcfg, drv_info->lossless);
        for (pg = 0; pg < SOC_MMU_CFG_PRI_GROUP_MAX; pg++) {
            SHR_IF_ERR_EXIT
                (tm_ing_port_pri_grp_populate_entry(unit, lport, pg, 1, buf));
        }
    }

exit:
    sal_free(buf);
    SHR_FUNC_EXIT();
}

int
bcm56996_a0_tm_ing_port_pri_grp_update(int unit, int pport, int up)
{
    int lport, pg, rv;
    const bcmlrd_map_t *map = NULL;
    bcmltd_fields_t lkup_fields, keys, out_fields;
    size_t num_fid;
    bcmltd_sid_t sid = TM_ING_PORT_PRI_GRPt;
    bcmltd_field_t *data_fields = NULL;
    int j;

    SHR_FUNC_ENTER(unit);

    sal_memset(&lkup_fields, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out_fields, 0, sizeof(bcmltd_fields_t));
    sal_memset(&keys, 0, sizeof(bcmltd_fields_t));
    rv = bcmlrd_map_get(unit, TM_ING_PORT_PRI_GRPt, &map);
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
    for (pg = 0; pg < SOC_MMU_CFG_PRI_GROUP_MAX; pg++) {
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_alloc(unit, num_fid, &out_fields));
        keys.field[0]->id   = TM_ING_PORT_PRI_GRPt_PORT_IDf;
        keys.field[0]->data = lport;
        keys.field[1]->id   = TM_ING_PORT_PRI_GRPt_TM_PRI_GRP_IDf;
        keys.field[1]->data = pg;

        if (SHR_SUCCESS(bcmimm_entry_lookup(unit, sid, &keys, &lkup_fields))) {
            for (j = 0; j < (int)lkup_fields.count; j++) {
               data_fields[j] = *(lkup_fields).field[j];
            }

            SHR_IF_ERR_EXIT
                (tm_ing_port_pri_grp_update(unit, sid, NULL, lport, pg,
                                               data_fields, &out_fields));
            if (up == 0) {
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, &out_fields,
                        TM_ING_PORT_PRI_GRPt_OPERATIONAL_STATEf, 0,
                        PORT_INFO_UNAVAILABLE));
            }
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, &out_fields,
                                      TM_ING_PORT_PRI_GRPt_PORT_IDf, 0, lport));
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, &out_fields,
                                      TM_ING_PORT_PRI_GRPt_TM_PRI_GRP_IDf, 0,
                                      pg));
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
