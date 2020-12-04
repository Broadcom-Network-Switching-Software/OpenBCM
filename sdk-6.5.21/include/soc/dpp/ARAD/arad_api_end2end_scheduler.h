/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __ARAD_API_END2END_SCHEDULER_INCLUDED__

#define __ARAD_API_END2END_SCHEDULER_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/TMC/tmc_api_end2end_scheduler.h>




#define  ARAD_SCH_PORT_MAX_EXPECTED_RATE_AUTO (SOC_TMC_SCH_PORT_MAX_EXPECTED_RATE_AUTO)

#define ARAD_SCH_PORT_LOWEST_HP_HR_CLASS_LAST          SOC_TMC_SCH_PORT_LOWEST_HP_HR_CLASS_LAST
#define ARAD_SCH_SE_HR_MODE_LAST                       SOC_TMC_SCH_SE_HR_MODE_LAST
#define ARAD_SCH_CL_CLASS_MODE_LAST                    SOC_TMC_SCH_CL_CLASS_MODE_LAST
#define ARAD_SCH_CL_CLASS_WEIGHTS_MODE_LAST            SOC_TMC_SCH_CL_CLASS_WEIGHTS_MODE_LAST
#define ARAD_SCH_CL_ENHANCED_MODE_LAST                 SOC_TMC_SCH_CL_ENHANCED_MODE_LAST
#define ARAD_SCH_SE_STATE_LAST                         SOC_TMC_SCH_SE_STATE_LAST
#define ARAD_SCH_SE_TYPE_LAST                          SOC_TMC_SCH_SE_TYPE_LAST
#define ARAD_SCH_SUB_FLOW_HR_CLASS_LAST                SOC_TMC_SCH_SUB_FLOW_HR_CLASS_LAST
#define ARAD_SCH_SUB_FLOW_CL_CLASS_LAST                SOC_TMC_SCH_SUB_FLOW_CL_CLASS_LAST
#define ARAD_SCH_FLOW_TYPE_LAST                        SOC_TMC_SCH_FLOW_TYPE_LAST

#define ARAD_SCH_MAX_NOF_DISCRETE_WEIGHT_VALS     SOC_TMC_SCH_MAX_NOF_DISCRETE_WEIGHT_VALS


#define ARAD_SCH_TOTAL_NOF_IFS                SOC_TMC_SCH_TOTAL_NOF_IFS


#define ARAD_IF_NOF_ENTRIES                   SOC_TMC_IF_NOF_ENTRIES


#define ARAD_SCH_NOF_LINKS                    SOC_TMC_SCH_NOF_LINKS


#define ARAD_SCH_NOF_RCI_LEVELS               SOC_TMC_SCH_NOF_RCI_LEVELS


#define ARAD_SCH_DRT_SIZE                     SOC_TMC_SCH_DRT_SIZE


#define ARAD_SCH_NOF_CLASS_TYPES              SOC_TMC_SCH_NOF_CLASS_TYPES


#define ARAD_SCH_NOF_SUB_FLOWS                SOC_TMC_SCH_NOF_SUB_FLOWS


#define ARAD_SCH_FLOW_ID_FIXED_TYPE0_END(unit)      (SOC_DPP_DEFS_GET(unit, flow_region_fixed_type0_end)*1024)
#define ARAD_SCH_FLOW_BASE_AGGR_FLOW_ID       SOC_TMC_SCH_FLOW_BASE_AGGR_FLOW_ID_ARAD
#define ARAD_SCH_MAX_SE_ID                    SOC_TMC_SCH_MAX_SE_ID_ARAD


#define ARAD_SCH_SE_ID_INVALID                SOC_TMC_SCH_SE_ID_INVALID_ARAD

#define ARAD_SCH_MAX_FLOW_ID                  SOC_TMC_SCH_MAX_FLOW_ID_ARAD


#define ARAD_SCH_FLOW_ID_INVALID              SOC_TMC_SCH_FLOW_ID_INVALID_ARAD

#define ARAD_SCH_MAX_PORT_ID                  SOC_TMC_SCH_MAX_PORT_ID_ARAD


#define ARAD_SCH_PORT_ID_INVALID              SOC_TMC_SCH_PORT_ID_INVALID_ARAD

#define ARAD_SCH_FLOW_HR_MIN_WEIGHT           SOC_TMC_SCH_FLOW_HR_MIN_WEIGHT
#define ARAD_SCH_FLOW_HR_MAX_WEIGHT           SOC_TMC_SCH_FLOW_HR_MAX_WEIGHT

#define ARAD_SCH_SUB_FLOW_CL_MIN_WEIGHT           SOC_TMC_SCH_SUB_FLOW_CL_MIN_WEIGHT
#define ARAD_SCH_SUB_FLOW_CL_MAX_WEIGHT_MODES_3_4 SOC_TMC_SCH_SUB_FLOW_CL_MAX_WEIGHT_MODES_3_4
#define ARAD_SCH_SUB_FLOW_CL_MAX_WEIGHT_MODE_5    SOC_TMC_SCH_SUB_FLOW_CL_MAX_WEIGHT_MODE_5

#define ARAD_SCH_NOF_SLOW_RATES                   SOC_TMC_SCH_NOF_SLOW_RATES


#define ARAD_SCH_LOW_FC_NOF_AVAIL_CONFS           SOC_TMC_SCH_LOW_FC_NOF_AVAIL_CONFS


#define ARAD_SCH_LOW_FC_NOF_VALID_CONFS           SOC_TMC_SCH_LOW_FC_NOF_VALID_CONFS

#define ARAD_SCH_NOF_GROUPS                       SOC_TMC_SCH_NOF_GROUPS


#define ARAD_SCH_NOF_IF_WEIGHTS                   SOC_TMC_SCH_NOF_IF_WEIGHTS

#define ARAD_SCH_SUB_FLOW_SHAPE_NO_LIMIT (SOC_TMC_SCH_SUB_FLOW_SHAPE_NO_LIMIT)
#define ARAD_SCH_SUB_FLOW_SHAPER_BURST_NO_LIMIT (SOC_TMC_SCH_SUB_FLOW_SHAPER_BURST_NO_LIMIT)





#define ARAD_CL_SE_ID_MIN   SOC_TMC_CL_SE_ID_MIN_ARAD
#define ARAD_CL_SE_ID_MAX   SOC_TMC_CL_SE_ID_MAX_ARAD
#define ARAD_FQ_SE_ID_MIN   SOC_TMC_FQ_SE_ID_MIN_ARAD
#define ARAD_FQ_SE_ID_MAX   SOC_TMC_FQ_SE_ID_MAX_ARAD
#define ARAD_HR_SE_ID_MIN   SOC_TMC_HR_SE_ID_MIN_ARAD
#define ARAD_HR_SE_ID_MAX   SOC_TMC_HR_SE_ID_MAX_ARAD

#define ARAD_SCH_CL_OFFSET_IN_QUARTET     SOC_TMC_SCH_CL_OFFSET_IN_QUARTET
#define ARAD_SCH_FQ_HR_OFFSET_IN_QUARTET  SOC_TMC_SCH_FQ_HR_OFFSET_IN_QUARTET

#define ARAD_SCH_DESCRETE_WEIGHT_MAX      SOC_TMC_SCH_DESCRETE_WEIGHT_MAX






#define ARAD_SCH_FLOW_TO_QRTT_ID(flow_id)             SOC_TMC_SCH_FLOW_TO_QRTT_ID(flow_id)
#define ARAD_SCH_QRTT_TO_FLOW_ID(q_flow_id)           SOC_TMC_SCH_QRTT_TO_FLOW_ID(q_flow_id)
#define ARAD_SCH_FLOW_TO_1K_ID(flow_id)               SOC_TMC_SCH_FLOW_TO_1K_ID(flow_id)
#define ARAD_SCH_1K_TO_FLOW_ID(k_flow_id)             SOC_TMC_SCH_1K_TO_FLOW_ID(k_flow_id)


#define ARAD_SCH_FLOW_IS_IN_AGGR_RANGE(flow_id)       SOC_TMC_SCH_FLOW_IS_IN_AGGR_RANGE_ARAD(flow_id)
#define ARAD_SCH_1K_FLOWS_IS_IN_AGGR_RANGE(k_flow_id) SOC_TMC_SCH_1K_FLOWS_IS_IN_AGGR_RANGE_ARAD(k_flow_id)

#define ARAD_SCH_FLOW_IS_EVEN(f)                      SOC_TMC_SCH_FLOW_IS_EVEN(f)
#define ARAD_SCH_FLOW_IS_ODD(f)                       SOC_TMC_SCH_FLOW_IS_ODD(f)
#define ARAD_SCH_FLOW_BASE_QRTT_ID(f)                 SOC_TMC_SCH_FLOW_BASE_QRTT_ID(f)
#define ARAD_SCH_FLOW_ID_IN_QRTT(f)                   SOC_TMC_SCH_FLOW_ID_IN_QRTT(f)


#define ARAD_SCH_SUB_FLOW_BASE_FLOW_0_1(sf)           SOC_TMC_SCH_SUB_FLOW_BASE_FLOW_0_1(sf)
#define ARAD_SCH_SUB_FLOW_BASE_FLOW_0_2(sf)           SOC_TMC_SCH_SUB_FLOW_BASE_FLOW_0_2(sf)
#define ARAD_SCH_SUB_FLOW_BASE_FLOW(sf,is_odd_even)   SOC_TMC_SCH_SUB_FLOW_BASE_FLOW(sf,is_odd_even)

#define ARAD_SCH_SUB_FLOW_SECOND_FLOW(sf,is_odd_even)   SOC_TMC_SCH_SUB_FLOW_SECOND_FLOW(sf,is_odd_even)

#define ARAD_SCH_SUB_FLOW_OTHER_FLOW(sf,is_odd_even)    SOC_TMC_SCH_SUB_FLOW_OTHER_FLOW(sf,is_odd_even)


#define ARAD_SCH_IS_SUB_FLOW_OF_FLOW_0_1(f,sf)        SOC_TMC_SCH_IS_SUB_FLOW_OF_FLOW_0_1(f,sf)


#define ARAD_SCH_IS_SUB_FLOW_OF_FLOW_0_2(f,sf)        SOC_TMC_SCH_IS_SUB_FLOW_OF_FLOW_0_2(f,sf)


#define ARAD_SCH_IS_SUB_FLOW_OF_FLOW(f,sf,is_odd_even) SOC_TMC_SCH_IS_SUB_FLOW_OF_FLOW(f,sf,is_odd_even)


#define ARAD_SCH_COMPOSITE_IS_SECOND_SUBFLOW_0_1(sf)   SOC_TMC_SCH_COMPOSITE_IS_SECOND_SUBFLOW_0_1(sf)


#define ARAD_SCH_COMPOSITE_IS_SECOND_SUBFLOW_0_2(sf)   SOC_TMC_SCH_COMPOSITE_IS_SECOND_SUBFLOW_0_2(sf)

#define ARAD_SCH_COMPOSITE_IS_SECOND_SUBFLOW(sf, is_odd_even) SOC_TMC_SCH_COMPOSITE_IS_SECOND_SUBFLOW(sf, is_odd_even)


#define ARAD_SCH_INDICATED_SE_ID_IS_VALID(id)           SOC_TMC_SCH_INDICATED_SE_ID_IS_VALID_ARAD(id)
#define ARAD_SCH_INDICATED_FLOW_ID_IS_VALID(id)         SOC_TMC_SCH_INDICATED_FLOW_ID_IS_VALID_ARAD(id)
#define ARAD_SCH_INDICATED_PORT_ID_IS_VALID(id)         SOC_TMC_SCH_INDICATED_PORT_ID_IS_VALID_ARAD(id)

#define ARAD_SCH_VERIFY_SE_ID(id, err_no,exit_place)   \
{                                                       \
  if(!(ARAD_SCH_INDICATED_SE_ID_IS_VALID(id)))         \
  {                                                     \
  SOC_SAND_SET_ERROR_CODE(ARAD_SCH_INVALID_SE_ID_ERR,err_no, exit_place); \
  }                                                     \
}

#define ARAD_SCH_VERIFY_FLOW_ID(id, err_no, exit_place)  \
{                                                         \
  if( !(ARAD_SCH_INDICATED_FLOW_ID_IS_VALID(id)) )       \
  {                                                       \
  SOC_SAND_SET_ERROR_CODE(ARAD_SCH_INVALID_FLOW_ID_ERR,err_no,exit_place); \
  }                                                       \
}



#define ARAD_SCH_IS_DISCRETE_WFQ_MODE(m)                SOC_TMC_SCH_IS_DISCRETE_WFQ_MODE(m)
#define ARAD_SCH_IS_INDEPENDENT_WFQ_MODE(m)             SOC_TMC_SCH_IS_INDEPENDENT_WFQ_MODE(m)
#define ARAD_SCH_IS_WFQ_CLASS_VAL(class_val)            SOC_TMC_SCH_IS_WFQ_CLASS_VAL(class_val)






#define ARAD_SCH_GROUP_LAST SOC_TMC_SCH_GROUP_LAST

#define ARAD_SCH_PORT_LOWEST_HP_HR_CLASS_NONE             SOC_TMC_SCH_PORT_LOWEST_HP_HR_CLASS_NONE
#define ARAD_SCH_PORT_LOWEST_HP_HR_CLASS_EF1              SOC_TMC_SCH_PORT_LOWEST_HP_HR_CLASS_EF1
#define ARAD_SCH_PORT_LOWEST_HP_HR_CLASS_EF2              SOC_TMC_SCH_PORT_LOWEST_HP_HR_CLASS_EF2
#define ARAD_SCH_PORT_LOWEST_HP_HR_CLASS_EF3              SOC_TMC_SCH_PORT_LOWEST_HP_HR_CLASS_EF3
#define ARAD_SCH_PORT_LOWEST_HP_HR_SINGLE_CLASS_AF1_WFQ   SOC_TMC_SCH_PORT_LOWEST_HP_HR_SINGLE_CLASS_AF1_WFQ
#define ARAD_SCH_PORT_LOWEST_HP_HR_DUAL_OR_ENHANCED       SOC_TMC_SCH_PORT_LOWEST_HP_HR_DUAL_OR_ENHANCED
typedef SOC_TMC_SCH_PORT_LOWEST_HP_HR_CLASS                    ARAD_SCH_PORT_LOWEST_HP_HR_CLASS;

#define ARAD_SCH_CL_MODE_NONE                             SOC_TMC_SCH_CL_MODE_NONE
#define ARAD_SCH_CL_MODE_1                                SOC_TMC_SCH_CL_MODE_1
#define ARAD_SCH_CL_MODE_2                                SOC_TMC_SCH_CL_MODE_2
#define ARAD_SCH_CL_MODE_3                                SOC_TMC_SCH_CL_MODE_3
#define ARAD_SCH_CL_MODE_4                                SOC_TMC_SCH_CL_MODE_4
#define ARAD_SCH_CL_MODE_5                                SOC_TMC_SCH_CL_MODE_5
typedef SOC_TMC_SCH_CL_CLASS_MODE                              ARAD_SCH_CL_CLASS_MODE;

#define ARAD_SCH_CL_WEIGHTS_MODE_DISCRETE_PER_FLOW        SOC_TMC_SCH_CL_WEIGHTS_MODE_DISCRETE_PER_FLOW
#define ARAD_SCH_CL_WEIGHTS_MODE_INDEPENDENT_PER_FLOW     SOC_TMC_SCH_CL_WEIGHTS_MODE_INDEPENDENT_PER_FLOW
#define ARAD_SCH_CL_WEIGHTS_MODE_DISCRETE_PER_CLASS       SOC_TMC_SCH_CL_WEIGHTS_MODE_DISCRETE_PER_CLASS
typedef SOC_TMC_SCH_CL_CLASS_WEIGHTS_MODE                      ARAD_SCH_CL_CLASS_WEIGHTS_MODE;

#define ARAD_CL_ENHANCED_MODE_DISABLED                    SOC_TMC_CL_ENHANCED_MODE_DISABLED
#define ARAD_CL_ENHANCED_MODE_ENABLED_HP                  SOC_TMC_CL_ENHANCED_MODE_ENABLED_HP
#define ARAD_CL_ENHANCED_MODE_ENABLED_LP                  SOC_TMC_CL_ENHANCED_MODE_ENABLED_LP
typedef SOC_TMC_SCH_CL_ENHANCED_MODE                           ARAD_SCH_CL_ENHANCED_MODE;

#define ARAD_SCH_GROUP_A                                  SOC_TMC_SCH_GROUP_A
#define ARAD_SCH_GROUP_B                                  SOC_TMC_SCH_GROUP_B
#define ARAD_SCH_GROUP_C                                  SOC_TMC_SCH_GROUP_C
#define ARAD_SCH_GROUP_AUTO                               SOC_TMC_SCH_GROUP_AUTO
#define ARAD_SCH_GROUP_NONE                               SOC_TMC_SCH_GROUP_NONE
typedef SOC_TMC_SCH_GROUP                                      ARAD_SCH_GROUP;

#define ARAD_SCH_SE_TYPE_NONE                             SOC_TMC_SCH_SE_TYPE_NONE
#define ARAD_SCH_SE_TYPE_HR                               SOC_TMC_SCH_SE_TYPE_HR
#define ARAD_SCH_SE_TYPE_CL                               SOC_TMC_SCH_SE_TYPE_CL
#define ARAD_SCH_SE_TYPE_FQ                               SOC_TMC_SCH_SE_TYPE_FQ
typedef SOC_TMC_SCH_SE_TYPE                                    ARAD_SCH_SE_TYPE;

#define ARAD_SCH_SE_STATE_DISABLE                         SOC_TMC_SCH_SE_STATE_DISABLE
#define ARAD_SCH_SE_STATE_ENABLE                          SOC_TMC_SCH_SE_STATE_ENABLE
typedef SOC_TMC_SCH_SE_STATE                                   ARAD_SCH_SE_STATE;

#define ARAD_SCH_HR_MODE_NONE                             SOC_TMC_SCH_HR_MODE_NONE
#define ARAD_SCH_HR_MODE_SINGLE_WFQ                       SOC_TMC_SCH_HR_MODE_SINGLE_WFQ
#define ARAD_SCH_HR_MODE_DUAL_WFQ                         SOC_TMC_SCH_HR_MODE_DUAL_WFQ
#define ARAD_SCH_HR_MODE_ENHANCED_PRIO_WFQ                SOC_TMC_SCH_HR_MODE_ENHANCED_PRIO_WFQ
typedef SOC_TMC_SCH_SE_HR_MODE                                 ARAD_SCH_SE_HR_MODE;

#define ARAD_SCH_FLOW_HR_CLASS_NONE                       SOC_TMC_SCH_FLOW_HR_CLASS_NONE
#define ARAD_SCH_FLOW_HR_CLASS_EF1                        SOC_TMC_SCH_FLOW_HR_CLASS_EF1
#define ARAD_SCH_FLOW_HR_CLASS_EF2                        SOC_TMC_SCH_FLOW_HR_CLASS_EF2
#define ARAD_SCH_FLOW_HR_CLASS_EF3                        SOC_TMC_SCH_FLOW_HR_CLASS_EF3
#define ARAD_SCH_FLOW_HR_SINGLE_CLASS_AF1_WFQ             SOC_TMC_SCH_FLOW_HR_SINGLE_CLASS_AF1_WFQ
#define ARAD_SCH_FLOW_HR_SINGLE_CLASS_BE1                 SOC_TMC_SCH_FLOW_HR_SINGLE_CLASS_BE1
#define ARAD_SCH_FLOW_HR_DUAL_CLASS_AF1_WFQ               SOC_TMC_SCH_FLOW_HR_DUAL_CLASS_AF1_WFQ
#define ARAD_SCH_FLOW_HR_DUAL_CLASS_BE1_WFQ               SOC_TMC_SCH_FLOW_HR_DUAL_CLASS_BE1_WFQ
#define ARAD_SCH_FLOW_HR_DUAL_CLASS_BE2                   SOC_TMC_SCH_FLOW_HR_DUAL_CLASS_BE2
#define ARAD_SCH_FLOW_HR_ENHANCED_CLASS_AF1               SOC_TMC_SCH_FLOW_HR_ENHANCED_CLASS_AF1
#define ARAD_SCH_FLOW_HR_ENHANCED_CLASS_AF2               SOC_TMC_SCH_FLOW_HR_ENHANCED_CLASS_AF2
#define ARAD_SCH_FLOW_HR_ENHANCED_CLASS_AF3               SOC_TMC_SCH_FLOW_HR_ENHANCED_CLASS_AF3
#define ARAD_SCH_FLOW_HR_ENHANCED_CLASS_AF4               SOC_TMC_SCH_FLOW_HR_ENHANCED_CLASS_AF4
#define ARAD_SCH_FLOW_HR_ENHANCED_CLASS_AF5               SOC_TMC_SCH_FLOW_HR_ENHANCED_CLASS_AF5
#define ARAD_SCH_FLOW_HR_ENHANCED_CLASS_AF6               SOC_TMC_SCH_FLOW_HR_ENHANCED_CLASS_AF6
#define ARAD_SCH_FLOW_HR_ENHANCED_CLASS_BE1_WFQ           SOC_TMC_SCH_FLOW_HR_ENHANCED_CLASS_BE1_WFQ
#define ARAD_SCH_FLOW_HR_ENHANCED_CLASS_BE2               SOC_TMC_SCH_FLOW_HR_ENHANCED_CLASS_BE2
typedef SOC_TMC_SCH_SUB_FLOW_HR_CLASS                          ARAD_SCH_SUB_FLOW_HR_CLASS;

#define ARAD_SCH_SUB_FLOW_CL_CLASS_NONE                   SOC_TMC_SCH_SUB_FLOW_CL_CLASS_NONE
#define ARAD_SCH_SUB_FLOW_CL_CLASS_SP1                    SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP1
#define ARAD_SCH_SUB_FLOW_CL_CLASS_SP2                    SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP2
#define ARAD_SCH_SUB_FLOW_CL_CLASS_SP3                    SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP3
#define ARAD_SCH_SUB_FLOW_CL_CLASS_SP4                    SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP4
#define ARAD_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ                SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ
#define ARAD_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ1               SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ1
#define ARAD_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ2               SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ2
#define ARAD_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ3               SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ3
#define ARAD_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ4               SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP1_WFQ4
#define ARAD_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ                SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ
#define ARAD_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ1               SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ1
#define ARAD_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ2               SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ2
#define ARAD_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ3               SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP2_WFQ3
#define ARAD_SCH_SUB_FLOW_CL_CLASS_SP3_WFQ1               SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP3_WFQ1
#define ARAD_SCH_SUB_FLOW_CL_CLASS_SP3_WFQ2               SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP3_WFQ2
#define ARAD_SCH_SUB_FLOW_CL_CLASS_SP_0_ENHANCED          SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP_0_ENHANCED
#define ARAD_SCH_SUB_FLOW_CL_CLASS_SP_5_ENHANCED          SOC_TMC_SCH_SUB_FLOW_CL_CLASS_SP_5_ENHANCED
typedef SOC_TMC_SCH_SUB_FLOW_CL_CLASS                          ARAD_SCH_SUB_FLOW_CL_CLASS;

#define ARAD_SCH_SLOW_RATE_NDX_1                          SOC_TMC_SCH_SLOW_RATE_NDX_1
#define ARAD_SCH_SLOW_RATE_NDX_2                          SOC_TMC_SCH_SLOW_RATE_NDX_2
#define ARAD_SCH_NOF_SLOW_RATE_NDXS                       SOC_TMC_SCH_NOF_SLOW_RATE_NDXS
typedef SOC_TMC_SCH_SLOW_RATE_NDX                              ARAD_SCH_SLOW_RATE_NDX;

#define ARAD_FLOW_NONE                                    SOC_TMC_FLOW_NONE
#define ARAD_FLOW_SIMPLE                                  SOC_TMC_FLOW_SIMPLE
#define ARAD_FLOW_AGGREGATE                               SOC_TMC_FLOW_AGGREGATE
typedef SOC_TMC_SCH_FLOW_TYPE                                  ARAD_SCH_FLOW_TYPE;

#define ARAD_SCH_FLOW_OFF                                 SOC_TMC_SCH_FLOW_OFF
#define ARAD_SCH_FLOW_ON                                  SOC_TMC_SCH_FLOW_ON
typedef SOC_TMC_SCH_FLOW_STATUS                                ARAD_SCH_FLOW_STATUS;

typedef SOC_TMC_SCH_DEVICE_RATE_ENTRY                          ARAD_SCH_DEVICE_RATE_ENTRY;
typedef SOC_TMC_SCH_DEVICE_RATE_TABLE                          ARAD_SCH_DEVICE_RATE_TABLE;
typedef SOC_TMC_SCH_IF_WEIGHT_ENTRY                            ARAD_SCH_IF_WEIGHT_ENTRY;
typedef SOC_TMC_SCH_IF_WEIGHTS                                 ARAD_SCH_IF_WEIGHTS;
typedef SOC_TMC_SCH_PORT_HP_CLASS_INFO                         ARAD_SCH_PORT_HP_CLASS_INFO;
typedef SOC_TMC_SCH_PORT_INFO                                  ARAD_SCH_PORT_INFO;
typedef SOC_TMC_EGRESS_PORT_QOS                                ARAD_EGRESS_PORT_QOS;
typedef SOC_TMC_EGRESS_PORT_QOS_TABLE                          ARAD_EGRESS_PORT_QOS_TABLE;
typedef SOC_TMC_SCH_SE_HR                                      ARAD_SCH_SE_HR;
typedef SOC_TMC_SCH_SE_CL                                      ARAD_SCH_SE_CL;
typedef SOC_TMC_SCH_SE_FQ                                      ARAD_SCH_SE_FQ;
typedef SOC_TMC_SCH_SE_CL_CLASS_INFO                           ARAD_SCH_SE_CL_CLASS_INFO;
typedef SOC_TMC_SCH_SE_CL_CLASS_TABLE                          ARAD_SCH_SE_CL_CLASS_TABLE;
typedef SOC_TMC_SCH_SE_PER_TYPE_INFO                           ARAD_SCH_SE_PER_TYPE_INFO;
typedef SOC_TMC_SCH_SE_INFO                                    ARAD_SCH_SE_INFO;
typedef SOC_TMC_SCH_SUB_FLOW_SHAPER                            ARAD_SCH_SUB_FLOW_SHAPER;
typedef SOC_TMC_SCH_SUB_FLOW_HR                                ARAD_SCH_SUB_FLOW_HR;
typedef SOC_TMC_SCH_SUB_FLOW_CL                                ARAD_SCH_SUB_FLOW_CL;
typedef SOC_TMC_SCH_SUB_FLOW_FQ                                ARAD_SCH_SUB_FLOW_FQ;
typedef SOC_TMC_SCH_SUB_FLOW_SE_INFO                           ARAD_SCH_SUB_FLOW_SE_INFO;
typedef SOC_TMC_SCH_SUB_FLOW_CREDIT_SOURCE                     ARAD_SCH_SUB_FLOW_CREDIT_SOURCE;
typedef SOC_TMC_SCH_SUBFLOW                                    ARAD_SCH_SUBFLOW;
typedef SOC_TMC_SCH_FLOW                                       ARAD_SCH_FLOW;
typedef SOC_TMC_SCH_GLOBAL_PER1K_INFO                          ARAD_SCH_GLOBAL_PER1K_INFO;
typedef SOC_TMC_SCH_QUARTET_MAPPING_INFO                       ARAD_SCH_QUARTET_MAPPING_INFO;
typedef SOC_TMC_SCH_SLOW_RATE                                  ARAD_SCH_SLOW_RATE;
typedef SOC_TMC_SCH_TCG_WEIGHT                                 ARAD_SCH_TCG_WEIGHT;
typedef SOC_TMC_SCH_FLOW_AND_UP_INFO                           ARAD_SCH_FLOW_AND_UP_INFO;
typedef SOC_TMC_SCH_FLOW_AND_UP_PORT_INFO                      ARAD_SCH_FLOW_AND_UP_PORT_INFO;
typedef SOC_TMC_SCH_FLOW_AND_UP_SE_INFO                        ARAD_SCH_FLOW_AND_UP_SE_INFO;
typedef SOC_TMC_SCH_FLOW_AND_UP_UNION_INFO                     ARAD_SCH_FLOW_AND_UP_UNION_INFO;

typedef SOC_TMC_SCH_CL_CLASS_TYPE_ID ARAD_SCH_CL_CLASS_TYPE_ID;


typedef SOC_TMC_SCH_SE_ID ARAD_SCH_SE_ID;


typedef SOC_TMC_SCH_PORT_ID ARAD_SCH_PORT_ID;


typedef SOC_TMC_SCH_FLOW_ID ARAD_SCH_FLOW_ID;

#define ARAD_SCH_FLOW_IPF_CONFIG_MODE_INVERSE       SOC_TMC_SCH_FLOW_IPF_CONFIG_MODE_INVERSE
#define ARAD_SCH_FLOW_IPF_CONFIG_MODE_PROPORTIONAL  SOC_TMC_SCH_FLOW_IPF_CONFIG_MODE_PROPORTIONAL
#define ARAD_SCH_NOF_FLOW_IPF_CONFIG_MODES          SOC_TMC_SCH_NOF_FLOW_IPF_CONFIG_MODES
typedef SOC_TMC_SCH_FLOW_IPF_CONFIG_MODE   ARAD_SCH_FLOW_IPF_CONFIG_MODE;












 uint32
  arad_sch_flow_ipf_config_mode_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_SCH_FLOW_IPF_CONFIG_MODE mode
  );


uint32
  arad_sch_flow_ipf_config_mode_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT ARAD_SCH_FLOW_IPF_CONFIG_MODE *mode
  );



uint32
  arad_sch_port_tcg_weight_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_IN  ARAD_TCG_NDX        tcg_ndx,
    SOC_SAND_IN  ARAD_SCH_TCG_WEIGHT *tcg_weight
  );

uint32
  arad_sch_port_tcg_weight_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  int                  core,
    SOC_SAND_IN  uint32               tm_port,
    SOC_SAND_IN  ARAD_TCG_NDX         tcg_ndx,
    SOC_SAND_OUT ARAD_SCH_TCG_WEIGHT  *tcg_weight
  );


uint32
  arad_sch_device_if_weight_idx_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  soc_port_t          port,
    SOC_SAND_IN  uint32              weight_index
  );


uint32
  arad_sch_device_if_weight_idx_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  soc_port_t          port,
    SOC_SAND_OUT uint32              *weight_index
  );


uint32
  arad_sch_if_weight_conf_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_SCH_IF_WEIGHTS      *if_weights
  );


uint32
  arad_sch_if_weight_conf_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_SCH_IF_WEIGHTS      *if_weights
  );


uint32
  arad_sch_class_type_params_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_SCH_CL_CLASS_TYPE_ID cl_type_ndx,
    SOC_SAND_IN  ARAD_SCH_SE_CL_CLASS_INFO *class_type,
    SOC_SAND_OUT ARAD_SCH_SE_CL_CLASS_INFO *exact_class_type
  );


uint32
  arad_sch_class_type_params_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_SCH_CL_CLASS_TYPE_ID cl_type_ndx,
    SOC_SAND_OUT ARAD_SCH_SE_CL_CLASS_INFO *class_type
  );


uint32
  arad_sch_class_type_params_table_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_SCH_SE_CL_CLASS_TABLE *sct,
    SOC_SAND_OUT ARAD_SCH_SE_CL_CLASS_TABLE *exact_sct
  );


uint32
  arad_sch_class_type_params_table_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_OUT ARAD_SCH_SE_CL_CLASS_TABLE *sct
  );



ARAD_SCH_SE_ID
  arad_sch_flow2se_id(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  ARAD_SCH_FLOW_ID        flow_id
  );


ARAD_SCH_FLOW_ID
  arad_sch_se2flow_id(
    SOC_SAND_IN  ARAD_SCH_SE_ID          se_id
  );


ARAD_SCH_SE_TYPE
  arad_sch_se_get_type_by_id(
    ARAD_SCH_SE_ID                   se_id
  );


uint32
  arad_sch_se2port_tc_id(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  int               core,
    SOC_SAND_IN  ARAD_SCH_SE_ID          se_id,
    SOC_SAND_OUT ARAD_SCH_PORT_ID        *port_id,
    SOC_SAND_OUT uint32               *tc 
  );


uint32
  arad_sch_port_tc2se_id(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  int               core,
    SOC_SAND_IN  uint32            tm_port,
    SOC_SAND_IN  uint32            tc,
    SOC_SAND_OUT ARAD_SCH_SE_ID    *se_id
  );


uint32
  arad_sch_port_sched_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32             tm_port,
    SOC_SAND_IN  ARAD_SCH_PORT_INFO      *port_info
  );


uint32
  arad_sch_port_sched_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_OUT ARAD_SCH_PORT_INFO  *port_info
  );


uint32
  arad_sch_slow_max_rates_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 slow_rate_type,
    SOC_SAND_IN  int                 slow_rate_val
  );


uint32
  arad_sch_slow_max_rates_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 slow_rate_type,
    SOC_SAND_OUT int      *slow_rate_val
  );


uint32
  arad_sch_aggregate_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  ARAD_SCH_SE_ID          se_ndx,
    SOC_SAND_IN  ARAD_SCH_SE_INFO        *se,
    SOC_SAND_IN  ARAD_SCH_FLOW           *flow,
    SOC_SAND_OUT ARAD_SCH_FLOW           *exact_flow
  );


uint32
  arad_sch_aggregate_group_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  ARAD_SCH_SE_ID          se_ndx,
    SOC_SAND_IN  ARAD_SCH_SE_INFO        *se,
    SOC_SAND_IN  ARAD_SCH_FLOW           *flow
  );


uint32
  arad_sch_aggregate_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_SCH_SE_ID          se_ndx,
    SOC_SAND_OUT ARAD_SCH_SE_INFO        *se,
    SOC_SAND_OUT ARAD_SCH_FLOW           *flow
  );


uint32
  arad_sch_flow_delete(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_SCH_FLOW_ID         flow_ndx
  );


uint32
  arad_sch_flow_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_SCH_FLOW_ID        flow_ndx,
    SOC_SAND_IN  ARAD_SCH_FLOW           *flow,
    SOC_SAND_OUT ARAD_SCH_FLOW           *exact_flow
  );


uint32
  arad_sch_flow_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_SCH_FLOW_ID        flow_ndx,
    SOC_SAND_OUT ARAD_SCH_FLOW           *flow
  );


uint32
  arad_sch_flow_status_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_SCH_FLOW_ID        flow_ndx,
    SOC_SAND_IN  ARAD_SCH_FLOW_STATUS    state
  );



uint32
  arad_sch_per1k_info_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                 k_flow_ndx,
    SOC_SAND_IN  ARAD_SCH_GLOBAL_PER1K_INFO *per1k_info
  );


uint32
  arad_sch_per1k_info_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                 k_flow_ndx,
    SOC_SAND_OUT ARAD_SCH_GLOBAL_PER1K_INFO *per1k_info
  );


uint32
  arad_sch_flow_to_queue_mapping_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                 quartet_ndx,
    SOC_SAND_IN  uint32                 nof_quartets_to_map,
    SOC_SAND_IN  ARAD_SCH_QUARTET_MAPPING_INFO *quartet_flow_info
  );


uint32
  arad_sch_flow_to_queue_mapping_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                 quartet_ndx,
    SOC_SAND_OUT ARAD_SCH_QUARTET_MAPPING_INFO *quartet_flow_info
  );


uint32
  arad_sch_flow_id_verify_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_SCH_FLOW_ID         flow_id
  );

uint8
  arad_sch_is_flow_id_valid(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  ARAD_SCH_FLOW_ID       flow_id
  );


uint32
  arad_sch_se_id_verify_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  ARAD_SCH_SE_ID        se_id
  );

uint8
  arad_sch_is_se_id_valid(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  ARAD_SCH_SE_ID        se_id
  );

uint32
  arad_sch_port_id_verify_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  ARAD_SCH_PORT_ID      port_id
  );

uint8
  arad_sch_is_port_id_valid(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  ARAD_SCH_PORT_ID        port_id
  );


uint32
  arad_sch_k_flow_id_verify_unsafe(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  uint32        k_flow_id
  );

uint32
  arad_sch_quartet_id_verify_unsafe(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  uint32        quartet_id
  );



void
  arad_ARAD_SCH_DEVICE_RATE_ENTRY_clear(
    SOC_SAND_OUT ARAD_SCH_DEVICE_RATE_ENTRY *info
  );

void
  arad_ARAD_SCH_DEVICE_RATE_TABLE_clear(
    SOC_SAND_OUT ARAD_SCH_DEVICE_RATE_TABLE *info
  );

void
  arad_ARAD_SCH_IF_WEIGHT_ENTRY_clear(
    SOC_SAND_OUT ARAD_SCH_IF_WEIGHT_ENTRY *info
  );

void
  arad_ARAD_SCH_IF_WEIGHTS_clear(
    SOC_SAND_OUT ARAD_SCH_IF_WEIGHTS *info
  );

void
  arad_ARAD_SCH_PORT_HP_CLASS_INFO_clear(
    SOC_SAND_OUT ARAD_SCH_PORT_HP_CLASS_INFO *info
  );

void
  arad_ARAD_SCH_PORT_INFO_clear(
    SOC_SAND_OUT ARAD_SCH_PORT_INFO *info
  );

void
  arad_ARAD_SCH_SE_CL_CLASS_INFO_clear(
    SOC_SAND_OUT ARAD_SCH_SE_CL_CLASS_INFO *info
  );

void
  arad_ARAD_SCH_SE_CL_CLASS_TABLE_clear(
    SOC_SAND_OUT ARAD_SCH_SE_CL_CLASS_TABLE *info
  );

void
  arad_ARAD_SCH_SE_PER_TYPE_INFO_clear(
    SOC_SAND_OUT ARAD_SCH_SE_PER_TYPE_INFO *info
  );

void
  arad_ARAD_SCH_SE_INFO_clear(
    SOC_SAND_OUT ARAD_SCH_SE_INFO *info
  );

void
  arad_ARAD_SCH_SUB_FLOW_SHAPER_clear(
    int unit,
    SOC_SAND_OUT ARAD_SCH_SUB_FLOW_SHAPER *info
  );

void
  arad_ARAD_SCH_SUB_FLOW_HR_clear(
    SOC_SAND_OUT ARAD_SCH_SUB_FLOW_HR *info
  );

void
  arad_ARAD_SCH_SUB_FLOW_CL_clear(
    SOC_SAND_OUT ARAD_SCH_SUB_FLOW_CL *info
  );

void
  arad_ARAD_SCH_SUB_FLOW_SE_INFO_clear(
    SOC_SAND_OUT ARAD_SCH_SUB_FLOW_SE_INFO *info
  );

void
  arad_ARAD_SCH_SUB_FLOW_CREDIT_SOURCE_clear(
    SOC_SAND_OUT ARAD_SCH_SUB_FLOW_CREDIT_SOURCE *info
  );

void
  arad_ARAD_SCH_SUBFLOW_clear(
    int unit,
    SOC_SAND_OUT ARAD_SCH_SUBFLOW *info
  );

void
  arad_ARAD_SCH_FLOW_clear(
    int unit,
    SOC_SAND_OUT ARAD_SCH_FLOW *info
  );

void
  arad_ARAD_SCH_GLOBAL_PER1K_INFO_clear(
    SOC_SAND_OUT ARAD_SCH_GLOBAL_PER1K_INFO *info
  );

void
  arad_ARAD_SCH_QUARTET_MAPPING_INFO_clear(
    SOC_SAND_OUT ARAD_SCH_QUARTET_MAPPING_INFO *info
  );

void
  arad_ARAD_SCH_SLOW_RATE_clear(
    SOC_SAND_OUT ARAD_SCH_SLOW_RATE *info
  );

void
  arad_ARAD_SCH_FLOW_AND_UP_INFO_clear(
    int unit,
    SOC_SAND_OUT ARAD_SCH_FLOW_AND_UP_INFO *info,
    SOC_SAND_IN uint32                         is_full 
  );

void
  arad_ARAD_SCH_FLOW_AND_UP_PORT_INFO_clear(
    SOC_SAND_OUT ARAD_SCH_FLOW_AND_UP_PORT_INFO *info
  );

void
  arad_ARAD_SCH_FLOW_AND_UP_SE_INFO_clear(
    int unit,
    SOC_SAND_OUT ARAD_SCH_FLOW_AND_UP_SE_INFO *info
  );

#if ARAD_DEBUG_IS_LVL1


const char*
  arad_ARAD_SCH_PORT_LOWEST_HP_HR_CLASS_to_string(
    SOC_SAND_IN ARAD_SCH_PORT_LOWEST_HP_HR_CLASS enum_val
  );

const char*
  arad_ARAD_SCH_CL_CLASS_MODE_to_string(
    SOC_SAND_IN ARAD_SCH_CL_CLASS_MODE enum_val
  );

const char*
  arad_ARAD_SCH_CL_CLASS_WEIGHTS_MODE_to_string(
    SOC_SAND_IN ARAD_SCH_CL_CLASS_WEIGHTS_MODE enum_val
  );


const char*
  arad_ARAD_SCH_CL_ENHANCED_MODE_to_string(
    SOC_SAND_IN ARAD_SCH_CL_ENHANCED_MODE enum_val
  );



const char*
  arad_ARAD_SCH_GROUP_to_string(
    SOC_SAND_IN ARAD_SCH_GROUP enum_val
  );



const char*
  arad_ARAD_SCH_SE_TYPE_to_string(
    SOC_SAND_IN ARAD_SCH_SE_TYPE enum_val
  );



const char*
  arad_ARAD_SCH_SE_STATE_to_string(
    SOC_SAND_IN ARAD_SCH_SE_STATE enum_val
  );



const char*
  arad_ARAD_SCH_SE_HR_MODE_to_string(
    SOC_SAND_IN ARAD_SCH_SE_HR_MODE enum_val
  );



const char*
  arad_ARAD_SCH_SUB_FLOW_HR_CLASS_to_string(
    SOC_SAND_IN ARAD_SCH_SUB_FLOW_HR_CLASS enum_val
  );



const char*
  arad_ARAD_SCH_SUB_FLOW_CL_CLASS_to_string(
    SOC_SAND_IN ARAD_SCH_SUB_FLOW_CL_CLASS enum_val
  );



const char*
  arad_ARAD_SCH_SLOW_RATE_NDX_to_string(
    SOC_SAND_IN ARAD_SCH_SLOW_RATE_NDX enum_val
  );



const char*
  arad_ARAD_SCH_FLOW_TYPE_to_string(
    SOC_SAND_IN ARAD_SCH_FLOW_TYPE enum_val
  );



const char*
  arad_ARAD_SCH_FLOW_STATUS_to_string(
    SOC_SAND_IN ARAD_SCH_FLOW_STATUS enum_val
  );



void
  arad_ARAD_SCH_DEVICE_RATE_ENTRY_print(
    SOC_SAND_IN ARAD_SCH_DEVICE_RATE_ENTRY *info
  );



void
  arad_ARAD_SCH_DEVICE_RATE_TABLE_print(
    SOC_SAND_IN uint32 unit,
    SOC_SAND_IN ARAD_SCH_DEVICE_RATE_TABLE *info
  );


void
  arad_ARAD_SCH_IF_WEIGHT_ENTRY_print(
    SOC_SAND_IN ARAD_SCH_IF_WEIGHT_ENTRY *info
  );



void
  arad_ARAD_SCH_IF_WEIGHTS_print(
    SOC_SAND_IN ARAD_SCH_IF_WEIGHTS *info
  );



void
  arad_ARAD_SCH_PORT_HP_CLASS_INFO_print(
    SOC_SAND_IN ARAD_SCH_PORT_HP_CLASS_INFO *info
  );



void
  arad_ARAD_SCH_PORT_INFO_print(
    SOC_SAND_IN ARAD_SCH_PORT_INFO *info,
    SOC_SAND_IN uint32           port_id
  );


void
  arad_ARAD_SCH_SE_HR_print(
    SOC_SAND_IN ARAD_SCH_SE_HR *info
  );



void
  arad_ARAD_SCH_SE_CL_print(
    SOC_SAND_IN ARAD_SCH_SE_CL *info
  );



void
  arad_ARAD_SCH_SE_FQ_print(
    SOC_SAND_IN ARAD_SCH_SE_FQ *info
  );



void
  arad_ARAD_SCH_SE_CL_CLASS_INFO_print(
    SOC_SAND_IN ARAD_SCH_SE_CL_CLASS_INFO *info
  );



void
  arad_ARAD_SCH_SE_CL_CLASS_TABLE_print(
    SOC_SAND_IN ARAD_SCH_SE_CL_CLASS_TABLE *info
  );

void
  arad_ARAD_SCH_SE_PER_TYPE_INFO_print(
    SOC_SAND_IN ARAD_SCH_SE_PER_TYPE_INFO *info,
    SOC_SAND_IN ARAD_SCH_SE_TYPE type
  );

void
  arad_ARAD_SCH_SE_INFO_print(
    SOC_SAND_IN ARAD_SCH_SE_INFO *info
  );



void
  arad_ARAD_SCH_SUB_FLOW_SHAPER_print(
    SOC_SAND_IN ARAD_SCH_SUB_FLOW_SHAPER *info
  );



void
  arad_ARAD_SCH_SUB_FLOW_HR_print(
    SOC_SAND_IN ARAD_SCH_SUB_FLOW_HR *info
  );



void
  arad_ARAD_SCH_SUB_FLOW_CL_print(
    SOC_SAND_IN ARAD_SCH_SUB_FLOW_CL *info
  );

void
  arad_ARAD_SCH_SUB_FLOW_FQ_print(
    SOC_SAND_IN ARAD_SCH_SUB_FLOW_FQ *info
  );

void
  arad_ARAD_SCH_SUB_FLOW_SE_INFO_print(
    SOC_SAND_IN ARAD_SCH_SUB_FLOW_SE_INFO *info,
    SOC_SAND_IN ARAD_SCH_SE_TYPE se_type
  );



void
  arad_ARAD_SCH_SUB_FLOW_CREDIT_SOURCE_print(
    SOC_SAND_IN ARAD_SCH_SUB_FLOW_CREDIT_SOURCE *info
  );



void
  arad_ARAD_SCH_SUBFLOW_print(
    SOC_SAND_IN ARAD_SCH_SUBFLOW *info,
    SOC_SAND_IN uint8 is_table_flow,
    SOC_SAND_IN uint32 subflow_id
  );


void
  arad_ARAD_SCH_FLOW_print(
    SOC_SAND_IN ARAD_SCH_FLOW *info,
    SOC_SAND_IN uint8 is_table
  );



void
  arad_ARAD_SCH_GLOBAL_PER1K_INFO_print(
    SOC_SAND_IN ARAD_SCH_GLOBAL_PER1K_INFO *info
  );



void
  arad_ARAD_SCH_QUARTET_MAPPING_INFO_print(
    SOC_SAND_IN ARAD_SCH_QUARTET_MAPPING_INFO *info
  );

void
  arad_ARAD_SCH_SLOW_RATE_print(
    SOC_SAND_IN ARAD_SCH_SLOW_RATE *info
  );


uint32
  arad_flow_and_up_info_get(
    SOC_SAND_IN                       int               unit,
    SOC_SAND_IN                       int               core,
    SOC_SAND_IN                       uint32               flow_id,
    SOC_SAND_IN                       uint32                          reterive_status,
    SOC_SAND_INOUT                    SOC_TMC_SCH_FLOW_AND_UP_INFO    *flow_and_up_info
  );

#endif 




#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
