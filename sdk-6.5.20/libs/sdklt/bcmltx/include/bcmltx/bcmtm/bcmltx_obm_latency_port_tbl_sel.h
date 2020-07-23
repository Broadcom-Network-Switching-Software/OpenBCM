/*! \file bcmltx_obm_latency_port_tbl_sel.h
 *
 * Table selection, instance and index calculation for OBM latency mode tables.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_OBM_LATENCY_PORT_TBL_SEL_H
#define BCMLTX_OBM_LATENCY_PORT_TBL_SEL_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief OBM latency mode table index and selection transform.
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
bcmltx_obm_latency_port_tbl_sel_transform(int unit,
                                          const bcmltd_fields_t *in,
                                          bcmltd_fields_t *out,
                                          const bcmltd_transform_arg_t *arg);

/*!
 * \brief OBM latency mode table index and selection reverse transform.
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
bcmltx_obm_latency_port_tbl_sel_rev_transform(int unit,
                                              const bcmltd_fields_t *in,
                                              bcmltd_fields_t *out,
                                              const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_OBM_LATENCY_PORT_TBL_SEL_H */
