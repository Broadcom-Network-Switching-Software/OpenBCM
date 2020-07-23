/*! \file bcm56996_a0_tm_egr_thd_service_pool.c
 *
 * IMM handlers for threshold LTs.
 *
 * This file contains callbcak handlers for LT TM_THD_EGR_SERVICE_POOL.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <bcmdrd/chip/bcm56996_a0_enum.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmtm/chip/bcm56996_a0_tm_thd_internal.h>
#include <bcmtm/chip/bcm56996_a0_tm.h>
#include <bcmcth/bcmcth_imm_util.h>

#include "bcm56996_a0_tm_imm_internal.h"

/* TM_EGR_THD_SERVICE_POOL_ENTRY_STATE_T */
#define VALID                   0
#define INCORRECT_SHARED_LIMIT  1
#define INCORRECT_RESUME_LIMIT  2
#define INCORRECT_COLOR_LIMIT   3

#define NUM_KEY_FIELDS          2
#define NUM_OPER_FIELDS         7

#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM_CHIP

static int
update_red_resume(int unit, int itm, int pool, uint32_t red_limit,
                  bcmltd_fields_t *entry, bcmltd_fields_t *ref_entry)
{
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_sid_t ptid = MMU_THDO_SHARED_DB_POOL_RED_RESUME_LIMITr;
    bcmdrd_fid_t fid = RED_RESUME_LIMITf;
    bcmltd_sid_t sid = TM_EGR_THD_SERVICE_POOLt;
    uint32_t ltmbuf[BCMTM_MAX_THD_ENTRY_WSIZE] = {0};
    uint32_t red_resume, fval;
    uint64_t lt_red_resume, lt_red_resume_oper;
    bool write_flag;

    SHR_FUNC_ENTER(unit);

    /* Read pool red resume limit. */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_get(unit, ref_entry,
            TM_EGR_THD_SERVICE_POOLt_RED_SHARED_RESUME_LIMIT_CELLSf, 0, &lt_red_resume));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_get(unit, ref_entry,
            TM_EGR_THD_SERVICE_POOLt_RED_SHARED_RESUME_LIMIT_CELLS_OPERf, 0, &lt_red_resume_oper));
    BCMTM_PT_DYN_INFO(pt_dyn_info, pool, itm);
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));

    write_flag = FALSE;
    if (CEILING(lt_red_resume, TH4G_GRANULARITY) <= red_limit &&
        lt_red_resume != lt_red_resume_oper) {
        /* Install user configured value */
        write_flag = TRUE;
        fval = CEILING(lt_red_resume, TH4G_GRANULARITY);
    } else {
        /* Check current operating value */
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
        SHR_IF_ERR_EXIT
            (bcmtm_field_get(unit, ptid, fid, ltmbuf, &red_resume));
        if (red_resume > red_limit) {
            write_flag = TRUE;
            fval = red_limit;
        }
    }

    if (write_flag) {
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, ptid, fid, ltmbuf, &fval));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, ptid, sid, &pt_dyn_info, ltmbuf));
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, entry,
                TM_EGR_THD_SERVICE_POOLt_RED_SHARED_RESUME_LIMIT_CELLS_OPERf,
                0, fval * TH4G_GRANULARITY));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
update_yellow_resume(int unit, int itm, int pool, uint32_t yellow_limit,
                     bcmltd_fields_t *entry, bcmltd_fields_t *ref_entry)
{
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_sid_t ptid = MMU_THDO_SHARED_DB_POOL_YELLOW_RESUME_LIMITr;
    bcmdrd_fid_t fid = YELLOW_RESUME_LIMITf;
    bcmltd_sid_t sid = TM_EGR_THD_SERVICE_POOLt;
    uint32_t ltmbuf[BCMTM_MAX_THD_ENTRY_WSIZE] = {0};
    uint32_t yellow_resume, fval;
    uint64_t lt_yellow_resume, lt_yellow_resume_oper;
    bool write_flag;

    SHR_FUNC_ENTER(unit);

    /* Read pool red resume limit. */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_get(unit, ref_entry,
            TM_EGR_THD_SERVICE_POOLt_YELLOW_SHARED_RESUME_LIMIT_CELLSf, 0, &lt_yellow_resume));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_get(unit, ref_entry,
            TM_EGR_THD_SERVICE_POOLt_YELLOW_SHARED_RESUME_LIMIT_CELLS_OPERf, 0, &lt_yellow_resume_oper));
    BCMTM_PT_DYN_INFO(pt_dyn_info, pool, itm);
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));

    write_flag = FALSE;
    if (CEILING(lt_yellow_resume, TH4G_GRANULARITY) <= yellow_limit &&
        lt_yellow_resume != lt_yellow_resume_oper) {
        /* Install user configured value */
        write_flag = TRUE;
        fval = CEILING(lt_yellow_resume, TH4G_GRANULARITY);
    } else {
        /* Check current operating value */
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
        SHR_IF_ERR_EXIT
            (bcmtm_field_get(unit, ptid, fid, ltmbuf, &yellow_resume));
        if (yellow_resume > yellow_limit) {
            write_flag = TRUE;
            fval = yellow_limit;
        }
    }

    if (write_flag) {
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, ptid, fid, ltmbuf, &fval));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, ptid, sid, &pt_dyn_info, ltmbuf));

        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, entry,
                TM_EGR_THD_SERVICE_POOLt_YELLOW_SHARED_RESUME_LIMIT_CELLS_OPERf,
                0, fval * TH4G_GRANULARITY));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
update_limits_by_shared_limit(int unit, int itm, int pool, uint32_t shared_limit,
                              bcmltd_fields_t *entry, bcmltd_fields_t *ref_entry)
{
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_sid_t ptid;
    bcmdrd_fid_t fid;
    bcmltd_sid_t sid = TM_EGR_THD_SERVICE_POOLt;
    uint32_t ltmbuf[BCMTM_MAX_THD_ENTRY_WSIZE] = {0};
    uint32_t resume_limit, yellow_limit, red_limit, fval;
    uint64_t lt_resume_limit, lt_yellow_limit, lt_red_limit, lt_resume_limit_oper,
             lt_yellow_limit_oper, lt_red_limit_oper;
    bool write_flag;

    SHR_FUNC_ENTER(unit);

    BCMTM_PT_DYN_INFO(pt_dyn_info, pool, itm);
    shared_limit = shared_limit / TH4G_GRANULARITY;

    /* Check shared resume limit. */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_get(unit, ref_entry,
            TM_EGR_THD_SERVICE_POOLt_SHARED_RESUME_LIMIT_CELLSf, 0, &lt_resume_limit));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_get(unit, ref_entry,
            TM_EGR_THD_SERVICE_POOLt_SHARED_RESUME_LIMIT_CELLS_OPERf, 0, &lt_resume_limit_oper));
    ptid = MMU_THDO_SHARED_DB_POOL_RESUME_LIMITr;
    fid = RESUME_LIMITf;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));

    write_flag = FALSE;
    if (CEILING(lt_resume_limit, TH4G_GRANULARITY) <= shared_limit &&
        lt_resume_limit != lt_resume_limit_oper) {
        /* Install user configured value */
        write_flag = TRUE;
        fval = CEILING(lt_resume_limit, TH4G_GRANULARITY);
    } else {
        /* Check current operating value */
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
        SHR_IF_ERR_EXIT
            (bcmtm_field_get(unit, ptid, fid, ltmbuf, &resume_limit));
        if (resume_limit > shared_limit) {
            write_flag = TRUE;
            fval = shared_limit;
        }
    }

    if (write_flag) {
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, ptid, fid, ltmbuf, &fval));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, ptid, sid, &pt_dyn_info, ltmbuf));
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, entry,
                 TM_EGR_THD_SERVICE_POOLt_SHARED_RESUME_LIMIT_CELLS_OPERf,
                 0, fval * TH4G_GRANULARITY));
    }

    /* Check yellow limit. */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_get(unit, ref_entry,
            TM_EGR_THD_SERVICE_POOLt_YELLOW_SHARED_LIMIT_CELLSf, 0, &lt_yellow_limit));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_get(unit, ref_entry,
            TM_EGR_THD_SERVICE_POOLt_YELLOW_SHARED_LIMIT_CELLS_OPERf, 0, &lt_yellow_limit_oper));
    ptid = MMU_THDO_SHARED_DB_POOL_YELLOW_SHARED_LIMITr;
    fid = YELLOW_SHARED_LIMITf;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));

    write_flag = FALSE;
    if (CEILING(lt_yellow_limit, TH4G_GRANULARITY) <= shared_limit &&
        lt_yellow_limit != lt_yellow_limit_oper) {
        /* Install user configured value */
        write_flag = TRUE;
        fval = CEILING(lt_yellow_limit, TH4G_GRANULARITY);
    } else {
        /* Check current operating value */
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
        SHR_IF_ERR_EXIT
            (bcmtm_field_get(unit, ptid, fid, ltmbuf, &yellow_limit));
        if (yellow_limit > shared_limit) {
            write_flag = TRUE;
            fval = shared_limit;
        }
    }

    if (write_flag) {
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, ptid, fid, ltmbuf, &fval));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, ptid, sid, &pt_dyn_info, ltmbuf));
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, entry,
                 TM_EGR_THD_SERVICE_POOLt_YELLOW_SHARED_LIMIT_CELLS_OPERf,
                 0, fval * TH4G_GRANULARITY));

        SHR_IF_ERR_EXIT
            (update_yellow_resume(unit, itm, pool, fval, entry, ref_entry));
    }

    /* Check red limit. */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_get(unit, ref_entry,
            TM_EGR_THD_SERVICE_POOLt_RED_SHARED_LIMIT_CELLSf, 0, &lt_red_limit));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_get(unit, ref_entry,
            TM_EGR_THD_SERVICE_POOLt_RED_SHARED_LIMIT_CELLS_OPERf, 0, &lt_red_limit_oper));
    ptid = MMU_THDO_SHARED_DB_POOL_RED_SHARED_LIMITr;
    fid = RED_SHARED_LIMITf;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));

    write_flag = FALSE;
    if (CEILING(lt_red_limit, TH4G_GRANULARITY) <= shared_limit &&
        lt_red_limit != lt_red_limit_oper) {
        /* Install user configured value */
        write_flag = TRUE;
        fval = CEILING(lt_red_limit, TH4G_GRANULARITY);
    } else {
        /* Check current operating value */
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
        SHR_IF_ERR_EXIT
            (bcmtm_field_get(unit, ptid, fid, ltmbuf, &red_limit));
        if (red_limit > shared_limit) {
            write_flag = TRUE;
            fval = shared_limit;
        }
    }

    if (write_flag) {
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, ptid, fid, ltmbuf, &fval));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, ptid, sid, &pt_dyn_info, ltmbuf));
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, entry,
                 TM_EGR_THD_SERVICE_POOLt_RED_SHARED_LIMIT_CELLS_OPERf,
                 0, fval * TH4G_GRANULARITY));

        SHR_IF_ERR_EXIT
            (update_red_resume(unit, itm, pool, fval, entry, ref_entry));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
tm_thd_egr_service_pool_populate_entry(int unit, int itm, int pool)
{
    bcmltd_fields_t entry;
    bcmltd_fields_t out;
    bcmltd_sid_t sid = TM_EGR_THD_SERVICE_POOLt;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_sid_t ptid;
    uint32_t shared_limit, resume_limit, yellow_limit, yellow_resume, red_limit, red_resume;
    SHR_BITDCL color_limit_bmp[1] = {0};
    uint32_t ltmbuf[BCMTM_MAX_THD_ENTRY_WSIZE] = {0};
    bool enable_color_limit;
    size_t num_fid;

    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));
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
            TM_EGR_THD_SERVICE_POOLt_BUFFER_POOLf, 0, itm));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_EGR_THD_SERVICE_POOLt_TM_EGR_SERVICE_POOL_IDf, 0, pool));

    /* Read the current limits from HW. */
    /* Per pool per ITM reg/mem. */
    BCMTM_PT_DYN_INFO(pt_dyn_info, pool, itm);

    /* Read pool shared limit. */
    ptid = MMU_THDO_SHARED_DB_POOL_SHARED_LIMITr;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, ptid, SHARED_LIMITf, ltmbuf, &shared_limit));

    /* Read pool resume limit. */
    ptid = MMU_THDO_SHARED_DB_POOL_RESUME_LIMITr;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, ptid, RESUME_LIMITf, ltmbuf, &resume_limit));
    resume_limit *= TH4G_GRANULARITY;

    /* Read pool yellow limit. */
    ptid = MMU_THDO_SHARED_DB_POOL_YELLOW_SHARED_LIMITr;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, ptid, YELLOW_SHARED_LIMITf, ltmbuf, &yellow_limit));
    yellow_limit *= TH4G_GRANULARITY;

    /* Read pool yellow resume limit. */
    ptid = MMU_THDO_SHARED_DB_POOL_YELLOW_RESUME_LIMITr;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, ptid, YELLOW_RESUME_LIMITf, ltmbuf, &yellow_resume));
    yellow_resume *= TH4G_GRANULARITY;

    /* Read pool red limit. */
    ptid = MMU_THDO_SHARED_DB_POOL_RED_SHARED_LIMITr;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, ptid, RED_SHARED_LIMITf, ltmbuf, &red_limit));
    red_limit *= TH4G_GRANULARITY;

    /* Read pool red resume limit. */
    ptid = MMU_THDO_SHARED_DB_POOL_RED_RESUME_LIMITr;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, ptid, RED_RESUME_LIMITf, ltmbuf, &red_resume));
    red_resume *= TH4G_GRANULARITY;

    /* Read per ITM color limit control bitmap. */
    ptid = MMU_THDO_SHARED_DB_POOL_CONFIGr;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, itm);
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, ptid, POOL_COLOR_LIMIT_ENABLEf, ltmbuf, color_limit_bmp));
    enable_color_limit = SHR_BITGET(color_limit_bmp, pool);

    /* Write to field list. */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_EGR_THD_SERVICE_POOLt_OPERATIONAL_STATEf, 0, VALID));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_EGR_THD_SERVICE_POOLt_RED_SHARED_RESUME_LIMIT_CELLS_OPERf,
            0, red_resume));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_EGR_THD_SERVICE_POOLt_RED_SHARED_RESUME_LIMIT_CELLSf,
            0, red_resume));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_EGR_THD_SERVICE_POOLt_RED_SHARED_LIMIT_CELLS_OPERf,
            0, red_limit));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_EGR_THD_SERVICE_POOLt_RED_SHARED_LIMIT_CELLSf, 0, red_limit));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_EGR_THD_SERVICE_POOLt_YELLOW_SHARED_RESUME_LIMIT_CELLS_OPERf,
            0, yellow_resume));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_EGR_THD_SERVICE_POOLt_YELLOW_SHARED_RESUME_LIMIT_CELLSf,
            0, yellow_resume));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_EGR_THD_SERVICE_POOLt_YELLOW_SHARED_LIMIT_CELLS_OPERf,
            0, yellow_limit));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_EGR_THD_SERVICE_POOLt_YELLOW_SHARED_LIMIT_CELLSf,
            0, yellow_limit));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_EGR_THD_SERVICE_POOLt_COLOR_SPECIFIC_LIMITSf,
            0, enable_color_limit));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_EGR_THD_SERVICE_POOLt_SHARED_RESUME_LIMIT_CELLS_OPERf,
            0, resume_limit));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_EGR_THD_SERVICE_POOLt_SHARED_RESUME_LIMIT_CELLSf,
            0, resume_limit));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_EGR_THD_SERVICE_POOLt_SHARED_LIMIT_CELLS_OPERf, 0, shared_limit););
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_EGR_THD_SERVICE_POOLt_SHARED_LIMIT_CELLSf, 0, shared_limit));

    /* Insert/update the LT. (Update is needed during flexport operation.) */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &out));
    if (SHR_SUCCESS(bcmimm_entry_lookup(unit, sid, &entry, &out))) {
        SHR_IF_ERR_EXIT
            (bcmimm_entry_update(unit, FALSE, sid, &entry));
    } else {
        SHR_IF_ERR_EXIT
            (bcmimm_entry_insert(unit, sid, &entry));
    }

exit:
    bcmtm_field_list_free(&entry);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}

static int
tm_thd_egr_service_pool_update(int unit,
                               bcmltd_sid_t sid,
                               const bcmltd_op_arg_t *op_arg,
                               int itm,
                               int pool,
                               const bcmltd_field_t *data_fields,
                               bcmltd_fields_t *output_fields)
{
    const bcmltd_field_t *data_ptr;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_sid_t ptid;
    bcmdrd_fid_t fid;
    SHR_BITDCL color_limit_bmp[1] = {0};
    uint32_t ltmbuf[BCMTM_MAX_THD_ENTRY_WSIZE] = {0};
    uint32_t shared_limit, fval, yellow_limit, red_limit;
    bool write_flag;
    long int delta;
    int rv;
    bcmltd_fid_t field_id;
    uint64_t field_data, flags = 0;
    bcmltd_fields_t in;
    bcmltd_fields_t entry;
    size_t num_fid;

    SHR_FUNC_ENTER(unit);

    if (op_arg) {
        flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    }

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_size_check(unit, output_fields, NUM_OPER_FIELDS));
    output_fields->count = 0;

    /* Get current shared and color limits. */
    /* From HW */
    ptid = MMU_THDO_SHARED_DB_POOL_SHARED_LIMITr;
    fid = SHARED_LIMITf;
    BCMTM_PT_DYN_INFO(pt_dyn_info, pool, itm);
    pt_dyn_info.flags = flags;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, ptid, fid, ltmbuf, &shared_limit));
    /* From LT */
    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&entry, 0, sizeof(bcmltd_fields_t));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, NUM_KEY_FIELDS, &in));
    in.field[0]->id   = TM_EGR_THD_SERVICE_POOLt_TM_EGR_SERVICE_POOL_IDf;
    in.field[0]->data = pool;
    in.field[1]->id   = TM_EGR_THD_SERVICE_POOLt_BUFFER_POOLf;
    in.field[1]->data = itm;
    in.count = 2;
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, sid, &num_fid));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &entry));
    SHR_IF_ERR_EXIT
        (bcmimm_entry_lookup(unit, sid, &in, &entry));

    /*
     * First iteration:
     * Shared limit and color enable is independent.
     */
    data_ptr = data_fields;
    while (data_ptr) {
        switch (data_ptr->id) {
            case TM_EGR_THD_SERVICE_POOLt_COLOR_SPECIFIC_LIMITSf:
                ptid = MMU_THDO_SHARED_DB_POOL_CONFIGr;
                /* BMP requires different sequence. */
                BCMTM_PT_DYN_INFO(pt_dyn_info, 0, itm);
                pt_dyn_info.flags = flags;
                sal_memset(ltmbuf, 0, sizeof(ltmbuf));
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
                SHR_IF_ERR_EXIT
                    (bcmtm_field_get(unit, ptid, POOL_COLOR_LIMIT_ENABLEf, ltmbuf,
                                     color_limit_bmp));
                if (data_ptr->data) {
                    SHR_BITSET(color_limit_bmp, pool);
                } else {
                    SHR_BITCLR(color_limit_bmp, pool);
                }
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, ptid, POOL_COLOR_LIMIT_ENABLEf, ltmbuf,
                                     color_limit_bmp));
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_write(unit, ptid, sid, &pt_dyn_info, ltmbuf));
                break;
            case TM_EGR_THD_SERVICE_POOLt_SHARED_LIMIT_CELLSf:
                /* Check if there is enough buffer size to allocate the new limit. */
                delta = (long int) data_ptr->data - shared_limit;
                rv = bcm56996_a0_tm_shared_buffer_update(unit, itm, pool, delta,
                                                         EGR_SERVICE_POOL_UPDATE,
                                                         FALSE);
                if (rv == SHR_E_NONE) {
                    /*
                     * Request accepted. All limits have been adjusted. Update
                     * corresponding operational field.
                     */
                    SHR_IF_ERR_EXIT
                        (bcmtm_field_list_set(unit, output_fields,
                            TM_EGR_THD_SERVICE_POOLt_SHARED_LIMIT_CELLS_OPERf,
                            0, data_ptr->data));
                    SHR_IF_ERR_EXIT
                        (bcmtm_field_list_set(unit, output_fields,
                            TM_EGR_THD_SERVICE_POOLt_OPERATIONAL_STATEf,
                            0, VALID));

                    /* Check all color limits and resume limits. */
                    SHR_IF_ERR_EXIT
                        (update_limits_by_shared_limit(unit, itm, pool,
                                                       data_ptr->data,
                                                       output_fields, &entry));
                    shared_limit = data_ptr->data;
                } else if (rv == SHR_E_PARAM) {
                    /* Request declined. Update operational state field. */
                    SHR_IF_ERR_EXIT
                        (bcmtm_field_list_set(unit, output_fields,
                            TM_EGR_THD_SERVICE_POOLt_OPERATIONAL_STATEf,
                            0, INCORRECT_SHARED_LIMIT));
                } else {
                    SHR_ERR_EXIT(rv);
                }
                break;
            default:
                break;
        }
        data_ptr = data_ptr->next;
    }

    /* Convert shared limit to 8-cell TH4G_GRANULARITY for comparison. */
    shared_limit = shared_limit / TH4G_GRANULARITY;
    BCMTM_PT_DYN_INFO(pt_dyn_info, pool, itm);
    pt_dyn_info.flags = flags;

    /*
     * Second iteration:
     * Shared resume limit and color limits depend on shared limit.
     */
    data_ptr = data_fields;
    while (data_ptr) {
        write_flag = TRUE;
        fval = data_ptr->data;
        switch (data_ptr->id) {
            case TM_EGR_THD_SERVICE_POOLt_RED_SHARED_LIMIT_CELLSf:
                ptid = MMU_THDO_SHARED_DB_POOL_RED_SHARED_LIMITr;
                fid = RED_SHARED_LIMITf;
                fval = CEILING(fval, TH4G_GRANULARITY);
                if (fval > shared_limit) {
                    /* Cannot accommodate. */
                    write_flag = FALSE;
                    field_id = TM_EGR_THD_SERVICE_POOLt_OPERATIONAL_STATEf;
                    field_data = INCORRECT_COLOR_LIMIT;
                } else {
                    field_id = TM_EGR_THD_SERVICE_POOLt_RED_SHARED_LIMIT_CELLS_OPERf;
                    field_data = fval * TH4G_GRANULARITY;
                    SHR_IF_ERR_EXIT
                        (update_red_resume(unit, itm, pool, fval, output_fields, &entry));
                }
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, output_fields, field_id, 0, field_data));
                break;
            case TM_EGR_THD_SERVICE_POOLt_YELLOW_SHARED_LIMIT_CELLSf:
                ptid = MMU_THDO_SHARED_DB_POOL_YELLOW_SHARED_LIMITr;
                fid = YELLOW_SHARED_LIMITf;
                fval = CEILING(fval, TH4G_GRANULARITY);
                if (fval > shared_limit) {
                    /* Cannot accommodate. */
                    write_flag = FALSE;
                    field_id = TM_EGR_THD_SERVICE_POOLt_OPERATIONAL_STATEf;
                    field_data = INCORRECT_COLOR_LIMIT;
                } else {
                    field_id = TM_EGR_THD_SERVICE_POOLt_YELLOW_SHARED_LIMIT_CELLS_OPERf;
                    field_data = fval * TH4G_GRANULARITY;
                    SHR_IF_ERR_EXIT
                        (update_yellow_resume(unit, itm, pool, fval, output_fields, &entry));
                }
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, output_fields, field_id, 0, field_data));
                break;
            case TM_EGR_THD_SERVICE_POOLt_SHARED_RESUME_LIMIT_CELLSf:
                ptid = MMU_THDO_SHARED_DB_POOL_RESUME_LIMITr;
                fid = RESUME_LIMITf;
                fval = CEILING(fval, TH4G_GRANULARITY);
                if (fval > shared_limit) {
                    /* Cannot accommodate. */
                    write_flag = FALSE;
                    field_id = TM_EGR_THD_SERVICE_POOLt_OPERATIONAL_STATEf;
                    field_data = INCORRECT_RESUME_LIMIT;
                } else {
                    field_id = TM_EGR_THD_SERVICE_POOLt_SHARED_RESUME_LIMIT_CELLS_OPERf;
                    field_data = fval * TH4G_GRANULARITY;
                }
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, output_fields, field_id, 0, field_data));
                break;
            default:
                write_flag = FALSE;
                break;
        }
        if (write_flag) {
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

    /*
     * Third iteration:
     * Color resume limits depend on corresponding color limits.
     */
    data_ptr = data_fields;
    while (data_ptr) {
        write_flag = TRUE;
        fval = data_ptr->data;
        switch (data_ptr->id) {
            case TM_EGR_THD_SERVICE_POOLt_RED_SHARED_RESUME_LIMIT_CELLSf:
                /* Red HW. */
                ptid = MMU_THDO_SHARED_DB_POOL_RED_SHARED_LIMITr;
                fid = RED_SHARED_LIMITf;
                sal_memset(ltmbuf, 0, sizeof(ltmbuf));
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
                SHR_IF_ERR_EXIT
                    (bcmtm_field_get(unit, ptid, fid, ltmbuf, &red_limit));

                ptid = MMU_THDO_SHARED_DB_POOL_RED_RESUME_LIMITr;
                fid = RED_RESUME_LIMITf;
                fval = CEILING(fval, TH4G_GRANULARITY);
                if (fval > red_limit) {
                    /* Cannot accommodate. */
                    write_flag = FALSE;
                    field_id = TM_EGR_THD_SERVICE_POOLt_OPERATIONAL_STATEf;
                    field_data = INCORRECT_RESUME_LIMIT;
                } else {
                    field_id =
                        TM_EGR_THD_SERVICE_POOLt_RED_SHARED_RESUME_LIMIT_CELLS_OPERf;
                    field_data = fval * TH4G_GRANULARITY;
                }
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, output_fields, field_id, 0, field_data));
                break;
            case TM_EGR_THD_SERVICE_POOLt_YELLOW_SHARED_RESUME_LIMIT_CELLSf:
                /* Red HW. */
                ptid = MMU_THDO_SHARED_DB_POOL_YELLOW_SHARED_LIMITr;
                fid = YELLOW_SHARED_LIMITf;
                sal_memset(ltmbuf, 0, sizeof(ltmbuf));
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
                SHR_IF_ERR_EXIT
                    (bcmtm_field_get(unit, ptid, fid, ltmbuf, &yellow_limit));

                ptid = MMU_THDO_SHARED_DB_POOL_YELLOW_RESUME_LIMITr;
                fid = YELLOW_RESUME_LIMITf;
                fval = CEILING(fval, TH4G_GRANULARITY);
                if (fval > yellow_limit) {
                    /* Cannot accommodate. */
                    write_flag = FALSE;
                    field_id = TM_EGR_THD_SERVICE_POOLt_OPERATIONAL_STATEf;
                    field_data = INCORRECT_RESUME_LIMIT;
                } else {
                    field_id =
                        TM_EGR_THD_SERVICE_POOLt_YELLOW_SHARED_RESUME_LIMIT_CELLS_OPERf;
                    field_data = fval * TH4G_GRANULARITY;
                }
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, output_fields, field_id, 0, field_data));
                break;
            default:
                write_flag = FALSE;
                break;
        }
        if (write_flag) {
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
    SHR_FUNC_EXIT();
}

static int
tm_thd_egr_service_pool_delete(int unit, bcmltd_sid_t sid,
                               const bcmltd_op_arg_t *op_arg,
                               int itm, int pool)
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
        (tm_thd_egr_service_pool_update(unit, sid, op_arg, itm, pool, data_fields,
                                        &dummy_fields));

exit:
    SHR_FREE(data_fields);
    bcmtm_field_list_free(&dummy_fields);
    SHR_FUNC_EXIT();
}

static int
tm_thd_egr_service_pool_stage(int unit,
                              bcmltd_sid_t sid,
                              const bcmltd_op_arg_t *op_arg,
                              bcmimm_entry_event_t event_reason,
                              const bcmltd_field_t *key_fields,
                              const bcmltd_field_t *data_fields,
                              void *context,
                              bcmltd_fields_t *output_fields)
{
    const bcmltd_field_t *key_ptr;
    int itm = -1;
    int pool = -1;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);

    key_ptr = key_fields;
    while (key_ptr) {
        switch (key_ptr->id) {
            case TM_EGR_THD_SERVICE_POOLt_BUFFER_POOLf:
                itm = key_ptr->data;
                break;
            case TM_EGR_THD_SERVICE_POOLt_TM_EGR_SERVICE_POOL_IDf:
                pool = key_ptr->data;
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        key_ptr = key_ptr->next;
    }

    if (itm < 0 || pool < 0) {
        return SHR_E_PARAM;
    }

    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
            /* Insert shares the same callback with update. */
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_EXIT
                (tm_thd_egr_service_pool_update(unit, sid, op_arg, itm, pool,
                                                data_fields, output_fields));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT
                (tm_thd_egr_service_pool_delete(unit, sid, op_arg, itm, pool));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
tm_thd_egr_service_pool_lookup(int unit,
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
    int itm, pool;
    uint32_t ltmbuf[BCMTM_MAX_THD_ENTRY_WSIZE] = {0};
    SHR_BITDCL color_limit_bmp[1] = {0};

    SHR_FUNC_ENTER(unit);

    if (!(op_arg->attrib & BCMLT_ENT_ATTR_GET_FROM_HW)) {
        SHR_EXIT();
    }
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    key = (lkup_type == BCMIMM_TRAVERSE) ? out : in;
    fid = TM_EGR_THD_SERVICE_POOLt_BUFFER_POOLf;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_imm_fields_get(unit, key, fid, &idx));
    itm= idx;
    fid = TM_EGR_THD_MC_PORT_SERVICE_POOLt_TM_EGR_SERVICE_POOL_IDf;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_imm_fields_get(unit, key, fid, &idx));
    pool = idx;

    ptid = MMU_THDO_SHARED_DB_POOL_CONFIGr;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, itm);
    pt_dyn_info.flags = req_flags;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));

    SHR_IF_ERR_EXIT(
        bcmtm_field_get(unit, ptid, POOL_COLOR_LIMIT_ENABLEf, ltmbuf, color_limit_bmp));
    SHR_IF_ERR_EXIT(
        bcmcth_imm_field_update(unit, out,
                                TM_EGR_THD_SERVICE_POOLt_COLOR_SPECIFIC_LIMITSf,
                                SHR_BITGET(color_limit_bmp, pool) ? 1 : 0));

    ptid = MMU_THDI_BUFFER_CELL_LIMIT_SPr;
    BCMTM_PT_DYN_INFO(pt_dyn_info, pool, itm);
    pt_dyn_info.flags = req_flags;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT(
        bcmtm_field_get(unit, ptid, LIMITf, ltmbuf, &fval));
    SHR_IF_ERR_EXIT(
        bcmcth_imm_field_update(unit, out,
                                TM_EGR_THD_SERVICE_POOLt_SHARED_LIMIT_CELLSf,
                                fval));

    ptid = MMU_THDO_SHARED_DB_POOL_RED_SHARED_LIMITr;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT(
        bcmtm_field_get(unit, ptid, RED_SHARED_LIMITf, ltmbuf, &fval));
    fval = fval * TH4G_GRANULARITY;
    SHR_IF_ERR_EXIT(
        bcmcth_imm_field_update(unit, out,
                                TM_EGR_THD_SERVICE_POOLt_RED_SHARED_LIMIT_CELLSf,
                                fval));

    ptid = MMU_THDO_SHARED_DB_POOL_YELLOW_SHARED_LIMITr;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT(
        bcmtm_field_get(unit, ptid, YELLOW_SHARED_LIMITf, ltmbuf, &fval));
    fval = fval * TH4G_GRANULARITY;
    SHR_IF_ERR_EXIT(
        bcmcth_imm_field_update(unit, out,
                                TM_EGR_THD_SERVICE_POOLt_YELLOW_SHARED_LIMIT_CELLSf,
                                fval));

    ptid = MMU_THDO_SHARED_DB_POOL_RESUME_LIMITr;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT(
        bcmtm_field_get(unit, ptid, RESUME_LIMITf, ltmbuf, &fval));
    fval = fval * TH4G_GRANULARITY;
    SHR_IF_ERR_EXIT(
        bcmcth_imm_field_update(unit, out,
                                TM_EGR_THD_SERVICE_POOLt_SHARED_RESUME_LIMIT_CELLSf,
                                fval));

    ptid = MMU_THDO_SHARED_DB_POOL_RED_RESUME_LIMITr;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT(
        bcmtm_field_get(unit, ptid, RED_RESUME_LIMITf, ltmbuf, &fval));
    fval = fval * TH4G_GRANULARITY;
    SHR_IF_ERR_EXIT(
        bcmcth_imm_field_update(unit, out,
                                TM_EGR_THD_SERVICE_POOLt_RED_SHARED_RESUME_LIMIT_CELLSf,
                                fval));

    ptid = MMU_THDO_SHARED_DB_POOL_YELLOW_RESUME_LIMITr;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT(
        bcmtm_field_get(unit, ptid, YELLOW_RESUME_LIMITf, ltmbuf, &fval));
    fval = fval * TH4G_GRANULARITY;
    SHR_IF_ERR_EXIT(
        bcmcth_imm_field_update(unit, out,
                                TM_EGR_THD_SERVICE_POOLt_YELLOW_SHARED_RESUME_LIMIT_CELLSf,
                                fval));

exit:
    SHR_FUNC_EXIT();
}
/*******************************************************************************
 * Public Functions
 */

int
bcm56996_a0_tm_thd_egr_service_pool_register(int unit)
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
    tm_thd_cb_handler.op_stage = tm_thd_egr_service_pool_stage;
    tm_thd_cb_handler.op_lookup = tm_thd_egr_service_pool_lookup;

    rv = bcmlrd_map_get(unit, TM_EGR_THD_SERVICE_POOLt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }
    /* Register callback handler for the LT. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_EGR_THD_SERVICE_POOLt, &tm_thd_cb_handler, NULL));

exit:
    SHR_FUNC_EXIT();
}

int
bcm56996_a0_tm_thd_egr_service_pool_populate(int unit)
{
    int pool, itm, rv;
    const bcmlrd_map_t *map = NULL;

    SHR_FUNC_ENTER(unit);
    rv = bcmlrd_map_get(unit, TM_EGR_THD_SERVICE_POOLt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }
    for (itm = 0; itm < SOC_MMU_CFG_ITM_MAX; itm++) {
        if (bcm56996_a0_tm_check_valid_itm(unit, itm) != SHR_E_NONE) {
            continue;
        }
        for (pool = 0; pool < SOC_MMU_CFG_SERVICE_POOL_MAX; pool++) {
            SHR_IF_ERR_EXIT
                (tm_thd_egr_service_pool_populate_entry(unit, itm, pool));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm56996_a0_tm_egr_pool_shared_limit_lt_update(int unit, int itm, int pool,
                                               uint32_t fval)
{
    bcmltd_fields_t in;
    bcmltd_fields_t out;
    bcmltd_sid_t sid = TM_EGR_THD_SERVICE_POOLt;
    uint64_t cur_limit;
    size_t num_fid;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    /* Allocate in field list. */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, NUM_KEY_FIELDS + NUM_OPER_FIELDS, &in));
    in.field[0]->id   = TM_EGR_THD_SERVICE_POOLt_TM_EGR_SERVICE_POOL_IDf;
    in.field[0]->data = pool;
    in.field[1]->id   = TM_EGR_THD_SERVICE_POOLt_BUFFER_POOLf;
    in.field[1]->data = itm;
    in.field[2]->id   = TM_EGR_THD_SERVICE_POOLt_SHARED_LIMIT_CELLS_OPERf;
    in.field[2]->data = fval;
    in.count = 3;

    /* Allocate out field list. */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, sid, &num_fid));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &out));

    /* Check if entry exist before update. */
    if (SHR_SUCCESS(bcmimm_entry_lookup(unit, sid, &in, &out))) {
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_get(unit, &out,
                TM_EGR_THD_SERVICE_POOLt_SHARED_LIMIT_CELLS_OPERf, 0, &cur_limit));
        if (fval == cur_limit) {
            SHR_EXIT();
        }

        /* Update. (bcmimm_entry_update will call notify API automatically.) */
        SHR_IF_ERR_EXIT
            (update_limits_by_shared_limit(unit, itm, pool, fval, &in, &out));
        SHR_IF_ERR_EXIT
            (bcmimm_entry_update(unit, TRUE, sid, &in));
    }

exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}
