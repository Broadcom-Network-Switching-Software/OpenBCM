/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: jer_nif.h
 */

#ifndef __JER_NIF_INCLUDED__

#define __JER_NIF_INCLUDED__

#include <soc/portmod/portmod.h>
#include <soc/dpp/TMC/tmc_api_ports.h>
#include <soc/dpp/JER/jer_stat.h>
#include <soc/dpp/JER/jer_nif_prd.h>
#include <soc/dcmn/dcmn_defs.h>

#define NUM_OF_LANES_IN_PM                4
#define MAX_NUM_OF_PMS_IN_ILKN            6
#define JER_NIF_ILKN_MAX_NOF_LANES        24

#define SOC_JER_NIF_NOF_ILKN_OVER_FABRIC_LINKS_JERICHO        24
#define SOC_JER_NIF_NOF_ILKN_OVER_FABRIC_LINKS_QMX            16

#define SOC_JER_NIF_PLL_TYPE_FABRIC0_FIRST_PHY_LANE 190
#define SOC_JER_NIF_PLL_TYPE_FABRIC0_LAST_PHY_LANE  226

#define SOC_JER_INVALID_PORT             -1

#define JER_NIF_PRIO_TDM_LEVEL 2
#define JER_NIF_PRIO_HIGH_LEVEL 1
#define JER_NIF_PRIO_LOW_LEVEL 0

#define JER_NIF_TSC_LANE_STATUS_REG_LINK_STATUS_BIT      (1 << 0)
#define JER_NIF_TSC_LANE_STATUS_REG_SIGNAL_OK_BIT        (1 << 1)
#define JER_NIF_TSC_LANE_STATUS_REG_CDR_LOCK_BIT         (1 << 2)
#define JER_NIF_TSC_LINK_IS_UP                           JER_NIF_TSC_LANE_STATUS_REG_CDR_LOCK_BIT
#define JER_FABRIC_SRD_QUAD_STATUS_REG_SYNC_STATUS_FLAG  (1 << 3)
#define JER_FABRIC_SRD_QUAD_STATUS_REG_RX_LOCK_FLAG      (1 << 4)
#define JER_FABRIC_SRD_LINK_IS_UP                        JER_FABRIC_SRD_QUAD_STATUS_REG_SYNC_STATUS_FLAG | JER_FABRIC_SRD_QUAD_STATUS_REG_RX_LOCK_FLAG


#define SOC_JER_NIF_PRD_F_PORT_EXTERNDER SOC_TMC_PORT_PRD_F_PORT_EXTERNDER

#define SOC_JER_NIF_FIRST_FABRIC_PHY_PORT(unit)   \
        SOC_DPP_FIRST_FABRIC_PHY_PORT(unit)

#define SOC_JER_NIF_IS_ILKN_IF_OVER_FABRIC(unit, ilkn_if_id) \
        SOC_DPP_IS_ILKN_PORT_OVER_FABRIC(unit, ilkn_if_id)

#define SOC_JER_NIF_IS_ILKN_OVER_FABRIC_ENABLED(unit) \
        ((SOC_DPP_CONFIG(unit)->jer->nif.ilkn_over_fabric[0]) ||  \
        (SOC_DPP_CONFIG(unit)->jer->nif.ilkn_over_fabric[1]))

#define SOC_JER_NIF_ILKN_OVER_FABRIC_MAX_LANE(unit) (SOC_IS_QMX(unit) ? SOC_JER_NIF_NOF_ILKN_OVER_FABRIC_LINKS_QMX : SOC_JER_NIF_NOF_ILKN_OVER_FABRIC_LINKS_JERICHO)

typedef SOC_TMC_PORT_PRD_FLEX_KEY_ENTRY soc_jer_nif_prd_flex_key_entry_t;


typedef enum
{
  SOC_JER_NIF_PLL_TYPE_PMH,
  SOC_JER_NIF_PLL_TYPE_PML0,
  SOC_JER_NIF_PLL_TYPE_PML1,
  SOC_JER_NIF_PLL_TYPE_FABRIC0,

  
  SOC_JER_NIF_NOF_PLL_TYPE
} SOC_JER_NIF_PLL_TYPE;


typedef enum soc_jer_nif_prd_enable_mode_e {
    socJerNifPrdDisable = 0, 
    socJerNifPrdEnableHardStage = 1, 
    socJerNifPrdEnableHardAndSoftStage = 2 
} soc_jer_nif_prd_enable_mode_t;

int soc_jer_portmod_calc_os(int unit, phymod_phy_access_t* phy_access, uint32* os_int, uint32* os_remainder);
int soc_jer_portmod_init(int unit);
int soc_jer_portmod_post_init(int unit, soc_pbmp_t* ports);
int soc_jer_portmod_deinit(int unit);
int soc_jer_portmod_probe(int unit, pbmp_t pbmp, pbmp_t *okay_pbmp, int is_init_sequence);
int soc_jer_portmod_port_detach(int unit, int port);
int soc_jer_portmod_port_enable_set(int unit, soc_port_t port, uint32 mac_only, int enable);
int soc_jer_portmod_port_enable_get(int unit, soc_port_t port, uint32 mac_only, int* enable);
int soc_jer_portmod_port_speed_set(int unit, soc_port_t port, int speed);
int soc_jer_portmod_port_speed_get(int unit, soc_port_t port, int* speed);
int soc_jer_portmod_port_interface_set(int unit, soc_port_t port, soc_port_if_t intf);
int soc_jer_portmod_port_interface_get(int unit, soc_port_t port, soc_port_if_t* intf);
int soc_jer_portmod_fabric_port_interface_set(int unit, soc_port_t port, soc_port_if_t intf);
int soc_jer_portmod_fabric_port_interface_get(int unit, soc_port_t port, soc_port_if_t* intf);
int soc_jer_portmod_port_link_state_get(int unit, soc_port_t port, int clear_status, int *is_link_up, int *is_latch_down);
int soc_jer_portmod_is_supported_encap_get(int unit, int mode, int* is_supported);
int soc_jer_port_link_get(int unit, soc_port_t port, int *link_up);
int soc_jer_portmod_autoneg_set(int unit, soc_port_t port, int enable);
int soc_jer_portmod_autoneg_get(int unit, soc_port_t port, int* enable);
int soc_jer_port_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability_mask);
int soc_jer_port_ability_remote_get(int unit, soc_port_t port, soc_port_ability_t *ability_mask);
int soc_jer_port_ability_advert_set(int unit, soc_port_t port, soc_port_ability_t *ability_mask);
int soc_jer_port_ability_advert_get(int unit, soc_port_t port, soc_port_ability_t *ability_mask);
int soc_jer_port_mdix_set(int unit, soc_port_t port, soc_port_mdix_t mode);
int soc_jer_port_mdix_get(int unit, soc_port_t port, soc_port_mdix_t *mode);
int soc_jer_port_mdix_status_get(int unit, soc_port_t port, soc_port_mdix_status_t *status);
int soc_jer_portmod_pfc_refresh_set(int unit, soc_port_t port, int value);
int soc_jer_portmod_pfc_refresh_get(int unit, soc_port_t port, int* value);
int soc_jer_port_phy_reset(int unit, soc_port_t port);
int soc_jer_nif_ilkn_nof_segments_set(int unit, int port, uint32 nof_segments);
int soc_jer_nif_ilkn_nof_segments_get(int unit, int port, uint32* nof_segments);
int soc_jer_nif_ilkn_nof_segments_calc(int unit, int port, uint32* nof_segments);
int soc_jer_port_mac_sa_set(int unit, int port, sal_mac_addr_t mac_sa);
int soc_jer_port_mac_sa_get(int unit, int port, sal_mac_addr_t mac_sa);
int soc_jer_port_pll_type_get(int unit, soc_port_t port, SOC_JER_NIF_PLL_TYPE *pll_type);
int soc_jer_port_duplex_set(int unit, soc_port_t port, int duplex);
int soc_jer_port_fault_get(int unit, soc_port_t port, uint32 *flags);
int soc_jer_port_eee_enable_set(int unit, soc_port_t port, uint32 value);
int soc_jer_port_eee_enable_get(int unit, soc_port_t port, uint32 *value);
int soc_jer_port_eee_tx_idle_time_set(int unit, soc_port_t port, uint32 value);
int soc_jer_port_eee_tx_idle_time_get(int unit, soc_port_t port, uint32 *value);
int soc_jer_port_eee_tx_wake_time_set(int unit, soc_port_t port, uint32 value);
int soc_jer_port_eee_tx_wake_time_get(int unit, soc_port_t port, uint32 *value);
int soc_jer_port_nif_nof_lanes_get(int unit, soc_port_if_t interface, uint32 first_phy_port, uint32 nof_lanes_to_set, uint32 *nof_lanes);
soc_error_t soc_jer_port_loopback_set(int unit, soc_port_t port, soc_dcmn_loopback_mode_t loopback);
int soc_jer_port_timestamp_adjust_set(int unit, soc_port_t port, soc_port_timestamp_adjust_t *ts_adjust);
int soc_jer_port_timestamp_adjust_get(int unit, soc_port_t port, soc_port_timestamp_adjust_t *ts_adjust);
int soc_jer_port_master_set(int unit, soc_port_t port, int ms_mode);
int soc_jer_port_master_get(int unit, soc_port_t port, int *ms_mode);

uint32 jer_port_link_up_mac_update(int unit, soc_port_t port, int link);
int soc_jer_port_remote_fault_enable_set(int unit, bcm_port_t port, int enable);
int soc_jer_nif_ilkn_pbmp_get(int unit, soc_port_t port, uint32 ilkn_id, soc_pbmp_t* phys, soc_pbmp_t* src_pbmp);
int soc_jer_nif_ilkn_over_fabric_pbmp_get(int unit, soc_pbmp_t* phys);
int soc_jer_nif_qsgmii_pbmp_get(int unit, soc_port_t port, uint32 id, soc_pbmp_t *phy_pbmp);

int soc_jer_pm_instances_get(int unit, portmod_pm_instances_t **pm_instances, int *pms_instances_arr_len);
int soc_jer_pml_table_get(int unit, soc_dpp_pm_entry_t **soc_pml_table);

int soc_jer_nif_priority_quad_ilkn_tdm_get(int unit, int core, uint32 quad_ilkn, uint32 is_ilkn, uint32* is_set);
int soc_jer_nif_priority_quad_ilkn_low_get(int unit, int core, uint32 quad_ilkn, uint32 is_ilkn, uint32* is_set);
int soc_jer_nif_priority_quad_ilkn_high_get(int unit, int core, uint32 quad_ilkn, uint32 is_ilkn, uint32* is_set);
int soc_jer_nif_is_ilkn_share_quad(int unit, uint32 ilkn_id, int *is_share_quad, soc_port_t* share_quad_port);

int soc_jer_lane_map_get(int unit, int quad, phymod_lane_map_t* lane_map);
int soc_jer_portmod_port_quad_get(int unit, soc_port_t port, soc_pbmp_t* quad_bmp, int *is_ilkn_over_fabric);
int soc_jer_port_sch_config(int unit, soc_port_t port);
int soc_jer_port_close_path(int unit, soc_port_t port);
int soc_jer_port_ports_to_same_quad_get(int unit, soc_port_t port, soc_pbmp_t* ports);
int soc_jer_port_quad_ports_get(int unit, uint32 quad, soc_pbmp_t* ports_bm);
int soc_jer_nif_port_rx_enable_set(int unit, soc_port_t port, int enable);
int soc_jer_nif_port_rx_enable_get(int unit, soc_port_t port, int *enable);
int soc_jer_nif_port_tx_enable_set(int unit, soc_port_t port, int enable);
int soc_jer_nif_port_tx_enable_get(int unit, soc_port_t port, int *enable);
int soc_jer_port_protocol_offset_verify(int unit, soc_port_t port, uint32 protocol_offset);
int soc_jer_port_ilkn_over_fabric_set(int unit, soc_port_t port, uint32 ilkn_id);


int soc_jer_nif_priority_ilkn_tdm_clear(int unit, uint32 ilkn_id);
int soc_jer_nif_priority_ilkn_high_low_clear(int unit, uint32 ilkn_id);
int soc_jer_nif_priority_quad_tdm_high_low_clear(int unit, uint32 quad, int clear_tdm, int clear_high_low);

int soc_jer_nif_priority_set(   SOC_SAND_IN     int     unit,
                                SOC_SAND_IN     int     core,
                                SOC_SAND_IN     uint32  quad_ilkn,
                                SOC_SAND_IN     uint32  is_ilkn,
                                SOC_SAND_IN     uint32  flags,
                                SOC_SAND_IN     uint32  allow_tdm,
                                SOC_SAND_IN     int     priority_level);

int soc_jer_nif_priority_get(   SOC_SAND_IN     int     unit,
                                SOC_SAND_IN     int     core,
                                SOC_SAND_IN     uint32  quad_ilkn,
                                SOC_SAND_IN     uint32  is_ilkn,
                                SOC_SAND_IN     uint32  flags,
                                SOC_SAND_IN     uint32  allow_tdm,
                                SOC_SAND_OUT    int*    priority_level);

int jer_nif_ilkn_counter_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  soc_port_t               port,
    SOC_SAND_IN  soc_jer_counters_t       counter_type,
    SOC_SAND_OUT uint64                   *counter_val
  );
int soc_jer_nif_ilkn_phys_aligned_pbmp_get(int unit, soc_port_t port, soc_pbmp_t *phys_aligned, int is_format_adjust);
int jer_nif_ilkn_counter_clear(int unit, soc_port_t port);


int soc_jer_qsgmii_offsets_add(int unit, uint32 phy, uint32 *new_phy);
int soc_jer_qsgmii_offsets_add_pbmp(int unit, soc_pbmp_t* pbmp, soc_pbmp_t* new_pbmp);


int soc_jer_qsgmii_offsets_remove(int unit, uint32 phy, uint32 *new_phy);
int soc_jer_qsgmii_offsets_remove_pbmp(int unit, soc_pbmp_t* pbmp, soc_pbmp_t* new_pbmp);

int soc_jer_port_open_ilkn_path(int unit, int port); 
int soc_jer_port_close_ilkn_path(int unit, int port); 

int soc_jer_port_ilkn_init(int unit);

int soc_jer_port_open_path(int unit, soc_port_t port);


soc_error_t soc_jer_port_fabric_clk_freq_init(int unit, soc_pbmp_t pbmp);

int soc_jer_port_nif_quad_to_core_validate(int unit);
int soc_jer_nif_ilkn_wrapper_port_enable_set(int unit, soc_port_t port, int enable);
int soc_jer_port_is_pcs_loopback(int unit, soc_port_t port, int *result);

int soc_jer_wait_gtimer_trigger(int unit);
int soc_jer_phy_nif_measure(int unit, soc_port_t port, uint32 *type_of_bit_clk, int *one_clk_time_measured_int, int *one_clk_time_measured_remainder, int *serdes_freq_int, int *serdes_freq_remainder, uint32 *lane);
int soc_jer_phy_nif_pll_div_get(int unit, soc_port_t port, soc_dcmn_init_serdes_ref_clock_t *ref_clk, int *p_div, int *n_div, int *m0_div);
int soc_jer_port_ilkn_bypass_interface_enable(int unit, int port, int enable);
soc_error_t soc_jer_port_ilkn_bypass_interface_rx_check_and_enable(int unit, soc_port_t port);
soc_error_t soc_jer_port_ilkn_bypass_interface_rx_enable_get(int unit, int port, uint32* rx_enable);


int soc_jer_port_prd_enable_set(int unit, soc_port_t port, uint32 flags, int enable);
int soc_jer_port_prd_enable_get(int unit, soc_port_t port, uint32 flags, int *enable);
int soc_jer_port_prd_config_set(int unit, soc_port_t port, uint32 flags, soc_dpp_port_prd_config_t *config);
int soc_jer_port_prd_config_get(int unit, soc_port_t port, uint32 flags, soc_dpp_port_prd_config_t *config);
int soc_jer_port_prd_threshold_set(int unit, soc_port_t port, uint32 flags, int priority, uint32 value);
int soc_jer_port_prd_threshold_get(int unit, soc_port_t port, uint32 flags, int priority, uint32 *value);
int soc_jer_port_prd_map_set(int unit, soc_port_t port, uint32 flags, soc_dpp_prd_map_t map, uint32 key, int priority);
int soc_jer_port_prd_map_get(int unit, soc_port_t port, uint32 flags, soc_dpp_prd_map_t map, uint32 key, int* priority);
int soc_jer_port_prd_drop_count_get(int unit, soc_port_t port, uint64 *count);
int soc_jer_port_prd_tpid_set(int unit, soc_port_t port, uint32 flags, int index, uint32 tpid);
int soc_jer_port_prd_tpid_get(int unit, soc_port_t port, uint32 flags, int index, uint32 *tpid);
int soc_jer_plus_prd_ignore_ip_dscp_set(int unit, soc_port_t port, uint32 flags, uint32 ip_dscp_ignore);
int soc_jer_plus_prd_ignore_ip_dscp_get(int unit, soc_port_t port, uint32 flags, uint32 *ip_dscp_ignore);
int soc_jer_plus_prd_ignore_mpls_exp_set(int unit, soc_port_t port, uint32 flags, uint32 mpls_exp_ignore);
int soc_jer_plus_prd_ignore_mpls_exp_get(int unit, soc_port_t port, uint32 flags, uint32 *mpls_exp_ignore);
int soc_jer_plus_prd_ignore_inner_tag_set(int unit, soc_port_t port, uint32 flags, uint32 inner_tag_ignore);
int soc_jer_plus_prd_ignore_inner_tag_get(int unit, soc_port_t port, uint32 flags, uint32 *inner_tag_ignore);
int soc_jer_plus_prd_ignore_outer_tag_set(int unit, soc_port_t port, uint32 flags, uint32 outer_tag_ignore);
int soc_jer_plus_prd_ignore_outer_tag_get(int unit, soc_port_t port, uint32 flags, uint32 *outer_tag_ignore);
int soc_jer_plus_prd_default_priority_set(int unit, soc_port_t port, uint32 flags, uint32 default_priority);
int soc_jer_plus_prd_default_priority_get(int unit, soc_port_t port, uint32 flags, uint32 *default_priority);
int soc_jer_plus_prd_custom_ether_type_set(int unit, soc_port_t port, uint32 flags, uint32 ether_type_code, uint32 ether_type_val);
int soc_jer_plus_prd_custom_ether_type_get(int unit, soc_port_t port, uint32 flags, uint32 ether_type_code, uint32 *ether_type_val);
int soc_jer_plus_prd_control_frame_set(int unit, soc_port_t port, uint32 flags, uint32 control_frame_index, soc_jer_nif_prd_control_plane_t *prd_ctrl_plane);
int soc_jer_plus_prd_control_frame_get(int unit, soc_port_t port, uint32 flags, uint32 control_frame_index, soc_jer_nif_prd_control_plane_t *prd_ctrl_plane);
int soc_jer_plus_prd_flex_key_construct_set(int unit, soc_port_t port, uint32 flags, uint32* offset_array, uint32 array_size);
int soc_jer_plus_prd_flex_key_construct_get(int unit, soc_port_t port, uint32 flags, uint32* offset_array, uint32* array_size);
int soc_jer_plus_prd_flex_key_entry_set(int unit, soc_port_t port, uint32 flags, uint32 key_index, soc_jer_nif_prd_flex_key_entry_t* key_entry);
int soc_jer_plus_prd_flex_key_entry_get(int unit, soc_port_t port, uint32 flags, uint32 key_index, soc_jer_nif_prd_flex_key_entry_t* key_entry);
int soc_jer_port_prd_restore_hw_defaults(int unit, soc_port_t port);
int soc_jer_port_speed_sku_restrictions(int unit, soc_port_t port, int speed);
int soc_jer_nif_sif_set(int unit, uint32 first_phy);
int soc_jer_nif_sku_restrictions(int unit, soc_pbmp_t phy_pbmp, soc_port_if_t interface, uint32 protocol_offset, uint32 is_kbp);
int soc_jer_nif_is_ilkn_port_exist(int unit, int ilkn_id, soc_port_t* port);
#endif 

