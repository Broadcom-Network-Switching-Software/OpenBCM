/*! \file bcmltx_std_table_validator.h
 *
 * Built-in Field Validation definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_STD_TABLE_VALIDATOR_H
#define BCMLTX_STD_TABLE_VALIDATOR_H

#include <bcmltx/bcmltx_util.h>

/*!
 * \brief Standard Table Field Validation.
 *
 * Performs the Standard Field Validation which uses the table definition.
 * This performs
 * - Wide field validation
 * - Read only field validation.
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
bcmltx_std_table_validator(int unit,
                           bcmlt_opcode_t opcode,
                           const bcmltd_fields_t *in,
                           const bcmltd_field_val_arg_t *arg);

/*!
 * \brief Port bitmap field array LTA validation.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  opcode          Table opcode.
 * \param [in]  in              LTD input field set.
 * \param [in]  arg             Handler arguments.
 *
 * Generic LTA args for port bitmap validate:
 * value[0] port_space Specification of the port space in which this
 *                     port bitmap is encoded.
 * value[1] port_num   Number of ports for this bitmap.
 * value[2] fid        Field ID.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltx_lta_val_port_bitmap(int unit,
                           bcmlt_opcode_t opcode,
                           const bcmltd_fields_t *in,
                           const bcmltd_field_val_arg_t *arg);

#endif /* BCMLTX_STD_TABLE_VALIDATOR_H */
