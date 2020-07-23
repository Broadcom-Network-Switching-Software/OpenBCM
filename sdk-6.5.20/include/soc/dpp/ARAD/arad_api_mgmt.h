/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_API_MGMT_INCLUDED__

#define __ARAD_API_MGMT_INCLUDED__



#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/SAND_FM/sand_chip_defines.h>
#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/TMC/tmc_api_mgmt.h>
#include <soc/dpp/ARAD/arad_api_fabric.h>
#include <soc/dpp/ARAD/arad_api_nif.h>
#include <soc/dpp/ARAD/arad_api_ingress_traffic_mgmt.h>
#include <soc/dpp/ARAD/arad_api_ports.h>
#include <soc/dpp/ARAD/arad_api_egr_queuing.h>
#include <soc/dpp/ARAD/arad_api_end2end_scheduler.h>
#include <soc/dpp/ARAD/arad_api_dram.h>
#include <soc/dpp/ARAD/arad_api_stat_if.h>
#include <soc/dpp/ARAD/arad_api_flow_control.h>
#include <soc/dpp/ARAD/arad_stat.h>
#include <soc/dpp/DRC/drc_combo28.h>

#include <soc/dcmn/dcmn_defs.h>
#include <soc/dcmn/fabric.h>






#define  ARAD_MAX_NOF_REVISIONS ARAD_REVISION_NOF_IDS


#define  ARAD_HW_DRAM_CONF_SIZE_MAX 100


#define  ARAD_MGMT_NOF_SHARED_SERDES_QUARTETS 2

#define ARAD_MGMT_VER_REG_BASE       0
#define ARAD_MGMT_CHIP_TYPE_FLD_LSB   4
#define ARAD_MGMT_CHIP_TYPE_FLD_MSB   23
#define ARAD_MGMT_DBG_VER_FLD_LSB     24
#define ARAD_MGMT_DBG_VER_FLD_MSB     27
#define ARAD_MGMT_CHIP_VER_FLD_LSB    28
#define ARAD_MGMT_CHIP_VER_FLD_MSB    31


#define ARAD_MGMT_PCKT_SIZE_EXTERN_NO_LIMIT 0

#define ARAD_HW_DRAM_CONF_MODE_MIN   (ARAD_HW_DRAM_CONF_MODE_BUFFER)
#define ARAD_HW_DRAM_CONF_MODE_MAX   (ARAD_HW_DRAM_CONF_MODE_PARAMS)

#define ARAD_MGMT_FDR_TRFC_DISABLE         0x0
#define ARAD_MGMT_FDR_TRFC_ENABLE_VAR_CELL_LSB 0x1FB007E8
#define ARAD_MGMT_FDR_TRFC_ENABLE_VAR_CELL_MSB 0x10


#define ARAD_MGMT_OCB_MC_RANGE_DISABLE 0xFFFFFFFF



#define ARAD_MGMT_PCKT_RNG_HW_OFFSET 1




#define ARAD_MGMT_PCKT_RNG_NIF_CRC_BYTES 4


#define ARAD_MGMT_PCKT_RNG_DRAM_CRC_BYTES 2



#define ARAD_MGMT_PCKT_RNG_CORRECTION_INTERNAL \
  (ARAD_MGMT_PCKT_RNG_HW_OFFSET)




#define ARAD_MGMT_PCKT_RNG_CORRECTION_EXTERNAL \
  (ARAD_MGMT_PCKT_RNG_NIF_CRC_BYTES - ARAD_MGMT_PCKT_RNG_HW_OFFSET  - ARAD_MGMT_PCKT_RNG_DRAM_CRC_BYTES)


#define ARAD_MGMT_PCKT_MAX_SIZE_EXTERNAL_MAX        ((0x3FFF)+ARAD_MGMT_PCKT_RNG_CORRECTION_EXTERNAL)
#define ARAD_MGMT_PCKT_MAX_SIZE_INTERNAL_MAX        ((0x3FFF)+ARAD_MGMT_PCKT_RNG_CORRECTION_INTERNAL)


#define ARAD_MGMT_STAT_IF_REPORT_INFO_ARRAY_SIZE_MAX (2)











#define ARAD_MGMT_FABRIC_HDR_TYPE_ARAD                     SOC_TMC_MGMT_FABRIC_HDR_TYPE_ARAD
#define ARAD_MGMT_FABRIC_HDR_TYPE_FAP20                    SOC_TMC_MGMT_FABRIC_HDR_TYPE_FAP20
#define ARAD_MGMT_FABRIC_HDR_TYPE_FAP10M                   SOC_TMC_MGMT_FABRIC_HDR_TYPE_FAP10M
#define ARAD_MGMT_NOF_FABRIC_HDR_TYPES                     SOC_TMC_MGMT_NOF_FABRIC_HDR_TYPES
typedef SOC_TMC_MGMT_FABRIC_HDR_TYPE                           ARAD_MGMT_FABRIC_HDR_TYPE;

#define ARAD_MGMT_TDM_MODE_PACKET                          SOC_TMC_MGMT_TDM_MODE_PACKET
#define ARAD_MGMT_TDM_MODE_TDM_OPT                         SOC_TMC_MGMT_TDM_MODE_TDM_OPT
#define ARAD_MGMT_TDM_MODE_TDM_STA                         SOC_TMC_MGMT_TDM_MODE_TDM_STA
typedef SOC_TMC_MGMT_TDM_MODE                                  ARAD_MGMT_TDM_MODE;

#define ARAD_MGMT_NOF_TDM_MODES                            SOC_TMC_MGMT_NOF_TDM_MODES

typedef SOC_TMC_MGMT_PCKT_SIZE                                 ARAD_MGMT_PCKT_SIZE;

#define ARAD_MGMT_PCKT_SIZE_CONF_MODE_EXTERN               SOC_TMC_MGMT_PCKT_SIZE_CONF_MODE_EXTERN     
#define ARAD_MGMT_PCKT_SIZE_CONF_MODE_INTERN               SOC_TMC_MGMT_PCKT_SIZE_CONF_MODE_INTERN     
#define ARAD_MGMT_NOF_PCKT_SIZE_CONF_MODES                 SOC_TMC_MGMT_NOF_PCKT_SIZE_CONF_MODES       
typedef SOC_TMC_MGMT_PCKT_SIZE_CONF_MODE                       ARAD_MGMT_PCKT_SIZE_CONF_MODE;

typedef SOC_TMC_MGMT_OCB_VOQ_INFO                              ARAD_MGMT_OCB_VOQ_INFO;


typedef enum
{
  
  ARAD_MGMT_FTMH_LB_EXT_MODE_DISABLED = 0,
  
  ARAD_MGMT_FTMH_LB_EXT_MODE_8B_LB_KEY_8B_STACKING_ROUTE_HISTORY = 1,
  
  ARAD_MGMT_FTMH_LB_EXT_MODE_16B_STACKING_ROUTE_HISTORY = 2,
  
  ARAD_MGMT_FTMH_LB_EXT_MODE_ENABLED = 3,
   
  ARAD_MGMT_FTMH_LB_EXT_MODE_STANDBY_MC_LB = 4,
     
  ARAD_MGMT_FTMH_LB_EXT_MODE_FULL_HASH = 5,
  
  ARAD_MGMT_NOF_FTMH_LB_EXT_MODES = 6

} ARAD_MGMT_FTMH_LB_EXT_MODE;

typedef enum
{
  

  ARAD_MGMT_TRUNK_HASH_FORMAT_NORMAL = 0,  
  
  ARAD_MGMT_TRUNK_HASH_FORMAT_INVERTED = 1,
  
  ARAD_MGMT_TRUNK_HASH_FORMAT_DUPLICATED = 2,
  
  ARAD_MGMT_NOF_TRUNK_HASH_FORMAT = 3
  
} ARAD_MGMT_TRUNK_HASH_FORMAT;

typedef enum
{
  
  ARAD_REVISION_ID_1=0,
  
  ARAD_REVISION_NOF_IDS=1
}ARAD_REVISION_ID;

typedef enum
{
  
  ARAD_INIT_FMC_4K_REP_64K_DBUFF_MODE = 0,
  
  ARAD_INIT_FMC_64_REP_128K_DBUFF_MODE = 1,
  
  ARAD_INIT_NBR_FULL_MULTICAST_DBUFF_NOF_MODES = 2
}ARAD_INIT_NBR_FULL_MULTICAST_DBUFF_MODES;

#define ARAD_INIT_PDM_MODE_SIMPLE SOC_TMC_INIT_PDM_MODE_SIMPLE
#define ARAD_INIT_PDM_MODE_REDUCED SOC_TMC_INIT_PDM_MODE_REDUCED
#define ARAD_INIT_PDM_NOF_MODES SOC_TMC_INIT_PDM_NOF_MODES
typedef SOC_TMC_INIT_PDM_MODE ARAD_INIT_PDM_MODE;

typedef enum
{
  
  ARAD_HW_DRAM_CONF_MODE_BUFFER=0,
  
  ARAD_HW_DRAM_CONF_MODE_PARAMS=1,
  
  ARAD_HW_NOF_DRAM_CONF_MODES
}ARAD_HW_DRAM_CONF_MODE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 m;
  
  uint32 n;
  
  uint32 p;

}ARAD_HW_PLL_PARAMS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  
  uint32  r;
  
  uint32  f;
  
  uint32  q;
}ARAD_INIT_DRAM_PLL;

#define ARAD_INIT_SERDES_REF_CLOCK_125 soc_dcmn_init_serdes_ref_clock_125
#define ARAD_INIT_SERDES_REF_CLOCK_156_25 soc_dcmn_init_serdes_ref_clock_156_25
#define ARAD_INIT_SREDES_NOF_REF_CLOCKS soc_dcmn_init_serdes_nof_ref_clocks
#define ARAD_INIT_SERDES_REF_CLOCK soc_dcmn_init_serdes_ref_clock_t
#define ARAD_INIT_SERDES_REF_CLOCK_DISABLE soc_dcmn_init_serdes_ref_clock_disable

typedef enum{
  
  ARAD_INIT_SYNTHESIZER_CLOCK_FREQUENCY_25_MHZ = 0,
  
  ARAD_INIT_SYNTHESIZER_CLOCK_FREQUENCY_125_MHZ = 1,
  
  ARAD_INIT_SYNTHESIZER_NOF_CLOCK_FREQUENCIES = 2
}ARAD_INIT_SYNTHESIZER_CLOCK_FREQUENCY;

typedef enum{
  
  ARAD_MGMT_CREDIT_WORTH_RESOLUTION_LOW = 0,

  
  ARAD_MGMT_CREDIT_WORTH_RESOLUTION_MEDIUM = 1,

  
  ARAD_MGMT_CREDIT_WORTH_RESOLUTION_HIGH = 2,

  
  ARAD_MGMT_CREDIT_WORTH_RESOLUTION_AUTO = 3,

  ARAD_MGMT_CREDIT_WORTH_RESOLUTIONS
}ARAD_MGMT_CREDIT_WORTH_RESOLUTION;

typedef enum
{
  
  ARAD_MGMT_EXT_VOL_MOD_HSTL_1p5V=0,
  
  ARAD_MGMT_EXT_VOL_MOD_3p3V,
  
  ARAD_MGMT_EXT_VOL_MOD_HSTL_1p5V_VDDO,
  
  ARAD_MGMT_EXT_VOL_MOD_HSTL_1p8V,

  ARAD_MGMT_EXT_VOL_NOF_MODES
} ARAD_MGMT_EXT_VOLT_MOD;

typedef struct
{
  
  ARAD_INIT_DRAM_PLL dram_pll;
  
  ARAD_INIT_SERDES_REF_CLOCK nif_clk_freq;
  
  ARAD_INIT_SERDES_REF_CLOCK fabric_clk_freq;
  
  ARAD_INIT_SYNTHESIZER_CLOCK_FREQUENCY synthesizer_clock_freq;
  
  uint32 ts_clk_mode;
  
  uint32 bs_clk_mode;
  
  uint32 ts_pll_phase_initial_lo;
  
  uint32 ts_pll_phase_initial_hi;

}ARAD_INIT_PLL; 

typedef struct
{
  
  uint32 buff_len;

  ARAD_REG_INFO  buff_seq[ARAD_HW_DRAM_CONF_SIZE_MAX];

  uint32 appl_max_buffer_crc_err;

}ARAD_HW_DRAM_CONF_BUFFER;

typedef struct
{
  uint32  dram_freq;

  ARAD_DRAM_INFO params;

}ARAD_HW_DRAM_CONF_PARAMS;

typedef union
{
  
   ARAD_HW_DRAM_CONF_BUFFER buffer_mode;

  
   ARAD_HW_DRAM_CONF_PARAMS params_mode;

}ARAD_HW_DRAM_CONF;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 enable;
  
  uint8 is_valid[SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX)];
  
  int nof_drams;
  
  uint8 bist_enable;
  
  ARAD_DRAM_TYPE dram_type;
  
  
  ARAD_HW_PLL_PARAMS pll_conf;
  
  ARAD_DRAM_NUM_BANKS nof_banks;
  
  ARAD_DRAM_NUM_COLUMNS nof_columns;
  
  ARAD_DRAM_NUM_ROWS nof_rows;
  
  int dram_size_total_mbyte;
  
  int dram_user_buffer_size_mbytes;
  
  int dram_user_buffer_start_ptr;
  
  int dram_device_buffer_size_mbytes;
  
  ARAD_ITM_DBUFF_SIZE_BYTES dbuff_size;
  
  int nof_dram_buffers;
  
  ARAD_INIT_NBR_FULL_MULTICAST_DBUFF_MODES fmc_dbuff_mode;
    
  ARAD_INIT_PDM_MODE pdm_mode;
  
  ARAD_HW_DRAM_CONF_MODE conf_mode;

  
  ARAD_HW_DRAM_CONF dram_conf;

  
  ARAD_DDR_CLAM_SHELL_MODE dram_clam_shell_mode[SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX)];

  
  uint32 interleaving_bits;
  
}ARAD_INIT_DDR;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 enable;
  
  ARAD_FABRIC_CONNECT_MODE connect_mode;
  
  ARAD_PORTS_FTMH_EXT_OUTLIF ftmh_extension;
  
  ARAD_MGMT_FTMH_LB_EXT_MODE ftmh_lb_ext_mode;
   
  ARAD_MGMT_TRUNK_HASH_FORMAT trunk_hash_format;
  
  uint8 ftmh_stacking_ext_mode;
   
  uint8 is_fe600;
   
  uint8 is_128_in_system;

  
  uint8 dual_pipe_tdm_packet;

  
  uint8 is_dual_mode_in_system;

  
  uint8 segmentation_enable;
  
  uint8 scheduler_adapt_to_links;
  
  uint8 system_contains_multiple_pipe_device;
  
  soc_dcmn_fabric_pipe_map_t fabric_pipe_map_config;

  
  uint8 fabric_pcp_enable;

  
  uint8 fabric_tdm_priority_min;

  
  uint8 fabric_links_to_core_mapping_mode;
  
  
  uint8 fabric_mesh_multicast_enable;

  
  uint8 fabric_mesh_topology_fast;

}ARAD_INIT_FABRIC;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 enable;
  
  uint32 frequency;
  
  uint32 system_ref_clock;

} ARAD_INIT_CORE_FREQ;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 profile_ndx;
  
  ARAD_PORT_PP_PORT_INFO conf;

} ARAD_INIT_PP_PORT;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 port_ndx;
  
  uint32 pp_port;

} ARAD_INIT_PP_PORT_MAP;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 port_ndx;
  
  uint32 conf;
  
  ARAD_EGR_PORT_PRIORITY_MODE priority_mode;
  
  uint32 base_q_pair;
  
  ARAD_EGR_PORT_SHAPER_MODE shaper_mode;

} ARAD_INIT_EGR_Q_PROFILE_MAP;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 port_ndx;
  
  uint32 multicast_offset;

} ARAD_MULTICAST_ID_OFFSET_MAP;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 port_ndx;
  
  ARAD_PORT_HEADER_TYPE header_type_in;
  
  ARAD_PORT_HEADER_TYPE header_type_out;
  
  uint32 first_header_size;

}ARAD_INIT_PORT_HDR_TYPE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 port_ndx;

}ARAD_INIT_PORT_TO_IF_MAP;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  ARAD_PORT_LAG_MODE lag_mode;
  
  uint8 is_stacking_system;
 
  uint8 use_trunk_as_ingress_mc_dest;

  
  uint8 add_dsp_extension_enable;
  
  uint8 pph_learn_extension_disable;
  
  uint8   stack_resolve_using_msb;
  
  uint8   smooth_division_resolve_using_msb;
  
  uint8 is_system_red;
  
  uint8 tm_domain;
  
  uint8 oamp_port_enable;
  
  soc_arad_stat_ilkn_counters_mode_t      ilkn_counters_mode;

  
  ARAD_PORTS_ILKN_CONFIG ilkn[SOC_DPP_DEFS_MAX(NOF_INTERLAKEN_PORTS)];

  
  int ilkn_first_packet_sw_bypass;

  
  ARAD_PORTS_CAUI_CONFIG caui[SOC_DPP_DEFS_MAX(NOF_CAUI_PORTS)];

  
  ARAD_SWAP_INFO    swap_info;

} ARAD_INIT_PORTS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 credit_worth_enable;
  
  uint32 credit_worth;

  
  ARAD_MGMT_CREDIT_WORTH_RESOLUTION credit_worth_resolution;

} ARAD_INIT_CREDIT;

typedef enum
{
  
  ARAD_OCB_REPARTITION_MODE_80_PRESENTS_UNICAST = 0,
  
  ARAD_OCB_REPARTITION_MODE_ALL_UNICAST = 1,
  
  ARAD_OCB_NOF_REPARTITION_MODES = 2
}ARAD_OCB_REPARTITION_MODE;

typedef enum
{
  
  OCB_DISABLED = 0,
  
  OCB_ENABLED = 1,
  
  OCB_ONLY = 2,
  
  OCB_ONLY_1_DRAM=3,
  
  OCB_DRAM_SEPARATE=4
}ARAD_OCB_ENABLE_MODE;

typedef enum
{
  
  ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_OFF,
  
  ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON,

  ARAD_MGMT_ILKN_TDM_SP_NOF_MODES
}ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE;

typedef struct{
  SOC_SAND_MAGIC_NUM_VAR
  
  ARAD_OCB_ENABLE_MODE ocb_enable;
  
  uint32 databuffer_size;
  
  ARAD_OCB_REPARTITION_MODE repartition_mode;
}ARAD_INIT_OCB;


typedef struct{
  SOC_SAND_MAGIC_NUM_VAR
  
  
  uint8 stat_if_enable;
  
  int stat_if_etpp_mode;
  

  SOC_TMC_SIF_CORE_MODE core_mode;

  ARAD_STAT_IF_REPORT_INFO stat_if_info[MAX_NUM_OF_CORES];


}ARAD_INIT_STAT_IF;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  
  ARAD_FC_OOB_TX_SPEED oob_tx_speed[SOC_TMC_FC_NOF_OOB_IDS];
  
  
  SOC_TMC_FC_OOB_TYPE fc_oob_type[SOC_TMC_FC_NOF_OOB_IDS];

  
  
  
  
  uint32 fc_directions[SOC_TMC_FC_NOF_OOB_IDS];

  
  uint32 fc_oob_calender_length[SOC_TMC_FC_NOF_OOB_IDS][SOC_TMC_CONNECTION_DIRECTION_BOTH];

  
  uint32 fc_oob_calender_rep_count[SOC_TMC_FC_NOF_OOB_IDS][SOC_TMC_CONNECTION_DIRECTION_BOTH];

  
  int cl_sch_enable;

}ARAD_INIT_FC;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  
  soc_pbmp_t slave_port_pbmp;

} ARAD_INIT_RCPU;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 pp_enable;
  
  uint8 is_petrab_in_system;

  
  
  ARAD_INIT_DDR dram;
  
  soc_dpp_drc_combo28_info_t drc_info;
  
  ARAD_INIT_SYNCE synce;
  
#if defined(INCLUDE_KBP)
  
  ARAD_INIT_ELK elk;
#endif

  
  ARAD_INIT_FABRIC fabric;
  
  ARAD_INIT_CORE_FREQ core_freq;
  
  ARAD_INIT_PORTS ports;
  
  ARAD_MGMT_TDM_MODE tdm_mode;
  
  ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE ilkn_tdm_dedicated_queuing;
  
  uint32 tdm_egress_priority;
  
  uint32 tdm_egress_dp;
  
  ARAD_INIT_CREDIT credit;
  
  ARAD_INIT_PLL pll;
  
  ARAD_INIT_OCB ocb;

  
  ARAD_INIT_STAT_IF stat_if;

  
  ARAD_INIT_FC fc;
   
  ARAD_EGR_QUEUING_PARTITION_SCHEME eg_cgm_scheme;
   
  ARAD_INIT_RCPU rcpu;
  
  uint8 dynamic_port_enable;
  
  ARAD_MGMT_EXT_VOLT_MOD ex_vol_mod;
  
  uint32 nif_recovery_enable; 
  
  uint32 nif_recovery_iter;

  uint32 max_burst_default_value_bucket_width;

  
  uint8 mirror_stamp_sys_dsp_ext;

  
  uint32 rcy_channelized_shared_context_enable; 
}ARAD_MGMT_INIT;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 min;
  
  uint32 max;
}ARAD_MGMT_OCB_MC_RANGE;

typedef struct
{
  
  int is_reserved;

  
  int core;

  
  uint32 tm_port;
}ARAD_MGMT_RESERVED_PORT_INFO;













uint32
  arad_register_device(
             uint32                  *base_address,
    SOC_SAND_IN  SOC_SAND_RESET_DEVICE_FUNC_PTR reset_device_ptr,
    SOC_SAND_INOUT int                 *unit_ptr
  );


uint32
  arad_unregister_device(
    SOC_SAND_IN  int                 unit
  );


uint32
  arad_calc_assigned_rebounded_credit_conf(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32              credit_worth,
    SOC_SAND_OUT uint32              *fld_val
  ) ;

int
  arad_mgmt_credit_worth_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32              credit_worth
  );


uint32
  arad_plus_mgmt_credit_worth_remote_set(
    SOC_SAND_IN  int    unit,
	SOC_SAND_IN  uint32    credit_worth_remote
  );


uint32
  arad_plus_mgmt_credit_worth_remote_get(
    SOC_SAND_IN  int    unit,
	SOC_SAND_OUT uint32    *credit_worth_remote
  );


uint32
  arad_plus_mgmt_module_to_credit_worth_map_set(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32    fap_id,
    SOC_SAND_IN  uint32    credit_value_type 
  );


uint32
  arad_plus_mgmt_module_to_credit_worth_map_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32    fap_id,
    SOC_SAND_OUT uint32    *credit_value_type 
  );

uint32
  arad_plus_mgmt_change_all_faps_credit_worth_unsafe(
    SOC_SAND_IN  int    unit,
    SOC_SAND_OUT uint8     credit_value_to_use
  );


uint32
  arad_mgmt_init_sequence_phase1(
    SOC_SAND_IN     int                 unit,
    SOC_SAND_IN     ARAD_MGMT_INIT           *init,
    SOC_SAND_IN     uint8                 silent
  );


uint32
  arad_mgmt_init_sequence_phase2(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  );


uint32
  arad_mgmt_system_fap_id_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_fap_id
  );


uint32
  arad_mgmt_tm_domain_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 tm_domain
  );


uint32
  arad_mgmt_tm_domain_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint32                 *tm_domain
  );

uint32
  arad_mgmt_all_ctrl_cells_enable_get(
    SOC_SAND_IN  int  unit,
    SOC_SAND_OUT uint8  *enable
  );


uint32
  arad_mgmt_all_ctrl_cells_enable_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 enable
  );


uint32
  arad_force_tdm_bypass_traffic_to_fabric_set(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  int     enable
  );

uint32
  arad_force_tdm_bypass_traffic_to_fabric_get(
    SOC_SAND_IN  int     unit,
    SOC_SAND_OUT int     *enable
  );


uint32
  arad_mgmt_enable_traffic_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 enable
  );


uint32
  arad_mgmt_enable_traffic_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *enable
  );


uint32
  arad_mgmt_max_pckt_size_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      port_ndx,
    SOC_SAND_IN  ARAD_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    SOC_SAND_IN  uint32                       max_size
  );


uint32
  arad_mgmt_max_pckt_size_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      port_ndx,
    SOC_SAND_IN  ARAD_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    SOC_SAND_OUT uint32                       *max_size
  );


uint32
  arad_mgmt_ocb_mc_range_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      range_ndx,
    SOC_SAND_IN  ARAD_MGMT_OCB_MC_RANGE         *range
  );

uint32
  arad_mgmt_ocb_mc_range_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      range_ndx,
    SOC_SAND_OUT ARAD_MGMT_OCB_MC_RANGE         *range
  );


uint32
  arad_mgmt_ocb_voq_eligible_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    q_category_ndx,
    SOC_SAND_IN  uint32                    rate_class_ndx,
    SOC_SAND_IN  uint32                    tc_ndx,
    SOC_SAND_IN  ARAD_MGMT_OCB_VOQ_INFO       *info,
    SOC_SAND_OUT ARAD_MGMT_OCB_VOQ_INFO    *exact_info
  );

uint32
  arad_mgmt_ocb_voq_eligible_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    q_category_ndx,
    SOC_SAND_IN  uint32                    rate_class_ndx,
    SOC_SAND_IN  uint32                    tc_ndx,
    SOC_SAND_OUT ARAD_MGMT_OCB_VOQ_INFO       *info
  );



uint32
  arad_mgmt_ocb_voq_eligible_dynamic_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    qid,
    SOC_SAND_IN  uint32                    enable
  );



uint32 soc_arad_mgmt_rev_b0_set(
    SOC_SAND_IN  int       unit);

#ifdef BCM_88660_A0

uint32 soc_arad_mgmt_rev_arad_plus_set(
    SOC_SAND_IN  int       unit);
    
#endif

void
  arad_ARAD_HW_PLL_PARAMS_clear(
    SOC_SAND_OUT ARAD_HW_PLL_PARAMS *info
  );

void
  arad_ARAD_INIT_DDR_clear(
    SOC_SAND_OUT ARAD_INIT_DDR *info
  );

void
  ARAD_INIT_DRAM_PLL_clear(
    SOC_SAND_OUT ARAD_INIT_DRAM_PLL *info
  );

void
  ARAD_INIT_PLL_clear(
    SOC_SAND_OUT ARAD_INIT_PLL *info
  );

void
  arad_ARAD_INIT_FABRIC_clear(
    SOC_SAND_OUT ARAD_INIT_FABRIC *info
  );

void
  ARAD_INIT_STAT_IF_clear(
    SOC_SAND_OUT ARAD_INIT_STAT_IF *info
  );

void
  arad_ARAD_INIT_CORE_FREQ_clear(
    SOC_SAND_OUT ARAD_INIT_CORE_FREQ *info
  );

void
  arad_ARAD_MGMT_INIT_clear(
    SOC_SAND_OUT ARAD_MGMT_INIT *info
  );

void
  arad_ARAD_MGMT_OCB_MC_RANGE_clear(
    SOC_SAND_OUT ARAD_MGMT_OCB_MC_RANGE *info
  );

void
  arad_ARAD_MGMT_OCB_VOQ_INFO_clear(
    SOC_SAND_OUT ARAD_MGMT_OCB_VOQ_INFO *info
  );

void
  arad_ARAD_MGMT_PCKT_SIZE_clear(
    SOC_SAND_OUT ARAD_MGMT_PCKT_SIZE *info
  );


void
  arad_ARAD_INIT_PORT_HDR_TYPE_clear(
    SOC_SAND_OUT ARAD_INIT_PORT_HDR_TYPE *info
  );

void
  arad_ARAD_INIT_PORT_TO_IF_MAP_clear(
    SOC_SAND_OUT ARAD_INIT_PORT_TO_IF_MAP *info
  );

void
  ARAD_INIT_PP_PORT_clear(
    SOC_SAND_OUT ARAD_INIT_PP_PORT *info
  );

void
  ARAD_INIT_PP_PORT_MAP_clear(
    SOC_SAND_OUT ARAD_INIT_PP_PORT_MAP *info
  );

void
  ARAD_INIT_EGR_Q_PROFILE_MAP_clear(
    SOC_SAND_OUT ARAD_INIT_EGR_Q_PROFILE_MAP *info
  );

void
  ARAD_INIT_CREDIT_clear(
    SOC_SAND_OUT ARAD_INIT_CREDIT *info
  );

void
  ARAD_INIT_OCB_clear(
    SOC_SAND_OUT ARAD_INIT_OCB *info
  );
  
void
  ARAD_INIT_PORTS_clear(
    SOC_SAND_OUT ARAD_INIT_PORTS *info
  );

void
  arad_ARAD_INIT_PORTS_clear(
    SOC_SAND_OUT ARAD_INIT_PORTS *info
  );

void
  arad_ARAD_INIT_FC_clear(
    SOC_SAND_OUT ARAD_INIT_FC *info
  );

#if ARAD_DEBUG_IS_LVL1
void
  arad_ARAD_HW_PLL_PARAMS_print(
    SOC_SAND_IN ARAD_HW_PLL_PARAMS *info
  );

void
  arad_ARAD_INIT_DDR_print(
    SOC_SAND_IN ARAD_INIT_DDR *info
  );

void
  arad_ARAD_INIT_FABRIC_print(
    SOC_SAND_IN ARAD_INIT_FABRIC *info
  );

void
  arad_ARAD_INIT_CORE_FREQ_print(
    SOC_SAND_IN  ARAD_INIT_CORE_FREQ *info
  );

void
  arad_ARAD_MGMT_INIT_print(
    SOC_SAND_IN ARAD_MGMT_INIT *info
  );

void
  arad_ARAD_INIT_PORT_HDR_TYPE_print(
    SOC_SAND_IN ARAD_INIT_PORT_HDR_TYPE *info
  );

void
  arad_ARAD_INIT_PORT_TO_IF_MAP_print(
    SOC_SAND_IN ARAD_INIT_PORT_TO_IF_MAP *info
  );

void
  ARAD_INIT_PP_PORT_print(
    SOC_SAND_IN  ARAD_INIT_PP_PORT *info
  );

void
  ARAD_INIT_PP_PORT_MAP_print(
    SOC_SAND_IN  ARAD_INIT_PP_PORT_MAP *info
  );

void
  ARAD_INIT_EGR_Q_PROFILE_MAP_print(
    SOC_SAND_IN  ARAD_INIT_EGR_Q_PROFILE_MAP *info
  );

void
  ARAD_INIT_PORTS_print(
    SOC_SAND_IN  ARAD_INIT_PORTS *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

