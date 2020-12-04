/*! \file ser_lt_ser_pt_status.h
 *
 * Interface functions for SER_PT_STATUS IMM LT.
 *
 * Used to Update data to SER_PT_STATUS IMM LT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SER_LT_SER_PT_STATUS_H
#define SER_LT_SER_PT_STATUS_H

#include "bcmdrd/bcmdrd_types.h"

/*!
 * \brief Insert a entry to SER_PT_STATUS for every PT.
 * Initialize default data to IMM LT SER_PT_STATUS.
 *
 * \param [in] unit           Unit number.
 * \param [in] sid            PT ID.
 *
 * \retval SHR_E_NONE for Success
 */
extern int
bcmptm_ser_pt_status_imm_insert(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Update latest data from cache to IMM LT SER_PT_STATUS.
 *
 * \param [in] unit           Unit number.
 * \param [in] sid            PT ID.
 * \param [in] fid_lt         Field ID of SER_PT_STATUS, except key field ID.
 * \param [in] data          Field data.
 *
 * \retval SHR_E_NONE for Success
 */
extern int
bcmptm_ser_pt_status_imm_update(int unit, bcmdrd_sid_t pt_id,
                                uint32_t fid_lt, uint32_t data);

/*!
 * \brief Register IMM callback routines for LT SER_PT_STATUS.
 *
 * \param [in] unit           Unit number.
 *
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_pt_status_callback_register(int unit);

/*!
 * \brief Get a field data from LT SER_PT_STATUS.
 *
 *
 * \param [in] unit           Unit number.
 * \param [in] fid_lt         Field ID.
 *
 * \retval Field data.
 */
extern uint32_t
bcmptm_ser_pt_status_field_val_get(int unit, bcmdrd_sid_t pt_id, uint32_t fid_lt);

#endif /* SER_LT_SER_PT_STATUS_H */

