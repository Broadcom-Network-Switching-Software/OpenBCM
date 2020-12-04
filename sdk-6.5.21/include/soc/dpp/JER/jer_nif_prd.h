/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: jer_nif_prd.h
 */

#ifndef __JER_NIF_PRD_INCLUDED__

#define __JER_NIF_PRD_INCLUDED__

#include <sal/types.h>
#include <soc/register.h>
#include <soc/dpp/TMC/tmc_api_ports.h>

typedef struct soc_jer_nif_prd_config_s {
    int never_drop_control_frames; 
    int seperate_tdm_and_non_tdm;
    int seperate_tdm_and_non_tdm_two_ports;
    int never_drop_tdm_packets;
} soc_jer_nif_prd_config_t;

typedef struct soc_jer_nif_prd_hard_stage_control_s {
    int itmh_type;                      
    int small_chunk_priority_override;  
} soc_jer_nif_prd_hard_stage_control_t;

typedef struct soc_jer_nif_prd_hard_stage_properties_s {
    int trust_ip_sdcp;
    int trust_mpls_exp;
    int trust_outer_eth_tag;
    int trust_inner_eth_tag;
    int outer_tag_size;
    int default_priority;
} soc_jer_nif_prd_hard_stage_properties_t;

typedef struct soc_jer_nif_prd_tcam_entry_s {
    uint64 key;
    uint64 mask;
    int priority;
    int valid;
} soc_jer_nif_prd_tcam_entry_t;

typedef SOC_TMC_PORT_PRD_CONTROL_PLANE soc_jer_nif_prd_control_plane_t;

int soc_jer_plus_prd_hard_stage_enable_set(int unit, int port, uint32 en_mask);
int soc_jer_plus_prd_hard_stage_enable_get(int unit, int port, uint32 *en_mask);

int soc_jer_plus_prd_config_set(int unit, int port, soc_jer_nif_prd_config_t *prd_config);
int soc_jer_plus_prd_config_get(int unit, int port, soc_jer_nif_prd_config_t *prd_config);

int soc_jer_plus_prd_hard_stage_control_set(int unit, int port, soc_jer_nif_prd_hard_stage_control_t* hard_stage_ctrl);
int soc_jer_plus_prd_hard_stage_control_get(int unit, int port, soc_jer_nif_prd_hard_stage_control_t* hard_stage_ctrl);

int soc_jer_plus_prd_tpid_set(int unit, int port, int tpid_index, uint32 tpid_val);
int soc_jer_plus_prd_tpid_get(int unit, int port, int tpid_index, uint32 *tpid_val);

int soc_jer_plus_prd_hard_stage_port_type_set(int unit, int port, int port_type);
int soc_jer_plus_prd_hard_stage_port_type_get(int unit, int port, int *port_type);

int soc_jer_plus_prd_hard_stage_properties_set(int unit, int port, soc_jer_nif_prd_hard_stage_properties_t *properties);
int soc_jer_plus_prd_hard_stage_properties_get(int unit, int port, soc_jer_nif_prd_hard_stage_properties_t *properties);

int soc_jer_plus_prd_hard_ether_type_set(int unit, int port, uint32 ether_type_field, uint32 ether_type_val);
int soc_jer_plus_prd_hard_ether_type_get(int unit, int port, uint32 ether_type_field, uint32 *ether_type_val);

int soc_jer_plus_prd_control_plane_set(int unit, int port, uint32 control_frame_index, soc_jer_nif_prd_control_plane_t *control_frame_conf);
int soc_jer_plus_prd_control_plane_get(int unit, int port, uint32 control_frame_index, soc_jer_nif_prd_control_plane_t *control_frame_conf);

int soc_jer_plus_prd_map_tm_tc_dp_set(int unit, int port, soc_reg_above_64_val_t map_value);
int soc_jer_plus_prd_map_tm_tc_dp_get(int unit, int port, soc_reg_above_64_val_t map_value);

int soc_jer_plus_prd_map_ip_dscp_set(int unit, int port, soc_reg_above_64_val_t map_value);
int soc_jer_plus_prd_map_ip_dscp_get(int unit, int port, soc_reg_above_64_val_t map_value);

int soc_jer_plus_prd_map_eth_pcp_dei_set(int unit, int port, uint32 map_value);
int soc_jer_plus_prd_map_eth_pcp_dei_get(int unit, int port, uint32 *map_value);

int soc_jer_plus_prd_map_mpls_exp_set(int unit, int port, uint32 map_value);
int soc_jer_plus_prd_map_mpls_exp_get(int unit, int port, uint32 *map_value);

int soc_jer_plus_soft_stage_enable_set(int unit, int port, int en_eth, int en_tm); 
int soc_jer_plus_soft_stage_enable_get(int unit, int port, int *en_eth, int *en_tm); 

int soc_jer_plus_soft_stage_key_construct_set(int unit, int port, uint32 *key_build_offset_array); 
int soc_jer_plus_soft_stage_key_construct_get(int unit, int port, uint32 *key_build_offset_array); 

int soc_jer_plus_soft_stage_key_entry_set(int unit, int port, uint32 key_index, soc_jer_nif_prd_tcam_entry_t *tcam_entry); 
int soc_jer_plus_soft_stage_key_entry_get(int unit, int port, uint32 key_index, soc_jer_nif_prd_tcam_entry_t *tcam_entry);

int soc_jer_plus_prd_drop_count_get(int unit, int port, uint64 *count);

#endif 

