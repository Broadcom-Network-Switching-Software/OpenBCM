/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_TMC_API_STAT_IF_INCLUDED__

#define __SOC_TMC_API_STAT_IF_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/TMC/tmc_api_general.h>





#define SOC_TMC_STAT_NOF_THRESHOLD_RANGES 15
#define SOC_TMC_STAT_IF_COUNTERS_COMMANDS 2
#define SOC_TMC_STAT_IF_REPORT_SIZE_61b  (SOC_IS_QAX(unit) ? 1 : 0)
#define SOC_TMC_STAT_IF_REPORT_SIZE_64b  (SOC_IS_QAX(unit) ? 0 : 1)










typedef enum
{
  
  SOC_TMC_STAT_IF_PHASE_000=0,
  
  SOC_TMC_STAT_IF_PHASE_090=1,
  
  SOC_TMC_STAT_IF_PHASE_180=2,
  
  SOC_TMC_STAT_IF_PHASE_270=3,
  
  SOC_TMC_STAT_NOF_IF_PHASES = 4
}SOC_TMC_STAT_IF_PHASE;

typedef enum
{
  
  SOC_TMC_STAT_IF_REPORT_MODE_BILLING = 0,
  
  SOC_TMC_STAT_IF_REPORT_MODE_FAP20V = 1,

    
    SOC_TMC_STAT_NOF_IF_REPORT_MODES_PETRA = 2,

  
  SOC_TMC_STAT_IF_REPORT_QSIZE = SOC_TMC_STAT_NOF_IF_REPORT_MODES_PETRA,

  
  SOC_TMC_STAT_IF_REPORT_MODE_BILLING_QUEUE_NUMBER = 3,

  

  SOC_TMC_STAT_IF_REPORT_MODE_BILLING_INGRESS = 4,
  
  SOC_TMC_STAT_IF_REPORT_MODE_NOT_CONFIGURE = 5,

  
  SOC_TMC_STAT_NOF_IF_REPORT_MODES = 6
}SOC_TMC_STAT_IF_REPORT_MODE;

typedef enum
{
  
  SOC_TMC_SIF_CORE_MODE_DEDICATED,
  
  SOC_TMC_SIF_CORE_MODE_COMBINED,

  SOC_SIF_CORE_NOF_MODES
}SOC_TMC_SIF_CORE_MODE;


typedef enum
{
  
  SOC_TMC_STAT_IF_BILLING_MODE_EGR_Q_NUM = 0,
  
  SOC_TMC_STAT_IF_BILLING_MODE_CUD = 1,
  
  SOC_TMC_STAT_IF_BILLING_MODE_VSI_VLAN = 2,
  
  SOC_TMC_STAT_IF_BILLING_MODE_BOTH_LIFS = 3,
  
  SOC_TMC_STAT_NOF_IF_BILLING_MODES = 4
}SOC_TMC_STAT_IF_BILLING_MODE;

typedef enum
{
  
  SOC_TMC_STAT_IF_FAP20V_MODE_Q_SIZE = 0,
  
  SOC_TMC_STAT_IF_FAP20V_MODE_PKT_SIZE = 1,
  
  SOC_TMC_STAT_NOF_IF_FAP20V_MODES = 2
}SOC_TMC_STAT_IF_FAP20V_MODE;

typedef enum
{
  
  SOC_TMC_STAT_IF_MC_MODE_Q_NUM = 0,
  
  SOC_TMC_STAT_IF_MC_MODE_MC_ID = 1,
  
  SOC_TMC_STAT_NOF_IF_MC_MODES = 2
}SOC_TMC_STAT_IF_MC_MODE;

typedef enum
{
  
  SOC_TMC_STAT_IF_CNM_MODE_DIS = 0,
  
  SOC_TMC_STAT_IF_CNM_MODE_EN = 1,
  
  SOC_TMC_STAT_NOF_IF_CNM_MODES = 2
}SOC_TMC_STAT_IF_CNM_MODE;

typedef enum
{
  
  SOC_TMC_STAT_IF_PARITY_MODE_DIS = 0,
  
  SOC_TMC_STAT_IF_PARITY_MODE_EN = 1,
  
  SOC_TMC_STAT_NOF_IF_PARITY_MODES = 2
}SOC_TMC_STAT_IF_PARITY_MODE;

typedef enum
{
  
  SOC_TMC_STAT_IF_BIST_EN_MODE_DIS = 0,
  
  SOC_TMC_STAT_IF_BIST_EN_MODE_PATTERN = 1,
  
  SOC_TMC_STAT_IF_BIST_EN_MODE_WALKING_ONE = 2,
  
  SOC_TMC_STAT_IF_BIST_EN_MODE_PRBS = 3,
  
  SOC_TMC_STAT_NOF_IF_BIST_EN_MODES = 4
}SOC_TMC_STAT_IF_BIST_EN_MODE;

typedef enum
{

  
  
  SOC_TMC_STAT_IF_PKT_SIZE_65B = 0,

  
  
  SOC_TMC_STAT_IF_PKT_SIZE_126B = 1,

  
  
  SOC_TMC_STAT_IF_PKT_SIZE_248B = 2,

  
  
  SOC_TMC_STAT_IF_PKT_SIZE_492B = 3,

    
    SOC_TMC_STAT_IF_PKT_SIZE_1024B = 4,

  
  SOC_TMC_NOF_STAT_IF_PKT_SIZES = 5

}SOC_TMC_STAT_IF_PKT_SIZE;

typedef enum
{
  
  SOC_TMC_STAT_IF_NOF_REPORTS_8 = 0,

  SOC_TMC_STAT_IF_NOF_REPORTS_16 = 1,

  SOC_TMC_STAT_IF_NOF_REPORTS_32 = 2,

  SOC_TMC_STAT_IF_NOF_REPORTS_64 = 3,

  SOC_TMC_STAT_IF_NOF_REPORTS_128 = 4

}SOC_TMC_STAT_IF_NOF_STAT_REPORTS_PER_PKT;


typedef enum
{
    STAT_IF_ETPP_COUNTERS_MODE_NONE = 0x0,
    STAT_IF_ETPP_COUNTERS_MODE_LATENCY = 0x1, 
    STAT_IF_ETPP_COUNTERS_MODE_PORT = 0x2,
    STAT_IF_ETPP_COUNTERS_MODE_VSI = 0x4, 
    STAT_IF_ETPP_COUNTERS_MODE_LIF = 0x8
}SOC_TMC_STAT_IF_ETPP_COUNTERS_MODE;




typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 enable;
  
  SOC_TMC_STAT_IF_PHASE if_phase;
}SOC_TMC_STAT_IF_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_STAT_IF_BILLING_MODE mode;

} SOC_TMC_STAT_IF_BILLING_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_STAT_IF_FAP20V_MODE mode;
  
  SOC_TMC_STAT_IF_MC_MODE fabric_mc;
  
  SOC_TMC_STAT_IF_MC_MODE ing_mc;
  
  uint8 is_original_pkt_size;
  
  uint8 ing_mc_is_report_single;
  
  uint8 count_snoop;
  
  SOC_TMC_STAT_IF_CNM_MODE cnm_report;

} SOC_TMC_STAT_IF_FAP20V_INFO;

typedef union
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_STAT_IF_BILLING_INFO billing;
  
  SOC_TMC_STAT_IF_FAP20V_INFO fap20v;

} SOC_TMC_STAT_IF_REPORT_MODE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
#ifdef BCM_88650_B0
  
  uint8 stat_if_port_enable[SOC_MAX_NUM_PORTS];
#endif
  
  SOC_TMC_STAT_IF_REPORT_MODE mode;

  
  uint32 rate;

  
  SOC_TMC_STAT_IF_REPORT_MODE_INFO format;
  
  uint32 sync_rate;
  
  uint8 parity_enable;

   
  uint8  is_idle_reports_present;

   
  uint8              if_report_original_pkt_size;

  
  SOC_TMC_STAT_IF_PKT_SIZE   if_pkt_size ;

    
   uint32              if_scrubber_queue_min; 
  
     
   uint32              if_scrubber_queue_max; 
  
   
   uint32              if_scrubber_rate_min;

   
   uint32              if_scrubber_rate_max;

#ifdef BCM_88650_B0
     
   uint32              if_qsize_queue_min; 
  
     
   uint32              if_qsize_queue_max; 
#endif

   
   uint32              if_scrubber_buffer_descr_th[SOC_TMC_STAT_NOF_THRESHOLD_RANGES];

    
   uint32              if_scrubber_bdb_th[SOC_TMC_STAT_NOF_THRESHOLD_RANGES];

    
   uint32              if_scrubber_uc_dram_buffer_th[SOC_TMC_STAT_NOF_THRESHOLD_RANGES];
   
   uint32              if_scrubber_sram_buffers_th[SOC_TMC_STAT_NOF_THRESHOLD_RANGES];   
      
   uint32              if_scrubber_sram_pdbs_th[SOC_TMC_STAT_NOF_THRESHOLD_RANGES];      
   
   
   uint8               if_report_mc_once;
  
   uint8 if_report_enqueue_enable;
  
   uint8 if_report_dequeue_enable;
   
   SOC_TMC_STAT_IF_NOF_STAT_REPORTS_PER_PKT if_nof_reports_per_packet;
   
   uint8 report_size;
   
   uint8 if_billing_ingress_queue_stamp;
   
   uint8 if_billing_ingress_drop_reason;
   
   uint8 if_billing_filter_reports_ingress;
   uint8 if_billing_filter_reports_egress;
   
   SOC_TMC_STAT_IF_ETPP_COUNTERS_MODE if_etpp_counter_mode[SOC_TMC_STAT_IF_COUNTERS_COMMANDS];
} SOC_TMC_STAT_IF_REPORT_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 word1;
  
  uint32 word2;

} SOC_TMC_STAT_IF_BIST_PATTERN;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_STAT_IF_BIST_EN_MODE en_mode;
  
  SOC_TMC_STAT_IF_BIST_PATTERN pattern;
  
  uint32 nof_duplications;
  
  uint32 nof_idle_clocks;

} SOC_TMC_STAT_IF_BIST_INFO;


typedef struct {
   
   uint32 packet_size ;

   
   uint32 records_in_packet ;

} SOC_TMC_STAT_IF_PACKET_INFO;









void
  SOC_TMC_STAT_IF_INFO_clear(
    SOC_SAND_OUT SOC_TMC_STAT_IF_INFO *info
  );

void
  SOC_TMC_STAT_IF_BILLING_INFO_clear(
    SOC_SAND_OUT SOC_TMC_STAT_IF_BILLING_INFO *info
  );

void
  SOC_TMC_STAT_IF_FAP20V_INFO_clear(
    SOC_SAND_OUT SOC_TMC_STAT_IF_FAP20V_INFO *info
  );

void
  SOC_TMC_STAT_IF_REPORT_MODE_INFO_clear(
    SOC_SAND_OUT SOC_TMC_STAT_IF_REPORT_MODE_INFO *info
  );

void
  SOC_TMC_STAT_IF_REPORT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_STAT_IF_REPORT_INFO *info
  );

void
  SOC_TMC_STAT_IF_BIST_PATTERN_clear(
    SOC_SAND_OUT SOC_TMC_STAT_IF_BIST_PATTERN *info
  );

void
  SOC_TMC_STAT_IF_BIST_INFO_clear(
    SOC_SAND_OUT SOC_TMC_STAT_IF_BIST_INFO *info
  );

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_STAT_IF_PHASE_to_string(
    SOC_SAND_IN  SOC_TMC_STAT_IF_PHASE enum_val
  );

const char*
  SOC_TMC_STAT_IF_REPORT_MODE_to_string(
    SOC_SAND_IN  SOC_TMC_STAT_IF_REPORT_MODE enum_val
  );

const char*
  SOC_TMC_STAT_IF_BILLING_MODE_to_string(
    SOC_SAND_IN  SOC_TMC_STAT_IF_BILLING_MODE enum_val
  );

const char*
  SOC_TMC_STAT_IF_FAP20V_MODE_to_string(
    SOC_SAND_IN  SOC_TMC_STAT_IF_FAP20V_MODE enum_val
  );

const char*
  SOC_TMC_STAT_IF_MC_MODE_to_string(
    SOC_SAND_IN  SOC_TMC_STAT_IF_MC_MODE enum_val
  );

const char*
  SOC_TMC_STAT_IF_CNM_MODE_to_string(
    SOC_SAND_IN  SOC_TMC_STAT_IF_CNM_MODE enum_val
  );

const char*
  SOC_TMC_STAT_IF_PARITY_MODE_to_string(
    SOC_SAND_IN  SOC_TMC_STAT_IF_PARITY_MODE enum_val
  );

const char*
  SOC_TMC_STAT_IF_BIST_EN_MODE_to_string(
    SOC_SAND_IN  SOC_TMC_STAT_IF_BIST_EN_MODE enum_val
  );

void
  SOC_TMC_STAT_IF_INFO_print(
    SOC_SAND_IN  SOC_TMC_STAT_IF_INFO *info
  );

void
  SOC_TMC_STAT_IF_BILLING_INFO_print(
    SOC_SAND_IN  SOC_TMC_STAT_IF_BILLING_INFO *info
  );

void
  SOC_TMC_STAT_IF_FAP20V_INFO_print(
    SOC_SAND_IN  SOC_TMC_STAT_IF_FAP20V_INFO *info
  );

void
  SOC_TMC_STAT_IF_REPORT_MODE_INFO_print(
    SOC_SAND_IN  SOC_TMC_STAT_IF_REPORT_MODE_INFO *info
  );

void
  SOC_TMC_STAT_IF_REPORT_INFO_print(
    SOC_SAND_IN  SOC_TMC_STAT_IF_REPORT_INFO *info
  );

void
  SOC_TMC_STAT_IF_BIST_PATTERN_print(
    SOC_SAND_IN  SOC_TMC_STAT_IF_BIST_PATTERN *info
  );

void
  SOC_TMC_STAT_IF_BIST_INFO_print(
    SOC_SAND_IN  SOC_TMC_STAT_IF_BIST_INFO *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
