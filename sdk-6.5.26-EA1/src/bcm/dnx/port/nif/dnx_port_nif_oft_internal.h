/** \file dnx_port_nif_oft_internal.h
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _PORT_NIF_OFT_INTERNAL_H_
#define _PORT_NIF_OFT_INTERNAL_H_

#include <bcm/types.h>
#include <soc/sand/shrextend/shrextend_error.h>

/**
 * \brief - Configure OFT destination credit
 *
 */
shr_error_e dnx_port_oft_destination_credits_bitmap_set(
    int unit,
    bcm_port_t logical_port);

/**
 * \brief - Configure reset/out of reset to all OFT counters and the port itself
 *
 */
shr_error_e dnx_port_oft_reset_counters_set(
    int unit,
    bcm_port_t logical_port,
    int in_reset);

/**
 * \brief - Configure OFT power down
 *
 */
shr_error_e dnx_port_oft_power_down_set(
    int unit,
    int enable);

/**
 * \brief - Configure OFT scheduler reset
 *
 */
shr_error_e dnx_port_oft_scheduler_reset_set(
    int unit,
    int enable);

/**
 * \brief - Configure OFT scheduler TDM counter enable
 *
 */
shr_error_e dnx_port_oft_scheduler_tdm_counter_enable_set(
    int unit);

/**
 * \brief - Configure OFT set frame preemption
 *
 */
shr_error_e dnx_port_oft_frame_preemption_set(
    int unit,
    int enable);

/**
 * \brief - Configure OFT credits counter reset
 *
 */
shr_error_e dnx_port_oft_credits_counter_reset(
    int unit,
    bcm_port_t logical_port,
    int in_reset);

/**
 * \brief - Configure OFT port reset for given logical port
 *
 */
shr_error_e dnx_port_oft_port_reset(
    int unit,
    bcm_port_t logical_port,
    int in_reset);

/**
 * \brief - Configure OFT EOB counter reset
 *
 */
shr_error_e dnx_port_oft_eob_counter_reset(
    int unit,
    bcm_port_t logical_port,
    int in_reset);

/**
 * \brief - Configure OFT fifo counter reset
 *
 */
shr_error_e dnx_port_oft_fifo_counter_reset(
    int unit,
    bcm_port_t logical_port,
    int in_reset);

/**
 * \brief - Configure OFT pack for given logical port
 *
 */
shr_error_e dnx_port_oft_pack_set(
    int unit,
    bcm_port_t logical_port,
    int in_reset);

/**
 * \brief - Calculate the required sections for a link list with a given speed
 *
 */
shr_error_e dnx_port_oft_link_list_sections_required_get(
    int unit,
    int speed,
    int *nof_sections);

/**
 * \brief - Init oft link list memories before allocation
 *
 */
shr_error_e dnx_port_oft_link_list_pre_alloc_init(
    int unit,
    int first_section,
    int oft_context);

/**
 * \brief - Enable OFT dynamic memories
 *
 */
shr_error_e dnx_port_oft_enable_dynamic_memories_set(
    int unit,
    int enable);

/**
 * \brief - Start Tx threshold
 *
 */
shr_error_e dnx_port_nif_oft_tx_start_thr_hw_set(
    int unit,
    bcm_port_t logical_port,
    int start_thr);

#endif /* _PORT_NIF_OFT_INTERNAL_H_ */
