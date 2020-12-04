/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: qax_ports.h
 */

#ifndef __QAX_PORTS_INCLUDED__

#define __QAX_PORTS_INCLUDED__
#include <soc/dpp/TMC/tmc_api_ports.h>
#include <soc/dcmn/error.h>

int
soc_qax_port_nrdy_th_optimal_value_get(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_port_t port, 
    SOC_SAND_OUT uint32 *value);

soc_error_t 
soc_qax_port_swap_global_info_set(int unit, SOC_TMC_PORT_SWAP_GLOBAL_INFO *ports_swap_info);

soc_error_t 
soc_qax_port_swap_global_info_get(int unit, SOC_TMC_PORT_SWAP_GLOBAL_INFO *ports_swap_info);

int
soc_qax_port_ilkn_nif_port_get(int unit, uint32 ilkn_intf_offset, uint32* nif_port);

int 
soc_qax_port_to_interface_egress_map_set(int unit, soc_port_t port);

#endif 
