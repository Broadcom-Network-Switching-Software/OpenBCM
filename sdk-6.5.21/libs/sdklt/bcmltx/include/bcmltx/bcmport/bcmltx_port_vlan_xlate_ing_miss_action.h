/*! \file bcmltx_port_vlan_xlate_ing_miss_action.h
 *
 * PORT_VLAN_XLATE.ING_MISS_ACTION Transform Handler
 *
 * This file contains field transform information for PORT_VLAN_XLATE.ING_MISS_ACTION.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_PORT_VLAN_XLATE_ING_MISS_ACTION_H
#define BCMLTX_PORT_VLAN_XLATE_ING_MISS_ACTION_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief PORT_VLAN_XLATE.ING_MISS_ACTION transform
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
bcmltx_port_vlan_xlate_ing_miss_action_transform(
    int unit, const bcmltd_fields_t *in,
    bcmltd_fields_t *out, const bcmltd_transform_arg_t *arg);

/*!
 * \brief PORT_VLAN_XLATE.ING_MISS_ACTION rev transform
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
bcmltx_port_vlan_xlate_ing_miss_action_rev_transform(
    int unit, const bcmltd_fields_t *in,
    bcmltd_fields_t *out, const bcmltd_transform_arg_t *arg);

#endif /* BCMLTX_PORT_VLAN_XLATE_ING_MISS_ACTION_H */
