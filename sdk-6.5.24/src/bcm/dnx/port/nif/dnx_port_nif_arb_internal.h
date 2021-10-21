/** \file dnx_port_nif_arb_internal.h
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifndef _PORT_NIF_ARB_INTERNAL_H_
#define _PORT_NIF_ARB_INTERNAL_H_

#include <bcm/types.h>
#include <shared/shrextend/shrextend_error.h>
#include <bcm_int/dnx/port/nif/dnx_port_nif_arb.h>

typedef enum
{
    DNX_PORT_ARB_MAP_DESTINATION_MACSEC_0 = 0,

    DNX_PORT_ARB_MAP_DESTINATION_MACSEC_1 = 1,

    DNX_PORT_ARB_MAP_DESTINATION_BYPASS = 2,

    DNX_PORT_ARB_MAP_DESTINATION_NOF
} dnx_port_map_destination_e;

/**
 * \brief - Return the Corresponding Tx context (eth or tmac)
 */
shr_error_e dnx_port_arb_tx_context_get(
    int unit,
    int logical_port,
    int *arb_context);

/**
 * \brief - Read mapping destination from hw for given logical port
 */
shr_error_e dnx_port_arb_map_destination_get(
    int unit,
    bcm_port_t logical_port,
    dnx_port_map_destination_e * mapping_type);

/**
 * \brief - Configure mac mode config for given logical port
 */
shr_error_e dnx_port_arb_port_mac_mode_config_set(
    int unit,
    bcm_port_t logical_port,
    uint32 mac_mode);

/**
 * \brief - Calculate the required sections for a link list with a given speed
 */
shr_error_e dnx_port_arb_link_list_sections_required_get(
    int unit,
    int speed,
    int *nof_sections);

/**
 * \brief - Calculate the new mac mode for a given logical port
 */
shr_error_e dnx_port_arb_mac_mode_get(
    int unit,
    bcm_port_t logical_port,
    int *mac_mode);

/**
 * \brief - Count the number of mac ports for given logical port
 */
shr_error_e dnx_port_arb_mac_mode_config_nof_ports_get(
    int unit,
    bcm_port_t logical_port,
    int *mac_mode_config_nof_ports);

#endif /* _PORT_NIF_ARB_INTERNAL_H_ */
