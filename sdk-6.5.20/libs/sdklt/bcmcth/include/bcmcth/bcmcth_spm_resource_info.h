/*! \file bcmcth_spm_resource_info.h
 *
 * Strenth Profile *_RESOURCE_INFO Custom Handler header file
 * This file contains APIs for SPLT associated resource information LT
 * handler functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_SPM_RESOURCE_INFO_H
#define BCMCTH_SPM_RESOURCE_INFO_H

/*******************************************************************************
  Includes
 */
#include <bcmltd/bcmltd_handler.h>
#include <bcmdrd/bcmdrd_types.h>

/*!
 * \brief Strength profile resource info table validation
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  opcode        LT opcode.
 * \param [in]  in            Input field values.
 * \param [in]  arg           Handler arguments.
 *
 * \retval SHR_E_NONE         OK.
 * \retval !SHR_E_NONE        ERROR.
 */
extern int
bcmcth_spm_resource_info_validate(int unit,
                                  bcmlt_opcode_t opcode,
                                  const bcmltd_fields_t *in,
                                  const bcmltd_generic_arg_t *arg);

/*!
 * \brief Strength profile resource info insert.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values (not applicable).
 * \param [in]  arg           Handler arguments.
 *
 * \retval SHR_E_NONE         OK.
 * \retval !SHR_E_NONE        ERROR.
 */
extern int
bcmcth_spm_resource_info_insert(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_generic_arg_t *arg);

/*!
 * \brief Strength profile resource info table lookup.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Handler arguments.
 *
 * \retval SHR_E_NONE         OK.
 * \retval !SHR_E_NONE        ERROR.
 */
extern int
bcmcth_spm_resource_info_lookup(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_generic_arg_t *arg);

/*!
 * \brief Strength profile resource info table delete.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values (not applicable).
 * \param [in]  arg           Handler arguments.
 *
 * \retval SHR_E_NONE         OK.
 * \retval !SHR_E_NONE        ERROR.
 */
extern int
bcmcth_spm_resource_info_delete(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_generic_arg_t *arg);

/*!
 * \brief Strength profile resource info table update.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values (not applicable).
 * \param [in]  arg           Handler arguments.
 *
 * \retval SHR_E_NONE         OK.
 * \retval !SHR_E_NONE        ERROR.
 */
extern int
bcmcth_spm_resource_info_update(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_generic_arg_t *arg);

/*!
 * \brief Strength profile resource info get first entry function.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values (not applicable).
 * \param [out] out           Output field values.
 * \param [in]  arg           Handler arguments.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmcth_spm_resource_info_first(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               const bcmltd_fields_t *in,
                               bcmltd_fields_t *out,
                               const bcmltd_generic_arg_t *arg);

/*!
 * \brief Strength profile resource info get next entry function.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Handler arguments.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmcth_spm_resource_info_next(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_generic_arg_t *arg);

#endif /* BCMCTH_SPM_RESOURCE_INFO_H */
