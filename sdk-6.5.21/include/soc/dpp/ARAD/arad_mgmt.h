/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __ARAD_MGMT_INCLUDED__

#define __ARAD_MGMT_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_api_mgmt.h>
#include <soc/dpp/drv.h>






#define ARAD_MGMT_PCKT_SIZE_BYTES_VSC_MIN      64

#define ARAD_MGMT_PCKT_SIZE_BYTES_VSC_MAX      (16*1024 - 128)

#define ARAD_MGMT_PCKT_SIZE_BYTES_FSC_MIN      33

#define ARAD_MGMT_PCKT_SIZE_BYTES_FSC_MAX      (16*1024 - 128)


#define ARAD_MGMT_PCKT_SIZE_BYTES_EXTERN_MAX      (ARAD_MGMT_PCKT_SIZE_BYTES_FSC_MAX    \
                                                   + (ARAD_MGMT_PCKT_RNG_NIF_CRC_BYTES  \
                                                   - ARAD_MGMT_PCKT_RNG_DRAM_CRC_BYTES))

#define ARAD_MGMT_IDR_NUM_OF_CONTEXT            (192)

#define ARAD_MGMT_MAX_BUFFERS_PER_PACKET        (32)


#define ARAD_MGMT_ALL_CTRL_CELLS_FLAGS_NONE           0
#define ARAD_MGMT_ALL_CTRL_CELLS_FLAGS_SOFT_RESET     0x1











typedef struct soc_dpp_config_arad_plus_s {
    uint16 nof_remote_faps_with_remote_credit_value; 
    uint32 per_module_credit_value[ARAD_PLUS_CREDIT_VALUE_MODE_WORDS];
} soc_dpp_config_arad_plus_t;












uint32
  arad_register_device_unsafe(
             uint32                  *base_address,
    SOC_SAND_IN  SOC_SAND_RESET_DEVICE_FUNC_PTR reset_device_ptr,
    SOC_SAND_INOUT int                 *unit_ptr
  );


uint32
  arad_unregister_device_unsafe(
    SOC_SAND_IN  int                 unit
  );


uint32
  arad_mgmt_credit_worth_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  credit_worth
  );


uint32
  arad_mgmt_credit_worth_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32              credit_worth
  );


int
  arad_mgmt_credit_worth_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint32              *credit_worth
  );


uint32
  arad_plus_mgmt_module_to_credit_worth_map_set_unsafe(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32    fap_id,
    SOC_SAND_IN  uint32    credit_value_type 
  );

uint32
  arad_plus_mgmt_module_to_credit_worth_map_get_unsafe(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32    fap_id,
    SOC_SAND_OUT uint32    *credit_value_type 
  );



uint32
  arad_mgmt_system_fap_id_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_fap_id
  );


uint32
  arad_mgmt_system_fap_id_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_fap_id
  );


uint32
  arad_mgmt_system_fap_id_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint32                 *sys_fap_id
  );

uint32
  arad_mgmt_tm_domain_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 tm_domain
  );

uint32
  arad_mgmt_tm_domain_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 tm_domain
  );

uint32
  arad_mgmt_tm_domain_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint32                 *tm_domain
  );

uint32
  arad_mgmt_all_ctrl_cells_enable_get_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_OUT  uint8  *enable
  );



uint32
  arad_mgmt_all_ctrl_cells_enable_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 enable,
    SOC_SAND_IN  uint32                 flags
  );

uint32
arad_mgmt_mesh_topology_state_modify(
    SOC_SAND_IN  int                 unit
  );


uint32
  arad_mgmt_all_ctrl_cells_enable_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 enable
  );


uint32
  arad_force_tdm_bypass_traffic_to_fabric_set_unsafe(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  int     enable
  );

uint32
  arad_force_tdm_bypass_traffic_to_fabric_get_unsafe(
    SOC_SAND_IN  int     unit,
    SOC_SAND_OUT int     *enable
  );


uint32
  arad_mgmt_enable_traffic_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 enable
  );


uint32
  arad_mgmt_enable_traffic_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 enable
  );


uint32
  arad_mgmt_enable_traffic_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_OUT uint8  *enable
  );


uint32
  arad_mgmt_max_pckt_size_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      port_ndx,
    SOC_SAND_IN  ARAD_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    SOC_SAND_IN  uint32                       max_size
  );

uint32
  arad_mgmt_max_pckt_size_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      port_ndx,
    SOC_SAND_IN  ARAD_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    SOC_SAND_IN  uint32                       max_size
  );

uint32
  arad_mgmt_max_pckt_size_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      port_ndx,
    SOC_SAND_IN  ARAD_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx
  );


uint32
  arad_mgmt_max_pckt_size_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      port_ndx,
    SOC_SAND_IN  ARAD_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    SOC_SAND_OUT uint32                       *max_size
  );
uint32
  arad_mgmt_pckt_size_range_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    SOC_SAND_IN  ARAD_MGMT_PCKT_SIZE          *size_range
  );

uint32
  arad_mgmt_pckt_size_range_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    SOC_SAND_OUT ARAD_MGMT_PCKT_SIZE          *size_range
  );


uint32
  arad_mgmt_set_mru_by_dbuff_size(
    SOC_SAND_IN  int     unit
  );

uint8
  arad_mgmt_is_pp_enabled(
    SOC_SAND_IN int unit
  );


uint32
  arad_mgmt_ocb_mc_range_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      range_ndx,
    SOC_SAND_IN  ARAD_MGMT_OCB_MC_RANGE         *range
  );

uint32
  arad_mgmt_ocb_mc_range_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      range_ndx,
    SOC_SAND_OUT ARAD_MGMT_OCB_MC_RANGE         *range
  );


uint32
  arad_mgmt_ocb_voq_eligible_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    q_category_ndx,
    SOC_SAND_IN  uint32                    rate_class_ndx,
    SOC_SAND_IN  uint32                    tc_ndx,
    SOC_SAND_IN  ARAD_MGMT_OCB_VOQ_INFO       *info,
    SOC_SAND_OUT ARAD_MGMT_OCB_VOQ_INFO    *exact_info
  );

uint32
  arad_mgmt_ocb_voq_eligible_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    q_category_ndx,
    SOC_SAND_IN  uint32                    rate_class_ndx,
    SOC_SAND_IN  uint32                    tc_ndx,
    SOC_SAND_OUT ARAD_MGMT_OCB_VOQ_INFO       *info
  );



uint32
soc_arad_cache_table_update_all(
    SOC_SAND_IN int unit
  );



uint32
  arad_mgmt_ocb_voq_eligible_dynamic_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    qid,
    SOC_SAND_IN  uint32                    enable
  );


uint32 soc_arad_mgmt_rev_b0_set_unsafe(
    SOC_SAND_IN  int       unit);

#ifdef BCM_88660_A0

uint32 soc_arad_mgmt_rev_arad_plus_set_unsafe(
    SOC_SAND_IN  int       unit);
    
#endif

int 
  arad_mgmt_nof_block_instances(
    int unit, 
    soc_block_types_t block_types, 
    int *nof_block_instances
  );

int ardon_mgmt_drv_pvt_monitor_enable(int unit);

int 
   arad_mgmt_temp_pvt_get(
     int unit,
     int temperature_max,
     soc_switch_temperature_monitor_t *temperature_array,
     int *temperature_count
   ) ;


uint32
  arad_mgmt_init_set_core_clock_frequency (
    SOC_SAND_IN int unit,
    SOC_SAND_IN ARAD_MGMT_INIT* init
    );



int arad_mgmt_avs_value_get(
        int       unit,
        uint32*      avs_val);

uint32
  arad_mgmt_sw_ver_set_unsafe(
    SOC_SAND_IN  int                      unit);

 
uint32 arad_mgmt_ocb_voq_info_defaults_set(
    SOC_SAND_IN     int                         unit,
    SOC_SAND_OUT    ARAD_MGMT_OCB_VOQ_INFO      *ocb_info
    );


#include <soc/dpp/SAND/Utils/sand_footer.h>




#endif
