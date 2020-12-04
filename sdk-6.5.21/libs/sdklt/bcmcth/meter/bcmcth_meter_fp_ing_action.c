/*! \file bcmcth_meter_fp_ing_action.c
 *
 * This file contains IFP Meter Action custom handler functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <shr/shr_fmm.h>
#include <shr/shr_util.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_internal.h>
#include <bcmlrd/bcmlrd_conf.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <sal/sal_time.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmissu/issu_api.h>
#include <bcmcth/bcmcth_meter_sysm.h>
#include <bcmcth/bcmcth_meter_util.h>
#include <bcmcth/bcmcth_meter_action_internal.h>
#include <bcmcth/bcmcth_meter_pdd_internal.h>
#include <bcmcth/bcmcth_meter_sbr_internal.h>

#include <bcmcth/bcmcth_meter_fp_ing_action.h>

/*******************************************************************************
 * Local definitions and macros
 */
#define BSL_LOG_MODULE  BSL_LS_BCMCTH_METER

/*! Device specific attach function type for METER_ING_ACTION_TEMPLATE. */
typedef bcmcth_meter_action_drv_t *(*bcmcth_meter_ing_action_drv_get_f)(int unit);
/*! Array of device specific attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
{ _bc##_cth_meter_ing_action_drv_get },
static struct {
    bcmcth_meter_ing_action_drv_get_f drv_get;
} meter_ing_action_drv_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

typedef int (*bcmcth_meter_ing_action_variant_init_f)(int unit, bcmcth_meter_action_drv_t *drv);
/* Array of device variant specific FP tables init function pointers. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
    { _bd##_vu##_va##_di_cth_meter_ing_action_variant_init},
static struct {
    bcmcth_meter_ing_action_variant_init_f init;
} meter_ing_action_variant_init[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmlrd/chip/bcmlrd_variant.h>
    { 0 } /* end-of-list */
};

/*! METER_ING_ACTION_TEMPLATE driver. */
static bcmcth_meter_action_drv_t *meter_fp_ing_action_drv[BCMDRD_CONFIG_MAX_UNITS];
static bcmcth_meter_action_pdd_sw_state_t meter_fp_ing_pdd_state[BCMDRD_CONFIG_MAX_UNITS];
static bcmcth_meter_action_pdd_sw_state_t meter_fp_ing_pdd_state_ha[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Public functions
 */

/*!
 * \brief IFP meter SBR HW entry prepare.
 *
 * Convert IFP meter action LT entry to HW entry.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  pt_id         Physical table id.
 * \param [in]  data          Input field list.
 * \param [out] buf           HW entry buffer.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_meter_fp_ing_sbr_hw_entry_prepare(int unit,
                                         uint32_t pt_id,
                                         const bcmltd_field_t *in_data,
                                         uint8_t *buf)
{
    uint32_t    fid = 0, data = 0, i = 0;
    uint32_t    cont_id = 0, num_cont = 0, map_fid = 0;
    uint32_t    s_bit = 0, e_bit = 0, width = 0;
    const bcmltd_field_t *field;
    bcmcth_meter_action_drv_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    bcmcth_meter_fp_ing_action_drv_get(unit, &drv);
    if (drv == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    field = in_data;
    while (field) {
        fid = field->id;
        data = field->data;

        map_fid = drv->sbr_lt_info->fid_info[fid].map_fid;
        num_cont = drv->pdd_lt_info->cont_info[map_fid].num_conts;
        width = drv->sbr_pt_info->bits_per_pri;

        for (i = 0; i < num_cont; i++) {
            cont_id = drv->pdd_lt_info->cont_info[map_fid].cont_ids[i];
            s_bit = (cont_id * width);
            e_bit = (s_bit + width - 1);
            bcmdrd_field_set((uint32_t *)buf,
                             s_bit,
                             e_bit,
                             &data);
        }

        field = field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_meter_fp_ing_action_variant_init(int unit,
                                        bcmcth_meter_action_drv_t *drv)
{
    bcmlrd_variant_t variant;

    SHR_FUNC_ENTER(unit);

    variant = bcmlrd_variant_get(unit);
    if (variant == BCMLRD_VARIANT_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_ing_action_variant_init[variant].init(unit, drv));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Initialize device driver.
 *
 * Initialize device features and install base driver functions.
 *
 * \param [in] unit Unit number.
 */
int
bcmcth_meter_fp_ing_action_drv_init(int unit)
{
    uint8_t     oper_mode = 0;
    uint32_t    pipe_map = 0, num_pipes = 0;
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    /* Perform device-specific software setup */
    meter_fp_ing_action_drv[unit] = meter_ing_action_drv_get[dev_type].drv_get(unit);

    SHR_IF_ERR_EXIT(
        bcmcth_meter_fp_ing_action_variant_init(unit,
                                                meter_fp_ing_action_drv[unit]));

    /* Driver specific init routine. */
    if (meter_fp_ing_action_drv[unit] != NULL) {
        SHR_IF_ERR_EXIT(meter_fp_ing_action_drv[unit]->init(unit));

        SHR_IF_ERR_EXIT
            (bcmdrd_dev_valid_pipes_get(unit, &pipe_map));
        num_pipes = shr_util_popcount(pipe_map);

        SHR_IF_ERR_EXIT
            (bcmcth_meter_fp_ing_oper_mode_get(unit, &oper_mode));

        if ((oper_mode != METER_FP_OPER_MODE_PIPE_UNIQUE) ||
            (num_pipes == 0)) {
            meter_fp_ing_action_drv[unit]->num_pipes = 1;
        } else {
            meter_fp_ing_action_drv[unit]->num_pipes = num_pipes;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup device driver.
 *
 * \param [in] unit Unit number.
 */
int
bcmcth_meter_fp_ing_action_drv_cleanup(int unit)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    if (meter_fp_ing_action_drv[unit] != NULL) {
        meter_fp_ing_action_drv[unit]->num_action_lts = 0;
        meter_fp_ing_action_drv[unit]->pdd_lt_info = NULL;
        meter_fp_ing_action_drv[unit]->sbr_lt_info = NULL;
        meter_fp_ing_action_drv[unit]->action_lt_info = NULL;
    }

    meter_fp_ing_action_drv[unit] = NULL;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief IFP meter init
 *
 * Initialize IFP Meter related data structures
 * for this unit.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  warm          Cold/Warm boot.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_meter_fp_ing_action_init (int unit, bool warm)
{
    uint32_t num_pipes = 0;
    uint32_t ha_alloc_size = 0, ha_req_size = 0;
    uint32_t pdd_action_count = 0, num_profiles = 0;
    bcmcth_meter_action_drv_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    bcmcth_meter_fp_ing_action_drv_get(unit, &drv);
    if (drv == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    /*
     * Allocate memory for PDD profiles.
     */
    num_pipes = drv->num_pipes;
    num_profiles = drv->pdd_pt_info->num_profiles;
    pdd_action_count = drv->pdd_pt_info->pdd_bmp_size;
    ha_req_size = (num_pipes *
                   num_profiles *
                   pdd_action_count *
                   sizeof(bcmcth_meter_action_pdd_offset_t));
    ha_alloc_size = ha_req_size;

    METER_FP_ING_PDD_STATE_HA(unit).offset_info =
        shr_ha_mem_alloc(unit,
                         BCMMGMT_METER_COMP_ID,
                         METER_FP_ING_PDD_OFFSET_SUB_COMP_ID,
                         "bcmcthMeterFpIngActionPdd",
                         &ha_alloc_size);

    SHR_NULL_CHECK(METER_FP_ING_PDD_STATE_HA(unit).offset_info, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT((ha_alloc_size < ha_req_size) ?
                    SHR_E_MEMORY : SHR_E_NONE);

    if (!warm) {
        sal_memset(METER_FP_ING_PDD_STATE_HA(unit).offset_info,
                   0,
                   ha_alloc_size);

        bcmissu_struct_info_report(unit, BCMMGMT_METER_COMP_ID,
                                         METER_FP_ING_PDD_OFFSET_SUB_COMP_ID, 0,
                                         sizeof(bcmcth_meter_action_pdd_offset_t),
                                         (num_profiles * pdd_action_count),
                                         BCMCTH_METER_ACTION_PDD_OFFSET_T_ID);
    }

    SHR_ALLOC(METER_FP_ING_PDD_STATE(unit).offset_info,
              ha_alloc_size,
              "bcmcthMeterFpIngPddOffsetInfo");
    SHR_NULL_CHECK(METER_FP_ING_PDD_STATE(unit).offset_info, SHR_E_MEMORY);
    sal_memset(METER_FP_ING_PDD_STATE(unit).offset_info,
               0,
               ha_alloc_size);

    /* Syncing the temp space to committed SW state. */
    sal_memcpy(METER_FP_ING_PDD_STATE(unit).offset_info,
               METER_FP_ING_PDD_STATE_HA(unit).offset_info,
               ha_alloc_size);

    /* Allocate memory for saving PDD state. */
    ha_req_size = (num_pipes * num_profiles * sizeof(uint8_t));
    ha_alloc_size = ha_req_size;

    METER_FP_ING_PDD_STATE_HA(unit).state =
        shr_ha_mem_alloc(unit,
                         BCMMGMT_METER_COMP_ID,
                         METER_FP_ING_PDD_STATE_SUB_COMP_ID,
                         "bcmcthMeterFpIngActionPddState",
                         &ha_alloc_size);

    SHR_NULL_CHECK(METER_FP_ING_PDD_STATE_HA(unit).state, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT((ha_alloc_size < ha_req_size) ?
                    SHR_E_MEMORY : SHR_E_NONE);

    if (!warm) {
        sal_memset(METER_FP_ING_PDD_STATE_HA(unit).state,
                   0,
                   ha_alloc_size);
    }

    SHR_ALLOC(METER_FP_ING_PDD_STATE(unit).state,
              ha_alloc_size,
              "bcmcthMeterFpIngPddOffsetInfo");
    SHR_NULL_CHECK(METER_FP_ING_PDD_STATE(unit).state, SHR_E_MEMORY);
    sal_memset(METER_FP_ING_PDD_STATE(unit).state,
               0,
               ha_alloc_size);

    /* Syncing the temp space to committed SW state. */
    sal_memcpy(METER_FP_ING_PDD_STATE(unit).state,
               METER_FP_ING_PDD_STATE_HA(unit).state,
               ha_alloc_size);

exit:
    if (SHR_FUNC_ERR()) {
        if (METER_FP_ING_PDD_STATE_HA(unit).offset_info != NULL) {
            shr_ha_mem_free(unit, METER_FP_ING_PDD_STATE_HA(unit).offset_info);
            METER_FP_ING_PDD_STATE_HA(unit).offset_info = NULL;
        }

        if (METER_FP_ING_PDD_STATE(unit).offset_info != NULL) {
            SHR_FREE(METER_FP_ING_PDD_STATE(unit).offset_info);
            METER_FP_ING_PDD_STATE(unit).offset_info = NULL;
        }
        if (METER_FP_ING_PDD_STATE_HA(unit).state != NULL) {
            shr_ha_mem_free(unit, METER_FP_ING_PDD_STATE_HA(unit).state);
            METER_FP_ING_PDD_STATE_HA(unit).state = NULL;
        }

        if (METER_FP_ING_PDD_STATE(unit).state != NULL) {
            SHR_FREE(METER_FP_ING_PDD_STATE(unit).state);
            METER_FP_ING_PDD_STATE(unit).state = NULL;
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief IFP meter cleanup
 *
 * Cleanup IFP Meter related data structures
 * allocated for this unit.
 *
 * \param [in]  unit          Unit Number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_meter_fp_ing_action_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (METER_FP_ING_PDD_STATE(unit).offset_info != NULL) {
        SHR_FREE(METER_FP_ING_PDD_STATE(unit).offset_info);
        METER_FP_ING_PDD_STATE(unit).offset_info = NULL;
    }

    if (METER_FP_ING_PDD_STATE(unit).state != NULL) {
        SHR_FREE(METER_FP_ING_PDD_STATE(unit).state);
        METER_FP_ING_PDD_STATE(unit).state = NULL;
    }

    SHR_FUNC_EXIT();
}

int
bcmcth_meter_ing_action_entry_insert(int unit,
                                     bcmltd_sid_t sid,
                                     uint32_t trans_id,
                                     const bcmltd_field_t *key,
                                     const bcmltd_field_t *data,
                                     bcmltd_fields_t *out)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_meter_action_entry_insert(unit,
                                         sid,
                                         METER_FP_STAGE_ID_INGRESS,
                                         trans_id,
                                         key,
                                         data,
                                         out));
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_ing_action_entry_update(int unit,
                                     bcmltd_sid_t sid,
                                     uint32_t trans_id,
                                     const bcmltd_field_t *key,
                                     const bcmltd_field_t *data,
                                     bcmltd_fields_t *out)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_meter_action_entry_update(unit,
                                         sid,
                                         METER_FP_STAGE_ID_INGRESS,
                                         trans_id,
                                         key,
                                         data,
                                         out));
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_ing_action_entry_delete(int unit,
                                     bcmltd_sid_t sid,
                                     uint32_t trans_id,
                                     const bcmltd_field_t *key,
                                     const bcmltd_field_t *data)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_meter_action_entry_delete(unit,
                                         sid,
                                         METER_FP_STAGE_ID_INGRESS,
                                         trans_id,
                                         key,
                                         data));
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_ing_pdd_entry_insert(int unit,
                                  bcmltd_sid_t sid,
                                  uint32_t trans_id,
                                  bcmimm_entry_event_t reason,
                                  const bcmltd_field_t *key,
                                  const bcmltd_field_t *data)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_meter_pdd_entry_insert(unit,
                                      sid,
                                      METER_FP_STAGE_ID_INGRESS,
                                      trans_id,
                                      reason,
                                      key,
                                      data));
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_ing_pdd_entry_delete(int unit,
                                  bcmltd_sid_t sid,
                                  uint32_t trans_id,
                                  const bcmltd_field_t *key,
                                  const bcmltd_field_t *data)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_meter_pdd_entry_delete(unit,
                                      METER_FP_STAGE_ID_INGRESS,
                                      trans_id,
                                      key,
                                      data));
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_ing_sbr_entry_insert(int unit,
                                  bcmltd_sid_t sid,
                                  uint32_t trans_id,
                                  const bcmltd_field_t *key,
                                  const bcmltd_field_t *data)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_meter_sbr_entry_insert(unit,
                                      METER_FP_STAGE_ID_INGRESS,
                                      trans_id,
                                      key,
                                      data));
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_ing_sbr_entry_delete(int unit,
                                  bcmltd_sid_t sid,
                                  uint32_t trans_id,
                                  const bcmltd_field_t *key,
                                  const bcmltd_field_t *data)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_meter_sbr_entry_delete(unit,
                                      METER_FP_STAGE_ID_INGRESS,
                                      trans_id,
                                      key,
                                      data));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief IFP meter action transaction commit
 *
 * Discard the backup state as the current
 * transaction is getting committed.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_meter_fp_ing_action_commit (int unit,
                                   bcmltd_sid_t sid,
                                   uint32_t trans_id,
                                   void *context)
{
    uint32_t num_pipes = 0;
    uint32_t alloc_sz = 0;
    uint32_t pdd_action_count = 0, num_profiles = 0;
    void *act_ptr;
    void *bkp_ptr;
    bcmcth_meter_action_drv_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    bcmcth_meter_fp_ing_action_drv_get(unit, &drv);
    if (drv == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    act_ptr = METER_FP_ING_PDD_STATE(unit).offset_info;
    bkp_ptr = METER_FP_ING_PDD_STATE_HA(unit).offset_info;

    num_pipes = drv->num_pipes;
    num_profiles = drv->pdd_pt_info->num_profiles;
    pdd_action_count = drv->pdd_pt_info->pdd_bmp_size;
    alloc_sz = (num_pipes *
                num_profiles *
                pdd_action_count *
                sizeof(bcmcth_meter_action_pdd_offset_t));

    /* Copy active data to backup data. */
    sal_memcpy(bkp_ptr, act_ptr, alloc_sz);

    act_ptr = METER_FP_ING_PDD_STATE(unit).state;
    bkp_ptr = METER_FP_ING_PDD_STATE_HA(unit).state;

    num_profiles = drv->pdd_pt_info->num_profiles;
    alloc_sz = (num_pipes * num_profiles * sizeof(uint8_t));

    /* Copy active data to backup data. */
    sal_memcpy(bkp_ptr, act_ptr, alloc_sz);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief IFP meter action transaction abort
 *
 * Rollback the Meter entry from the backup state
 * as the current transaction is getting aborted.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
void
bcmcth_meter_fp_ing_action_abort (int unit,
                                  bcmltd_sid_t sid,
                                  uint32_t trans_id,
                                  void *context)
{
    uint32_t num_pipes = 0;
    uint32_t alloc_sz = 0;
    uint32_t pdd_action_count = 0, num_profiles = 0;
    void *act_ptr;
    void *bkp_ptr;
    bcmcth_meter_action_drv_t *drv = NULL;

    bcmcth_meter_fp_ing_action_drv_get(unit, &drv);
    if (drv == NULL) {
        SHR_EXIT();
    }

    act_ptr = METER_FP_ING_PDD_STATE(unit).offset_info;
    bkp_ptr = METER_FP_ING_PDD_STATE_HA(unit).offset_info;

    num_pipes = drv->num_pipes;
    num_profiles = drv->pdd_pt_info->num_profiles;
    pdd_action_count = drv->pdd_pt_info->pdd_bmp_size;
    alloc_sz = (num_pipes *
                num_profiles *
                pdd_action_count *
                sizeof(bcmcth_meter_action_pdd_offset_t));

    /* Copy backup data to active data. */
    sal_memcpy(act_ptr, bkp_ptr, alloc_sz);

    act_ptr = METER_FP_ING_PDD_STATE(unit).state;
    bkp_ptr = METER_FP_ING_PDD_STATE_HA(unit).state;

    num_profiles = drv->pdd_pt_info->num_profiles;
    alloc_sz = (num_pipes * num_profiles * sizeof(uint8_t));

    /* Copy active data to backup data. */
    sal_memcpy(act_ptr, bkp_ptr, alloc_sz);

exit:
    return;
}

/*!
 * \brief Get pointer to IFP meter driver device structure.
 *
 * \param [in] unit Unit number.
 * \param [out] drv Chip driver structure.
 *
 * \return Pointer to device structure, or NULL if not found.
 */
int
bcmcth_meter_fp_ing_action_drv_get (int unit,
                                    bcmcth_meter_action_drv_t **drv)
{
     SHR_FUNC_ENTER(unit);

     if (meter_fp_ing_action_drv[unit] != NULL) {
         *drv = meter_fp_ing_action_drv[unit];
     } else {
         SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
     }

 exit:
     SHR_FUNC_EXIT();
}

int
bcmcth_meter_fp_ing_pdd_state_get(int unit,
                                 bcmcth_meter_action_pdd_sw_state_t **ptr)
{
    SHR_FUNC_ENTER(unit);

    *ptr = &meter_fp_ing_pdd_state[unit];

    SHR_FUNC_EXIT();
}

int
bcmcth_meter_fp_ing_oper_mode_get(int unit,
                                  uint8_t *mode)
{
    int rv = SHR_E_NONE;
    uint64_t val = 0;
    uint32_t pipe_ltid = 0, pipe_fid = 0;
    const bcmlrd_map_t *map = NULL;
    bcmcth_meter_action_drv_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    bcmcth_meter_fp_ing_action_drv_get(unit, &drv);
    if (drv == NULL) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    *mode = METER_FP_OPER_MODE_GLOBAL;

    if (drv->action_lt_info != NULL) {
        pipe_ltid = drv->action_lt_info[0].pipe_ltid;
        pipe_fid = drv->action_lt_info[0].pipe_cfg_fid;

        if (pipe_ltid != 0) {
            rv = bcmlrd_map_get(unit, pipe_ltid, &map);
            if (SHR_SUCCESS(rv) && map) {
                rv = bcmcfg_field_get(unit,
                                      pipe_ltid,
                                      pipe_fid,
                                      &val);
            }

            if (rv == SHR_E_NONE) {
                *mode = val;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}
