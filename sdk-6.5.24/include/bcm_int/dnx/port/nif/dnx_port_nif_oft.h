/**
 *
 * \file dnx_port_nif_oft.h
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 * NIF OFT procedures for DNX.
 */

#ifndef _NIF_OFT_H_
#define _NIF_OFT_H_

#include <bcm/types.h>

shr_error_e dnx_port_oft_init(
    int unit);

shr_error_e dnx_port_oft_power_down(
    int unit);

shr_error_e dnx_port_oft_port_add(
    int unit,
    bcm_port_t logical_port);

shr_error_e dnx_port_oft_port_remove(
    int unit,
    bcm_port_t logical_port);

shr_error_e dnx_port_oft_link_list_set(
    int unit,
    bcm_port_t logical_port,
    int enable);

shr_error_e dnx_port_oft_calendar_set(
    int unit);

shr_error_e dnx_port_oft_port_enable_set(
    int unit,
    bcm_port_t logical_port,
    int enable);

#endif /* _NIF_OFT_H_ */
