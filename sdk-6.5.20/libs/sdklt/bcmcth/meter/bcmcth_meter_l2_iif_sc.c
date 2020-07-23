/*! \file bcmcth_meter_l2_iif_sc.c
 *
 * This file contains Storm Control meter Action custom handler functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <shr/shr_fmm.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <sal/sal_time.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmcth/bcmcth_meter_sysm.h>
#include <bcmcth/bcmcth_meter_l2_iif_sc_drv.h>

#include <bcmcth/bcmcth_meter_l2_iif_sc.h>

#define BSL_LOG_MODULE  BSL_LS_BCMCTH_METER

/*******************************************************************************
 * Local definitions
 */

/*! Device specific attach function type for tunnel encap. */
typedef bcmcth_meter_l2_iif_sc_drv_t *(*bcmcth_meter_l2_iif_sc_drv_get_f)(int unit);

/*! Array of device specific attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
{ _bc##_cth_meter_l2_iif_sc_drv_get },
static struct {
    bcmcth_meter_l2_iif_sc_drv_get_f drv_get;
} meter_l2_iif_sc_drv_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

static bcmcth_meter_l2_iif_sc_drv_t *meter_l2_iif_sc_drv[BCMDRD_CONFIG_MAX_UNITS];
static shr_famm_hdl_t meter_l2_iif_sc_array_hdl[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */
static int
meter_l2_iif_sc_handler(int unit,
                        bcmltd_sid_t sid,
                        uint32_t trans_id,
                        bcmimm_entry_event_t event_reason,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        void *context,
                        bcmltd_fields_t *output_fields)
{
    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event_reason) {
    case BCMIMM_ENTRY_INSERT:
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmcth_meter_l2_iif_sc_insert(unit,
                                          sid,
                                          trans_id,
                                          key,
                                          data,
                                          output_fields));
        break;
    case BCMIMM_ENTRY_UPDATE:
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmcth_meter_l2_iif_sc_update(unit,
                                          sid,
                                          trans_id,
                                          key,
                                          data,
                                          output_fields));
        break;
    case BCMIMM_ENTRY_DELETE:
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmcth_meter_l2_iif_sc_delete(unit,
                                          sid,
                                          trans_id,
                                          key));
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
/*!
 * \brief METER_L2_IIF_STORM_CONTROL In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to METER_L2_IIF_STORM_CONTROL logical table entry commit stages.
 */
static bcmimm_lt_cb_t meter_l2_iif_sc_imm_callback = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = meter_l2_iif_sc_handler,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL,

    /*! Lookup function. */
    .lookup = NULL,
};

int
bcmcth_meter_l2_iif_sc_imm_register(int unit)
{
    const bcmlrd_map_t  *map = NULL;
    int rv = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (shr_famm_hdl_init(64, &meter_l2_iif_sc_array_hdl[unit]));

    rv = bcmlrd_map_get(unit, METER_L2_IIF_STORM_CONTROLt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT(bcmimm_lt_event_reg(unit,
                                            METER_L2_IIF_STORM_CONTROLt,
                                            &meter_l2_iif_sc_imm_callback,
                                            NULL));
    }

exit:
    SHR_FUNC_EXIT();
}

void
bcmcth_meter_l2_iif_sc_imm_db_cleanup(int unit)
{
    if (meter_l2_iif_sc_array_hdl[unit]) {
        shr_famm_hdl_delete(meter_l2_iif_sc_array_hdl[unit]);
        meter_l2_iif_sc_array_hdl[unit] = 0;
    }
}

/*!
 * \brief Initialize device driver.
 *
 * Initialize device features and install base driver functions.
 *
 * \param [in] unit Unit number.
 */
int
bcmcth_meter_l2_iif_sc_drv_init(int unit)
{
    bcmdrd_dev_type_t dev_type;

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        return SHR_E_UNIT;
    }

    /* Perform device-specific software setup */
    meter_l2_iif_sc_drv[unit] = meter_l2_iif_sc_drv_get[dev_type].drv_get(unit);

    return SHR_E_NONE;
}

int
bcmcth_meter_l2_iif_sc_drv_cleanup(int unit)
{
    bcmdrd_dev_type_t dev_type;

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        return SHR_E_UNIT;
    }

    meter_l2_iif_sc_drv[unit] = NULL;

    return SHR_E_NONE;
}

int
bcmcth_meter_l2_iif_sc_drv_get(int unit,
                               bcmcth_meter_l2_iif_sc_drv_t **drv)
{
    SHR_FUNC_ENTER(unit);

    if (meter_l2_iif_sc_drv[unit] != NULL) {
        *drv = meter_l2_iif_sc_drv[unit];
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_meter_l2_iif_sc_arr_hdl_get(int unit,
                                   shr_famm_hdl_t *hdl)
{
    SHR_FUNC_ENTER(unit);

    *hdl = meter_l2_iif_sc_array_hdl[unit];

    SHR_FUNC_EXIT();
}
