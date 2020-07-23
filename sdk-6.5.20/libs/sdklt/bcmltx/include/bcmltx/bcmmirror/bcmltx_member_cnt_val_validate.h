/*! \file bcmltx_member_cnt_val_validate.h
 *
 * Mirror Member Counter validator.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_MEMBER_CNT_VAL_VALIDATE_H
#define BCMLTX_MEMBER_CNT_VAL_VALIDATE_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Sample Field Validation Function.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  opcode          Table opcode.
 * \param [in]  in              LTD input field set.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltx_member_cnt_val_validate(int unit,
                               bcmlt_opcode_t opcode,
                               const bcmltd_fields_t *in,
                               const bcmltd_field_val_arg_t *arg);

#endif /* BCMLTX_MEMBER_CNT_VAL_VALIDATE_H */
