/*! \file ser_lt_ser_injection.h
 *
 * Interface functions for SER_INJECTION IMM LT
 *
 * Use functions to get data from cache of SER_INJECTION
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SER_LT_SER_INJECTION_H
#define SER_LT_SER_INJECTION_H

#include <bcmltd/chip/bcmltd_id.h>

/*!
  *\brief Used to determine whether to
  *insert single bit error or insert double bit error.
  */
typedef enum ser_error_bit_num_s {
    SER_SINGLE_BIT_ERR = 0,
    SER_DOUBLE_BIT_ERR = 1
} ser_error_bit_num_t;

/*!
  *\brief Used to determine whether an validation is necessary.
  */
typedef enum ser_validate_type_s {
    SER_VALIDATION = 0,
    SER_NO_VALIDATION = 1
} ser_validate_type_t;

/*!
 * \brief Initialize IMM LT SER_INJECTION.
 *
 * Called during Coldboot.
 *
 * \param [in] unit           Unit number.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_injection_imm_init(int unit);

/*!
 * \brief Register IMM callback routine for LT SER_INJECTION.
 *
 * \param [in] unit           Unit number.
 *
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ser_injection_callback_register(int unit);

#endif /* SER_LT_SER_INJECTION_H */
