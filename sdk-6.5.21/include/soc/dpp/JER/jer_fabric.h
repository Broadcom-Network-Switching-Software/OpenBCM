/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __JER_FABRIC_INCLUDED__

#define __JER_FABRIC_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/cosq.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/error.h>




#define SOC_JER_FABRIC_PCP_LENGTH                   (2)
#define SOC_JER_FABRIC_PRIORITY_LENGTH              (2)
#define SOC_JER_FABRIC_PRIORITY_NOF                 (4)

#define SOC_JER_FABRIC_PRIORITY_NDX_MASK            (0xff)
#define SOC_JER_FABRIC_PRIORITY_NDX_NOF             (256)
#define SOC_JER_FABRIC_TDM_NDX_NOF                  (256)

#define SOC_JER_FABRIC_PRIORITY_NDX_IS_TDM_MASK     (0x1)
#define SOC_JER_FABRIC_PRIORITY_NDX_IS_TDM_OFFSET   (0)
#define SOC_JER_FABRIC_PRIORITY_NDX_TC_MASK         (0xE)
#define SOC_JER_FABRIC_PRIORITY_NDX_TC_OFFSET       (1)
#define SOC_JER_FABRIC_PRIORITY_NDX_DP_MASK         (0x30)
#define SOC_JER_FABRIC_PRIORITY_NDX_DP_OFFSET       (4)
#define SOC_JER_FABRIC_PRIORITY_NDX_IS_MC_MASK      (0x40)
#define SOC_JER_FABRIC_PRIORITY_NDX_IS_MC_OFFSET    (6)
#define SOC_JER_FABRIC_PRIORITY_NDX_IS_HP_MASK      (0x80)
#define SOC_JER_FABRIC_PRIORITY_NDX_IS_HP_OFFSET    (7)
#define SOC_JER_FABRIC_STK_FAP_GROUP_SIZE           (32)
#define SOC_JER_FABRIC_STK_MAX_IGNORED_FAP_IDS      (16)
#define SOC_JER_FABRIC_MESH_MC_FAP_GROUP_SIZE       (32)
#define SOC_JER_FABRIC_MESH_MC_REPLICATION_LENGTH   (4)
#define SOC_JER_FABRIC_MESH_MC_REPLICATION_MASK     (0xf)
#define SOC_JER_FABRIC_GROUP_CTX_LENGTH             (4)

#define SOC_JER_FABRIC_MESH_MC_REPLICATION_DEST_0_BIT    (0x1)
#define SOC_JER_FABRIC_MESH_MC_REPLICATION_DEST_1_BIT    (0x2)
#define SOC_JER_FABRIC_MESH_MC_REPLICATION_LOCAL_0_BIT   (0x4)
#define SOC_JER_FABRIC_MESH_MC_REPLICATION_LOCAL_1_BIT   (0x8)

#define SOC_JER_FABRIC_DATA_CELL_RECEIVED_SIZE_IN_BYTES  (74)

#define SOC_JER_FABRIC_DQCF_NOF_CTX_PER_IPT_CORE         (20)
#define SOC_JER_FABRIC_DTQ_NOF_CTX_PER_IPT_CORE          (6)

#define SOC_JER_FABRIC_SYNC_E_MIN_DIVIDER              (2)
#define SOC_JER_FABRIC_SYNC_E_MAX_DIVIDER              (16)









typedef struct {
    int dest_0[SOC_DPP_DEFS_MAX(MAX_NOF_FAP_ID_MAPPED_TO_DEST_LIMIT)];
    int dest_0_count;
    int dest_1[SOC_DPP_DEFS_MAX(MAX_NOF_FAP_ID_MAPPED_TO_DEST_LIMIT)];
    int dest_1_count;
    int dest_2[SOC_DPP_DEFS_MAX(MAX_NOF_FAP_ID_MAPPED_TO_DEST_LIMIT)];
    int dest_2_count;
} JER_MODID_GROUP_MAP;












uint32
  soc_jer_fabric_init(
    SOC_SAND_IN  int unit  
  );



uint32
  soc_jer_fabric_regs_init(
    SOC_SAND_IN  int         unit
  );


soc_error_t
  jer_fabric_gci_backoff_masks_init(
    SOC_SAND_IN  int                 unit
  );



soc_error_t
  jer_fabric_pcp_dest_mode_config_set(
    SOC_SAND_IN int              unit,
    SOC_SAND_IN uint32           flags,
    SOC_SAND_IN uint32           modid,
    SOC_SAND_IN uint32           pcp_mode
  );

soc_error_t
  jer_fabric_pcp_dest_mode_config_get(
    SOC_SAND_IN int              unit,
    SOC_SAND_IN uint32           flags,
    SOC_SAND_IN uint32           modid,
    SOC_SAND_OUT uint32          *pcp_mode
  );


soc_error_t
  soc_jer_cosq_gport_sched_set(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                pipe,
    SOC_SAND_IN  int                                weight,
    SOC_SAND_IN  soc_dpp_cosq_gport_fabric_pipe_t   fabric_pipe_type  
  );

soc_error_t
  soc_jer_cosq_gport_sched_get(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                pipe,
    SOC_SAND_OUT  int*                               weight,
    SOC_SAND_IN  soc_dpp_cosq_gport_fabric_pipe_t   fabric_pipe_type  
  );


uint32
  soc_jer_fabric_link_thresholds_pipe_set(
    SOC_SAND_IN int                                  unit,
    SOC_SAND_IN soc_dpp_fabric_pipe_t                pipe,  
    SOC_SAND_IN soc_dpp_fabric_link_threshold_type_t type, 
    SOC_SAND_IN int                                  value
  );

uint32
  soc_jer_fabric_link_thresholds_pipe_get(
    SOC_SAND_IN int                                  unit,
    SOC_SAND_IN soc_dpp_fabric_pipe_t                pipe,  
    SOC_SAND_IN soc_dpp_fabric_link_threshold_type_t type, 
    SOC_SAND_OUT int                                 *value
  );



soc_error_t
  soc_jer_fabric_cosq_control_backward_flow_control_set(
      SOC_SAND_IN int                                   unit,
      SOC_SAND_IN soc_gport_t                           port,
      SOC_SAND_IN int                                   enable,
      SOC_SAND_IN soc_dpp_cosq_gport_egress_core_fifo_t fifo_type
  );

soc_error_t
  soc_jer_fabric_cosq_control_backward_flow_control_get(
      SOC_SAND_IN int                                   unit,
      SOC_SAND_IN soc_gport_t                           port,
      SOC_SAND_OUT int                                  *enable,
      SOC_SAND_IN soc_dpp_cosq_gport_egress_core_fifo_t fifo_type
  );



soc_error_t
  soc_jer_fabric_egress_core_cosq_gport_sched_set(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                pipe,
    SOC_SAND_IN  int                                weight,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t   fifo_type 
  );

soc_error_t
  soc_jer_fabric_egress_core_cosq_gport_sched_get(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                pipe,
    SOC_SAND_OUT int                                *weight,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t   fifo_type 
  );



soc_error_t
  soc_jer_fabric_cosq_gport_rci_threshold_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  soc_gport_t            gport,
    SOC_SAND_IN  int                    threshold_val,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t  fifo_type
  );

soc_error_t
  soc_jer_fabric_cosq_gport_rci_threshold_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  soc_gport_t            gport,
    SOC_SAND_OUT  int                    *threshold_val,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t  fifo_type
  );



soc_error_t
  soc_jer_fabric_cosq_gport_priority_drop_threshold_set(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  soc_gport_t                            gport,
    SOC_SAND_IN  soc_cosq_threshold_t                   *threshold,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t  fifo_type
  );

soc_error_t
  soc_jer_fabric_cosq_gport_priority_drop_threshold_get(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  soc_gport_t                            gport,
    SOC_SAND_INOUT  soc_cosq_threshold_t                *threshold,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t  fifo_type
  );



uint32
  soc_jer_fabric_stack_module_all_reachable_ignore_id_set(
    SOC_SAND_IN  int                unit, 
    SOC_SAND_IN  soc_module_t       module, 
    SOC_SAND_IN  int                enable
  );

uint32
  soc_jer_fabric_stack_module_all_reachable_ignore_id_get(
    SOC_SAND_IN  int                unit, 
    SOC_SAND_IN  soc_module_t       module, 
    SOC_SAND_OUT int                *enable
  );



uint32
  soc_jer_fabric_stack_module_max_all_reachable_set(
    SOC_SAND_IN  int                unit, 
    SOC_SAND_IN  soc_module_t       max_module
  );

uint32
  soc_jer_fabric_stack_module_max_all_reachable_get(
    SOC_SAND_IN  int                unit, 
    SOC_SAND_OUT soc_module_t       *max_module
  );



uint32
  soc_jer_fabric_stack_module_max_set(
    SOC_SAND_IN  int                unit, 
    SOC_SAND_IN  soc_module_t       max_module
  );

uint32
  soc_jer_fabric_stack_module_max_get(
    SOC_SAND_IN  int                unit, 
    SOC_SAND_OUT soc_module_t       *max_module
  );

uint32
  soc_jer_fabric_stack_module_devide_by_32_verify(
    SOC_SAND_IN  int                unit, 
    SOC_SAND_IN soc_module_t        max_module
  );



soc_error_t
  soc_jer_fabric_force_set(
    SOC_SAND_IN  int                unit, 
    SOC_SAND_IN soc_dpp_fabric_force_t        force
  );
  

uint32
soc_jer_fabric_priority_bits_mapping_to_fdt_index_get(
    SOC_SAND_IN  int                        unit, 
    SOC_SAND_IN  uint32                     tc, 
    SOC_SAND_IN  uint32                     dp,
    SOC_SAND_IN  uint32                     flags,
    SOC_SAND_OUT uint32                     *index
  );


soc_error_t
soc_jer_fabric_priority_set(
    SOC_SAND_IN  int                unit, 
    SOC_SAND_IN  uint32             tc, 
    SOC_SAND_IN  uint32             dp,
    SOC_SAND_IN  uint32             flags,
    SOC_SAND_IN  int                fabric_priority
  );

soc_error_t
soc_jer_fabric_priority_get(
    SOC_SAND_IN  int                unit, 
    SOC_SAND_IN  uint32             tc, 
    SOC_SAND_IN  uint32             dp,
    SOC_SAND_IN  uint32             flags,
    SOC_SAND_OUT  int                *fabric_priority
  );


uint32
soc_jer_fabric_port_to_fmac(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  soc_port_t                     port,
    SOC_SAND_OUT  int                            *fmac_index,
    SOC_SAND_OUT  int                            *fmac_inner_link
    );

uint32
soc_jer_fabric_link_topology_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  soc_module_t                   local_dest_id, 
    SOC_SAND_IN  int                            links_count,
    SOC_SAND_IN  soc_port_t                     *links_array
  );

uint32
soc_jer_fabric_link_topology_get(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  soc_module_t                   local_dest_id, 
    SOC_SAND_IN  int                            max_links_count,
    SOC_SAND_OUT int                            *links_count, 
    SOC_SAND_OUT soc_port_t                     *links_array
  );

uint32
soc_jer_fabric_link_topology_unset(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  soc_module_t                   local_dest_id
  );

soc_error_t
soc_jer_fabric_multicast_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  soc_multicast_t                mc_id,
    SOC_SAND_IN  uint32                         destid_count,
    SOC_SAND_IN  soc_module_t                   *destid_array
  );

uint32
soc_jer_fabric_multicast_get(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  soc_multicast_t                mc_id,
    SOC_SAND_IN  uint32                         destid_count_max,
    SOC_SAND_OUT int                            *destid_count,
    SOC_SAND_OUT soc_module_t                   *destid_array
  );

uint32 
soc_jer_fabric_local_dest_id_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  soc_module_t                   local_dest_id
  );

uint32
soc_jer_fabric_modid_group_get(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  soc_module_t                   local_dest,
    SOC_SAND_IN  int                            modid_max_count,
    SOC_SAND_OUT soc_module_t                   *modid_array,
    SOC_SAND_OUT int                            *modid_count
  );

uint32
soc_jer_fabric_modid_group_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  soc_module_t                   local_dest,
    SOC_SAND_IN  int                            modid_count,
    SOC_SAND_IN  soc_module_t                   *modid_array
  );

int
  soc_jer_fabric_mode_validate(
    SOC_SAND_IN int                           unit, 
    SOC_SAND_OUT SOC_TMC_FABRIC_CONNECT_MODE   *fabric_connect_mode
  );
  

uint32
soc_jer_fabric_minimal_links_to_dest_set(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN soc_module_t    module_id,
    SOC_SAND_IN int             num_of_links
  );

uint32
soc_jer_fabric_minimal_links_to_dest_get(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN soc_module_t    module_id,
    SOC_SAND_OUT int            *num_of_links
  );



uint32
soc_jer_fabric_minimal_links_all_reachable_set(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN int             num_of_links
  );

uint32
soc_jer_fabric_minimal_links_all_reachable_get(
    SOC_SAND_IN int          unit,
    SOC_SAND_OUT int            *num_of_links
  );


uint32
  soc_jer_fabric_rci_thresholds_config_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  soc_dpp_fabric_rci_config_t    rci_config);

uint32
  soc_jer_fabric_rci_thresholds_config_get(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_OUT soc_dpp_fabric_rci_config_t    *rci_config);


uint32
  soc_jer_fabric_link_repeater_enable_set(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_IN  int enable,
    SOC_SAND_IN  int empty_cell_size
  );

uint32
  soc_jer_fabric_link_repeater_enable_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_OUT int *enable,
    SOC_SAND_OUT int *empty_cell_size
  );


uint32
  soc_jer_fabric_load_balance_init(
    SOC_SAND_IN  int            unit
  );


uint32
soc_jer_fabric_cell_cpu_data_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_OUT uint32                  *cell_buffer
  );
  
  
uint32
  soc_jer_fabric_queues_info_get(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_OUT soc_dpp_fabric_queues_info_t    *queues_info
  );


uint32 
  soc_jer_fabric_mesh_topology_get(
    SOC_SAND_IN  int                                     unit, 
    SOC_SAND_OUT soc_dcmn_fabric_mesh_topology_diag_t    *mesh_topology_diag
  );


uint32 
  soc_jer_fabric_mesh_check(
    SOC_SAND_IN  int                                     unit, 
    SOC_SAND_IN uint8                                    stand_alone,
    SOC_SAND_OUT uint8                                   *success
  );


uint32 
  soc_jer_fabric_rx_fifo_status_get(
    SOC_SAND_IN  int                                unit, 
    SOC_SAND_OUT soc_dpp_fabric_rx_fifo_diag_t     *rx_fifo_diag
  );


uint32
  soc_jer_fabric_port_sync_e_link_set(
    SOC_SAND_IN  int                                unit, 
    SOC_SAND_IN  int                                is_master, 
    SOC_SAND_IN  int                                port
  );

uint32
  soc_jer_fabric_port_sync_e_link_get(
    SOC_SAND_IN  int                                unit, 
    SOC_SAND_IN  int                                is_master, 
    SOC_SAND_OUT int                                *port
  );


uint32 
  soc_jer_fabric_port_sync_e_divider_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             is_master,
    SOC_SAND_IN  int             divider
  );

uint32 
  soc_jer_fabric_port_sync_e_divider_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             is_master,
    SOC_SAND_OUT int             *divider
  );



uint32
  soc_jer_fabric_sync_e_enable_set(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                is_master,
    SOC_SAND_IN  int                                to_enable
  );

uint32
  soc_jer_fabric_sync_e_enable_get(
    SOC_SAND_IN  int                                unit, 
    SOC_SAND_IN  int                                is_master, 
    SOC_SAND_OUT int                                *is_fabric_synce
  );




int
  soc_jer_fabric_efms_enable_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32             modid,
    SOC_SAND_IN  int                val
  );

int
  soc_jer_fabric_efms_enable_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32             modid,
    SOC_SAND_OUT  int               *val
  );



soc_error_t
soc_jer_fabric_link_config_ovrd(
  int                unit
);

#ifdef BCM_FLAIR_SUPPORT
#define SOC_JER_FABRIC_FGL_INGRESS_TDM_MIN_SIZE        (65)
#define SOC_JER_FABRIC_FGL_INGRESS_TDM_MAX_SIZE        (256)
#define FABRIC_FGL_EGR_MC_MAPPING_MODE_MC_ID           (0)
#define FABRIC_FGL_EGR_MC_MAPPING_MODE_EGR_MCT_MAPPING (1)
#define FABRIC_ECI_TDM_CONFIG_ENTRY_NUMBER             (160)
#define FABRIC_ECI_EGR_PORT_MAP_ENTRY_NUMBER           (256)
#define FABRIC_ECI_EGR_MCT_ENTRY_NUMBER                (160)
#define FABRIC_ECI_EGR_MCT_MAPPING_ENTRY_NUMBER        (0x3fff)

typedef enum soc_fabric_fgl_egress_ftmh_mode_type_e {
    NINE_BYTE_STANDARD_FTMH = 0x1,
    FOUR_BYTE_TDM_FTMH = 0x2
} soc_fabric_fgl_egress_ftmh_mode_type_t;

typedef struct soc_fabric_fgl_egress_config_s {
    int egress_drop_threshold; 
    int egress_almost_full_threshold; 
    soc_fabric_fgl_egress_ftmh_mode_type_t egress_ftmh_mode;
    int egress_packet_crc;
    int egress_packet_crc_discard;
} soc_fabric_fgl_egress_config_t;

typedef enum soc_fabric_fgl_egr_to_ing_lpck_mode_type_e {
    ALL_CHANNELS = 0x0,
    ONLY_SELECTED_CHANNELS = 0x1
} soc_fabric_fgl_egr_to_ing_lpck_mode_type_t;

typedef struct soc_fabric_fgl_loopback_config_s {
    int egr_to_ing_lpck;
    int ing_to_egr_lpck;
    soc_fabric_fgl_egr_to_ing_lpck_mode_type_t egr_to_ing_lpck_mode;
    uint32 egr_to_ing_lpck_framer_en[5];
} soc_fabric_fgl_loopback_config_t;

typedef struct soc_fabric_fgl_ingress_tdm_context_drop_s {
    uint32 bitmap[5];
} soc_fabric_fgl_ingress_tdm_context_drop_t;

typedef struct soc_fabric_fgl_ingress_config_s {
    int    tdm_min_size; 
    int    tdm_max_size; 
} soc_fabric_fgl_ingress_config_t;

typedef struct soc_fabric_eci_egr_tdm_config_s {
  uint32 mode;
  uint32 cpu;
  uint32 header[3];
  uint32 link_mask_ptr;
  uint32 add_packet_crc;
  uint32 mc_replication;
} soc_fabric_eci_egr_tdm_config_t;

typedef struct soc_fabric_eci_egr_port_map_s {
  uint32 channel_id;
  uint32 interface_id;
} soc_fabric_eci_egr_port_map_t;

typedef struct soc_fabric_eci_egr_mct_s {
    uint32 channel_id_0_valid;
    uint32 channel_id_0;
    uint32 channel_id_1_valid;
    uint32 channel_id_1;
    uint32 channel_id_2_valid;
    uint32 channel_id_2;
    uint32 channel_id_3_valid;
    uint32 channel_id_3;
} soc_fabric_eci_egr_mct_t;

typedef struct soc_fabric_eci_egr_mct_mapping_s {
    uint32 mc_map_0;
    uint32 mc_map_1;
    uint32 mc_map_2;
    uint32 mc_map_3;
    uint32 mc_map_4;
    uint32 mc_map_5;
    uint32 mc_map_6;
    uint32 mc_map_7;
} soc_fabric_eci_egr_mct_mapping_t;


soc_error_t
  soc_jer_fabric_fgl_egress_config_get(
    SOC_SAND_IN int                unit,
    SOC_SAND_OUT soc_fabric_fgl_egress_config_t    *fgl_egress_config);

soc_error_t
  soc_jer_fabric_fgl_egress_config_set(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN soc_fabric_fgl_egress_config_t    fgl_egress_config);


soc_error_t
  soc_jer_fabric_fgl_ingress_config_get(
    SOC_SAND_IN int                unit,
    SOC_SAND_OUT soc_fabric_fgl_ingress_config_t    *fgl_ingress_config);

soc_error_t
  soc_jer_fabric_fgl_ingress_config_set(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN soc_fabric_fgl_ingress_config_t    fgl_ingress_config);


soc_error_t
  soc_jer_fabric_fgl_loopback_config_get(
    SOC_SAND_IN int                unit,
    SOC_SAND_OUT soc_fabric_fgl_loopback_config_t    *fgl_loopback_config);

soc_error_t
  soc_jer_fabric_fgl_loopback_config_set(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN soc_fabric_fgl_loopback_config_t    fgl_loopback_config);


soc_error_t
  soc_jer_fabric_fgl_ingress_tdm_context_drop_get(
    SOC_SAND_IN int                unit,
    SOC_SAND_OUT soc_fabric_fgl_ingress_tdm_context_drop_t    *tdm_context_drop);

soc_error_t
  soc_jer_fabric_fgl_ingress_tdm_context_drop_set(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN soc_fabric_fgl_ingress_tdm_context_drop_t    tdm_context_drop);


soc_error_t
  soc_jer_fabric_egr_mc_mapping_mode_set(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN int    egr_mc_mapping_mode);

soc_error_t
  soc_jer_fabric_egr_mc_mapping_mode_get(
    SOC_SAND_IN int                unit,
    SOC_SAND_OUT int    *egr_mc_mapping_mode);


soc_error_t
  soc_jer_fabric_eci_tdm_config_table_get(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN uint32                index,
    SOC_SAND_OUT soc_fabric_eci_egr_tdm_config_t    *eci_egr_tdm_config);

soc_error_t
  soc_jer_fabric_eci_tdm_config_table_set(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN uint32                index,
    SOC_SAND_IN soc_fabric_eci_egr_tdm_config_t    eci_egr_tdm_config);


soc_error_t
  soc_jer_fabric_eci_egr_port_map_table_get(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN uint32                index,
    SOC_SAND_OUT soc_fabric_eci_egr_port_map_t    *egr_port_map);

soc_error_t
  soc_jer_fabric_eci_egr_port_map_table_set(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN uint32                index,
    SOC_SAND_IN soc_fabric_eci_egr_port_map_t    egr_port_map);


soc_error_t
  soc_jer_fabric_eci_egr_mct_table_get(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN uint32                index,
    SOC_SAND_OUT soc_fabric_eci_egr_mct_t    *eci_egr_mct);

soc_error_t
  soc_jer_fabric_eci_egr_mct_table_set(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN uint32                index,
    SOC_SAND_IN soc_fabric_eci_egr_mct_t    eci_egr_mct);


soc_error_t
  soc_jer_fabric_eci_egr_mct_mapping_table_get(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN uint32                index,
    SOC_SAND_OUT soc_fabric_eci_egr_mct_mapping_t    *eci_egr_mct_mapping);

soc_error_t
  soc_jer_fabric_eci_egr_mct_mapping_table_set(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN uint32                index,
    SOC_SAND_IN soc_fabric_eci_egr_mct_mapping_t    eci_egr_mct_mapping);
#endif 

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif
