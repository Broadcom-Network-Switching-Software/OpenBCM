/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __SOC_TMC_API_END2END_SCHEDULER_INCLUDED__

#define __SOC_TMC_API_END2END_SCHEDULER_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/TMC/tmc_api_general.h>
#include <soc/dpp/TMC/tmc_api_ofp_rates.h>
#include <soc/dpp/TMC/tmc_api_ingress_packet_queuing.h>





#define SOC_TMC_COSQ_TOTAL_FLOW_REGIONS            128

#define SOC_TMC_COSQ_ANY_NOF_REMOTE_CORES          0xffff


#define SOC_TMC_SCH_MAX_NOF_DISCRETE_WEIGHT_VALS     4


#define SOC_TMC_SCH_TOTAL_NOF_IFS                36


#define SOC_TMC_IF_NOF_ENTRIES               SOC_TMC_SCH_TOTAL_NOF_IFS


#define SOC_TMC_SCH_NOF_LINKS                    36


#define SOC_TMC_SCH_NOF_RCI_LEVELS               8


#define SOC_TMC_SCH_DRT_SIZE                     296


#define SOC_TMC_SCH_NOF_CLASS_TYPES              256


#define SOC_TMC_SCH_NOF_SUB_FLOWS                2


#define SOC_TMC_SCH_FLOW_BASE_AGGR_FLOW_ID_PETRA (24*1024)
#define SOC_TMC_SCH_MAX_SE_ID_PETRA              (16*1024 - 1)

#define SOC_TMC_SCH_SE_ID_INVALID_PETRA          (16*1024)

#define SOC_TMC_SCH_FLOW_BASE_AGGR_FLOW_ID_ARAD  (64*1024)
#define SOC_TMC_SCH_MAX_SE_ID_ARAD               (32*1024 - 1)
#define SOC_TMC_SCH_SE_ID_INVALID_ARAD           (32*1024)


#define SOC_TMC_SCH_FLOW_ID_INVALID_PETRA        (56*1024)
#define SOC_TMC_SCH_MAX_FLOW_ID_PETRA            (56*1024 - 1)

#define SOC_TMC_SCH_FLOW_ID_INVALID_ARAD         (128*1024)
#define SOC_TMC_SCH_MAX_FLOW_ID_ARAD             (128*1024 - 1)


#define SOC_TMC_SCH_MAX_PORT_ID_PETRA            80


#define SOC_TMC_SCH_MAX_PORT_ID_ARAD             255


#define SOC_TMC_SCH_PORT_ID_INVALID_PETRA        81
#define SOC_TMC_SCH_PORT_ID_INVALID_ARAD         256


#define SOC_TMC_SCH_FLOW_HR_MIN_WEIGHT           1
#define SOC_TMC_SCH_FLOW_HR_MAX_WEIGHT           4096

#define SOC_TMC_SCH_SUB_FLOW_CL_MIN_WEIGHT           1
#define SOC_TMC_SCH_SUB_FLOW_CL_MAX_WEIGHT_MODES_3_4 64
#define SOC_TMC_SCH_SUB_FLOW_CL_MAX_WEIGHT_MODE_5    256

#define SOC_TMC_SCH_NOF_SLOW_RATES                   2


#define SOC_TMC_SCH_LOW_FC_NOF_AVAIL_CONFS           4


#define SOC_TMC_SCH_LOW_FC_NOF_VALID_CONFS           5

#define SOC_TMC_SCH_NOF_GROUPS                       3


#define SOC_TMC_SCH_NOF_IF_WEIGHTS                   8


#define SOC_TMC_SCH_PORT_MAX_EXPECTED_RATE_AUTO  1

#define SOC_TMC_SCH_SUB_FLOW_SHAPE_NO_LIMIT        0xffffffff

#define SOC_TMC_SCH_SUB_FLOW_SHAPER_BURST_NO_LIMIT (0x1ff*(256))




#define SOC_TMC_CL_SE_ID_MIN_PETRA   0
#define SOC_TMC_CL_SE_ID_MAX_PETRA   (8  * 1024 - 1      )
#define SOC_TMC_FQ_SE_ID_MIN_PETRA   (8  * 1024          )
#define SOC_TMC_FQ_SE_ID_MAX_PETRA   (16 * 1024 - 256 - 1)
#define SOC_TMC_HR_SE_ID_MIN_PETRA   (16 * 1024 - 256    )
#define SOC_TMC_HR_SE_ID_MAX_PETRA   (16 * 1024 - 1      )


#define SOC_TMC_CL_SE_ID_MIN_ARAD    0
#define SOC_TMC_CL_SE_ID_MAX_ARAD    (16 * 1024 - 1      )
#define SOC_TMC_FQ_SE_ID_MIN_ARAD    (16 * 1024          )
#define SOC_TMC_FQ_SE_ID_MAX_ARAD    (32 * 1024 - 256 - 1)
#define SOC_TMC_HR_SE_ID_MIN_ARAD    (32 * 1024 - 256    )
#define SOC_TMC_HR_SE_ID_MAX_ARAD    (32 * 1024 - 1      )
#define SOC_TMC_HR_SE_REGION_ID      SOC_TMC_COSQ_TOTAL_FLOW_REGIONS


#define SOC_TMC_SCH_CL_OFFSET_IN_QUARTET     0
#define SOC_TMC_SCH_FQ_HR_OFFSET_IN_QUARTET  1

#define SOC_TMC_SCH_DESCRETE_WEIGHT_MAX 1024

#define SOC_TMC_FLOW_AND_UP_MAX_CREDIT_SOURCES  (10)






#define SOC_TMC_SCH_FLOW_TO_QRTT_ID(flow_id) ((flow_id)/4)
#define SOC_TMC_SCH_QRTT_TO_FLOW_ID(q_flow_id) ((q_flow_id)*4)
#define SOC_TMC_SCH_FLOW_TO_1K_ID(flow_id) ((flow_id)/1024)
#define SOC_TMC_SCH_1K_TO_FLOW_ID(k_flow_id) ((k_flow_id)*1024)

#define SOC_TMC_SCH_FLOW_IS_IN_AGGR_RANGE_PETRA(flow_id) \
          ((flow_id) >= SOC_TMC_SCH_FLOW_BASE_AGGR_FLOW_ID_PETRA)
#define SOC_TMC_SCH_1K_FLOWS_IS_IN_AGGR_RANGE_PETRA(k_flow_id) \
          (SOC_TMC_SCH_FLOW_IS_IN_AGGR_RANGE_PETRA(SOC_TMC_SCH_1K_TO_FLOW_ID(k_flow_id)))

#define SOC_TMC_SCH_FLOW_IS_IN_AGGR_RANGE_ARAD(flow_id) \
          ((flow_id) >= SOC_TMC_SCH_FLOW_BASE_AGGR_FLOW_ID_ARAD)
#define SOC_TMC_SCH_1K_FLOWS_IS_IN_AGGR_RANGE_ARAD(k_flow_id) \
          (SOC_TMC_SCH_FLOW_IS_IN_AGGR_RANGE_ARAD(SOC_TMC_SCH_1K_TO_FLOW_ID(k_flow_id)))

#define SOC_TMC_SCH_FLOW_IS_EVEN(f) ((f)&0x1)==0)
#define SOC_TMC_SCH_FLOW_IS_ODD(f) (!(SOC_TMC_SCH_FLOW_IS_EVEN(f)))
#define SOC_TMC_SCH_FLOW_BASE_QRTT_ID(f) (((f)/4)*4)
#define SOC_TMC_SCH_FLOW_ID_IN_QRTT(f) ((f)%4)


#define SOC_TMC_SCH_SUB_FLOW_BASE_FLOW_0_1(sf) ((sf) & ~0x1)
#define SOC_TMC_SCH_SUB_FLOW_BASE_FLOW_0_2(sf) ((sf) & ~0x2)
#define SOC_TMC_SCH_SUB_FLOW_BASE_FLOW(sf,is_odd_even) \
          ((is_odd_even)                         ? \
          SOC_TMC_SCH_SUB_FLOW_BASE_FLOW_0_1(sf) : \
          SOC_TMC_SCH_SUB_FLOW_BASE_FLOW_0_2(sf))


#define SOC_TMC_SCH_SUB_FLOW_SECOND_FLOW(sf,is_odd_even) \
          ((is_odd_even)                        ? \
          (SOC_TMC_SCH_SUB_FLOW_BASE_FLOW_0_1(sf) + 1) : \
          (SOC_TMC_SCH_SUB_FLOW_BASE_FLOW_0_2(sf) + 2))


#define SOC_TMC_SCH_SUB_FLOW_OTHER_FLOW(sf,is_odd_even) \
          ((SOC_TMC_SCH_COMPOSITE_IS_SECOND_SUBFLOW(sf,is_odd_even)) ? \
          (SOC_TMC_SCH_SUB_FLOW_BASE_FLOW(sf,is_odd_even)) : \
          (SOC_TMC_SCH_SUB_FLOW_SECOND_FLOW(sf,is_odd_even)))
 


#define SOC_TMC_SCH_IS_SUB_FLOW_OF_FLOW_0_1(f,sf)    \
  ( ((sf)==SOC_TMC_SCH_SUB_FLOW_BASE_FLOW_0_1(f)) || \
   ((sf)==SOC_TMC_SCH_SUB_FLOW_BASE_FLOW_0_1(f)+1) )


#define SOC_TMC_SCH_IS_SUB_FLOW_OF_FLOW_0_2(f,sf)    \
  ( ((sf)==SOC_TMC_SCH_SUB_FLOW_BASE_FLOW_0_2(f)) || \
   ((sf)==SOC_TMC_SCH_SUB_FLOW_BASE_FLOW_0_2(f)+2) )


#define SOC_TMC_SCH_IS_SUB_FLOW_OF_FLOW(f,sf,is_odd_even)            \
   ( ((is_odd_even)&&(SOC_TMC_SCH_IS_SUB_FLOW_OF_FLOW_0_1(f,sf))) || \
   ((!(is_odd_even)&&(SOC_TMC_SCH_IS_SUB_FLOW_OF_FLOW_0_2(f,sf)))) )


#define SOC_TMC_SCH_COMPOSITE_IS_SECOND_SUBFLOW_0_1(sf) ( (sf)&0x1 )


#define SOC_TMC_SCH_COMPOSITE_IS_SECOND_SUBFLOW_0_2(sf) \
  ( (SOC_TMC_SCH_FLOW_ID_IN_QRTT(sf)==2) ||  (SOC_TMC_SCH_FLOW_ID_IN_QRTT(sf)==3) )

#define SOC_TMC_SCH_COMPOSITE_IS_SECOND_SUBFLOW(sf, is_odd_even) \
  ( ((is_odd_even)&&SOC_TMC_SCH_COMPOSITE_IS_SECOND_SUBFLOW_0_1(sf)) || \
     ((!(is_odd_even))&&SOC_TMC_SCH_COMPOSITE_IS_SECOND_SUBFLOW_0_2(sf)) )


#define SOC_TMC_SCH_INDICATED_SE_ID_IS_VALID_PETRA(id) ((id)!= SOC_TMC_SCH_SE_ID_INVALID_PETRA)
#define SOC_TMC_SCH_INDICATED_PORT_ID_IS_VALID_PETRA(id) ((id)!= SOC_TMC_SCH_PORT_ID_INVALID_PETRA)

#define SOC_TMC_SCH_INDICATED_SE_ID_IS_VALID_ARAD(id) ((id)!= SOC_TMC_SCH_SE_ID_INVALID_ARAD)
#define SOC_TMC_SCH_INDICATED_FLOW_ID_IS_VALID_ARAD(id) ((id)!= SOC_TMC_SCH_FLOW_ID_INVALID_ARAD)
#define SOC_TMC_SCH_INDICATED_PORT_ID_IS_VALID_ARAD(id) ((id)!= SOC_TMC_SCH_PORT_ID_INVALID_ARAD)


#define SOC_TMC_SCH_IS_DISCRETE_WFQ_MODE(m) \
  ( ((m) == SOC_TMC_SCH_CL_WEIGHTS_MODE_DISCRETE_PER_FLOW) || \
    ((m) == SOC_TMC_SCH_CL_WEIGHTS_MODE_DISCRETE_PER_CLASS) )

#define SOC_TMC_SCH_IS_INDEPENDENT_WFQ_MODE(m) \
  ((m) == SOC_TMC_SCH_CL_WEIGHTS_MODE_INDEPENDENT_PER_FLOW)

#define SOC_TMC_SCH_IS_WFQ_CLASS_VAL(class_val)                 \
  ( ((class_val) >= SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ) && \
    ((class_val) <= SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP3_WFQ2)   \
  )







typedef uint32 SOC_TMC_SCH_CL_CLASS_TYPE_ID;


typedef uint32 SOC_TMC_SCH_SE_ID;


typedef uint32 SOC_TMC_SCH_PORT_ID;


typedef uint32 SOC_TMC_SCH_FLOW_ID;

typedef enum
{
  
  SOC_TMC_SCH_CL_MODE_NONE=0,
  
  SOC_TMC_SCH_CL_MODE_1=1,
  
  SOC_TMC_SCH_CL_MODE_2=2,
  
  SOC_TMC_SCH_CL_MODE_3=3,
  
  SOC_TMC_SCH_CL_MODE_4=4,
  
  SOC_TMC_SCH_CL_MODE_5=5,
  
  SOC_TMC_SCH_CL_CLASS_MODE_LAST
}SOC_TMC_SCH_CL_CLASS_MODE;

typedef enum
{
  
  SOC_TMC_SCH_CL_WEIGHTS_MODE_DISCRETE_PER_FLOW=0,
  
  SOC_TMC_SCH_CL_WEIGHTS_MODE_INDEPENDENT_PER_FLOW=1,
  
  SOC_TMC_SCH_CL_WEIGHTS_MODE_DISCRETE_PER_CLASS=2,
  
  SOC_TMC_SCH_CL_CLASS_WEIGHTS_MODE_LAST
}SOC_TMC_SCH_CL_CLASS_WEIGHTS_MODE;

typedef enum
{
  
  SOC_TMC_CL_ENHANCED_MODE_DISABLED=0,
  
  SOC_TMC_CL_ENHANCED_MODE_ENABLED_HP=1,
  
  SOC_TMC_CL_ENHANCED_MODE_ENABLED_LP=2,
  
  SOC_TMC_SCH_CL_ENHANCED_MODE_LAST
}SOC_TMC_SCH_CL_ENHANCED_MODE;

typedef enum
{
  
  SOC_TMC_SCH_SE_TYPE_NONE=0,
  
  SOC_TMC_SCH_SE_TYPE_HR=1,
  
  SOC_TMC_SCH_SE_TYPE_CL=2,
  
  SOC_TMC_SCH_SE_TYPE_FQ=3,
  
  SOC_TMC_SCH_SE_TYPE_LAST
}SOC_TMC_SCH_SE_TYPE;

typedef enum
{
  
  SOC_TMC_AM_SCH_FLOW_TYPE_NONE = SOC_TMC_SCH_SE_TYPE_NONE,
  
  SOC_TMC_AM_SCH_FLOW_TYPE_HR = SOC_TMC_SCH_SE_TYPE_HR,
  
  SOC_TMC_AM_SCH_FLOW_TYPE_CL = SOC_TMC_SCH_SE_TYPE_CL,
  
  SOC_TMC_AM_SCH_FLOW_TYPE_FQ = SOC_TMC_SCH_SE_TYPE_FQ,
  
  SOC_TMC_AM_SCH_FLOW_TYPE_CONNECTOR,
  
  SOC_TMC_AM_SCH_FLOW_TYPE_HR_COMPOSITE,
  
  SOC_TMC_AM_SCH_FLOW_TYPE_CL_COMPOSITE,
  
  SOC_TMC_AM_SCH_FLOW_TYPE_FQ_COMPOSITE,
  
  SOC_TMC_AM_SCH_FLOW_TYPE_CONNECTOR_COMPOSITE,
  
  SOC_TMC_AM_SCH_FLOW_TYPE_LAST
}SOC_TMC_AM_SCH_FLOW_TYPE;


typedef enum
{
  
  SOC_TMC_SCH_SE_STATE_DISABLE=0,
  
  SOC_TMC_SCH_SE_STATE_ENABLE=1,
  
  SOC_TMC_SCH_SE_STATE_LAST
}SOC_TMC_SCH_SE_STATE;

typedef enum
{
  
  SOC_TMC_SCH_HR_MODE_NONE=0,
  
  SOC_TMC_SCH_HR_MODE_SINGLE_WFQ=1,
  
  SOC_TMC_SCH_HR_MODE_DUAL_WFQ=2,
  
  SOC_TMC_SCH_HR_MODE_ENHANCED_PRIO_WFQ=3,
  
  SOC_TMC_SCH_SE_HR_MODE_LAST
}SOC_TMC_SCH_SE_HR_MODE;

typedef enum
{
  
  SOC_TMC_SCH_FLOW_HR_CLASS_NONE=0,
  
  SOC_TMC_SCH_FLOW_HR_CLASS_EF1=1,
  
  SOC_TMC_SCH_FLOW_HR_CLASS_EF2=2,
  
  SOC_TMC_SCH_FLOW_HR_CLASS_EF3=3,
  
  SOC_TMC_SCH_FLOW_HR_SINGLE_CLASS_AF1_WFQ=4,
  
  SOC_TMC_SCH_FLOW_HR_SINGLE_CLASS_BE1=5,
  
  SOC_TMC_SCH_FLOW_HR_DUAL_CLASS_AF1_WFQ=6,
  
  SOC_TMC_SCH_FLOW_HR_DUAL_CLASS_BE1_WFQ=7,
  
  SOC_TMC_SCH_FLOW_HR_DUAL_CLASS_BE2=8,
  
  SOC_TMC_SCH_FLOW_HR_ENHANCED_CLASS_AF1=9,
  
  SOC_TMC_SCH_FLOW_HR_ENHANCED_CLASS_AF2=10,
  
  SOC_TMC_SCH_FLOW_HR_ENHANCED_CLASS_AF3=11,
  
  SOC_TMC_SCH_FLOW_HR_ENHANCED_CLASS_AF4=12,
  
  SOC_TMC_SCH_FLOW_HR_ENHANCED_CLASS_AF5=13,
  
  SOC_TMC_SCH_FLOW_HR_ENHANCED_CLASS_AF6=14,
  
  SOC_TMC_SCH_FLOW_HR_ENHANCED_CLASS_BE1_WFQ=15,
  
  SOC_TMC_SCH_FLOW_HR_ENHANCED_CLASS_BE2=16,
  
  SOC_TMC_SCH_SUB_FLOW_HR_CLASS_LAST
}SOC_TMC_SCH_SUB_FLOW_HR_CLASS;

typedef enum
{
  
  SOC_TMC_SCH_SUB_FLOW_CL_CLASS_NONE=0,
  
  SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP1=1,
  
  SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP2=2,
  
  SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP3=3,
  
  SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP4=4,
  
  SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ=5,
  
  SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ1=6,
  
  SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ2=7,
  
  SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ3=8,
  
  SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ4=9,
  
  SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ=10,
  
  SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ1=11,
  
  SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ2=12,
  
  SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ3=13,
  
  SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP3_WFQ1=14,
  
  SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP3_WFQ2=15,
  
  SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP_0_ENHANCED=16,
  
  SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP_5_ENHANCED=17,
  
  SOC_TMC_SCH_SUB_FLOW_CL_CLASS_LAST
}SOC_TMC_SCH_SUB_FLOW_CL_CLASS;


typedef enum
{
  
  SOC_TMC_SCH_FLOW_IPF_CONFIG_MODE_INVERSE = 0,
  
  SOC_TMC_SCH_FLOW_IPF_CONFIG_MODE_PROPORTIONAL = 1,
  
  SOC_TMC_SCH_NOF_FLOW_IPF_CONFIG_MODES = 2
}SOC_TMC_SCH_FLOW_IPF_CONFIG_MODE;



typedef enum
{
  
  SOC_TMC_SCH_SLOW_RATE_NDX_1=0,
  
  SOC_TMC_SCH_SLOW_RATE_NDX_2=1,
  
  SOC_TMC_SCH_NOF_SLOW_RATE_NDXS=2
}SOC_TMC_SCH_SLOW_RATE_NDX;

typedef enum
{
  
  SOC_TMC_FLOW_NONE=0,
  
  SOC_TMC_FLOW_SIMPLE=1,
  
  SOC_TMC_FLOW_AGGREGATE=2,
  
  SOC_TMC_SCH_FLOW_TYPE_LAST
}SOC_TMC_SCH_FLOW_TYPE;


typedef enum
{
  
  SOC_TMC_SCH_FLOW_OFF=0,
  
  SOC_TMC_SCH_FLOW_ON=2,
  
  SOC_TMC_SCH_FLOW_STATUS_LAST
}SOC_TMC_SCH_FLOW_STATUS;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 rci_level;
  
  uint32 num_active_links;
  
  uint32 rate;
}SOC_TMC_SCH_DEVICE_RATE_ENTRY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_SCH_DEVICE_RATE_ENTRY rates[SOC_TMC_SCH_DRT_SIZE];
}SOC_TMC_SCH_DEVICE_RATE_TABLE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 id;
  
  uint32 val;
}SOC_TMC_SCH_IF_WEIGHT_ENTRY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_SCH_IF_WEIGHT_ENTRY weight[SOC_TMC_SCH_NOF_IF_WEIGHTS];
  
  uint32 nof_enties;
}SOC_TMC_SCH_IF_WEIGHTS;


typedef enum
{
  
  SOC_TMC_SCH_PORT_LOWEST_HP_HR_CLASS_NONE=0,
  
  SOC_TMC_SCH_PORT_LOWEST_HP_HR_CLASS_EF1=1,
  
  SOC_TMC_SCH_PORT_LOWEST_HP_HR_CLASS_EF2=2,
  
  SOC_TMC_SCH_PORT_LOWEST_HP_HR_CLASS_EF3=3,
  
  SOC_TMC_SCH_PORT_LOWEST_HP_HR_SINGLE_CLASS_AF1_WFQ=4,
  
  SOC_TMC_SCH_PORT_LOWEST_HP_HR_DUAL_OR_ENHANCED=5,
  
  SOC_TMC_SCH_PORT_LOWEST_HP_HR_CLASS_LAST
}SOC_TMC_SCH_PORT_LOWEST_HP_HR_CLASS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  
  SOC_TMC_SCH_PORT_LOWEST_HP_HR_CLASS lowest_hp_class[SOC_TMC_SCH_LOW_FC_NOF_AVAIL_CONFS];
}SOC_TMC_SCH_PORT_HP_CLASS_INFO;

typedef enum
{
  
    SOC_TMC_SCH_GROUP_A = 0,
  
  SOC_TMC_SCH_GROUP_B = 1,
  
  SOC_TMC_SCH_GROUP_C = 2,
  
  SOC_TMC_SCH_GROUP_AUTO = 3,

  
  SOC_TMC_SCH_GROUP_NONE = 4,

  SOC_TMC_SCH_GROUP_LAST
}SOC_TMC_SCH_GROUP;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 max_bandwidth;
  
  uint32 credit_bandwidth;
  
  uint32 priority;

} SOC_TMC_EGRESS_PORT_QOS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_EGRESS_PORT_QOS ports[SOC_TMC_NOF_FAP_PORTS];

} SOC_TMC_EGRESS_PORT_QOS_TABLE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  
  SOC_TMC_SCH_CL_CLASS_TYPE_ID id;
  
  SOC_TMC_SCH_CL_CLASS_MODE mode;
  
  uint32 weight[SOC_TMC_SCH_MAX_NOF_DISCRETE_WEIGHT_VALS];
  
  SOC_TMC_SCH_CL_CLASS_WEIGHTS_MODE weight_mode;
  
  SOC_TMC_SCH_CL_ENHANCED_MODE enhanced_mode;
}SOC_TMC_SCH_SE_CL_CLASS_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 nof_class_types;
  
  SOC_TMC_SCH_SE_CL_CLASS_INFO class_types[SOC_TMC_SCH_NOF_CLASS_TYPES];
}SOC_TMC_SCH_SE_CL_CLASS_TABLE;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  SOC_TMC_SCH_SE_HR_MODE mode;
  
  uint32 tcg_ndx;
} SOC_TMC_SCH_SE_HR;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  SOC_TMC_SCH_CL_CLASS_TYPE_ID id;
} SOC_TMC_SCH_SE_CL;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  uint32 no_info;
} SOC_TMC_SCH_SE_FQ;

typedef union
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_SCH_SE_HR hr;
  
  SOC_TMC_SCH_SE_CL cl;
  
  SOC_TMC_SCH_SE_FQ fq;
}SOC_TMC_SCH_SE_PER_TYPE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_SCH_SE_ID id;
  
  SOC_TMC_SCH_SE_STATE state;
  
  SOC_TMC_SCH_SE_TYPE type;
  
  SOC_TMC_SCH_SE_PER_TYPE_INFO type_info;
  
  uint8 is_dual;
  
  SOC_TMC_SCH_GROUP group;
}SOC_TMC_SCH_SE_INFO;




typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 max_rate;
  
  uint32 max_burst;
}SOC_TMC_SCH_SUB_FLOW_SHAPER;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_SCH_SUB_FLOW_HR_CLASS sp_class;
  
  uint32 weight;
}SOC_TMC_SCH_SUB_FLOW_HR;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_SCH_SUB_FLOW_CL_CLASS sp_class;
  
  uint32 weight;
}SOC_TMC_SCH_SUB_FLOW_CL;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  
  uint32 no_val;
}SOC_TMC_SCH_SUB_FLOW_FQ;

typedef union
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_SCH_SUB_FLOW_HR hr;
  
  SOC_TMC_SCH_SUB_FLOW_CL cl;
  
  SOC_TMC_SCH_SUB_FLOW_FQ fq;
}SOC_TMC_SCH_SUB_FLOW_SE_INFO;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  SOC_TMC_SCH_SE_TYPE se_type;
  
  SOC_TMC_SCH_SUB_FLOW_SE_INFO se_info;
  
  SOC_TMC_SCH_SE_ID id;
}SOC_TMC_SCH_SUB_FLOW_CREDIT_SOURCE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 is_valid;
  
  SOC_TMC_SCH_FLOW_ID id ;
  
  SOC_TMC_SCH_SUB_FLOW_SHAPER shaper;
  
  SOC_TMC_SCH_SLOW_RATE_NDX slow_rate_ndx;
  
  SOC_TMC_SCH_SUB_FLOW_CREDIT_SOURCE credit_source;
  
  uint8 update_bw_only ;
}SOC_TMC_SCH_SUBFLOW;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_SCH_SUBFLOW sub_flow[SOC_TMC_SCH_NOF_SUB_FLOWS];
  
  SOC_TMC_SCH_FLOW_TYPE flow_type;
  
  uint8 is_slow_enabled;

}SOC_TMC_SCH_FLOW;



typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 is_interdigitated;
  
  uint8 is_odd_even;
  
  uint8 is_cl_cir;
}SOC_TMC_SCH_GLOBAL_PER1K_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 base_q_qrtt_id;
  
  uint8 is_composite;
  
  uint8 other_quartet_is_valid ;
  
  uint32 fip_id;
}SOC_TMC_SCH_QUARTET_MAPPING_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 rates[SOC_TMC_SCH_NOF_SLOW_RATES];
}SOC_TMC_SCH_SLOW_RATE;

typedef struct
{

  SOC_SAND_MAGIC_NUM_VAR

  
   uint8 enable;

  
  uint32  max_expected_rate;
  
  SOC_TMC_SCH_SE_HR_MODE hr_mode;
  
  SOC_TMC_SCH_SE_HR_MODE hr_modes[SOC_TMC_NOF_TRAFFIC_CLASSES];
  
  SOC_TMC_TCG_NDX tcg_ndx[SOC_TMC_NOF_TRAFFIC_CLASSES];
  
  SOC_TMC_SCH_PORT_LOWEST_HP_HR_CLASS lowest_hp_class;

  
  SOC_TMC_SCH_GROUP group;
} SOC_TMC_SCH_PORT_INFO;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 tcg_weight;

  
  uint8 tcg_weight_valid;

} SOC_TMC_SCH_TCG_WEIGHT;


typedef struct {
    SOC_TMC_SCH_PORT_INFO      port_info;
    SOC_TMC_OFP_RATE_INFO      ofp_rate_info;
    uint32                     credit_rate;
    uint32                     fc_cnt;
    uint32                     fc_percent;
} SOC_TMC_SCH_FLOW_AND_UP_PORT_INFO;

typedef struct {
    SOC_TMC_SCH_SE_INFO        se_info;
    SOC_TMC_SCH_FLOW           sch_consumer;
    uint32                     credit_rate;
    uint32                     credit_rate_overflow;
    SOC_TMC_SCH_CL_CLASS_WEIGHTS_MODE cl_mode;
    SOC_TMC_SCH_FLOW_IPF_CONFIG_MODE ipf_mode;
} SOC_TMC_SCH_FLOW_AND_UP_SE_INFO;

typedef union  {
    SOC_TMC_SCH_FLOW_AND_UP_PORT_INFO        port_sch_info;
    SOC_TMC_SCH_FLOW_AND_UP_SE_INFO          se_sch_info;
} SOC_TMC_SCH_FLOW_AND_UP_UNION_INFO;

typedef struct  {
    uint32                          base_queue;
    SOC_TMC_IPQ_QUARTET_MAP_INFO    qrtt_map_info;
    SOC_TMC_SCH_FLOW                sch_consumer;
    uint32                          credit_rate;
    SOC_TMC_OFP_RATES_TBL_INFO      ofp_rates_table;
    uint32                          ofp_rate_valid;

    
    
    uint32                          credit_sources[SOC_TMC_FLOW_AND_UP_MAX_CREDIT_SOURCES];
    uint32                          credit_sources_nof;

    
    
    uint32                              is_port_sch[SOC_TMC_FLOW_AND_UP_MAX_CREDIT_SOURCES];
    SOC_TMC_SCH_FLOW_AND_UP_UNION_INFO  sch_union_info[SOC_TMC_FLOW_AND_UP_MAX_CREDIT_SOURCES];
    SOC_TMC_SCH_PORT_ID                 sch_port_id[SOC_TMC_FLOW_AND_UP_MAX_CREDIT_SOURCES];
    uint32                              sch_priority_ndx[SOC_TMC_FLOW_AND_UP_MAX_CREDIT_SOURCES];

    
    
    uint32                          next_level_credit_sources[SOC_TMC_FLOW_AND_UP_MAX_CREDIT_SOURCES];
    uint32                          next_level_credit_sources_nof;
} SOC_TMC_SCH_FLOW_AND_UP_INFO;












void
  SOC_TMC_SCH_DEVICE_RATE_ENTRY_clear(
    SOC_SAND_OUT SOC_TMC_SCH_DEVICE_RATE_ENTRY *info
  );

void
  SOC_TMC_SCH_DEVICE_RATE_TABLE_clear(
    SOC_SAND_OUT SOC_TMC_SCH_DEVICE_RATE_TABLE *info
  );

void
  SOC_TMC_SCH_IF_WEIGHT_ENTRY_clear(
    SOC_SAND_OUT SOC_TMC_SCH_IF_WEIGHT_ENTRY *info
  );

void
  SOC_TMC_SCH_IF_WEIGHTS_clear(
    SOC_SAND_OUT SOC_TMC_SCH_IF_WEIGHTS *info
  );

void
  SOC_TMC_SCH_PORT_HP_CLASS_INFO_clear(
    SOC_SAND_OUT SOC_TMC_SCH_PORT_HP_CLASS_INFO *info
  );

void
  SOC_TMC_SCH_PORT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_SCH_PORT_INFO *info
  );

void
  SOC_TMC_SCH_TCG_WEIGHT_clear(
    SOC_SAND_OUT SOC_TMC_SCH_TCG_WEIGHT *tcg_weight
  );

void
  SOC_TMC_SCH_SE_HR_clear(
    SOC_SAND_OUT SOC_TMC_SCH_SE_HR *info
  );

void
  SOC_TMC_SCH_SE_CL_clear(
    SOC_SAND_OUT SOC_TMC_SCH_SE_CL *info
  );

void
  SOC_TMC_SCH_SE_FQ_clear(
    SOC_SAND_OUT SOC_TMC_SCH_SE_FQ *info
  );

void
  SOC_TMC_SCH_SE_CL_CLASS_INFO_clear(
    SOC_SAND_OUT SOC_TMC_SCH_SE_CL_CLASS_INFO *info
  );

void
  SOC_TMC_SCH_SE_CL_CLASS_TABLE_clear(
    SOC_SAND_OUT SOC_TMC_SCH_SE_CL_CLASS_TABLE *info
  );

void
  SOC_TMC_SCH_SE_PER_TYPE_INFO_clear(
    SOC_SAND_OUT SOC_TMC_SCH_SE_PER_TYPE_INFO *info
  );

void
  SOC_TMC_SCH_SE_INFO_clear(
    SOC_SAND_OUT SOC_TMC_SCH_SE_INFO *info
  );

void
  SOC_TMC_SCH_SUB_FLOW_SHAPER_clear(
    int unit,
    SOC_SAND_OUT SOC_TMC_SCH_SUB_FLOW_SHAPER *info
  );

void
  SOC_TMC_SCH_SUB_FLOW_HR_clear(
    SOC_SAND_OUT SOC_TMC_SCH_SUB_FLOW_HR *info
  );

void
  SOC_TMC_SCH_SUB_FLOW_CL_clear(
    SOC_SAND_OUT SOC_TMC_SCH_SUB_FLOW_CL *info
  );


void
  SOC_TMC_SCH_SUB_FLOW_SE_INFO_clear(
    SOC_SAND_OUT SOC_TMC_SCH_SUB_FLOW_SE_INFO *info
  );

void
  SOC_TMC_SCH_SUB_FLOW_CREDIT_SOURCE_clear(
    SOC_SAND_OUT SOC_TMC_SCH_SUB_FLOW_CREDIT_SOURCE *info
  );

void
  SOC_TMC_SCH_SUBFLOW_clear(
    int unit,
    SOC_SAND_OUT SOC_TMC_SCH_SUBFLOW *info
  );

void
  SOC_TMC_SCH_FLOW_clear(
    int unit,
    SOC_SAND_OUT SOC_TMC_SCH_FLOW *info
  );

void
  SOC_TMC_SCH_GLOBAL_PER1K_INFO_clear(
    SOC_SAND_OUT SOC_TMC_SCH_GLOBAL_PER1K_INFO *info
  );

void
  SOC_TMC_SCH_QUARTET_MAPPING_INFO_clear(
    SOC_SAND_OUT SOC_TMC_SCH_QUARTET_MAPPING_INFO *info
  );

void
  SOC_TMC_SCH_SLOW_RATE_clear(
    SOC_SAND_OUT SOC_TMC_SCH_SLOW_RATE *info
  );

void
  SOC_TMC_SCH_FLOW_AND_UP_PORT_INFO_clear(
     SOC_SAND_OUT SOC_TMC_SCH_FLOW_AND_UP_PORT_INFO *info
  );

void
  SOC_TMC_SCH_FLOW_AND_UP_SE_INFO_clear(
     int unit,
     SOC_SAND_OUT SOC_TMC_SCH_FLOW_AND_UP_SE_INFO *info
  );

void
  SOC_TMC_SCH_FLOW_AND_UP_INFO_clear(
     int unit,
     SOC_SAND_OUT SOC_TMC_SCH_FLOW_AND_UP_INFO *info,
     SOC_SAND_IN uint32                         is_full 
  );

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_SCH_PORT_LOWEST_HP_HR_CLASS_to_string(
    SOC_SAND_IN SOC_TMC_SCH_PORT_LOWEST_HP_HR_CLASS enum_val
  );

const char*
  SOC_TMC_SCH_CL_CLASS_MODE_to_string(
    SOC_SAND_IN SOC_TMC_SCH_CL_CLASS_MODE enum_val
  );

const char*
  SOC_TMC_SCH_CL_CLASS_WEIGHTS_MODE_to_string(
    SOC_SAND_IN SOC_TMC_SCH_CL_CLASS_WEIGHTS_MODE enum_val
  );

const char*
  SOC_TMC_SCH_CL_ENHANCED_MODE_to_string(
    SOC_SAND_IN SOC_TMC_SCH_CL_ENHANCED_MODE enum_val
  );

const char*
  SOC_TMC_SCH_GROUP_to_string(
    SOC_SAND_IN SOC_TMC_SCH_GROUP enum_val
  );

const char*
  SOC_TMC_SCH_SE_TYPE_to_string(
    SOC_SAND_IN SOC_TMC_SCH_SE_TYPE enum_val
  );

const char*
  SOC_TMC_SCH_SE_STATE_to_string(
    SOC_SAND_IN SOC_TMC_SCH_SE_STATE enum_val
  );

const char*
  SOC_TMC_SCH_SE_HR_MODE_to_string(
    SOC_SAND_IN SOC_TMC_SCH_SE_HR_MODE enum_val
  );

const char*
  SOC_TMC_SCH_SUB_FLOW_HR_CLASS_to_string(
    SOC_SAND_IN SOC_TMC_SCH_SUB_FLOW_HR_CLASS enum_val
  );

const char*
  SOC_TMC_SCH_SUB_FLOW_CL_CLASS_to_string(
    SOC_SAND_IN SOC_TMC_SCH_SUB_FLOW_CL_CLASS enum_val
  );

const char*
  SOC_TMC_SCH_SLOW_RATE_NDX_to_string(
    SOC_SAND_IN SOC_TMC_SCH_SLOW_RATE_NDX enum_val
  );

const char*
  SOC_TMC_SCH_FLOW_TYPE_to_string(
    SOC_SAND_IN SOC_TMC_SCH_FLOW_TYPE enum_val
  );

const char*
  SOC_TMC_SCH_FLOW_STATUS_to_string(
    SOC_SAND_IN SOC_TMC_SCH_FLOW_STATUS enum_val
  );

const char*
  SOC_TMC_SCH_FLOW_IPF_CONFIG_MODE_to_string(
    SOC_SAND_IN  SOC_TMC_SCH_FLOW_IPF_CONFIG_MODE enum_val
  );

void
  SOC_TMC_SCH_DEVICE_RATE_ENTRY_print(
    SOC_SAND_IN SOC_TMC_SCH_DEVICE_RATE_ENTRY *info
  );

void
  SOC_TMC_SCH_DEVICE_RATE_TABLE_print(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_IN SOC_TMC_SCH_DEVICE_RATE_TABLE *info
  );

void
  SOC_TMC_SCH_IF_WEIGHT_ENTRY_print(
    SOC_SAND_IN SOC_TMC_SCH_IF_WEIGHT_ENTRY *info
  );

void
  SOC_TMC_SCH_IF_WEIGHTS_print(
    SOC_SAND_IN SOC_TMC_SCH_IF_WEIGHTS *info
  );

void
  SOC_TMC_SCH_PORT_HP_CLASS_INFO_print(
    SOC_SAND_IN SOC_TMC_SCH_PORT_HP_CLASS_INFO *info
  );

void
  SOC_TMC_SCH_PORT_INFO_print(
    SOC_SAND_IN SOC_TMC_SCH_PORT_INFO *info,
    SOC_SAND_IN uint32           port_id
  );

void
  SOC_TMC_SCH_TCG_WEIGHT_print(
    SOC_SAND_IN SOC_TMC_SCH_TCG_WEIGHT *tcg_weight
  );
void
  SOC_TMC_SCH_SE_HR_print(
    SOC_SAND_IN SOC_TMC_SCH_SE_HR *info
  );

void
  SOC_TMC_SCH_SE_CL_print(
    SOC_SAND_IN SOC_TMC_SCH_SE_CL *info
  );

void
  SOC_TMC_SCH_SE_FQ_print(
    SOC_SAND_IN SOC_TMC_SCH_SE_FQ *info
  );

void
  SOC_TMC_SCH_SE_CL_CLASS_INFO_print(
    SOC_SAND_IN SOC_TMC_SCH_SE_CL_CLASS_INFO *info
  );

void
  SOC_TMC_SCH_SE_CL_CLASS_TABLE_print(
    SOC_SAND_IN SOC_TMC_SCH_SE_CL_CLASS_TABLE *info
  );

void
  SOC_TMC_SCH_SE_PER_TYPE_INFO_print(
    SOC_SAND_IN SOC_TMC_SCH_SE_PER_TYPE_INFO *info,
    SOC_SAND_IN SOC_TMC_SCH_SE_TYPE type
  );

void
  SOC_TMC_SCH_SE_INFO_print(
    SOC_SAND_IN SOC_TMC_SCH_SE_INFO *info
  );

void
  SOC_TMC_SCH_SUB_FLOW_SHAPER_print(
    SOC_SAND_IN SOC_TMC_SCH_SUB_FLOW_SHAPER *info
  );

void
  SOC_TMC_SCH_SUB_FLOW_HR_print(
    SOC_SAND_IN SOC_TMC_SCH_SUB_FLOW_HR *info
  );

void
  SOC_TMC_SCH_SUB_FLOW_CL_print(
    SOC_SAND_IN SOC_TMC_SCH_SUB_FLOW_CL *info
  );

void
  SOC_TMC_SCH_SUB_FLOW_FQ_print(
    SOC_SAND_IN SOC_TMC_SCH_SUB_FLOW_FQ *info
  );

void
  SOC_TMC_SCH_SUB_FLOW_SE_INFO_print(
    SOC_SAND_IN SOC_TMC_SCH_SUB_FLOW_SE_INFO *info,
    SOC_SAND_IN SOC_TMC_SCH_SE_TYPE se_type
  );

void
  SOC_TMC_SCH_SUB_FLOW_CREDIT_SOURCE_print(
    SOC_SAND_IN SOC_TMC_SCH_SUB_FLOW_CREDIT_SOURCE *info
  );

void
  SOC_TMC_SCH_SUBFLOW_print(
    SOC_SAND_IN SOC_TMC_SCH_SUBFLOW *info,
    SOC_SAND_IN uint8 is_table_flow,
    SOC_SAND_IN uint32 subflow_id
  );

void
  SOC_TMC_SCH_FLOW_print(
    SOC_SAND_IN SOC_TMC_SCH_FLOW *info,
    SOC_SAND_IN uint8 is_table
  );

void
  SOC_TMC_SCH_GLOBAL_PER1K_INFO_print(
    SOC_SAND_IN SOC_TMC_SCH_GLOBAL_PER1K_INFO *info
  );

void
  SOC_TMC_SCH_QUARTET_MAPPING_INFO_print(
    SOC_SAND_IN SOC_TMC_SCH_QUARTET_MAPPING_INFO *info
  );

void
  SOC_TMC_SCH_SLOW_RATE_print(
    SOC_SAND_IN SOC_TMC_SCH_SLOW_RATE *info
  );


#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
