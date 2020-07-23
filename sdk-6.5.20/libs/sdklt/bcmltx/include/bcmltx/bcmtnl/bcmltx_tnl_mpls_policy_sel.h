/*! \file bcmltx_tnl_mpls_policy_sel.h
 *
 * TNL_DEFAULT_POLICY.POLICY_CMD_SELECT/POLICY_OBJ_SELECT Transform Handler
 *
 * This file contains field transform information for
 * TNL_DEFAULT_POLICY.POLICY_CMD_SELECT/POLICY_OBJ_SELECT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_TNL_MPLS_POLICY_SEL_H
#define BCMLTX_TNL_MPLS_POLICY_SEL_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief TNL_DEFAULT_POLICY.POLICY_CMD_SELECT/POLICY_OBJ_SELECT transform
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmltx_tnl_mpls_policy_sel_transform(int unit,
									 const bcmltd_fields_t *in,
									 bcmltd_fields_t *out,
									 const bcmltd_transform_arg_t *arg);

/*!
 * \brief TNL_DEFAULT_POLICY.POLICY_CMD_SELECT/POLICY_OBJ_SELECT rev transform
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmltx_tnl_mpls_policy_sel_rev_transform(int unit,
                                         const bcmltd_fields_t *in,
                                         bcmltd_fields_t *out,
										 const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_TNL_MPLS_POLICY_SEL_H */
