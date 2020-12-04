/*! \file bcmltx_uc_q_validate.h
 *
 * Validates number of multicast queues (cos) available for the system.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_UC_Q_VALIDATE_H
#define BCMLTX_UC_Q_VALIDATE_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief UC COS validation
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  opcode        LT opcode.
 * \param [out] in            Input field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM UC_COS exceeds number of unicast queue.
 */
extern int
bcmltx_uc_q_validate(int unit,
                     bcmlt_opcode_t opcode,
                     const bcmltd_fields_t *in,
                     const bcmltd_field_val_arg_t *arg);

#endif /* BCMLTX_UC_Q_VALIDATE_H */
