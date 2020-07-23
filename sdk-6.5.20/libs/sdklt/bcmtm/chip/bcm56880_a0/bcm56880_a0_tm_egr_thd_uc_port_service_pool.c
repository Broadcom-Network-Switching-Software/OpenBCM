/*! \file bcm56880_a0_tm_egr_thd_uc_port_service_pool.c
 *
 * IMM handlers for threshold LTs.
 *
 * This file contains callbcak handlers for LT TM_EGR_THD_UC_PORT_SERVICE_POOL.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/chip/bcm56880_a0_enum.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmtm/chip/bcm56880_a0_tm_thd_internal.h>
#include <bcmdrd/chip/bcm56880_a0_defs.h>

#include "bcm56880_a0_tm_imm_internal.h"

/* TM_PORT_ENTRY_STATE_T */
#define VALID                   0
#define PORT_INFO_UNAVAILABLE   1

#define NUM_KEY_FIELDS          2
#define NUM_OPER_FIELDS         6

#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM_CHIP

static int
tm_thd_egr_uc_port_service_pool_populate_entry(int unit, int lport, int pool, int up)
{
    bcmltd_fields_t entry;
    bcmltd_sid_t sid = TM_EGR_THD_UC_PORT_SERVICE_POOLt;
    bcmltd_fid_t fid;
    size_t num_fid;
    uint32_t flags;
    uint32_t fval;

    SHR_FUNC_ENTER(unit);

    flags = bcmdrd_dev_flags_get(unit);
    if ((flags & CHIP_FLAG_BW8T) == CHIP_FLAG_BW8T) {
        /* BCM56883 */
        fval = BCM56883_A0_MMU_TOTAL_CELLS_PER_ITM;
    } else {
        /* BCM56880 */
        fval =  BCM56880_A0_MMU_TOTAL_CELLS_PER_ITM;
    }

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

    fid = TM_EGR_THD_UC_PORT_SERVICE_POOLt_RED_SHARED_RESUME_LIMIT_CELLSf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry, fid, 0, fval / TD4_GRANULARITY * TD4_GRANULARITY));

    fid = TM_EGR_THD_UC_PORT_SERVICE_POOLt_RED_SHARED_LIMIT_CELLSf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry, fid, 0, fval / TD4_GRANULARITY * TD4_GRANULARITY));

    fid = TM_EGR_THD_UC_PORT_SERVICE_POOLt_YELLOW_SHARED_RESUME_LIMIT_CELLSf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry, fid, 0, fval / TD4_GRANULARITY * TD4_GRANULARITY));

    fid = TM_EGR_THD_UC_PORT_SERVICE_POOLt_YELLOW_SHARED_LIMIT_CELLSf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry, fid, 0, fval / TD4_GRANULARITY * TD4_GRANULARITY));

    fid = TM_EGR_THD_UC_PORT_SERVICE_POOLt_SHARED_RESUME_LIMIT_CELLSf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry, fid, 0, fval / TD4_GRANULARITY * TD4_GRANULARITY));

    fid = TM_EGR_THD_UC_PORT_SERVICE_POOLt_SHARED_LIMIT_CELLSf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry, fid, 0, fval));

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
                         TM_EGR_THD_UC_PORT_SERVICE_POOLt_PORT_IDf,
                         0, lport));

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
                         TM_EGR_THD_UC_PORT_SERVICE_POOLt_TM_EGR_SERVICE_POOL_IDf,
                         0, pool));

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
                              TM_EGR_THD_UC_PORT_SERVICE_POOLt_OPERATIONAL_STATEf,
                              0, PORT_INFO_UNAVAILABLE));
    SHR_IF_ERR_EXIT
        (bcmimm_entry_insert(unit, sid, &entry));

exit:
    bcmtm_field_list_free(&entry);
    SHR_FUNC_EXIT();
}

static int
tm_thd_egr_uc_port_service_pool_update(int unit,
                                       bcmltd_sid_t sid,
                                       int lport,
                                       int pool,
                                       const bcmltd_field_t *data_fields,
                                       bcmltd_fields_t *output_fields)
{
    const bcmltd_field_t *data_ptr;
    int pport, midx;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_sid_t ptid;
    bcmdrd_fid_t fid;
    uint32_t fval;
    uint32_t ltmbuf[BCMTM_MAX_THD_ENTRY_WSIZE] = {0};
    bool write_flag;

    bcmdrd_fid_t field_sp_color_en[SOC_MMU_CFG_SERVICE_POOL_MAX] = {
            SP0_COLOR_LIMIT_ENABLEf, SP1_COLOR_LIMIT_ENABLEf,
            SP2_COLOR_LIMIT_ENABLEf, SP3_COLOR_LIMIT_ENABLEf};
    bcmdrd_fid_t field_sp_shared[SOC_MMU_CFG_SERVICE_POOL_MAX] = {
            SP0_SHARED_LIMITf, SP1_SHARED_LIMITf,
            SP2_SHARED_LIMITf, SP3_SHARED_LIMITf};
    bcmdrd_fid_t field_sp_yellow[SOC_MMU_CFG_SERVICE_POOL_MAX] = {
            SP0_YELLOW_LIMITf, SP1_YELLOW_LIMITf,
            SP2_YELLOW_LIMITf, SP3_YELLOW_LIMITf};
    bcmdrd_fid_t field_sp_red[SOC_MMU_CFG_SERVICE_POOL_MAX] = {
            SP0_RED_LIMITf, SP1_RED_LIMITf,
            SP2_RED_LIMITf, SP3_RED_LIMITf};
    bcmdrd_fid_t field_bst_shared_profile[SOC_MMU_CFG_SERVICE_POOL_MAX] = {
            BST_PORTSP0_WM_SHARED_THRESHOLD_SELf,
            BST_PORTSP1_WM_SHARED_THRESHOLD_SELf,
            BST_PORTSP2_WM_SHARED_THRESHOLD_SELf,
            BST_PORTSP3_WM_SHARED_THRESHOLD_SELf};
    bcmdrd_fid_t field_ebst_profile[SOC_MMU_CFG_SERVICE_POOL_MAX] = {
            BST_PORTSP0_EBST_STARTSTOP_SHARED_THRESHOLD_SELf,
            BST_PORTSP0_EBST_STARTSTOP_SHARED_THRESHOLD_SELf,
            BST_PORTSP0_EBST_STARTSTOP_SHARED_THRESHOLD_SELf,
            BST_PORTSP0_EBST_STARTSTOP_SHARED_THRESHOLD_SELf};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_size_check(unit, output_fields, NUM_OPER_FIELDS));
    output_fields->count = 0;

    /* Check for invalid port (port is not added). */
    if (SHR_FAILURE(bcmtm_lport_pport_get(unit, lport, &pport))) {
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, output_fields,
                             TM_EGR_THD_UC_PORT_SERVICE_POOLt_OPERATIONAL_STATEf,
                             0, PORT_INFO_UNAVAILABLE));
        SHR_EXIT();
    }

    /* Limit configurations. */
    ptid = MMU_THDO_CONFIG_PORT_UC0m;
    midx = td4_piped_mem_index(unit, lport, ptid, 0);
    BCMTM_PT_DYN_INFO(pt_dyn_info, midx, 0);
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));

    data_ptr = data_fields;
    while (data_ptr) {
        write_flag = TRUE;
        fval = data_ptr->data;
        switch (data_ptr->id) {
            case TM_EGR_THD_UC_PORT_SERVICE_POOLt_SHARED_LIMIT_CELLSf:
                fid = field_sp_shared[pool];
                break;
            case TM_EGR_THD_UC_PORT_SERVICE_POOLt_COLOR_SPECIFIC_LIMITSf:
                fid = field_sp_color_en[pool];
                break;
            case TM_EGR_THD_UC_PORT_SERVICE_POOLt_YELLOW_SHARED_LIMIT_CELLSf:
                fid = field_sp_yellow[pool];
                fval = CEILING(fval, TD4_GRANULARITY);
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set
                        (unit, output_fields,
                        TM_EGR_THD_UC_PORT_SERVICE_POOLt_YELLOW_SHARED_LIMIT_CELLS_OPERf,
                        0, fval * TD4_GRANULARITY));
                break;
            case TM_EGR_THD_UC_PORT_SERVICE_POOLt_RED_SHARED_LIMIT_CELLSf:
                fid = field_sp_red[pool];
                fval = CEILING(fval, TD4_GRANULARITY);
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set
                        (unit, output_fields,
                        TM_EGR_THD_UC_PORT_SERVICE_POOLt_RED_SHARED_LIMIT_CELLS_OPERf,
                        0, fval * TD4_GRANULARITY));
                break;
            case TM_EGR_THD_UC_PORT_SERVICE_POOLt_TM_EGR_BST_THD_UC_PORT_SERVICE_POOL_PROFILE_IDf:
                fid = field_bst_shared_profile[pool];
                break;
            case TM_EGR_THD_UC_PORT_SERVICE_POOLt_TM_EBST_PROFILE_IDf:
                fid = field_ebst_profile[pool];
                break;
            default:
                write_flag = FALSE;
                break;
        }
        if (write_flag) {
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, ptid, fid, ltmbuf, &fval));

        }
        data_ptr = data_ptr->next;
    }
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, ptid, sid, &pt_dyn_info, ltmbuf));

    /* Resume limit configurations. */
    ptid = MMU_THDO_RESUME_PORT_UC0m;
    midx = td4_piped_mem_index(unit, lport, ptid, pool);
    BCMTM_PT_DYN_INFO(pt_dyn_info, midx, 0);
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));

    data_ptr = data_fields;
    while (data_ptr) {
        write_flag = TRUE;
        fval = data_ptr->data;
        switch (data_ptr->id) {
            case TM_EGR_THD_UC_PORT_SERVICE_POOLt_SHARED_RESUME_LIMIT_CELLSf:
                fid = SHARED_RESUMEf;
                fval = CEILING(fval, TD4_GRANULARITY);
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set
                        (unit, output_fields,
                        TM_EGR_THD_UC_PORT_SERVICE_POOLt_SHARED_RESUME_LIMIT_CELLS_OPERf,
                        0, fval * TD4_GRANULARITY));
                break;
            case TM_EGR_THD_UC_PORT_SERVICE_POOLt_YELLOW_SHARED_RESUME_LIMIT_CELLSf:
                fid = YELLOW_RESUMEf;
                fval = CEILING(fval, TD4_GRANULARITY);
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set
                        (unit, output_fields,
                        TM_EGR_THD_UC_PORT_SERVICE_POOLt_YELLOW_SHARED_RESUME_LIMIT_CELLS_OPERf,
                        0, fval * TD4_GRANULARITY));
                break;
            case TM_EGR_THD_UC_PORT_SERVICE_POOLt_RED_SHARED_RESUME_LIMIT_CELLSf:
                fid = RED_RESUMEf;
                fval = CEILING(fval, TD4_GRANULARITY);
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set
                        (unit, output_fields,
                        TM_EGR_THD_UC_PORT_SERVICE_POOLt_RED_SHARED_RESUME_LIMIT_CELLS_OPERf,
                        0, fval * TD4_GRANULARITY));
                break;
            default:
                write_flag = FALSE;
                break;
        }
        if (write_flag) {
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, ptid, fid, ltmbuf, &fval));

        }
        data_ptr = data_ptr->next;
    }
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, output_fields,
                         TM_EGR_THD_UC_PORT_SERVICE_POOLt_OPERATIONAL_STATEf,
                         0, VALID));
exit:
    SHR_FUNC_EXIT();
}

static int
tm_thd_egr_uc_port_service_pool_delete(int unit, int sid, int lport, int pool)
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
        (tm_thd_egr_uc_port_service_pool_update(unit, sid, lport, pool,
                                                data_fields, &dummy_fields));

exit:
    SHR_FREE(data_fields);
    bcmtm_field_list_free(&dummy_fields);
    SHR_FUNC_EXIT();
}

static int
tm_thd_egr_uc_port_service_pool_stage(int unit,
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
    int pool = -1;

    SHR_FUNC_ENTER(unit);

    key_ptr = key_fields;
    while (key_ptr) {
        switch (key_ptr->id) {
            case TM_EGR_THD_UC_PORT_SERVICE_POOLt_PORT_IDf:
                lport = key_ptr->data;
                break;
            case TM_EGR_THD_UC_PORT_SERVICE_POOLt_TM_EGR_SERVICE_POOL_IDf:
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
                (tm_thd_egr_uc_port_service_pool_update(unit, sid, lport, pool,
                                                        data_fields, output_fields));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT
                (tm_thd_egr_uc_port_service_pool_delete(unit, sid, lport, pool));
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
bcm56880_a0_tm_thd_egr_uc_port_service_pool_register(int unit)
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
    tm_thd_cb_handler.stage    = tm_thd_egr_uc_port_service_pool_stage;

    rv = bcmlrd_map_get(unit, TM_EGR_THD_UC_PORT_SERVICE_POOLt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL)|| (!map)) {
        SHR_EXIT();
    }
    /* Register callback handler for the LT. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_EGR_THD_UC_PORT_SERVICE_POOLt,
                             &tm_thd_cb_handler, NULL));

exit:
    SHR_FUNC_EXIT();
}

int
bcm56880_a0_tm_thd_egr_uc_port_service_pool_populate(int unit)
{
    int lport, pool, rv;
    const bcmlrd_map_t *map = NULL;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, TM_EGR_THD_UC_PORT_SERVICE_POOLt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL)|| (!map)) {
        SHR_EXIT();
    }
    for (lport = 1; lport < SOC_MMU_CFG_PORT_MAX; lport++) {
        for (pool = 0; pool < SOC_MMU_CFG_SERVICE_POOL_MAX; pool++) {
            SHR_IF_ERR_EXIT
                (tm_thd_egr_uc_port_service_pool_populate_entry(unit, lport, pool, 1));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm56880_a0_tm_thd_egr_uc_port_service_pool_update(int unit, int pport, int up)
{
    int lport, pool, rv;
    const bcmlrd_map_t *map = NULL;
    bcmltd_fields_t lkup_fields, keys, out_fields;
    size_t num_fid;
    bcmltd_sid_t sid = TM_EGR_THD_UC_PORT_SERVICE_POOLt;
    bcmltd_field_t *data_fields = NULL;
    int j;

    SHR_FUNC_ENTER(unit);

    sal_memset(&lkup_fields, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out_fields, 0, sizeof(bcmltd_fields_t));
    sal_memset(&keys, 0, sizeof(bcmltd_fields_t));
    rv = bcmlrd_map_get(unit, TM_EGR_THD_UC_PORT_SERVICE_POOLt, &map);
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
    if (!bcmtm_lport_is_cpu(unit, lport)) {
        /* Table not valid for CPU port */
        for (pool = 0; pool < SOC_MMU_CFG_SERVICE_POOL_MAX; pool++) {
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_alloc(unit, num_fid, &out_fields));
            keys.field[0]->id   = TM_EGR_THD_UC_PORT_SERVICE_POOLt_PORT_IDf;
            keys.field[0]->data = lport;
            keys.field[1]->id   = TM_EGR_THD_UC_PORT_SERVICE_POOLt_TM_EGR_SERVICE_POOL_IDf;
            keys.field[1]->data = pool;

            if (SHR_SUCCESS(bcmimm_entry_lookup(unit, sid, &keys, &lkup_fields))) {
                for (j = 0; j < (int)lkup_fields.count; j++) {
                   data_fields[j] = *(lkup_fields).field[j];
                }

                SHR_IF_ERR_EXIT
                    (tm_thd_egr_uc_port_service_pool_update(unit, sid, lport,
                                         pool, data_fields, &out_fields));
                if (up == 0) {
                    SHR_IF_ERR_EXIT
                        (bcmtm_field_list_set(unit, &out_fields,
                             TM_EGR_THD_UC_PORT_SERVICE_POOLt_OPERATIONAL_STATEf,
                             0, PORT_INFO_UNAVAILABLE));
                }
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, &out_fields,
                                     TM_EGR_THD_UC_PORT_SERVICE_POOLt_PORT_IDf,
                                     0, lport));
                SHR_IF_ERR_EXIT
                    (bcmtm_field_list_set(unit, &out_fields,
                                     TM_EGR_THD_UC_PORT_SERVICE_POOLt_TM_EGR_SERVICE_POOL_IDf,
                                     0, pool));
                SHR_IF_ERR_EXIT
                    (bcmimm_entry_update(unit, FALSE, sid, &out_fields));
            }
            bcmtm_field_list_free(&out_fields);

        }
    }

exit:
    SHR_FREE(data_fields);
    bcmtm_field_list_free(&lkup_fields);
    bcmtm_field_list_free(&keys);
    SHR_FUNC_EXIT();
}
