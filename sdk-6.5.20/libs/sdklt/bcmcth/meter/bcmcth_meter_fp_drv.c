/*! \file bcmcth_meter_drv.c
 *
 * BCMCTH FP meter driver APIs wrappers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmcth/bcmcth_meter_fp_drv.h>

#define BSL_LOG_MODULE  BSL_LS_BCMCTH_METER

/*******************************************************************************
 * Public functions
 */

int
bcmcth_meter_fp_hw_config(int unit,
                          bcmcth_meter_fp_drv_t *drv,
                          uint32_t refresh_en)
{
    if (drv && drv->hw_config) {
        return drv->hw_config(unit, refresh_en);
    }
    return SHR_E_UNAVAIL;
}

int
bcmcth_meter_fp_hw_param_compute(int unit,
                                 bcmcth_meter_fp_drv_t *drv,
                                 bcmcth_meter_sw_params_t *lt_entry)
{
    if (drv && drv->hw_param_compute) {
        return drv->hw_param_compute(unit, lt_entry);
    }
    return SHR_E_UNAVAIL;
}

int
bcmcth_meter_fp_hw_oper_get(int unit,
                            bcmcth_meter_fp_drv_t *drv,
                            bcmcth_meter_sw_params_t *lt_entry)
{
    if (drv && drv->hw_oper_get) {
        return drv->hw_oper_get(unit, lt_entry);
    }
    return SHR_E_UNAVAIL;
}

int
bcmcth_meter_fp_hw_idx_alloc(int unit,
                             bcmcth_meter_fp_drv_t *drv,
                             int pool,
                             uint8_t oper_mode,
                             bcmcth_meter_sw_params_t *lt_entry,
                             uint32_t *hw_idx)
{
    if (drv && drv->hw_idx_alloc) {
        return drv->hw_idx_alloc(unit, pool, oper_mode, lt_entry, hw_idx);
    }
    return SHR_E_UNAVAIL;
}

int
bcmcth_meter_fp_hw_idx_free(int unit,
                            bcmcth_meter_fp_drv_t *drv,
                            uint32_t hw_idx,
                            bcmcth_meter_sw_params_t *lt_entry)
{
    if (drv && drv->hw_idx_free) {
        return drv->hw_idx_free(unit, hw_idx, lt_entry);
    }
    return SHR_E_UNAVAIL;
}

int
bcmcth_meter_fp_hw_write(int unit,
                         bcmcth_meter_fp_drv_t *drv,
                         uint8_t oper_mode,
                         uint32_t trans_id,
                         bcmcth_meter_sw_params_t *lt_entry)
{
    if (drv && drv->hw_write) {
        return drv->hw_write(unit, oper_mode, trans_id, lt_entry);
    }
    return SHR_E_UNAVAIL;
}

int
bcmcth_meter_fp_hw_clear(int unit,
                         bcmcth_meter_fp_drv_t *drv,
                         uint8_t oper_mode,
                         uint32_t trans_id,
                         int pool,
                         uint32_t hw_idx,
                         bcmcth_meter_sw_params_t *lt_entry)
{
    if (drv && drv->hw_clear) {
        return drv->hw_clear(unit, oper_mode, trans_id, pool,
                             hw_idx, lt_entry);
    }
    return SHR_E_UNAVAIL;
}

int
bcmcth_meter_fp_meter_format_get(int unit,
                                 bcmcth_meter_fp_drv_t *drv,
                                 bcmcth_meter_sw_params_t *lt_entry)
{
    if (drv && drv->meter_format_get) {
        return drv->meter_format_get(unit, lt_entry);
    }
    /* No error if unsupported */
    return SHR_E_NONE;
}
