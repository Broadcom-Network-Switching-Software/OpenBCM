#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_METERING
#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/mem.h>




#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>


#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_metering.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>

#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/SAND/Utils/sand_multi_set.h>
#include <soc/dpp/SAND/Utils/sand_u64.h>
#include <soc/dpp/drv.h>

#include <soc/dpp/ARAD/arad_ingress_traffic_mgmt.h>





#define ARAD_PP_METERING_MTR_GROUP_NDX_MAX                       (1)
#define ARAD_PP_METERING_BW_PROFILE_NDX_MAX                      (0x3ff)
#define ARAD_PP_METERING_ETH_TYPE_NDX_MAX                        (SOC_PPC_NOF_MTR_ETH_TYPES-1)

#define ARAD_PP_METERING_GLBL_PROFILE_IDX_MIN                    (0)
#define ARAD_PP_METERING_GLBL_PROFILE_IDX_MAX                    (7)

#define ARAD_PP_METERING_GROUP_MAX                               (ARAD_PP_METERING_MTR_GROUP_NDX_MAX)
#define ARAD_PP_METERING_ID_MAX                                  (32767)
#define ARAD_PP_METERING_COLOR_MODE_MAX                          (SOC_PPC_NOF_MTR_COLOR_MODES-1)
#define ARAD_PP_METER_TC_SIZE                                    2
#define ARAD_PP_METER_TC_MASK                                    0x3

#define ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_EXP_BS 15
#define ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_BS 63

#define ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_MNT_IR 1 
#define ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_IR 127
#define ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_MNT_EXP 0 
#define ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_EXP 7 
#define ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_REV_EXP 0 

#define ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MULTI_BS 1
#define ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_BS 64
#define ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR 0


#define ARAD_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MULTI_BS 1
#define ARAD_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MNT_INC_BS 0
#define ARAD_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR 0

#define ARAD_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR 125

#define ARAD_PP_MTR_PROFILE_VAL_TO_EXP_MNT_MAX_MNT_BS 255 
#define ARAD_PP_MTR_PROFILE_VAL_TO_EXP_MNT_MAX_EXP_BS 31

#define JER_PP_MTR_PROFILE_VAL_TO_EXP_MNT_MAX_MNT_BS 63 
#define JER_PP_MTR_PROFILE_VAL_TO_EXP_MNT_MAX_EXP_BS 15
#define JER_PP_MTR_PROFILE_REV_EXP_BIAS 2


#define ARAD_PP_POLICER_PROFILE_ADD_VERIFY_IR_MIN 146

#define ARAD_PP_BW_PROFILE_ADD_VERIFY_BS_MAX 4161536    
#define ARAD_PP_BW_PROFILE_ADD_VERIFY_BS_MIN 64


#define ARAD_PP_MTR_PROFILE_MAX_BS 536870911    


#define ARAD_PP_BW_PROFILE_MAX_RATES_FOR_CALC 6    




#define ARAD_PP_MTR_PCD_OPT_NUM                        (SOC_IS_QUX(unit) ? 256 : 64)


#define ARAD_PP_MTR_PCD_RESULT_BEST                     0
#define ARAD_PP_MTR_PCD_RESULT_WORST                    1


#define ARAD_PP_MTR_PCD_BUCKET_UPDATE_CONSTANT          0
#define ARAD_PP_MTR_PCD_BUCKET_UPDATE_TRANSPARENT       1
#define ARAD_PP_MTR_PCD_BUCKET_UPDATE_DEFERRED          2


#define ARAD_PP_MTR_PCD_MCDA_COL_LSB                1
#define ARAD_PP_MTR_PCD_MCDB_COL_LSB                (SOC_IS_QUX(unit) ? 5 : 4)
#define ARAD_PP_MTR_PCD_COL_NOF_BITS                2
#define ARAD_PP_MTR_PCD_MCDA_OTHER_HAS_CREDITS_LSB  0
#define ARAD_PP_MTR_PCD_MCDB_OTHER_HAS_CREDITS_LSB  (SOC_IS_QUX(unit) ? 4 : 3)
#define ARAD_PP_MTR_PCD_OTHER_HAS_CREDITS_NOF_BITS  1
#define ARAD_PP_MTR_PCD_MCDA_MARK_BLACK_LSB         3
#define ARAD_PP_MTR_PCD_MCDB_MARK_BLACK_LSB         7
#define ARAD_PP_MTR_PCD_MARK_BLACK_NOF_BITS         1




#define ARAD_PP_MTR_DP_MAP_MTR_DP_LSB                    0
#define ARAD_PP_MTR_DP_MAP_MTR_DP_NOF_BITS               2
#define ARAD_PP_MTR_DP_MAP_EM_DP_LSB                     2
#define ARAD_PP_MTR_DP_MAP_EM_DP_NOF_BITS                2
#define ARAD_PP_MTR_DP_MAP_IN_DP_LSB                     SOC_IS_JERICHO(unit) ? 4 : 2
#define ARAD_PP_MTR_DP_MAP_IN_DP_NOF_BITS                2
#define ARAD_PP_MTR_DP_MAP_METER_COMMAND_LSB             SOC_IS_JERICHO(unit) ? 6 : 4
#define ARAD_PP_MTR_DP_MAP_METER_COMMAND_NOF_BITS        2


typedef enum {
  ARAD_PP_MTR_PCD_COL_GREEN     = 0,
  ARAD_PP_MTR_PCD_COL_INVALID   = 1, 
  ARAD_PP_MTR_PCD_COL_YELLOW    = 2,
  ARAD_PP_MTR_PCD_COL_RED       = 3,
  ARAD_PP_MTR_PCD_NOF_COLS      = 4
} arad_pp_mtr_pcd_col_t;


#define ARAD_PP_MTR_PCD_COL_INVALID_EQUIV ARAD_PP_MTR_PCD_COL_YELLOW

#define ARAD_PP_MTR_KBITS_TO_BYTES_FACTOR   125
#define ARAD_PP_MTR_PACKETS_TO_BYTES_FACTOR 64

#define ARAD_BUCKET_EXP_MAX_VALUE (15)
#define ARAD_BUCKET_MANTISSA_MAX_VALUE (63)


typedef enum {
    ARAD_PP_MTR_PCD_PARALLEL_METER_A     = 0,
    ARAD_PP_MTR_PCD_PARALLEL_METER_B     = 1,
    ARAD_PP_MTR_PCD_NOF_PARALLEL_METERS  = 2
} arad_pp_mtr_meter_t;












typedef struct 
{
  
  
  uint8 bucket_credits[ARAD_PP_MTR_PCD_NOF_COLS];
  
  uint32 reduce_buck_col;
  
  uint8 addr_col_lsb;
  
  uint8 addr_oth_cred_lsb;
} arad_pp_mtr_pcd_meter_info;





CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_metering[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_METERS_GROUP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_METERS_GROUP_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_METERS_GROUP_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_METERS_GROUP_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_METERS_GROUP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_METERS_GROUP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_METERS_GROUP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_METERS_GROUP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_BW_PROFILE_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_BW_PROFILE_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_BW_PROFILE_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_BW_PROFILE_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_BW_PROFILE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_BW_PROFILE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_BW_PROFILE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_BW_PROFILE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_BW_PROFILE_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_BW_PROFILE_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_BW_PROFILE_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_BW_PROFILE_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_ENABLE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_ENABLE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_ENABLE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_ENABLE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_ENABLE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_ENABLE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_ENABLE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_ENABLE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_PARAMS_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_PARAMS_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_PARAMS_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_PARAMS_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_PARAMS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_PARAMS_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_PARAMS_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_PARAMS_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_METERING_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_METERING_GET_ERRS_PTR),
  

  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_IR_VAL_FROM_REVERSE_EXP_MNT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_BS_VAL_TO_EXP_MNT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_CHK_PROFILE_RATE),

  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_metering[] =
{
  
  {
    ARAD_PP_METERING_MTR_GROUP_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_MTR_GROUP_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'mtr_group_ndx' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_BW_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_BW_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'bw_profile_ndx' is out of range. \n\r "
    "The range is: 0 - 511.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_SUCCESS_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_BW_PROFILE_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_BW_PROFILE_ID_OUT_OF_RANGE_ERR",
    "The parameter 'bw_profile_id' is out of range. \n\r "
    "The range is: 0 - 511.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_RESULT_USE_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_RESULT_USE_OUT_OF_RANGE_ERR",
    "The parameter 'result_use' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_MTR_RES_USES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_METERED_TYPE_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_METERED_TYPE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'metered_type_ndx' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_MTR_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_FIRST_METERED_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_FIRST_METERED_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'first_metered_ndx' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_MTR_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_NOF_METER_INS_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_NOF_METER_INS_OUT_OF_RANGE_ERR",
    "The parameter 'nof_meter_ins' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_MTR_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_ETH_TYPE_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_ETH_TYPE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'eth_type_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_MTR_ETH_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_GLBL_PROFILE_IDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_GLBL_PROFILE_IDX_OUT_OF_RANGE_ERR",
    "The parameter 'glbl_profile_idx' is out of range. \n\r "
    "The range is: 1 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_GROUP_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_GROUP_OUT_OF_RANGE_ERR",
    "The parameter 'group' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_ID_OUT_OF_RANGE_ERR",
    "The parameter 'id' is out of range. \n\r "
    "The range is: 0 - 511.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_IS_HR_ENABLED_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_IS_HR_ENABLED_OUT_OF_RANGE_ERR",
    "The parameter 'is_hr_enabled' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_MAX_PACKET_SIZE_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_MAX_PACKET_SIZE_OUT_OF_RANGE_ERR",
    "The parameter 'max_packet_size' is out of range. \n\r "
    "The range is: 0 - 16*1024-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_CIR_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_CIR_OUT_OF_RANGE_ERR",
    "The parameter 'cir' is out of range. \n\r "
    "The range is: 18 KBPS - 9750 GBPS.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_EIR_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_EIR_OUT_OF_RANGE_ERR",
    "The parameter 'eir' is out of range. \n\r "
    "The range is: 18 KBPS - 9750 GBPS.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_CBS_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_CBS_OUT_OF_RANGE_ERR",
    "The parameter 'cbs' is out of range. \n\r "
    "The range is: 64B - 4MB.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_EBS_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_EBS_OUT_OF_RANGE_ERR",
    "The parameter 'ebs' is out of range. \n\r "
    "The range is: 64B - 4MB.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_COLOR_MODE_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_COLOR_MODE_OUT_OF_RANGE_ERR",
    "The parameter 'color_mode' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_MTR_COLOR_MODES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'type' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_MTR_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  
  {
    ARAD_PP_METERING_CHK_PROFILE_RATE_ERR,
    "ARAD_PP_METERING_CHK_PROFILE_RATE_ERR",
    "The parameters BW_PROFILE are mixed High rate - Low rate. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_VAL_FROM_REVERSE_EXP_MNT_CLK_OF_ERR,
    "ARAD_PP_METERING_VAL_FROM_REVERSE_EXP_MNT_CLK_OF_ERR",
    "The clock core frequency may cause Overflow. \n\r "
    "The range is: 0 - ARAD_PP_METERING_VAL_FROM_REVERSE_EXP_MNT_CLK_OF.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_UPDATE_ALL_COPIES_MAX_RANGE_ERR,
    "ARAD_PP_METERING_UPDATE_ALL_COPIES_MAX_RANGE_ERR",
    "The parameter 'update_all_copies' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_BW_PROFILE_HR_RATE_ERR,
    "ARAD_PP_METERING_BW_PROFILE_HR_RATE_ERR",
    "Profile rate parameters are for High Rate. \n\r "
    "MeterProcc is in Low rate or profile index is for Low rate\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_BW_PROFILE_LR_RATE_ERR,
    "ARAD_PP_METERING_BW_PROFILE_LR_RATE_ERR",
    "Profile rate parameters are for Low Rate. \n\r "
    "MeterProcc is in High rate and Profile index is for High rate\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_MTR_ETH_POLICER_ADD_FAIL_ERR,
    "ARAD_PP_MTR_ETH_POLICER_ADD_FAIL_ERR",
    "Fail to add profile to multiset. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_MTR_ETH_POLICER_PARAMS_SET_VERIFY_OUT_OF_RANGE_ERR,
    "ARAD_PP_MTR_ETH_POLICER_PARAMS_SET_VERIFY_OUT_OF_RANGE_ERR",
    "One of the parameters: color_mode, is_coupling_enabled, is_fairness_enabled, disable_cir, disable_eir is out of range. \n\r "
    "This parameters value should be 0 (FALSE), in eth policer. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_POLICER_INGRESS_COUNT_INVALID_ERR,
    "ARAD_PP_METERING_POLICER_INGRESS_COUNT_INVALID_ERR",
    "The soc property 'policer_ingress_count' is invalid. \n\r "
    "Valid values are: 32/64.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_POLICER_INGRESS_SHARING_MODE_INVALID_ERR,
    "ARAD_PP_METERING_POLICER_INGRESS_SHARING_MODE_INVALID_ERR",
    "The soc property 'policer_ingress_sharing_mode' is invalid. \n\r "
    "Valid values are: 0 (if policer_ingress_count is 64) or 1/2 (if policer_ingress_count is 32).\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_MAX_IR_INVALID_ERR,
    "ARAD_PP_METERING_MAX_IR_INVALID_ERR",
    "max CIR/max EIR cannot be smaller than CIR/EIR. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  
SOC_ERR_DESC_ELEMENT_DEF_LAST
};







STATIC uint32 arad_pp_metering_pcd_init_serial(SOC_SAND_IN int unit);
STATIC uint32 arad_pp_metering_pcd_init_parallel(SOC_SAND_IN int unit);
STATIC void arad_pp_mtr_max_min_vals_for_calc_get(SOC_SAND_IN uint32 *vals, SOC_SAND_IN uint32  nof_vals,SOC_SAND_OUT uint32 *min_val, SOC_SAND_OUT uint32 *max_val);
STATIC uint32 arad_pp_mtr_meter_ins_to_bw_profile_map_init_buckets(SOC_SAND_IN  int unit, SOC_SAND_IN  int core_id, SOC_SAND_IN SOC_PPC_MTR_METER_ID *meter_ins_ndx, SOC_SAND_IN uint32 bw_profile_id);


int
  arad_pp_mtr_translate_kbits_to_packets(
    SOC_SAND_IN int                       unit,
    SOC_SAND_IN int                       kbits
    )
{
    int packets= kbits*ARAD_PP_MTR_KBITS_TO_BYTES_FACTOR;
    packets /= ARAD_PP_MTR_PACKETS_TO_BYTES_FACTOR;
    return packets;
}

int
  arad_pp_mtr_translate_packets_to_kbits(
    SOC_SAND_IN int                       unit,
    SOC_SAND_IN int                       packets
    )
{
    int kbits = packets*ARAD_PP_MTR_PACKETS_TO_BYTES_FACTOR;
    kbits /= ARAD_PP_MTR_KBITS_TO_BYTES_FACTOR;
    return kbits;
}

uint32
  arad_pp_mtr_ir_val_from_reverse_exp_mnt(
    SOC_SAND_IN int                       unit,
    SOC_SAND_IN uint32                       rev_exp,
    SOC_SAND_IN uint32                       mant_exp,
    SOC_SAND_IN uint32                       mnt,
    SOC_SAND_OUT uint32                      *val
  )
{
  uint32
    res = SOC_SAND_OK,
    clk_freq;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_IR_VAL_FROM_REVERSE_EXP_MNT);

  SOC_SAND_CHECK_NULL_INPUT(val);

  clk_freq = arad_chip_ticks_per_sec_get(unit);

  res = soc_sand_compute_complex_to_mnt_exp_reverse_exp(
      mnt,
      mant_exp,
      rev_exp + 2, 
      clk_freq, 
      ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR,
      ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR,
      TRUE,
      val
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_ir_val_from_reverse_exp_mnt()", 0, 0);
}

uint32
  arad_pp_mtr_bs_val_to_exp_mnt(
    SOC_SAND_IN int                       unit,
    SOC_SAND_IN uint32                       val,
    SOC_SAND_OUT uint32                      *exp,
    SOC_SAND_OUT uint32                      *mnt
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_BS_VAL_TO_EXP_MNT);

  SOC_SAND_CHECK_NULL_INPUT(exp);
  SOC_SAND_CHECK_NULL_INPUT(mnt);

  res = soc_sand_break_complex_to_mnt_exp_round_down(
    val,
    ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_BS,
    ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_EXP_BS,
    ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MULTI_BS,
    ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_BS,
    mnt,
    exp
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
   
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_val_to_exp_mnt()", 0, 0);
}


STATIC uint32
  arad_pp_mtr_ir_val_to_max_rev_exp(
    SOC_SAND_IN int                       unit,
    SOC_SAND_IN uint32                       min_rate, 
    SOC_SAND_IN uint32                       max_rate, 
    SOC_SAND_OUT uint32                      *rev_exp
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    clk_freq;
  uint32
    calc_min_rate, 
    calc_max_rate; 
  int 
    tmp_rev_exp;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_IR_VAL_TO_MAX_REV_EXP);

  SOC_SAND_CHECK_NULL_INPUT(rev_exp);

  clk_freq = arad_chip_ticks_per_sec_get(unit);

  for (tmp_rev_exp = ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_REV_EXP; tmp_rev_exp >= ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_REV_EXP; tmp_rev_exp--) {

      
      res = soc_sand_compute_complex_to_mnt_exp_reverse_exp(
          ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_IR,
          ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_EXP,
          tmp_rev_exp + 2, 
          clk_freq,
          ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR,
          ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR, 
          FALSE,
          &calc_max_rate 
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      
      res = soc_sand_compute_complex_to_mnt_exp_reverse_exp(
          ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_MNT_IR,
          ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_MNT_EXP,
          tmp_rev_exp + 2, 
          clk_freq,
          ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR,
          ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR, 
          FALSE,
          &calc_min_rate 
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      if (calc_max_rate >= max_rate) {
          if (calc_min_rate > min_rate) {
              LOG_INFO(BSL_LS_SOC_METERING, 
               (BSL_META_U(unit, 
                 "Metering warning- Large difference between parameters sharing rev_exp, this leads to high error in the smaller value.\n\r Original low value: %d, Configured low value: %d\n\r "), min_rate, calc_min_rate));
          }

          
          break;
      }
  }

  if (tmp_rev_exp < ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_REV_EXP) {
      
      SOC_SAND_SET_ERROR_CODE(SOC_PPD_ERR_OUT_OF_RANGE_ERR, 50, exit);
  }

  *rev_exp = tmp_rev_exp;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_ir_val_to_max_rev_exp()", 0, 0);
}


uint32
  arad_pp_mtr_ir_val_to_max_rev_exp_optimized_for_bucket_rate(
    SOC_SAND_IN int                          unit,
    SOC_SAND_IN uint32                       *rates,
    SOC_SAND_IN uint32                       nof_rates,
    SOC_SAND_OUT uint32                      *rev_exp
  )
{
  uint32 clk_freq; 
  uint32 min_rate;
  uint32 max_rate;
  int tmp_rev_exp;
  uint32 highest_available_rev_exp = 0;
  uint32 rate_idx = 0;
  
  uint32 rate_errors[SOC_PPC_BW_PROFILE_MAX_NUMBER_OF_RATES];
  uint32 selected_rev_exp = -1;
  uint32 res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_PROFILE_RATE_TO_RES_EXP_MNT);
  SOC_SAND_CHECK_NULL_INPUT(rev_exp);

  clk_freq = arad_chip_ticks_per_sec_get(unit);

  
  if (nof_rates <= 0 || SOC_PPC_BW_PROFILE_MAX_NUMBER_OF_RATES < nof_rates) {
    SOC_SAND_SET_ERROR_CODE(1, 0, exit); 
  }
  
  
  arad_pp_mtr_max_min_vals_for_calc_get(rates, nof_rates, &min_rate, &max_rate);

  
  res = arad_pp_mtr_ir_val_to_max_rev_exp(unit, min_rate, max_rate, &highest_available_rev_exp);  
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  selected_rev_exp = highest_available_rev_exp;

  for (tmp_rev_exp = highest_available_rev_exp; tmp_rev_exp >= ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_REV_EXP; tmp_rev_exp--) {
    for (rate_idx = 0; rate_idx < nof_rates; rate_idx++) {
      uint32 mant;
      uint32 exp;
      uint32 cfg_rate;
      uint32 curr_error;

      
      res = soc_sand_break_complex_to_mnt_exp_round_down_2(
          rates[rate_idx],
          ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_IR,
          ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_MNT_IR,
          ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_EXP,
          clk_freq,
          (ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR << (tmp_rev_exp + 2)),
          ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR,
          &mant,
          &exp
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      
      res = soc_sand_compute_complex_to_mnt_exp_reverse_exp(
         mant,
         exp,
         tmp_rev_exp + 2, 
         clk_freq,
         ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR,
         ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR, 
         FALSE,
         &cfg_rate
         );

      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      
      
      curr_error = soc_sand_abs(rates[rate_idx] - cfg_rate);
      
      if (tmp_rev_exp == highest_available_rev_exp) {
        rate_errors[rate_idx] = curr_error;
        continue;
      }

      
      if (curr_error > rate_errors[rate_idx]) {
        break;
      }
      rate_errors[rate_idx] = curr_error;
    }
    
    if (rate_idx < nof_rates) {
      break;
    }

    
    selected_rev_exp = tmp_rev_exp;
  }
  
  *rev_exp = selected_rev_exp;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_ir_val_to_max_rev_exp_optimized_for_bucket_rate()", 0, 0);
}


uint32
  arad_pp_mtr_profile_rate_to_res_exp_mnt(
    SOC_SAND_IN int                       unit,
    SOC_SAND_IN uint32                       val,
    SOC_SAND_OUT uint32                      *rev_exp,
    SOC_SAND_OUT uint32                      *mnt,
    SOC_SAND_OUT uint32                      *exp
  )
{
  uint32
    res = SOC_SAND_OK,
    clk_freq,
    calc_max_rate,
    calc_min_rate;
  int
    tmp_rev_exp;

  uint32
      max_rev_exp,
      min_rev_exp,
      div_const;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_PROFILE_RATE_TO_RES_EXP_MNT);

  SOC_SAND_CHECK_NULL_INPUT(rev_exp);
  SOC_SAND_CHECK_NULL_INPUT(mnt);
  SOC_SAND_CHECK_NULL_INPUT(exp);

  clk_freq = arad_chip_ticks_per_sec_get(unit);

  max_rev_exp = ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_REV_EXP;
  min_rev_exp = ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_REV_EXP;
  div_const = ARAD_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR;
  if (SOC_IS_JERICHO(unit)) {
      max_rev_exp += JER_PP_MTR_PROFILE_REV_EXP_BIAS;
      min_rev_exp += JER_PP_MTR_PROFILE_REV_EXP_BIAS;
      div_const = ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR;
  }

  
  for (tmp_rev_exp = max_rev_exp; tmp_rev_exp >= min_rev_exp; tmp_rev_exp--) {

      
      soc_sand_compute_complex_to_mnt_exp_reverse_exp(
          ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_IR,
          ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_EXP,
          tmp_rev_exp,
          clk_freq,
          div_const,
          ARAD_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR,
          FALSE,
          &calc_max_rate
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      soc_sand_compute_complex_to_mnt_exp_reverse_exp(
          ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_MNT_IR,
          ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_MNT_EXP,
          tmp_rev_exp,
          clk_freq,
          div_const,
          ARAD_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR,
          FALSE,
          &calc_min_rate
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      if ((calc_max_rate >= val) && (calc_min_rate <= val)) {
          
          break;
      }
  }

  if (tmp_rev_exp < min_rev_exp) {
      tmp_rev_exp = min_rev_exp;
  }

  
  res = soc_sand_break_complex_to_mnt_exp_round_down_2(
      val,
      ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_IR,
      ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_MNT_IR,
      ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_EXP, 
      clk_freq,
      (div_const << tmp_rev_exp),
      ARAD_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR, 
      mnt,
      exp
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  
  *rev_exp = tmp_rev_exp;

  if (SOC_IS_JERICHO(unit)) {
      *rev_exp -= JER_PP_MTR_PROFILE_REV_EXP_BIAS;
  }

  if (*rev_exp >= *exp) {
      *rev_exp = *rev_exp - *exp;
      *exp = 0;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_profile_rate_to_res_exp_mnt()", 0, 0);
}


uint32
  arad_pp_mtr_profile_burst_to_exp_mnt(
    SOC_SAND_IN int                       unit,
    SOC_SAND_IN uint32                       val,
    SOC_SAND_OUT uint32                      resolution, 
    SOC_SAND_OUT uint32                      *mnt,
    SOC_SAND_OUT uint32                      *exp
  )
{
  uint32
    res = SOC_SAND_OK,
    calc_val;
  SOC_SAND_U64
    tmp_val;
  uint32
      max_cbs_exp,
      max_cbs_mant;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(mnt);
  SOC_SAND_CHECK_NULL_INPUT(exp);

  

  
  soc_sand_u64_multiply_longs(val, (uint32)(1 << resolution), &tmp_val); 
  res = soc_sand_u64_to_long(&tmp_val, &calc_val);
  if (res  || (calc_val > ARAD_PP_MTR_PROFILE_MAX_BS)) { 
      
      calc_val = ARAD_PP_MTR_PROFILE_MAX_BS;
  }

  if (SOC_IS_JERICHO(unit)) {
      max_cbs_exp  = JER_PP_MTR_PROFILE_VAL_TO_EXP_MNT_MAX_EXP_BS;
      max_cbs_mant = JER_PP_MTR_PROFILE_VAL_TO_EXP_MNT_MAX_MNT_BS;
  }else{
      max_cbs_exp  = ARAD_PP_MTR_PROFILE_VAL_TO_EXP_MNT_MAX_EXP_BS;
      max_cbs_mant = ARAD_PP_MTR_PROFILE_VAL_TO_EXP_MNT_MAX_MNT_BS;
  }


  res = soc_sand_break_complex_to_mnt_exp_round_down_2(
      calc_val,
      max_cbs_mant,
      1,
      max_cbs_exp,
      ARAD_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MULTI_BS,
      1,
      ARAD_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MNT_INC_BS,
      mnt,
      exp
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_profile_burst_to_exp_mnt()", 0, 0);
}


void
  arad_pp_mtr_verify_valid_bucket_size(
    SOC_SAND_IN     int         unit,
    SOC_SAND_IN     uint32      rate1_man,
    SOC_SAND_IN     uint32      rate1_exp,
    SOC_SAND_IN     uint32      rate2_man,
    SOC_SAND_IN     uint32      rate2_exp,
    SOC_SAND_INOUT  uint32      *bucket_man,
    SOC_SAND_INOUT  uint32      *bucket_exp
  )
{
    uint32 bucket_size, bucket_size_min, bucket_size_max;
    uint32 rate1, rate2, total_rate;

    rate1 = (rate1_man)<<rate1_exp;
    rate2 = (rate2_man)<<rate2_exp;

    total_rate = rate1+rate2;
    bucket_size = (*bucket_man+64)<<(*bucket_exp);
    if (bucket_size < total_rate) 
    {
        *bucket_man = 0;
        *bucket_exp = 0;
        
        while (*bucket_exp <= ARAD_BUCKET_EXP_MAX_VALUE)
        {
            bucket_size_min = (64 + 0)<<(*bucket_exp);
            bucket_size_max = (64 + ARAD_BUCKET_MANTISSA_MAX_VALUE)<<(*bucket_exp);
            if( (bucket_size_min <= total_rate) && (bucket_size_max >= total_rate) )
            {
                break;   
            }   
            *bucket_exp = *bucket_exp + 1;
        }        
        
        if(*bucket_exp > ARAD_BUCKET_EXP_MAX_VALUE)
        {
            *bucket_man = ARAD_BUCKET_MANTISSA_MAX_VALUE;
            *bucket_exp = ARAD_BUCKET_EXP_MAX_VALUE;
        }
        else
        {
            
            *bucket_man = total_rate>>*bucket_exp;
            *bucket_man = *bucket_man - 64;
            bucket_size = (*bucket_man+64)<<(*bucket_exp);
            if(bucket_size < total_rate)
            {
                *bucket_man = *bucket_man + 1;
            }
        }
        
        LOG_DEBUG(BSL_LS_SOC_METERING,
                  (BSL_META_U(unit,
                              "WARNING: bucket size has changed!!!"
                              "Orginal value: %d [kbps]\n"
                              "Actual value: %d [kbps]\n"), bucket_size, total_rate));
    }
}

void
  arad_pp_mtr_eth_policer_profile_key_get(
    SOC_SAND_IN  int                                               unit,
    SOC_SAND_IN  ARAD_IDR_ETHERNET_METER_PROFILES_TBL_DATA         profile_tbl_data,
    SOC_SAND_OUT uint32                                            *eth_mtr_profile_multiset_key
  )
{
  *eth_mtr_profile_multiset_key = 0;
  *eth_mtr_profile_multiset_key |= SOC_SAND_SET_BITS_RANGE(profile_tbl_data.rate_mantissa,6,0); 
  *eth_mtr_profile_multiset_key |= SOC_SAND_SET_BITS_RANGE(profile_tbl_data.rate_exp,9,7); 
  *eth_mtr_profile_multiset_key |= SOC_SAND_SET_BITS_RANGE(profile_tbl_data.burst_mantissa,17,10); 
  *eth_mtr_profile_multiset_key |= SOC_SAND_SET_BITS_RANGE(profile_tbl_data.burst_exp,22,18); 
  *eth_mtr_profile_multiset_key |= SOC_SAND_SET_BITS_RANGE(profile_tbl_data.packet_mode,23,23);
  *eth_mtr_profile_multiset_key |= SOC_SAND_SET_BITS_RANGE(profile_tbl_data.meter_resolution,27,24);
  if (SOC_IS_ARADPLUS(unit)) {
    *eth_mtr_profile_multiset_key |= SOC_SAND_SET_BITS_RANGE(profile_tbl_data.color_blind,28,28);
  }
  if (SOC_IS_JERICHO(unit)) {
    *eth_mtr_profile_multiset_key |= SOC_SAND_SET_BITS_RANGE(profile_tbl_data.pkt_adj_header_truncate,29,29);
  }
}

void
  arad_pp_mtr_eth_policer_global_key_get(
    SOC_SAND_IN  int                                               unit,
    SOC_SAND_IN  ARAD_IDR_GLOBAL_METER_PROFILES_TBL_DATA           global_tbl_data,
    SOC_SAND_OUT uint32                                            *eth_mtr_global_multiset_key
  )
{
  *eth_mtr_global_multiset_key = 0;
  *eth_mtr_global_multiset_key |= SOC_SAND_SET_BITS_RANGE(global_tbl_data.rate_mantissa,6,0);
  *eth_mtr_global_multiset_key |= SOC_SAND_SET_BITS_RANGE(global_tbl_data.rate_exp,9,7);
  *eth_mtr_global_multiset_key |= SOC_SAND_SET_BITS_RANGE(global_tbl_data.burst_mantissa,17,10);
  *eth_mtr_global_multiset_key |= SOC_SAND_SET_BITS_RANGE(global_tbl_data.burst_exp,22,18);
  *eth_mtr_global_multiset_key |= SOC_SAND_SET_BITS_RANGE(global_tbl_data.packet_mode,23,23);
  *eth_mtr_global_multiset_key |= SOC_SAND_SET_BITS_RANGE(global_tbl_data.meter_resolution,27,24);
  if (SOC_IS_ARADPLUS(unit)) {
    *eth_mtr_global_multiset_key |= SOC_SAND_SET_BITS_RANGE(global_tbl_data.color_blind,28,28);
  }
}

STATIC
void
  arad_pp_mtr_max_min_vals_for_calc_get(
    SOC_SAND_IN  uint32           *vals,
    SOC_SAND_IN  uint32           nof_vals,
    SOC_SAND_OUT  uint32          *min_val,
    SOC_SAND_OUT  uint32          *max_val
  )
{
    int i = 0;
    *min_val = vals[i]; 
    *max_val = vals[i]; 
    for (i = 1; i < nof_vals; i++) {
        if (*min_val > vals[i]) {
            *min_val = vals[i];
        }
        else if (*max_val < vals[i]) {
            *max_val = vals[i];
        }
    }
}

#ifdef BCM_88660_A0

uint32 arad_pp_metering_dp_map_entry_set(
    SOC_SAND_IN int                                  unit, 
    SOC_SAND_IN SOC_PPC_MTR_COLOR_RESOLUTION_INFO     *dp_map_entry)
{
    uint32 addr_idx, dp_map_line;
    soc_mem_t mem;
    soc_field_t field;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(dp_map_entry);

    addr_idx = dp_map_line = 0;
    mem =  SOC_IS_QAX(unit) ? CGM_DP_MAPPINGm : IDR_DROP_PRECEDENCE_MAPPINGm;

    

    
    SHR_BITCOPY_RANGE(&addr_idx, ARAD_PP_MTR_DP_MAP_MTR_DP_LSB,
                      &(dp_map_entry->policer_color), 0, 
                      ARAD_PP_MTR_DP_MAP_MTR_DP_NOF_BITS);

    
    if (SOC_IS_JERICHO(unit)) {
        SHR_BITCOPY_RANGE(&addr_idx, ARAD_PP_MTR_DP_MAP_EM_DP_LSB,
                      &(dp_map_entry->ethernet_policer_color), 0, 
                      ARAD_PP_MTR_DP_MAP_IN_DP_NOF_BITS);
    }

    
    SHR_BITCOPY_RANGE(&addr_idx, ARAD_PP_MTR_DP_MAP_IN_DP_LSB,
                      &(dp_map_entry->incoming_color), 0, 
                      ARAD_PP_MTR_DP_MAP_IN_DP_NOF_BITS);


    
    SHR_BITCOPY_RANGE(&addr_idx, ARAD_PP_MTR_DP_MAP_METER_COMMAND_LSB,
                      &(dp_map_entry->policer_action), 0, 
                      ARAD_PP_MTR_DP_MAP_METER_COMMAND_NOF_BITS);

    
    field = SOC_IS_QAX(unit) ? ING_DPf : INGRESS_DROP_PRECEDENCEf;
    soc_mem_field32_set(unit, mem, (void *)&dp_map_line, field, dp_map_entry->ingress_color);
    field = SOC_IS_QAX(unit) ? EGR_DPf : EGRESS_DROP_PRECEDENCEf;    
    soc_mem_field32_set(unit, mem, (void *)&dp_map_line, field, dp_map_entry->egress_color);

    
    SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, mem, (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) ? IDR_BLOCK(unit) :  MEM_BLOCK_ANY, addr_idx, (void *)&dp_map_line)); 
    
exit:
    SOCDNX_FUNC_RETURN;
}

uint32 arad_pp_metering_dp_map_entry_get(
    SOC_SAND_IN int                                  unit, 
    SOC_SAND_OUT SOC_PPC_MTR_COLOR_RESOLUTION_INFO     *dp_map_entry)
{
    uint32 addr_idx, dp_map_line;
    soc_mem_t mem;
    soc_field_t field;    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(dp_map_entry);

    addr_idx = dp_map_line = 0;
    mem =  SOC_IS_QAX(unit) ? CGM_DP_MAPPINGm : IDR_DROP_PRECEDENCE_MAPPINGm;

    

    
    SHR_BITCOPY_RANGE(&addr_idx, ARAD_PP_MTR_DP_MAP_MTR_DP_LSB,
                      &(dp_map_entry->policer_color), 0, 
                      ARAD_PP_MTR_DP_MAP_MTR_DP_NOF_BITS);

    
    if (SOC_IS_JERICHO(unit)) {
        SHR_BITCOPY_RANGE(&addr_idx, ARAD_PP_MTR_DP_MAP_EM_DP_LSB,
                      &(dp_map_entry->ethernet_policer_color), 0, 
                      ARAD_PP_MTR_DP_MAP_IN_DP_NOF_BITS);
    }

    
    SHR_BITCOPY_RANGE(&addr_idx, ARAD_PP_MTR_DP_MAP_IN_DP_LSB,
                      &(dp_map_entry->incoming_color), 0, 
                      ARAD_PP_MTR_DP_MAP_IN_DP_NOF_BITS);

    
    SHR_BITCOPY_RANGE(&addr_idx, ARAD_PP_MTR_DP_MAP_METER_COMMAND_LSB,
                      &(dp_map_entry->policer_action), 0, 
                      ARAD_PP_MTR_DP_MAP_METER_COMMAND_NOF_BITS);

    SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, mem, (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) ? IDR_BLOCK(unit) :  MEM_BLOCK_ANY, addr_idx, (void *)&dp_map_line));

    
    field = SOC_IS_QAX(unit) ? ING_DPf : INGRESS_DROP_PRECEDENCEf;    
    dp_map_entry->ingress_color = soc_mem_field32_get(unit, mem, (void *)&dp_map_line, field);
    field = SOC_IS_QAX(unit) ? EGR_DPf : EGRESS_DROP_PRECEDENCEf;        
    dp_map_entry->egress_color = soc_mem_field32_get(unit, mem, (void *)&dp_map_line, field);
    
exit:
    SOCDNX_FUNC_RETURN;
}

uint32 arad_pp_metering_pcd_entry_set(
    SOC_SAND_IN int                                  unit, 
    SOC_SAND_IN SOC_PPC_MTR_COLOR_DECISION_INFO      *pcd_entry)
{
    uint32 addr_idx, pcd_line;
    int core_index;
    soc_mem_t mem;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(pcd_entry);

    addr_idx = pcd_line = 0;
    core_index = 0; 
    mem =  ARAD_PP_MTR_MEMORY(unit, PCD_MAPm);

    

    
    SHR_BITCOPY_RANGE(&addr_idx, ARAD_PP_MTR_PCD_MCDA_COL_LSB,
                      &(pcd_entry->policer0_color), 0, 
                      ARAD_PP_MTR_PCD_COL_NOF_BITS);
    SHR_BITCOPY_RANGE(&addr_idx, ARAD_PP_MTR_PCD_MCDB_COL_LSB,
                      &(pcd_entry->policer1_color), 0, 
                      ARAD_PP_MTR_PCD_COL_NOF_BITS);

    
    SHR_BITCOPY_RANGE(&addr_idx, ARAD_PP_MTR_PCD_MCDA_OTHER_HAS_CREDITS_LSB,
                      &(pcd_entry->policer0_other_bucket_has_credits), 0, 
                      ARAD_PP_MTR_PCD_OTHER_HAS_CREDITS_NOF_BITS);
    SHR_BITCOPY_RANGE(&addr_idx, ARAD_PP_MTR_PCD_MCDB_OTHER_HAS_CREDITS_LSB,
                      &(pcd_entry->policer1_other_bucket_has_credits), 0, 
                      ARAD_PP_MTR_PCD_OTHER_HAS_CREDITS_NOF_BITS);

    
    if(SOC_IS_QUX(unit))
    {
        SHR_BITCOPY_RANGE(&addr_idx, ARAD_PP_MTR_PCD_MCDA_MARK_BLACK_LSB,
                          &(pcd_entry->policer0_mark_black), 0, 
                          ARAD_PP_MTR_PCD_MARK_BLACK_NOF_BITS);
        SHR_BITCOPY_RANGE(&addr_idx, ARAD_PP_MTR_PCD_MCDB_MARK_BLACK_LSB,
                          &(pcd_entry->policer1_mark_black), 0, 
                          ARAD_PP_MTR_PCD_MARK_BLACK_NOF_BITS);     
    }

    
    soc_mem_field32_set(unit, mem, (void *)&pcd_line, MCDA_UPDATE_COLORf, pcd_entry->policer0_update_bucket);
    soc_mem_field32_set(unit, mem, (void *)&pcd_line, MCDB_UPDATE_COLORf, pcd_entry->policer1_update_bucket);
    soc_mem_field32_set(unit, mem, (void *)&pcd_line, OUT_COLORf, pcd_entry->policer_color);

    
    ARAD_PP_MTR_CORES_ITER(core_index){
        SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, mem, ARAD_PP_MTR_MEM_BLOCK(unit, core_index), addr_idx, (void *)&pcd_line)); 
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

uint32 arad_pp_metering_pcd_entry_get(
    SOC_SAND_IN int                                  unit, 
    SOC_SAND_OUT SOC_PPC_MTR_COLOR_DECISION_INFO      *pcd_entry)
{
    uint32 addr_idx, pcd_line;
    soc_mem_t mem;
    int core_index;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(pcd_entry);

    addr_idx = pcd_line = 0;
    core_index = 0; 
    mem =  ARAD_PP_MTR_MEMORY(unit, PCD_MAPm);

    

    
    SHR_BITCOPY_RANGE(&addr_idx, ARAD_PP_MTR_PCD_MCDA_COL_LSB,
                      &(pcd_entry->policer0_color), 0, 
                      ARAD_PP_MTR_PCD_COL_NOF_BITS);
    SHR_BITCOPY_RANGE(&addr_idx, ARAD_PP_MTR_PCD_MCDB_COL_LSB,
                      &(pcd_entry->policer1_color), 0, 
                      ARAD_PP_MTR_PCD_COL_NOF_BITS);

    
    SHR_BITCOPY_RANGE(&addr_idx, ARAD_PP_MTR_PCD_MCDA_OTHER_HAS_CREDITS_LSB,
                      &(pcd_entry->policer0_other_bucket_has_credits), 0, 
                      ARAD_PP_MTR_PCD_OTHER_HAS_CREDITS_NOF_BITS);
    SHR_BITCOPY_RANGE(&addr_idx, ARAD_PP_MTR_PCD_MCDB_OTHER_HAS_CREDITS_LSB,
                      &(pcd_entry->policer1_other_bucket_has_credits), 0, 
                      ARAD_PP_MTR_PCD_OTHER_HAS_CREDITS_NOF_BITS);

    
    if(SOC_IS_QUX(unit))
    {
        SHR_BITCOPY_RANGE(&addr_idx, ARAD_PP_MTR_PCD_MCDA_MARK_BLACK_LSB,
                          &(pcd_entry->policer0_mark_black), 0, 
                          ARAD_PP_MTR_PCD_MARK_BLACK_NOF_BITS);
        SHR_BITCOPY_RANGE(&addr_idx, ARAD_PP_MTR_PCD_MCDB_MARK_BLACK_LSB,
                          &(pcd_entry->policer1_mark_black), 0, 
                          ARAD_PP_MTR_PCD_MARK_BLACK_NOF_BITS);     
    }

    SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, mem, ARAD_PP_MTR_MEM_BLOCK(unit, core_index), addr_idx, (void *)&pcd_line));

    
    pcd_entry->policer0_update_bucket = soc_mem_field32_get(unit, mem, (void *)&pcd_line, MCDA_UPDATE_COLORf);
    pcd_entry->policer1_update_bucket = soc_mem_field32_get(unit, mem, (void *)&pcd_line, MCDB_UPDATE_COLORf);
    pcd_entry->policer_color = soc_mem_field32_get(unit, mem, (void *)&pcd_line, OUT_COLORf);
    
exit:
    SOCDNX_FUNC_RETURN;
}

uint32 
  arad_pp_metering_pcd_init(
     SOC_SAND_IN int unit,
     SOC_SAND_IN uint8 sharing_mode)
{
    uint32
        res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_METERING_PCD_INIT);

    
    if (SOC_PPC_MTR_SHARING_MODE_SERIAL == sharing_mode) {
        res = arad_pp_metering_pcd_init_serial(unit);
    }
    else{
        res = arad_pp_metering_pcd_init_parallel(unit);
    }
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_metering_pcd_init()", 0, 0);
}

 uint32
  arad_pp_metering_pcd_init_serial(
      SOC_SAND_IN int unit)
{
    uint32
        
        mcda_mtr_color,
        mcdb_mtr_color, 
        
        mcda_other_credits,
        mcdb_other_credits,
        
        final_color, 
        
        mcda_update_color,
        mcdb_update_color,
        addr_idx,
        pcd_line;
    uint32
        res = SOC_SAND_OK;
    soc_mem_t
        mem = ARAD_PP_MTR_MEMORY(unit, PCD_MAPm);
    int
        core_index;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    for (addr_idx = 0; addr_idx < ARAD_PP_MTR_PCD_OPT_NUM; addr_idx++) {

        mcda_mtr_color = mcdb_mtr_color = mcda_other_credits = mcdb_other_credits = 0;
        mcda_update_color = mcdb_update_color = final_color = 0;

        
        SHR_BITCOPY_RANGE(&(mcda_mtr_color), 0, &addr_idx, 
                          ARAD_PP_MTR_PCD_MCDA_COL_LSB, ARAD_PP_MTR_PCD_COL_NOF_BITS);
        SHR_BITCOPY_RANGE(&(mcdb_mtr_color), 0, &addr_idx, 
                          ARAD_PP_MTR_PCD_MCDB_COL_LSB, ARAD_PP_MTR_PCD_COL_NOF_BITS);

        
        SHR_BITCOPY_RANGE(&(mcda_other_credits), 0, &addr_idx, 
                          ARAD_PP_MTR_PCD_MCDA_OTHER_HAS_CREDITS_LSB, ARAD_PP_MTR_PCD_OTHER_HAS_CREDITS_NOF_BITS);
        SHR_BITCOPY_RANGE(&(mcdb_other_credits), 0, &addr_idx, 
                          ARAD_PP_MTR_PCD_MCDB_OTHER_HAS_CREDITS_LSB, ARAD_PP_MTR_PCD_OTHER_HAS_CREDITS_NOF_BITS);


        

        if ((mcda_mtr_color == ARAD_PP_MTR_PCD_COL_INVALID) && (mcdb_mtr_color == ARAD_PP_MTR_PCD_COL_INVALID)) {
            final_color = mcda_update_color = mcdb_update_color = ARAD_PP_MTR_PCD_COL_RED;
        } else if (mcda_mtr_color == ARAD_PP_MTR_PCD_COL_INVALID) {
            final_color = mcdb_update_color = mcdb_mtr_color;
            mcda_update_color = ARAD_PP_MTR_PCD_COL_INVALID;
        }
        else if (mcdb_mtr_color == ARAD_PP_MTR_PCD_COL_INVALID) {
            final_color = mcda_update_color = mcda_mtr_color;
            mcdb_update_color = ARAD_PP_MTR_PCD_COL_INVALID;
        }
        else if ((mcda_mtr_color == ARAD_PP_MTR_PCD_COL_RED) || (mcdb_mtr_color == ARAD_PP_MTR_PCD_COL_RED)) {
            final_color = mcda_update_color = mcdb_update_color = ARAD_PP_MTR_PCD_COL_RED;
        } 
        else if (mcda_mtr_color == mcdb_mtr_color) {
            final_color = mcda_update_color = mcdb_update_color = mcda_mtr_color;
        } 
        else if ((mcda_mtr_color == ARAD_PP_MTR_PCD_COL_GREEN) && (mcdb_mtr_color == ARAD_PP_MTR_PCD_COL_YELLOW)) {
            final_color = mcdb_update_color = ARAD_PP_MTR_PCD_COL_YELLOW;
            mcda_update_color = (mcda_other_credits ? ARAD_PP_MTR_PCD_COL_YELLOW : ARAD_PP_MTR_PCD_COL_GREEN);
        }
        else {
            final_color = mcda_update_color = mcdb_update_color = (mcdb_other_credits ? ARAD_PP_MTR_PCD_COL_YELLOW : ARAD_PP_MTR_PCD_COL_RED);
        }

        
        pcd_line = 0;
        soc_mem_field32_set(unit, mem, &pcd_line, MCDA_UPDATE_COLORf, mcda_update_color);
        soc_mem_field32_set(unit, mem, &pcd_line, MCDB_UPDATE_COLORf, mcdb_update_color);
        soc_mem_field32_set(unit, mem, &pcd_line, OUT_COLORf, final_color);

        
        ARAD_PP_MTR_CORES_ITER(core_index){
            res = soc_mem_write(unit, mem, ARAD_PP_MTR_MEM_BLOCK(unit, core_index), addr_idx, &pcd_line); 
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_metering_pcd_init_serial()", 0, 0);

}

 uint32 
  arad_pp_metering_pcd_init_parallel(
     SOC_SAND_IN int unit)
{
  
  uint32 
    addr_idx,
    meter_idx,
    color_idx,
    
    pcd_result = ARAD_PP_MTR_PCD_RESULT_WORST,
    
    pcd_bucket_update = ARAD_PP_MTR_PCD_BUCKET_UPDATE_CONSTANT,
    
    res_col,
    
    pcd_line,
    mtr_col,
    oth_cred,
    same_color_creds,
    oth_color_creds,
    oth_col,
    reduce_col;
  uint32
    res = SOC_SAND_OK;
  soc_mem_t
      mem = ARAD_PP_MTR_MEMORY(unit, PCD_MAPm);
  int
      core_index;

  
  arad_pp_mtr_pcd_meter_info meter_infos[ARAD_PP_MTR_PCD_NOF_PARALLEL_METERS];

  
  uint8 other_color_map[ARAD_PP_MTR_PCD_NOF_COLS];

  char 
    * propval,
    * propkey;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  other_color_map[ARAD_PP_MTR_PCD_COL_GREEN] = ARAD_PP_MTR_PCD_COL_YELLOW;
  other_color_map[ARAD_PP_MTR_PCD_COL_YELLOW] = ARAD_PP_MTR_PCD_COL_GREEN;
  other_color_map[ARAD_PP_MTR_PCD_COL_RED] = ARAD_PP_MTR_PCD_COL_YELLOW;

  meter_infos[ARAD_PP_MTR_PCD_PARALLEL_METER_A].addr_col_lsb = ARAD_PP_MTR_PCD_MCDA_COL_LSB;
  meter_infos[ARAD_PP_MTR_PCD_PARALLEL_METER_B].addr_col_lsb = ARAD_PP_MTR_PCD_MCDB_COL_LSB;
  meter_infos[ARAD_PP_MTR_PCD_PARALLEL_METER_A].addr_oth_cred_lsb = ARAD_PP_MTR_PCD_MCDA_OTHER_HAS_CREDITS_LSB;
  meter_infos[ARAD_PP_MTR_PCD_PARALLEL_METER_B].addr_oth_cred_lsb = ARAD_PP_MTR_PCD_MCDB_OTHER_HAS_CREDITS_LSB;
  
  
  pcd_result = ARAD_PP_MTR_PCD_RESULT_WORST;
  propkey = spn_POLICER_RESULT_PARALLEL_COLOR_MAP;
  propval = soc_property_get_str(unit, propkey);
  if (propval) {
    if (sal_strcmp(propval, "BEST") == 0) {
      pcd_result = ARAD_PP_MTR_PCD_RESULT_BEST;
    } else if (sal_strcmp(propval, "WORST") == 0) {
      pcd_result = ARAD_PP_MTR_PCD_RESULT_WORST;
    } else {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_METERING_POLICER_RESULT_PARALLEL_COLOR_MAPPING_INVALID_ERR, 1000, exit);
    }
  }

  
  pcd_bucket_update = ARAD_PP_MTR_PCD_BUCKET_UPDATE_CONSTANT;
  propkey = spn_POLICER_RESULT_PARALLEL_BUCKET_UPDATE;
  propval = soc_property_get_str(unit, propkey);
  if (propval) {
    if (sal_strcmp(propval, "CONSTANT") == 0) {
      pcd_bucket_update = ARAD_PP_MTR_PCD_BUCKET_UPDATE_CONSTANT;
    } else if (sal_strcmp(propval, "TRANSPARENT") == 0) {
      pcd_bucket_update = ARAD_PP_MTR_PCD_BUCKET_UPDATE_TRANSPARENT;
    } else if (sal_strcmp(propval, "DEFERRED") == 0) {
      pcd_bucket_update = ARAD_PP_MTR_PCD_BUCKET_UPDATE_DEFERRED;
    } else {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_METERING_POLICER_RESULT_PARALLEL_BUCKET_UPDATE_INVALID_ERR, 2000, exit);
    }
  }

  
  

  
  for (addr_idx = 0; addr_idx < ARAD_PP_MTR_PCD_OPT_NUM; addr_idx++) {
    
    
    res_col = (pcd_result == ARAD_PP_MTR_PCD_RESULT_BEST) ? ARAD_PP_MTR_PCD_COL_RED : ARAD_PP_MTR_PCD_COL_GREEN;

    
    for (meter_idx = 0; meter_idx < ARAD_PP_MTR_PCD_NOF_PARALLEL_METERS; meter_idx++) {
        for (color_idx = 0; color_idx < ARAD_PP_MTR_PCD_NOF_COLS; color_idx++) {
            meter_infos[meter_idx].bucket_credits[color_idx] = 0;
        }
    }

    
    for (meter_idx = 0; meter_idx < ARAD_PP_MTR_PCD_NOF_PARALLEL_METERS; meter_idx++) {
        mtr_col = 0;
        oth_cred = 0;
        
        
        SHR_BITCOPY_RANGE(&(mtr_col), 0, &addr_idx, 
                          meter_infos[meter_idx].addr_col_lsb, ARAD_PP_MTR_PCD_COL_NOF_BITS);
        SHR_BITCOPY_RANGE(&(oth_cred), 0, &addr_idx, 
                          meter_infos[meter_idx].addr_oth_cred_lsb, ARAD_PP_MTR_PCD_OTHER_HAS_CREDITS_NOF_BITS);

        
        
        if (mtr_col == ARAD_PP_MTR_PCD_COL_INVALID) {
            
            if (SOC_IS_ARADPLUS_AND_BELOW(unit)){
                mtr_col = ARAD_PP_MTR_PCD_COL_INVALID_EQUIV;
            }
        }

        oth_cred = oth_cred ? 1 : 0;

        
        
        
        meter_infos[meter_idx].bucket_credits[mtr_col] = 1;
        if (mtr_col != ARAD_PP_MTR_PCD_COL_INVALID) {
            meter_infos[meter_idx].bucket_credits[other_color_map[mtr_col]] = oth_cred;
        }
        if (mtr_col == ARAD_PP_MTR_PCD_COL_INVALID) {
            
            continue;
        }

        res_col = (pcd_result == ARAD_PP_MTR_PCD_RESULT_BEST) ? SOC_SAND_MIN(res_col, mtr_col) :
            SOC_SAND_MAX(res_col, mtr_col);
    }

    
    
    for (meter_idx = 0; meter_idx < ARAD_PP_MTR_PCD_NOF_PARALLEL_METERS; meter_idx++) {

        
        oth_col = other_color_map[res_col];
        same_color_creds = meter_infos[meter_idx].bucket_credits[res_col];
        oth_color_creds = meter_infos[meter_idx].bucket_credits[oth_col];
        
        
        
        reduce_col = ARAD_PP_MTR_PCD_COL_RED;

        switch (pcd_bucket_update) {
        case ARAD_PP_MTR_PCD_BUCKET_UPDATE_DEFERRED:
            
            
            if (res_col == ARAD_PP_MTR_PCD_COL_RED) {
                reduce_col = ARAD_PP_MTR_PCD_COL_RED;
            
            } else if (same_color_creds) {
                reduce_col = res_col;
            
            } else if (oth_color_creds) {
                reduce_col = oth_col;
            
            } else {
                reduce_col = ARAD_PP_MTR_PCD_COL_RED;
            }

            break;
        case ARAD_PP_MTR_PCD_BUCKET_UPDATE_TRANSPARENT:

            
            if (res_col == ARAD_PP_MTR_PCD_COL_RED) {
                reduce_col = ARAD_PP_MTR_PCD_COL_RED;
            
            } else if (same_color_creds) {
                reduce_col = res_col;
            
            } else {
                reduce_col = ARAD_PP_MTR_PCD_COL_RED;
            }

            break;
        case ARAD_PP_MTR_PCD_BUCKET_UPDATE_CONSTANT:
        default:
             
            if (res_col == ARAD_PP_MTR_PCD_COL_RED) {
                reduce_col = ARAD_PP_MTR_PCD_COL_RED;
            
            } else {
                reduce_col = res_col;
            }
            break;
        }

        meter_infos[meter_idx].reduce_buck_col = reduce_col;
    }

    pcd_line = 0;

    soc_mem_field32_set(unit, mem, &pcd_line, MCDA_UPDATE_COLORf, 
                        meter_infos[ARAD_PP_MTR_PCD_PARALLEL_METER_A].reduce_buck_col);
    soc_mem_field32_set(unit, mem, &pcd_line, MCDB_UPDATE_COLORf, 
                        meter_infos[ARAD_PP_MTR_PCD_PARALLEL_METER_B].reduce_buck_col);
    soc_mem_field32_set(unit, mem, &pcd_line, OUT_COLORf, res_col);

    ARAD_PP_MTR_CORES_ITER(core_index){
        res = soc_mem_write(unit, mem, ARAD_PP_MTR_MEM_BLOCK(unit, core_index), addr_idx, &pcd_line); 
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 3000, exit);
    }
  }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_metering_pcd_init_parallel()", 0, 0);
}
#endif 

uint32
  arad_pp_metering_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    reg_val,
    range_mode,
    reg_val_a,
    reg_val_b,
    reg_val_2;
  uint32
    eth_mtr_global_multiset_key = 0,
    eth_mtr_global_multiset_ndx = 0,
    eth_mtr_profile_multiset_key = 0,
    eth_mtr_profile_multiset_ndx = 0;
  uint8
    eth_mtr_global_multiset_first_appear,
    eth_mtr_profile_multiset_first_appear;
  SOC_SAND_SUCCESS_FAILURE
    eth_mtr_global_multiset_success = SOC_SAND_SUCCESS,
    eth_mtr_profile_multiset_success = SOC_SAND_SUCCESS;
  ARAD_IDR_GLOBAL_METER_PROFILES_TBL_DATA
    global_tbl_data;
  uint8
    range_mode_64k,
    color_blind_mode,
    sharing_mode;
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_METERING_INIT_UNSAFE);

  res = arad_itm_setup_dp_map(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 2300, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,READ_MRPS_GLBL_CFGr(unit, &reg_val));

  
  soc_reg_field_set(unit, MRPS_GLBL_CFGr, &reg_val, MCDA_INITf, 0);
  soc_reg_field_set(unit, MRPS_GLBL_CFGr, &reg_val, MCDB_INITf, 0);

  
  range_mode = SOC_DPP_CONFIG(unit)->meter.meter_range_mode;

  range_mode_64k = (range_mode == 64 ? 1 : 0);
  soc_reg_field_set(unit, MRPS_GLBL_CFGr, &reg_val, MCQ_SIZE_SELf, range_mode_64k);

  
  
  if (SOC_IS_ARADPLUS_AND_BELOW(unit) 
      && (range_mode_64k || (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "meter_pointer_0_enable", 0) != 1))) {
    
    res = READ_IDR_INVALID_METER_POINTERr(unit, &reg_val_2);
    SOC_SAND_CHECK_FUNC_RESULT(res, 23, exit);

    soc_reg_field_set(unit, IDR_INVALID_METER_POINTERr, &reg_val_2, INVALID_METER_POINTER_VALIDf, 1);
    soc_reg_field_set(unit, IDR_INVALID_METER_POINTERr, &reg_val_2, INVALID_METER_POINTERf, 0);

    res = WRITE_IDR_INVALID_METER_POINTERr(unit, reg_val_2);
    SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
  }

  
  sharing_mode = SOC_DPP_CONFIG(unit)->meter.sharing_mode;
  if ((range_mode_64k && (sharing_mode != SOC_PPC_MTR_SHARING_MODE_NONE)) || 
      (!range_mode_64k && ((sharing_mode != SOC_PPC_MTR_SHARING_MODE_SERIAL) && (sharing_mode != SOC_PPC_MTR_SHARING_MODE_PARALLEL)))) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_METERING_POLICER_INGRESS_SHARING_MODE_INVALID_ERR, 30, exit);
  }
  if (range_mode == 32) { 
      soc_reg_field_set(unit, MRPS_GLBL_CFGr, &reg_val, MCQ_MCDS_PARALLELf, sharing_mode == SOC_PPC_MTR_SHARING_MODE_PARALLEL ? 1 : 0);
  }

  
  soc_reg_field_set(unit, MRPS_GLBL_CFGr, &reg_val, HIERARCHICAL_MODEf, 1);

  
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  0,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FTMH_DP_OVERWRITEf,  1));

#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
      
      
      color_blind_mode = soc_property_get(unit, spn_RATE_COLOR_BLIND, 0) ? 1 : 0;
      
      
      
      if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  __LINE__,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IDR_COMPATIBILITY_REGISTERr, REG_PORT_ANY, 0, SYS_CONFIG_6f, color_blind_mode ? 1 : 0));
      }

      if (color_blind_mode == 1) {
        uint32 profile_data;
        uint32 profile_idx;

        
        for (profile_idx = 0; profile_idx < ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_PROFILE_METER_PROFILE_NOF_MEMBER; profile_idx++) {
          res = READ_IDR_ETHERNET_METER_PROFILESm(unit, MEM_BLOCK_ANY, profile_idx, &profile_data);
          SOC_SAND_CHECK_FUNC_RESULT(res, 576, exit);

          soc_mem_field32_set(unit, IDR_ETHERNET_METER_PROFILESm, &profile_data, COLOR_BLINDf, 1);

          res = WRITE_IDR_ETHERNET_METER_PROFILESm(unit, MEM_BLOCK_ANY, profile_idx, &profile_data);
          SOC_SAND_CHECK_FUNC_RESULT(res, 577, exit);
        }

        
        for (profile_idx = 0; profile_idx < ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_GLOBAL_METER_PROFILE_NOF_MEMBER; profile_idx++) {
          
          res = READ_IDR_GLOBAL_METER_PROFILESm(unit, MEM_BLOCK_ANY, profile_idx, &profile_data);
          SOC_SAND_CHECK_FUNC_RESULT(res, 578, exit);

          soc_mem_field32_set(unit, IDR_GLOBAL_METER_PROFILESm, &profile_data, COLOR_BLINDf, 1);
          
          res = WRITE_IDR_GLOBAL_METER_PROFILESm(unit, MEM_BLOCK_ANY, profile_idx, &profile_data);
          SOC_SAND_CHECK_FUNC_RESULT(res, 579, exit);
        }
      }

      
      
      if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  __LINE__,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, IDR_SPARE_REGISTER_2r, REG_PORT_ANY, 0, SYS_CONFIG_3f,  SOC_DPP_CONFIG(unit)->meter.color_resolution_mode == SOC_PPC_MTR_COLOR_RESOLUTION_MODE_RED_DIFF));
      }

      
      soc_reg_field_set(unit, MRPS_GLBL_CFGr, &reg_val, PACKET_MODE_ENf, 1);

      
      if ((range_mode == 32) && (sharing_mode != SOC_PPC_MTR_SHARING_MODE_NONE)){
        soc_reg_field_set(unit, MRPS_GLBL_CFGr, &reg_val, PCD_MAP_ENf, 1);
      }
  }
#endif 

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,WRITE_MRPS_GLBL_CFGr(unit,  reg_val));

  reg_val_a = 0;
  reg_val_b = 0;

  
  soc_reg_field_set(unit, MRPS_MCDA_CFGr, &reg_val_a, MCDA_TIMER_ENf, 1);
  soc_reg_field_set(unit, MRPS_MCDB_CFGr, &reg_val_b, MCDB_TIMER_ENf, 1);

  
  soc_reg_field_set(unit, MRPS_MCDA_CFGr, &reg_val_a, MCDA_REFRESH_ENf, 1);
  soc_reg_field_set(unit, MRPS_MCDB_CFGr, &reg_val_b, MCDB_REFRESH_ENf, 1);

  
  soc_reg_field_set(unit, MRPS_MCDA_CFGr, &reg_val_a, MCDA_CBL_RND_MODE_ENf, 1);
  soc_reg_field_set(unit, MRPS_MCDB_CFGr, &reg_val_b, MCDB_CBL_RND_MODE_ENf, 1);

  
  soc_reg_field_set(unit, MRPS_MCDA_CFGr, &reg_val_a, MCDA_ERR_COMP_ENABLEf, 1);
  soc_reg_field_set(unit, MRPS_MCDB_CFGr, &reg_val_b, MCDB_ERR_COMP_ENABLEf, 1);

  soc_reg_field_set(unit, MRPS_MCDA_CFGr, &reg_val_a, MCDA_RND_RANGEf, 3);
  soc_reg_field_set(unit, MRPS_MCDB_CFGr, &reg_val_b, MCDB_RND_RANGEf, 3);

  soc_reg_field_set(unit, MRPS_MCDA_CFGr, &reg_val_a, MCDA_BUBBLE_RATEf, 0xff);
  soc_reg_field_set(unit, MRPS_MCDB_CFGr, &reg_val_b, MCDB_BUBBLE_RATEf, 0xff);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,WRITE_MRPS_MCDA_CFGr(unit,  reg_val_a));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,WRITE_MRPS_MCDB_CFGr(unit,  reg_val_b));

  reg_val_a = 0;
  reg_val_b = 0;

  
  soc_reg_field_set(unit, MRPS_MCDA_REFRESH_CFGr, &reg_val_a, MCDA_REFRESH_START_INDEXf, 0);
  soc_reg_field_set(unit, MRPS_MCDB_REFRESH_CFGr, &reg_val_b, MCDB_REFRESH_START_INDEXf, 0);

  soc_reg_field_set(unit, MRPS_MCDA_REFRESH_CFGr, &reg_val_a, MCDA_REFRESH_END_INDEXf, 0x1fff);
  soc_reg_field_set(unit, MRPS_MCDB_REFRESH_CFGr, &reg_val_b, MCDB_REFRESH_END_INDEXf, 0x1fff);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,WRITE_MRPS_MCDA_REFRESH_CFGr(unit,  reg_val_a));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,WRITE_MRPS_MCDB_REFRESH_CFGr(unit,  reg_val_b));

#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
      res = arad_pp_metering_pcd_init(unit, sharing_mode);
      SOC_SAND_CHECK_FUNC_RESULT(res, 888, exit);
    
  }
#endif 

  res = arad_sw_db_multiset_add(
        unit,
        ARAD_SW_DB_CORE_0,
        ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_ETH_METER_PROFILE,
        &eth_mtr_profile_multiset_key,
        &eth_mtr_profile_multiset_ndx,
        &eth_mtr_profile_multiset_first_appear,
        &eth_mtr_profile_multiset_success
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

   
  ARAD_CLEAR(&global_tbl_data, ARAD_IDR_GLOBAL_METER_PROFILES_TBL_DATA, 1);

  arad_pp_mtr_eth_policer_global_key_get(
    unit,
    global_tbl_data,
    &eth_mtr_global_multiset_key
  );

  res = arad_sw_db_multiset_add(
          unit,
          ARAD_SW_DB_CORE_0,
          ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_GLOBAL_METER_PROFILE,
          &eth_mtr_global_multiset_key,
          &eth_mtr_global_multiset_ndx,
          &eth_mtr_global_multiset_first_appear,
          &eth_mtr_global_multiset_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);
     
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_metering_init_unsafe()", 0, 0);
}



uint32
  arad_pp_mtr_policer_ipg_compensation_set(
    int                         unit,
    uint8                       ipg_compensation_enabled
)
{
uint32 res;
    SOCDNX_INIT_FUNC_DEFS;

    res = soc_reg_above_64_field32_modify(unit, IDR_SPARE_REGISTER_2r, REG_PORT_ANY, 0, IPG_COMPENSATION_ENf, ipg_compensation_enabled);
    SOCDNX_IF_ERR_EXIT(res);
    
exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_pp_mtr_policer_ipg_compensation_get(
    int                         unit,
    uint8                       *ipg_compensation_enabled
)
{
uint32 reg_val, res;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(ipg_compensation_enabled);

    res = soc_reg32_get(unit, IDR_SPARE_REGISTER_2r, REG_PORT_ANY, 0, &reg_val);
    SOCDNX_IF_ERR_EXIT(res);

    *ipg_compensation_enabled = soc_reg_field_get(unit, IDR_SPARE_REGISTER_2r, reg_val, IPG_COMPENSATION_ENf);

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_pp_mtr_meters_group_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                    core_id,
    SOC_SAND_IN  uint32                                 mtr_group_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_GROUP_INFO                 *mtr_group_info
  )
{
  uint32
    res = SOC_SAND_OK;
  soc_reg_t 
    reg;
  soc_field_t
    field_rnd_mode_en,
    field_rnd_range;
  uint32
    reg_val,
    enable;
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_METERS_GROUP_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mtr_group_info);

  if (mtr_group_ndx == 0)
  {
    if (SOC_IS_QAX(unit)) {
        reg = IMP_MCDA_CFGr;
    }else{
        reg = MRPS_MCDA_CFGr;
    }
    field_rnd_mode_en = MCDA_CBL_RND_MODE_ENf;
    field_rnd_range = MCDA_RND_RANGEf;
  }
  else 
  {
    if (SOC_IS_QAX(unit)) {
        reg = IMP_MCDB_CFGr;
    }else{
        reg = MRPS_MCDB_CFGr;
    }
    field_rnd_mode_en = MCDB_CBL_RND_MODE_ENf;
    field_rnd_range = MCDB_RND_RANGEf;
  }

  res = soc_reg32_get(unit, reg, ARAD_PP_MTR_REG_PORT(unit, core_id), 0, &reg_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  enable =  SOC_SAND_BOOL2NUM(mtr_group_info->rand_mode_enable);
  soc_reg_field_set(unit, reg, &reg_val, field_rnd_mode_en, enable);

  if (enable) {
    
    soc_reg_field_set(unit, reg, &reg_val, field_rnd_range, 0);
  }

  res = soc_reg32_set(unit, reg, ARAD_PP_MTR_REG_PORT(unit, core_id), 0, reg_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_meters_group_info_set_unsafe()", mtr_group_ndx, 0);
}

uint32
  arad_pp_mtr_meters_group_info_set_verify(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_GROUP_INFO                  *mtr_group_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_METERS_GROUP_INFO_SET_VERIFY);
 
  SOC_SAND_ERR_IF_ABOVE_MAX(mtr_group_ndx, ARAD_PP_METERING_MTR_GROUP_NDX_MAX, ARAD_PP_METERING_MTR_GROUP_NDX_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_MTR_GROUP_INFO, mtr_group_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_meters_group_info_set_verify()", mtr_group_ndx, 0);
}

uint32
  arad_pp_mtr_meters_group_info_get_verify(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  uint32                                  mtr_group_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_METERS_GROUP_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mtr_group_ndx, ARAD_PP_METERING_MTR_GROUP_NDX_MAX, ARAD_PP_METERING_MTR_GROUP_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_meters_group_info_get_verify()", mtr_group_ndx, 0);
}


uint32
  arad_pp_mtr_meters_group_info_get_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_OUT SOC_PPC_MTR_GROUP_INFO                  *mtr_group_info
  )
{
  uint32
    res = SOC_SAND_OK;
  soc_reg_t 
    reg;
  soc_field_t
    field_rnd_mode_en;
  uint32
    reg_val,
    enable;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_METERS_GROUP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mtr_group_info);

  SOC_PPC_MTR_GROUP_INFO_clear(mtr_group_info);
  
  if (mtr_group_ndx == 0)
  {
    reg = MRPS_MCDA_CFGr;
    field_rnd_mode_en = MCDA_CBL_RND_MODE_ENf;
  }
  else 
  {
    reg = MRPS_MCDB_CFGr;
    field_rnd_mode_en = MCDB_CBL_RND_MODE_ENf;
  }

  res = soc_reg32_get(unit, reg, ARAD_PP_MTR_REG_PORT(unit, core_id), 0, &reg_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  enable = soc_reg_field_get(unit, reg, reg_val, field_rnd_mode_en);
  mtr_group_info->rand_mode_enable = SOC_SAND_NUM2BOOL(enable);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_meters_group_info_get_unsafe()", mtr_group_ndx, 0);
}


uint32
  arad_pp_mtr_bw_profile_add_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_BW_PROFILE_INFO             *bw_profile_info,
    SOC_SAND_OUT SOC_PPC_MTR_BW_PROFILE_INFO             *exact_bw_profile_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                *success
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_IDR_PRFCFG_TBL_DATA
    prfcfg_tbl_data;
  uint32
    clk_freq;
  soc_reg_above_64_val_t
    reg_above64_val;
  soc_mem_t 
    format_mem,
    mem;
  uint32
    entry_offset = bw_profile_ndx;
  uint32
    max_eir_for_calc;
  
  uint32
    ir_reverse_exponent_2 = ARAD_PP_INVALID_IR_REV_EXP,
    max_cir_mantissa_exponent = 0,
    max_cir_mantissa = 127;
  
  uint32
    cir_reverse_exponent_2 = 0,
    eir_reverse_exponent_2 = 0;
  uint32 
    rates[ARAD_PP_BW_PROFILE_MAX_RATES_FOR_CALC],
    nof_rates = -1;
  uint32 eir_valid_rate=0,max_eir_valid_rate=0,cir_valid_rate=0,max_cir_valid_rate=0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_BW_PROFILE_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(bw_profile_info);
  SOC_SAND_CHECK_NULL_INPUT(exact_bw_profile_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  ARAD_CLEAR(&prfcfg_tbl_data, ARAD_IDR_PRFCFG_TBL_DATA, 1);
  SOC_REG_ABOVE_64_CLEAR(reg_above64_val);

  clk_freq = arad_chip_ticks_per_sec_get(unit);

  if (bw_profile_info->max_eir == SOC_PPC_BW_PROFILE_IR_MAX_UNLIMITED) { 
      
      max_eir_for_calc = bw_profile_info->eir;

      
      prfcfg_tbl_data.max_eir_mantissa = ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_IR;
      prfcfg_tbl_data.max_eir_mantissa_exponent = ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_EXP;
  }
  else if(bw_profile_info->max_eir == 0) {
      prfcfg_tbl_data.max_eir_mantissa = 0;
      prfcfg_tbl_data.max_eir_mantissa_exponent = 0;
      max_eir_for_calc = 0;
  }
  else {
      max_eir_for_calc = bw_profile_info->max_eir;
      max_eir_valid_rate = 1;
  }
    
  
  if (bw_profile_info->is_sharing_enabled) {

      
      format_mem = SOC_IS_QAX(unit) ? IMP_MCDA_PRFCFG_SHARING_ENm : MRPS_MCDA_PRFCFG_SHARING_ENm;


      if (bw_profile_info->max_cir == SOC_PPC_BW_PROFILE_IR_MAX_UNLIMITED) { 
          
          max_cir_mantissa = ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_IR;
          max_cir_mantissa_exponent = ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_EXP;
      }
      else if (bw_profile_info->max_cir == 0) {
        max_cir_mantissa = 0;
        max_cir_mantissa_exponent = 0;        
      }
      else {
          max_cir_valid_rate = 1;
      }

      
      if(bw_profile_info->ir_rev_exp == ARAD_PP_INVALID_IR_REV_EXP)
      {
          LOG_ERROR(BSL_LS_SOC_METERING,(BSL_META_U(unit, "\n\r")));
          res = SOC_SAND_FAILURE_INTERNAL_ERR;
          SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);          
      }
      else
      {
          ir_reverse_exponent_2 = bw_profile_info->ir_rev_exp;
      }

      
      if (bw_profile_info->disable_cir && bw_profile_info->disable_eir 
            && (0 == bw_profile_info->max_cir)
            && (0 == bw_profile_info->max_eir)) 
      {
            max_cir_mantissa = 0;
            max_cir_mantissa_exponent = 0;
            prfcfg_tbl_data.max_eir_mantissa = 0;
            prfcfg_tbl_data.max_eir_mantissa_exponent = 0;           
            prfcfg_tbl_data.cir_mantissa = 0;
            prfcfg_tbl_data.cir_mantissa_exponent = 0;
            prfcfg_tbl_data.eir_mantissa = 0;
            prfcfg_tbl_data.eir_mantissa_exponent = 0;
      }
      else 
      {
          
          res = soc_sand_break_complex_to_mnt_exp_round_down_2(
              bw_profile_info->cir,
              ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_IR,
              ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_MNT_IR,
              ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_EXP,
              clk_freq,
              (ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR << (ir_reverse_exponent_2 + 2)),
              ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR, 
              &prfcfg_tbl_data.cir_mantissa,
              &prfcfg_tbl_data.cir_mantissa_exponent
            );
          SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

          res = soc_sand_break_complex_to_mnt_exp_round_down_2(
              bw_profile_info->eir,
              ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_IR,
              ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_MNT_IR,
              ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_EXP,
              clk_freq,
              (ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR << (ir_reverse_exponent_2 + 2)),
              ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR, 
              &prfcfg_tbl_data.eir_mantissa,
              &prfcfg_tbl_data.eir_mantissa_exponent
            );
          SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);

          if ((bw_profile_info->max_cir != SOC_PPC_BW_PROFILE_IR_MAX_UNLIMITED) && (bw_profile_info->max_cir != 0)) { 
              res = soc_sand_break_complex_to_mnt_exp_round_down_2(
                  bw_profile_info->max_cir,
                  ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_IR,
                  ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_MNT_IR,
                  ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_EXP,
                  clk_freq,
                  (ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR << (ir_reverse_exponent_2 + 2)),
                  ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR, 
                  &max_cir_mantissa,
                  &max_cir_mantissa_exponent
                  );
              SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
          }

          if ((bw_profile_info->max_eir != SOC_PPC_BW_PROFILE_IR_MAX_UNLIMITED) && (bw_profile_info->max_eir != 0)) { 
              res = soc_sand_break_complex_to_mnt_exp_round_down_2(
                  bw_profile_info->max_eir,
                  ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_IR,
                  ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_MNT_IR,
                  ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_EXP,
                  clk_freq,
                  (ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR << (ir_reverse_exponent_2 + 2)),
                  ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR, 
                  &prfcfg_tbl_data.max_eir_mantissa,
                  &prfcfg_tbl_data.max_eir_mantissa_exponent
                  );
              SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
          }
      }

      
      soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, IR_REV_EXP_2f, ir_reverse_exponent_2);
      soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, MAX_CIR_MANTf, max_cir_mantissa);
      soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, MAX_CIR_MANT_EXPf, max_cir_mantissa_exponent);
  }
  else {
      max_cir_valid_rate=0;
      
      format_mem = SOC_IS_QAX(unit) ? IMP_MCDA_PRFCFG_SHARING_DISm : MRPS_MCDA_PRFCFG_SHARING_DISm;

      
      if (bw_profile_info->is_coupling_enabled) {

        
        if (bw_profile_info->disable_cir && bw_profile_info->disable_eir) {
            
            cir_reverse_exponent_2 = 0;
            eir_reverse_exponent_2 = 0;
            max_eir_valid_rate=0;
        }
        else {
          if (bw_profile_info->disable_cir) {
              
              rates[0] = bw_profile_info->eir;
              rates[1] = max_eir_for_calc;
              nof_rates = 2;
              eir_valid_rate = 1;
              max_eir_valid_rate=0;
          }
          else if (bw_profile_info->disable_eir) {
              
              rates[0] = bw_profile_info->cir;
              nof_rates = 1;
              if (max_eir_for_calc >= SOC_DPP_CONFIG(unit)->meter.meter_min_rate) {
                  rates[1] = max_eir_for_calc;
                  nof_rates = 2;
              }
              cir_valid_rate = 1;
          }
          else { 
              rates[0] = bw_profile_info->eir;
              rates[1] = max_eir_for_calc;
              rates[2] = bw_profile_info->cir;
              nof_rates = 3;
              cir_valid_rate = 1;
              eir_valid_rate = 1;
          }

          
          res = arad_pp_mtr_ir_val_to_max_rev_exp_optimized_for_bucket_rate(
            unit,
            rates,
            nof_rates,
            &eir_reverse_exponent_2
            );
          SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);

          cir_reverse_exponent_2 = eir_reverse_exponent_2;
        }
      }
      else { 
        max_eir_valid_rate=0;
        eir_valid_rate=1;
        cir_valid_rate=1;
        rates[0] = bw_profile_info->cir;
        nof_rates = 1;

        
        if (bw_profile_info->disable_cir == FALSE)
        {         
            res = arad_pp_mtr_ir_val_to_max_rev_exp_optimized_for_bucket_rate(
                unit,
                rates,
                nof_rates,
                &cir_reverse_exponent_2
              );
            SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);
        }
        if (bw_profile_info->disable_eir == FALSE)
        {
            rates[0] = bw_profile_info->eir;
            rates[1] = max_eir_for_calc;
            nof_rates = 2;            
            res = arad_pp_mtr_ir_val_to_max_rev_exp_optimized_for_bucket_rate(
                unit,
                rates,
                nof_rates,
                &eir_reverse_exponent_2
              );
            SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);        
        }
      }

      
      res = soc_sand_break_complex_to_mnt_exp_round_down_2(
          bw_profile_info->cir,
          ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_IR,
          ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_MNT_IR,
          ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_EXP,
          clk_freq,
          (ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR << (cir_reverse_exponent_2 + 2)),
          ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR, 
          &prfcfg_tbl_data.cir_mantissa,
          &prfcfg_tbl_data.cir_mantissa_exponent
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);

      res = soc_sand_break_complex_to_mnt_exp_round_down_2(
          bw_profile_info->eir,
          ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_IR,
          ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_MNT_IR,
          ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_EXP,
          clk_freq,
          (ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR << (eir_reverse_exponent_2 + 2)),
          ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR, 
          &prfcfg_tbl_data.eir_mantissa,
          &prfcfg_tbl_data.eir_mantissa_exponent
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 19, exit);

      if ((bw_profile_info->max_eir != SOC_PPC_BW_PROFILE_IR_MAX_UNLIMITED) && (bw_profile_info->max_eir != 0)) { 
          res = soc_sand_break_complex_to_mnt_exp_round_down_2(
              bw_profile_info->max_eir,
              ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_IR,
              ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_MNT_IR,
              ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_EXP,
              clk_freq,
              (ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR << (eir_reverse_exponent_2 + 2)),
              ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR, 
              &prfcfg_tbl_data.max_eir_mantissa,
              &prfcfg_tbl_data.max_eir_mantissa_exponent
              );
          SOC_SAND_CHECK_FUNC_RESULT(res, 21, exit);
      }

      
      soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, CIR_REV_EXP_2f, cir_reverse_exponent_2);
      soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, EIR_REV_EXP_2f, eir_reverse_exponent_2);
  }

  
  res = arad_pp_mtr_bs_val_to_exp_mnt(
      unit,
      bw_profile_info->cbs,
      &prfcfg_tbl_data.cbs_exponent,
      &prfcfg_tbl_data.cbs_mantissa_64
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
  
  arad_pp_mtr_verify_valid_bucket_size(unit,
                                       prfcfg_tbl_data.cir_mantissa*cir_valid_rate,prfcfg_tbl_data.cir_mantissa_exponent*cir_valid_rate,
                                       max_cir_mantissa*max_cir_valid_rate,max_cir_mantissa_exponent*max_cir_valid_rate,
                                       &prfcfg_tbl_data.cbs_mantissa_64,&prfcfg_tbl_data.cbs_exponent);

  res = arad_pp_mtr_bs_val_to_exp_mnt(
      unit,
      bw_profile_info->ebs,
      &prfcfg_tbl_data.ebs_exponent,
      &prfcfg_tbl_data.ebs_mantissa_64
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 23, exit);

  arad_pp_mtr_verify_valid_bucket_size(unit,
                                       prfcfg_tbl_data.eir_mantissa*eir_valid_rate,prfcfg_tbl_data.eir_mantissa_exponent*eir_valid_rate,
                                       prfcfg_tbl_data.max_eir_mantissa*max_eir_valid_rate,prfcfg_tbl_data.max_eir_mantissa_exponent*max_eir_valid_rate,
                                       &prfcfg_tbl_data.ebs_mantissa_64,&prfcfg_tbl_data.ebs_exponent);

  prfcfg_tbl_data.color_aware   = bw_profile_info->color_mode == SOC_PPC_MTR_COLOR_MODE_AWARE ? 0x1 : 0x0;
  prfcfg_tbl_data.coupling_flag = SOC_SAND_BOOL2NUM(bw_profile_info->is_coupling_enabled);
  prfcfg_tbl_data.reset_cir = SOC_SAND_BOOL2NUM(bw_profile_info->disable_cir);
  prfcfg_tbl_data.reset_eir = SOC_SAND_BOOL2NUM(bw_profile_info->disable_eir);
#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
    prfcfg_tbl_data.packet_mode = bw_profile_info->is_packet_mode;
  }
#endif 

  
  soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, SHARING_FLAGf, bw_profile_info->is_sharing_enabled);
  if (SOC_IS_QUX(unit)) 
  {
      soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, MARK_BLACKf, bw_profile_info->mark_black);    
  }

  soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, COLOR_AWAREf, prfcfg_tbl_data.color_aware);
#ifdef BCM_88660_A0
  
  if (SOC_IS_ARADPLUS(unit)) {
      if (!bw_profile_info->is_sharing_enabled) {
          soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, COUPLING_FLAGf, prfcfg_tbl_data.coupling_flag);
      }
  }
  else
#endif 
  {
      soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, COUPLING_FLAGf, prfcfg_tbl_data.coupling_flag);
  }
  soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, RESET_CIRf, prfcfg_tbl_data.reset_cir);
  soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, RESET_EIRf, prfcfg_tbl_data.reset_eir);
  soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, CBS_MANT_64f, prfcfg_tbl_data.cbs_mantissa_64);
  soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, CBS_EXPONENTf, prfcfg_tbl_data.cbs_exponent);
  soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, EBS_MANT_64f, prfcfg_tbl_data.ebs_mantissa_64);
  
  soc_sand_bitstream_set_any_field(&(prfcfg_tbl_data.ebs_exponent), 21, 4, &(reg_above64_val[0]));
  soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, CIR_MANTf, prfcfg_tbl_data.cir_mantissa);
  soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, CIR_MANT_EXPf, prfcfg_tbl_data.cir_mantissa_exponent);
  soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, EIR_MANTf, prfcfg_tbl_data.eir_mantissa);
  soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, EIR_MANT_EXPf, prfcfg_tbl_data.eir_mantissa_exponent);
  soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, MAX_EIR_MANTf, prfcfg_tbl_data.max_eir_mantissa);
  soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, MAX_EIR_MANT_EXPf, prfcfg_tbl_data.max_eir_mantissa_exponent);    
#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
    soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, PACKET_MODEf, prfcfg_tbl_data.packet_mode);
  }
#endif 

  if (SOC_IS_JERICHO(unit)) {
      
      soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, MTR_PROFILE_MAP_TO_3f, 0);
      
      if (bw_profile_info->is_pkt_truncate) {
          soc_mem_field32_set(unit, format_mem, (void *)reg_above64_val, MTR_PROFILE_MAP_TO_4f, 11);
      }else{
          soc_mem_field32_set(unit, format_mem, (void *)reg_above64_val, MTR_PROFILE_MAP_TO_4f, 9);
      }
  }

  
  if (mtr_group_ndx == 0) { 
      mem = ARAD_PP_MTR_MEMORY(unit, MCDA_PRFCFG_0m);
  } else { 
      mem = ARAD_PP_MTR_MEMORY(unit, MCDB_PRFCFG_0m);
  }

  
  res = soc_mem_write(unit, mem, ARAD_PP_MTR_MEM_BLOCK(unit, core_id), entry_offset, reg_above64_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 24, exit);

  
  if (SOC_IS_JERICHO(unit)) {
       
      if (mtr_group_ndx == 0) { 
         mem = JER_PP_MTR_MEMORY(unit, MCDA_PRFCFG_1m);
      } else { 
          mem = JER_PP_MTR_MEMORY(unit, MCDB_PRFCFG_1m);
      }

      
      res = soc_mem_write(unit, mem, ARAD_PP_MTR_MEM_BLOCK(unit, core_id), entry_offset, reg_above64_val);
      SOC_SAND_CHECK_FUNC_RESULT(res, 24, exit);
  }

  
  res = arad_pp_mtr_bw_profile_get_unsafe(
    unit,
    core_id,
    mtr_group_ndx,
    bw_profile_ndx,
    exact_bw_profile_info
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
  
  *success = SOC_SAND_SUCCESS;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_bw_profile_add_unsafe()", mtr_group_ndx, bw_profile_ndx);
}

uint32
  arad_pp_mtr_bw_profile_add_verify(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_BW_PROFILE_INFO             *bw_profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_BW_PROFILE_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mtr_group_ndx, ARAD_PP_METERING_MTR_GROUP_NDX_MAX, ARAD_PP_METERING_MTR_GROUP_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(bw_profile_ndx, ARAD_PP_METERING_BW_PROFILE_NDX_MAX, ARAD_PP_METERING_BW_PROFILE_NDX_OUT_OF_RANGE_ERR, 20, exit);

  res = SOC_PPC_MTR_BW_PROFILE_INFO_verify(unit, bw_profile_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_bw_profile_add_verify()", mtr_group_ndx, bw_profile_ndx);
}


uint32
  arad_pp_mtr_bw_profile_get_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx,
    SOC_SAND_OUT SOC_PPC_MTR_BW_PROFILE_INFO             *bw_profile_info
  )
{
  uint32
    res = SOC_SAND_OK;
  soc_reg_above_64_val_t
    reg_above64_val;
  soc_mem_t
    format_mem, 
    mem;
  uint32
    entry_offset = bw_profile_ndx;
  ARAD_IDR_PRFCFG_TBL_DATA
    prfcfg_tbl_data;
  uint32
    cir_reverse_exponent_2,
    eir_reverse_exponent_2;
  
  uint32
    max_cir_mantissa_exponent = 0,
    max_cir_mantissa = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_BW_PROFILE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(bw_profile_info);

  SOC_PPC_MTR_BW_PROFILE_INFO_clear(bw_profile_info);
  ARAD_CLEAR(&prfcfg_tbl_data, ARAD_IDR_PRFCFG_TBL_DATA, 1);
  SOC_REG_ABOVE_64_CLEAR(reg_above64_val);

  
  if (mtr_group_ndx == 0) { 
      mem = ARAD_PP_MTR_MEMORY(unit, MCDA_PRFCFG_0m);
  } else { 
      mem = ARAD_PP_MTR_MEMORY(unit, MCDB_PRFCFG_0m);
  }
 
  
  res = soc_mem_read(unit, mem, ARAD_PP_MTR_MEM_BLOCK(unit, core_id), entry_offset, (void*)reg_above64_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  if (SOC_IS_QAX(unit)) {
      bw_profile_info->is_sharing_enabled = soc_mem_field32_get(unit, IMP_MCDA_PRFCFG_SHARING_ENm, (void *)reg_above64_val, SHARING_FLAGf);
  }else{
      bw_profile_info->is_sharing_enabled = soc_mem_field32_get(unit, MRPS_MCDA_PRFCFG_SHARING_ENm, (void *)reg_above64_val, SHARING_FLAGf);
  }

  if (bw_profile_info->is_sharing_enabled) {

      
      format_mem = SOC_IS_QAX(unit) ? IMP_MCDA_PRFCFG_SHARING_ENm : MRPS_MCDA_PRFCFG_SHARING_ENm;
      max_cir_mantissa = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, MAX_CIR_MANTf);
      max_cir_mantissa_exponent = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, MAX_CIR_MANT_EXPf);
      bw_profile_info->ir_rev_exp = cir_reverse_exponent_2 = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, IR_REV_EXP_2f);
      eir_reverse_exponent_2 = cir_reverse_exponent_2; 
  }
  else {

      
      format_mem = SOC_IS_QAX(unit) ? IMP_MCDA_PRFCFG_SHARING_DISm : MRPS_MCDA_PRFCFG_SHARING_DISm;
      cir_reverse_exponent_2 = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, CIR_REV_EXP_2f);
      eir_reverse_exponent_2 = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, EIR_REV_EXP_2f);
      bw_profile_info->ir_rev_exp = ARAD_PP_INVALID_IR_REV_EXP;
  }
  if (SOC_IS_QUX(unit)) 
  {
      bw_profile_info->mark_black = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, MARK_BLACKf);
  }

  prfcfg_tbl_data.color_aware = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, COLOR_AWAREf);
#ifdef BCM_88660_A0
  
  if (SOC_IS_ARADPLUS(unit)) {
      if (!bw_profile_info->is_sharing_enabled) {
          prfcfg_tbl_data.coupling_flag = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, COUPLING_FLAGf);
      }
  }
  else
#endif 
  {
      prfcfg_tbl_data.coupling_flag = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, COUPLING_FLAGf);
  }
  prfcfg_tbl_data.reset_cir = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, RESET_CIRf);
  prfcfg_tbl_data.reset_eir = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, RESET_EIRf);
  prfcfg_tbl_data.cbs_mantissa_64 = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, CBS_MANT_64f);
  prfcfg_tbl_data.cbs_exponent = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, CBS_EXPONENTf);
  prfcfg_tbl_data.ebs_mantissa_64 = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, EBS_MANT_64f);
  
  soc_sand_bitstream_get_any_field(&(reg_above64_val[0]), 21, 4, &(prfcfg_tbl_data.ebs_exponent));
  prfcfg_tbl_data.cir_mantissa = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, CIR_MANTf);
  prfcfg_tbl_data.cir_mantissa_exponent = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, CIR_MANT_EXPf);
  prfcfg_tbl_data.eir_mantissa = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, EIR_MANTf);
  prfcfg_tbl_data.eir_mantissa_exponent = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, EIR_MANT_EXPf);
  prfcfg_tbl_data.max_eir_mantissa = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, MAX_EIR_MANTf);
  prfcfg_tbl_data.max_eir_mantissa_exponent = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, MAX_EIR_MANT_EXPf);
#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
    prfcfg_tbl_data.packet_mode = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, PACKET_MODEf);
  }
#endif 

  
  res = arad_pp_mtr_ir_val_from_reverse_exp_mnt(
      unit,
      cir_reverse_exponent_2,
      prfcfg_tbl_data.cir_mantissa_exponent,
      prfcfg_tbl_data.cir_mantissa,
      &bw_profile_info->cir
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = arad_pp_mtr_ir_val_from_reverse_exp_mnt(
      unit,
      eir_reverse_exponent_2,
      prfcfg_tbl_data.eir_mantissa_exponent,
      prfcfg_tbl_data.eir_mantissa,
      &bw_profile_info->eir
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if ((prfcfg_tbl_data.max_eir_mantissa_exponent == ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_EXP) && 
      (prfcfg_tbl_data.max_eir_mantissa == ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_IR)) { 
      bw_profile_info->max_eir = 0xffffffff;
  }
  else {
      res = arad_pp_mtr_ir_val_from_reverse_exp_mnt(
          unit,
          eir_reverse_exponent_2,
          prfcfg_tbl_data.max_eir_mantissa_exponent,
          prfcfg_tbl_data.max_eir_mantissa,
          &bw_profile_info->max_eir
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

  if (bw_profile_info->is_sharing_enabled) {
      if ((max_cir_mantissa_exponent == ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_EXP) && 
          (max_cir_mantissa == ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_IR)) { 
          bw_profile_info->max_cir = 0xffffffff;
      }
      else {
          res = arad_pp_mtr_ir_val_from_reverse_exp_mnt(
              unit,
              cir_reverse_exponent_2,
              max_cir_mantissa_exponent,
              max_cir_mantissa,
              &bw_profile_info->max_cir
              );
          SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
      }
  }

  
  res = soc_sand_compute_complex_to_mnt_exp(
          prfcfg_tbl_data.cbs_mantissa_64,
          prfcfg_tbl_data.cbs_exponent,
          ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MULTI_BS,
          ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_BS,
          &bw_profile_info->cbs
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  res = soc_sand_compute_complex_to_mnt_exp(
          prfcfg_tbl_data.ebs_mantissa_64,
          prfcfg_tbl_data.ebs_exponent,
          ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MULTI_BS,
          ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_BS,
          &bw_profile_info->ebs
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  bw_profile_info->color_mode          = prfcfg_tbl_data.color_aware ? SOC_PPC_MTR_COLOR_MODE_AWARE : SOC_PPC_MTR_COLOR_MODE_BLIND;
  bw_profile_info->is_coupling_enabled = SOC_SAND_NUM2BOOL(prfcfg_tbl_data.coupling_flag);
  bw_profile_info->disable_cir = SOC_SAND_NUM2BOOL(prfcfg_tbl_data.reset_cir);
  bw_profile_info->disable_eir = SOC_SAND_NUM2BOOL(prfcfg_tbl_data.reset_eir);
#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
    bw_profile_info->is_packet_mode = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, PACKET_MODEf);
  }
  if (SOC_IS_JERICHO(unit)) {
    bw_profile_info->is_pkt_truncate = (soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, MTR_PROFILE_MAP_TO_4f) & 0x2) == 0 ? 0 : 1;
  }
#endif 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_bw_profile_get_unsafe()", mtr_group_ndx, bw_profile_ndx);
}

uint32 arad_pp_mtr_bw_profile_get_bucket_size(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core_id, 
    SOC_SAND_IN  uint32 mtr_group_ndx, 
    SOC_SAND_IN uint32 bw_profile_ndx, 
    SOC_SAND_OUT uint32 *cbs, 
    SOC_SAND_OUT uint32 *ebs)
{
  uint32
    res = SOC_SAND_OK,
    is_sharing_enabled = 0;
  soc_reg_above_64_val_t
    reg_above64_val;
  soc_mem_t
    mem,
    format_mem;
  ARAD_IDR_PRFCFG_TBL_DATA
    prfcfg_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(cbs);
  SOC_SAND_CHECK_NULL_INPUT(ebs);

  ARAD_CLEAR(&prfcfg_tbl_data, ARAD_IDR_PRFCFG_TBL_DATA, 1);
  SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
  *cbs = *ebs = 0;

  
  if (mtr_group_ndx == 0) { 
      mem = ARAD_PP_MTR_MEMORY(unit, MCDA_PRFCFG_0m);
  } else { 
      mem = ARAD_PP_MTR_MEMORY(unit, MCDB_PRFCFG_0m);
  }
 
  
  res = soc_mem_read(unit, mem, ARAD_PP_MTR_MEM_BLOCK(unit, core_id), bw_profile_ndx, (void*)reg_above64_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  is_sharing_enabled = soc_mem_field32_get(unit, SOC_IS_QAX(unit) ? IMP_MCDA_PRFCFG_SHARING_ENm : MRPS_MCDA_PRFCFG_SHARING_ENm, (void *)reg_above64_val, SHARING_FLAGf);

  if (is_sharing_enabled) {
      
      format_mem = SOC_IS_QAX(unit) ? IMP_MCDA_PRFCFG_SHARING_ENm : MRPS_MCDA_PRFCFG_SHARING_ENm;
  }
  else {
      
      format_mem = SOC_IS_QAX(unit) ? IMP_MCDA_PRFCFG_SHARING_DISm : MRPS_MCDA_PRFCFG_SHARING_DISm;
  }

  prfcfg_tbl_data.cbs_mantissa_64 = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, CBS_MANT_64f);
  prfcfg_tbl_data.cbs_exponent = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, CBS_EXPONENTf);
  prfcfg_tbl_data.ebs_mantissa_64 = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, EBS_MANT_64f);
  soc_sand_bitstream_get_any_field(&(reg_above64_val[0]), 21, 4, &(prfcfg_tbl_data.ebs_exponent));

  
  res = soc_sand_compute_complex_to_mnt_exp(
          prfcfg_tbl_data.cbs_mantissa_64,
          prfcfg_tbl_data.cbs_exponent,
          ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MULTI_BS,
          ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_BS,
          cbs
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_sand_compute_complex_to_mnt_exp(
          prfcfg_tbl_data.ebs_mantissa_64,
          prfcfg_tbl_data.ebs_exponent,
          ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MULTI_BS,
          ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_BS,
          ebs
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_bw_profile_get_bucket_size()", 0, 0);
}

uint32
  arad_pp_mtr_bw_profile_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_BW_PROFILE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mtr_group_ndx, ARAD_PP_METERING_MTR_GROUP_NDX_MAX, ARAD_PP_METERING_MTR_GROUP_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(bw_profile_ndx, ARAD_PP_METERING_BW_PROFILE_NDX_MAX, ARAD_PP_METERING_BW_PROFILE_NDX_OUT_OF_RANGE_ERR, 20, exit);

  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_bw_profile_get_verify()", mtr_group_ndx, bw_profile_ndx);
}


uint32
  arad_pp_mtr_bw_profile_remove_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx
  )
{
  uint32
    res = SOC_SAND_OK;
  soc_reg_above_64_val_t
    reg_above64_val;
  soc_mem_t
    mem;
  uint32
    entry_offset = bw_profile_ndx;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_BW_PROFILE_REMOVE_UNSAFE);

  SOC_REG_ABOVE_64_CLEAR(reg_above64_val);

  
  if (mtr_group_ndx == 0) { 
      mem = ARAD_PP_MTR_MEMORY(unit, MCDA_PRFCFG_0m);
  } else { 
      mem = ARAD_PP_MTR_MEMORY(unit, MCDB_PRFCFG_0m);
  }

  
  res = soc_mem_write(unit, mem, ARAD_PP_MTR_MEM_BLOCK(unit, core_id) , entry_offset, (void*)reg_above64_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_bw_profile_remove_unsafe()", mtr_group_ndx, bw_profile_ndx);
}

uint32
  arad_pp_mtr_bw_profile_remove_verify(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_BW_PROFILE_REMOVE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mtr_group_ndx, ARAD_PP_METERING_MTR_GROUP_NDX_MAX, ARAD_PP_METERING_MTR_GROUP_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(bw_profile_ndx, ARAD_PP_METERING_BW_PROFILE_NDX_MAX, ARAD_PP_METERING_BW_PROFILE_NDX_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_bw_profile_remove_verify()", mtr_group_ndx, bw_profile_ndx);
}


uint32
  arad_pp_mtr_meter_ins_to_bw_profile_map_set_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  SOC_PPC_MTR_METER_ID                    *meter_ins_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_id
  )
{
  uint32
    res = SOC_SAND_OK,
    array_index,
    index,
    profile_index,
    tmp_id,
    fld_val,
    profile_mem_val[2];
  soc_mem_t
    profile_mem;
  soc_field_t
    profile_fld;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(meter_ins_ndx);

  
  if (meter_ins_ndx->group == 0) {
      profile_mem = ARAD_PP_MTR_MEMORY(unit, MCDA_PRFSELm);
  }
  else {
      profile_mem = ARAD_PP_MTR_MEMORY(unit, MCDB_PRFSELm);
  }

  
  if (meter_ins_ndx->id < 16384 || SOC_IS_JERICHO(unit)) {
      array_index = 0;
      tmp_id = meter_ins_ndx->id;
  }
  else {
      array_index = 1;
      tmp_id = meter_ins_ndx->id - 16384;
  }

  
  
  index = tmp_id / 4;
  profile_index = tmp_id % 4;

  switch(profile_index) {
  case 0:
      profile_fld = PROFILE_0f;
      break;
  case 1:
      profile_fld = PROFILE_1f;
      break;
  case 2:
      profile_fld = PROFILE_2f;
      break;
  default: 
      profile_fld = PROFILE_3f;
  }

  
  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      res = soc_mem_array_read(unit, profile_mem, array_index, ARAD_PP_MTR_MEM_BLOCK(unit, core_id), index, profile_mem_val); 
  }
  else{
      res = soc_mem_read(unit, profile_mem, ARAD_PP_MTR_MEM_BLOCK(unit, core_id), index, profile_mem_val); 
  }
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  fld_val = bw_profile_id;
  soc_mem_field_set(unit, profile_mem, profile_mem_val, profile_fld , &fld_val);
 
  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      res = soc_mem_array_write(unit, profile_mem, array_index, ARAD_PP_MTR_MEM_BLOCK(unit, core_id), index, profile_mem_val);
  }
  else{
      res = soc_mem_write(unit, profile_mem, ARAD_PP_MTR_MEM_BLOCK(unit, core_id), index, profile_mem_val);
  }
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  res = arad_pp_mtr_meter_ins_to_bw_profile_map_init_buckets(unit, core_id, meter_ins_ndx, bw_profile_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_meter_ins_to_bw_profile_map_set_unsafe()", 0, 0);
}

uint32
  arad_pp_mtr_meter_ins_to_bw_profile_map_init_buckets(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  SOC_PPC_MTR_METER_ID                    *meter_ins_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_id
  )
{
uint32
   res,
   array_index,
   line_number,
   profile_index_in_array,
   offset_in_line,
   table_line[7],
   bucket_size[2],
   fld_val[2],
   write_access_enable;
soc_mem_t
   buckets_mem,
   dynamic_memory_mem;
soc_field_t
   bucket_fld[4][2] = {{CBL_0f, EBL_0f}, {CBL_1f, EBL_1f}, {CBL_2f, EBL_2f}, {CBL_3f, EBL_3f}};
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(meter_ins_ndx);

  res = SOC_SAND_OK;

  dynamic_memory_mem =  ARAD_PP_MTR_MEMORY(unit, ENABLE_DYNAMIC_MEMORY_ACCESSr);

  
  if (meter_ins_ndx->group == 0) {
      buckets_mem = ARAD_PP_MTR_MEMORY(unit, MCDA_DYNAMICm);
  }
  else {
      buckets_mem = ARAD_PP_MTR_MEMORY(unit, MCDB_DYNAMICm);
  }

  
  if (meter_ins_ndx->id < 16384 || SOC_IS_JERICHO(unit)) {
      array_index = 0;
      profile_index_in_array = meter_ins_ndx->id;
  }
  else {
      array_index = 1;
      profile_index_in_array = meter_ins_ndx->id - 16384;
  }

  
  line_number = profile_index_in_array / 4;
  offset_in_line = profile_index_in_array % 4;

  
  soc_reg32_get(unit, dynamic_memory_mem, ARAD_PP_MTR_REG_PORT(unit,core_id), 0, &write_access_enable);
  if (0 == write_access_enable) {
      res = soc_reg32_set(unit, dynamic_memory_mem, ARAD_PP_MTR_REG_PORT(unit,core_id), 0, 1);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  
  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      res = soc_mem_array_read(unit, buckets_mem, array_index, ARAD_PP_MTR_MEM_BLOCK(unit, core_id), line_number, table_line); 
  }
  else{
      res = soc_mem_read(unit, buckets_mem, ARAD_PP_MTR_MEM_BLOCK(unit, core_id), line_number, table_line); 
  }
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  
  res = arad_pp_mtr_bw_profile_get_bucket_size(unit, core_id, meter_ins_ndx->group, bw_profile_id, &bucket_size[0], &bucket_size[1]);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  ARAD_PP_MTR_DECIMAL_TO_BUKCET(bucket_size[0], fld_val[0]);
  ARAD_PP_MTR_DECIMAL_TO_BUKCET(bucket_size[1], fld_val[1]);

  soc_mem_field_set(unit, buckets_mem, table_line, bucket_fld[offset_in_line][0] , &fld_val[0]);
  soc_mem_field_set(unit, buckets_mem, table_line, bucket_fld[offset_in_line][1] , &fld_val[1]);

  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      res = soc_mem_array_write(unit, buckets_mem, array_index, ARAD_PP_MTR_MEM_BLOCK(unit, core_id), line_number, table_line);
  }
  else{
      res = soc_mem_write(unit, buckets_mem, ARAD_PP_MTR_MEM_BLOCK(unit, core_id), line_number, table_line);
  }
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  
  if (0 == write_access_enable) {
      res = soc_reg32_set(unit, dynamic_memory_mem, ARAD_PP_MTR_REG_PORT(unit,core_id), 0, 0);
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_meter_ins_to_bw_profile_map_init_buckets()", 0, 0);
}

uint32
  arad_pp_mtr_meter_ins_to_bw_profile_map_set_verify(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  SOC_PPC_MTR_METER_ID                    *meter_ins_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_MTR_METER_ID, meter_ins_ndx, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(bw_profile_id, ARAD_PP_METERING_BW_PROFILE_NDX_MAX, ARAD_PP_METERING_BW_PROFILE_ID_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_meter_ins_to_bw_profile_map_set_verify()", 0, 0);
}


soc_error_t
  arad_pp_mtr_meter_ins_bucket_get(
    int                                  unit,
    int                                  core_id,
    SOC_PPC_MTR_METER_ID                 *meter_ins_ndx,
    int                                  *cbl,
    int                                  *ebl
  )
{
uint32
    array_index,
    index,
    pair_index,
    tmp_id,
    fld_val,
    buckets_mem_val[7];
soc_mem_t
    buckets_mem;
soc_field_t
    cbl_bucket_fld,
    ebl_bucket_fld;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(meter_ins_ndx);
    SOCDNX_NULL_CHECK(cbl);
    SOCDNX_NULL_CHECK(ebl);

    
    if (meter_ins_ndx->group == 0) {
        buckets_mem = ARAD_PP_MTR_MEMORY(unit, MCDA_DYNAMICm);
    } else {
        buckets_mem = ARAD_PP_MTR_MEMORY(unit, MCDB_DYNAMICm);
    }

    
    if (meter_ins_ndx->id < 16384 || SOC_IS_JERICHO(unit)) {
        array_index = 0;
        tmp_id = meter_ins_ndx->id;
    } else {
        array_index = 1;
        tmp_id = meter_ins_ndx->id - 16384;
    }

    
    index = tmp_id / 4;
    pair_index = tmp_id % 4;

    switch(pair_index) {
    case 0:
      cbl_bucket_fld = CBL_0f;
      ebl_bucket_fld = EBL_0f;
      break;
    case 1:
      cbl_bucket_fld = CBL_1f;
      ebl_bucket_fld = EBL_1f;
      break;
    case 2:
      cbl_bucket_fld = CBL_2f;
      ebl_bucket_fld = EBL_2f;
      break;
    default: 
      cbl_bucket_fld = CBL_3f;
      ebl_bucket_fld = EBL_3f;
  }

  
  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      SOCDNX_SAND_IF_ERR_EXIT(soc_mem_array_read(unit, buckets_mem, array_index, ARAD_PP_MTR_MEM_BLOCK(unit, core_id), index, buckets_mem_val)); 
  }
  else{
      SOCDNX_SAND_IF_ERR_EXIT(soc_mem_read(unit, buckets_mem, ARAD_PP_MTR_MEM_BLOCK(unit, core_id), index, buckets_mem_val)); 
  }

  
  soc_mem_field_get(unit, buckets_mem, buckets_mem_val, cbl_bucket_fld , &fld_val);
  ARAD_PP_MTR_BUCKET_TO_DECIMAL(fld_val, *cbl);

  soc_mem_field_get(unit, buckets_mem, buckets_mem_val, ebl_bucket_fld , &fld_val);
  ARAD_PP_MTR_BUCKET_TO_DECIMAL(fld_val, *ebl);

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_pp_mtr_meter_ins_to_bw_profile_map_get_verify(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  SOC_PPC_MTR_METER_ID                    *meter_ins_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_GET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_MTR_METER_ID, meter_ins_ndx, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_meter_ins_to_bw_profile_map_get_verify()", 0, 0);
}


uint32
  arad_pp_mtr_meter_ins_to_bw_profile_map_get_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  SOC_PPC_MTR_METER_ID                    *meter_ins_ndx,
    SOC_SAND_OUT uint32                                  *bw_profile_id
  )
{
  uint32
    res = SOC_SAND_OK,
    array_index,
    index,
    profile_index,
    tmp_id,
    profile_id,
    mem_val[2];
  soc_mem_t
    mem;
  soc_field_t
    fld;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(meter_ins_ndx);
  SOC_SAND_CHECK_NULL_INPUT(bw_profile_id);
  
  
  if (meter_ins_ndx->group == 0) {
      mem = ARAD_PP_MTR_MEMORY(unit, MCDA_PRFSELm);
  }
  else {
      mem = ARAD_PP_MTR_MEMORY(unit, MCDB_PRFSELm);
  }

  
  if (meter_ins_ndx->id < 16384 || SOC_IS_JERICHO(unit)) {
      array_index = 0;
      tmp_id = meter_ins_ndx->id;
  }
  else {
      array_index = 1;
      tmp_id = meter_ins_ndx->id - 16384;
  }

  
  index = tmp_id / 4;
  profile_index = tmp_id % 4;

  switch(profile_index) {
  case 0:
      fld = PROFILE_0f;
      break;
  case 1:
      fld = PROFILE_1f;
      break;
  case 2:
      fld = PROFILE_2f;
      break;
  default: 
      fld = PROFILE_3f;
  }

  
  if (!SOC_IS_JERICHO(unit)) {
      res = soc_mem_array_read(unit, mem, array_index, ARAD_PP_MTR_MEM_BLOCK(unit, core_id), index, mem_val); 
  }
  else{
      res = soc_mem_read(unit, mem, ARAD_PP_MTR_MEM_BLOCK(unit, core_id), index, mem_val); 
  }
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  soc_mem_field_get(unit, mem, mem_val, fld, &profile_id);

  *bw_profile_id = profile_id;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_meter_ins_to_bw_profile_map_get_unsafe()", 0, 0);
}


uint32
  arad_pp_mtr_eth_policer_enable_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                  enable
  )
{
  uint32
    res = SOC_SAND_OK,
    idx,
    status_enable,
    status_enable_start,
    reg_val;
  uint8
    config_status_bit;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_ENABLE_SET_UNSAFE);

  if (SOC_IS_JERICHO(unit)) {
      goto exit;
  }

  
  res = arad_pp_sw_db_eth_policer_config_status_bit_set(
    unit,
    0,
    ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_ENABLE_STATUS_BIT_MAIN,
    enable
  );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  
  for (idx = 0; idx < ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_CONFIG_METER_PROFILE_NOF_MEMBER_BIT; idx++)
  {
      res = READ_IDR_ETHERNET_METER_CONFIGm(unit, MEM_BLOCK_ANY, idx, &reg_val); 
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      soc_mem_field_get(unit, IDR_ETHERNET_METER_CONFIGm, &reg_val, ENABLEf, &status_enable_start);

      res = sw_state_access[unit].dpp.soc.arad.pp.eth_policer_mtr_profile.config_meter_status.bit_get(
          unit,
          0,
          idx,
          &config_status_bit
          );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

      if ((enable == TRUE) && (config_status_bit == TRUE))
      {
          status_enable = SOC_SAND_BOOL2NUM(TRUE);
      }
      else 
      {
          status_enable = SOC_SAND_BOOL2NUM(FALSE);
      }

      if (status_enable_start != status_enable)
      {
          soc_mem_field_set(unit, IDR_ETHERNET_METER_CONFIGm, &reg_val, ENABLEf, &status_enable);
          
          res = WRITE_IDR_ETHERNET_METER_CONFIGm(unit, MEM_BLOCK_ANY, idx, &reg_val); 
          SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      }
  }

  
  for (idx = 0 ; idx < ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_GLOBAL_METER_PROFILE_NOF_MEMBER; idx++)
  {
    res = READ_IDR_GLOBAL_METER_PROFILESm(unit, MEM_BLOCK_ANY, idx, &reg_val); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    soc_mem_field_get(unit, IDR_GLOBAL_METER_PROFILESm, &reg_val, ENABLEf, &status_enable_start);

    res = sw_state_access[unit].dpp.soc.arad.pp.eth_policer_mtr_profile.config_meter_status.bit_get(
      unit,
      0,
      ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_CONFIG_METER_PROFILE_NOF_MEMBER_BIT + idx,
      &config_status_bit
    );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    if ((enable == TRUE) && (config_status_bit == TRUE))
    {
      status_enable = SOC_SAND_BOOL2NUM(TRUE);
    }
    else 
    {
      status_enable = SOC_SAND_BOOL2NUM(FALSE);
    }

    if (status_enable_start != status_enable)
    {
      soc_mem_field_set(unit, IDR_GLOBAL_METER_PROFILESm, &reg_val, ENABLEf, &status_enable);
          
      res = WRITE_IDR_GLOBAL_METER_PROFILESm(unit, MEM_BLOCK_ANY, idx, &reg_val); 
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }
  }
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_enable_set_unsafe()", 0, 0);
}

uint32
  arad_pp_mtr_eth_policer_enable_set_verify(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  uint8                                 enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_ENABLE_SET_VERIFY);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_enable_set_verify()", 0, 0);
}

uint32
  arad_pp_mtr_eth_policer_enable_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_ENABLE_GET_VERIFY);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_enable_get_verify()", 0, 0);
}


uint32
  arad_pp_mtr_eth_policer_enable_get_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_OUT uint8                                 *enable
  )
{
  soc_error_t rv;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_ENABLE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(enable);

  
  rv = sw_state_access[unit].dpp.soc.arad.pp.eth_policer_mtr_profile.config_meter_status.bit_get(
    unit,
    0,
    ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_ENABLE_STATUS_BIT_MAIN,
    enable
  );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_enable_get_unsafe()", 0, 0);
}


uint32
  arad_pp_mtr_eth_policer_params_set_unsafe(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  SOC_PPC_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE                        eth_type_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_BW_PROFILE_INFO                 *policer_info
  )
{
  uint32
    res = SOC_SAND_OK,
    eth_mtr_profile_multiset_key = 0x0,
    eth_mtr_profile_multiset_ndx = 0x0,
    config_tbl_offset,
    status_tbl_offset;
  uint32
    eth_mtr_config_val,
    eth_mtr_profile_val;
  uint8
    eth_mtr_profile_multiset_first_appear = 0x0,
    eth_mtr_profile_multiset_last_appear = 0x0,
    sw_db_enable_bit,
    main_enable,
    info_enable,
    enable;
  SOC_SAND_SUCCESS_FAILURE
    eth_mtr_profile_multiset_success = SOC_SAND_SUCCESS;
  ARAD_IDR_ETHERNET_METER_STATUS_TBL_DATA
    status_tbl_data;
  ARAD_IDR_ETHERNET_METER_CONFIG_TBL_DATA
    config_tbl_data_start,
    config_tbl_data;
  ARAD_IDR_ETHERNET_METER_PROFILES_TBL_DATA
    profile_tbl_data;
  int cir, cbs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_PARAMS_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(policer_info);

  ARAD_CLEAR(&status_tbl_data,ARAD_IDR_ETHERNET_METER_STATUS_TBL_DATA,1);
  ARAD_CLEAR(&config_tbl_data,ARAD_IDR_ETHERNET_METER_CONFIG_TBL_DATA,1);
  ARAD_CLEAR(&config_tbl_data_start,ARAD_IDR_ETHERNET_METER_CONFIG_TBL_DATA,1);
  ARAD_CLEAR(&profile_tbl_data,ARAD_IDR_ETHERNET_METER_PROFILES_TBL_DATA,1);

  status_tbl_offset = config_tbl_offset = port_ndx * SOC_PPC_NOF_MTR_ETH_TYPES + eth_type_ndx;

  info_enable = SOC_SAND_BOOL2NUM_INVERSE(policer_info->disable_cir);
  
    
  res = READ_IDR_ETHERNET_METER_CONFIGm(unit, MEM_BLOCK_ANY, config_tbl_offset, &eth_mtr_config_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  config_tbl_data_start.profile = soc_mem_field32_get(unit, IDR_ETHERNET_METER_CONFIGm, &eth_mtr_config_val, PROFILEf);

  res = sw_state_access[unit].dpp.soc.arad.pp.eth_policer_mtr_profile.config_meter_status.bit_get(
      unit,
      0,
      status_tbl_offset,
      &sw_db_enable_bit
    );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  
  
  res = sw_state_access[unit].dpp.soc.arad.pp.eth_policer_mtr_profile.config_meter_status.bit_get(
    unit,
    0,
    ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_ENABLE_STATUS_BIT_MAIN,
    &main_enable
  );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  if (sw_db_enable_bit == TRUE)
  {
       
    res = arad_sw_db_multiset_remove_by_index(
            unit,
            ARAD_SW_DB_CORE_0,
            ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_ETH_METER_PROFILE,
            config_tbl_data_start.profile,
            &eth_mtr_profile_multiset_last_appear
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

  cir = policer_info->cir;
  cbs = policer_info->cbs;

  
  if (info_enable == TRUE)
  {
    
    res = arad_pp_mtr_profile_rate_to_res_exp_mnt(
        unit,
        cir,
        &profile_tbl_data.meter_resolution,
        &profile_tbl_data.rate_mantissa,
        &profile_tbl_data.rate_exp
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    
    arad_pp_mtr_profile_burst_to_exp_mnt(
      unit, 
      cbs,
      profile_tbl_data.meter_resolution, 
      &profile_tbl_data.burst_mantissa,
      &profile_tbl_data.burst_exp
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

    profile_tbl_data.packet_mode = SOC_SAND_BOOL2NUM(policer_info->is_packet_mode);
    if (SOC_IS_ARADPLUS(unit)) {
      profile_tbl_data.color_blind = policer_info->color_mode == SOC_PPC_MTR_COLOR_MODE_BLIND ? 1 : 0;
    }

    arad_pp_mtr_eth_policer_profile_key_get(
      unit,
      profile_tbl_data,
      &eth_mtr_profile_multiset_key
    );
    
    res = arad_sw_db_multiset_add(
          unit,
          ARAD_SW_DB_CORE_0,
          ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_ETH_METER_PROFILE,
          &eth_mtr_profile_multiset_key,
          &eth_mtr_profile_multiset_ndx,
          &eth_mtr_profile_multiset_first_appear,
          &eth_mtr_profile_multiset_success
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

    if (eth_mtr_profile_multiset_success != SOC_SAND_SUCCESS)
    { 
      
      res = READ_IDR_ETHERNET_METER_PROFILESm(unit, MEM_BLOCK_ANY, config_tbl_data_start.profile, &eth_mtr_profile_val);
      SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);

      profile_tbl_data.rate_mantissa = soc_mem_field32_get(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, RATE_MANTISSAf);
      profile_tbl_data.rate_exp = soc_mem_field32_get(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, RATE_EXPf);
      profile_tbl_data.burst_mantissa = soc_mem_field32_get(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, BURST_MANTISSAf);
      profile_tbl_data.burst_exp = soc_mem_field32_get(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, BURST_EXPf);
      profile_tbl_data.packet_mode = soc_mem_field32_get(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, PACKET_MODEf);
      profile_tbl_data.meter_resolution = soc_mem_field32_get(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, METER_RESOLUTIONf);
      if (SOC_IS_ARADPLUS(unit)) {
        profile_tbl_data.color_blind = soc_mem_field32_get(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, COLOR_BLINDf);
      }

      arad_pp_mtr_eth_policer_profile_key_get(
        unit,
        profile_tbl_data,
        &eth_mtr_profile_multiset_key
      );

      res = arad_sw_db_multiset_add(
              unit,
              ARAD_SW_DB_CORE_0,
              ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_ETH_METER_PROFILE,
              &eth_mtr_profile_multiset_key,
              &eth_mtr_profile_multiset_ndx,
              &eth_mtr_profile_multiset_first_appear,
              &eth_mtr_profile_multiset_success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

      SOC_SAND_SET_ERROR_CODE(ARAD_PP_MTR_ETH_POLICER_ADD_FAIL_ERR, 100, exit);
    }
  }

  
  res = arad_pp_sw_db_eth_policer_config_status_bit_set(
    unit,
    0,
    status_tbl_offset,
    info_enable
  );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  
  
  if (eth_mtr_profile_multiset_first_appear == 0x1)
  {
    soc_mem_field32_set(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, RATE_MANTISSAf, profile_tbl_data.rate_mantissa);
    soc_mem_field32_set(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, RATE_EXPf, profile_tbl_data.rate_exp);
    soc_mem_field32_set(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, BURST_MANTISSAf, profile_tbl_data.burst_mantissa);
    soc_mem_field32_set(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, BURST_EXPf, profile_tbl_data.burst_exp);
    soc_mem_field32_set(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, PACKET_MODEf, profile_tbl_data.packet_mode);
    soc_mem_field32_set(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, METER_RESOLUTIONf, profile_tbl_data.meter_resolution);
    if (SOC_IS_ARADPLUS(unit)) {
      soc_mem_field32_set(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, COLOR_BLINDf, profile_tbl_data.color_blind);
    }
    
    res = WRITE_IDR_ETHERNET_METER_PROFILESm(unit, MEM_BLOCK_ANY, eth_mtr_profile_multiset_ndx, &eth_mtr_profile_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
  }

  
  
  res = READ_IDR_ETHERNET_METER_CONFIGm(unit, MEM_BLOCK_ANY, config_tbl_offset, &eth_mtr_config_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);
  
  soc_mem_field32_set(unit, IDR_ETHERNET_METER_CONFIGm, &eth_mtr_config_val, PROFILEf, eth_mtr_profile_multiset_ndx);

  
  if ((main_enable == TRUE) && (info_enable))
  {
    enable = TRUE;
  }
  else
  {
    enable = FALSE;
  }

  soc_mem_field32_set(unit, IDR_ETHERNET_METER_CONFIGm, &eth_mtr_config_val, ENABLEf, enable);
 
  res = WRITE_IDR_ETHERNET_METER_CONFIGm(unit, MEM_BLOCK_ANY, config_tbl_offset, &eth_mtr_config_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);

  
  if ((eth_mtr_profile_multiset_last_appear == 0x1) && (eth_mtr_profile_multiset_ndx != config_tbl_data_start.profile))
  {
    ARAD_CLEAR(&profile_tbl_data, ARAD_IDR_ETHERNET_METER_PROFILES_TBL_DATA, 1);

    soc_mem_field32_set(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, RATE_MANTISSAf, profile_tbl_data.rate_mantissa);
    soc_mem_field32_set(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, RATE_EXPf, profile_tbl_data.rate_exp);
    soc_mem_field32_set(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, BURST_MANTISSAf, profile_tbl_data.burst_mantissa);
    soc_mem_field32_set(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, BURST_EXPf, profile_tbl_data.burst_exp);
    soc_mem_field32_set(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, PACKET_MODEf, profile_tbl_data.packet_mode);
    soc_mem_field32_set(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, METER_RESOLUTIONf, profile_tbl_data.meter_resolution);
    if (SOC_IS_ARADPLUS(unit)) {
      if (soc_property_get(unit, spn_RATE_COLOR_BLIND, 0)) {
        profile_tbl_data.color_blind = 1;
      }
      soc_mem_field32_set(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, COLOR_BLINDf, profile_tbl_data.color_blind);
    }
    
    res = WRITE_IDR_ETHERNET_METER_PROFILESm(unit, MEM_BLOCK_ANY, config_tbl_data_start.profile, &eth_mtr_profile_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_params_set_unsafe()", port_ndx, 0);
}

uint32
  arad_pp_mtr_eth_policer_params_set_verify(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  SOC_PPC_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE                        eth_type_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_BW_PROFILE_INFO                 *policer_info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_PARAMS_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(eth_type_ndx, ARAD_PP_METERING_ETH_TYPE_NDX_MAX, ARAD_PP_METERING_ETH_TYPE_NDX_OUT_OF_RANGE_ERR, 20, exit);
  
  SOC_SAND_ERR_IF_ABOVE_MAX(policer_info->cir, ARAD_IF_MAX_RATE_KBPS(unit)              , ARAD_PP_METERING_CIR_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_BELOW_MIN(policer_info->cir, ARAD_PP_POLICER_PROFILE_ADD_VERIFY_IR_MIN, ARAD_PP_METERING_CIR_OUT_OF_RANGE_ERR, 40, exit); 

  SOC_SAND_ERR_IF_ABOVE_MAX(policer_info->cbs, ARAD_PP_BW_PROFILE_ADD_VERIFY_BS_MAX, ARAD_PP_METERING_CBS_OUT_OF_RANGE_ERR, 50, exit);
  SOC_SAND_ERR_IF_BELOW_MIN(policer_info->cbs, ARAD_PP_BW_PROFILE_ADD_VERIFY_BS_MIN, ARAD_PP_METERING_CBS_OUT_OF_RANGE_ERR, 60, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_params_set_verify()", port_ndx, 0);
}

uint32
  arad_pp_mtr_eth_policer_params_get_verify(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  SOC_PPC_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE                        eth_type_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_PARAMS_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(eth_type_ndx, ARAD_PP_METERING_ETH_TYPE_NDX_MAX, ARAD_PP_METERING_ETH_TYPE_NDX_OUT_OF_RANGE_ERR, 20, exit);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_params_get_verify()", port_ndx, 0);
}


uint32
  arad_pp_mtr_eth_policer_params_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE                        eth_type_ndx,
    SOC_SAND_OUT SOC_PPC_MTR_BW_PROFILE_INFO                 *policer_info
  )
{
  uint32
    res = SOC_SAND_OK,
    clk_freq,
    config_tbl_offset,
    status_tbl_offset,
    tmp_cbs;
  ARAD_IDR_ETHERNET_METER_CONFIG_TBL_DATA
    config_tbl_data;
  ARAD_IDR_ETHERNET_METER_PROFILES_TBL_DATA
    profile_tbl_data;
  uint8
    sw_db_enable_bit;
  uint32
    eth_mtr_config_val,
    eth_mtr_profile_val;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_PARAMS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(policer_info);

  SOC_PPC_MTR_BW_PROFILE_INFO_clear(policer_info);

  ARAD_CLEAR(&config_tbl_data,ARAD_IDR_ETHERNET_METER_CONFIG_TBL_DATA,1);
  ARAD_CLEAR(&profile_tbl_data,ARAD_IDR_ETHERNET_METER_PROFILES_TBL_DATA,1);

  clk_freq = arad_chip_ticks_per_sec_get(unit);

  status_tbl_offset = config_tbl_offset = port_ndx * SOC_PPC_NOF_MTR_ETH_TYPES + eth_type_ndx;

  
  res = READ_IDR_ETHERNET_METER_CONFIGm(unit, MEM_BLOCK_ANY, config_tbl_offset, &eth_mtr_config_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  config_tbl_data.profile = soc_mem_field32_get(unit, IDR_ETHERNET_METER_CONFIGm, &eth_mtr_config_val, PROFILEf);

  
  config_tbl_data.enable = soc_mem_field32_get(unit, IDR_ETHERNET_METER_CONFIGm, &eth_mtr_config_val, ENABLEf);

  res = sw_state_access[unit].dpp.soc.arad.pp.eth_policer_mtr_profile.config_meter_status.bit_get(
    unit,
    0,
    status_tbl_offset,
    &sw_db_enable_bit
  );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  policer_info->disable_cir = SOC_SAND_NUM2BOOL_INVERSE(sw_db_enable_bit);

  
  res = READ_IDR_ETHERNET_METER_PROFILESm(unit, MEM_BLOCK_ANY, config_tbl_data.profile, &eth_mtr_profile_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  profile_tbl_data.rate_mantissa = soc_mem_field32_get(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, RATE_MANTISSAf);
  profile_tbl_data.rate_exp = soc_mem_field32_get(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, RATE_EXPf);
  profile_tbl_data.burst_mantissa = soc_mem_field32_get(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, BURST_MANTISSAf);
  profile_tbl_data.burst_exp = soc_mem_field32_get(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, BURST_EXPf);
  profile_tbl_data.packet_mode = soc_mem_field32_get(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, PACKET_MODEf);
  profile_tbl_data.meter_resolution = soc_mem_field32_get(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, METER_RESOLUTIONf);
  if (SOC_IS_ARADPLUS(unit)) {
    profile_tbl_data.color_blind = soc_mem_field32_get(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, COLOR_BLINDf);
  }

  policer_info->is_packet_mode = SOC_SAND_NUM2BOOL(profile_tbl_data.packet_mode);
  policer_info->color_mode = profile_tbl_data.color_blind ? SOC_PPC_MTR_COLOR_MODE_BLIND : SOC_PPC_MTR_COLOR_MODE_AWARE;

  
  res = soc_sand_compute_complex_to_mnt_exp_reverse_exp(
      profile_tbl_data.rate_mantissa,
      profile_tbl_data.rate_exp,
      profile_tbl_data.meter_resolution,
      clk_freq,
      ARAD_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR,
      ARAD_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR,
      TRUE,
      &policer_info->cir
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_compute_complex_to_mnt_exp(
          profile_tbl_data.burst_mantissa,
          profile_tbl_data.burst_exp,
          ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MULTI_BS,
          ARAD_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MNT_INC_BS,
          &tmp_cbs
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  policer_info->cbs = (tmp_cbs >> profile_tbl_data.meter_resolution);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_params_get_unsafe()", port_ndx, 0);
}


uint32  
  arad_pp_mtr_eth_policer_glbl_profile_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      glbl_profile_idx,
    SOC_SAND_IN  SOC_PPC_MTR_BW_PROFILE_INFO *policer_info
  )
{
  uint32
    res = SOC_SAND_OK,
    reg_val = 0,
    global_enable;
  uint8
    main_enable,
    glbl_info_enable;
  ARAD_IDR_GLOBAL_METER_PROFILES_TBL_DATA
    glbl_tbl_data;
  uint32 color_blind = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(policer_info);

  ARAD_CLEAR(&glbl_tbl_data, ARAD_IDR_GLOBAL_METER_PROFILES_TBL_DATA, 1);

  
  res = arad_pp_mtr_profile_rate_to_res_exp_mnt(
      unit,
      policer_info->cir,
      &glbl_tbl_data.meter_resolution,
      &glbl_tbl_data.rate_mantissa,
      &glbl_tbl_data.rate_exp
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  arad_pp_mtr_profile_burst_to_exp_mnt(
        unit, 
        policer_info->cbs, 
        glbl_tbl_data.meter_resolution, 
        &glbl_tbl_data.burst_mantissa,
        &glbl_tbl_data.burst_exp
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  glbl_info_enable = SOC_SAND_BOOL2NUM_INVERSE(policer_info->disable_cir);
  
  
  res = sw_state_access[unit].dpp.soc.arad.pp.eth_policer_mtr_profile.config_meter_status.bit_get(
    unit,
    0,
    ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_ENABLE_STATUS_BIT_MAIN,
    &main_enable
  );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  if ((main_enable == TRUE) && (glbl_info_enable == TRUE) )
  {
    global_enable = TRUE;
  }
  else 
  {
    global_enable = FALSE;
  }
  soc_mem_field_set(unit, IDR_GLOBAL_METER_PROFILESm, &reg_val, ENABLEf, &global_enable);

  
  
  res = arad_pp_sw_db_eth_policer_config_status_bit_set(
      unit,
      0,
      ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_CONFIG_METER_PROFILE_NOF_MEMBER_BIT + glbl_profile_idx,
      glbl_info_enable
  );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  glbl_tbl_data.packet_mode = SOC_SAND_BOOL2NUM(policer_info->is_packet_mode);
  soc_mem_field_set(unit, IDR_GLOBAL_METER_PROFILESm, &reg_val, PACKET_MODEf, &glbl_tbl_data.packet_mode);
  soc_mem_field_set(unit, IDR_GLOBAL_METER_PROFILESm, &reg_val, RATE_EXPf, &glbl_tbl_data.rate_exp);
  soc_mem_field_set(unit, IDR_GLOBAL_METER_PROFILESm, &reg_val, RATE_MANTISSAf, &glbl_tbl_data.rate_mantissa);
  soc_mem_field_set(unit, IDR_GLOBAL_METER_PROFILESm, &reg_val, BURST_EXPf, &glbl_tbl_data.burst_exp);
  soc_mem_field_set(unit, IDR_GLOBAL_METER_PROFILESm, &reg_val, BURST_MANTISSAf, &glbl_tbl_data.burst_mantissa);
  soc_mem_field_set(unit, IDR_GLOBAL_METER_PROFILESm, &reg_val, METER_RESOLUTIONf, &glbl_tbl_data.meter_resolution);
  
  if (SOC_IS_ARADPLUS(unit)) {
    
    if (soc_property_get(unit, spn_RATE_COLOR_BLIND, 0) || policer_info->color_mode == SOC_PPC_MTR_COLOR_MODE_BLIND) {
      color_blind = 1;
    }
    soc_mem_field_set(unit, IDR_GLOBAL_METER_PROFILESm, &reg_val, COLOR_BLINDf, &color_blind);
  }

  res = WRITE_IDR_GLOBAL_METER_PROFILESm(unit, MEM_BLOCK_ANY, glbl_profile_idx, &reg_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_glbl_profile_set_unsafe()", 0, 0);
}

uint32  
  arad_pp_mtr_eth_policer_glbl_profile_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      glbl_profile_idx,
    SOC_SAND_IN  SOC_PPC_MTR_BW_PROFILE_INFO *policer_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(glbl_profile_idx, ARAD_PP_METERING_GLBL_PROFILE_IDX_MAX, ARAD_PP_METERING_GLBL_PROFILE_IDX_OUT_OF_RANGE_ERR, 10, exit);
  
  
  SOC_SAND_ERR_IF_BELOW_MIN(glbl_profile_idx, ARAD_PP_METERING_GLBL_PROFILE_IDX_MIN, ARAD_PP_METERING_GLBL_PROFILE_IDX_OUT_OF_RANGE_ERR, 20, exit);
  
  SOC_SAND_ERR_IF_ABOVE_MAX(policer_info->cir, ARAD_IF_MAX_RATE_KBPS(unit), ARAD_PP_METERING_CIR_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_BELOW_MIN(policer_info->cir, ARAD_PP_POLICER_PROFILE_ADD_VERIFY_IR_MIN, ARAD_PP_METERING_CIR_OUT_OF_RANGE_ERR, 40, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(policer_info->cbs, ARAD_PP_BW_PROFILE_ADD_VERIFY_BS_MAX, ARAD_PP_METERING_CBS_OUT_OF_RANGE_ERR, 50, exit);
  SOC_SAND_ERR_IF_BELOW_MIN(policer_info->cbs, ARAD_PP_BW_PROFILE_ADD_VERIFY_BS_MIN, ARAD_PP_METERING_CBS_OUT_OF_RANGE_ERR, 60, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_glbl_profile_set_verify()", 0, 0);
}

uint32  
  arad_pp_mtr_eth_policer_glbl_profile_get_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  glbl_profile_idx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(glbl_profile_idx, ARAD_PP_METERING_GLBL_PROFILE_IDX_MAX, ARAD_PP_METERING_GLBL_PROFILE_IDX_OUT_OF_RANGE_ERR, 10, exit);
  
  
  SOC_SAND_ERR_IF_BELOW_MIN(glbl_profile_idx, ARAD_PP_METERING_GLBL_PROFILE_IDX_MIN, ARAD_PP_METERING_GLBL_PROFILE_IDX_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_glbl_profile_get_verify()", 0, 0);
}


uint32  
  arad_pp_mtr_eth_policer_glbl_profile_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      glbl_profile_idx,
    SOC_SAND_OUT SOC_PPC_MTR_BW_PROFILE_INFO *policer_info
  )
{
  uint32
    res = SOC_SAND_OK,
    clk_freq,
    reg_val,
    tmp_cbs;
  ARAD_IDR_GLOBAL_METER_PROFILES_TBL_DATA
    global_tbl_data;
  uint8
    glbl_info_enable;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(policer_info);

  SOC_PPC_MTR_BW_PROFILE_INFO_clear(policer_info);
  ARAD_CLEAR(&global_tbl_data, ARAD_IDR_GLOBAL_METER_PROFILES_TBL_DATA, 1);

  clk_freq = arad_chip_ticks_per_sec_get(unit); 

  
  res = READ_IDR_GLOBAL_METER_PROFILESm(unit, MEM_BLOCK_ANY, glbl_profile_idx, &reg_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  soc_mem_field_get(unit, IDR_GLOBAL_METER_PROFILESm, &reg_val, RATE_MANTISSAf, &global_tbl_data.rate_mantissa);
  soc_mem_field_get(unit, IDR_GLOBAL_METER_PROFILESm, &reg_val, RATE_EXPf, &global_tbl_data.rate_exp);
  soc_mem_field_get(unit, IDR_GLOBAL_METER_PROFILESm, &reg_val, BURST_MANTISSAf, &global_tbl_data.burst_mantissa);
  soc_mem_field_get(unit, IDR_GLOBAL_METER_PROFILESm, &reg_val, BURST_EXPf, &global_tbl_data.burst_exp);
  soc_mem_field_get(unit, IDR_GLOBAL_METER_PROFILESm, &reg_val, PACKET_MODEf, &global_tbl_data.packet_mode);
  soc_mem_field_get(unit, IDR_GLOBAL_METER_PROFILESm, &reg_val, METER_RESOLUTIONf, &global_tbl_data.meter_resolution);
  if (SOC_IS_ARADPLUS(unit)) {
    soc_mem_field_get(unit, IDR_GLOBAL_METER_PROFILESm, &reg_val, COLOR_BLINDf, &global_tbl_data.color_blind);
  }

  
  res = soc_sand_compute_complex_to_mnt_exp_reverse_exp(
      global_tbl_data.rate_mantissa,
      global_tbl_data.rate_exp,
      global_tbl_data.meter_resolution,
      clk_freq,
      ARAD_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR,
      ARAD_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR,
      TRUE,
      &policer_info->cir
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = soc_sand_compute_complex_to_mnt_exp(
          global_tbl_data.burst_mantissa,
          global_tbl_data.burst_exp,
          ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MULTI_BS,
          ARAD_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MNT_INC_BS,
          &tmp_cbs
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  policer_info->cbs = (tmp_cbs >> global_tbl_data.meter_resolution);

  res = sw_state_access[unit].dpp.soc.arad.pp.eth_policer_mtr_profile.config_meter_status.bit_get(
      unit,
      0,
      ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_CONFIG_METER_PROFILE_NOF_MEMBER_BIT + glbl_profile_idx,
      &glbl_info_enable
  );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  policer_info->disable_cir = SOC_SAND_NUM2BOOL_INVERSE(glbl_info_enable);
  policer_info->is_packet_mode = SOC_SAND_NUM2BOOL(global_tbl_data.packet_mode);
  if (SOC_IS_ARADPLUS(unit)) {
    policer_info->color_mode = global_tbl_data.color_blind ? SOC_PPC_MTR_COLOR_MODE_BLIND : SOC_PPC_MTR_COLOR_MODE_AWARE;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_glbl_profile_get_unsafe()", 0, 0);
}


uint32  
  arad_pp_mtr_eth_policer_glbl_profile_map_set_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPC_PORT            port_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE    eth_type_ndx,
    SOC_SAND_IN  uint32                  glbl_profile_idx
  )
{
  uint32
    config_tbl_offset,
    res = SOC_SAND_OK,
    reg_val,
    glbl_mtr_ptr_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_UNSAFE);

  config_tbl_offset = port_ndx * SOC_PPC_NOF_MTR_ETH_TYPES + eth_type_ndx;

  
  res = READ_IDR_ETHERNET_METER_CONFIGm(unit, MEM_BLOCK_ANY, config_tbl_offset, &reg_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  glbl_mtr_ptr_val = glbl_profile_idx;
  soc_mem_field_set(unit, IDR_ETHERNET_METER_CONFIGm, &reg_val, GLOBAL_METER_PTRf, &glbl_mtr_ptr_val);

  res = WRITE_IDR_ETHERNET_METER_CONFIGm(unit, MEM_BLOCK_ANY, config_tbl_offset, &reg_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_glbl_profile_map_set_unsafe()", 0, 0);
}

uint32  
  arad_pp_mtr_eth_policer_glbl_profile_map_set_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPC_PORT            port_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE    eth_type_ndx,
    SOC_SAND_IN  uint32                  glbl_profile_idx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(eth_type_ndx, ARAD_PP_METERING_ETH_TYPE_NDX_MAX, ARAD_PP_METERING_ETH_TYPE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(glbl_profile_idx, ARAD_PP_METERING_GLBL_PROFILE_IDX_MAX, ARAD_PP_METERING_GLBL_PROFILE_IDX_OUT_OF_RANGE_ERR, 20, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_glbl_profile_map_set_verify()", 0, 0);
}

uint32  
  arad_pp_mtr_eth_policer_glbl_profile_map_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PPC_PORT                port_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE        eth_type_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(eth_type_ndx, ARAD_PP_METERING_ETH_TYPE_NDX_MAX, ARAD_PP_METERING_ETH_TYPE_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_glbl_profile_map_get_verify()", 0, 0);
}


uint32  
  arad_pp_mtr_eth_policer_glbl_profile_map_get_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPC_PORT            port_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE    eth_type_ndx,
    SOC_SAND_OUT uint32                  *glbl_profile_idx
  )
{
  uint32
    config_tbl_offset,
    res = SOC_SAND_OK,
    reg_val,
    glbl_mtr_ptr_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(glbl_profile_idx);

  config_tbl_offset = port_ndx * SOC_PPC_NOF_MTR_ETH_TYPES + eth_type_ndx;

  
  res = READ_IDR_ETHERNET_METER_CONFIGm(unit, MEM_BLOCK_ANY, config_tbl_offset, &reg_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  soc_mem_field_get(unit, IDR_ETHERNET_METER_CONFIGm, &reg_val, GLOBAL_METER_PTRf, &glbl_mtr_ptr_val);
  *glbl_profile_idx = glbl_mtr_ptr_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_glbl_profile_map_get_unsafe()", 0, 0);
}


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_metering_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_metering;
}

CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_metering_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_metering;
}

uint32
  SOC_PPC_MTR_METER_ID_verify(
    SOC_SAND_IN  SOC_PPC_MTR_METER_ID *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->group, ARAD_PP_METERING_GROUP_MAX, ARAD_PP_METERING_GROUP_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->id, ARAD_PP_METERING_ID_MAX, ARAD_PP_METERING_ID_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_MTR_METER_ID_verify()",0,0);
}


uint32
  SOC_PPC_MTR_GROUP_INFO_verify(
    SOC_SAND_IN  SOC_PPC_MTR_GROUP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_MTR_GROUP_INFO_verify()",0,0);
}

uint32
  SOC_PPC_MTR_BW_PROFILE_INFO_verify(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PPC_MTR_BW_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->cir, ARAD_IF_MAX_RATE_KBPS(unit), ARAD_PP_METERING_CIR_OUT_OF_RANGE_ERR, 10, exit);
  if(info->disable_cir == FALSE)
  {
      SOC_SAND_ERR_IF_BELOW_MIN(info->cir, SOC_DPP_CONFIG(unit)->meter.meter_min_rate, ARAD_PP_METERING_CIR_OUT_OF_RANGE_ERR, 20, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->eir, ARAD_IF_MAX_RATE_KBPS(unit), ARAD_PP_METERING_EIR_OUT_OF_RANGE_ERR, 30, exit);
  if(info->disable_eir == FALSE)
  {
      SOC_SAND_ERR_IF_BELOW_MIN(info->eir, SOC_DPP_CONFIG(unit)->meter.meter_min_rate, ARAD_PP_METERING_EIR_OUT_OF_RANGE_ERR, 40, exit);  
  }  
  if (info->is_sharing_enabled && (info->max_cir != SOC_PPC_BW_PROFILE_IR_MAX_UNLIMITED) && (info->max_cir != 0)) {
      SOC_SAND_ERR_IF_ABOVE_MAX(info->max_cir, ARAD_IF_MAX_RATE_KBPS(unit), ARAD_PP_METERING_CIR_OUT_OF_RANGE_ERR, 15, exit);
      SOC_SAND_ERR_IF_BELOW_MIN(info->max_cir, SOC_DPP_CONFIG(unit)->meter.meter_min_rate, ARAD_PP_METERING_CIR_OUT_OF_RANGE_ERR, 25, exit);
  }

  if ((info->max_eir != SOC_PPC_BW_PROFILE_IR_MAX_UNLIMITED) && (info->max_eir != 0)) {
      SOC_SAND_ERR_IF_ABOVE_MAX(info->max_eir, ARAD_IF_MAX_RATE_KBPS(unit), ARAD_PP_METERING_EIR_OUT_OF_RANGE_ERR, 35, exit);
      SOC_SAND_ERR_IF_BELOW_MIN(info->max_eir, SOC_DPP_CONFIG(unit)->meter.meter_min_rate, ARAD_PP_METERING_EIR_OUT_OF_RANGE_ERR, 45, exit);
  }

  if ((info->eir > info->max_eir) && (info->disable_eir == FALSE)) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_METERING_MAX_IR_INVALID_ERR, 95, exit);
  }
  if ((info->is_sharing_enabled) && (info->cir > info->max_cir) && (info->disable_cir == FALSE)) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_METERING_MAX_IR_INVALID_ERR, 100, exit);
  }

  SOC_SAND_ERR_IF_ABOVE_MAX(info->cbs, ARAD_PP_BW_PROFILE_ADD_VERIFY_BS_MAX, ARAD_PP_METERING_CBS_OUT_OF_RANGE_ERR, 50, exit);
  SOC_SAND_ERR_IF_BELOW_MIN(info->cbs, ARAD_PP_BW_PROFILE_ADD_VERIFY_BS_MIN, ARAD_PP_METERING_CBS_OUT_OF_RANGE_ERR, 60, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->ebs, ARAD_PP_BW_PROFILE_ADD_VERIFY_BS_MAX, ARAD_PP_METERING_EBS_OUT_OF_RANGE_ERR, 70, exit);
  SOC_SAND_ERR_IF_BELOW_MIN(info->ebs, ARAD_PP_BW_PROFILE_ADD_VERIFY_BS_MIN, ARAD_PP_METERING_EBS_OUT_OF_RANGE_ERR, 80, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->color_mode, ARAD_PP_METERING_COLOR_MODE_MAX, ARAD_PP_METERING_COLOR_MODE_OUT_OF_RANGE_ERR, 90, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_MTR_BW_PROFILE_INFO_verify()",0,0);
}

soc_error_t
  arad_pp_mtr_policer_global_sharing_get(
    int                         unit,
    int                         core_id,
    int                         meter_id,
    int                         meter_group,
    uint32*                     global_sharing_ptr
)
{
    unsigned int            soc_sand_dev_id;
    uint32                  soc_sand_rv;
    uint32                  mem_val[2];
    int mem,array_index,index;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(global_sharing_ptr);

    soc_sand_dev_id = (unit);

    
    mem = (meter_group == 0) ? IDR_MCDA_PRFSELm : IDR_MCDB_PRFSELm;

    if (meter_id < 16384) {
        array_index = 0;
        index = meter_id / 4;
    } else {
        array_index = 1;
        index = (meter_id - 16384) / 4;
    }

    
    soc_sand_rv = soc_mem_array_read(soc_sand_dev_id, mem, array_index, MEM_BLOCK_ANY, index, mem_val);
    SOCDNX_IF_ERR_EXIT(soc_sand_rv);

    
    soc_mem_field_get(soc_sand_dev_id, mem, mem_val, GLOBAL_SHARINGf, global_sharing_ptr);
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  arad_pp_mtr_policer_global_sharing_set(
    int                         unit,
    int                         core_id,
    int                         meter_id,
    int                         meter_group,
    uint32*                     global_sharing_ptr
)
{
    unsigned int            soc_sand_dev_id;
    uint32                  soc_sand_rv;
    uint32                  mem_val[2];
    int mem,array_index,index;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(global_sharing_ptr);

    soc_sand_dev_id = (unit);

    
    mem = (meter_group == 0) ? IDR_MCDA_PRFSELm : IDR_MCDB_PRFSELm;

    if (meter_id < 16384) {
        array_index = 0;
        index = meter_id / 4;
    } else {
        array_index = 1;
        index = (meter_id - 16384) / 4;
    }

    
    soc_sand_rv = soc_mem_array_read(soc_sand_dev_id, mem, array_index, MEM_BLOCK_ANY, index, mem_val); 
    SOCDNX_IF_ERR_EXIT(soc_sand_rv);

    
    soc_mem_field_set(soc_sand_dev_id, mem, mem_val, GLOBAL_SHARINGf, global_sharing_ptr);

    soc_sand_rv = soc_mem_array_write(soc_sand_dev_id, mem, array_index, MEM_BLOCK_ANY, index, mem_val); 
    SOCDNX_IF_ERR_EXIT(soc_sand_rv);
exit:
    SOCDNX_FUNC_RETURN;
}



uint32
  arad_pp_meter_tc_map_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                   tc,
    SOC_SAND_IN  uint32                   meter_tc
  )
{
    uint32    reg_val;
    uint32   res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, READ_IHP_METER_TRAFFIC_CLASS_BITMAPr(unit, REG_PORT_ANY, &reg_val));

    reg_val &= ~(ARAD_PP_METER_TC_MASK << (tc*ARAD_PP_METER_TC_SIZE));
    reg_val |= meter_tc << (tc*ARAD_PP_METER_TC_SIZE);
    soc_reg_field_set(unit, IHP_METER_TRAFFIC_CLASS_BITMAPr, &reg_val, METER_TRAFFIC_CLASS_BITMAPf, reg_val);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, WRITE_IHP_METER_TRAFFIC_CLASS_BITMAPr(unit, REG_PORT_ANY, reg_val));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_meter_tc_map_set()",0, 0);
}


uint32
  arad_pp_meter_tc_map_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                               tc,
    SOC_SAND_OUT uint32                              *meter_tc
  )
{
    uint32    reg_val;
    uint32    fld_val;
    uint32    res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(meter_tc);
    *meter_tc =0;

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_METER_TRAFFIC_CLASS_BITMAPr(unit, REG_PORT_ANY, &reg_val));
    fld_val = soc_reg_field_get(unit, IHP_METER_TRAFFIC_CLASS_BITMAPr, reg_val, METER_TRAFFIC_CLASS_BITMAPf);

    *meter_tc = (fld_val & (ARAD_PP_METER_TC_MASK << (tc*ARAD_PP_METER_TC_SIZE))) >> (tc*ARAD_PP_METER_TC_SIZE);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_meter_tc_map_get()",0, 0);
}


soc_error_t
arad_pp_mtr_none_ethernet_packet_ptr_set(
    int                         unit,
    int*                        arg)
{
    uint64 reg64_val;
    uint32 field32_val;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(arg);

    SOCDNX_IF_ERR_EXIT(READ_IHB_LBP_GENERAL_CONFIG_0r(unit, SOC_CORE_ALL, &reg64_val));
    field32_val = (*arg);
    soc_reg64_field32_set(unit, IHB_LBP_GENERAL_CONFIG_0r, &reg64_val, NON_ETHERNET_METER_POINTERf, field32_val);
    SOCDNX_IF_ERR_EXIT(WRITE_IHB_LBP_GENERAL_CONFIG_0r(unit, SOC_CORE_ALL, reg64_val));
  
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
arad_pp_mtr_none_ethernet_packet_ptr_get(
    int                         unit,
    int*                        arg)
{
    uint64 reg64_val;
    uint32 field32_val;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(arg);

    SOCDNX_IF_ERR_EXIT(READ_IHB_LBP_GENERAL_CONFIG_0r(unit, SOC_CORE_ALL, &reg64_val));
    field32_val = soc_reg64_field32_get(unit, IHB_LBP_GENERAL_CONFIG_0r, reg64_val, NON_ETHERNET_METER_POINTERf);

    *arg = field32_val;
    
exit:
    SOCDNX_FUNC_RETURN;
}






#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 

