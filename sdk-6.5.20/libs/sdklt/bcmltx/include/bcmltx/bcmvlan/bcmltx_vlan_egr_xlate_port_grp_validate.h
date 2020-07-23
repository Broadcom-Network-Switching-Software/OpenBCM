/*! \file bcmltx_vlan_egr_xlate_port_grp_validate.h
 *
 * VLAN egress Xlate Validation Handler.
 *
 * This file contains handler to validate vlan egress xlate.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_VLAN_EGR_XLATE_PORT_GRP_VALIDATE_H
#define BCMLTX_VLAN_EGR_XLATE_PORT_GRP_VALIDATE_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Vlan Egress Xlate validation
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_PARAM        Failure.
 */
extern int
bcmltx_vlan_egr_xlate_port_grp_validate(int unit,
                                        bcmlt_opcode_t opcode,
                                        const bcmltd_fields_t *in,
                                        const bcmltd_field_val_arg_t *arg);

#endif /* BCMLTX_VLAN_EGR_XLATE_PORT_GRP_VALIDATE_H */
