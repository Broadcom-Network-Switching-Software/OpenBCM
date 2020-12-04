/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains structure and routine declarations for the
 * Switch-on-a-Chip Driver.
 *
 * This file also includes the more common include files so the
 * individual driver files don't have to include as much.
 */
#ifndef SOC_DPP_PORT_MAP_H
#define SOC_DPP_PORT_MAP_H

#define _SOC_DPP_PORT_INVALID 0xffffffff
#define SOC_DPP_MAX_LOCAL_PORTS_PER_DEVICE(unit) (SOC_MAX_NUM_PORTS)

typedef struct {
  uint32 channel;
  SOC_TMC_INTERFACE_ID nif_id;
  int tm_port;
  int pp_port;
  int if_rate_mbps;
} soc_dpp_port_map_t;

typedef struct {
   uint8 in_use;
} soc_dpp_port_t;

typedef struct {
    int pp_port[SOC_MAX_NUM_PORTS];
    soc_dpp_port_t pp_port_use[SOC_DPP_DEFS_MAX(NOF_LOCAL_PORTS)];
} soc_dpp_pp_port_map;

typedef struct soc_dpp_vt_profile_info_s {
    uint8 vlan_translation_profile[SOC_DPP_DEFS_MAX(NOF_VLAN_DOMAINS)];
} soc_dpp_vt_profile_info_t;

extern int _dflt_tm_pp_port_map[SOC_MAX_NUM_DEVICES];
extern soc_dpp_port_map_t _port_map[SOC_MAX_NUM_DEVICES][SOC_MAX_NUM_PORTS];
extern soc_dpp_port_t *soc_dpp_pp_ports[SOC_MAX_NUM_DEVICES];
extern soc_dpp_port_t *soc_dpp_tm_ports[SOC_MAX_NUM_DEVICES];

#define SOC_DPP_TM_PORT_IN_USE(unit, port)  (soc_dpp_tm_ports[unit][port].in_use == TRUE)
#define SOC_DPP_TM_PORT_RESERVE(unit, port) (soc_dpp_tm_ports[unit][port].in_use = TRUE)
#define SOC_DPP_TM_PORT_FREE(unit, port)    (soc_dpp_tm_ports[unit][port].in_use = FALSE)

#define SOC_DPP_PP_PORT_IN_USE(unit, port)  (soc_dpp_pp_ports[unit][port].in_use == TRUE)
#ifdef BCM_ARAD_SUPPORT
#define SOC_DPP_PP_PORT_RESERVE(unit, port)            \
{                                                      \
    if (SOC_IS_ARAD(unit)) {                           \
        rv = sw_state_access[unit].dpp.soc.arad.pp.pp_port_map.pp_port_use.in_use.set(unit, port, TRUE); \
        SOCDNX_IF_ERR_EXIT(rv);                        \
    }                                                  \
    (soc_dpp_pp_ports[unit][port].in_use = TRUE);      \
}
#define SOC_DPP_PP_PORT_FREE(unit, port)               \
{                                                      \
    if (SOC_IS_ARAD(unit) && !SOC_IS_DETACHING(unit)) {                           \
        rv = sw_state_access[unit].dpp.soc.arad.pp.pp_port_map.pp_port_use.in_use.set(unit, port, FALSE);\
        SOCDNX_IF_ERR_EXIT(rv);                        \
    }                                                  \
    (soc_dpp_pp_ports[unit][port].in_use = FALSE);     \
}
#else
#define SOC_DPP_PP_PORT_RESERVE(unit, port) (soc_dpp_pp_ports[unit][port].in_use = TRUE)
#define SOC_DPP_PP_PORT_FREE(unit, port)    (soc_dpp_pp_ports[unit][port].in_use = FALSE)
#endif

int _soc_dpp_port_map_deinit(int unit);
int _soc_dpp_port_map_init(int unit);
int _soc_dpp_wb_pp_port_restore(int unit);

int petra_soc_dpp_local_to_nif_id_set(int unit, soc_port_t port, SOC_TMC_INTERFACE_ID nif_id, uint32 channel); 
int petra_soc_dpp_local_port_partial(int unit, soc_port_t port);
int petra_soc_dpp_local_to_tm_port_set_internal(int unit, soc_port_t port, int tm_port); 
int petra_soc_dpp_local_to_pp_port_set_internal(int unit, soc_port_t port, int pp_port);
int petra_soc_dpp_local_port_valid(int unit, soc_port_t port); 
int petra_soc_dpp_local_to_nif_id_get(int unit, soc_port_t port, SOC_TMC_INTERFACE_ID *nif_id, uint32 *channel);
int petra_soc_dpp_local_to_pp_port_get(int unit, soc_port_t port, uint32* pp_port, int* core);
int petra_soc_dpp_local_to_tm_port_get(int unit, soc_port_t port, uint32* tm_port, int* core);
int petra_soc_dpp_pp_to_local_port_get(int unit, int core, uint32 pp_port, soc_port_t *port);
int petra_soc_dpp_tm_to_local_port_get(int unit, int core, uint32 tm_port, soc_port_t *port);
int petra_soc_dpp_local_to_tm_port_set(int unit,soc_port_t port, int core, uint32 tm_port);
int petra_soc_dpp_local_to_pp_port_set(int unit, soc_port_t port, uint32 pp_port);

#endif  
