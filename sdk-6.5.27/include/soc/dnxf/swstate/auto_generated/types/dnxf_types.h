
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNXF_TYPES_H__
#define __DNXF_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <include/bcm/port.h>
#include <include/bcm/switch.h>
#include <include/shared/fabric.h>
#include <include/shared/pkt.h>
#include <include/soc/dnxc/dnxc_fabric_source_routed_cell.h>
#include <include/soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_fabric.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>
#include <soc/dnxc/swstate/types/sw_state_pbmp.h>
#include <soc/types.h>
#include <soc/dnxc/swstate/auto_generated/types/dnxc_types.h>


typedef sw_state_user_def_t* soc_dnxf_signal_quality_callback_userdata_t;



typedef struct {
    SHR_BITDCL* active_core_bmp;
    SHR_BITDCL* valid_fsrd_bmp;
    SHR_BITDCL* valid_dch_dcml_core_bmp;
} soc_dnxf_info_t;

typedef struct {
    SHR_BITDCL* raw;
} soc_dnxf_modid_group_map_t;

typedef struct {
    uint32 valid;
    uint32 module_id;
} soc_dnxf_modid_local_map_t;

typedef struct {
    soc_dnxf_modid_local_map_t* local_map;
    soc_dnxf_modid_group_map_t* group_map;
    soc_module_t* modid_to_group_map;
} soc_dnxf_modid_t;

typedef struct {
    uint32 mode;
    uint32* id_map;
    uint32 nof_mc_groups_created;
} soc_dnxf_mc_t;

typedef struct {
    soc_port_t** group_to_first_link;
} soc_dnxf_lb_t;

typedef struct {
    uint32* cl72_conf;
    int* phy_lane_config;
    int* speed;
    int* fec_type;
    SHR_BITDCL* is_connected_to_repeater;
    int* isolation_status_stored;
    int* orig_isolated_status;
} soc_dnxf_port_t;

typedef struct {
    int* is_enable;
    int* is_master;
} soc_dnxf_cable_swap_t;

typedef struct {
    uint32 degrade_thr;
    uint32 degrade_cu_thr;
    uint32 fail_thr;
    uint32 fail_cu_thr;
    uint32 interval;
} soc_dnxf_signal_quality_fmac_config_t;

typedef struct {
    bcm_port_signal_quality_callback_t callback;
    soc_dnxf_signal_quality_callback_userdata_t userdata;
} soc_dnxf_signal_quality_intr_t;

typedef struct {
    soc_dnxf_signal_quality_intr_t interrupt;
    soc_dnxf_signal_quality_fmac_config_t* fmac_config;
    bcm_pbmp_t action_enabled;
    bcm_pbmp_t enabled_ports;
    bcm_pbmp_t measure_mode;
} soc_dnxf_signal_quality_db_fabric_t;

typedef struct {
    uint32* flags;
    uint32* storm_timed_count;
    uint32* storm_timed_period;
    uint32 storm_nominal;
} soc_dnxf_intr_t;

typedef struct {
    uint32 state_0;
    uint32 state_1;
    uint32 state_2;
} soc_dnxf_system_upgrade_t;

typedef struct {
    soc_dnxc_lane_map_db_t serdes_map;
    int* link_to_fmac_lane_map;
} soc_dnxf_lane_map_db_t;

typedef struct {
    bcm_switch_hard_reset_callback_t callback;
    uint32* userdata;
} soc_dnxf_hard_reset_t;

typedef struct {
    int dest_modid;
    int dest_sysport;
} soc_dnxf_ip_self_routed_config_t;

typedef struct {
    soc_port_t path_link[DNXC_CELL_NOF_LINKS_IN_PATH_LINKS];
    _shr_fabric_device_type_t dest_device_type;
} soc_dnxf_ip_source_routed_config_t;

typedef struct {
    soc_dnxf_ip_self_routed_config_t self_routed_config;
    soc_dnxf_ip_source_routed_config_t source_routed_config;
} soc_dnxf_ip_config_t;

typedef struct {
    soc_dnxf_info_t info;
    soc_dnxf_modid_t modid;
    soc_dnxf_mc_t mc;
    soc_dnxf_lb_t lb;
    soc_dnxf_port_t port;
    soc_dnxf_cable_swap_t cable_swap;
    soc_dnxf_intr_t intr;
    soc_dnxf_lane_map_db_t lane_map_db;
    soc_dnxc_synce_status_t synce;
    soc_dnxf_system_upgrade_t system_upgrade;
    soc_dnxf_hard_reset_t hard_reset;
    soc_dnxc_mib_db_t mib;
} dnxf_state_t;


#endif 
