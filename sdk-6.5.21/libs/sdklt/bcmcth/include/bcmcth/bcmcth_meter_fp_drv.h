/*! \file bcmcth_meter_fp_drv.h
 *
 * BCMCTH Meter top-level APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_METER_FP_DRV_H
#define BCMCTH_METER_FP_DRV_H

#include <bcmcth/bcmcth_meter_internal.h>

/*!
 * \brief FP meter HW clear.
 *
 * \param [in] unit Unit number.
 * \param [in] drv FP meter driver object.
 * \param [in] refresh_en Refresh enable status.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_meter_fp_hw_config(int unit,
                          bcmcth_meter_fp_drv_t *drv,
                          uint32_t refresh_en);

/*!
 * \brief FP meter HW clear.
 *
 * \param [in] unit Unit number.
 * \param [in] drv FP meter driver object.
 * \param [in] lt_entry Meter Logical table entry.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_meter_fp_hw_param_compute(int unit,
                                 bcmcth_meter_fp_drv_t *drv,
                                 bcmcth_meter_sw_params_t *lt_entry);

/*!
 * \brief Get actual FP meter operational rate.
 *
 * \param [in] unit Unit number.
 * \param [in] drv FP meter driver object.
 * \param [in] lt_entry Meter Logical table entry.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_meter_fp_hw_oper_get(int unit,
                            bcmcth_meter_fp_drv_t *drv,
                            bcmcth_meter_sw_params_t *lt_entry);

/*!
 * \brief Allocate FP meter HW index.
 *
 * \param [in] unit Unit number.
 * \param [in] drv FP meter driver object.
 * \param [in] pool Pool instance.
 * \param [in] oper_mode FP operating mode (global/unique).
 * \param [in] lt_entry Meter Logical table entry.
 * \param [out] hw_idx HW index.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_meter_fp_hw_idx_alloc(int unit,
                             bcmcth_meter_fp_drv_t *drv,
                             int pool,
                             uint8_t oper_mode,
                             bcmcth_meter_sw_params_t *lt_entry,
                             uint32_t *hw_idx);

/*!
 * \brief Free FP meter HW index.
 *
 * \param [in] unit Unit number.
 * \param [in] drv FP meter driver object.
 * \param [in] hw_idx HW index.
 * \param [in] lt_entry Meter Logical table entry.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_meter_fp_hw_idx_free(int unit,
                            bcmcth_meter_fp_drv_t *drv,
                            uint32_t hw_idx,
                            bcmcth_meter_sw_params_t *lt_entry);

/*!
 * \brief FP meter HW write.
 *
 * \param [in] unit Unit number.
 * \param [in] drv FP meter driver object.
 * \param [in] oper_mode FP operating mode (global/unique).
 * \param [in] trans_id Transaction ID.
 * \param [in] lt_entry Meter Logical table entry.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_meter_fp_hw_write(int unit,
                         bcmcth_meter_fp_drv_t *drv,
                         uint8_t oper_mode,
                         uint32_t trans_id,
                         bcmcth_meter_sw_params_t *lt_entry);

/*!
 * \brief FP meter HW clear.
 *
 * \param [in] unit Unit number.
 * \param [in] drv FP meter driver object.
 * \param [in] oper_mode FP operating mode (global/unique).
 * \param [in] trans_id Transaction ID.
 * \param [in] pool Pool instance.
 * \param [in] hw_idx HW index.
 * \param [in] lt_entry Meter Logical table entry.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_meter_fp_hw_clear(int unit,
                         bcmcth_meter_fp_drv_t *drv,
                         uint8_t oper_mode,
                         uint32_t trans_id,
                         int pool,
                         uint32_t hw_idx,
                         bcmcth_meter_sw_params_t *lt_entry);

/*!
 * \brief FP meter action set prepare.
 *
 * \param [in] unit Unit number.
 * \param [in] drv FP meter driver object.
 * \param [in] lt_entry Meter Logical table entry.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_meter_fp_meter_format_get(int unit,
                                 bcmcth_meter_fp_drv_t *drv,
                                 bcmcth_meter_sw_params_t *lt_entry);

#endif /* BCMCTH_METER_FP_DRV_H */
