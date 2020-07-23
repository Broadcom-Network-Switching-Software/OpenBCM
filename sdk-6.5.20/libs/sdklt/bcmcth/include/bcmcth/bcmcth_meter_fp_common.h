/*! \file bcmcth_meter_fp_common.h
 *
 * This file contains chip specific function
 * pointer declarations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_METER_FP_COMMON_H
#define BCMCTH_METER_FP_COMMON_H

#include <bcmcth/bcmcth_meter_internal.h>

/*!
 * \brief Clear FP meter entry
 *
 * Clear FP meter table entry in hardware.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id LT transaction ID.
 * \param [in] attr FP attributes.
 * \param [in] state FP software state.
 * \param [in] oper_mode FP operating mode (global/unique).
 * \param [in] pool Pool instance.
 * \param [in] hw_index HW index to be cleared.
 * \param [in] lt_entry Meter Logical table entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
extern int
bcmcth_meter_util_entry_clear(int unit,
                              bcmcth_meter_fp_attrib_t *attr,
                              bcmcth_meter_fp_sw_state_t *state,
                              uint8_t oper_mode,
                              uint32_t trans_id,
                              int pool,
                              uint32_t hw_index,
                              bcmcth_meter_sw_params_t *lt_entry);

/*!
 * \brief Write FP meter entry
 *
 * Write FP meter table entry in hardware.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id LT transaction ID.
 * \param [in] attr FP attributes.
 * \param [in] state FP software state.
 * \param [in] oper_mode FP operating mode (global/unique).
 * \param [in] lt_entry Meter Logical table entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
extern int
bcmcth_meter_util_entry_write(int unit,
                              uint32_t trans_id,
                              bcmcth_meter_fp_attrib_t *attr,
                              bcmcth_meter_fp_sw_state_t *state,
                              uint8_t oper_mode,
                              bcmcth_meter_sw_params_t *lt_entry);

/*!
 * \brief Free meter index
 *
 * Free the hardware meter index.
 *
 * \param [in] unit Unit number.
 * \param [in] attr FP attributes.
 * \param [in] state FP software state.
 * \param [in] index Hardware meter index.
 * \param [in] lt_entry Meter Logical table entry.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_meter_util_hw_index_free(int unit,
                                bcmcth_meter_fp_attrib_t *attr,
                                bcmcth_meter_fp_sw_state_t *state,
                                uint32_t index,
                                bcmcth_meter_sw_params_t *lt_entry);

/*!
 * \brief Allocate meter index
 *
 * Find a free hardware meter index in hardware meter table.
 *
 * \param [in] unit Unit number.
 * \param [in] attr FP attributes.
 * \param [in] state FP software state.
 * \param [in] pool Meter pool id.
 * \param [in] oper_mode FP operating mode (global/unique).
 * \param [in] lt_entry Meter Logical table entry.
 * \param [out] index Hardware meter index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FULL Hardware meter table is full
 */
extern int
bcmcth_meter_util_hw_index_alloc(int unit,
                                 bcmcth_meter_fp_attrib_t *attr,
                                 bcmcth_meter_fp_sw_state_t *state,
                                 int pool,
                                 uint8_t oper_mode,
                                 bcmcth_meter_sw_params_t *lt_entry,
                                 uint32_t *index);

/*!
 * \brief Get meter operating values
 *
 * Calculate the operational meter rate and burst size of a meter
 * from hardware entry.
 *
 * \param [in] unit Unit number.
 * \param [in] attr FP attributes.
 * \param [in] state FP software state.
 * \param [in] lt_entry Meter Logical table entry.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_meter_util_hw_oper_get(int unit,
                              bcmcth_meter_fp_attrib_t *attr,
                              bcmcth_meter_fp_sw_state_t *state,
                              bcmcth_meter_sw_params_t *lt_entry);

/*!
 * \brief Get meter hardware parameters.
 *
 * Get hardware parameters of this meter entry.
 *
 * \param [in] unit Unit number.
 * \param [in] attr FP attributes.
 * \param [in] state FP software state.
 * \param [in] lt_entry Meter Logical table entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid meter parameter.
 */
extern int
bcmcth_meter_util_convert_to_hw_params(int unit,
                                       bcmcth_meter_fp_attrib_t *attr,
                                       bcmcth_meter_fp_sw_state_t *state,
                                       bcmcth_meter_sw_params_t *lt_entry);

/*!
 * \brief Configure FP meters.
 *
 * Initialize FP meter related device configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] attr FP attributes.
 * \param [in] refresh_en Refresh enable status.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_meter_util_hw_config(int unit,
                            bcmcth_meter_fp_attrib_t *attr,
                            uint32_t refresh_en);

/*!
 * \brief Get meter entry format in policy table
 *
 * Get hardware format of this meter entry.
 *
 * \param [in] unit Unit number.
 * \param [in] attr FP attributes.
 * \param [in] state FP software state.
 * \param [in] lt_entry Meter Logical table entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid meter parameter.
 */
extern int
bcmcth_meter_util_policy_format_get(int unit,
                                    bcmcth_meter_fp_attrib_t *attr,
                                    bcmcth_meter_fp_sw_state_t *state,
                                    bcmcth_meter_sw_params_t *lt_entry);

#endif /* BCMCTH_METER_FP_COMMON_H */
