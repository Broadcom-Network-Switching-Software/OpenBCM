/*! \file bcm56880_a0_tm_ing_port.c
 *
 * IMM handlers for threshold LTs.
 *
 * This file contains callbcak handlers for LT TM_ING_PORT.
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
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmtm/chip/bcm56880_a0_tm_thd_internal.h>

#include "bcm56880_a0_tm_imm_internal.h"

/* TM_PORT_ENTRY_STATE_T */
#define VALID                   0
#define PORT_INFO_UNAVAILABLE   1

#define NUM_KEY_FIELDS          1
#define NUM_OPER_FIELDS         1

#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM_CHIP

static int
tm_ing_port_populate_entry(int unit, int lport, int up, soc_mmu_cfg_buf_t *buf)
{
    bcmltd_fields_t entry;
    bcmltd_sid_t sid = TM_ING_PORTt;
    size_t num_fid;
    int fc;
    soc_mmu_cfg_buf_port_t *buf_port;
    bcmtm_drv_info_t *drv_info;

    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(bcmltd_fields_t));
    buf_port = &buf->ports[lport];
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
                              TM_ING_PORTt_PORT_IDf, 0, lport));

    /* Write to field list. */
    /* Populate user-configured fields during init. */
    fc = (drv_info->lossless == 0) ? 0: 1;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_ING_PORTt_PAUSEf, 0, fc));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_ING_PORTt_ING_PRI_MAP_IDf, 0,
            (buf_port->pri_to_prigroup_profile_idx)));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_ING_PORTt_PRI_GRP_MAP_IDf, 0,
            (buf_port->prigroup_profile_idx)));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_ING_PORTt_OPERATIONAL_STATEf, 0, PORT_INFO_UNAVAILABLE));

    SHR_IF_ERR_EXIT
        (bcmimm_entry_insert(unit, sid, &entry));

exit:
    bcmtm_field_list_free(&entry);
    SHR_FUNC_EXIT();
}

static int
tm_ing_port_update(int unit,
                   bcmltd_sid_t sid,
                   int lport,
                   const bcmltd_field_t *data_fields,
                   bcmltd_fields_t *output_fields)
{
    const bcmltd_field_t *data_ptr;
    int pport, mmu_port;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_sid_t ptid;
    uint32_t fval;
    uint32_t ltmbuf[BCMTM_MAX_THD_ENTRY_WSIZE] = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_size_check(unit, output_fields, NUM_OPER_FIELDS));
    output_fields->count = 0;

    /* Check for invalid port (port is not added). */
    if (SHR_FAILURE(bcmtm_lport_pport_get(unit, lport, &pport))) {
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, output_fields,
                             TM_ING_PORTt_OPERATIONAL_STATEf,
                             0, PORT_INFO_UNAVAILABLE));
        SHR_EXIT();
    }

    ptid = MMU_THDI_ING_PORT_CONFIGr;
    SHR_IF_ERR_EXIT
        (bcmtm_lport_mport_get(unit, lport, &mmu_port));
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, mmu_port);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));

    data_ptr = data_fields;
    while (data_ptr) {
        fval = data_ptr->data;
        switch (data_ptr->id) {
            case TM_ING_PORTt_ING_PRI_MAP_IDf:
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, ptid, INPPRI_PROFILE_SELf, ltmbuf, &fval));
                break;
            case TM_ING_PORTt_PRI_GRP_MAP_IDf:
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, ptid, PG_PROFILE_SELf, ltmbuf, &fval));
                break;
            case TM_ING_PORTt_PAUSEf:
               SHR_IF_ERR_EXIT
                   (bcmtm_field_set(unit, ptid, PAUSE_ENABLEf, ltmbuf, &fval));
                break;
            default:
                break;
        }
        data_ptr = data_ptr->next;
    }

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, output_fields,
                         TM_ING_PORTt_OPERATIONAL_STATEf,
                         0, VALID));
exit:
    SHR_FUNC_EXIT();
}


static int
tm_ing_port_stage(int unit,
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

    SHR_FUNC_ENTER(unit);

    key_ptr = key_fields;
    while (key_ptr) {
        switch (key_ptr->id) {
            case TM_ING_PORTt_PORT_IDf:
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
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_EXIT
                (tm_ing_port_update(unit, sid, lport, data_fields, output_fields));
            break;
        default:
            /* LT has update_only attribut. */
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */

int
bcm56880_a0_tm_ing_port_register(int unit)
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
    tm_thd_cb_handler.stage    = tm_ing_port_stage;

    rv = bcmlrd_map_get(unit, TM_ING_PORTt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    /* Register callback handler for the LT. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_ING_PORTt, &tm_thd_cb_handler, NULL));

exit:
    SHR_FUNC_EXIT();
}

int
bcm56880_a0_tm_ing_port_populate(int unit)
{
    int lport, rv;
    const bcmlrd_map_t *map = NULL;
    bcmtm_drv_info_t *drv_info;
    soc_mmu_cfg_buf_t *buf;
    soc_mmu_device_info_t devcfg;

    SHR_FUNC_ENTER(unit);

    buf = soc_mmu_buf_cfg_alloc(unit);
    if (!buf) {
        return SHR_E_MEMORY;
    }
    bcmtm_drv_info_get(unit, &drv_info);
    td4_mmu_init_dev_config(unit, &devcfg, drv_info->lossless);
    rv = bcmlrd_map_get(unit, TM_ING_PORTt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    for (lport = 0; lport < SOC_MMU_CFG_PORT_MAX; lport++) {
        td4_mmu_config_thdi_set(unit, lport, buf, &devcfg, drv_info->lossless);
        SHR_IF_ERR_EXIT
            (tm_ing_port_populate_entry(unit, lport, 1, buf));
    }

exit:
    sal_free(buf);
    SHR_FUNC_EXIT();
}

int
bcm56880_a0_tm_ing_port_update(int unit, int pport, int up)
{
    int lport, rv;
    const bcmlrd_map_t *map = NULL;
    bcmltd_fields_t lkup_fields, keys, out_fields;
    size_t num_fid;
    bcmltd_sid_t sid = TM_ING_PORTt;
    bcmltd_field_t *data_fields = NULL;
    int j;

    SHR_FUNC_ENTER(unit);

    sal_memset(&lkup_fields, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out_fields, 0, sizeof(bcmltd_fields_t));
    sal_memset(&keys, 0, sizeof(bcmltd_fields_t));
    rv = bcmlrd_map_get(unit, TM_ING_PORTt, &map);
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
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &out_fields));
    keys.field[0]->id   = TM_ING_PORTt_PORT_IDf;
    keys.field[0]->data = lport;

    if (SHR_SUCCESS(bcmimm_entry_lookup(unit, sid, &keys, &lkup_fields))) {
        for (j = 0; j < (int)lkup_fields.count; j++) {
           data_fields[j] = *(lkup_fields).field[j];
        }

        SHR_IF_ERR_EXIT
            (tm_ing_port_update(unit, sid, lport, data_fields, &out_fields));
        if (up == 0) {
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, &out_fields,
                    TM_ING_PORTt_OPERATIONAL_STATEf, 0, PORT_INFO_UNAVAILABLE));
        }
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, &out_fields,
                              TM_ING_PORTt_PORT_IDf, 0, lport));
        SHR_IF_ERR_EXIT
            (bcmimm_entry_update(unit, FALSE, sid, &out_fields));

    }

exit:
    SHR_FREE(data_fields);
    bcmtm_field_list_free(&lkup_fields);
    bcmtm_field_list_free(&keys);
    bcmtm_field_list_free(&out_fields);
    SHR_FUNC_EXIT();
}
