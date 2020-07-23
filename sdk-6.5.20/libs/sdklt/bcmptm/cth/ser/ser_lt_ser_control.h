/*! \file ser_lt_ser_control.h
 *
 * Interface functions for SER_CONTROL IMM LT
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SER_LT_SER_CONTROL_H
#define SER_LT_SER_CONTROL_H

#include <sal/sal_types.h>

/*!
 * \brief Register IMM callback routine for LT SER_CONTROL.
 *
 * \param [in] unit           Unit number.
 *
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_control_callback_register(int unit);

/*!
 * \brief Initialize SER_CONTROL by default value.
 *
 * Called during Coldboot.
 *
 * \param [in] unit           Unit number.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_control_imm_init(int unit);

/*!
 * \brief Sync data from IMM to cache of LT SER_CONTROL.
 *
 * Called during Warmboot.
 *
 * \param [in] unit           Unit number.
 *
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_control_cache_sync(int unit);

/*!
 * \brief Get field value of SER_CONTROL from cache.
 *
 *
 * \param [in] unit           Unit number.
 * \param [in] fid_lt         field id.
 *
 * \retval Field data or \ref BCMPTM_SER_INVALID_FIELD_VAL
 */
extern uint32_t
bcmptm_ser_control_field_val_get(int unit, uint32_t fid_lt);

#endif /* SER_LT_SER_CONTROL_H */
