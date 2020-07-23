/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: jer_sch.h
 */

#ifndef __JER_PORTS_INCLUDED__

#define __JER_PORTS_INCLUDED__

#include <soc/dpp/TMC/tmc_api_ports.h>
#include <soc/dcmn/error.h>

int
  soc_jer_port_to_interface_map_set(
    int                   unit,
    soc_port_t            port,
    int                   unmap,
    int                   is_dynamic_port
   );

int
  soc_jer_port_to_interface_egress_map_set(
    int                   unit,
    soc_port_t            port
   );

uint32
  soc_jer_port_ingr_reassembly_context_get(
      SOC_SAND_IN int         unit,
      SOC_SAND_IN soc_port_t  port,
      SOC_SAND_OUT uint32     *port_termination_context,
      SOC_SAND_OUT uint32     *reassembly_context
  );

uint32 
  soc_jer_port_rate_egress_pps_set (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_port_t port, 
    SOC_SAND_IN uint32 pps, 
    SOC_SAND_IN uint32 burst
   );

uint32 
  soc_jer_port_rate_egress_pps_get (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_port_t port, 
    SOC_SAND_OUT uint32 *pps, 
    SOC_SAND_OUT uint32 *burst
    );
int
  soc_jer_port_to_interface_ingress_map_set(
      int                   unit,
      soc_port_t            port,
      int                   is_dynamic_port
   );

int
  soc_jer_port_to_interface_ingress_unmap_set(
      int                   unit,
      soc_port_t            port
   );

uint32
 soc_jer_port_synce_clk_sel_get(
                            int                         unit,
                            uint32                      synce_cfg_num,
                            soc_port_t                  *port);

uint32
 soc_jer_port_synce_clk_sel_set(
                            int                         unit,
                            uint32                      synce_cfg_num,
                            soc_port_t                  port);

int
  soc_jer_port_reference_clock_set(
      int                   unit,
      soc_port_t            port
   );

uint32
  soc_jer_port_header_type_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core_id,
    SOC_SAND_IN  uint32                 port,
    SOC_SAND_IN  ARAD_PORT_DIRECTION    direction_ndx,
    SOC_SAND_IN  ARAD_PORT_HEADER_TYPE  header_type
  );

int
soc_jer_port_control_tx_nif_enable_set(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_IN  int        enable);

int
soc_jer_port_control_tx_nif_enable_get(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_OUT int        *enable);

int
soc_jer_port_nrdy_th_profile_set(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_port_t port, 
    SOC_SAND_IN uint32 profile);

int
soc_jer_port_nrdy_th_profile_get(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_port_t port, 
    SOC_SAND_OUT uint32 *profile);

int
soc_jer_port_nrdy_th_optimal_value_get(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_port_t port, 
    SOC_SAND_OUT uint32 *value);

int
soc_jer_port_mirrored_channel_and_context_map(int unit, 
                                              int core, 
                                              uint32 termination_context,
                                              uint32 reassembly_context, 
                                              uint32 channel);
											  

soc_error_t 
soc_jer_port_swap_global_info_set(int unit, SOC_TMC_PORT_SWAP_GLOBAL_INFO *ports_swap_info);

soc_error_t 
soc_jer_port_swap_global_info_get(int unit, SOC_TMC_PORT_SWAP_GLOBAL_INFO *ports_swap_info);

soc_error_t 
soc_jer_port_fabric_detach(int unit, soc_port_t port);

soc_error_t
soc_jer_port_first_link_in_fsrd_get(int unit, soc_port_t port, int *is_first_link, int enable);

soc_error_t
soc_jer_port_update_fsrd_block(int unit, soc_port_t port, int enable);

soc_error_t soc_jer_port_timestamp_and_ssp_update(int   unit,soc_port_t   port);
soc_error_t soc_jer_port_oam_ts_init(int unit,soc_port_t   port);
int
soc_jer_port_ilkn_nif_port_get(int unit, uint32 ilkn_intf_offset, uint32* nif_port);

int
soc_jer_port_ingress_enable_get(
    int                   unit,
    soc_port_t            port,
    int                   *enable);

int
soc_jer_port_ingress_enable_set(
    int                   unit,
    soc_port_t            port,
    int                   enable);

#endif 

