/**
 *
 * \file dnx_port_nif_oft.h
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * NIF OFT procedures for DNX.
 *
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

shr_error_e dnx_port_oft_link_list_switch(
    int unit,
    bcm_port_t logical_port);

shr_error_e dnx_port_oft_calendar_set(
    int unit);

shr_error_e dnx_port_oft_shadow_calendar_build(
    int unit);

shr_error_e dnx_port_oft_calendar_switch_active_id(
    int unit);

shr_error_e dnx_port_nif_oft_tx_start_thr_set(
    int unit,
    bcm_port_t logical_port,
    int start_thr);

shr_error_e dnx_port_oft_port_enable_set(
    int unit,
    bcm_port_t logical_port,
    int enable);

shr_error_e dnx_port_oft_port_credits_change_set(
    int unit,
    bcm_port_t logical_port,
    int old_speed);

#endif /* _NIF_OFT_H_ */
