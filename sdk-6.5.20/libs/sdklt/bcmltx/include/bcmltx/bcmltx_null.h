/*! \file bcmltx_null.h
 *
 * NULL CTH handler definitions.
 *
 * Built-in Custom Table Handler which does nothing but return
 * the unavailable error code.  This handler is a default for
 * testing purposes.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_NULL_H
#define BCMLTX_NULL_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Null table validation
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  opcode        LT opcode.
 * \param [out] in            Input field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmltx_null_validate(int unit,
                     bcmlt_opcode_t opcode,
                     const bcmltd_fields_t *in,
                     const bcmltd_generic_arg_t *arg);

/*!
 * \brief Null table insert
 *
 * \param [in]  unit            Unit number.
 * \param [in]  op_arg          Operation arguments.
 * \param [in]  in              LTD input field set.
 * \param [out] out             LTD output field set.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE           No errors.
 * \retval !SHR_E_NONE          Failure.
 */
extern int
bcmltx_null_insert(int unit,
                   const bcmltd_op_arg_t *op_arg,
                   const bcmltd_fields_t *in,
                   bcmltd_fields_t *out,
                   const bcmltd_generic_arg_t *arg);

/*!
 * \brief Null table lookup
 *
 * \param [in]  unit            Unit number.
 * \param [in]  op_arg          Operation arguments.
 * \param [in]  in              LTD input field set.
 * \param [out] out             LTD output field set.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE           No errors.
 * \retval !SHR_E_NONE          Failure.
 */
extern int
bcmltx_null_lookup(int unit,
                   const bcmltd_op_arg_t *op_arg,
                   const bcmltd_fields_t *in,
                   bcmltd_fields_t *out,
                   const bcmltd_generic_arg_t *arg);

/*!
 * \brief Null table delete
 *
 * \param [in]  unit            Unit number.
 * \param [in]  op_arg          Operation arguments.
 * \param [in]  in              LTD input field set.
 * \param [out] out             LTD output field set.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE           No errors.
 * \retval !SHR_E_NONE          Failure.
 */
extern int
bcmltx_null_delete(int unit,
                   const bcmltd_op_arg_t *op_arg,
                   const bcmltd_fields_t *in,
                   bcmltd_fields_t *out,
                   const bcmltd_generic_arg_t *arg);

/*!
 * \brief Null table update
 *
 * \param [in]  unit            Unit number.
 * \param [in]  op_arg          Operation arguments.
 * \param [in]  in              LTD input field set.
 * \param [out] out             LTD output field set.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE           No errors.
 * \retval !SHR_E_NONE          Failure.
 */
extern int
bcmltx_null_update(int unit,
                   const bcmltd_op_arg_t *op_arg,
                   const bcmltd_fields_t *in,
                   bcmltd_fields_t *out,
                   const bcmltd_generic_arg_t *arg);

/*!
 * \brief Null table first
 *
 * \param [in]  unit            Unit number.
 * \param [in]  op_arg          Operation arguments.
 * \param [in]  in              LTD input field set.
 * \param [out] out             LTD output field set.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE           No errors.
 * \retval !SHR_E_NONE          Failure.
 */
extern int
bcmltx_null_first(int unit,
                  const bcmltd_op_arg_t *op_arg,
                  const bcmltd_fields_t *in,
                  bcmltd_fields_t *out,
                  const bcmltd_generic_arg_t *arg);

/*!
 * \brief Null table next
 *
 * \param [in]  unit            Unit number.
 * \param [in]  op_arg          Operation arguments.
 * \param [in]  in              LTD input field set.
 * \param [out] out             LTD output field set.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE           No errors.
 * \retval !SHR_E_NONE          Failure.
 */
extern int
bcmltx_null_next(int unit,
                 const bcmltd_op_arg_t *op_arg,
                 const bcmltd_fields_t *in,
                 bcmltd_fields_t *out,
                 const bcmltd_generic_arg_t *arg);

#endif /* BCMLTX_NULL_H */
