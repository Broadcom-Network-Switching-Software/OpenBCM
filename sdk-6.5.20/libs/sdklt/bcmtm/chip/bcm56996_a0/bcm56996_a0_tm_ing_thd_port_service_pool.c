/*! \file bcm56996_a0_tm_ing_thd_port_service_pool.c
 *
 * IMM handlers for threshold LTs.
 *
 * This file contains callbcak handlers for LT TM_ING_THD_PORT_SERVICE_POOL.
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
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmtm/chip/bcm56996_a0_tm_thd_internal.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmcth/bcmcth_imm_util.h>

#include "bcm56996_a0_tm_imm_internal.h"

#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM_CHIP

/* TM_ING_THD_PORT_SERVICE_POOL_ENTRY_STATE_T */
#define VALID                   0
#define PORT_INFO_UNAVAILABLE   1
#define INCORRECT_MIN_GUARANTEE 2
#define INCORRECT_RESUME_LIMIT  3

#define NUM_KEY_FIELDS          2
#define NUM_OPER_FIELDS         3

static int
tm_thd_ing_port_service_pool_populate_entry(int unit, int lport, int pool, int up)
{
    bcmltd_fields_t entry;
    bcmltd_sid_t sid = TM_ING_THD_PORT_SERVICE_POOLt;
    size_t num_fid;

    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(bcmltd_fields_t));

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
            TM_ING_THD_PORT_SERVICE_POOLt_PORT_IDf, 0, lport));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_ING_THD_PORT_SERVICE_POOLt_TM_ING_SERVICE_POOL_IDf, 0, pool));

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_ING_THD_PORT_SERVICE_POOLt_OPERATIONAL_STATEf, 0, PORT_INFO_UNAVAILABLE));
    SHR_IF_ERR_EXIT
        (bcmimm_entry_insert(unit, sid, &entry));

exit:
    bcmtm_field_list_free(&entry);
    SHR_FUNC_EXIT();
}

static int
tm_thd_ing_port_service_pool_update(int unit,
                                    bcmltd_sid_t sid,
                                    const bcmltd_op_arg_t *op_arg,
                                    int lport,
                                    int pool,
                                    const bcmltd_field_t *data_fields,
                                    bcmltd_fields_t *output_fields,
                                    bool port_add)
{
    const bcmltd_field_t *data_ptr;
    uint64_t field_data;
    bcmltd_fields_t entry;
    int pport, pipe, midx;
    long int delta;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_sid_t ptid;
    bcmdrd_fid_t fid;
    uint32_t shared_limit, min_guarantee, resume_limit, fval;
    uint32_t ltmbuf[BCMTM_MAX_THD_ENTRY_WSIZE] = {0};
    bool write_flag;
    bcmltd_fields_t keys;
    size_t num_fid;
    soc_mmu_cfg_buf_port_pool_t buf_port_pool;
    uint64_t flags = 0;

    bcmdrd_fid_t field_sp_min[SOC_MMU_CFG_SERVICE_POOL_MAX] = {
            PORTSP0_MIN_LIMITf, PORTSP1_MIN_LIMITf,
            PORTSP2_MIN_LIMITf, PORTSP3_MIN_LIMITf};
    bcmdrd_fid_t field_sp_shared[SOC_MMU_CFG_SERVICE_POOL_MAX] = {
            PORTSP0_SHARED_LIMITf, PORTSP1_SHARED_LIMITf,
            PORTSP2_SHARED_LIMITf, PORTSP3_SHARED_LIMITf};
    bcmdrd_fid_t field_sp_resume[SOC_MMU_CFG_SERVICE_POOL_MAX] = {
            PORTSP0_RESUME_LIMITf, PORTSP1_RESUME_LIMITf,
            PORTSP2_RESUME_LIMITf, PORTSP3_RESUME_LIMITf};
    bcmdrd_fid_t field_bst_shared_profile[SOC_MMU_CFG_SERVICE_POOL_MAX] = {
            PORTSP0_BST_SHARED_PROFILE_SELf, PORTSP1_BST_SHARED_PROFILE_SELf,
            PORTSP2_BST_SHARED_PROFILE_SELf, PORTSP3_BST_SHARED_PROFILE_SELf};

    SHR_FUNC_ENTER(unit);
    if (op_arg) {
        flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    }

    th4g_mmu_config_buf_pool_get(unit, pool, &buf_port_pool);

    /* Try to retrieve operational fields from the LT entry. */
    sal_memset(&entry, 0, sizeof(bcmltd_fields_t));
    sal_memset(&keys, 0, sizeof(bcmltd_fields_t));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, NUM_KEY_FIELDS, &keys));
    keys.field[0]->id   = TM_ING_THD_PORT_SERVICE_POOLt_PORT_IDf;
    keys.field[0]->data = lport;
    keys.field[1]->id   = TM_ING_THD_PORT_SERVICE_POOLt_TM_ING_SERVICE_POOL_IDf;
    keys.field[1]->data = pool;
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, sid, &num_fid));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &entry));
    SHR_IF_ERR_EXIT
        (bcmimm_entry_lookup(unit, sid, &keys, &entry));

    /* Initialize output_ields. */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_size_check(unit, output_fields, NUM_OPER_FIELDS));
    output_fields->count = 0;

    /* Check for invalid port (port is not added). */
    if (SHR_FAILURE(bcmtm_lport_pport_get(unit, lport, &pport))) {
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, output_fields,
                TM_ING_THD_PORT_SERVICE_POOLt_OPERATIONAL_STATEf, 0, PORT_INFO_UNAVAILABLE));
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT
        (bcmtm_lport_pipe_get(unit, lport, &pipe));

    /* Retrieve current configuration from LT or HW. */
    /* LT entry available. */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_get(unit, &entry,
            TM_ING_THD_PORT_SERVICE_POOLt_MIN_GUARANTEE_CELLS_OPERf, 0, &field_data));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, output_fields,
            TM_ING_THD_PORT_SERVICE_POOLt_MIN_GUARANTEE_CELLS_OPERf, 0, field_data));
    min_guarantee = (uint32_t)field_data;

    /* Also get shared_limit for future reference. */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_get(unit, &entry,
            TM_ING_THD_PORT_SERVICE_POOLt_SHARED_LIMIT_CELLSf, 0, &field_data));
    shared_limit = (uint32_t)field_data;

    if (port_add) {
        /* OPERATIONAL_STATE is VALID by default. */
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, output_fields,
                TM_ING_THD_PORT_SERVICE_POOLt_OPERATIONAL_STATEf, 0, VALID));
        /* Port added for frst time. Read Arch defalt values form structure */
        min_guarantee = buf_port_pool.guarantee;
    }

    /*
     * First iteration:
     * Shared limit is independent of min guarantee.
     * Resume limit must be less than or equal to shared limit.
     */
    data_ptr = data_fields;
    while (data_ptr) {
        if (data_ptr->id == TM_ING_THD_PORT_SERVICE_POOLt_SHARED_LIMIT_CELLSf) {
            shared_limit = data_ptr->data;
            ptid = MMU_THDI_PORTSP_CONFIGm;
            fid = field_sp_shared[pool];
            midx = th4g_piped_mem_index(unit, lport, ptid, 0);
            BCMTM_PT_DYN_INFO(pt_dyn_info, midx, pipe);
            pt_dyn_info.flags = flags;
            sal_memset(ltmbuf, 0, sizeof(ltmbuf));
            /* Read - modify - write. */
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, ptid, fid, ltmbuf, &shared_limit));
            /* Check current resume limit and adjust if necessary. */
            fid = field_sp_resume[pool];
            SHR_IF_ERR_EXIT
                (bcmtm_field_get(unit, ptid, fid, ltmbuf, &resume_limit));
            if (resume_limit > shared_limit) {
                resume_limit = shared_limit;
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, ptid, fid, ltmbuf, &resume_limit));
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, output_fields,
                        TM_ING_THD_PORT_SERVICE_POOLt_RESUME_LIMIT_CELLS_OPERf,
                        0, resume_limit));
            }
            /* Check if user configured resume limit can be allocated now. */
            if (SHR_SUCCESS
                    (bcmtm_field_list_get(unit, &entry,
                              TM_ING_THD_PORT_SERVICE_POOLt_RESUME_LIMIT_CELLSf,
                              0, &field_data))) {
                resume_limit = (uint32_t)field_data;
                if (resume_limit <= shared_limit) {
                    SHR_IF_ERR_EXIT
                        (bcmtm_field_set(unit, ptid, fid, ltmbuf, &resume_limit));
                    SHR_IF_ERR_EXIT
                        (bcmtm_field_list_set(unit, output_fields,
                            TM_ING_THD_PORT_SERVICE_POOLt_RESUME_LIMIT_CELLS_OPERf,
                            0, resume_limit));
                }
            }
            /* Continue to write the final value. */
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, ptid, sid, &pt_dyn_info, ltmbuf));
            break;
        } else {
            data_ptr = data_ptr->next;
        }
    }

    /* Second iteration: other fields. */
    data_ptr = data_fields;
    while (data_ptr) {
        write_flag = TRUE;
        switch (data_ptr->id) {
            int itm, rv;
            uint32_t new_min_guarantee;

            case TM_ING_THD_PORT_SERVICE_POOLt_TM_ING_BST_THD_PORT_SERVICE_POOL_PROFILE_IDf:
                ptid = MMU_THDI_PORT_BST_CONFIGm;
                fid = field_bst_shared_profile[pool];
                break;
            case TM_ING_THD_PORT_SERVICE_POOLt_RESUME_LIMIT_CELLSf:
                if (data_ptr->data > shared_limit) {
                    /* Cannot alocate. Update operational state. */
                    SHR_IF_ERR_EXIT
                        (bcmtm_field_list_set(unit, output_fields,
                            TM_ING_THD_PORT_SERVICE_POOLt_OPERATIONAL_STATEf,
                            0, INCORRECT_RESUME_LIMIT));
                    write_flag = FALSE;
                } else {
                    ptid = MMU_THDI_PORTSP_CONFIGm;
                    fid = field_sp_resume[pool];
                    SHR_IF_ERR_EXIT
                        (bcmtm_field_list_set(unit, output_fields,
                            TM_ING_THD_PORT_SERVICE_POOLt_RESUME_LIMIT_CELLS_OPERf,
                            0, data_ptr->data));
                }
                break;
            case TM_ING_THD_PORT_SERVICE_POOLt_MIN_GUARANTEE_CELLSf:
                new_min_guarantee = data_ptr->data;
                /*
                 * Calculate delta of available shared size.
                 * delta of shared_size always equals to -(delta of reserved_size).
                 */
                delta = -((long int) new_min_guarantee - min_guarantee);
                SHR_IF_ERR_EXIT
                    (bcm56996_a0_tm_thdi_port_to_itm_mapping_get(unit, lport, &itm));
                /* Check if there is enough buffer size to allocate the delta. */
                rv = bcm56996_a0_tm_shared_buffer_update(unit, itm, pool, delta,
                                                         RESERVED_BUFFER_UPDATE,
                                                         FALSE);
                if (rv == SHR_E_NONE) {
                    /*
                     * Request accepted. Other limmits have been adjusted. Update pg
                     * min in HW and update corresponding operational field.
                     */
                    ptid = MMU_THDI_PORTSP_CONFIGm;
                    fid = field_sp_min[pool];

                    /* Update operational fields. */
                    SHR_IF_ERR_EXIT
                        (bcmtm_field_list_set(unit, output_fields,
                            TM_ING_THD_PORT_SERVICE_POOLt_MIN_GUARANTEE_CELLS_OPERf,
                            0, new_min_guarantee));
                    SHR_IF_ERR_EXIT
                        (bcmtm_field_list_set(unit, output_fields,
                            TM_ING_THD_PORT_SERVICE_POOLt_OPERATIONAL_STATEf,
                            0, VALID));
                } else if (rv == SHR_E_PARAM) {
                    /* Request declined. Update operational state field. */
                    SHR_IF_ERR_EXIT
                        (bcmtm_field_list_set(unit, output_fields,
                            TM_ING_THD_PORT_SERVICE_POOLt_OPERATIONAL_STATEf,
                            0, INCORRECT_MIN_GUARANTEE));
                    write_flag = FALSE;
                } else {
                    SHR_ERR_EXIT(rv);
                }
                break;
            default:
                write_flag = FALSE;
                break;
        }
        if (write_flag) {
            fval = data_ptr->data;
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
tm_thd_ing_port_service_pool_delete(int unit, int sid,
                                    const bcmltd_op_arg_t *op_arg,
                                    int lport, int pool)
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
        (tm_thd_ing_port_service_pool_update(unit, sid, op_arg, lport, pool,
                                             data_fields, &dummy_fields, FALSE));

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
tm_thd_ing_port_service_pool_stage(int unit,
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
    int pool = -1;

    SHR_FUNC_ENTER(unit);

    key_ptr = key_fields;
    while (key_ptr) {
        switch (key_ptr->id) {
            case TM_ING_THD_PORT_SERVICE_POOLt_PORT_IDf:
                lport = key_ptr->data;
                break;
            case TM_ING_THD_PORT_SERVICE_POOLt_TM_ING_SERVICE_POOL_IDf:
                pool = key_ptr->data;
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        key_ptr = key_ptr->next;
    }

    if (lport < 0 || pool < 0) {
        return SHR_E_PARAM;
    }

    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
            /* Insert shares the same callback with update. */
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_EXIT
                (tm_thd_ing_port_service_pool_update(unit, sid, op_arg, lport, pool,
                                                     data_fields, output_fields,
                                                     FALSE));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT
                (tm_thd_ing_port_service_pool_delete(unit, sid, op_arg, lport, pool));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
tm_thd_ing_port_service_pool_lookup(int unit,
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
    int lport, pport, pool, midx, pipe;
    uint32_t ltmbuf[BCMTM_MAX_THD_ENTRY_WSIZE] = {0};
    bcmdrd_fid_t field_sp_min[SOC_MMU_CFG_SERVICE_POOL_MAX] = {
            PORTSP0_MIN_LIMITf, PORTSP1_MIN_LIMITf,
            PORTSP2_MIN_LIMITf, PORTSP3_MIN_LIMITf};
    bcmdrd_fid_t field_sp_shared[SOC_MMU_CFG_SERVICE_POOL_MAX] = {
            PORTSP0_SHARED_LIMITf, PORTSP1_SHARED_LIMITf,
            PORTSP2_SHARED_LIMITf, PORTSP3_SHARED_LIMITf};
    bcmdrd_fid_t field_sp_resume[SOC_MMU_CFG_SERVICE_POOL_MAX] = {
            PORTSP0_RESUME_LIMITf, PORTSP1_RESUME_LIMITf,
            PORTSP2_RESUME_LIMITf, PORTSP3_RESUME_LIMITf};
    bcmdrd_fid_t field_bst_shared_profile[SOC_MMU_CFG_SERVICE_POOL_MAX] = {
            PORTSP0_BST_SHARED_PROFILE_SELf, PORTSP1_BST_SHARED_PROFILE_SELf,
            PORTSP2_BST_SHARED_PROFILE_SELf, PORTSP3_BST_SHARED_PROFILE_SELf};

    SHR_FUNC_ENTER(unit);

    if (!(op_arg->attrib & BCMLT_ENT_ATTR_GET_FROM_HW)) {
        SHR_EXIT();
    }
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    key = (lkup_type == BCMIMM_TRAVERSE) ? out : in;
    fid = TM_ING_THD_PORT_SERVICE_POOLt_PORT_IDf;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_imm_fields_get(unit, key, fid, &idx));
    lport = idx;
    fid = TM_ING_THD_PORT_SERVICE_POOLt_TM_ING_SERVICE_POOL_IDf;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_imm_fields_get(unit, key, fid, &idx));
    pool = idx;
    /* Check for invalid port (port is not added). */
    if (SHR_FAILURE(bcmtm_lport_pport_get(unit, lport, &pport))) {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

    SHR_IF_ERR_EXIT
        (bcmtm_lport_pipe_get(unit, lport, &pipe));

    ptid = MMU_THDI_PORTSP_CONFIGm;
    midx = th4g_piped_mem_index(unit, lport, ptid, 0);
    BCMTM_PT_DYN_INFO(pt_dyn_info, midx, pipe);
    pt_dyn_info.flags = req_flags;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT(
        bcmtm_field_get(unit, ptid, field_sp_shared[pool], ltmbuf, &fval));
    SHR_IF_ERR_EXIT(
        bcmcth_imm_field_update(unit, out,
                                TM_ING_THD_PORT_SERVICE_POOLt_SHARED_LIMIT_CELLSf,
                                fval));
    SHR_IF_ERR_EXIT(
        bcmtm_field_get(unit, ptid, field_sp_resume[pool], ltmbuf, &fval));
    SHR_IF_ERR_EXIT(
        bcmcth_imm_field_update(unit, out,
                                TM_ING_THD_PORT_SERVICE_POOLt_RESUME_LIMIT_CELLSf,
                                fval));
    SHR_IF_ERR_EXIT(
        bcmtm_field_get(unit, ptid, field_sp_min[pool], ltmbuf, &fval));
    SHR_IF_ERR_EXIT(
        bcmcth_imm_field_update(unit, out,
                                TM_ING_THD_PORT_SERVICE_POOLt_MIN_GUARANTEE_CELLSf,
                                fval));

    ptid = MMU_THDI_PORT_BST_CONFIGm;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT(
        bcmtm_field_get(unit, ptid, field_bst_shared_profile[pool], ltmbuf, &fval));
    SHR_IF_ERR_EXIT(
        bcmcth_imm_field_update(unit, out,
                                TM_ING_THD_PORT_SERVICE_POOLt_TM_ING_BST_THD_PORT_SERVICE_POOL_PROFILE_IDf,
                                fval));

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */

int
bcm56996_a0_tm_thd_ing_port_service_pool_register(int unit)
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
    tm_thd_cb_handler.op_stage = tm_thd_ing_port_service_pool_stage;
    tm_thd_cb_handler.op_lookup = tm_thd_ing_port_service_pool_lookup;

    rv = bcmlrd_map_get(unit, TM_ING_THD_PORT_SERVICE_POOLt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    /* Register callback handler for the LT. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_ING_THD_PORT_SERVICE_POOLt,
                             &tm_thd_cb_handler, NULL));

exit:
    SHR_FUNC_EXIT();
}

int
bcm56996_a0_tm_thd_ing_port_service_pool_populate(int unit)
{
    int lport, pool, rv;
    const bcmlrd_map_t *map = NULL;

    SHR_FUNC_ENTER(unit);
    rv = bcmlrd_map_get(unit, TM_ING_THD_PORT_SERVICE_POOLt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }


    for (lport = 0; lport < SOC_MMU_CFG_PORT_MAX; lport++) {
        for (pool = 0; pool < SOC_MMU_CFG_SERVICE_POOL_MAX; pool++) {
            SHR_IF_ERR_EXIT
                (tm_thd_ing_port_service_pool_populate_entry(unit, lport, pool, 1));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm56996_a0_tm_thd_ing_port_service_pool_update(int unit, int pport, int up)
{
    int lport;
    int pool, rv;
    const bcmlrd_map_t *map = NULL;
    bcmltd_fields_t lkup_fields, keys, out_fields;
    size_t num_fid;
    bcmltd_sid_t sid = TM_ING_THD_PORT_SERVICE_POOLt;
    bcmltd_field_t *data_fields = NULL;
    int j;

    SHR_FUNC_ENTER(unit);
    sal_memset(&lkup_fields, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out_fields, 0, sizeof(bcmltd_fields_t));
    sal_memset(&keys, 0, sizeof(bcmltd_fields_t));
    rv = bcmlrd_map_get(unit, TM_ING_THD_PORT_SERVICE_POOLt, &map);
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
    for (pool = 0; pool < SOC_MMU_CFG_SERVICE_POOL_MAX; pool++) {
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_alloc(unit, num_fid, &out_fields));
        keys.field[0]->id   = TM_ING_THD_PORT_SERVICE_POOLt_PORT_IDf;
        keys.field[0]->data = lport;
        keys.field[1]->id   =
            TM_ING_THD_PORT_SERVICE_POOLt_TM_ING_SERVICE_POOL_IDf;
        keys.field[1]->data = pool;

        if (SHR_SUCCESS(bcmimm_entry_lookup(unit, sid, &keys, &lkup_fields))) {
            for (j = 0; j < (int)lkup_fields.count; j++) {
               data_fields[j] = *(lkup_fields).field[j];
            }

            SHR_IF_ERR_EXIT
                (tm_thd_ing_port_service_pool_update(unit, sid, NULL, lport, pool,
                                           data_fields, &out_fields, TRUE));
            if (up == 0) {
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, &out_fields,
                        TM_ING_THD_PORT_SERVICE_POOLt_OPERATIONAL_STATEf, 0,
                        PORT_INFO_UNAVAILABLE));
            }
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, &out_fields,
                    TM_ING_THD_PORT_SERVICE_POOLt_PORT_IDf, 0, lport));
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, &out_fields,
                    TM_ING_THD_PORT_SERVICE_POOLt_TM_ING_SERVICE_POOL_IDf, 0,
                    pool));
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
