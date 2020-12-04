/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: jer_mgmt.h
 */

#ifndef __JER_MGMT_INCLUDED__

#define __JER_MGMT_INCLUDED__

#include <shared/swstate/sw_state.h>

#define NOF_FIFO_DMA_CHANNELS (12)
#define NOF_DMA_FIFO_PER_CMC (4)

typedef enum dma_fifo_channel_src_e {
    dma_fifo_channel_src_crps_0_to_3 = 0x0,
    dma_fifo_channel_src_crps_4_to_7,
    dma_fifo_channel_src_crps_8_to_11,    
    dma_fifo_channel_src_crps_12_to_15,        
    dma_fifo_channel_src_oam_status,
    dma_fifo_channel_src_oam_event,
    dma_fifo_channel_src_olp,
    dma_fifo_channel_src_ppdb_cpu_reply,    
    dma_fifo_channel_src_max,
    dma_fifo_channel_src_reserved = 0xF
} dma_fifo_channel_src_t;


typedef struct jer_mgmt_dma_fifo_source_channels_s {
    int dma_fifo_source_channels_array[dma_fifo_channel_src_max]; 
} jer_mgmt_dma_fifo_source_channels_t; 


typedef struct jer_mgmt_fda_fabfifos_prio_drop_thresholds_s {
    uint32 fda_fabfif_0_prio_0_drop_th;
    uint32 fda_fabfif_0_prio_1_drop_th;
    uint32 fda_fabfif_0_prio_2_drop_th;
    uint32 fda_fabfif_1_prio_0_drop_th;
    uint32 fda_fabfif_1_prio_1_drop_th;
    uint32 fda_fabfif_1_prio_2_drop_th;
    uint32 fda_fabfif_2_prio_0_drop_th;
    uint32 fda_fabfif_2_prio_1_drop_th;
    uint32 fda_fabfif_2_prio_2_drop_th;
} jer_mgmt_fda_fabfifos_prio_drop_thresholds_t;


typedef struct jer_mgmt_enable_traffic_config_s {
    jer_mgmt_fda_fabfifos_prio_drop_thresholds_t fda_fabfifos_prio_drop_thresholds;
} jer_mgmt_enable_traffic_config_t;


int
  jer_mgmt_credit_worth_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32              credit_worth
  ) ;

int
   jer_mgmt_credit_worth_get(
           SOC_SAND_IN  int                 unit,
           SOC_SAND_OUT uint32              *credit_worth
          ) ;



uint32
  jer_mgmt_credit_worth_remote_set(
    SOC_SAND_IN  int    unit,
	SOC_SAND_IN  uint32    credit_worth_remote
  ) ;


uint32
  jer_mgmt_credit_worth_remote_get(
    SOC_SAND_IN  int    unit,
	SOC_SAND_OUT uint32    *credit_worth_remote
  ) ;


uint32
  jer_mgmt_module_to_credit_worth_map_set(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32    fap_id,
    SOC_SAND_IN  uint32    credit_value_type 
  ) ;


uint32
  jer_mgmt_module_to_credit_worth_map_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32    fap_id,
    SOC_SAND_OUT uint32    *credit_value_type 
  ) ;



uint32
  jer_mgmt_change_all_faps_credit_worth_unsafe(
    SOC_SAND_IN  int    unit,
    SOC_SAND_OUT uint8     credit_value_to_use
  ) ;





uint32 jer_mgmt_system_fap_id_set(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  uint32  sys_fap_id
  );


uint32
  jer_mgmt_system_fap_id_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint32              *sys_fap_id
  );



int jer_mgmt_temp_pvt_get(int unit, int temperature_max, soc_switch_temperature_monitor_t *temperature_array, int *temperature_count);
int jer_mgmt_drv_pvt_monitor_enable(int unit);
int jer_mgmt_pvt_monitor_correction_enable(int unit, int interval);
int jer_mgmt_pvt_monitor_correction_disable(int unit);
int jer_mgmt_pvt_monitor_correction_status_get(int unit, uint8  *enable, int *interval, int *counter);
int jer_mgmt_pvt_monitor_db_init (int unit);
int jer_mgmt_pvt_correction_once(int unit, pbmp_t pbmp);


int jer_mgmt_revision_fixes (int unit);


uint32
  jer_mgmt_enable_traffic_set(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  uint8                              enable
  );

uint32
  jer_mgmt_enable_traffic_get(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_OUT uint8                              *enable
  );


uint32
  jer_mgmt_enable_traffic_advanced_set(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  jer_mgmt_enable_traffic_config_t   *enable_traffic_config,
    SOC_SAND_IN  uint8                              enable
  );

uint32
  jer_mgmt_enable_traffic_advanced_get(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_OUT jer_mgmt_enable_traffic_config_t   *enable_traffic_config,
    SOC_SAND_OUT uint8                              *enable
  );


uint32
  jer_mgmt_enable_traffic_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  jer_mgmt_enable_traffic_config_t   *enable_traffic_config,
    SOC_SAND_IN  uint8                 enable
  );

uint32
  jer_mgmt_enable_traffic_get_unsafe(
    SOC_SAND_IN   int unit,
    SOC_SAND_OUT  jer_mgmt_enable_traffic_config_t   *enable_traffic_config,
    SOC_SAND_OUT  uint8 *enable
  );



uint32 jer_mgmt_set_mru_by_dbuff_size(
    SOC_SAND_IN  int     unit
  );

int jer_mgmt_dma_fifo_channel_free_find(SOC_SAND_IN int unit, SOC_SAND_IN uint8 skip_pci_cmc, SOC_SAND_OUT int * channel_number);
int jer_mgmt_dma_fifo_channel_set(SOC_SAND_IN int unit, SOC_SAND_IN int channel, SOC_SAND_IN dma_fifo_channel_src_t value);
int jer_mgmt_dma_fifo_channel_get (SOC_SAND_IN int unit, SOC_SAND_IN dma_fifo_channel_src_t source, SOC_SAND_OUT int* channel);
int jer_mgmt_dma_fifo_source_channels_db_init (int unit);

int
  jer_mgmt_voq_is_ocb_eligible_get(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  int         core_id,
    SOC_SAND_IN  uint32      qid,
    SOC_SAND_OUT uint32      *is_ocb_eligible
  );

int jer_mgmt_avs_value_get(
    SOC_SAND_IN    int      unit,
    SOC_SAND_OUT   uint32*  avs_val);


int jer_mgmt_mirror_snoop_forward_original_when_dropped_set(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  uint8       enabled
  );


int jer_mgmt_mirror_snoop_forward_original_when_dropped_get(
    SOC_SAND_IN  int         unit,
    SOC_SAND_OUT uint8       *enabled
  );

#endif 

