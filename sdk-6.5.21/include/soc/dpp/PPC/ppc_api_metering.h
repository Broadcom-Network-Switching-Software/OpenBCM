/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPC_API_METERING_INCLUDED__

#define __SOC_PPC_API_METERING_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>
#include <bcm/policer.h>







#define  SOC_PPC_MTR_ASSIGN_NO_METER (0xFFFFFFFF)
#define SOC_PPC_BW_PROFILE_IR_MAX_UNLIMITED 0xffffffff
#define SOC_PPC_BW_PROFILE_MAX_NUMBER_OF_RATES (24)
#define ARAD_PP_INVALID_IR_REV_EXP (0xFFFFFFFF)


 
#define  SOC_PPC_MTR_RES_USE_GET_BY_TYPE(_ingress, _egress) \
  ((_ingress) && (_egress)) ? SOC_PPC_MTR_RES_USE_OW_DP_DE : \
  (((_ingress) && !(_egress)) ? SOC_PPC_MTR_RES_USE_OW_DP: \
  ((!(_ingress) && (_egress)) ? SOC_PPC_MTR_RES_USE_OW_DE : \
  SOC_PPC_MTR_RES_USE_NONE))









typedef enum
{
  
  SOC_PPC_MTR_ETH_TYPE_UNKNOW_UC = 0,
  
  SOC_PPC_MTR_ETH_TYPE_KNOW_UC = 1,
  
  SOC_PPC_MTR_ETH_TYPE_UNKNOW_MC = 2,
  
  SOC_PPC_MTR_ETH_TYPE_KNOW_MC = 3,
  
  SOC_PPC_MTR_ETH_TYPE_BC = 4,
  
  SOC_PPC_NOF_MTR_ETH_TYPES = 5
}SOC_PPC_MTR_ETH_TYPE;

typedef enum
{
  
  SOC_PPC_MTR_COLOR_MODE_BLIND = 0,
  
  SOC_PPC_MTR_COLOR_MODE_AWARE = 1,
  
  SOC_PPC_NOF_MTR_COLOR_MODES = 2
}SOC_PPC_MTR_COLOR_MODE;

typedef enum
{
  
  SOC_PPC_MTR_SHARING_MODE_NONE = 0,
  
  SOC_PPC_MTR_SHARING_MODE_SERIAL = 1,
  
  SOC_PPC_MTR_SHARING_MODE_PARALLEL = 2,
  
  SOC_PPC_NOF_MTR_SHARING_MODES = 3
}SOC_PPC_MTR_SHARING_MODE;

typedef enum
{
  
  SOC_PPC_MTR_COLOR_RESOLUTION_MODE_DEFAULT = 0,
   
  SOC_PPC_MTR_COLOR_RESOLUTION_MODE_RED_DIFF = 1,
  
  SOC_PPC_NOF_MTR_COLOR_RESOLUTION_MODES = 2
}SOC_PPC_MTR_COLOR_RESOLUTION_MODE;

typedef enum
{
  
  SOC_PPC_MTR_RES_USE_OW_DP_DE = 0,
  
  SOC_PPC_MTR_RES_USE_OW_DP = 1,
  
  SOC_PPC_MTR_RES_USE_OW_DE = 2,
  
  SOC_PPC_MTR_RES_USE_NONE = 3,
  
  SOC_PPC_NOF_MTR_RES_USES = 4
}SOC_PPC_MTR_RES_USE;

typedef enum
{
  
  SOC_PPC_MTR_TYPE_INLIF = 0,
  
  SOC_PPC_MTR_TYPE_OUTLIF = 1,
  
  SOC_PPC_MTR_TYPE_IN_AC = 2,
  
  SOC_PPC_MTR_TYPE_OUT_AC = 3,
  
  SOC_PPC_MTR_TYPE_VSID = 4,
  
  SOC_PPC_MTR_TYPE_IN_PWE = 5,
  
  SOC_PPC_MTR_TYPE_IN_LABEL = 6,
  
  SOC_PPC_MTR_TYPE_OUT_LABEL = 7,
  
  SOC_PPC_MTR_TYPE_FEC_ID = 8,
  
  SOC_PPC_MTR_TYPE_IN_AC_AND_TC = 9,
  
  SOC_PPC_NOF_MTR_TYPES = 10
}SOC_PPC_MTR_TYPE;

typedef enum
{
  
  SOC_PPC_MTR_IPG_COMPENSATION_DISABLED_SIZE = 0,
  
  SOC_PPC_MTR_IPG_COMPENSATION_ENABLED_SIZE = 20,
  
  SOC_PPC_NOF_MTR_IPG_COMPENSATION_SIZE = 2
}SOC_PPC_MTR_IPG_COMPENSATION_SIZE_TYPE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 group;
  
  uint32 id;

} SOC_PPC_MTR_METER_ID;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 is_hr_enabled;
  
  uint32 max_packet_size;
  
  uint8 rand_mode_enable;

} SOC_PPC_MTR_GROUP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 disable_cir;
   
  uint8 disable_eir;
  
  uint32 cir;
  
  uint32 eir;
  
  uint32 max_cir;
  
  uint32 max_eir;
  
  uint32 cbs;
  
  uint32 ebs;
  

  SOC_PPC_MTR_COLOR_MODE color_mode;
  
  uint8 is_fairness_enabled;
  
  uint8 is_coupling_enabled;
  
  uint8 is_sharing_enabled;

  
  uint8 is_mtr_enabled;

  
  uint8 is_packet_mode;

    
  uint8 is_pkt_truncate;


  uint32 ir_rev_exp;

  
  uint32 mark_black; 
  
} SOC_PPC_MTR_BW_PROFILE_INFO;

typedef struct
{
	SOC_SAND_MAGIC_NUM_VAR

	
	uint32 flags;

	
    bcm_policer_color_t policer0_color; 

	
    uint32 policer0_other_bucket_has_credits; 

	
    bcm_policer_color_t policer1_color;

	
    uint32 policer1_other_bucket_has_credits;

    
    uint32 policer0_mark_black; 
    uint32 policer1_mark_black; 

	
    bcm_policer_color_t policer0_update_bucket; 

	
    bcm_policer_color_t policer1_update_bucket;

	
    bcm_color_t policer_color;          

} SOC_PPC_MTR_COLOR_DECISION_INFO;

typedef struct
{
	SOC_SAND_MAGIC_NUM_VAR

	
	uint32 flags;
	
	              
    uint32 policer_action;
	
	         
    bcm_color_t incoming_color;
	
	 
    bcm_color_t ethernet_policer_color;

	
    bcm_color_t policer_color;

	
    bcm_color_t ingress_color;

	
    bcm_color_t egress_color;

} SOC_PPC_MTR_COLOR_RESOLUTION_INFO;










void
  SOC_PPC_MTR_METER_ID_clear(
    SOC_SAND_OUT SOC_PPC_MTR_METER_ID *info
  );

void
  SOC_PPC_MTR_GROUP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_MTR_GROUP_INFO *info
  );

void
  SOC_PPC_MTR_BW_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_MTR_BW_PROFILE_INFO *info
  );

void
  SOC_PPC_MTR_COLOR_DECISION_INFO_clear(
	SOC_SAND_OUT SOC_PPC_MTR_COLOR_DECISION_INFO *info
  );

void
  SOC_PPC_MTR_COLOR_RESOLUTION_INFO_clear(
	SOC_SAND_OUT SOC_PPC_MTR_COLOR_RESOLUTION_INFO *info
  );


#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_MTR_ETH_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE enum_val
  );

const char*
  SOC_PPC_MTR_COLOR_MODE_to_string(
    SOC_SAND_IN  SOC_PPC_MTR_COLOR_MODE enum_val
  );

const char*
  SOC_PPC_MTR_RES_USE_to_string(
    SOC_SAND_IN  SOC_PPC_MTR_RES_USE enum_val
  );

const char*
  SOC_PPC_MTR_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_MTR_TYPE enum_val
  );

const char*
  SOC_PPC_MTR_MODE_to_string(
    SOC_SAND_IN  bcm_policer_mode_t enum_val
  );

void
  SOC_PPC_MTR_METER_ID_print(
    SOC_SAND_IN  SOC_PPC_MTR_METER_ID *info
  );

void
  SOC_PPC_MTR_GROUP_INFO_print(
    SOC_SAND_IN  SOC_PPC_MTR_GROUP_INFO *info
  );

void
  SOC_PPC_MTR_BW_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PPC_MTR_BW_PROFILE_INFO *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

