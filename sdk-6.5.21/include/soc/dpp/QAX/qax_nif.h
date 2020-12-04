/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: qax_nif.h
 */

#ifndef __QAX_NIF_INCLUDED__

#define __QAX_NIF_INCLUDED__

#include <soc/portmod/portmod.h>


#define QAX_NIF_PHY_SIF_PORT_NBIL0 (64)
#define QAX_NIF_PHY_SIF_PORT_NBIL1 (116)

#define QUX_NIF_PHY_SIF_PORT_NBI (0)

#define SOC_QAX_HRF_SCH_PRIO_BIT_NUM    5


typedef enum
{    
    SOC_QUX_NIF_PLL_TYPE_PML,
    SOC_QUX_NIF_PLL_TYPE_PMX,

    
    SOC_QUX_NIF_NOF_PLL_TYPE
} SOC_QUX_NIF_PLL_TYPE;


typedef enum
{
    SOC_QAX_DIRECTION_INVALID=-1,
    SOC_QAX_DIRECTION_FIRST=0,
    SOC_QAX_DIRECTION_TX=SOC_QAX_DIRECTION_FIRST,
    SOC_QAX_DIRECTION_RX,
    SOC_QAX_NOF_DIRECTIONS
} soc_qax_direction_t;


int soc_qax_qsgmii_offsets_add(int unit, uint32 phy, uint32 *new_phy);


int soc_qax_qsgmii_offsets_remove(int unit, uint32 phy, uint32 *new_phy);

int soc_qax_port_sch_config(int unit, soc_port_t port);

int soc_qax_nif_priority_set( int unit, int core, uint32  quad_ilkn, uint32  is_ilkn, uint32  flags, uint32  allow_tdm,int priority_level);

int soc_qax_port_open_fab_o_nif_path(int unit, int port);

int soc_qax_port_fabric_o_nif_bypass_interface_enable(int unit, int port, int enable);

int soc_qax_port_open_ilkn_path(int unit, int port);

int soc_qax_port_close_ilkn_path(int unit, int port);

int soc_qax_pm_instances_get(int unit, portmod_pm_instances_t **pm_instances, int *pms_instances_arr_len);

int soc_qax_pml_table_get(int unit, soc_dpp_pm_entry_t **soc_pml_table);

int soc_qax_wait_gtimer_trigger(int unit);

int soc_qax_port_ilkn_init(int unit);

int soc_qax_nif_ilkn_pbmp_from_num_lanes_get(int unit ,uint32 num_lanes,uint32 first_phy, uint32 ilkn_id, soc_pbmp_t* pbmp);

int soc_qax_nif_ilkn_pbmp_get(int unit, soc_port_t port, uint32 ilkn_id, soc_pbmp_t* phys, soc_pbmp_t* src_pbmp);

int soc_qax_nif_ilkn_phys_aligned_pbmp_get(int unit, soc_port_t port, soc_pbmp_t *phys_aligned, int is_format_adjust);

int soc_qax_nif_qsgmii_pbmp_get(int unit, soc_port_t port, uint32 id, soc_pbmp_t *phy_pbmp);

int soc_qax_nif_ilkn_nof_segments_set(int unit, int port, uint32 nof_segments);

int soc_qax_port_ilkn_bypass_interface_enable(int unit, int port, int enable);
soc_error_t soc_qax_port_ilkn_bypass_interface_rx_check_and_enable(int unit, int port);
soc_error_t soc_qax_port_ilkn_bypass_interface_rx_enable_get(int unit, int port, uint32* rx_enable);
int soc_qax_nif_sif_set(int unit, uint32 first_phy);
int soc_qax_port_protocol_offset_verify(int unit, soc_port_t port, uint32 protocol_offset);


int soc_qax_nif_priority_ilkn_tdm_clear(int unit, uint32 ilkn_id);
int soc_qax_nif_priority_ilkn_high_low_clear(int unit, uint32 ilkn_id);
int soc_qax_nif_priority_quad_tdm_high_low_clear(int unit, uint32 quad, int clear_tdm, int clear_high_low);

int soc_qax_port_prd_enable_set(int unit, soc_port_t port, uint32 flags, int enable);
int soc_qax_port_prd_enable_get(int unit, soc_port_t port, uint32 flags, int *enable);
int soc_qax_port_prd_config_set(int unit, soc_port_t port, uint32 flags, soc_dpp_port_prd_config_t *config);
int soc_qax_port_prd_config_get(int unit, soc_port_t port, uint32 flags, soc_dpp_port_prd_config_t *config);
int soc_qax_port_prd_threshold_set(int unit, soc_port_t port, uint32 flags, int priority, uint32 value);
int soc_qax_port_prd_threshold_get(int unit, soc_port_t port, uint32 flags, int priority, uint32 *value);
int soc_qax_port_prd_map_set(int unit, soc_port_t port, uint32 flags, soc_dpp_prd_map_t map, uint32 key, int priority);
int soc_qax_port_prd_map_get(int unit, soc_port_t port, uint32 flags, soc_dpp_prd_map_t map, uint32 key, int *priority);
int soc_qax_port_prd_drop_count_get(int unit, soc_port_t port, uint64 *count);
int soc_qax_port_speed_sku_restrictions(int unit, soc_port_t port, int speed);
int soc_qax_nif_sku_restrictions(int unit, soc_pbmp_t phy_pbmp, soc_port_if_t interface, uint32 interface_id, uint32 is_kbp);


int soc_qux_port_pll_type_get(int unit, soc_port_t port, SOC_QUX_NIF_PLL_TYPE *pll_type);
int soc_qax_ilkn_pml_override_clk_data_mux(int unit, pbmp_t *pbmp);
int soc_qax_led_pmlq_bypass_config_adjust(int unit, soc_port_t port, int add);
#endif 
