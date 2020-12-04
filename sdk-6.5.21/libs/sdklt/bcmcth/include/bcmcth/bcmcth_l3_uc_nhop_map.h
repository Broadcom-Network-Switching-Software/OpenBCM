/*! \file bcmcth_l3_uc_nhop_map.h
 *
 * L3_UC_NHOP_MAP Transform Handler
 *
 * This file contains field transform information
 * for L3_UC_NHOP.L3_EIF_ID/VALID fields.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_L3_UC_NHOP_MAP_H
#define BCMCTH_L3_UC_NHOP_MAP_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief L3_UC_NHOP_MAP transform
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
bcmcth_l3_uc_nhop_map_transform(int unit,
                                const bcmltd_fields_t *key,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg);


/*!
 * \brief L3_UC_NHOP_MAP reverse transform
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
bcmcth_l3_uc_nhop_map_rev_transform(int unit,
                                    const bcmltd_fields_t *key,
                                    const bcmltd_fields_t *in,
                                    bcmltd_fields_t *out,
                                    const bcmltd_transform_arg_t *arg);

#endif /* BCMCTH_L3_UC_NHOP_MAP_H */
