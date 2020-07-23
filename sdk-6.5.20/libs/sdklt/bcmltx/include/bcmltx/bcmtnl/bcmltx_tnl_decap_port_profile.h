/*! \file bcmltx_tnl_decap_port_profile.h
 *
 * ECN wred table's index Transform Handler
 *
 * This file contains field transform information for ECN.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_TNL_DECAP_PORT_PROFILE_H
#define BCMLTX_TNL_DECAP_PORT_PROFILE_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief TNL decap port transform
 *
 * This function transforms DECAP_PORT list
 * into portbitmap.
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
bcmltx_tnl_decap_port_profile_transform(int unit,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out,
                                        const bcmltd_transform_arg_t *arg);

/*!
 * \brief TNL decap port rev transform
 *
 * This function transforms DECAP port pbmp
 * into port list.
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
bcmltx_tnl_decap_port_profile_rev_transform(int unit,
                                            const bcmltd_fields_t *in,
                                            bcmltd_fields_t *out,
                                            const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_TNL_DECAP_PORT_PROFILE_H */
