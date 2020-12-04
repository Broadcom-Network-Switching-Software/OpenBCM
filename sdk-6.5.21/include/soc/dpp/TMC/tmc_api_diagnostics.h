/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __SOC_TMC_API_DIAGNOSTICS_INCLUDED__

#define __SOC_TMC_API_DIAGNOSTICS_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Utils/sand_64cnt.h>

#include <soc/dpp/TMC/tmc_api_general.h>
#include <soc/dpp/TMC/tmc_api_mgmt.h>
#include <soc/dpp/TMC/tmc_api_statistics.h>
#include <soc/dpp/TMC/tmc_api_end2end_scheduler.h>
#include <soc/dpp/TMC/tmc_api_ingress_packet_queuing.h>
#include <soc/dpp/TMC/tmc_api_ingress_traffic_mgmt.h>
#include <soc/dpp/TMC/tmc_api_egr_queuing.h>
#include <soc/dpp/TMC/tmc_api_ports.h>









#define SOC_TMC_DIAG_LAST_PCKT_SNAPSHOT_LEN_BYTES_PB                (64)
#define SOC_TMC_DIAG_LAST_PCKT_SNAPSHOT_LEN_BYTES_ARAD              (128)
#define SOC_TMC_DIAG_LAST_PCKT_SNAPSHOT_LEN_BYTES_MAX               SOC_SAND_MAX(SOC_TMC_DIAG_LAST_PCKT_SNAPSHOT_LEN_BYTES_ARAD, SOC_TMC_DIAG_LAST_PCKT_SNAPSHOT_LEN_BYTES_PB)







#define SOC_TMC_DIAG_NOF_DDR_TRAIN_SEQS               8
#define SOC_TMC_DIAG_NOF_DLLS_PER_DRAM                4


#define SOC_TMC_DIAG_LBG_PAYLOAD_BYTE_SIZE         (256)


#define  DIAG_DRAM_NOF_DQSS 4






#define  SOC_TMC_DIAG_NOF_SONE_PER_SMS_MAX (8192)


#define  SOC_TMC_DIAG_CHAIN_LENGTH_MAX_IN_UINT32S (144)










typedef enum
{
  SOC_TMC_DIAG_PKT_HDR_U_TYPE      = 0x8,
  SOC_TMC_DIAG_PKT_HDR_F_TYPE      = 0x9,
  SOC_TMC_DIAG_PKT_HDR_M_TYPE      = 0xA,
  SOC_TMC_DIAG_PKT_HDR_IN_RCY_TYPE = 0xC,

  
  SOC_TMC_DIAG_PKT_HDR_UNKOWN_TYPE = 0xFF

} SOC_TMC_DIAG_ING_PKT_HDR_TYPE;





typedef enum
{
  
  SOC_TMC_DIAG_QDR_BIST_ADDR_MODE_NORMAL=0,
  
  SOC_TMC_DIAG_QDR_BIST_ADDR_MODE_ADDRESS_SHIFT=1,
  
  SOC_TMC_DIAG_QDR_BIST_ADDR_MODE_ADDRESS_TEST=2,

  SOC_TMC_DIAG_NOF_QDR_BIST_ADDR_MODES=3
}SOC_TMC_DIAG_QDR_BIST_ADDR_MODE;

typedef enum
{
  
  SOC_TMC_DIAG_QDR_BIST_DATA_MODE_NORMAL=0,
  
  SOC_TMC_DIAG_QDR_BIST_DATA_MODE_PATTERN_BIT=1,
  
  SOC_TMC_DIAG_QDR_BIST_DATA_MODE_RANDOM=2,
  
  SOC_TMC_DIAG_QDR_BIST_DATA_MODE_DATA_SHIFT=3,

  SOC_TMC_DIAG_NOF_QDR_BIST_DATA_MODES=4
}SOC_TMC_DIAG_QDR_BIST_DATA_MODE;

typedef enum
{
  
  SOC_TMC_DIAG_BIST_DATA_PATTERN_DIFF=0,
  
  SOC_TMC_DIAG_BIST_DATA_PATTERN_ONE=1,
  
  SOC_TMC_DIAG_BIST_DATA_PATTERN_ZERO=2,
  SOC_TMC_DIAG_NOF_BIST_DATA_PATTERNS=3
}SOC_TMC_DIAG_BIST_DATA_PATTERN;

typedef enum
{
  
  SOC_TMC_DIAG_DRAM_BIST_DATA_MODE_NORMAL=0,
  
  SOC_TMC_DIAG_DRAM_BIST_DATA_MODE_PATTERN_BIT=1,
  
  SOC_TMC_DIAG_DRAM_DATA_MODE_RANDOM=2,
  SOC_TMC_DIAG_NOF_DRAM_BIST_DATA_MODES =3
}SOC_TMC_DIAG_DRAM_BIST_DATA_MODE;

typedef enum
{
  
  SOC_TMC_DIAG_SOFT_ERROR_PATTERN_ONE = 0,
  
  SOC_TMC_DIAG_SOFT_ERROR_PATTERN_ZERO = 1,
  
  SOC_TMC_DIAG_SOFT_ERROR_PATTERN_DIFF1 = 2,
  
  SOC_TMC_DIAG_SOFT_ERROR_PATTERN_DIFF2 = 3,
  
  SOC_TMC_NOF_DIAG_SOFT_ERROR_PATTERNS = 4
}SOC_TMC_DIAG_SOFT_ERROR_PATTERN;

typedef enum
{
  
  SOC_TMC_DIAG_SOFT_COUNT_TYPE_FAST = 0,
  
  SOC_TMC_DIAG_SOFT_COUNT_TYPE_COMPLETE = 1,
  
  SOC_TMC_NOF_DIAG_SOFT_COUNT_TYPES = 2
}SOC_TMC_DIAG_SOFT_COUNT_TYPE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 nof_cmnds_write;
  
  uint32 nof_cmnds_read;
  
  uint32 start_addr;
  
  uint32 end_addr;
  
  uint32 read_offset;
  
  SOC_TMC_DIAG_QDR_BIST_DATA_MODE data_mode;
  
  SOC_TMC_DIAG_QDR_BIST_ADDR_MODE address_mode;
  
  SOC_TMC_DIAG_BIST_DATA_PATTERN data_pattern;
}SOC_TMC_DIAG_QDR_BIST_TEST_RUN_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 is_test_finished;
  
   uint8 is_qdr_up;
  
  uint32 bit_err_counter;
  
  uint32 reply_err_counter;
  
  uint32 bits_error_bitmap;
  
  uint32 last_addr_err;
  
  uint32 last_data_err;
}SOC_TMC_DIAG_QDR_BIST_TEST_RES_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 is_finite_nof_cmnds;
  
  uint32 writes_per_cycle;
  
  uint32 reads_per_cycle;
  
  uint32 start_addr;
  
  uint32 end_addr;
  
  SOC_TMC_DIAG_DRAM_BIST_DATA_MODE data_mode;
  
  SOC_TMC_DIAG_BIST_DATA_PATTERN data_pattern;
}SOC_TMC_DIAG_DRAM_BIST_TEST_RUN_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
   uint8 is_test_finished;

  
   uint8 is_dram_up;

  
  uint32 reply_err_counter;
}SOC_TMC_DIAG_DRAM_BIST_TEST_RES_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 address;
  
  uint8 is_data_size_bits_256_not_32;
  
  uint8 is_infinite_nof_actions;
}SOC_TMC_DIAG_DRAM_ACCESS_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 success;
  
  uint32 error_bits_global;
  
  uint32 nof_addr_with_errors;
}SOC_TMC_DIAG_DRAM_READ_COMPARE_STATUS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8  valid;
  
  uint32 queue_qrtt;
  
  uint32 queue_id;
  
  uint32 flow_qrtt;
  
  uint32 flow_id;
  
  uint8 is_composite;
  
  uint32 sys_phy_port;
  
  uint32 dst_fap_id;
  
  uint32 dst_prt_id;
}SOC_TMC_DIAG_PACKET_WALKTROUGH;

typedef union
{
  
  struct
  {
    SOC_SAND_MAGIC_NUM_VAR
    
    uint32  destination_id;
    
    uint32 drp;
    
    uint32 snoop_cmd;
    
    uint32 class_val;
  } unicast;

  
  struct
  {
    SOC_SAND_MAGIC_NUM_VAR
    
    uint32 flow_id;
    
    uint32 drp;
    
    uint32 snoop_cmd;
  } flow;

  
  struct
  {
    SOC_SAND_MAGIC_NUM_VAR
    
    uint32 source_port_addr;
    
    uint32 multicast_id;
    
    uint32 drp;
    
    uint32 snoop_cmd;
    
    uint32 exclude_src;
    
    uint32 class_val;
  } multicast;

} SOC_TMC_DIAG_ING_PKT_HDR_DATA;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 data[SOC_TMC_DIAG_LBG_PAYLOAD_BYTE_SIZE];
  
  uint32 data_byte_size;

}SOC_TMC_DIAG_LBG_PACKET_PATTERN;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
 
  SOC_TMC_DIAG_LBG_PACKET_PATTERN pattern;
 
  uint32 packet_size;
 
  uint32 nof_packets;
}SOC_TMC_DIAG_LBG_TRAFFIC_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
 
  SOC_TMC_STAT_ALL_STATISTIC_COUNTERS counters;
 
  uint32 ingress_rate;
 
  uint32 egress_rate;
 
  uint32 credits;
}SOC_TMC_DIAG_LBG_RESULT_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
 
  uint32 ports[SOC_TMC_NOF_FAP_PORTS];
 
  uint32 nof_ports;
}SOC_TMC_DIAG_LBG_PATH_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
   
    SOC_TMC_DIAG_LBG_PATH_INFO     path;
}SOC_TMC_DIAG_LBG_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 ddl_control_0;
  
  uint32 ddl_control_1;
  
  uint32 ddl_control_2;
  
  uint8 rnd_trp;
  
  uint8 rnd_trp_diff;
  
  uint8 dll_init_done;
  
  uint8 dll_ph_dn;
  
  uint8 dll_ph_up;
  
  uint8 main_ph_sel;
  
  uint8 ph2sel;
  
  uint8 hc_sel_vec;
  
  uint8 qc_sel_vec;
  
  uint8 sel_vec;
  
  uint8 sel_hg;
  
  uint8 ph_sel_hc_up;
  
  uint8 ins_dly_min_vec;
  
  uint8 ddl_init_main_ph_sel_ofst;
  
  uint8 ddl_ph_sel_hc_up;
  
  uint8 ddl_train_trig_up_limit;
  
  uint8 ddl_train_trig_dn_limit;
  
  uint8 ph_sel_err;
  
  uint8 dly_max_min_mode;
  
  uint8 ph_sel;
}SOC_TMC_DIAG_DLL_STATUS_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 training_seq;
  
  uint32 calibration_st;
  
  uint32 ddl_periodic_training;
  
  uint8 dll_mstr_s;
  
  uint32 ddr_training_sequence[SOC_TMC_DIAG_NOF_DDR_TRAIN_SEQS];
  
  SOC_TMC_DIAG_DLL_STATUS_INFO dll_status[SOC_TMC_DIAG_NOF_DLLS_PER_DRAM];
}SOC_TMC_DIAG_DRAM_STATUS_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 bit_err_bitmap;
  
  uint8 is_clocking_err;
  
  uint8 is_dqs_con_err[DIAG_DRAM_NOF_DQSS];
  
  uint8 is_phy_ready_err;
  
  uint8 is_rtt_avg_min_err;
  
  uint8 is_rtt_avg_max_err;

}  SOC_TMC_DIAG_DRAM_ERR_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_DIAG_SOFT_ERROR_PATTERN pattern;
  
  uint32 sms;

} SOC_TMC_DIAG_SOFT_ERR_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 err_sp;
  
  uint32 err_dp;
  
  uint32 err_rf;

} SOC_TMC_DIAG_SOFT_ERR_RESULT;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 diag_chain[SOC_TMC_DIAG_CHAIN_LENGTH_MAX_IN_UINT32S];
  
  SOC_TMC_DIAG_SOFT_ERR_RESULT nof_errs;

} SOC_TMC_DIAG_SOFT_SMS_RESULT;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 is_valid;
  
  uint32 tm_port;
  
  uint32 pp_port;
  
  uint32 src_syst_port;
  
  SOC_TMC_PORT_HEADER_TYPE port_header_type;
  
  SOC_TMC_PORTS_ITMH itmh;
  
  uint8 buffer[SOC_TMC_DIAG_LAST_PCKT_SNAPSHOT_LEN_BYTES_MAX];
  
  uint32 packet_size;
} SOC_TMC_DIAG_LAST_PACKET_INFO;












void
  SOC_TMC_DIAG_SOFT_ERR_INFO_clear(
    SOC_SAND_OUT SOC_TMC_DIAG_SOFT_ERR_INFO *info
  );

void
  SOC_TMC_DIAG_SOFT_ERR_RESULT_clear(
    SOC_SAND_OUT SOC_TMC_DIAG_SOFT_ERR_RESULT *info
  );

void
  SOC_TMC_DIAG_SOFT_SMS_RESULT_clear(
    SOC_SAND_OUT SOC_TMC_DIAG_SOFT_SMS_RESULT *info
  );

#if SOC_TMC_DEBUG_IS_LVL1

void
  SOC_TMC_DIAG_LBG_PACKET_PATTERN_print(
    SOC_SAND_IN SOC_TMC_DIAG_LBG_PACKET_PATTERN *info
  );

void
  SOC_TMC_DIAG_LBG_TRAFFIC_INFO_print(
    SOC_SAND_IN SOC_TMC_DIAG_LBG_TRAFFIC_INFO *info
  );

void
  SOC_TMC_DIAG_LBG_PATH_INFO_print(
    SOC_SAND_IN SOC_TMC_DIAG_LBG_PATH_INFO *info
  );

void
  SOC_TMC_DIAG_LBG_INFO_print(
    SOC_SAND_IN SOC_TMC_DIAG_LBG_INFO *info
  );

const char*
  SOC_TMC_DIAG_QDR_BIST_ADDR_MODE_to_string(
    SOC_SAND_IN SOC_TMC_DIAG_QDR_BIST_ADDR_MODE enum_val
  );

const char*
  SOC_TMC_DIAG_QDR_BIST_DATA_MODE_to_string(
    SOC_SAND_IN SOC_TMC_DIAG_QDR_BIST_DATA_MODE enum_val
  );

const char*
  SOC_TMC_DIAG_BIST_DATA_PATTERN_to_string(
    SOC_SAND_IN SOC_TMC_DIAG_BIST_DATA_PATTERN enum_val
  );

const char*
  SOC_TMC_DIAG_DRAM_BIST_DATA_MODE_to_string(
    SOC_SAND_IN SOC_TMC_DIAG_DRAM_BIST_DATA_MODE enum_val
  );

const char*
  SOC_TMC_DIAG_SOFT_ERROR_PATTERN_to_string(
    SOC_SAND_IN  SOC_TMC_DIAG_SOFT_ERROR_PATTERN enum_val
  );

const char*
  SOC_TMC_DIAG_SOFT_COUNT_TYPE_to_string(
    SOC_SAND_IN  SOC_TMC_DIAG_SOFT_COUNT_TYPE enum_val
  );

void
  SOC_TMC_DIAG_QDR_BIST_TEST_RUN_INFO_print(
    SOC_SAND_IN SOC_TMC_DIAG_QDR_BIST_TEST_RUN_INFO *info
  );

void
  SOC_TMC_DIAG_QDR_BIST_TEST_RES_INFO_print(
    SOC_SAND_IN SOC_TMC_DIAG_QDR_BIST_TEST_RES_INFO *info
  );

void
  SOC_TMC_DIAG_DRAM_BIST_TEST_RUN_INFO_print(
    SOC_SAND_IN SOC_TMC_DIAG_DRAM_BIST_TEST_RUN_INFO *info
  );

void
  SOC_TMC_DIAG_DRAM_BIST_TEST_RES_INFO_print(
    SOC_SAND_IN SOC_TMC_DIAG_DRAM_BIST_TEST_RES_INFO *info
  );

void
  SOC_TMC_DIAG_DRAM_ACCESS_INFO_print(
    SOC_SAND_IN SOC_TMC_DIAG_DRAM_ACCESS_INFO *info
  );

void
  SOC_TMC_DIAG_DRAM_READ_COMPARE_STATUS_print(
    SOC_SAND_IN SOC_TMC_DIAG_DRAM_READ_COMPARE_STATUS *info
  );

void
  SOC_TMC_DIAG_DLL_STATUS_INFO_print(
    SOC_SAND_IN SOC_TMC_DIAG_DLL_STATUS_INFO *info
  );

void
  SOC_TMC_DIAG_DRAM_STATUS_INFO_print(
    SOC_SAND_IN SOC_TMC_DIAG_DRAM_STATUS_INFO *info
  );

void
  SOC_TMC_DIAG_DRAM_ERR_INFO_print(
    SOC_SAND_IN  SOC_TMC_DIAG_DRAM_ERR_INFO *info
  );

void
  SOC_TMC_DIAG_SOFT_ERR_INFO_print(
    SOC_SAND_IN  SOC_TMC_DIAG_SOFT_ERR_INFO *info
  );

void
  SOC_TMC_DIAG_SOFT_ERR_RESULT_print(
    SOC_SAND_IN  SOC_TMC_DIAG_SOFT_ERR_RESULT *info
  );

void
  SOC_TMC_DIAG_SOFT_SMS_RESULT_print(
    SOC_SAND_IN  uint32                  sms_ndx,
    SOC_SAND_IN  uint32                  sone_ndx,
    SOC_SAND_IN  SOC_TMC_DIAG_SOFT_SMS_RESULT *info
  );

#endif 

void
  SOC_TMC_DIAG_LAST_PACKET_INFO_clear(
    SOC_SAND_OUT SOC_TMC_DIAG_LAST_PACKET_INFO *info
  );

#if SOC_TMC_DEBUG_IS_LVL1

void
  SOC_TMC_DIAG_LAST_PACKET_INFO_print(
    SOC_SAND_IN  SOC_TMC_DIAG_LAST_PACKET_INFO *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
