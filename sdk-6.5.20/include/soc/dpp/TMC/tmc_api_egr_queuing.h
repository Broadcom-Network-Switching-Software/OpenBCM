/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __SOC_TMC_API_EGR_QUEUING_INCLUDED__

#define __SOC_TMC_API_EGR_QUEUING_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/TMC/tmc_api_general.h>




#define SOC_TMC_EGR_OFP_SCH_WFQ_WEIGHT_MAX 255
#define SOC_TMC_NOF_EGR_QUEUING_MC_POOLS    (2) 
#define SOC_TMC_NOF_THRESH_TYPES            (16)

#define SHR_DEVICE_QUEUE_RESERVED_FLOW_START (0)










typedef enum
{
  
  SOC_TMC_EGR_Q_PRIO_LOW=0,
  
  SOC_TMC_EGR_Q_PRIO_HIGH=1,
  
  SOC_TMC_EGR_NOF_Q_PRIO_PB=2,
  
  SOC_TMC_EGR_Q_PRIO_0 = 0,
  SOC_TMC_EGR_Q_PRIO_1 = 1,
  SOC_TMC_EGR_Q_PRIO_2 = 2,
  SOC_TMC_EGR_Q_PRIO_3 = 3,
  SOC_TMC_EGR_Q_PRIO_4 = 4,
  SOC_TMC_EGR_Q_PRIO_5 = 5,
  SOC_TMC_EGR_Q_PRIO_6 = 6,
  SOC_TMC_EGR_Q_PRIO_7 = 7,
  
  SOC_TMC_EGR_NOF_Q_PRIO_ARAD,
  
  SOC_TMC_EGR_Q_PRIO_ALL=0xFFFF
}SOC_TMC_EGR_Q_PRIO;

typedef enum
{
  
  SOC_TMC_EGR_PORT_THRESH_TYPE_0=0,
  
  SOC_TMC_EGR_PORT_THRESH_TYPE_1=1,
  
  SOC_TMC_EGR_PORT_THRESH_TYPE_2=2,
  
  SOC_TMC_EGR_PORT_THRESH_TYPE_3=3,
  
  SOC_TMC_EGR_PORT_NOF_THRESH_TYPES_PETRA=4,
  
  SOC_TMC_EGR_PORT_THRESH_TYPE_4=4,
  
  SOC_TMC_EGR_PORT_THRESH_TYPE_5=5,
  
  SOC_TMC_EGR_PORT_THRESH_TYPE_6=6,
  
  SOC_TMC_EGR_PORT_THRESH_TYPE_7=7,
  
  SOC_TMC_EGR_PORT_THRESH_TYPE_8=8,
  
  SOC_TMC_EGR_PORT_THRESH_TYPE_9=9,
  
  SOC_TMC_EGR_PORT_THRESH_TYPE_10=10,
  
  SOC_TMC_EGR_PORT_THRESH_TYPE_11=11,
  
  SOC_TMC_EGR_PORT_THRESH_TYPE_12=12,
  
  SOC_TMC_EGR_PORT_THRESH_TYPE_13=13,
  
  SOC_TMC_EGR_PORT_THRESH_TYPE_14=14,
  
  SOC_TMC_EGR_PORT_THRESH_TYPE_15=15,
  
  SOC_TMC_EGR_PORT_NOF_THRESH_TYPES_ARAD = 16,
  SOC_TMC_EGR_PORT_NOF_THRESH_TYPES = 16
}SOC_TMC_EGR_PORT_THRESH_TYPE;

typedef enum
{
  
  SOC_TMC_EGR_FC_MCI_ENABLE=0,
  
  SOC_TMC_EGR_FC_MCI_DISABLE=1,
  
  SOC_TMC_EGR_FC_MCI_ENABLE_ERP=2,
  
  SOC_TMC_EGR_FC_MCI_NOF_ENABLE_STATUSS=3
}SOC_TMC_EGR_FC_MCI_EN_STATUS;

typedef enum
{
  
  SOC_TMC_EGR_MCI_GUARANTEED=0,
  
  SOC_TMC_EGR_MCI_BE=1,
  
  SOC_TMC_EGR_NOF_MCI_PRIORITIES=2
}SOC_TMC_EGR_MCI_PRIO;

typedef enum
{
  
  SOC_TMC_EGR_OFP_INTERFACE_PRIO_HIGH=0,
  
  SOC_TMC_EGR_OFP_INTERFACE_PRIO_MID=1,
  
  SOC_TMC_EGR_OFP_INTERFACE_PRIO_LOW=2,
  
  SOC_TMC_EGR_OFP_INTERFACE_PRIO_PFC_LOWEST=3,
  
  SOC_TMC_EGR_OFP_CHNIF_NOF_PRIORITIES=4
}SOC_TMC_EGR_OFP_INTERFACE_PRIO;


#define SOC_TMC_EGR_OFP_INTERFACE_PRIO_NONE SOC_TMC_EGR_OFP_CHNIF_NOF_PRIORITIES;

typedef enum
{
  
  SOC_TMC_EGR_HP_OVER_LP_ALL=0,
  
  SOC_TMC_EGR_HP_OVER_LP_PER_TYPE=1,
  
  SOC_TMC_EGR_HP_OVER_LP_FAIR = 2,
  
  SOC_TMC_EGR_OFP_NOF_SCH_MODES = 3
}SOC_TMC_EGR_OFP_SCH_MODE;

typedef enum
{
  
  SOC_TMC_EGR_UCAST_TO_SCHED=0,
  
  SOC_TMC_EGR_MCAST_TO_UNSCHED=1,
  
  SOC_TMC_EGR_NOF_Q_PRIO_MAPPING_TYPES=2
}SOC_TMC_EGR_Q_PRIO_MAPPING_TYPE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 words_consumed;
  
  uint32 packets_consumed;
  
  uint32 descriptors_avail;
  
  uint32 buffers_avail;
  
  uint32 buffers_used;
  
  uint32 queue_words_consumed[SOC_TMC_EGR_PORT_NOF_THRESH_TYPES];
  
  uint32 queue_pkts_consumed[SOC_TMC_EGR_PORT_NOF_THRESH_TYPES];
    

  uint32 queue_pkts_consumed_min[SOC_TMC_EGR_PORT_NOF_THRESH_TYPES];

  int32 queue_pkts_consumed_alpha[SOC_TMC_EGR_PORT_NOF_THRESH_TYPES];

}SOC_TMC_EGR_DROP_THRESH;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 words;
  
  uint32 buffers;
  
  uint32 descriptors;

  uint32 descriptors_min;

  int32 descriptors_alpha;

}SOC_TMC_EGR_FC_DEV_THRESH_INNER;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_EGR_FC_DEV_THRESH_INNER global;
  
  SOC_TMC_EGR_FC_DEV_THRESH_INNER scheduled;
  
  SOC_TMC_EGR_FC_DEV_THRESH_INNER unscheduled;
  
  SOC_TMC_EGR_FC_DEV_THRESH_INNER unscheduled_pool[SOC_TMC_NOF_EGR_QUEUING_MC_POOLS]; 

}SOC_TMC_EGR_FC_DEVICE_THRESH;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 words_unsch;
  
  uint32 packets_unsch;
  
  uint32 buffers_unsch;
  
  uint32 descriptors_total;

} SOC_TMC_EGR_FC_MCI_THRESH;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 words;
  
  uint32 packets;

} SOC_TMC_EGR_FC_CHNIF_THRESH;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 words;
  
  uint32 packet_descriptors;
  uint32 packet_descriptors_min;
  int packet_descriptors_alpha;
  
  uint32 data_buffers;
  uint32 data_buffers_min;
  int data_buffers_alpha;
  
  SOC_TMC_EGR_FC_DEV_THRESH_INNER mc;
}SOC_TMC_EGR_FC_OFP_THRESH;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 unsched_weight;
  
  uint32 sched_weight;

}SOC_TMC_EGR_OFP_SCH_WFQ;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_EGR_OFP_INTERFACE_PRIO nif_priority;
  
  SOC_TMC_EGR_OFP_SCH_WFQ ofp_wfq;
  
  SOC_TMC_EGR_OFP_SCH_WFQ ofp_wfq_high;

  
  SOC_TMC_EGR_OFP_SCH_WFQ ofp_wfq_q_pair[SOC_TMC_EGR_NOF_Q_PRIO_ARAD]; 

}SOC_TMC_EGR_OFP_SCH_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 tc;
  
  uint32 dp;

} SOC_TMC_EGR_Q_PRIORITY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 dbuff;

  uint32 dbuff_min;

  int dbuff_alpha;
  
  uint32 packet_descriptors;
  
  uint32 packet_descriptors_min;
    
  int packet_descriptors_alpha;

} SOC_TMC_EGR_THRESH_INFO;


typedef enum
{
  
  SOC_TMC_EGR_PORT_ONE_PRIORITY=1,
  
  SOC_TMC_EGR_PORT_TWO_PRIORITIES=2,
  
  SOC_TMC_EGR_PORT_EIGHT_PRIORITIES=8,
  
  SOC_TMC_EGR_NOF_PORT_PRIORITY_MODES=3
}SOC_TMC_EGR_PORT_PRIORITY_MODE;

typedef enum
{
  
  SOC_TMC_EGR_PORT_SHAPER_DATA_MODE=0,
  
  SOC_TMC_EGR_PORT_SHAPER_PACKET_MODE=1,  
  
  SOC_TMC_EGR_NOF_PORT_SHAPER_MODES=2
}SOC_TMC_EGR_PORT_SHAPER_MODE;

typedef enum
{
  
  SOC_TMC_EGR_QUEUING_PARTITION_SCHEME_DISCRETE = 0,
  
  SOC_TMC_EGR_QUEUING_PARTITION_SCHEME_STRICT = 1,
  
  SOC_TMC_NOF_EGR_QUEUING_PARTITION_SCHEMES = 2
}SOC_TMC_EGR_QUEUING_PARTITION_SCHEME;

typedef struct
{
  
  SOC_TMC_EGR_FC_DEV_THRESH_INNER uc;
  
  SOC_TMC_EGR_FC_DEV_THRESH_INNER mc;
  
  SOC_TMC_EGR_FC_DEV_THRESH_INNER total;
}SOC_TMC_EGR_QUEUING_TH_DB_GLOBAL;

typedef struct
{
  
  SOC_TMC_EGR_FC_DEV_THRESH_INNER mc;
  
  SOC_TMC_EGR_FC_DEV_THRESH_INNER reserved;
}SOC_TMC_EGR_QUEUING_TH_DB_POOL;

typedef struct
{
  
  SOC_TMC_EGR_FC_DEV_THRESH_INNER uc;
  
  SOC_TMC_EGR_FC_DEV_THRESH_INNER mc_shared;
  
  SOC_TMC_EGR_FC_DEV_THRESH_INNER reserved[SOC_TMC_NOF_TRAFFIC_CLASSES];
}SOC_TMC_EGR_QUEUING_TH_DB_PORT;

typedef struct
{
  
  SOC_TMC_EGR_QUEUING_TH_DB_GLOBAL global;
  
  SOC_TMC_EGR_QUEUING_TH_DB_POOL pool[SOC_TMC_NOF_EGR_QUEUING_MC_POOLS];
  
  SOC_TMC_EGR_QUEUING_TH_DB_POOL pool_tc[SOC_TMC_NOF_EGR_QUEUING_MC_POOLS][SOC_TMC_NOF_TRAFFIC_CLASSES];
  
  SOC_TMC_EGR_QUEUING_TH_DB_PORT thresh_type[SOC_TMC_NOF_THRESH_TYPES];
}SOC_TMC_EGR_QUEUING_DEV_TH;

typedef struct
{
  
  uint32 pool_eligibility;
  
  uint32 pool_id;
  
  uint32 tc_group;
}SOC_TMC_EGR_QUEUING_MC_COS_MAP;

typedef struct
{
  
  uint32 mc_pd_profile;
  
  uint32 uc_profile;
}SOC_TMC_EGR_QUEUING_IF_FC;

typedef struct
{
  
  uint32 pd_th;
  

  uint32 pd_th_min;
  int32 pd_th_alpha;
  uint32 size256_th;
  uint32 size256_th_min;
  int32 size256_th_alpha;
}SOC_TMC_EGR_QUEUING_IF_UC_FC;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_TCG_NDX tcg_ndx[SOC_TMC_EGR_NOF_Q_PRIO_ARAD];
} SOC_TMC_EGR_QUEUING_TCG_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 tcg_weight;

  
  uint8 tcg_weight_valid;

} SOC_TMC_EGR_TCG_SCH_WFQ;

typedef struct  {
    int mc_reserved_pds;
    int threshold_port;
    int threshold_queue;
    int port_mc_drop_pds;
    int port_mc_drop_dbs;
    int drop_pds_th;
    int drop_dbs_th;
    int drop_pds_th_tc;
} SOC_TMC_EGR_QUEUING_CGM_INIT_THRESHOLDS;












void
  SOC_TMC_EGR_DROP_THRESH_clear(
    SOC_SAND_OUT SOC_TMC_EGR_DROP_THRESH *info
  );

void
  SOC_TMC_EGR_THRESH_INFO_clear(
    SOC_SAND_OUT SOC_TMC_EGR_THRESH_INFO *info
  );

void
  SOC_TMC_EGR_QUEUING_TH_DB_GLOBAL_clear(
    SOC_SAND_OUT SOC_TMC_EGR_QUEUING_TH_DB_GLOBAL *info
  );

void
  SOC_TMC_EGR_QUEUING_TH_DB_POOL_clear(
    SOC_SAND_OUT SOC_TMC_EGR_QUEUING_TH_DB_POOL *info
  );

void
  SOC_TMC_EGR_QUEUING_TH_DB_PORT_clear(
    SOC_SAND_OUT SOC_TMC_EGR_QUEUING_TH_DB_PORT *info
  );

void
  SOC_TMC_EGR_QUEUING_DEV_TH_clear(
    SOC_SAND_OUT SOC_TMC_EGR_QUEUING_DEV_TH *info
  );

void
  SOC_TMC_EGR_QUEUING_MC_COS_MAP_clear(
    SOC_SAND_OUT SOC_TMC_EGR_QUEUING_MC_COS_MAP *info
  );

void
  SOC_TMC_EGR_QUEUING_IF_FC_clear(
    SOC_SAND_OUT SOC_TMC_EGR_QUEUING_IF_FC *info
  );
void
  SOC_TMC_EGR_QUEUING_IF_UC_FC_clear(
    SOC_SAND_OUT SOC_TMC_EGR_QUEUING_IF_UC_FC *info
  );
void
  SOC_TMC_EGR_FC_DEV_THRESH_INNER_clear(
    SOC_SAND_OUT SOC_TMC_EGR_FC_DEV_THRESH_INNER *info
  );

void
  SOC_TMC_EGR_FC_DEVICE_THRESH_clear(
    SOC_SAND_OUT SOC_TMC_EGR_FC_DEVICE_THRESH *info
  );

void
  SOC_TMC_EGR_FC_MCI_THRESH_clear(
    SOC_SAND_OUT SOC_TMC_EGR_FC_MCI_THRESH *info
  );

void
  SOC_TMC_EGR_FC_CHNIF_THRESH_clear(
    SOC_SAND_OUT SOC_TMC_EGR_FC_CHNIF_THRESH *info
  );

void
  SOC_TMC_EGR_FC_OFP_THRESH_clear(
    SOC_SAND_OUT SOC_TMC_EGR_FC_OFP_THRESH *info
  );

void
  SOC_TMC_EGR_OFP_SCH_WFQ_clear(
    SOC_SAND_OUT SOC_TMC_EGR_OFP_SCH_WFQ *info
  );

void
  SOC_TMC_EGR_OFP_SCH_INFO_clear(
    SOC_SAND_OUT SOC_TMC_EGR_OFP_SCH_INFO *info
  );

void
  SOC_TMC_EGR_Q_PRIORITY_clear(
    SOC_SAND_OUT SOC_TMC_EGR_Q_PRIORITY *info
  );

void
  SOC_TMC_EGR_QUEUING_TCG_INFO_clear(
    SOC_SAND_OUT SOC_TMC_EGR_QUEUING_TCG_INFO *info
  );

void
  SOC_TMC_EGR_TCG_SCH_WFQ_clear(
    SOC_SAND_OUT SOC_TMC_EGR_TCG_SCH_WFQ *info
   );

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_EGR_Q_PRIO_to_string(
    SOC_SAND_IN SOC_TMC_EGR_Q_PRIO enum_val
  );

const char*
  SOC_TMC_EGR_PORT_THRESH_TYPE_to_string(
    SOC_SAND_IN SOC_TMC_EGR_PORT_THRESH_TYPE enum_val
  );

const char*
  SOC_TMC_EGR_MCI_PRIO_to_string(
    SOC_SAND_IN SOC_TMC_EGR_MCI_PRIO enum_val
  );

const char*
  SOC_TMC_EGR_OFP_INTERFACE_PRIO_to_string(
    SOC_SAND_IN SOC_TMC_EGR_OFP_INTERFACE_PRIO enum_val
  );

const char*
  SOC_TMC_EGR_OFP_SCH_MODE_to_string(
    SOC_SAND_IN SOC_TMC_EGR_OFP_SCH_MODE enum_val
  );

const char*
  SOC_TMC_EGR_Q_PRIO_MAPPING_TYPE_to_string(
    SOC_SAND_IN SOC_TMC_EGR_Q_PRIO_MAPPING_TYPE enum_val
  );

void
  SOC_TMC_EGR_DROP_THRESH_print(
    SOC_SAND_IN SOC_TMC_EGR_DROP_THRESH *info
  );

void
  SOC_TMC_EGR_THRESH_INFO_print(
    SOC_SAND_IN SOC_TMC_EGR_THRESH_INFO *info
  );

void
  SOC_TMC_EGR_FC_DEV_THRESH_INNER_print(
    SOC_SAND_IN SOC_TMC_EGR_FC_DEV_THRESH_INNER *info
  );

void
  SOC_TMC_EGR_FC_DEVICE_THRESH_print(
    SOC_SAND_IN SOC_TMC_EGR_FC_DEVICE_THRESH *info
  );

void
  SOC_TMC_EGR_FC_MCI_THRESH_print(
    SOC_SAND_IN SOC_TMC_EGR_FC_MCI_THRESH *info
  );

void
  SOC_TMC_EGR_FC_CHNIF_THRESH_print(
    SOC_SAND_IN SOC_TMC_EGR_FC_CHNIF_THRESH *info
  );

void
  SOC_TMC_EGR_FC_OFP_THRESH_print(
    SOC_SAND_IN SOC_TMC_EGR_FC_OFP_THRESH *info
  );

void
  SOC_TMC_EGR_OFP_SCH_WFQ_print(
    SOC_SAND_IN SOC_TMC_EGR_OFP_SCH_WFQ *info
  );

void
  SOC_TMC_EGR_OFP_SCH_INFO_print(
    SOC_SAND_IN SOC_TMC_EGR_OFP_SCH_INFO *info
  );

void
  SOC_TMC_EGR_QUEUING_TCG_INFO_print(
    SOC_SAND_IN  SOC_TMC_EGR_QUEUING_TCG_INFO *info
  );

void
  SOC_TMC_EGR_TCG_SCH_WFQ_print(
    SOC_SAND_IN  SOC_TMC_EGR_TCG_SCH_WFQ *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
