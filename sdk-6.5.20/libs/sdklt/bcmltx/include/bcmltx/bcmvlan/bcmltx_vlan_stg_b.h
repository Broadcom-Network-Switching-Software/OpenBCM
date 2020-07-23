/*! \file bcmltx_vlan_stg_b.h
 *
 * STG Transform Handler header file.
 * This file contains field transform information for STG.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_VLAN_STG_B_H
#define BCMLTX_VLAN_STG_B_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief stg transform
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
bcmltx_vlan_stg_b_transform(int unit,
                            const bcmltd_fields_t *in,
                            bcmltd_fields_t *out,
                            const bcmltd_transform_arg_t *arg);
#endif /* BCMLTX_VLAN_STG_B_H */
