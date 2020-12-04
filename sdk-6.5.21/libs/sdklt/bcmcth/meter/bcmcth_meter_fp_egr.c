/*! \file bcmcth_meter_fp_egr.c
 *
 * This file contains EFP Meter custom handler functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <sal/sal_time.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmissu/issu_api.h>
#include <bcmcth/bcmcth_meter_sysm.h>

#include <bcmcth/bcmcth_meter_fp_egr.h>

#define BSL_LOG_MODULE  BSL_LS_BCMCTH_METER

/*******************************************************************************
 * Local definitions
 */

/*! Device specific attach function type for METER_EGR_FP_TEMPLATE. */
typedef bcmcth_meter_fp_drv_t *(*bcmcth_meter_fp_egr_drv_get_f)(int unit);

/*! Array of device specific attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
{ _bc##_cth_meter_fp_egr_drv_get },
static struct {
    bcmcth_meter_fp_egr_drv_get_f drv_get;
} meter_fp_egr_drv_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

/*! EFP Meter driver. */
static bcmcth_meter_fp_drv_t *meter_fp_egr_drv[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Public functions
 */

int
bcmcth_meter_fp_egr_drv_init(int unit)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    /* Perform device-specific software setup */
    meter_fp_egr_drv[unit] = meter_fp_egr_drv_get[dev_type].drv_get(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_fp_egr_drv_cleanup(int unit)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    meter_fp_egr_drv[unit] = NULL;

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_fp_egr_init(int unit, bool warm)
{
    bcmcth_meter_fp_sw_state_t *efp_sw_state = NULL;
    uint32_t ha_alloc_size = 0;
    uint32_t ha_req_size = 0;
    uint32_t num_meters = 0, num_sw_meters = 0;
    bcmcth_meter_fp_attrib_t *attr = NULL;

    SHR_FUNC_ENTER(unit);

    bcmcth_meter_fp_egr_attrib_get(unit, &attr);
    SHR_NULL_CHECK(attr, SHR_E_UNAVAIL);

    SHR_IF_ERR_EXIT
        (bcmcth_meter_fp_egr_sw_state_get(unit, &efp_sw_state));

    /* Meter ID 0 is reserved. */
    num_meters = (attr->num_meters_per_pipe * attr->num_pipes);
    num_sw_meters = (num_meters + 1);

    /* Allocate memory for the backup entry. */
    ha_req_size = (num_sw_meters * sizeof(bcmcth_meter_fp_entry_t));
    ha_alloc_size = ha_req_size;
    efp_sw_state->bkp_list =
        shr_ha_mem_alloc(unit,
                         BCMMGMT_METER_COMP_ID,
                         METER_FP_EGR_BKP_SUB_COMP_ID,
                         "bcmcthMeterFpEgrList",
                         &ha_alloc_size);
    SHR_NULL_CHECK(efp_sw_state->bkp_list, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT((ha_alloc_size < ha_req_size) ?
                    SHR_E_MEMORY : SHR_E_NONE);

    if (!warm) {
        sal_memset(efp_sw_state->bkp_list, 0, ha_alloc_size);

        bcmissu_struct_info_report(unit, BCMMGMT_METER_COMP_ID,
                                         METER_FP_EGR_BKP_SUB_COMP_ID, 0,
                                         sizeof(bcmcth_meter_fp_entry_t),
                                         num_sw_meters,
                                         BCMCTH_METER_FP_ENTRY_T_ID);
    }
    efp_sw_state->bkp_list[0].meter_in_use = true;

    /*
     * Allocate memory for EGR meter table.
     */
    SHR_ALLOC(efp_sw_state->list,
              ha_alloc_size,
              "bcmcthMeterFpEgrSwState");
    SHR_NULL_CHECK(efp_sw_state->list, SHR_E_MEMORY);
    sal_memset(efp_sw_state->list, 0, ha_alloc_size);

    sal_memcpy(efp_sw_state->list,
               efp_sw_state->bkp_list,
               ha_alloc_size);

    /*
     * Allocate HA memory for EFP meter table H/W index usage.
     */
    ha_req_size = (num_meters * sizeof(bool));
    ha_alloc_size = ha_req_size;

    efp_sw_state->bkp_hw_idx =
        shr_ha_mem_alloc(unit,
                         BCMMGMT_METER_COMP_ID,
                         METER_FP_EGR_HW_IDX_BKP_SUB_COMP_ID,
                         "bcmcthMeterFpEgrHwIdx",
                         &ha_alloc_size);
    SHR_NULL_CHECK(efp_sw_state->bkp_hw_idx, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT((ha_alloc_size < ha_req_size) ?
                    SHR_E_MEMORY : SHR_E_NONE);

    if (!warm) {
        sal_memset(efp_sw_state->bkp_hw_idx, 0, ha_alloc_size);
    }

    /*
     * Allocate memory for EFP meter table H/W index usage.
     */
    SHR_ALLOC(efp_sw_state->hw_idx,
              ha_alloc_size,
              "bcmcthMeterFpEgrHwIdxState");
    SHR_NULL_CHECK(efp_sw_state->hw_idx, SHR_E_MEMORY);
    sal_memset(efp_sw_state->hw_idx, 0, ha_alloc_size);

    sal_memcpy(efp_sw_state->hw_idx,
               efp_sw_state->bkp_hw_idx,
               ha_alloc_size);
exit:
    if (SHR_FUNC_ERR() && efp_sw_state) {
        if (efp_sw_state->bkp_hw_idx != NULL) {
            shr_ha_mem_free(unit, efp_sw_state->bkp_hw_idx);
            efp_sw_state->bkp_hw_idx = NULL;
        }
        if (efp_sw_state->bkp_list != NULL) {
            shr_ha_mem_free(unit, efp_sw_state->bkp_list);
            efp_sw_state->bkp_list = NULL;
        }
        SHR_FREE(efp_sw_state->hw_idx);
        SHR_FREE(efp_sw_state->list);
    }
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_fp_egr_cleanup(int unit)
{
    bcmcth_meter_fp_sw_state_t *efp_sw_state;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_meter_fp_egr_sw_state_get(unit, &efp_sw_state));

    SHR_FREE(efp_sw_state->hw_idx);
    SHR_FREE(efp_sw_state->list);

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_fp_egr_dev_config(int unit)
{
    uint64_t            refresh_en = 1;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_meter_fp_dev_config(unit,
                                    METER_FP_STAGE_ID_EGRESS,
                                    refresh_en));
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_egr_template_insert(int unit,
                                 bcmltd_sid_t sid,
                                 uint32_t trans_id,
                                 const bcmltd_field_t *key,
                                 const bcmltd_field_t *data)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_meter_template_insert(unit,
                                     METER_FP_STAGE_ID_EGRESS,
                                     trans_id,
                                     key,
                                     data));
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_egr_template_delete(int unit,
                                 bcmltd_sid_t sid,
                                 uint32_t trans_id,
                                 const bcmltd_field_t *key)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_meter_template_delete(unit,
                                     METER_FP_STAGE_ID_EGRESS,
                                     trans_id,
                                     key));
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_egr_template_update(int unit,
                                 bcmltd_sid_t sid,
                                 uint32_t trans_id,
                                 const bcmltd_field_t *key,
                                 const bcmltd_field_t *data)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_meter_template_update(unit,
                                     METER_FP_STAGE_ID_EGRESS,
                                     trans_id,
                                     key,
                                     data));
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_egr_template_lookup(int unit,
                                 bcmltd_sid_t sid,
                                 uint32_t trans_id,
                                 void *context,
                                 bcmimm_lookup_type_t lkup_type,
                                 const bcmltd_fields_t *in,
                                 bcmltd_fields_t *out)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_meter_template_lookup(unit,
                                     METER_FP_STAGE_ID_EGRESS,
                                     trans_id,
                                     context,
                                     lkup_type,
                                     in,
                                     out));
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_egr_template_commit(int unit,
                                 bcmltd_sid_t sid,
                                 uint32_t trans_id,
                                 void *context)
{
    bcmcth_meter_fp_sw_state_t *efp_sw_state;
    uint32_t table_size;
    uint32_t num_sw_meters = 0;
    bcmcth_meter_fp_attrib_t *attr = NULL;
    bcmcth_meter_fp_entry_t *bkp_ptr = NULL;
    bcmcth_meter_fp_entry_t *active_ptr = NULL;
    bool *hw_idx = NULL;
    bool *bkp_hw_idx = NULL;

    SHR_FUNC_ENTER(unit);

    bcmcth_meter_fp_egr_attrib_get(unit, &attr);
    SHR_NULL_CHECK(attr, SHR_E_UNAVAIL);

    SHR_IF_ERR_EXIT
        (bcmcth_meter_fp_egr_sw_state_get(unit, &efp_sw_state));

    /* Meter ID 0 is reserved. */
    num_sw_meters = ((attr->num_meters_per_pipe * attr->num_pipes) + 1);

    active_ptr = efp_sw_state->list;
    bkp_ptr = efp_sw_state->bkp_list;
    hw_idx = efp_sw_state->hw_idx;
    bkp_hw_idx = efp_sw_state->bkp_hw_idx;
    if ((bkp_ptr == NULL) || (active_ptr == NULL) ||
        (hw_idx == NULL) || (bkp_hw_idx == NULL)) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    table_size = (num_sw_meters * sizeof(bcmcth_meter_fp_entry_t));
    /* Copy active list to backup list. */
    sal_memcpy(bkp_ptr, active_ptr, table_size);

    table_size = (attr->num_pipes *
                  attr->num_meters_per_pipe *
                  sizeof(bool));
    /* Copy active hw idx list to backup list. */
    sal_memcpy(bkp_hw_idx, hw_idx, table_size);
exit:
    SHR_FUNC_EXIT();
}

void
bcmcth_meter_egr_template_abort(int unit,
                                bcmltd_sid_t sid,
                                uint32_t trans_id,
                                void *context)
{
    bcmcth_meter_fp_sw_state_t *efp_sw_state;
    uint32_t table_size;
    uint32_t num_sw_meters = 0;
    bcmcth_meter_fp_attrib_t *attr = NULL;
    bcmcth_meter_fp_entry_t *bkp_ptr = NULL;
    bcmcth_meter_fp_entry_t *active_ptr = NULL;
    bool *hw_idx = NULL;
    bool *bkp_hw_idx = NULL;

    bcmcth_meter_fp_egr_attrib_get(unit, &attr);
    if (attr == NULL) {
        return;
    }

    if (SHR_FAILURE(bcmcth_meter_fp_egr_sw_state_get(unit, &efp_sw_state))) {
        return;
    }

    /* Meter ID 0 is reserved. */
    num_sw_meters = ((attr->num_meters_per_pipe * attr->num_pipes) + 1);

    active_ptr = efp_sw_state->list;
    bkp_ptr = efp_sw_state->bkp_list;
    hw_idx = efp_sw_state->hw_idx;
    bkp_hw_idx = efp_sw_state->bkp_hw_idx;
    if ((bkp_ptr == NULL) || (active_ptr == NULL) ||
        (hw_idx == NULL) || (bkp_hw_idx == NULL)) {
        return;
    }

    table_size = (num_sw_meters * sizeof(bcmcth_meter_fp_entry_t));
    /* Copy backup list to active list. */
    sal_memcpy(active_ptr, bkp_ptr, table_size);

    table_size = (attr->num_pipes *
                  attr->num_meters_per_pipe *
                  sizeof(bool));
    /* Copy backup hw idx list to active list. */
    sal_memcpy(hw_idx, bkp_hw_idx, table_size);

    return;
}

int
bcmcth_meter_fp_egr_drv_get(int unit,
                            bcmcth_meter_fp_drv_t **drv)
{
    SHR_FUNC_ENTER(unit);

    if (meter_fp_egr_drv[unit] != NULL) {
        *drv = meter_fp_egr_drv[unit];
    } else {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_fp_egr_attrib_get(int unit,
                               bcmcth_meter_fp_attrib_t **attr)
{
    bcmcth_meter_fp_drv_t *drv = NULL;

    bcmcth_meter_fp_egr_drv_get(unit, &drv);

    if (drv && drv->attrib_get) {
        return drv->attrib_get(unit, attr);
    }
    return SHR_E_UNAVAIL;
}
