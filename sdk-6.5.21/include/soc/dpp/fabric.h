/*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*
* This file contains structures and functions declarations for 
* In-band cell configuration and Source Routed Cell.
 */
#ifndef _SOC_DPP_FABRIC_H
#define _SOC_DPP_FABRIC_H


#include <soc/dpp/TMC/tmc_api_cell.h>
#include <soc/dpp/TMC/tmc_api_fabric.h>
#include <soc/error.h>
#include <soc/dcmn/dcmn_fabric_source_routed_cell.h>
#include <soc/dcmn/dcmn_fabric_cell.h>


#define SOC_FABRIC_QUEUE_PRIORITY_HIGH_ONLY _SHR_FABRIC_QUEUE_PRIORITY_HIGH_ONLY 
#define SOC_FABRIC_PRIORITY_MULTICAST       _SHR_FABRIC_PRIORITY_MULTICAST
#define SOC_FABRIC_PRIORITY_TDM             _SHR_FABRIC_PRIORITY_TDM

#define SOC_FABRIC_INBAND_ROUTE_GRP_SET(id) (id+2048)
#define SOC_FABRIC_INBAND_ROUTE_GRP_GET(id) (id-2048)
#define SOC_FABRIC_INBAND_IS_GROUP(id) (id>=2048)

#define SOC_DPP_FABRIC_PIPES_CONFIG(unit)          SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_pipe_map_config
#define SOC_DPP_PROPERTY_UNAVAIL                   (-1)

#define SOC_DPP_FABRIC_GROUP_MODID_BIT             \
(1<<31)  
#define SOC_DPP_FABRIC_GROUP_MODID_SET(id)  \
id = id | SOC_DPP_FABRIC_GROUP_MODID_BIT; 
#define SOC_DPP_FABRIC_MODID_IS_GROUP(modid)  \
        (modid & SOC_DPP_FABRIC_GROUP_MODID_BIT ? 1 : 0)  
#define SOC_DPP_FABRIC_GROUP_MODID_UNSET(id)  \
id & ~SOC_DPP_FABRIC_GROUP_MODID_BIT 

#define SOC_DPP_FABRIC_NUM_OF_RCI_LEVELS        (7)
#define SOC_DPP_FABRIC_NUM_OF_RCI_SEVERITIES    (3)

#define SOC_DPP_MAX_NUM_OF_FABRIC_DTQS          (6) 
#define SOC_DPP_MAX_NUM_OF_LOCAL_DTQS           (2) 

typedef enum soc_dpp_fabric_pipe_e {
    socDppFabricPipe0 = 0,     
    socDppFabricPipe1 = 1,     
    socDppFabricPipe2 = 2,     
    socDppFabricPipeAll = -1   
} soc_dpp_fabric_pipe_t;

typedef enum soc_dpp_fabric_force {
    socDppFabricForceFabric = 0,     
    socDppFabricForceLocal0 = 1,     
    socDppFabricForceLocal1 = 2,     
    socDppFabricForceLocal = 3,     
    socDppFabricForceRestore = 4,     
    socDppFabricForceNotSupported = 5     
} soc_dpp_fabric_force_t;

typedef enum soc_dpp_fabric_link_threshold_type_e {
    socDppFabricLinkRxFifoLLFC,
    socDppFabricLinkRxRciLvl1FC,
    socDppFabricLinkRxRciLvl2FC,
    socDppFabricLinkRxRciLvl3FC
} soc_dpp_fabric_link_threshold_type_t;

typedef struct soc_dpp_fabric_topology_status_s {
    SOC_TMC_FABRIC_CONNECT_MODE fabric_connect_mode;
    int is_mesh_mc;
    int fap_id_group0_count;
    int fap_id_group1_count;
    int fap_id_group2_count;
    soc_module_t fap_id_group0_array[SOC_DPP_DEFS_MAX(MAX_NOF_FAP_ID_MAPPED_TO_DEST_LIMIT)];
    soc_module_t fap_id_group1_array[SOC_DPP_DEFS_MAX(MAX_NOF_FAP_ID_MAPPED_TO_DEST_LIMIT)];
    soc_module_t fap_id_group2_array[SOC_DPP_DEFS_MAX(MAX_NOF_FAP_ID_MAPPED_TO_DEST_LIMIT)];
    int links_group0_count;
    int links_group1_count;
    int links_group2_count;
    soc_module_t links_group0_array[SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS)];
    soc_module_t links_group1_array[SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS)];
    soc_module_t links_group2_array[SOC_DPP_DEFS_MAX(NOF_FABRIC_LINKS)];
} soc_dpp_fabric_topology_status_t;

typedef struct soc_dpp_fabric_rci_config_s {
    uint32 soc_dpp_fabric_rci_core_level_thresholds[SOC_DPP_FABRIC_NUM_OF_RCI_LEVELS]; 
    uint32 soc_dpp_fabric_rci_device_level_thresholds[SOC_DPP_FABRIC_NUM_OF_RCI_LEVELS];
    uint32 soc_dpp_fabric_rci_severity_factors[SOC_DPP_FABRIC_NUM_OF_RCI_SEVERITIES];
    uint32 soc_dpp_fabric_rci_high_score_fabric_rx_queue;
    uint32 soc_dpp_fabric_rci_high_score_fabric_rx_local_queue;
    uint32 soc_dpp_fabric_rci_threshold_num_of_congested_links;
    uint32 soc_dpp_fabric_rci_high_score_congested_links; 
} soc_dpp_fabric_rci_config_t;

typedef struct soc_dpp_fabric_queue_info_s {
    uint32 info;
    uint32 is_valid;
} soc_dpp_fabric_queue_info_t;

typedef struct soc_dpp_dtq_info_s {
    uint32 soc_dpp_dtq_occ_val;
    uint32 soc_dpp_dtq_min_occ_val;
    uint32 soc_dpp_dtq_max_occ_val;
    uint32 is_valid;
} soc_dpp_dtq_info_t;

typedef struct soc_dpp_fabric_queues_info_s {

    
    soc_dpp_fabric_queue_info_t soc_dpp_fabric_dtq_data_max_occ_ctx;
    soc_dpp_fabric_queue_info_t soc_dpp_fabric_dtq_data_max_occ_val;
    soc_dpp_fabric_queue_info_t soc_dpp_fabric_dtq_ctrl_max_occ_ctx;
    soc_dpp_fabric_queue_info_t soc_dpp_fabric_dtq_ctrl_max_occ_val;
    soc_dpp_fabric_queue_info_t soc_dpp_fabric_dblf0_max_occ_ctx;
    soc_dpp_fabric_queue_info_t soc_dpp_fabric_dblf0_max_occ_val;
    soc_dpp_fabric_queue_info_t soc_dpp_fabric_dblf1_max_occ_ctx;
    soc_dpp_fabric_queue_info_t soc_dpp_fabric_dblf1_max_occ_val;

    
    soc_dpp_dtq_info_t soc_dpp_fabric_dtq_info[SOC_DPP_MAX_NUM_OF_FABRIC_DTQS];
    soc_dpp_dtq_info_t soc_dpp_local_dtq_info[SOC_DPP_MAX_NUM_OF_LOCAL_DTQS];

    soc_dpp_fabric_queue_info_t soc_dpp_fabric_sram_dqcf_max_occ_ctx;
    soc_dpp_fabric_queue_info_t soc_dpp_fabric_sram_dqcf_max_occ_val;
    soc_dpp_fabric_queue_info_t soc_dpp_fabric_dram_dblf_max_occ_ctx;
    soc_dpp_fabric_queue_info_t soc_dpp_fabric_dram_dblf_max_occ_val;

}soc_dpp_fabric_queues_info_t;

typedef struct soc_dpp_fabric_rx_fifo_diag_s {
    uint32 soc_dpp_fabric_egq0_pipe1_watermark;
    uint32 soc_dpp_fabric_egq0_pipe2_watermark;
    uint32 soc_dpp_fabric_egq0_pipe3_watermark;
    uint32 soc_dpp_fabric_egq1_pipe1_watermark;
    uint32 soc_dpp_fabric_egq1_pipe2_watermark;
    uint32 soc_dpp_fabric_egq1_pipe3_watermark;
}soc_dpp_fabric_rx_fifo_diag_t;




int 
  soc_dpp_fabric_inband_route_set(
    int unit, 
    int route_id, 
    soc_fabric_inband_route_t *route
  );

int 
  soc_dpp_fabric_inband_route_get(
    int unit, 
    int route_id, 
    soc_fabric_inband_route_t *route
  );







int 
  soc_dpp_fabric_inband_route_group_set(
    int unit, 
    int group_id, 
    int flags, 
    int route_count, 
    int *route_ids
  );

int
  soc_dpp_fabric_inband_route_group_get(
    int unit, 
    int group_id, 
    int flags, 
    int route_count_max, 
    int *route_count, 
    int *route_ids
  );







int 
  soc_dpp_fabric_mem_read(
    int unit, 
    uint32 flags, 
    int route_id, 
    int max_count,  
    soc_mem_t *mem, 
    int *copyno,
    int *index,
    void **entry_data, 
    int *array_count
  ); 

int
  soc_dpp_fabric_mem_write(
     int unit, 
     uint32 flags, 
     int route_id, 
     int array_count,
     soc_mem_t *mem, 
     int *copyno,
     int *index,
     void **entry_data 
  ); 






int 
  soc_dpp_fabric_reg_read(
    int unit, 
    uint32 flags, 
    int route_id,
    int max_count, 
    soc_reg_t *reg, 
    int *port,
    int *index,
    uint64 *data, 
    int *array_count
  );

int 
  soc_dpp_fabric_reg_write(
   int unit, 
   uint32 flags, 
   int route_id,
   int array_count, 
   soc_reg_t *reg, 
   int *port,
   int *index,
   uint64 *data
  ); 



 



uint32
soc_dpp_fabric_sr_cell_send( 
  int unit, 
  uint32 flags, 
  soc_dcmn_fabric_route_t* route,
  uint32 data_in_size, 
  uint32 *data_in
);



uint32
soc_dpp_fabric_sr_cell_receive(
  int unit, 
  uint32 flags, 
  uint32 data_out_max_size, 
  uint32 *data_out_size,
  uint32 *data_out
);


int
soc_dpp_fabric_inband_route2sr_link_list(
  int unit,
  const soc_fabric_inband_route_t* route,
  SOC_TMC_SR_CELL_LINK_LIST *sr_link_list
); 
 
 
int
soc_dpp_fabric_route2sr_link_list(
  int unit,
  const soc_dcmn_fabric_route_t* route,
  dcmn_sr_cell_link_list_t *sr_link_list
);



int
soc_dpp_fabric_reg_above_64_read(
  int unit, uint32 flags,
  int route_id,
  int max_count,
  soc_reg_t *reg,
  int *port,
  int *index,
  soc_reg_above_64_val_t *data,
  int *array_count
); 

int
soc_dpp_fabric_reg_above_64_write(
  int unit, uint32 flags,
  int route_id, int array_count,
  soc_reg_t *reg,
  int *port,
  int *index,
  soc_reg_above_64_val_t *data
); 



soc_error_t
soc_dpp_fabric_reachability_status_get(
  int unit,
  int moduleid,
  int links_max,
  uint32 *links_array,
  int *links_count
);



soc_error_t
soc_dpp_fabric_link_status_all_get(
  int unit,
  int links_array_max_size,
  uint32* link_status,
  uint32* errored_token_count,
  int* links_array_count
);



soc_error_t
soc_dpp_fabric_link_status_get(
  int unit,
  bcm_port_t link_id,
  uint32 *link_status,
  uint32 *errored_token_count
);


soc_error_t
soc_dpp_fabric_pipes_config(
    int unit
);


uint32
soc_dpp_fabric_topology_status_get(
    int unit, 
    soc_dpp_fabric_topology_status_t *topo_stat
);


uint32
soc_dpp_fabric_cell_parse(
  int unit, 
  soc_dcmn_fabric_cell_entry_t entry, 
  vsc256_sr_cell_t *cell
);

uint32 
soc_dpp_fabric_rx_fifo_diag_get(
  int unit, 
  soc_dpp_fabric_rx_fifo_diag_t *rx_fifo_diag
);

uint32
soc_dpp_fabric_force_set(
    int unit, 
    soc_dpp_fabric_force_t force 
);


uint32
soc_dpp_fabric_sync_e_enable_get(
    int unit, 
    int is_master, 
    int *is_fabric_synce
);

#endif  
