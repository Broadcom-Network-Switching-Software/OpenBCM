/** \file dnx_port_nif_oft_internal.h
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifndef _PORT_NIF_OFT_INTERNAL_H_
#define _PORT_NIF_OFT_INTERNAL_H_

#include <bcm/types.h>
#include <shared/shrextend/shrextend_error.h>

/**
 * \brief - Configure OFT credits counter reset
 */
shr_error_e dnx_port_oft_credits_counter_reset(
    int unit,
    bcm_port_t logical_port,
    int enable);

/**
 * \brief - Configure OFT port reset for given logical port
 */
shr_error_e dnx_port_oft_port_reset(
    int unit,
    bcm_port_t logical_port,
    int enable);

/**
 * \brief - Configure OFT EOB counter reset
 */
shr_error_e dnx_port_oft_eob_counter_reset(
    int unit,
    bcm_port_t logical_port,
    int enable);

/**
 * \brief - Configure OFT fifo counter reset
 */
shr_error_e dnx_port_oft_fifo_counter_reset(
    int unit,
    bcm_port_t logical_port,
    int enable);

/**
 * \brief - Configure OFT pack for given logical port
 */
shr_error_e dnx_port_oft_pack_set(
    int unit,
    bcm_port_t logical_port,
    int enable);

/**
 * \brief - Calculate the required sections for a link list with a given speed
 */
shr_error_e dnx_port_oft_link_list_sections_required_get(
    int unit,
    int speed,
    int *nof_sections);

#endif /* _PORT_NIF_OFT_INTERNAL_H_ */
