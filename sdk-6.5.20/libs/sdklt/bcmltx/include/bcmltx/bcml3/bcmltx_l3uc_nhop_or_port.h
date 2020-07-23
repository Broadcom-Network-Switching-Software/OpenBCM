/*! \file bcmltx_l3uc_nhop_or_port.h
 *
 * ING_UNDERLAY_NHOP/IS_TRUNK/MODPORT/TRUNK_ID Transform Handler
 * This file contains field transform information for
 * L3_UC_NHOP fields.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_L3UC_NHOP_OR_PORT_H
#define BCMLTX_L3UC_NHOP_OR_PORT_H

#include <bcmltd/bcmltd_handler.h>


/*!
 * \brief DST_MAC/CLASS_ID transform
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
bcmltx_l3uc_nhop_or_port_transform(int unit,
                                   const bcmltd_fields_t *in,
                                   bcmltd_fields_t *out,
                                   const bcmltd_transform_arg_t *arg);


/*!
 * \brief DST_MAC/CLASS_ID reverse transform
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
bcmltx_l3uc_nhop_or_port_rev_transform(int unit,
                                       const bcmltd_fields_t *in,
                                       bcmltd_fields_t *out,
                                       const bcmltd_transform_arg_t *arg);


#endif /* BCMLTX_L3UC_NHOP_OR_PORT_H */

