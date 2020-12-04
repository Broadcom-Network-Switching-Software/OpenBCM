/*! \file bcmcth_tnl_decap.h
 *
 * TNL_MPLS_DECAP Custom Handler header file
 * This file contains APIs for MPLS decap
 * handler functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_TNL_DECAP_H
#define BCMCTH_TNL_DECAP_H

/*******************************************************************************
  Includes
 */
#include <bcmltd/bcmltd_handler.h>
#include <bcmdrd/bcmdrd_types.h>

/*!
 * \brief MPLS decap table validation
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  opcode        LT opcode.
 * \param [out] in            Input field values.
 * \param [in]  arg           Handler arguments.
 *
 * \retval SHR_E_NONE         OK.
 * \retval !SHR_E_NONE        ERROR.
 */
extern int
bcmcth_tnl_decap_validate(int unit,
                          bcmlt_opcode_t opcode,
                          const bcmltd_fields_t *in,
                          const bcmltd_generic_arg_t *arg);

/*!
 * \brief MPLS decap Entry insert.
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
bcmcth_tnl_decap_insert(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out,
                        const bcmltd_generic_arg_t *arg);

/*!
 * \brief MPLS decap Entry table lookup.
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
bcmcth_tnl_decap_lookup(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out,
                        const bcmltd_generic_arg_t *arg);

/*!
 * \brief MPLS decap Entry table delete.
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
bcmcth_tnl_decap_delete(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out,
                        const bcmltd_generic_arg_t *arg);

/*!
 * \brief MPLS decap Entry table update.
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
bcmcth_tnl_decap_update(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out,
                        const bcmltd_generic_arg_t *arg);

/*!
 * \brief TNL_MPLS_DECAP get first entry function.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values (not applicable).
 * \param [out] out           Output field values.
 * \param [in]  arg           Handler arguments.
 *
 * Fetch the first entry for IPV4 tunnels.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmcth_tnl_decap_first(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       const bcmltd_fields_t *in,
                       bcmltd_fields_t *out,
                       const bcmltd_generic_arg_t *arg);

/*!
 * \brief TNL_MPLS_DECAP get next entry function.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  op_arg        Operation arguments.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Handler arguments.
 *
 * Fetch the next entry for IPV4 tunnels.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmcth_tnl_decap_next(int unit,
                      const bcmltd_op_arg_t *op_arg,
                      const bcmltd_fields_t *in,
                      bcmltd_fields_t *out,
                      const bcmltd_generic_arg_t *arg);

#endif /* BCMCTH_TNL_DECAP_H */
