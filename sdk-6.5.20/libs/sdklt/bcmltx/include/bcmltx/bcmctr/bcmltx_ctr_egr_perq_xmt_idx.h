/*! \file bcmltx_ctr_egr_perq_xmt_idx.h
 *
 * EGR_PERQ_XMT_COUNTERS.__INDEX/__INSTANCE transform handler
 *
 * This file contains field transform information for
 * EGR_PERQ_XMT_COUNTERS.__INDEX/__INSTANCE.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_CTR_EGR_PERQ_XMT_IDX_H
#define BCMLTX_CTR_EGR_PERQ_XMT_IDX_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief EGR_PERQ_XMT_COUNTERS.__INDEX/__INSTANCE forward transform
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  in            EGR_PERQ_XMT_COUNTERS.PORT_ID/MC_Q(or UC_Q).
 * \param [out] out           EGR_PERQ_XMT_COUNTERS.__INDEX/__INSTANCE.
 * \param [in]  arg           Transform arguments.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmltx_ctr_egr_perq_xmt_idx_transform(int unit,
                                      const bcmltd_fields_t *in,
                                      bcmltd_fields_t *out,
                                      const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_CTR_EGR_PERQ_XMT_IDX_H */
