/*! \file ser_lt_ser_pt_control.h
 *
 * Interface functions for IMM SER_PT_CONTROL
 *
 * Get data from cache of SER_PT_CONTROL or
 * update data to SER_PT_CONTROL.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SER_LT_SER_PT_CONTROL_H
#define SER_LT_SER_PT_CONTROL_H

/*!
 * \brief Allocate and initialize resources for SER_PT_CONTROL.
 *
 * \param [in] unit           Unit number.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_pt_control_cache_init(int unit);

/*!
 * \brief Free resources for SER_PT_CONTROL.
 *
 * \param [in] unit           Unit number.
 *
 * \retval SHR_E_NONE Success
 */
extern void
bcmptm_ser_pt_control_cache_cleanup(int unit);

/*!
 * \brief Synchronize data from IMM to cache of SER_PT_CONTROL.
 *
 * Called during Warmboot.
 *
 * \param [in] unit           Unit number.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_pt_control_cache_sync(int unit);

/*!
 * \brief Register IMM callback routines for LT SER_PT_CONTROL.
 *
 * \param [in] unit           Unit number.
 *
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_pt_control_callback_register(int unit);

/*!
 * \brief For a PT, SER logic can update IMM SER_PT_CONTROL
 * to re-configure one or more parameters of a PT.
 *
 * \param [in] unit           Unit number.
 * \param [in] pt_id         physical table id.
 * \param [in] fid_lt         field id.
 * \param [in] data          field data.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_pt_control_imm_update(int unit, bcmdrd_sid_t pt_id,
                                 uint32_t fid_lt, uint32_t data);

/*!
 * \brief For every PT, SER logic needs to update default value
 * to IMM SER_PT_CONTROL.
 *
 *
 * \param [in] unit           Unit number.
 * \param [in] pt_id         Physical table id.
 * \param [in] enable       SER checking enable.
 * \param [in] enable       SER single bit enable.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_pt_control_imm_insert(int unit, bcmdrd_sid_t pt_id,
                                 int enable, int enable_1bit);

/*!
 * \brief Get a field data from cache of LT SER_PT_CONTROL.
 *
 *
 * \param [in] unit           Unit number.
 * \param [in] fid_lt         Field ID.
 *
 * \retval Field data.
 */
extern uint32_t
bcmptm_ser_pt_control_field_val_get(int unit, bcmdrd_sid_t pt_id, uint32_t fid_lt);

#endif /* SER_LT_SER_PT_CONTROL_H */
