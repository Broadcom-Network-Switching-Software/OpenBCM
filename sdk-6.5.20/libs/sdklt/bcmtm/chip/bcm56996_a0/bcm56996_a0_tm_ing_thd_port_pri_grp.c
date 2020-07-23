/*! \file bcm56996_a0_tm_ing_thd_port_pri_grp.c
 *
 * IMM handlers for threshold LTs.
 *
 * This file contains callbcak handlers for LT TM_ING_THD_PORT_PRI_GRP.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/chip/bcm56996_a0_enum.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmtm/chip/bcm56996_a0_tm_thd_internal.h>
#include <bcmcth/bcmcth_imm_util.h>

#include "bcm56996_a0_tm_imm_internal.h"

/* TM_ING_THD_PORT_PRI_GRP_ENTRY_STATE_T */
#define VALID                   0
#define PORT_INFO_UNAVAILABLE   1
#define INCORRECT_MIN_GUARANTEE 2

#define NUM_KEY_FIELDS          2
#define NUM_OPER_FIELDS         3

#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM_CHIP

static int
tm_thd_ing_port_pri_grp_populate_entry(int unit, int lport, int pg, int up,
                                       soc_mmu_cfg_buf_prigroup_t *buf_prigroup)
{
    bcmltd_fields_t entry;
    bcmltd_sid_t sid = TM_ING_THD_PORT_PRI_GRPt;
    size_t num_fid;
    bcmtm_drv_info_t *drv_info;

    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(bcmltd_fields_t));
    bcmtm_drv_info_get(unit, &drv_info);

    /*
     * Populate default entry (value fields) with HW default values.
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
                         TM_ING_THD_PORT_PRI_GRPt_PORT_IDf, 0, lport));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
                         TM_ING_THD_PORT_PRI_GRPt_TM_PRI_GRP_IDf, 0, pg));

    /* Write to field list. */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_ING_THD_PORT_PRI_GRPt_MIN_GUARANTEE_CELLSf, 0,
            buf_prigroup->guarantee));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_ING_THD_PORT_PRI_GRPt_HEADROOM_LIMIT_CELLSf, 0,
            buf_prigroup->headroom));
    if (!drv_info->lossless) {
        /* Lossy mode */
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, &entry,
                TM_ING_THD_PORT_PRI_GRPt_DYNAMIC_SHARED_LIMITSf, 0, 0));
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, &entry,
                TM_ING_THD_PORT_PRI_GRPt_SHARED_LIMIT_CELLS_STATICf, 0,
                TH4G_MMU_TOTAL_CELLS_PER_ITM));
    } else {
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, &entry,
                TM_ING_THD_PORT_PRI_GRPt_DYNAMIC_SHARED_LIMITSf, 0, 1));
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, &entry,
                TM_ING_THD_PORT_PRI_GRPt_SHARED_LIMIT_DYNAMICf, 0,
                buf_prigroup->pool_scale));
    }
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_ING_THD_PORT_PRI_GRPt_RESUME_FLOOR_CELLSf, 0,
            buf_prigroup->pool_floor));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_ING_THD_PORT_PRI_GRPt_RESUME_OFFSET_CELLSf, 0,
            buf_prigroup->pool_resume));
    /* set Operational fields to 0 as hardware is not written */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_ING_THD_PORT_PRI_GRPt_MIN_GUARANTEE_CELLS_OPERf, 0, 0));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_ING_THD_PORT_PRI_GRPt_HEADROOM_LIMIT_CELLS_OPERf, 0, 0));
    /* Pre-populate called during TM chip init, before port is added */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_ING_THD_PORT_PRI_GRPt_OPERATIONAL_STATEf, 0, PORT_INFO_UNAVAILABLE));

    /* Insert the LT */
    SHR_IF_ERR_EXIT
        (bcmimm_entry_insert(unit, sid, &entry));

exit:
    bcmtm_field_list_free(&entry);
    SHR_FUNC_EXIT();
}

static int
tm_thd_ing_port_pri_grp_update(int unit,
                               bcmltd_sid_t sid,
                               const bcmltd_op_arg_t *op_arg,
                               int lport,
                               int pg,
                               const bcmltd_field_t *data_fields,
                               bcmltd_fields_t *output_fields,
                               uint8_t action)
{
    const bcmltd_field_t *data_ptr;
    uint64_t field_data;
    bcmltd_fields_t entry;
    int pport, pipe, midx;
    long int delta;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_sid_t ptid;
    bcmdrd_fid_t fid;
    uint32_t pg_min = 0, use_dynamic = 0, fval, pg_hdrm = 0;
    uint32_t ltmbuf[BCMTM_MAX_THD_ENTRY_WSIZE] = {0};
    bool write_flag;
    bcmltd_fields_t keys;
    size_t num_fid;
    bool hdrm_auto = TRUE;
    uint64_t flags = 0;

    /* Per-port-per-pg fields. */
    bcmdrd_fid_t field_pgmin_limit[SOC_MMU_CFG_PRI_GROUP_MAX] = {
            PG0_MIN_LIMITf, PG1_MIN_LIMITf, PG2_MIN_LIMITf,
            PG3_MIN_LIMITf, PG4_MIN_LIMITf, PG5_MIN_LIMITf,
            PG6_MIN_LIMITf, PG7_MIN_LIMITf};
    bcmdrd_fid_t field_pghdrm_limit[SOC_MMU_CFG_PRI_GROUP_MAX] = {
            PG0_HDRM_LIMITf, PG1_HDRM_LIMITf, PG2_HDRM_LIMITf,
            PG3_HDRM_LIMITf, PG4_HDRM_LIMITf, PG5_HDRM_LIMITf,
            PG6_HDRM_LIMITf, PG7_HDRM_LIMITf};
    bcmdrd_fid_t field_pgshared_limit[SOC_MMU_CFG_PRI_GROUP_MAX] = {
            PG0_SHARED_LIMITf, PG1_SHARED_LIMITf, PG2_SHARED_LIMITf,
            PG3_SHARED_LIMITf, PG4_SHARED_LIMITf, PG5_SHARED_LIMITf,
            PG6_SHARED_LIMITf, PG7_SHARED_LIMITf};
    bcmdrd_fid_t field_pgshared_dynamic[SOC_MMU_CFG_PRI_GROUP_MAX] = {
            PG0_SHARED_DYNAMICf, PG1_SHARED_DYNAMICf, PG2_SHARED_DYNAMICf,
            PG3_SHARED_DYNAMICf, PG4_SHARED_DYNAMICf, PG5_SHARED_DYNAMICf,
            PG6_SHARED_DYNAMICf, PG7_SHARED_DYNAMICf};
    bcmdrd_fid_t field_pgreset_floor[SOC_MMU_CFG_PRI_GROUP_MAX] = {
            PG0_RESET_FLOORf, PG1_RESET_FLOORf, PG2_RESET_FLOORf,
            PG3_RESET_FLOORf, PG4_RESET_FLOORf, PG5_RESET_FLOORf,
            PG6_RESET_FLOORf, PG7_RESET_FLOORf};
    bcmdrd_fid_t field_pgreset_offset[SOC_MMU_CFG_PRI_GROUP_MAX] = {
            PG0_RESET_OFFSETf, PG1_RESET_OFFSETf, PG2_RESET_OFFSETf,
            PG3_RESET_OFFSETf, PG4_RESET_OFFSETf, PG5_RESET_OFFSETf,
            PG6_RESET_OFFSETf, PG7_RESET_OFFSETf};
    bcmdrd_fid_t field_bst_hdrm_profile[SOC_MMU_CFG_PRI_GROUP_MAX] = {
            PG0_BST_HDRM_PROFILE_SELf, PG1_BST_HDRM_PROFILE_SELf,
            PG2_BST_HDRM_PROFILE_SELf, PG3_BST_HDRM_PROFILE_SELf,
            PG4_BST_HDRM_PROFILE_SELf, PG5_BST_HDRM_PROFILE_SELf,
            PG6_BST_HDRM_PROFILE_SELf, PG7_BST_HDRM_PROFILE_SELf};
    bcmdrd_fid_t field_bst_shared_profile[SOC_MMU_CFG_PRI_GROUP_MAX] = {
            PG0_BST_SHARED_PROFILE_SELf, PG1_BST_SHARED_PROFILE_SELf,
            PG2_BST_SHARED_PROFILE_SELf, PG3_BST_SHARED_PROFILE_SELf,
            PG4_BST_SHARED_PROFILE_SELf, PG5_BST_SHARED_PROFILE_SELf,
            PG6_BST_SHARED_PROFILE_SELf, PG7_BST_SHARED_PROFILE_SELf};
    soc_mmu_cfg_buf_prigroup_t buf_prigroup;
    bcmtm_drv_info_t *drv_info;

    SHR_FUNC_ENTER(unit);

    if (op_arg) {
        flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    }

    bcmtm_drv_info_get(unit, &drv_info);
    /* Try to retrieve operational fields from the LT entry. */
    sal_memset(&entry, 0, sizeof(bcmltd_fields_t));
    sal_memset(&keys, 0, sizeof(bcmltd_fields_t));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, NUM_KEY_FIELDS, &keys));
    keys.field[0]->id   = TM_ING_THD_PORT_PRI_GRPt_PORT_IDf;
    keys.field[0]->data = lport;
    keys.field[1]->id   = TM_ING_THD_PORT_PRI_GRPt_TM_PRI_GRP_IDf;
    keys.field[1]->data = pg;
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, sid, &num_fid));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &entry));

    /* Initialize output_ields. */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_size_check(unit, output_fields, NUM_OPER_FIELDS));
    output_fields->count = 0;

    /* Check for invalid port (port is not added). */
    if (SHR_FAILURE(bcmtm_lport_pport_get(unit, lport, &pport))) {
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, output_fields,
                TM_ING_THD_PORT_PRI_GRPt_OPERATIONAL_STATEf, 0, PORT_INFO_UNAVAILABLE));
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT
        (bcmtm_lport_pipe_get(unit, lport, &pipe));

    th4g_mmu_config_buf_prigroup_get(unit, lport, pg,
                                    &buf_prigroup, drv_info->lossless);
    /* Retrieve current configuration from LT. */
    SHR_IF_ERR_EXIT(bcmimm_entry_lookup(unit, sid, &keys, &entry));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_get(unit, &entry,
            TM_ING_THD_PORT_PRI_GRPt_MIN_GUARANTEE_CELLS_OPERf, 0, &field_data));
    pg_min = (uint32_t)field_data;

    /* Also get use_dynamic for future reference. */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_get(unit, &entry,
            TM_ING_THD_PORT_PRI_GRPt_DYNAMIC_SHARED_LIMITSf, 0, &field_data));
    use_dynamic = (uint32_t)field_data;
    if (action == PORT_ADD) {
        pg_min = buf_prigroup.guarantee;
    }

    /* Retrieve current values of headroom limit and auto for future reference */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_get(unit, &entry,
            TM_ING_THD_PORT_PRI_GRPt_HEADROOM_LIMIT_AUTOf, 0, &field_data));
    hdrm_auto = (uint32_t)field_data;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_get(unit, &entry,
            TM_ING_THD_PORT_PRI_GRPt_HEADROOM_LIMIT_CELLSf, 0, &field_data));
    pg_hdrm = (uint32_t)field_data;

    /*
     * First iteration:
     * use_dynamic takes precedence over all other fields.
     */
    data_ptr = data_fields;
    while (data_ptr) {
        if (data_ptr->id == TM_ING_THD_PORT_PRI_GRPt_DYNAMIC_SHARED_LIMITSf) {
            use_dynamic = data_ptr->data;
            ptid = MMU_THDI_PORT_PG_SHARED_CONFIGm;
            fid = field_pgshared_dynamic[pg];
            midx = th4g_piped_mem_index(unit, lport, ptid, 0);
            BCMTM_PT_DYN_INFO(pt_dyn_info, midx, pipe);
            pt_dyn_info.flags = flags;
            sal_memset(ltmbuf, 0, sizeof(ltmbuf));
            /* Read - modify - write. */
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, ptid, fid, ltmbuf, &use_dynamic));
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, ptid, sid, &pt_dyn_info, ltmbuf));
            break;
        } else if (data_ptr->id == TM_ING_THD_PORT_PRI_GRPt_HEADROOM_LIMIT_AUTOf) {
            /* HEADROOM_LIMI_AUTO takes precedence over HEADROOM_LIMIT_CELLS */
            /* Hardware and OPER fields for headroom_limit need to be updated
             * even when AUTO field chnages */
            hdrm_auto = data_ptr->data;
            if (hdrm_auto) {
                pg_hdrm = buf_prigroup.headroom;
            }
            ptid = MMU_THDI_PORT_PG_HDRM_CONFIGm;
            fid = field_pghdrm_limit[pg];
            /* Update operational field. */
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, output_fields,
                    TM_ING_THD_PORT_PRI_GRPt_HEADROOM_LIMIT_CELLS_OPERf, 0,
                    pg_hdrm));
            midx = th4g_piped_mem_index(unit, lport, ptid, 0);
            BCMTM_PT_DYN_INFO(pt_dyn_info, midx, pipe);
            pt_dyn_info.flags = flags;
            sal_memset(ltmbuf, 0, sizeof(ltmbuf));
            /* Read - modify - write. */
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, ptid, fid, ltmbuf, &pg_hdrm));
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, ptid, sid, &pt_dyn_info, ltmbuf));

            break;
        } else {
            data_ptr = data_ptr->next;
        }
    }

    /*
     * Second iteration:
     * pg_min takes precedence over the rest of fields.
     * Adjust corresponding pool shared limit based on pg_min.
     */
    data_ptr = data_fields;
    while (data_ptr) {
        if (data_ptr->id == TM_ING_THD_PORT_PRI_GRPt_MIN_GUARANTEE_CELLSf) {
            int itm, pool, rv;
            uint32_t new_pg_min;

            new_pg_min = data_ptr->data;
            if (action == PORT_DELETE) {
                new_pg_min = buf_prigroup.guarantee;
            }
            /*
             * Calculate delta of available shared size.
             * delta of shared_size always equals to -(delta of reserved_size).
             */
            delta = -((long int) new_pg_min - pg_min);
            SHR_IF_ERR_EXIT
                (bcm56996_a0_tm_thdi_pg_to_pool_mapping_get(unit, lport, pg,
                                                            &itm, &pool, FALSE));
            /* Check if there is enough buffer size to allocate the delta. */
            rv = bcm56996_a0_tm_shared_buffer_update(unit, itm, pool, delta,
                                                     RESERVED_BUFFER_UPDATE, FALSE);
            if (rv == SHR_E_NONE) {
                /*
                 * Request accepted. Other limmits have been adjusted. Update pg
                 * min in HW and update corresponding operational field.
                 */
                ptid = MMU_THDI_PORT_PG_MIN_CONFIGm;
                midx = th4g_piped_mem_index(unit, lport, ptid, 0);
                BCMTM_PT_DYN_INFO(pt_dyn_info, midx, pipe);
                pt_dyn_info.flags = flags;
                sal_memset(ltmbuf, 0, sizeof(ltmbuf));
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, ptid, field_pgmin_limit[pg],
                                     ltmbuf, &new_pg_min));
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_write(unit, ptid, sid, &pt_dyn_info, ltmbuf));

                /* Update operational fields. */
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, output_fields,
                        TM_ING_THD_PORT_PRI_GRPt_MIN_GUARANTEE_CELLS_OPERf, 0, new_pg_min));
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, output_fields,
                        TM_ING_THD_PORT_PRI_GRPt_OPERATIONAL_STATEf, 0, VALID));
            } else if (rv == SHR_E_PARAM) {
                /* Request declined. Update operational state field. */
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, output_fields,
                        TM_ING_THD_PORT_PRI_GRPt_OPERATIONAL_STATEf,
                        0, INCORRECT_MIN_GUARANTEE));
            } else {
                SHR_ERR_EXIT(rv);
            }
            break;
        } else {
            data_ptr = data_ptr->next;
        }
    }

    /* Third iteration: other fields. */
    data_ptr = data_fields;
    while (data_ptr) {
        write_flag = TRUE;
        switch (data_ptr->id) {
            case TM_ING_THD_PORT_PRI_GRPt_TM_ING_BST_THD_PRI_GRP_PROFILE_IDf:
                /* Write both SHARED/HDRM_PROFILE_SEL. */
                ptid = MMU_THDI_PORT_BST_CONFIGm;
                fval = data_ptr->data;
                midx = th4g_piped_mem_index(unit, lport, ptid, 0);
                BCMTM_PT_DYN_INFO(pt_dyn_info, midx, pipe);
                pt_dyn_info.flags = flags;
                sal_memset(ltmbuf, 0, sizeof(ltmbuf));
                /* Read - modify - write (both fields) . */
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, ptid, field_bst_hdrm_profile[pg],
                                     ltmbuf, &fval));
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, ptid, field_bst_shared_profile[pg],
                                     ltmbuf, &fval));
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_write(unit, ptid, sid, &pt_dyn_info, ltmbuf));
                /* Do not write again. */
                write_flag = FALSE;
                break;
            case TM_ING_THD_PORT_PRI_GRPt_HEADROOM_LIMIT_CELLSf:
                ptid = MMU_THDI_PORT_PG_HDRM_CONFIGm;
                fid = field_pghdrm_limit[pg];
                if (!hdrm_auto) {
                    pg_hdrm = data_ptr->data;
                } else {
                    pg_hdrm = buf_prigroup.headroom;
                }
                /* Update operational field. */
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, output_fields,
                        TM_ING_THD_PORT_PRI_GRPt_HEADROOM_LIMIT_CELLS_OPERf, 0, pg_hdrm));
                break;
            case TM_ING_THD_PORT_PRI_GRPt_RESUME_FLOOR_CELLSf:
                ptid = MMU_THDI_PORT_PG_RESUME_CONFIGm;
                fid = field_pgreset_floor[pg];
                break;
            case TM_ING_THD_PORT_PRI_GRPt_RESUME_OFFSET_CELLSf:
                ptid = MMU_THDI_PORT_PG_RESUME_CONFIGm;
                fid = field_pgreset_offset[pg];
                break;
            /* Only one shared limit will be written to HW. */
            case TM_ING_THD_PORT_PRI_GRPt_SHARED_LIMIT_DYNAMICf:
                if (use_dynamic) {
                    ptid = MMU_THDI_PORT_PG_SHARED_CONFIGm;
                    fid = field_pgshared_limit[pg];
                } else {
                    write_flag = FALSE;
                }
                break;
            case TM_ING_THD_PORT_PRI_GRPt_SHARED_LIMIT_CELLS_STATICf:
                if (use_dynamic) {
                    write_flag = FALSE;
                } else {
                    ptid = MMU_THDI_PORT_PG_SHARED_CONFIGm;
                    fid = field_pgshared_limit[pg];
                }
                break;
            default:
                write_flag = FALSE;
                break;
        }
        if (write_flag) {
            fval = data_ptr->data;
            if (data_ptr->id == TM_ING_THD_PORT_PRI_GRPt_HEADROOM_LIMIT_CELLSf) {
                fval = pg_hdrm;
            }
            midx = th4g_piped_mem_index(unit, lport, ptid, 0);
            BCMTM_PT_DYN_INFO(pt_dyn_info, midx, pipe);
            pt_dyn_info.flags = flags;
            sal_memset(ltmbuf, 0, sizeof(ltmbuf));
            /* Read - modify - write. */
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, ptid, fid, ltmbuf, &fval));
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, ptid, sid, &pt_dyn_info, ltmbuf));
        }
        data_ptr = data_ptr->next;
    }

exit:
    bcmtm_field_list_free(&keys);
    bcmtm_field_list_free(&entry);
    SHR_FUNC_EXIT();
}

static int
tm_thd_ing_port_pri_grp_delete(int unit, bcmltd_sid_t sid,
                               const bcmltd_op_arg_t *op_arg,
                               int lport, int pg)
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
        (tm_thd_ing_port_pri_grp_update(unit, sid, op_arg, lport, pg, data_fields,
                                        &dummy_fields, FALSE));

exit:
    SHR_FREE(data_fields);
    bcmtm_field_list_free(&dummy_fields);
    SHR_FUNC_EXIT();
}


/*!
 * \brief In-memory logical table staging callback function.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] event_reason This is the reason for the entry event.
 * \param [in] key_fields This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data_field This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 * \param [out] output_fields This output parameter can be used by the component
 * to add fields into the entry. Typically this should be used for read-only
 * type fields that otherwise can not be set by the application.
 * The component must set the count field of the \c output_fields parameter to
 * indicate the actual number of fields that were set.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
tm_thd_ing_port_pri_grp_stage(int unit,
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

    key_ptr = key_fields;
    while (key_ptr) {
        switch (key_ptr->id) {
            case TM_ING_THD_PORT_PRI_GRPt_PORT_IDf:
                lport = key_ptr->data;
                break;
            case TM_ING_THD_PORT_PRI_GRPt_TM_PRI_GRP_IDf:
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
                (tm_thd_ing_port_pri_grp_update(unit, sid, op_arg, lport, pg,
                                                data_fields, output_fields,
                                                UPDATE_ONLY));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT
                (tm_thd_ing_port_pri_grp_delete(unit, sid, op_arg, lport, pg));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
tm_thd_ing_port_pri_grp_lookup(int unit,
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
    int lport, pport, pg, midx, pipe;
    uint32_t ltmbuf[BCMTM_MAX_THD_ENTRY_WSIZE] = {0};
    /* Per-port-per-pg fields. */
    bcmdrd_fid_t field_pgmin_limit[SOC_MMU_CFG_PRI_GROUP_MAX] = {
            PG0_MIN_LIMITf, PG1_MIN_LIMITf, PG2_MIN_LIMITf,
            PG3_MIN_LIMITf, PG4_MIN_LIMITf, PG5_MIN_LIMITf,
            PG6_MIN_LIMITf, PG7_MIN_LIMITf};
    bcmdrd_fid_t field_pghdrm_limit[SOC_MMU_CFG_PRI_GROUP_MAX] = {
            PG0_HDRM_LIMITf, PG1_HDRM_LIMITf, PG2_HDRM_LIMITf,
            PG3_HDRM_LIMITf, PG4_HDRM_LIMITf, PG5_HDRM_LIMITf,
            PG6_HDRM_LIMITf, PG7_HDRM_LIMITf};
    bcmdrd_fid_t field_pgshared_limit[SOC_MMU_CFG_PRI_GROUP_MAX] = {
            PG0_SHARED_LIMITf, PG1_SHARED_LIMITf, PG2_SHARED_LIMITf,
            PG3_SHARED_LIMITf, PG4_SHARED_LIMITf, PG5_SHARED_LIMITf,
            PG6_SHARED_LIMITf, PG7_SHARED_LIMITf};
    bcmdrd_fid_t field_pgshared_dynamic[SOC_MMU_CFG_PRI_GROUP_MAX] = {
            PG0_SHARED_DYNAMICf, PG1_SHARED_DYNAMICf, PG2_SHARED_DYNAMICf,
            PG3_SHARED_DYNAMICf, PG4_SHARED_DYNAMICf, PG5_SHARED_DYNAMICf,
            PG6_SHARED_DYNAMICf, PG7_SHARED_DYNAMICf};
    bcmdrd_fid_t field_pgreset_floor[SOC_MMU_CFG_PRI_GROUP_MAX] = {
            PG0_RESET_FLOORf, PG1_RESET_FLOORf, PG2_RESET_FLOORf,
            PG3_RESET_FLOORf, PG4_RESET_FLOORf, PG5_RESET_FLOORf,
            PG6_RESET_FLOORf, PG7_RESET_FLOORf};
    bcmdrd_fid_t field_pgreset_offset[SOC_MMU_CFG_PRI_GROUP_MAX] = {
            PG0_RESET_OFFSETf, PG1_RESET_OFFSETf, PG2_RESET_OFFSETf,
            PG3_RESET_OFFSETf, PG4_RESET_OFFSETf, PG5_RESET_OFFSETf,
            PG6_RESET_OFFSETf, PG7_RESET_OFFSETf};
    bcmdrd_fid_t field_bst_hdrm_profile[SOC_MMU_CFG_PRI_GROUP_MAX] = {
            PG0_BST_HDRM_PROFILE_SELf, PG1_BST_HDRM_PROFILE_SELf,
            PG2_BST_HDRM_PROFILE_SELf, PG3_BST_HDRM_PROFILE_SELf,
            PG4_BST_HDRM_PROFILE_SELf, PG5_BST_HDRM_PROFILE_SELf,
            PG6_BST_HDRM_PROFILE_SELf, PG7_BST_HDRM_PROFILE_SELf};

    SHR_FUNC_ENTER(unit);

    if (!(op_arg->attrib & BCMLT_ENT_ATTR_GET_FROM_HW)) {
        SHR_EXIT();
    }
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    key = (lkup_type == BCMIMM_TRAVERSE) ? out : in;
    fid = TM_ING_THD_PORT_PRI_GRPt_PORT_IDf;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_imm_fields_get(unit, key, fid, &idx));
    lport = idx;
    fid = TM_ING_THD_PORT_PRI_GRPt_TM_PRI_GRP_IDf;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_imm_fields_get(unit, key, fid, &idx));
    pg = idx;

    /* Check for invalid port (port is not added). */
    if (SHR_FAILURE(bcmtm_lport_pport_get(unit, lport, &pport))) {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

    SHR_IF_ERR_EXIT
        (bcmtm_lport_pipe_get(unit, lport, &pipe));

    ptid = MMU_THDI_PORT_PG_SHARED_CONFIGm;
    midx = th4g_piped_mem_index(unit, lport, ptid, 0);
    BCMTM_PT_DYN_INFO(pt_dyn_info, midx, pipe);
    pt_dyn_info.flags = req_flags;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));

    SHR_IF_ERR_EXIT(
        bcmtm_field_get(unit, ptid, field_pgshared_dynamic[pg], ltmbuf, &fval));
    SHR_IF_ERR_EXIT(
        bcmcth_imm_field_update(unit, out,
                                TM_ING_THD_PORT_PRI_GRPt_DYNAMIC_SHARED_LIMITSf,
                                fval));

    ptid = MMU_THDI_PORT_PG_HDRM_CONFIGm;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    /* Read - modify - write. */
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT(
        bcmtm_field_get(unit, ptid, field_pghdrm_limit[pg], ltmbuf, &fval));
    SHR_IF_ERR_EXIT(
        bcmcth_imm_field_update(unit, out,
                                TM_ING_THD_PORT_PRI_GRPt_HEADROOM_LIMIT_AUTOf,
                                fval));

    ptid = MMU_THDI_PORT_PG_MIN_CONFIGm;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT(
        bcmtm_field_get(unit, ptid, field_pgmin_limit[pg], ltmbuf, &fval));
    SHR_IF_ERR_EXIT(
        bcmcth_imm_field_update(unit, out,
                                TM_ING_THD_PORT_PRI_GRPt_MIN_GUARANTEE_CELLSf,
                                fval));

    ptid = MMU_THDI_PORT_BST_CONFIGm;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT(
        bcmtm_field_get(unit, ptid, field_bst_hdrm_profile[pg], ltmbuf, &fval));
    SHR_IF_ERR_EXIT(
        bcmcth_imm_field_update(unit, out,
                                TM_ING_THD_PORT_PRI_GRPt_TM_ING_BST_THD_PRI_GRP_PROFILE_IDf,
                                fval));

    ptid = MMU_THDI_PORT_PG_HDRM_CONFIGm;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT(
        bcmtm_field_get(unit, ptid, field_pghdrm_limit[pg], ltmbuf, &fval));
    SHR_IF_ERR_EXIT(
        bcmcth_imm_field_update(unit, out,
                                TM_ING_THD_PORT_PRI_GRPt_HEADROOM_LIMIT_CELLSf,
                                fval));

    ptid = MMU_THDI_PORT_PG_RESUME_CONFIGm;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT(
        bcmtm_field_get(unit, ptid, field_pgreset_floor[pg],
                        ltmbuf, &fval));
    SHR_IF_ERR_EXIT(
        bcmcth_imm_field_update(unit, out,
                                TM_ING_THD_PORT_PRI_GRPt_RESUME_FLOOR_CELLSf,
                                fval));

    ptid = MMU_THDI_PORT_PG_RESUME_CONFIGm;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT(
        bcmtm_field_get(unit, ptid, field_pgreset_offset[pg], ltmbuf, &fval));
    SHR_IF_ERR_EXIT(
        bcmcth_imm_field_update(unit, out,
                                TM_ING_THD_PORT_PRI_GRPt_RESUME_OFFSET_CELLSf,
                                fval));

    ptid = MMU_THDI_PORT_PG_SHARED_CONFIGm;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT(
        bcmtm_field_get(unit, ptid, field_pgshared_limit[pg], ltmbuf, &fval));
    SHR_IF_ERR_EXIT(
        bcmcth_imm_field_update(unit, out,
                                TM_ING_THD_PORT_PRI_GRPt_SHARED_LIMIT_DYNAMICf,
                                fval));
    SHR_IF_ERR_EXIT(
        bcmtm_field_get(unit, ptid, field_pgshared_limit[pg], ltmbuf, &fval));
    SHR_IF_ERR_EXIT(
        bcmcth_imm_field_update(unit, out,
                                TM_ING_THD_PORT_PRI_GRPt_SHARED_LIMIT_CELLS_STATICf,
                                fval));

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */

int
bcm56996_a0_tm_thd_ing_port_pri_grp_register(int unit)
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
    tm_thd_cb_handler.op_stage = tm_thd_ing_port_pri_grp_stage;
    tm_thd_cb_handler.op_lookup = tm_thd_ing_port_pri_grp_lookup;

    rv = bcmlrd_map_get(unit, TM_ING_THD_PORT_PRI_GRPt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    /* Register callback handler for the LT. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_ING_THD_PORT_PRI_GRPt, &tm_thd_cb_handler, NULL));

exit:
    SHR_FUNC_EXIT();
}

int
bcm56996_a0_tm_thd_ing_port_pri_grp_populate(int unit)
{
    int lport, pg, rv;
    const bcmlrd_map_t *map = NULL;
    bcmtm_drv_info_t *drv_info;
    soc_mmu_cfg_buf_prigroup_t buf_prigroup;

    SHR_FUNC_ENTER(unit);

    bcmtm_drv_info_get(unit, &drv_info);

    rv = bcmlrd_map_get(unit, TM_ING_THD_PORT_PRI_GRPt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }
    for (lport = 0; lport < SOC_MMU_CFG_PORT_MAX; lport++) {
        for (pg = 0; pg < SOC_MMU_CFG_PRI_GROUP_MAX; pg++) {
            th4g_mmu_config_buf_prigroup_get(unit, lport, pg,
                                            &buf_prigroup, drv_info->lossless);
            SHR_IF_ERR_EXIT
                (tm_thd_ing_port_pri_grp_populate_entry(unit, lport, pg, 1, &buf_prigroup));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm56996_a0_tm_thd_ing_port_pri_grp_update(int unit, int pport, int up)
{
    int lport, pg, rv;
    const bcmlrd_map_t *map = NULL;
    bcmltd_fields_t lkup_fields, keys, out_fields;
    size_t num_fid;
    bcmltd_sid_t sid = TM_ING_THD_PORT_PRI_GRPt;
    bcmltd_field_t *data_fields = NULL;
    int j;

    SHR_FUNC_ENTER(unit);

    sal_memset(&lkup_fields, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out_fields, 0, sizeof(bcmltd_fields_t));
    sal_memset(&keys, 0, sizeof(bcmltd_fields_t));
    rv = bcmlrd_map_get(unit, TM_ING_THD_PORT_PRI_GRPt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL)|| (!map)) {
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
        keys.field[0]->id   = TM_ING_THD_PORT_PRI_GRPt_PORT_IDf;
        keys.field[0]->data = lport;
        keys.field[1]->id   = TM_ING_THD_PORT_PRI_GRPt_TM_PRI_GRP_IDf;
        keys.field[1]->data = pg;

        if (SHR_SUCCESS(bcmimm_entry_lookup(unit, sid, &keys, &lkup_fields))) {
            for (j = 0; j < (int)lkup_fields.count; j++) {
               data_fields[j] = *(lkup_fields).field[j];
            }

            SHR_IF_ERR_EXIT
                (tm_thd_ing_port_pri_grp_update(unit, sid, NULL, lport, pg,
                                               data_fields,
                                               &out_fields, up));
            if (up == 0) {
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, &out_fields,
                        TM_ING_THD_PORT_PRI_GRPt_OPERATIONAL_STATEf, 0,
                        PORT_INFO_UNAVAILABLE));
            }
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, &out_fields,
                                 TM_ING_THD_PORT_PRI_GRPt_PORT_IDf, 0, lport));
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, &out_fields,
                                 TM_ING_THD_PORT_PRI_GRPt_TM_PRI_GRP_IDf, 0,
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
