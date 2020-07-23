/*! \file bcmltx_l3mc_mtu_index.h
 *
 * L3 MC MTU Transform Handler
 *
 * This file contains field transform information for L3 MC MTU Index field.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_L3MC_MTU_INDEX_H
#define BCMLTX_L3MC_MTU_INDEX_H

#include <bcmltd/bcmltd_handler.h>

/*! MTU offset for L3 MC. */
#define L3_MC_MTU_OFFSET  8192

/*!
 * \brief MTU INDEX transform
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
bcmltx_l3mc_mtu_index_transform(int unit,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out,
                                        const bcmltd_transform_arg_t *arg);


/*!
 * \brief MTU INDEX reverse transform
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
bcmltx_l3mc_mtu_index_rev_transform(int unit,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out,
                                        const bcmltd_transform_arg_t *arg);


#endif /* BCMLTX_L3MC_MTU_INDEX_H */
