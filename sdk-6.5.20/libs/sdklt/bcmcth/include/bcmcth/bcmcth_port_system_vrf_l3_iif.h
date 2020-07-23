/*! \file bcmcth_port_system_vrf_l3_iif.h
 *
 * VRF_ID/L3_IIF_ID Transform Handler
 *
 * This file contains field transform information
 * for PORT_SYSTEM.VRF_ID/L3_IIF_ID field.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_PORT_SYSTEM_VRF_L3_IIF_H
#define BCMCTH_PORT_SYSTEM_VRF_L3_IIF_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief VRF_ID/L3_IIF_ID transform
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  key           Input key values.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_port_system_vrf_l3_iif_transform(int unit,
                                        const bcmltd_fields_t *key,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out,
                                        const bcmltd_transform_arg_t *arg);


/*!
 * \brief VRF_ID/L3_IIF_ID reverse transform
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  key           Input key values.
 * \param [in]  in            Input field values.
 * \param [out] out           Output field values.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmcth_port_system_vrf_l3_iif_rev_transform(int unit,
                                            const bcmltd_fields_t *key,
                                            const bcmltd_fields_t *in,
                                            bcmltd_fields_t *out,
                                            const bcmltd_transform_arg_t *arg);


#endif /* BCMCTH_PORT_SYSTEM_VRF_L3_IIF_H */
