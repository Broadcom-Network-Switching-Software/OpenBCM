/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __ARAD_FABRIC_INCLUDED__

#define __ARAD_FABRIC_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dcmn/dcmn_defs.h>
#include <soc/dpp/ARAD/arad_api_fabric.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/ARAD/arad_chip_defines.h>
#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/ARAD/arad_api_mgmt.h>
#include <soc/dcmn/dcmn_port.h>




#define ARAD_FBC_PRIORITY_NDX_MASK          (0xff)
#define ARAD_FBC_PRIORITY_NDX_NOF           (256)

#define ARAD_FBC_PRIORITY_NDX_IS_TDM_MASK       (0x1)
#define ARAD_FBC_PRIORITY_NDX_IS_TDM_OFFSET     (0)
#define ARAD_FBC_PRIORITY_NDX_TC_MASK           (0xE)
#define ARAD_FBC_PRIORITY_NDX_TC_OFFSET         (1)
#define ARAD_FBC_PRIORITY_NDX_DP_MASK           (0x30)
#define ARAD_FBC_PRIORITY_NDX_DP_OFFSET         (4)
#define ARAD_FBC_PRIORITY_NDX_IS_HP_MASK        (0x80)
#define ARAD_FBC_PRIORITY_NDX_IS_HP_OFFSET      (7)

#define ARAD_FBC_PRIORITY_NOF                   (0x4)
#define ARAD_FBC_PRIORITY_LENGTH                (2)


#define ARAD_FABRIC_ALDWP_MIN                   (0x2)
#define ARAD_FABRIC_ALDWP_MAX                   (0xf)
#define ARAD_FABRIC_VSC128_MAX_CELL_SIZE        (128  + 8  + 1 )
#define ARAD_FABRIC_VSC256_MAX_CELL_SIZE        (256  + 11  + 1)



#define ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_1_LEVEL_0       (0x1)
#define ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_1_LEVEL_1       (0xf)
#define ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_1_LEVEL_2       (0x7f)
#define ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_1_LEVEL_3       (0x3ff)

#define ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_2_LEVEL_0       (0x3)
#define ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_2_LEVEL_1       (0x1f)
#define ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_2_LEVEL_2       (0xff)
#define ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_2_LEVEL_3       (0x7ff)

#define ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_3_LEVEL_0       (0x7)
#define ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_3_LEVEL_1       (0x3f)
#define ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_3_LEVEL_2       (0x1ff)
#define ARAD_FABRIC_IPT_GCI_BACKOFF_GCI_3_LEVEL_3       (0xfff)




#define ARAD_FBC_LINK_IN_MAC(unit, link_id) ((link_id) % SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac))
#define ARAD_FBC_MAC_OF_LINK(unit, link_id) ((link_id) / SOC_DPP_DEFS_GET(unit, nof_fabric_links_in_mac))

#define ARAD_FBC_IS_MAC_COMBO_NIF_NOT_FBR(mac_id, is_combo_nif_not_fabric) \
  ( (mac_id == SOC_DPP_DEFS_GET(unit, nof_instances_fmac)-1) && (is_combo_nif_not_fabric) )
















uint32
  arad_fabric_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_INIT_FABRIC*      fabric
  );


uint32
  arad_fabric_regs_init(
    SOC_SAND_IN  int                 unit
  );


uint32
  arad_fabric_common_regs_init(
    SOC_SAND_IN  int                 unit
  );



uint32
  arad_fabric_fc_enable_set_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  ARAD_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  ARAD_FABRIC_FC            *info
  );


uint32
  arad_fabric_fc_enable_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  ARAD_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  ARAD_FABRIC_FC            *info
  );


uint32
  arad_fabric_fc_enable_get_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT ARAD_FABRIC_FC            *info_rx,
    SOC_SAND_OUT ARAD_FABRIC_FC            *info_tx
  );


uint32
  arad_fabric_flow_control_init(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  ARAD_CONNECTION_DIRECTION direction_ndx,
    SOC_SAND_IN  ARAD_FABRIC_FC            *info
  );



soc_error_t
  arad_fabric_gci_backoff_masks_init(
    SOC_SAND_IN  int                 unit
  );

uint32
    arad_fabric_scheduler_adaptation_init(
       SOC_SAND_IN int    unit
  );


uint32
  arad_fabric_fc_shaper_set_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                        link_ndx,
    SOC_SAND_IN  ARAD_FABRIC_FC_SHAPER_MODE_INFO  *shaper_mode,  
    SOC_SAND_IN  ARAD_FABRIC_FC_SHAPER            *info,
    SOC_SAND_OUT ARAD_FABRIC_FC_SHAPER            *exact_info
  );



uint32
  arad_fabric_fc_shaper_verify(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                        link_ndx,
    SOC_SAND_IN  ARAD_FABRIC_FC_SHAPER_MODE_INFO  *shaper_mode,  
    SOC_SAND_IN  ARAD_FABRIC_FC_SHAPER            *info,
    SOC_SAND_OUT ARAD_FABRIC_FC_SHAPER            *exact_info
  );



uint32
  arad_fabric_fc_shaper_get_unsafe(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  uint32                         link_ndx,
    SOC_SAND_OUT  ARAD_FABRIC_FC_SHAPER_MODE_INFO  *shaper_mode,  
    SOC_SAND_OUT  ARAD_FABRIC_FC_SHAPER            *info
  );



int
  arad_fabric_mesh_topology_values_config(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int speed,
    SOC_SAND_IN soc_dcmn_port_pcs_t pcs
  );

uint32
  arad_fabric_stand_alone_fap_mode_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 is_single_fap_mode
  );


uint32
  arad_fabric_stand_alone_fap_mode_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 is_single_fap_mode
  );


uint32
  arad_fabric_stand_alone_fap_mode_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *is_single_fap_mode
  );

uint32
  arad_fabric_connect_mode_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FABRIC_CONNECT_MODE fabric_mode
  );


uint32
  arad_fabric_connect_mode_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FABRIC_CONNECT_MODE fabric_mode
  );



uint32
  arad_fabric_topology_status_connectivity_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                    link_index_min,
    SOC_SAND_IN  int                    link_index_max,
    SOC_SAND_OUT ARAD_FABRIC_LINKS_CON_STAT_INFO_ARR *connectivity_map
  );

int
  arad_link_on_off_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  soc_port_t               port,
    SOC_SAND_IN  ARAD_LINK_STATE_INFO     *info
  );

soc_error_t
    arad_link_power_set(
        int unit, 
        soc_port_t port, 
        uint32 flags, 
        soc_dcmn_port_power_t power
        );


uint32
  arad_link_on_off_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 link_ndx,
    SOC_SAND_IN  ARAD_LINK_STATE_INFO     *info
  );


int
  arad_link_on_off_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  soc_port_t             port,
    SOC_SAND_OUT ARAD_LINK_STATE_INFO   *info
  );

soc_error_t
    arad_link_power_get(
        int unit, 
        soc_port_t port, 
        soc_dcmn_port_power_t* power
        );



soc_error_t
arad_fabric_reachability_status_get(
  int unit,
  int moduleid,
  int links_max,
  uint32 *links_array,
  int *links_count
);



soc_error_t
arad_fabric_link_status_all_get(
  int unit,
  int links_array_max_size,
  uint32* link_status,
  uint32* errored_token_count,
  int* links_array_count
);


soc_error_t
arad_fabric_link_status_clear(
    int unit,
    soc_port_t link
);




soc_error_t
arad_fabric_link_status_get(
  int unit,
  bcm_port_t link_id,
  uint32 *link_status,
  uint32 *errored_token_count
);



soc_error_t
arad_fabric_loopback_set(
  int unit,
  soc_port_t port,
  soc_dcmn_loopback_mode_t loopback
);



soc_error_t
arad_fabric_loopback_get(
  int unit,
  soc_port_t port,
  soc_dcmn_loopback_mode_t* loopback
);

soc_error_t arad_link_control_strip_crc_set(int unit, soc_port_t port, int strip_crc);
soc_error_t arad_link_control_strip_crc_get(int unit, soc_port_t port, int* strip_crc);

soc_error_t arad_link_control_rx_enable_set(int unit, soc_port_t port, uint32 flags, int enable);
soc_error_t arad_link_control_tx_enable_set(int unit, soc_port_t port, int enable);
soc_error_t arad_link_control_rx_enable_get(int unit, soc_port_t port, int* enable);
soc_error_t arad_link_control_tx_enable_get(int unit, soc_port_t port, int* enable);


soc_error_t 
arad_link_port_fault_get(
  int unit,
  bcm_port_t link_id,
  uint32* flags
);

soc_error_t arad_link_port_bucket_fill_rate_validate(int unit, uint32 bucket_fill_rate);


soc_error_t 
arad_fabric_links_cell_interleaving_set(int unit, soc_port_t link, int val);


soc_error_t 
arad_fabric_links_cell_interleaving_get(int unit, soc_port_t link, int* val);


soc_error_t 
arad_fabric_links_llf_control_source_set(int unit, soc_port_t link, soc_dcmn_fabric_control_source_t val);


soc_error_t 
arad_fabric_links_llf_control_source_get(int unit, soc_port_t link, soc_dcmn_fabric_control_source_t* val);


soc_error_t 
arad_fabric_links_isolate_set(int unit, soc_port_t link, soc_dcmn_isolation_status_t val);


soc_error_t 
arad_fabric_links_isolate_get(int unit, soc_port_t link, soc_dcmn_isolation_status_t* val);


soc_error_t 
arad_fabric_links_nof_links_get(int unit, int* nof_links);


soc_error_t
arad_fabric_force_signal_detect_set(int unit, int mac_instance);


uint32
  arad_fabric_cell_format_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_FABRIC_CELL_FORMAT  *info
  );



int
  arad_fabric_link_up_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  soc_port_t          port,
    SOC_SAND_OUT int                 *up
  );



uint32
  arad_fabric_nof_links_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT int                    *nof_links
  );


int
arad_fabric_port_speed_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN uint32                   port,
    SOC_SAND_OUT int                    *speed
);


uint32
arad_fabric_port_speed_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN uint32                  port,
    SOC_SAND_IN int                     speed
);



soc_error_t
arad_fabric_priority_set(
    SOC_SAND_IN  int                              unit, 
    SOC_SAND_IN  uint32                              tc, 
    SOC_SAND_IN  uint32                              dp,
    SOC_SAND_IN  uint32                              flags,
    SOC_SAND_IN  int                               fabric_priority
  );


soc_error_t
arad_fabric_priority_get(
    SOC_SAND_IN  int                              unit, 
    SOC_SAND_IN  uint32                              tc, 
    SOC_SAND_IN  uint32                              dp,
    SOC_SAND_IN  uint32                              flags,
    SOC_SAND_OUT int                                 *fabric_priority
  );


uint32
arad_fabric_wcmod_ucode_load(
    int unit,
    int port, 
    uint8 *arr, 
    int arr_len
);


uint32 arad_fabric_gci_enable_set ( 
    SOC_SAND_IN  int                                 unit,  
    SOC_SAND_IN  ARAD_FABRIC_GCI_TYPE                   type,
    SOC_SAND_OUT int                                    value
);



uint32 arad_fabric_gci_enable_get ( 
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_FABRIC_GCI_TYPE                   type,  
    SOC_SAND_OUT int                                    *value
);



uint32 arad_fabric_gci_config_set ( 
    SOC_SAND_IN  int                                 unit,  
    SOC_SAND_IN ARAD_FABRIC_GCI_CONFIG_TYPE             type,
    SOC_SAND_OUT int                                    value
);



uint32 arad_fabric_gci_config_get ( 
    SOC_SAND_IN  int                                 unit,  
    SOC_SAND_IN ARAD_FABRIC_GCI_CONFIG_TYPE             type,
    SOC_SAND_OUT int                                    *value
);



uint32 arad_fabric_llfc_threshold_set ( 
    SOC_SAND_IN  int                                 unit,  
    SOC_SAND_OUT int                                    value
);



uint32 arad_fabric_llfc_threshold_get ( 
    SOC_SAND_IN  int                                 unit,  
    SOC_SAND_OUT int                                    *value
);


uint32 arad_fabric_rci_enable_set ( 
    SOC_SAND_IN  int                                 unit,  
    SOC_SAND_OUT soc_dcmn_fabric_control_source_t        value
);



uint32 arad_fabric_rci_enable_get ( 
    SOC_SAND_IN  int                                 unit,  
    SOC_SAND_OUT soc_dcmn_fabric_control_source_t       *value
);



uint32 arad_fabric_rci_config_set ( 
    SOC_SAND_IN int                                unit,  
    SOC_SAND_IN ARAD_FABRIC_RCI_CONFIG_TYPE           rci_config_type,
    SOC_SAND_IN int                                   value
);



uint32 arad_fabric_rci_config_get ( 
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_FABRIC_RCI_CONFIG_TYPE            rci_config_type,  
    SOC_SAND_OUT int                                    *value
  );

#ifdef BCM_88660_A0

uint32
arad_fabric_empty_cell_size_set (
   SOC_SAND_IN  int                                 unit,
   SOC_SAND_IN  uint32                                 fmac_index,  
   SOC_SAND_IN  uint32                                 cell_size
);

#endif 



int 
arad_fabric_aldwp_config(
   SOC_SAND_IN  int                                 unit);



uint32 arad_fabric_minimal_links_to_dest_set(
    SOC_SAND_IN int                                  unit,
    SOC_SAND_IN soc_module_t                         module_id,
    SOC_SAND_IN int                                  minimum_links
   );




uint32 arad_fabric_minimal_links_to_dest_get(
    SOC_SAND_IN int                                 unit,
    SOC_SAND_IN soc_module_t                        module_id,
    SOC_SAND_OUT int                                *minimum_links
    );




soc_error_t arad_fabric_link_tx_traffic_disable_set(int unit, soc_port_t link, int disable);




soc_error_t arad_fabric_link_tx_traffic_disable_get(int unit, soc_port_t link, int *disable);




uint32 arad_fabric_mesh_check(int unit, uint8 stand_alone, uint8 *success);



soc_error_t arad_fabric_prbs_polynomial_set(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int value);
soc_error_t arad_fabric_prbs_polynomial_get(int unit, soc_port_t port, soc_dcmn_port_prbs_mode_t mode, int* value);


int arad_brdc_fsrd_blk_id_set(int unit);

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

