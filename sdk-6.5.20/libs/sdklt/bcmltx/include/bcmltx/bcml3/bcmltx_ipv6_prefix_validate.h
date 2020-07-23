/*! \file bcmltx_ipv6_prefix_validate.h
 *
 * IPv6 Prefix Validation Handler
 *
 * This file contains table handler for IPV6 prefix to IPv4 map.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTX_IPV6_PREFIX_VALIDATE_H
#define BCMLTX_IPV6_PREFIX_VALIDATE_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief IPv6 Prefix validation
 *
 *
 * \retval SHR_E_NONE         No errors.
 * \retval SHR_E_PARAM        Failure.
 */
extern int
bcmltx_ipv6_prefix_validate(int unit,
                            bcmlt_opcode_t opcode,
                            const bcmltd_fields_t *in,
                            const bcmltd_field_val_arg_t *arg);

#endif /* BCMLTX_IPV6_PREFIX_VALIDATE_H */
