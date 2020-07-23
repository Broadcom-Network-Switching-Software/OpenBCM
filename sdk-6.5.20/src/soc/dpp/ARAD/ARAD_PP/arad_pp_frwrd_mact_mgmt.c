#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FORWARD

#include <soc/mem.h>



#include <shared/bsl.h>
#include <sal/core/dpc.h>
#include <soc/dcmn/error.h>
#include <soc/dcmn/dcmn_dev_feature_manager.h>
#include <soc/dpp/JER/jer_mgmt.h>

#include <soc/dpp/drv.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_bmact.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_mgmt.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact_mgmt.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>


#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_reg_access.h>

#include <soc/dpp/mbcm.h>
#include <shared/swstate/access/sw_state_access.h>





#define ARAD_PP_FRWRD_MACT_FID_PROFILE_NDX_MIN              (0)
#define ARAD_PP_FRWRD_MACT_FID_PROFILE_NDX_MAX              (6)
#define ARAD_PP_FRWRD_MACT_MAC_LEARN_PROFILE_NDX_MAX        (7)
#define ARAD_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_MAX         (3)
#define ARAD_PP_FRWRD_MACT_FID_AGING_PROFILE_MAX            (3)
#define SOC_PPC_FRWRD_MACT_TRAP_TYPE_NDX_MAX                (SOC_PPC_NOF_FRWRD_MACT_TRAP_TYPES-1)
#define ARAD_PP_FRWRD_MACT_PORT_PROFILE_NDX_MAX             (3)
#define SOC_PPC_FRWRD_MACT_EVENT_TYPE_MAX                   ((1 << (SOC_PPC_NOF_FRWRD_MACT_EVENT_TYPES-1))-1)
#define ARAD_PP_FRWRD_MACT_HEADER_TYPE_MAX                  (SOC_PPC_NOF_FRWRD_MACT_MSG_HDR_TYPES) 
#define ARAD_PP_FRWRD_MACT_TYPE_MAX                         (SOC_PPC_NOF_FRWRD_MACT_EVENT_PATH_TYPES-1)
#define SOC_PPC_FRWRD_MACT_LEARNING_MODE_MAX                (SOC_PPC_NOF_FRWRD_MACT_LEARNING_MODES-1)
#define SOC_PPC_FRWRD_MACT_SHADOW_MODE_MAX                  (SOC_PPC_NOF_FRWRD_MACT_SHADOW_MODES-1)
#define ARAD_PP_FRWRD_MACT_ACTION_WHEN_EXCEED_MAX           (SOC_PPC_NOF_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPES-1)
#define ARAD_PP_FRWRD_MACT_SA_DROP_ACTION_PROFILE_MAX       (3)
#define ARAD_PP_FRWRD_MACT_SA_UNKNOWN_ACTION_PROFILE_MAX    (3)
#define ARAD_PP_FRWRD_MACT_DA_UNKNOWN_ACTION_PROFILE_MAX    (3)
#define ARAD_PP_FRWRD_MACT_ID_MAX                           (255)

#define ARAD_PP_FRWRD_MACT_MGMT_BUFF_MAX                         (SOC_SAND_U32_MAX)
#define ARAD_PP_FRWRD_MACT_MGMT_BUFF_LEN_MAX                     (SOC_SAND_U32_MAX)

#define ARAD_PP_FRWRD_MACT_FID_FAIL_MAX                     (0x7ffe)

#define SOC_PPC_FRWRD_MACT_TIME_SEC_MAX                               (687)

#define SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY_NOF_IS_LAGS                          (2)

#define ARAD_PP_FRWRD_MACT_16_CLOCKS_RESOLUTION                            (16)
#define ARAD_PP_FRWRD_MACT_AGE_CONF_PTR_FOR_MAC                       (0x1)
#define ARAD_PP_FRWRD_MACT_AGE_NOF_BITS                               (SOC_IS_JERICHO(unit)? 5:4)
#define ARAD_PP_FRWRD_MACT_NOF_VSI_AGING_PROFILES                                    (4)
#define ARAD_PP_FRWRD_MACT_NOF_VSI_AGING_DEFAULT_PROFILES        (2)

#define ARAD_PP_FRWRD_MACT_AGE_AGING_DYNAMIC_CONF_OFF (16) 
#define ARAD_PP_FRWRD_MACT_AGE_AGING_CONF_SIZE        (4)  


#define ARAD_PP_FRWRD_MACT_LEM_DB_MACT_ID(unit)                     (ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_ETH(unit))

#define ARAD_PP_FRWRD_MACT_SA_LOOKUP_TYPE_AUTHENTIFICATION_FLD_VAL         (0x1)
#define ARAD_PP_FRWRD_MACT_SA_LOOKUP_TYPE_SA_LOOKUP_FLD_VAL                (0x2)
#define ARAD_PP_FRWRD_MACT_SA_LOOKUP_TYPE_SLB_LOOKUP_FLD_VAL               (0x3)

#define ARAD_PP_FRWRD_MACT_LOOKUP_MODE_TRANSPLANT_FLD_VAL                  (0x2)
#define ARAD_PP_FRWRD_MACT_LOOKUP_MODE_TRANSPLANT_AND_REFRESH_FLD_VAL      (0x3)

#define ARAD_PP_FRWRD_MACT_FIFO_FULL_THRESHOLD                             (127)


#define ARAD_PP_FRWRD_MACT_MESSAGE_HEADER_SIZE                                   (8)

#define ARAD_PP_FRWRD_MACT_ETHERNET_TYPE_SIZE                                    (2)

#define ARAD_PP_FRWRD_MACT_ETHERNET_HEADER_SIZE                                  (14)

#define ARAD_PP_FRWRD_MACT_ITMH_HEADER_SIZE                                      (4)
#define ARAD_PP_FRWRD_MACT_OTMH_HEADER_SIZE                                      (2)

#define ARAD_PP_FRWRD_MACT_DSP_HEADER_SIZE                                       (8)
#define ARAD_PP_FRWRD_MACT_DSP_NOF_DSP_MSGS_IN_OLP_MSG_DISABLED                  (0)
#define ARAD_PP_FRWRD_MACT_DSP_NOF_DSP_MSGS_IN_OLP_MSG_SINGLE                    (1)
#define ARAD_PP_FRWRD_MACT_DSP_NOF_DSP_MSGS_IN_OLP_MSG_AGGR                      (8)
#define ARAD_PP_FRWRD_MACT_DSP_MSG_MIN_SIZE                                      (64)

#define  ARAD_PP_FRWRD_MACT_MESSAGE_DSP_TYPE                                      (0XAB00)

#define  ARAD_PP_FRWRD_MACT_LEARN_FILTER_TTL_LOOPBACK                       (12)
#define  ARAD_PP_FRWRD_MACT_LEARN_FILTER_TTL_MSGS                           (48)


#define  ARAD_PP_FRWRD_MACT_LEARN_LIMIT_DFLT_PROFILE_ID (0)

#define ARAD_PP_FRWRD_MACT_AGING_HIGH_RES_DISABLED  \
  (!(arad_pp_frwrd_mact_mgmt_is_b0_high_resolution(unit)))

#define SOC_PPC_FRWRD_MACT_LEARN_MSG_EVENT_LEN        18
#define SOC_PPC_FRWRD_MACT_LEARN_MSG_NUM_OF_EVENTS    8

#define SOC_PPC_FRWRD_MACT_LEARN_MSG_MAX_LEN         ((SOC_PPC_FRWRD_MACT_LEARN_MSG_EVENT_LEN) * SOC_PPC_FRWRD_MACT_LEARN_MSG_NUM_OF_EVENTS + ARAD_PP_FRWRD_MACT_ETHERNET_TYPE_SIZE) 

#define SOC_PPC_FRWRD_MACT_LEARN_MSG_VALID_INDEX    0
#define SOC_PPC_FRWRD_MACT_LEARN_MSG_VALID_LEN      1
#define SOC_PPC_FRWRD_MACT_LEARN_MSG_TYPE_INDEX     1
#define SOC_PPC_FRWRD_MACT_LEARN_MSG_TYPE_LEN       3
#define SOC_PPC_FRWRD_MACT_LEARN_MSG_KEY_1_INDEX      19 
#define SOC_PPC_FRWRD_MACT_LEARN_MSG_KEY_1_LEN        10
#define SOC_PPC_FRWRD_MACT_LEARN_MSG_KEY_2_INDEX      29 
#define SOC_PPC_FRWRD_MACT_LEARN_MSG_KEY_2_LEN        32
#define SOC_PPC_FRWRD_MACT_LEARN_MSG_KEY_3_INDEX      61 
#define SOC_PPC_FRWRD_MACT_LEARN_MSG_KEY_3_LEN        32
#define SOC_PPC_FRWRD_MACT_LEARN_MSG_ASD_1_INDEX      97
#define SOC_PPC_FRWRD_MACT_LEARN_MSG_ASD_1_LEN        32
#define SOC_PPC_FRWRD_MACT_LEARN_MSG_ASD_2_INDEX      129
#define SOC_PPC_FRWRD_MACT_LEARN_MSG_ASD_2_LEN        10
#define SOC_PPC_FRWRD_MACT_LEARN_MSG_ASD_3_INDEX      139
#define SOC_PPC_FRWRD_MACT_LEARN_MSG_ASD_3_LEN        1
#define SOC_PPC_FRWRD_MACT_LEARN_MSG_MFF_IS_KEY_INDEX   15
#define SOC_PPC_FRWRD_MACT_LEARN_MSG_MFF_IS_KEY_LEN     1
#define SOC_PPC_FRWRD_MACT_LEARN_MSG_LAG_INDEX          5
#define SOC_PPC_FRWRD_MACT_LEARN_MSG_LAG_LEN            10
#define SOC_PPC_FRWRD_MACT_LEARN_MSG_IS_LAG_INDEX          4
#define SOC_PPC_FRWRD_MACT_LEARN_MSG_IS_LAG_LEN            1

#define ARAD_PP_MACT_DSP_ENGINE_CONFIGURATION_REG_NOF_REGS (2)



#define ARAD_PP_FRWRD_MACT_LIMIT_RANGE_MAP_ZERO_MSBS_MAX    (31)
#define ARAD_PP_FRWRD_MACT_LIMIT_RANGE_MAP_SHIFT_RIGHT_MAX  (31)

#define SOC_DPP_FRWRD_MACT_LIMIT_INV_MAP_PTR_MAX(unit)      ((1 << SOC_DPP_IMP_DEFS_GET(unit, mact_lif_base_length)) - 1)
#define ARAD_PP_FRWRD_MACT_LIMIT_RANGE_END_MAX              (0x7FFF)


#define ARAD_PP_FRWRD_MACT_LIMIT_RANGE_MAP_SIZE             (0x4000)


#define ARAD_PP_FRWRD_MACT_NOF_ROUTED_LEARNING_APPS         (5)


#define SOC_PPC_OUTLIF_BASE                                 (0x80)

#define LIF_MASK(unit)    (SOC_DPP_DEFS_GET(unit, nof_global_lifs) - 1)

#define RANGE_MAP_SIZE    (SOC_IS_JERICHO(unit)? 27 : 25)




#define _ARAD_PP_LEARNING_SIZE_OF_HOST_BUFFER_FOR_EVENT_FIFO_USED_BY_DMA 133
#define _ARAD_PP_LEARNING_SIZE_OF_EVENT_IN_BYTES ((_ARAD_PP_LEARNING_SIZE_OF_HOST_BUFFER_FOR_EVENT_FIFO_USED_BY_DMA + 7) >> 3)
#define _ARAD_PP_LEARNING_SIZE_OF_EVENT_IN_WORDS ((_ARAD_PP_LEARNING_SIZE_OF_EVENT_IN_BYTES + 3) >> 2)

#define LARGEST_NUMBER_OF_ENTRIES_FOR_DMA_HOST_MEMORY 0x4000
#define SMALLEST_NUMBER_OF_ENTRIES_FOR_DMA_HOST_MEMORY 0x20


#define ROUND_UP_TO_NEAREST_POWER_OF_2(N) \
    --N;  N |= N>>1 ; N |= N>>2 ; N|=N>>4  ; \
     N |= N>>8 ; N |= N>>16; ++N;

#define NUM_OF_EVENT_ROUTE_PROFILES 8


static int g_next_dma_interrupt_message_num[SOC_MAX_NUM_DEVICES] ; 
static uint8 *g_dma_host_memory[SOC_MAX_NUM_DEVICES]; 











typedef enum
{
  
  ARAD_PP_FRWRD_MACT_FIFO_LEARNING = 0,
  
  ARAD_PP_FRWRD_MACT_FIFO_SHADOW = 1,
 
  ARAD_PP_FRWRD_MACT_NOF_FIFOS = 2
}ARAD_PP_FRWRD_MACT_FIFO;

typedef enum
{
 
  ARAD_PP_FRWRD_MACT_POSITION_LOOPBACK = 0,
 
  ARAD_PP_FRWRD_MACT_POSITION_LEARNING = 1,
 
  ARAD_PP_FRWRD_MACT_POSITION_SHADOW = 2,
 
  ARAD_PP_FRWRD_MACT_NOF_POSITIONS = 3
}ARAD_PP_FRWRD_MACT_POSITION;


typedef struct
{
   soc_reg_t reg_ref;
   soc_field_t field_ref;
} ARAD_PP_FRWRD_MACT_REG_FIELD_REF;

typedef struct
{
   soc_field_t field_ref;
   uint8 learn_enable;
} ARAD_PP_FRWRD_MACT_SA_LOOKUP_MAPPING_REG_FIELD_REF;

typedef struct
{
  int32 app_type;
  int32 fwd_code;
} ARAD_PP_FRWRD_MACT_APP_TO_FLP_PROGS;




VOLATILE sal_sem_t     jer_mact_fifo_sem[SOC_SAND_MAX_DEVICE];
VOLATILE sal_thread_t  jer_mact_fifo_tid[SOC_SAND_MAX_DEVICE];
VOLATILE uint8         jer_mact_fifo_terminate[SOC_SAND_MAX_DEVICE];

CONST STATIC
  SOC_PROCEDURE_DESC_ELEMENT
    Arad_pp_procedure_desc_element_frwrd_mact_mgmt[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_OPER_MODE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_OPER_MODE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_OPER_MODE_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_OPER_MODE_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_OPER_MODE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_OPER_MODE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_OPER_MODE_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_OPER_MODE_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_AGING_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_ONE_PASS_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_AGING_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_AGING_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_AGING_ONE_PASS_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_AGING_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_AGING_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_AGING_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_AGING_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_AGING_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_EVENT_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_EVENT_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_EVENT_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_EVENT_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_PORT_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_PORT_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_PORT_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_PORT_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_PORT_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_PORT_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_PORT_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_PORT_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_TRAP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_TRAP_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_TRAP_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_TRAP_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_TRAP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_TRAP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_TRAP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_TRAP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_EVENT_PARSE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_EVENT_PARSE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_EVENT_PARSE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_EVENT_PARSE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_MGMT_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_MGMT_GET_ERRS_PTR),
  

   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_MAX_AGE_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_AGE_CONF_DEFAULT_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_AGE_CONF_WRITE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_AGE_MODIFY_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_AGE_CONF_READ),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_EVENT_KEY_INDEX_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_LEARN_MSGS_OLP_MSG_SET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_LEARN_MSGS_OLP_MSG_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_VERIFY),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_INIT),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_REGS_INIT),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_MACT_IS_INGRESS_LEARNING_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_LEARN_MSG_CONF_GET),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_LEARN_MSG_PARSE),
   SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_MACT_LEARN_MSG_PARSE_UNSAFE),

  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC
SOC_ERROR_DESC_ELEMENT
    Arad_pp_error_desc_element_frwrd_mact_mgmt[] =
{
  
  {
    ARAD_PP_FRWRD_MACT_FID_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_MACT_FID_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'fid_profile_ndx' is out of range. \n\r "
    "The range is: 0 - 6.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_MAC_LEARN_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_MACT_MAC_LEARN_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'mac_learn_profile_ndx' is out of range. \n\r "
    "The range is: No min - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'event_handle_profile' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_FRWRD_MACT_TRAP_TYPE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PPC_FRWRD_MACT_TRAP_TYPE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'trap_type_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_FRWRD_MACT_TRAP_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_MACT_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'port_profile_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_FRWRD_MACT_TRAP_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_FRWRD_MACT_EVENT_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PPC_FRWRD_MACT_EVENT_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'event_type' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_FRWRD_MACT_EVENT_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_VSI_EVENT_HANDLE_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_MACT_VSI_EVENT_HANDLE_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'vsi_event_handle_profile' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_HEADER_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_MACT_HEADER_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'header_type' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_FRWRD_MACT_MSG_HDR_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_MGMT_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_MACT_MGMT_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'type' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_FRWRD_MACT_EVENT_PATH_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_FRWRD_MACT_LEARNING_MODE_OUT_OF_RANGE_ERR,
    "SOC_PPC_FRWRD_MACT_LEARNING_MODE_OUT_OF_RANGE_ERR",
    "The parameter 'learning_mode' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_FRWRD_MACT_LEARNING_MODES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_FRWRD_MACT_SHADOW_MODE_OUT_OF_RANGE_ERR,
    "SOC_PPC_FRWRD_MACT_SHADOW_MODE_OUT_OF_RANGE_ERR",
    "The parameter 'shadow_mode' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_FRWRD_MACT_SHADOW_MODES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_ACTION_WHEN_EXCEED_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_MACT_ACTION_WHEN_EXCEED_OUT_OF_RANGE_ERR",
    "The parameter 'action_when_exceed' is out of range. \n\r "
    "The range is: 0 - SOC_PPC_NOF_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_SA_DROP_ACTION_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_MACT_SA_DROP_ACTION_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'sa_drop_action_profile' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_SA_UNKNOWN_ACTION_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_MACT_SA_UNKNOWN_ACTION_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'sa_unknown_action_profile' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_DA_UNKNOWN_ACTION_PROFILE_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_MACT_DA_UNKNOWN_ACTION_PROFILE_OUT_OF_RANGE_ERR",
    "The parameter 'da_unknown_action_profile' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_MACT_ID_OUT_OF_RANGE_ERR",
    "The parameter 'id' is out of range. \n\r "
    "The range is: 0 - 255.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_MGMT_BUFF_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_MACT_MGMT_BUFF_OUT_OF_RANGE_ERR",
    "The parameter 'buff' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_MGMT_BUFF_LEN_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_MACT_MGMT_BUFF_LEN_OUT_OF_RANGE_ERR",
    "The parameter 'buff_len' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  

  {
    ARAD_PP_FRWRD_MACT_MGMT_SEC_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_MACT_MGMT_SEC_OUT_OF_RANGE_ERR",
    "The parameter 'sec' is out of range. \n\r "
    "The range is: 0 - 200.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_MAC_LIMIT_FID_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_MACT_MAC_LIMIT_FID_OUT_OF_RANGE_ERR",
    "The parameter 'fid_base' is out of range. \n\r "
    "The range is: 0 - 0.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_MAC_LIMIT_GENERATE_EVENT_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_MACT_MAC_LIMIT_GENERATE_EVENT_OUT_OF_RANGE_ERR",
    "The parameter 'generate_event' is out of range. \n\r "
    "The range is: 0 - 0.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY_IS_LAG_OUT_OF_RANGE_ERR,
    "SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY_IS_LAG_OUT_OF_RANGE_ERR",
    "The parameter 'is_lag' is out of range. \n\r "
    "The range is: 0 - 0 for the ingress learning.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_ACTION_TRAP_CODE_LSB_INVALID_ERR,
    "ARAD_PP_FRWRD_MACT_ACTION_TRAP_CODE_LSB_INVALID_ERR",
    "The parameter 'trap_code' is not valid (useless for this API). \n\r "
    "The range is: 0 - 0.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_FID_FAIL_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_MACT_FID_FAIL_OUT_OF_RANGE_ERR",
    "'' is out of range. \n\r "
    "The range is: 0 to 0xfffe.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_FIFO_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_MACT_FIFO_OUT_OF_RANGE_ERR",
    "The parameter 'distribution_fifo' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_GLBL_LIMIT_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_MACT_GLBL_LIMIT_OUT_OF_RANGE_ERR",
    "global limit is out of range . \n\r "
    "The range is: 0 - 256K.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_FID_LIMIT_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_MACT_FID_LIMIT_OUT_OF_RANGE_ERR",
    "global limit is out of range . \n\r "
    "The range is: 0 - 64K .\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_FID_LIMIT_DIS_ERR,
    "ARAD_PP_FRWRD_MACT_FID_LIMIT_DIS_ERR",
    "cannot disable FID limit per FID \n\r "
    "limit, can be enabled/disable only globally \n\r "
    "The range is: 0 - 64K .\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_MSG_LEN_ERR,
    "ARAD_PP_FRWRD_MACT_MSG_LEN_ERR",
    "learning message is too small (unexpected size). \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_MSG_IS_NOT_LEARN_MSG_ERR,
    "ARAD_PP_FRWRD_MACT_MSG_IS_NOT_LEARN_MSG_ERR",
    "message passed to arad_pp_frwrd_mact_learn_msg_parse is not a learning message\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRARD_MACT_FRWRD_MACT_LOOKUP_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRARD_MACT_FRWRD_MACT_LOOKUP_TYPE_OUT_OF_RANGE_ERR",
    "lookup_type is out of range. Valid range: 0 - 2\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_SA_LOOKUP_MIM_AND_SA_AUTH_CANNOT_COEXIST_ERR,
    "ARAD_PP_FRWRD_MACT_SA_LOOKUP_MIM_AND_SA_AUTH_CANNOT_COEXIST_ERR",
    "SA authentication cannot co-exit with mac-in-mac\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_MACT_MIM_IS_NOT_ENABLED_ERR,
    "ARAD_PP_FRWRD_MACT_MIM_IS_NOT_ENABLED_ERR",
    "Cannot enable Mac-in-Mac lookup if Mac-in-Mac is not enabled\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },  

  
SOC_ERR_DESC_ELEMENT_DEF_LAST
};


static const ARAD_PP_FRWRD_MACT_APP_TO_FLP_PROGS app_to_flp_prog_map[ARAD_PP_FRWRD_MACT_NOF_ROUTED_LEARNING_APPS] = 
  
{ 
  {SOC_PPC_FRWRD_MACT_L3_LEARN_IPV4_UC, ARAD_PP_FWD_CODE_IPV4_UC},
  {SOC_PPC_FRWRD_MACT_L3_LEARN_IPV4_MC, ARAD_PP_FWD_CODE_IPV4_MC},
  {SOC_PPC_FRWRD_MACT_L3_LEARN_IPV6_UC, ARAD_PP_FWD_CODE_IPV6_UC},
  {SOC_PPC_FRWRD_MACT_L3_LEARN_IPV6_MC, ARAD_PP_FWD_CODE_IPV6_MC},
  {SOC_PPC_FRWRD_MACT_L3_LEARN_MPLS,    ARAD_PP_FWD_CODE_MPLS},
};






STATIC uint32
  arad_pp_frwrd_mact_event_type_to_event_val(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_TYPE                     event_type_ndx
  )
{

  switch(event_type_ndx)
  {
  case SOC_PPC_FRWRD_MACT_EVENT_TYPE_AGED_OUT:
    return 0;
  case SOC_PPC_FRWRD_MACT_EVENT_TYPE_LEARN:
    return 3;
  case SOC_PPC_FRWRD_MACT_EVENT_TYPE_TRANSPLANT:
    return 6;
  case SOC_PPC_FRWRD_MACT_EVENT_TYPE_REFRESH:
    return 2;
  case SOC_PPC_FRWRD_MACT_EVENT_TYPE_ACK:
    return 5;
  case SOC_PPC_FRWRD_MACT_EVENT_TYPE_REQ_FAIL:
    return 7;
  case SOC_PPC_FRWRD_MACT_EVENT_TYPE_RETRIEVE:
      return 4;
  default:
    return 1;
  }
}



soc_error_t arad_pp_frwrd_mact_is_dma_supported(SOC_SAND_IN int unit, SOC_SAND_OUT uint32 *dma_supported)
{
    uint32 host_mem_size_in_bytes;
    int res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *dma_supported = 0;
    host_mem_size_in_bytes = soc_property_get(unit, spn_LEARNING_FIFO_DMA_BUFFER_SIZE, 0);

    if (SOC_IS_JERICHO(unit) && (host_mem_size_in_bytes > 0)) 
    {
        *dma_supported =  1;
    }

    ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_is_dma_supported()", 0, 0);
}


uint32 arad_pp_frwrd_mact_is_cpu_mode_enabled(SOC_SAND_IN int unit, SOC_SAND_OUT uint8 *cpu_enabled)
{
    int res;
    uint32 reg_val;
	uint32 reg_ndx;
    uint32 dma_supported;
    int channel_number = -1;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_pp_frwrd_mact_is_dma_supported(unit, &dma_supported);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

    
    
    if (dma_supported)
    {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_mgmt_dma_fifo_channel_get, (unit, dma_fifo_channel_src_olp, &channel_number)));     
        *cpu_enabled = (channel_number != -1) ? 1 : 0;
    }
    else
    {
		reg_ndx = 0;
		SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, READ_OLP_DSP_ENGINE_CONFIGURATIONr(unit, reg_ndx, &reg_val));
        *cpu_enabled = (soc_reg_field_get(unit, OLP_DSP_ENGINE_CONFIGURATIONr, reg_val, DSP_GENERATION_ENf)==0); 
		
		reg_ndx = 1;
		SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, READ_OLP_DSP_ENGINE_CONFIGURATIONr(unit, reg_ndx, &reg_val));
        *cpu_enabled = (*cpu_enabled) | (soc_reg_field_get(unit, OLP_DSP_ENGINE_CONFIGURATIONr, reg_val, DSP_GENERATION_ENf)==0); 
    }
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_is_cpu_mode_enabled()", 0, 0);
}


uint32
  arad_pp_frwrd_mact_is_age_resolution_low(
    SOC_SAND_IN  int   unit,
    SOC_SAND_OUT uint8 *is_age_resolution_low
  )
{
    uint8
        is_petra_a_compatible;
    soc_error_t
        rv;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
    if(ARAD_PP_FRWRD_MACT_AGING_HIGH_RES_DISABLED) {
        *is_age_resolution_low = TRUE;
        ARAD_DO_NOTHING_AND_EXIT;
    }
    
    rv = sw_state_access[unit].dpp.soc.arad.pp.fwd_mact.is_petra_a_compatible.get(unit, &is_petra_a_compatible);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 10, exit);
    
    *is_age_resolution_low = is_petra_a_compatible;
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_is_age_resolution_low()", 0, 0);
}


STATIC uint32
  arad_pp_frwrd_mact_max_age_default_get(
      SOC_SAND_IN  int  unit,
      SOC_SAND_OUT uint32  *max_age
    )
{
  uint32
    res;
  uint8
    is_age_resolution_low;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_MAX_AGE_GET);

  
  res = arad_pp_frwrd_mact_is_age_resolution_low(unit, &is_age_resolution_low);
  SOC_SAND_CHECK_FUNC_RESULT(res, 7, exit);

  if (is_age_resolution_low)
  {
    *max_age = 2;
  }
  else
  {
    *max_age = 6;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_max_age_default_get()", 0, 0);
}














STATIC void
  arad_pp_frwrd_mact_sw_age_to_aging_config_line(SOC_SAND_IN  int  unit,
                                                 SOC_SAND_OUT uint32 *line_number,
                                                 SOC_SAND_IN uint32 sw_age,
                                                 SOC_SAND_IN uint8 profile_number,
                                                 SOC_SAND_IN uint8 is_owned)
{
    uint32 hw_age=0;
    uint8 rbd_bit=0;
    uint32 profile_start_line=0;

    switch (sw_age) {
    case 0:
        hw_age = 0;
        rbd_bit = 0;
        break;
    case 1:
        hw_age = 1;
        rbd_bit = 0;
        break;
    case 2:
        hw_age = 2;
        rbd_bit = 0;
        break;
    case 3:
        hw_age = 0;
        rbd_bit = 1;
        break;
    case 4:
        hw_age = 1;
        rbd_bit = 1;
        break;
    case 5:
        hw_age = 2;
        rbd_bit = 1;
        break;
    case 6:
        hw_age = 3;

        if (SOC_IS_JERICHO(unit))
        {
            rbd_bit = 1;
        }
        else
        {
            rbd_bit = 0;
        }
        break;
    case 7:
        hw_age = 3;

        if (SOC_IS_JERICHO(unit))
        {
            rbd_bit = 0;
        }
        else
        {
            rbd_bit = 1;
        }
    }

    if (SOC_IS_JERICHO(unit))
    {
        profile_start_line = profile_number * 32;
        hw_age = (rbd_bit << 2) + hw_age;
        rbd_bit = 0;
    }
    else
    {
        profile_start_line = profile_number * 16;
    }

    
    *line_number = profile_start_line + (hw_age << 2) + (rbd_bit << 1) + is_owned;
}


STATIC uint32
  arad_pp_frwrd_mact_age_conf_default_get(
      SOC_SAND_IN  int                      unit,
      SOC_SAND_OUT ARAD_PP_FRWRD_MACT_AGING_EVENT_TABLE  *default_conf
    )
{
  uint8
    is_owned,
    is_age_resolution_low;
  uint32
    res,
    age_ndx;
  SOC_PPC_FRWRD_MACT_LEARNING_MODE
    learning_mode;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_AGE_CONF_DEFAULT_GET);

  

  
  res = sw_state_access[unit].dpp.soc.arad.pp.fwd_mact.learning_mode.get(unit, &learning_mode);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);  
  
  res = arad_pp_frwrd_mact_is_age_resolution_low(unit, &is_age_resolution_low);
  if (is_age_resolution_low)
  {
    switch(learning_mode)
    {
    case SOC_PPC_FRWRD_MACT_LEARNING_MODE_INGRESS_DISTRIBUTED:
    case SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_DISTRIBUTED:
      
      for (age_ndx = 0; age_ndx < ARAD_PP_FRWRD_MACT_ENTRY_NOF_AGES; ++age_ndx)
      {
        for (is_owned = 0; is_owned < ARAD_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED; ++is_owned)
        {
          if (age_ndx == 0)
          {
            default_conf->age_action[age_ndx][is_owned].deleted = TRUE;
            default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
            default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
          }
          else if (age_ndx < 2)
          {
            default_conf->age_action[age_ndx][is_owned].deleted = TRUE;
            default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
            default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
          }
          else if (age_ndx < 3)
          {
            if (is_owned == FALSE)
            {
              default_conf->age_action[age_ndx][is_owned].deleted = FALSE;
              default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
              default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
            }
            else 
            {
              default_conf->age_action[age_ndx][is_owned].deleted = TRUE;
              default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
              default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
            }
          }
          else if (age_ndx == 6)
          {
            default_conf->age_action[age_ndx][is_owned].deleted = FALSE;
            default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
            default_conf->age_action[age_ndx][is_owned].refreshed = TRUE;
          }
          else
          {
            default_conf->age_action[age_ndx][is_owned].deleted = FALSE;
            default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
            default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
          }
        }
      }
      break;

    case SOC_PPC_FRWRD_MACT_LEARNING_MODE_INGRESS_CENTRALIZED:
    case SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_CENTRALIZED:
      
      for (age_ndx = 0; age_ndx < ARAD_PP_FRWRD_MACT_ENTRY_NOF_AGES; ++age_ndx)
      {
        for (is_owned = 0; is_owned < ARAD_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED; ++is_owned)
        {
          if (age_ndx < 2)
          {
            default_conf->age_action[age_ndx][is_owned].deleted = FALSE;
            default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
            default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
          }
          else if (age_ndx < 4)
          {
            default_conf->age_action[age_ndx][is_owned].deleted = FALSE;
            default_conf->age_action[age_ndx][is_owned].aged_out = TRUE;
            default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
          }
          else if (age_ndx == 6)
          {
            default_conf->age_action[age_ndx][is_owned].deleted = FALSE;
            default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
            default_conf->age_action[age_ndx][is_owned].refreshed = TRUE;
          }
          else
          {
            default_conf->age_action[age_ndx][is_owned].deleted = FALSE;
            default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
            default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
          }
        }
      }
      break;

    case SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_INDEPENDENT:
      
      for (age_ndx = 0; age_ndx < ARAD_PP_FRWRD_MACT_ENTRY_NOF_AGES; ++age_ndx)
      {
        for (is_owned = 0; is_owned < ARAD_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED; ++is_owned)
        {
          if (age_ndx == 0)
          {
            default_conf->age_action[age_ndx][is_owned].deleted = TRUE;
            default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
            default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
          }
          else
          {
            default_conf->age_action[age_ndx][is_owned].deleted = FALSE;
            default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
            default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
          }
        }
      }
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PPC_FRWRD_MACT_LEARNING_MODE_OUT_OF_RANGE_ERR, 10, exit);
    }
  }
  else
  {
    switch(learning_mode)
    {
    case SOC_PPC_FRWRD_MACT_LEARNING_MODE_INGRESS_DISTRIBUTED:
    case SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_DISTRIBUTED:
      
      for (age_ndx = 0; age_ndx < ARAD_PP_FRWRD_MACT_ENTRY_NOF_AGES; ++age_ndx)
      {
        for (is_owned = 0; is_owned < ARAD_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED; ++is_owned)
        {
          if (age_ndx == 0)
          {
            default_conf->age_action[age_ndx][is_owned].deleted = TRUE;
            default_conf->age_action[age_ndx][is_owned].aged_out = TRUE;
            default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
          }
          if ((age_ndx > 0 &&
              age_ndx < 6) ||
              age_ndx == 7)
          {
            default_conf->age_action[age_ndx][is_owned].deleted = FALSE;
            default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
            default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
          }
          else if (age_ndx == 6)
          {
            default_conf->age_action[age_ndx][is_owned].deleted = FALSE;
            default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
            default_conf->age_action[age_ndx][is_owned].refreshed = TRUE;
          }
        }
      }
      break;

    case SOC_PPC_FRWRD_MACT_LEARNING_MODE_INGRESS_CENTRALIZED:
    case SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_CENTRALIZED:
      
      for (age_ndx = 0; age_ndx < ARAD_PP_FRWRD_MACT_ENTRY_NOF_AGES; ++age_ndx)
      {
        for (is_owned = 0; is_owned < ARAD_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED; ++is_owned)
        {          
          if (age_ndx == 0)
          {
            default_conf->age_action[age_ndx][is_owned].deleted = FALSE;
            default_conf->age_action[age_ndx][is_owned].aged_out = TRUE;
            default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
          }
          else if (age_ndx == 6)
          {
            default_conf->age_action[age_ndx][is_owned].deleted = FALSE;
            default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
            default_conf->age_action[age_ndx][is_owned].refreshed = TRUE;
          }
          else
          {
            default_conf->age_action[age_ndx][is_owned].deleted = FALSE;
            default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
            default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
          }
        }
      }
      break;

    case SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_INDEPENDENT:
      
      for (age_ndx = 0; age_ndx < ARAD_PP_FRWRD_MACT_ENTRY_NOF_AGES; ++age_ndx)
      {
        for (is_owned = 0; is_owned < ARAD_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED; ++is_owned)
        {
          if (age_ndx == 0)
          {
            default_conf->age_action[age_ndx][is_owned].deleted = TRUE;
            default_conf->age_action[age_ndx][is_owned].aged_out = TRUE;
            default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
          }
          else
          {
            default_conf->age_action[age_ndx][is_owned].deleted = FALSE;
            default_conf->age_action[age_ndx][is_owned].aged_out = FALSE;
            default_conf->age_action[age_ndx][is_owned].refreshed = FALSE;
          }
        }
      }
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PPC_FRWRD_MACT_LEARNING_MODE_OUT_OF_RANGE_ERR, 10, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_age_conf_default_get()", 0, 0);
}


uint32
  arad_pp_frwrd_mact_age_conf_write(
      SOC_SAND_IN  int                      unit,
      SOC_SAND_IN  uint8                       aging_cfg_ptr,
      SOC_SAND_IN  ARAD_PP_FRWRD_MACT_AGING_EVENT_TABLE  *conf
    )
{
  uint32 res;
  uint32 line_number;
  uint8 is_owned;
  uint32 age_ndx;
  ARAD_PP_IHP_MACT_AGING_CONFIGURATION_TABLE_TBL_DATA tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_AGE_CONF_WRITE);
  
  for (age_ndx = 0; age_ndx < ARAD_PP_FRWRD_MACT_ENTRY_NOF_AGES; ++age_ndx)
  {
    for (is_owned = 0; is_owned < ARAD_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED; ++is_owned)
    {
        tbl_data.aging_cfg_info_delete_entry = conf->age_action[age_ndx][is_owned].deleted;
        tbl_data.aging_cfg_info_create_aged_out_event = conf->age_action[age_ndx][is_owned].aged_out;
        tbl_data.aging_cfg_info_create_refresh_event = conf->age_action[age_ndx][is_owned].refreshed; 

        arad_pp_frwrd_mact_sw_age_to_aging_config_line(unit,
                                                       &line_number,
                                                       age_ndx,
                                                       aging_cfg_ptr,
                                                       is_owned);

        res = arad_pp_ihp_mact_aging_configuration_table_tbl_set_unsafe(
                unit,
                line_number,
                &tbl_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10 + age_ndx, exit);

        
        if (SOC_IS_JERICHO(unit)) {
            line_number += 2; 

            tbl_data.aging_cfg_info_create_refresh_event = 0;

            
            res = arad_pp_ihp_mact_aging_configuration_table_tbl_set_unsafe(
                    unit,
                    line_number,
                    &tbl_data
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 10 + age_ndx, exit);
        }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_age_conf_write()", age_ndx, 0);
}


STATIC uint32
  arad_pp_frwrd_mact_age_conf_read(
      SOC_SAND_IN  int                      unit,
      SOC_SAND_OUT ARAD_PP_FRWRD_MACT_AGING_EVENT_TABLE  *conf
    )
{
  uint32
      res;
  uint8
      is_owned;
  uint32
      line_number,
      age_ndx;
  ARAD_PP_IHP_MACT_AGING_CONFIGURATION_TABLE_TBL_DATA
      tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_AGE_CONF_READ);

  SOC_SAND_CHECK_NULL_INPUT(conf);

  for (age_ndx = 0; age_ndx < ARAD_PP_FRWRD_MACT_ENTRY_NOF_AGES; ++age_ndx)
  {
    for (is_owned = 0; is_owned < ARAD_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED; ++is_owned)
    {
      arad_pp_frwrd_mact_sw_age_to_aging_config_line(unit,
                                                 &line_number,
                                                 age_ndx,
                                                 ARAD_PP_FRWRD_MACT_AGE_CONF_PTR_FOR_MAC,
                                                 1);
      res = arad_pp_ihp_mact_aging_configuration_table_tbl_get_unsafe(
              unit,
              line_number,
              &tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10 + age_ndx, exit);

      conf->age_action[age_ndx][is_owned].deleted = SOC_SAND_NUM2BOOL(tbl_data.aging_cfg_info_delete_entry);
      conf->age_action[age_ndx][is_owned].aged_out = SOC_SAND_NUM2BOOL(tbl_data.aging_cfg_info_create_aged_out_event);
      conf->age_action[age_ndx][is_owned].refreshed = SOC_SAND_NUM2BOOL(tbl_data.aging_cfg_info_create_refresh_event);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_age_conf_read()", 0, 0);
}


STATIC uint32
  arad_pp_frwrd_mact_age_modify_get(
      SOC_SAND_IN  int                      unit,
      SOC_SAND_OUT ARAD_PP_FRWRD_MACT_AGING_MODIFICATION  *age_modif
    )
{
  uint8
    is_owned;
  SOC_PPC_FRWRD_MACT_LEARNING_MODE
    learning_mode;
  uint32
    res;
  uint8
    is_age_resolution_low;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_AGE_MODIFY_GET);

  
  res = sw_state_access[unit].dpp.soc.arad.pp.fwd_mact.learning_mode.get(unit, &learning_mode);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);  

  
  res = arad_pp_frwrd_mact_is_age_resolution_low(unit, &is_age_resolution_low);
  SOC_SAND_CHECK_FUNC_RESULT(res, 7, exit);

  if (is_age_resolution_low)
  {
    switch(learning_mode)
    {
    case SOC_PPC_FRWRD_MACT_LEARNING_MODE_INGRESS_DISTRIBUTED:
    case SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_DISTRIBUTED:
      
      age_modif->age_delete[FALSE] = 1;
      age_modif->age_delete[TRUE] = 2;
      age_modif->age_aged_out[FALSE] = 2;
      age_modif->age_aged_out[TRUE] = 3;
      age_modif->age_refresh[FALSE] = 3;
      age_modif->age_refresh[TRUE] = 3;
      break;

    case SOC_PPC_FRWRD_MACT_LEARNING_MODE_INGRESS_CENTRALIZED:
    case SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_CENTRALIZED:
      
      for (is_owned = 0; is_owned < ARAD_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED; ++is_owned)
      {
        age_modif->age_delete[is_owned] = 0;
        age_modif->age_aged_out[is_owned] = 1;
        age_modif->age_refresh[is_owned] = 2;
      }
      break;

    case SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_INDEPENDENT:
      
      for (is_owned = 0; is_owned < ARAD_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED; ++is_owned)
      {
        age_modif->age_delete[is_owned] = 0;
        age_modif->age_aged_out[is_owned] = 1;
        age_modif->age_refresh[is_owned] = 2;
      }
       break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PPC_FRWRD_MACT_LEARNING_MODE_OUT_OF_RANGE_ERR, 10, exit);
    }
    
      for (is_owned = 0; is_owned < ARAD_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED; ++is_owned)
      {
        age_modif->age_delete[is_owned] = age_modif->age_delete[is_owned] << 1;
        age_modif->age_aged_out[is_owned] = age_modif->age_aged_out[is_owned] << 1;
        age_modif->age_refresh[is_owned] = age_modif->age_refresh[is_owned] << 1;
      }
    }
  else
  {
    switch(learning_mode)
    {
    case SOC_PPC_FRWRD_MACT_LEARNING_MODE_INGRESS_DISTRIBUTED:
    case SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_DISTRIBUTED:
      
      age_modif->age_delete[FALSE] = 2;
      age_modif->age_delete[TRUE] = 4;
      age_modif->age_aged_out[FALSE] = 3;
      age_modif->age_aged_out[TRUE] = 5;
      age_modif->age_refresh[FALSE] = 4;
      age_modif->age_refresh[TRUE] = 6;
      break;

    case SOC_PPC_FRWRD_MACT_LEARNING_MODE_INGRESS_CENTRALIZED:
    case SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_CENTRALIZED:
      
      age_modif->age_delete[FALSE] = 0;
      age_modif->age_delete[TRUE] = 0;
      age_modif->age_aged_out[FALSE] = 1;
      age_modif->age_aged_out[TRUE] = 2;
      age_modif->age_refresh[FALSE] = 2;
      age_modif->age_refresh[TRUE] = 3;
      break;

    case SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_INDEPENDENT:
      
      for (is_owned = 0; is_owned < ARAD_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED; ++is_owned)
      {
        age_modif->age_delete[is_owned] = 0;
        age_modif->age_aged_out[is_owned] = 1;
        age_modif->age_refresh[is_owned] = 2;
      }
       break;

    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PPC_FRWRD_MACT_LEARNING_MODE_OUT_OF_RANGE_ERR, 10, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_age_modify_get()", 0, 0);
}




uint32
  arad_pp_frwrd_mact_defrag_enable(
    SOC_SAND_IN  int                 unit
  )
{
	soc_reg_t mact_reg;
    uint32 field_val, field_idx;
    uint32 nof_defrag_config_registers;
    uint32 res = SOC_SAND_OK;
        							
    const soc_reg_t defrag_config_registers_array[] = {IHB_REG_028Dr, IHB_REG_02CDr, EPNI_REG_01AFr, OAMP_REG_020Dr, IHP_REG_0154r, IHP_REG_0174r};    
    const soc_reg_t defrag_config_registers_jericho[] = {PPDB_A_OEMA_DEFRAG_CONFIGURATION_REGISTERr, PPDB_A_OEMB_DEFRAG_CONFIGURATION_REGISTERr, EDB_ESEM_DEFRAG_CONFIGURATION_REGISTERr, OAMP_REMOTE_MEP_EXACT_MATCH_DEFRAG_CONFIGURATION_REGISTERr, IHB_ISEM_DEFRAG_CONFIGURATION_REGISTERr};
    
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    if (SOC_IS_JERICHO(unit)) {
        goto exit;
    }

    
    mact_reg = SOC_IS_JERICHO(unit)? PPDB_B_LARGE_EM_DEFRAG_CONFIGURATION_REGISTERr: (SOC_IS_ARADPLUS(unit)? IHP_REG_02B3r: IHP_REG_0273r);
    if (SOC_IS_JERICHO(unit)) {
        nof_defrag_config_registers = sizeof(defrag_config_registers_jericho)/sizeof(defrag_config_registers_jericho[0]);
    } else {
        nof_defrag_config_registers = sizeof(defrag_config_registers_array)/sizeof(defrag_config_registers_array[0]);
    }
    field_val = 0x1;
    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, mact_reg, REG_PORT_ANY, 0, FIELD_0_0f,  field_val));

    for (field_idx = 0; field_idx < nof_defrag_config_registers; field_idx++) {        
        mact_reg = SOC_IS_JERICHO(unit)? defrag_config_registers_jericho[field_idx]: defrag_config_registers_array[field_idx];
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, mact_reg, REG_PORT_ANY, 0, FIELD_0_0f,  field_val));
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_mact_defrag_enable()", 0, 0);
}



STATIC uint32
  arad_pp_frwrd_mact_regs_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    fld_val,
    cmd_to_cmd = 0xFAC688,
    res = SOC_SAND_OK;
  ARAD_PP_MGMT_OPERATION_MODE
    *oper_mode = NULL;
  soc_reg_t
     event_fifo_configuration_reg = SOC_IS_JERICHO(unit) ? PPDB_B_LARGE_EM_EVENT_FIFO_CONFIGURATIONr : IHP_MACT_EVENT_FIFO_CONFIGURATIONr;
  soc_field_t
     field;
  uint64  data_uint64;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_REGS_INIT);

  ARAD_ALLOC(oper_mode, ARAD_PP_MGMT_OPERATION_MODE, 1, "arad_pp_frwrd_mact_regs_init.oper_mode");
  res = arad_pp_mgmt_operation_mode_get_unsafe(
          unit,
          oper_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  
  res = sw_state_access[unit].dpp.soc.arad.pp.oper_mode.authentication_enable.set(
    unit,
    oper_mode->authentication_enable
  );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 7, exit);

  
  fld_val = 0x1;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_MANAGEMENT_UNIT_CONFIGURATION_REGISTERr, REG_PORT_ANY, 0,  LARGE_EM_MNGMNT_UNIT_ENABLEf,  fld_val));

  

  res = arad_pp_frwrd_mact_defrag_enable(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  fld_val = ARAD_PP_FRWRD_MACT_FIFO_FULL_THRESHOLD;
  if(SOC_IS_ARADPLUS_AND_BELOW(unit)) { 
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, event_fifo_configuration_reg, REG_PORT_ANY, 0, MACT_EVENT_FIFO_FULL_THRESHOLDf,  fld_val));
  }
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_REPLY_FIFO_CONFIGURATIONr, REG_PORT_ANY, 0,  LARGE_EM_REPLY_FIFO_FULL_THRESHOLDf,  fld_val));

  
  if (SOC_IS_JERICHO(unit))
  {
    field = LARGE_EM_JR_TO_AR_TRANSLATION_AGEf;
    COMPILER_64_SET(data_uint64, 0xFEDCBA98, 0x36543210);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, PPDB_B_LARGE_EM_JR_TO_AR_TRANSLATIONr, REG_PORT_ANY, 0, field,  data_uint64));

    field = LARGE_EM_AR_TO_JR_TRANSLATION_AGEf;
    COMPILER_64_SET(data_uint64, 0x0, 0xF6517210);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, PPDB_B_LARGE_EM_AR_TO_JR_TRANSLATIONr, REG_PORT_ANY, 0, field,  data_uint64));

  }

  
  field = SOC_IS_JERICHO(unit) ? LARGE_EM_EVENT_FIFO_ACCESS_FID_DBf : MACT_EVENT_FIFO_ACCESS_FID_DBf;
  fld_val = 0x1;

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, event_fifo_configuration_reg, REG_PORT_ANY, 0, field,  fld_val));

  
  fld_val = 0x2;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  28,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_DIAGNOSTICS_ACCESSED_MODEr, REG_PORT_ANY, 0,  LARGE_EM_DIAGNOSTICS_ACCESSED_MODEf,  fld_val));

  if(SOC_IS_ARADPLUS_AND_BELOW(unit)) { 
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OLP_CONFIGS_GENERALr, REG_PORT_ANY, 0, PETRA_SOURCE_DEVICEf,  fld_val));
  }

  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, WRITE_PPDB_B_LARGE_EM_CMD_TO_CMD_TRANSLATIONr(unit, cmd_to_cmd));

exit:
  ARAD_FREE(oper_mode);
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_mact_regs_init()",0,0);
}

uint8
  arad_pp_frwrd_mact_mgmt_is_b0_high_resolution(
    SOC_SAND_IN  int                                 unit
  )
{
    return TRUE;
}



uint32
  arad_pp_frwrd_mact_learn_limit_init_unsafe(
    SOC_SAND_IN  int                 unit
  )
{
  uint64 
      field_val;
  uint32 lif_range_ix,
         res = SOC_SAND_OK;
  soc_reg_above_64_val_t lif_limit_reg_value;
  soc_reg_above_64_val_t lif_ptr_range_map_reg_value, field_zero_msbs, field_shift_right, field_lif_base;
  soc_reg_above_64_val_t tmp_lif_ptr_range_map_reg_value;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (SOC_IS_ARADPLUS(unit)) {
      
      
      if (SOC_PPC_FRWRD_MACT_LEARN_LIMIT_MODE == SOC_PPC_FRWRD_MACT_LEARN_LIMIT_TYPE_LIF) {
          COMPILER_64_SET(field_val, 0, 0x0);
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, PPDB_B_LARGE_EM_COUNTER_DB_CNTR_PTR_CONFIGURATIONr, REG_PORT_ANY, 0,  LARGE_EM_CFG_LIMIT_MODE_FIDf,  field_val));
      }

      
      if (SOC_PPC_FRWRD_MACT_LEARN_LIMIT_MODE == SOC_PPC_FRWRD_MACT_LEARN_LIMIT_TYPE_LIF) {

          
          SOC_REG_ABOVE_64_CLEAR(lif_limit_reg_value);

          SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_reg_above_64_get(unit, PPDB_B_LARGE_EM_COUNTER_DB_CNTR_PTR_CONFIGURATIONr, REG_PORT_ANY, 0, lif_limit_reg_value));

          
          soc_reg_above_64_field32_set(unit, PPDB_B_LARGE_EM_COUNTER_DB_CNTR_PTR_CONFIGURATIONr, lif_limit_reg_value,
                                      LARGE_EM_INVALID_LIF_CNTR_PTRf, ARAD_PP_FRWRD_MACT_LIMIT_RANGE_END_MAX);

          
          soc_reg_above_64_field32_set(unit, PPDB_B_LARGE_EM_COUNTER_DB_CNTR_PTR_CONFIGURATIONr, lif_limit_reg_value,
                                      LARGE_EM_LIF_RANGE_0f, ARAD_PP_FRWRD_MACT_LIMIT_RANGE_MAP_SIZE);
          soc_reg_above_64_field32_set(unit, PPDB_B_LARGE_EM_COUNTER_DB_CNTR_PTR_CONFIGURATIONr, lif_limit_reg_value,
                                      LARGE_EM_LIF_RANGE_1f, ARAD_PP_FRWRD_MACT_LIMIT_RANGE_MAP_SIZE * 2);
          soc_reg_above_64_field32_set(unit, PPDB_B_LARGE_EM_COUNTER_DB_CNTR_PTR_CONFIGURATIONr, lif_limit_reg_value,
                                      LARGE_EM_LIF_RANGE_2f, ARAD_PP_FRWRD_MACT_LIMIT_RANGE_MAP_SIZE * 3);
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, soc_reg_above_64_set(unit, PPDB_B_LARGE_EM_COUNTER_DB_CNTR_PTR_CONFIGURATIONr, REG_PORT_ANY, 0, lif_limit_reg_value));

          
          SOC_REG_ABOVE_64_CLEAR(tmp_lif_ptr_range_map_reg_value);

          
          for (lif_range_ix = 0; lif_range_ix < SOC_PPC_MAX_NOF_MACT_LIMIT_LIF_RANGES; lif_range_ix++) {

              
              
              
              SOC_REG_ABOVE_64_CLEAR(lif_ptr_range_map_reg_value);

              SOC_REG_ABOVE_64_CLEAR(field_zero_msbs);
              SOC_REG_ABOVE_64_CLEAR(field_shift_right);
              SOC_REG_ABOVE_64_CLEAR(field_lif_base);

              
              
              if ((lif_range_ix * ARAD_PP_FRWRD_MACT_LIMIT_RANGE_MAP_SIZE) == SOC_PPC_FRWRD_MACT_LEARN_LIF_RANGE_BASE(0)) {
                  
                  field_zero_msbs[0] = SOC_DPP_DEFS_GET(unit, out_lif_nof_bits) - 14;
                  field_shift_right[0] = 0x0;
                  field_lif_base[0] = 0x0;   

              
              } else if ((lif_range_ix * ARAD_PP_FRWRD_MACT_LIMIT_RANGE_MAP_SIZE) ==  SOC_PPC_FRWRD_MACT_LEARN_LIF_RANGE_BASE(1)) {
                  
                  field_zero_msbs[0] = SOC_DPP_DEFS_GET(unit, out_lif_nof_bits) - 14;
                  field_shift_right[0] = 0x0;
                  field_lif_base[0] = 0x4000;    
              }
                  
              else {
                  field_zero_msbs[0] = 0x1F;
                  field_shift_right[0] = 0x1;
                  field_lif_base[0] = SOC_DPP_FRWRD_MACT_LIMIT_INV_MAP_PTR_MAX(unit);
              }

              
              soc_reg_above_64_field_set(unit, PPDB_B_LARGE_EM_COUNTER_DB_LIF_PTR_RANGE_MAPr, lif_ptr_range_map_reg_value,  LARGE_EM_LIF_ZERO_MSBSf, field_zero_msbs);
              soc_reg_above_64_field_set(unit, PPDB_B_LARGE_EM_COUNTER_DB_LIF_PTR_RANGE_MAPr, lif_ptr_range_map_reg_value,  LARGE_EM_LIF_SHIFT_RIGHTf, field_shift_right);
              soc_reg_above_64_field_set(unit, PPDB_B_LARGE_EM_COUNTER_DB_LIF_PTR_RANGE_MAPr, lif_ptr_range_map_reg_value,  LARGE_EM_LIF_BASEf, field_lif_base);

              
              SHR_BITCOPY_RANGE(tmp_lif_ptr_range_map_reg_value, lif_range_ix * RANGE_MAP_SIZE, lif_ptr_range_map_reg_value, 0, RANGE_MAP_SIZE);
          }

          SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit,
              soc_reg_above_64_set(unit, PPDB_B_LARGE_EM_COUNTER_DB_LIF_PTR_RANGE_MAPr, REG_PORT_ANY, 0, tmp_lif_ptr_range_map_reg_value));
      }

      

  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_mact_learn_limit_init_unsafe()", 0, 0);
}


#ifdef BCM_88660_A0


uint32
  arad_pp_frwrd_mact_arad_plus_em_hash_init_unsafe(
    SOC_SAND_IN  int                 unit
  )
{
    uint32 reg_val, field_val, field_idx;
    uint32 nof_exact_match_tables;
    uint32 res = SOC_SAND_OK;
    ARAD_PP_FRWRD_MACT_REG_FIELD_REF *em_hash_encryption_enable_ref;
    const ARAD_PP_FRWRD_MACT_REG_FIELD_REF exact_match_hash_encryption_enable_ref[] = { 
                    { PPDB_B_LARGE_EM_GENERAL_EM_CONFIGURATION_REGISTERr,       MACT_DECRYPTION_ENCRYPTION_ENABLEf      },
                    { PPDB_A_OEMA_GENERAL_EM_CONFIGURATION_REGISTERr,      OEMA_DECRYPTION_ENCRYPTION_ENABLEf      },
                    { PPDB_A_OEMB_GENERAL_EM_CONFIGURATION_REGISTERr,      OEMB_DECRYPTION_ENCRYPTION_ENABLEf      },
                    { EDB_ESEM_GENERAL_EM_CONFIGURATION_REGISTERr,     ESEM_DECRYPTION_ENCRYPTION_ENABLEf      },
                    { OAMP_REMOTE_MEP_EXACT_MATCH_GENERAL_EM_CONFIGURATION_REGISTERr,   RMAPEM_DECRYPTION_ENCRYPTION_ENABLEf    },
                    { IHP_ISA_GENERAL_EM_CONFIGURATION_REGISTERr,       ISA_DECRYPTION_ENCRYPTION_ENABLEf       },
                    { IHP_ISB_GENERAL_EM_CONFIGURATION_REGISTERr,       ISB_DECRYPTION_ENCRYPTION_ENABLEf       }
    };
    const ARAD_PP_FRWRD_MACT_REG_FIELD_REF exact_match_hash_encryption_enable_ref_jericho[] = { 
                    { PPDB_B_LARGE_EM_GENERAL_EM_CONFIGURATION_REGISTERr, MACT_DECRYPTION_ENCRYPTION_ENABLEf      },
                    { PPDB_A_OEMA_GENERAL_EM_CONFIGURATION_REGISTERr,      OEMA_DECRYPTION_ENCRYPTION_ENABLEf      },
                    { PPDB_A_OEMB_GENERAL_EM_CONFIGURATION_REGISTERr,      OEMB_DECRYPTION_ENCRYPTION_ENABLEf      },
                    { EDB_ESEM_GENERAL_EM_CONFIGURATION_REGISTERr,      ESEM_DECRYPTION_ENCRYPTION_ENABLEf      },
                    { OAMP_REMOTE_MEP_EXACT_MATCH_GENERAL_EM_CONFIGURATION_REGISTERr,   RMAPEM_DECRYPTION_ENCRYPTION_ENABLEf    },
                    { IHB_ISEM_GENERAL_EM_CONFIGURATION_REGISTERr,      ISEM_DECRYPTION_ENCRYPTION_ENABLEf     }
    };
    const ARAD_PP_FRWRD_MACT_REG_FIELD_REF exact_match_hash_encryption_enable_ref_jericho_plus[] = { 
                    { PPDB_B_LARGE_EM_GENERAL_EM_CONFIGURATION_REGISTERr, MACT_DECRYPTION_ENCRYPTION_ENABLEf      },
                    { PPDB_A_OEMA_GENERAL_EM_CONFIGURATION_REGISTERr,      OEMA_DECRYPTION_ENCRYPTION_ENABLEf      },
                    { PPDB_A_OEMB_GENERAL_EM_CONFIGURATION_REGISTERr,      OEMB_DECRYPTION_ENCRYPTION_ENABLEf      },
                    { OAMP_REMOTE_MEP_EXACT_MATCH_GENERAL_EM_CONFIGURATION_REGISTERr,   RMAPEM_DECRYPTION_ENCRYPTION_ENABLEf     },
                    { EDB_ESEM_0_GENERAL_EM_CONFIGURATION_REGISTERr,      ESEM_0_DECRYPTION_ENCRYPTION_ENABLEf      },
                    { EDB_ESEM_1_GENERAL_EM_CONFIGURATION_REGISTERr,      ESEM_1_DECRYPTION_ENCRYPTION_ENABLEf      },
                    { EDB_GLEM_GENERAL_EM_CONFIGURATION_REGISTERr,   GLEM_DECRYPTION_ENCRYPTION_ENABLEf    }, 
                    { IHB_ISEM_GENERAL_EM_CONFIGURATION_REGISTERr,      ISEM_DECRYPTION_ENCRYPTION_ENABLEf     },
    };
    const ARAD_PP_FRWRD_MACT_REG_FIELD_REF exact_match_hash_encryption_enable_ref_qax[] = { 
                    { PPDB_B_LARGE_EM_GENERAL_EM_CONFIGURATION_REGISTERr, MACT_DECRYPTION_ENCRYPTION_ENABLEf      },
                    { PPDB_A_OEMA_GENERAL_EM_CONFIGURATION_REGISTERr,      OEMA_DECRYPTION_ENCRYPTION_ENABLEf      },
                    { PPDB_A_OEMB_GENERAL_EM_CONFIGURATION_REGISTERr,      OEMB_DECRYPTION_ENCRYPTION_ENABLEf      },
                    { OAMP_REMOTE_MEP_EXACT_MATCH_GENERAL_EM_CONFIGURATION_REGISTERr,   RMAPEM_DECRYPTION_ENCRYPTION_ENABLEf     },
                    { EDB_ESEM_GENERAL_EM_CONFIGURATION_REGISTERr,      ESEM_DECRYPTION_ENCRYPTION_ENABLEf      },
                    { EDB_GLEM_GENERAL_EM_CONFIGURATION_REGISTERr,   GLEM_DECRYPTION_ENCRYPTION_ENABLEf    }, 
                    { PPDB_A_ISEM_GENERAL_EM_CONFIGURATION_REGISTERr,      ISEM_DECRYPTION_ENCRYPTION_ENABLEf     },
                    { PPDB_B_ISEM_GENERAL_EM_CONFIGURATION_REGISTERr,       ISEM_DECRYPTION_ENCRYPTION_ENABLEf      }
    };

    ARAD_PP_FRWRD_MACT_REG_FIELD_REF reg_field_ref;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    if (SOC_IS_QAX(unit)) {
        nof_exact_match_tables = sizeof(exact_match_hash_encryption_enable_ref_qax) / sizeof(exact_match_hash_encryption_enable_ref_qax[0]);
        em_hash_encryption_enable_ref = (ARAD_PP_FRWRD_MACT_REG_FIELD_REF*)exact_match_hash_encryption_enable_ref_qax;
    } else if (SOC_IS_JERICHO_PLUS(unit)) {
        nof_exact_match_tables = sizeof(exact_match_hash_encryption_enable_ref_jericho_plus) / sizeof(exact_match_hash_encryption_enable_ref_jericho_plus[0]);
        em_hash_encryption_enable_ref = (ARAD_PP_FRWRD_MACT_REG_FIELD_REF*)exact_match_hash_encryption_enable_ref_jericho_plus;
    } else if (SOC_IS_JERICHO(unit)) {
        nof_exact_match_tables = sizeof(exact_match_hash_encryption_enable_ref_jericho) / sizeof(exact_match_hash_encryption_enable_ref_jericho[0]);
        em_hash_encryption_enable_ref = (ARAD_PP_FRWRD_MACT_REG_FIELD_REF*)exact_match_hash_encryption_enable_ref_jericho;
    } else {
        nof_exact_match_tables = sizeof(exact_match_hash_encryption_enable_ref) / sizeof(exact_match_hash_encryption_enable_ref[0]);
        em_hash_encryption_enable_ref = (ARAD_PP_FRWRD_MACT_REG_FIELD_REF*)exact_match_hash_encryption_enable_ref;
    }

    for (field_idx = 0; field_idx < nof_exact_match_tables; field_idx++) {
        reg_field_ref = em_hash_encryption_enable_ref[field_idx];
        field_val = (1 << soc_reg_field_length(unit, reg_field_ref.reg_ref, reg_field_ref.field_ref)) - 1; 
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10 + field_idx,  exit, ARAD_REG_ACCESS_ERR,soc_reg32_get(unit, reg_field_ref.reg_ref, REG_PORT_ANY,  0, &reg_val));
        ARAD_FLD_TO_REG(reg_field_ref.reg_ref, reg_field_ref.field_ref, field_val, reg_val, 30 + field_idx, exit);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50 + field_idx,  exit, ARAD_REG_ACCESS_ERR,soc_reg32_set(unit, reg_field_ref.reg_ref, REG_PORT_ANY,  0,  reg_val));
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_mact_arad_plus_em_hash_init_unsafe()", 0, 0);
}



uint32
  arad_pp_frwrd_mact_arad_plus_sa_lookup_mapping_init_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                  enable
  )
{
    uint32 reg_val, field_val, mapping_idx;
    uint32 nof_sa_lookup_mapping_results;
    uint32 res = SOC_SAND_OK;
    const ARAD_PP_FRWRD_MACT_SA_LOOKUP_MAPPING_REG_FIELD_REF sa_lookup_mapping_result_ref[] = {
                    { SA_LOOKUP_RESULT_MAPPING_0f, 1 },     
                    { SA_LOOKUP_RESULT_MAPPING_1f, 1 },  
                    { SA_LOOKUP_RESULT_MAPPING_2f, 1 },     
                    { SA_LOOKUP_RESULT_MAPPING_3f, 1 },  
                    { SA_LOOKUP_RESULT_MAPPING_4f, 0 },  
                    { SA_LOOKUP_RESULT_MAPPING_5f, 1 },  
                    { SA_LOOKUP_RESULT_MAPPING_6f, 0 },  
                    { SA_LOOKUP_RESULT_MAPPING_7f, 0 }   
    };

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    field_val = (enable) ? 0x1:0x0;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_FLP_GENERAL_CFGr, REG_PORT_ANY, 0, SA_LOOKUP_RESULT_MAPPING_ENABLEf,  field_val));

    if (enable) 
    {
        
        nof_sa_lookup_mapping_results = sizeof(sa_lookup_mapping_result_ref) / sizeof(sa_lookup_mapping_result_ref[0]);


        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,READ_IHP_SA_LOOKUP_RESULT_MAPPINGr(unit, 0, &reg_val));

        for (mapping_idx = 0; mapping_idx < nof_sa_lookup_mapping_results; mapping_idx++) {

            field_val = mapping_idx | ((sa_lookup_mapping_result_ref[mapping_idx].learn_enable) << 3);
            
            if (mapping_idx == 4) {
                field_val |= 0x1; 
            }
            ARAD_FLD_TO_REG(IHP_SA_LOOKUP_RESULT_MAPPINGr,
                            sa_lookup_mapping_result_ref[mapping_idx].field_ref,
                            field_val, reg_val, 30 + mapping_idx, exit);
        }

        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHP_SA_LOOKUP_RESULT_MAPPINGr(unit, SOC_CORE_ALL, reg_val));
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_mact_arad_plus_sa_lookup_mapping_init_unsafe()", 0, 0);
}



uint32
  arad_pp_frwrd_mact_arad_plus_init_unsafe(
    SOC_SAND_IN  int                 unit
  )
{
    uint32 reg_val, field_val,
           res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    field_val = 0x0;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,
       soc_reg_above_64_field32_modify(unit, SOC_IS_JERICHO(unit)? PPDB_B_LARGE_EM_LOOKUP_ARBITER_CONFIGURATIONr: IHP_REG_028Ar, REG_PORT_ANY, 0, SOC_IS_JERICHO(unit)? LARGE_EM_OPPORTUNISTIC_PRIORITY_FIX_DISABLEf : FIELD_0_0f,  field_val));

    
    field_val = 0x0;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_AGE_MACHINE_CONFIGURATIONr, REG_PORT_ANY, 0, SOC_IS_JERICHO(unit)? LARGE_EM_AGE_FORMER_COUNTDOWNf : ITEM_18_18f,  field_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_FLU_MACHINE_CONFIGURATIONr, REG_PORT_ANY, 0, SOC_IS_JERICHO(unit)? LARGE_EM_FLU_FORMER_COUNTDOWNf : ITEM_15_15f,  field_val));

    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,READ_PPDB_B_LARGE_EM_MNGMNT_REQ_FLU_DB_CONFIGURATIONr(unit, &reg_val));
    field_val = 0x1;
    ARAD_FLD_TO_REG(PPDB_B_LARGE_EM_MNGMNT_REQ_FLU_DB_CONFIGURATIONr,  LARGE_EM_MNGMNT_REQ_ACCESS_FLU_DBf, field_val, reg_val, 50, exit);
    field_val = 0x0;
    ARAD_FLD_TO_REG(PPDB_B_LARGE_EM_MNGMNT_REQ_FLU_DB_CONFIGURATIONr,  LARGE_EM_MNGMNT_REQ_FLU_DB_HIT_MESSAGE_ENf, field_val, reg_val, 60, exit);
    field_val = 0x0;
    ARAD_FLD_TO_REG(PPDB_B_LARGE_EM_MNGMNT_REQ_FLU_DB_CONFIGURATIONr,  LARGE_EM_MNGMNT_REQ_FLU_DB_HIT_MESSAGE_STAMPf, field_val, reg_val, 70, exit);
    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR,WRITE_PPDB_B_LARGE_EM_MNGMNT_REQ_FLU_DB_CONFIGURATIONr(unit, reg_val));

    
    res = arad_pp_frwrd_mact_arad_plus_em_hash_init_unsafe(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

    
    if (SOC_IS_QUX(unit)) {
        field_val = SOC_DPP_DEFS_GET(unit, nof_isem_lines);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_field32_modify(unit, PPDB_A_ISEM_KEY_TABLE_ENTRY_LIMITr, REG_PORT_ANY, ISEM_KEY_TABLE_ENTRY_LIMITf,  field_val));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_field32_modify(unit, PPDB_B_ISEM_KEY_TABLE_ENTRY_LIMITr, REG_PORT_ANY, ISEM_KEY_TABLE_ENTRY_LIMITf,  field_val));
    }
        

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_mact_arad_plus_init_unsafe()", 0, 0);
}
#endif 



uint32
  arad_pp_frwrd_mact_mgmt_init_unsafe(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    fld_val, slb_prefix, size_of_learn_event,
    res = SOC_SAND_OK;
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO
    mac_limit_glbl_info;
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO
    limit_info;
  uint32 host_memory_allocated_bytes;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_MGMT_INIT);
  
  res = arad_pp_frwrd_mact_regs_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  

 
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_clear(&mac_limit_glbl_info);
  mac_limit_glbl_info.static_may_exceed = TRUE;
  mac_limit_glbl_info.cpu_may_exceed = FALSE;
  mac_limit_glbl_info.enable = FALSE;
  mac_limit_glbl_info.glbl_limit = SOC_DPP_DEFS_GET(unit, nof_lem_lines) - 1;
  mac_limit_glbl_info.generate_event = FALSE;

  res = arad_pp_frwrd_mact_mac_limit_glbl_info_set_unsafe(
          unit,
          &mac_limit_glbl_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO_clear(&limit_info);
  limit_info.action_when_exceed = SOC_PPC_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_NONE;
  limit_info.nof_entries = SOC_DPP_DEFS_GET(unit, max_mact_limit);
  limit_info.is_limited = TRUE;

  res = arad_pp_frwrd_mact_learn_profile_limit_info_set_unsafe(
          unit,
          ARAD_PP_FRWRD_MACT_LEARN_LIMIT_DFLT_PROFILE_ID,
          &limit_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 62, exit);

  fld_val = ARAD_PP_FRWRD_MACT_LEM_DB_MACT_ID(unit);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  65,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_PP_1r, REG_PORT_ANY, 0,  MACT_DB_ID_MACT_IDf,  fld_val)); 

  if ((SOC_IS_JERICHO(unit)) && (soc_property_get(unit, spn_RESILIENT_HASH_ENABLE, 0) == 0))
  {
      
      slb_prefix = ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_SLB;

      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  65,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_PP_1r, REG_PORT_ANY, 0,  MACT_DB_ID_PROG_IDf, (slb_prefix >> 1)));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_CONSISTENT_HASHING_CONFIGRATIONr, REG_PORT_ANY, 0, MACT_DB_ID_CONSISTENT_HASHING_IDf, (slb_prefix >> 1)));
  }

  
  fld_val = 1;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_PP_CONFIGr, SOC_CORE_ALL, 0, LEARN_ALL_COPIESf,  fld_val));
  
  fld_val = 0;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_PP_CONFIGr, SOC_CORE_ALL, 0, CUSTOM_LEARNf,  fld_val));

  
  res = arad_pp_frwrd_mact_learn_limit_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  if (SOC_IS_JERICHO(unit))
  {
      
      size_of_learn_event = 4 * BYTES2WORDS(soc_reg_bytes(unit, OLP_HEAD_OF_CPU_DSPG_CMD_FIFOr));
      res = sw_state_access[unit].dpp.soc.arad.pp.fwd_mact.size_of_learn_event.set(unit, size_of_learn_event);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  }

  
  if (!SOC_WARM_BOOT(unit))
  {
      res = sw_state_access[unit].dpp.soc.arad.pp.fwd_mact.host_memory_allocated_bytes.set(unit, 0);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  }
  
  res = sw_state_access[unit].dpp.soc.arad.pp.fwd_mact.host_memory_allocated_bytes.get(unit, &host_memory_allocated_bytes);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  
  if (host_memory_allocated_bytes) 
  {
      g_dma_host_memory[unit] = soc_cm_salloc(unit, host_memory_allocated_bytes, "learning DMA");
      g_next_dma_interrupt_message_num[unit] = 0;
  }

#ifdef BCM_88660_A0
  
  if (SOC_IS_ARADPLUS(unit)) {
      res = arad_pp_frwrd_mact_arad_plus_init_unsafe(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
  }
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_mact_mgmt_init_unsafe()",0,0);
}

STATIC uint32
  arad_pp_frwrd_mact_is_ingress_learning_get(
        SOC_SAND_IN  int                      unit,
        SOC_SAND_IN  SOC_PPC_FRWRD_MACT_LEARNING_MODE learning_mode,
        SOC_SAND_OUT uint8                      *is_ingress_learning
      )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_IS_INGRESS_LEARNING_GET);

  switch(learning_mode)
  {
  case SOC_PPC_FRWRD_MACT_LEARNING_MODE_INGRESS_DISTRIBUTED:
  case SOC_PPC_FRWRD_MACT_LEARNING_MODE_INGRESS_CENTRALIZED:
    *is_ingress_learning = TRUE;
    break;

  case SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_INDEPENDENT:
  case SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_CENTRALIZED:
  case SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_DISTRIBUTED:
    *is_ingress_learning = FALSE;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PPC_FRWRD_MACT_LEARNING_MODE_OUT_OF_RANGE_ERR, 140, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_mact_is_ingress_learning_get()", learning_mode,0);
}

STATIC uint32
  arad_pp_frwrd_mact_opport_mode_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint8                    learn_enable, 
    SOC_SAND_IN  uint8                    is_ingress_learning,
    SOC_SAND_IN  uint32                   opport
  )
{
  uint32
    res = SOC_SAND_OK;
  uint8 
    is_enable_learning,
    slb_enabled = 0,
    sa_auth_enabled = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_INGRESS_LEARN_TYPEr, REG_PORT_ANY, 0,  LARGE_EM_INGRESS_LEARN_TYPEf,  opport));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_LEARNING_CFGr, REG_PORT_ANY, 0, OPPORTUNISTIC_LEARNINGf,  opport));

  
  if (learn_enable == 2) {
      res = arad_pp_flp_ethernet_prog_learn_get(unit,&is_enable_learning);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
  else{
      is_enable_learning = learn_enable;
  }
  res = arad_pp_flp_ethernet_prog_update(unit, is_enable_learning, is_ingress_learning, opport, 0);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_RESILIENT_HASH_ENABLE, 0)) {
      slb_enabled = 1;
  }

  if (soc_property_get(unit, spn_SA_AUTH_ENABLED, 0)) {
      sa_auth_enabled = 1;
  }


#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {    
        
    res = arad_pp_frwrd_mact_arad_plus_sa_lookup_mapping_init_unsafe(unit,(!sa_auth_enabled && !slb_enabled));
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);    
  }
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_mact_opport_mode_set()", opport,0);
}

uint32
  arad_pp_frwrd_mact_lookup_type_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_LOOKUP_TYPE         lookup_type,
    SOC_SAND_IN  uint8                                  learn_enable 
  )
{
  uint32
    opport=0,
    res = SOC_SAND_OK,
    sa_lookup_type_fld_val,
    routed_learning_apps;
  SOC_PPC_FRWRD_MACT_LEARNING_MODE
    learning_mode;
  uint8
    authentication_enable,
    is_ingress_learning;
  uint8 slb_enabled = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  res = sw_state_access[unit].dpp.soc.arad.pp.fwd_mact.learning_mode.get(unit, &learning_mode);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_frwrd_mact_is_ingress_learning_get(
          unit,
          learning_mode,
          &is_ingress_learning
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

  authentication_enable = (lookup_type == SOC_PPC_FRWRD_MACT_LOOKUP_TYPE_SA_AUTH);

  res = arad_pp_frwrd_mact_routed_learning_get_unsafe(unit, &routed_learning_apps);
  SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);

  
  
  
  
#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_RESILIENT_HASH_ENABLE, 0)) {
    SOC_SAND_VERIFY(!authentication_enable);
    SOC_SAND_VERIFY(SOC_PPC_FRWRD_MACT_LOOKUP_TYPE_SLB_LOOKUP == lookup_type);
    slb_enabled = 1;
  }
#endif 

  
  opport = authentication_enable || !is_ingress_learning || (routed_learning_apps != 0) || slb_enabled;

  switch (lookup_type) {  
  case SOC_PPC_FRWRD_MACT_LOOKUP_TYPE_SA_AUTH:
      sa_lookup_type_fld_val = ARAD_PP_FRWRD_MACT_SA_LOOKUP_TYPE_AUTHENTIFICATION_FLD_VAL;
      break;

  case SOC_PPC_FRWRD_MACT_LOOKUP_TYPE_SA_LOOKUP:
      sa_lookup_type_fld_val = ARAD_PP_FRWRD_MACT_SA_LOOKUP_TYPE_SA_LOOKUP_FLD_VAL;
      break;
#ifdef BCM_88660_A0
  case SOC_PPC_FRWRD_MACT_LOOKUP_TYPE_SLB_LOOKUP:
      if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_RESILIENT_HASH_ENABLE, 0)) {
          sa_lookup_type_fld_val = ARAD_PP_FRWRD_MACT_SA_LOOKUP_TYPE_SLB_LOOKUP_FLD_VAL;
      } else {
          SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_MACT_MGMT_TYPE_OUT_OF_RANGE_ERR, 177, exit); 
      }
      break;
#endif 
  case SOC_PPC_FRWRD_MACT_LOOKUP_TYPE_MIM: 
  default:
     SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_MACT_MGMT_TYPE_OUT_OF_RANGE_ERR, 170, exit); 
  }
  res = sw_state_access[unit].dpp.soc.arad.pp.oper_mode.authentication_enable.set(unit, authentication_enable);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 175, exit);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  150,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_PP_7r, REG_PORT_ANY, 0, SA_LOOKUP_TYPEf,  sa_lookup_type_fld_val));

  res = arad_pp_frwrd_mact_opport_mode_set(unit, learn_enable, is_ingress_learning, opport);
  SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_mact_lookup_type_set_unsafe()", lookup_type,0);
}



uint32
  arad_pp_frwrd_mact_oper_mode_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_OPER_MODE_INFO           *oper_mode_info
  )
{
  SOC_PPC_FRWRD_MACT_LOOKUP_TYPE
    sa_lookup_type;
  uint32
    reg_val,    
    dsp_generation_en,
    dsp_delay,
    dsp_nof_cmds,
    fld_val,
    res = SOC_SAND_OK;
  SOC_PPC_FRWRD_MACT_LEARNING_MODE
    learning_mode;
  uint8
    is_centralized,
    is_egr_independent,
    is_ingress_learning,
    sys_phy_port,
    sys_lag_port,
    pph_petra_a_compatible;
  ARAD_PP_FRWRD_MACT_AGING_EVENT_TABLE
    default_conf;
  uint32
    pp_port_ndx,
    is_lag_ndx,
    handle_profile_ndx,
    reg_ndx,
    fld_ndx,
    age_key_ndx,
    last_age_key_ndx;
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY
    event_key,
    event_key_no_mac;
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO
    handle_info,
    handle_info_no_mac;
  ARAD_PP_IHB_PINFO_FLP_TBL_DATA
    tbl_data;
  SOC_PPC_FRWRD_MACT_EVENT_PATH_TYPE
    msg_type[ARAD_PP_FRWRD_MACT_NOF_FIFOS];
  SOC_PPC_FRWRD_MACT_EVENT_TYPE
    event_type_ndx;
  soc_reg_above_64_val_t
    reg_above_64_val;
  uint8    
    authentication_enable;
  soc_reg_t
     event_fifo_configuration_reg = SOC_IS_JERICHO(unit) ? PPDB_B_LARGE_EM_EVENT_FIFO_CONFIGURATIONr : IHP_MACT_EVENT_FIFO_CONFIGURATIONr;
  soc_field_t
    field;
  uint8 dyn_age_pointer_default[ARAD_PP_FRWRD_MACT_NOF_VSI_AGING_PROFILES]; 
  int   core;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_OPER_MODE_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(oper_mode_info);

  
  ARAD_PP_FRWRD_MACT_AGING_EVENT_TABLE_clear(&default_conf);
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY_clear(&event_key);
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY_clear(&event_key_no_mac);
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_clear(&handle_info);
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_clear(&handle_info_no_mac);



  
  learning_mode = oper_mode_info->learning_mode;
  res = sw_state_access[unit].dpp.soc.arad.pp.fwd_mact.learning_mode.set(unit,learning_mode);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 2, exit);
  pph_petra_a_compatible = oper_mode_info->soc_petra_a_compatible;
  res = sw_state_access[unit].dpp.soc.arad.pp.fwd_mact.is_petra_a_compatible.set(unit,pph_petra_a_compatible);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 3, exit);

  
  fld_val = ARAD_PP_FRWRD_MACT_AGING_HIGH_RES_DISABLED || pph_petra_a_compatible;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  5,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_AGE_AGING_RESOLUTIONr, REG_PORT_ANY, 0,  LARGE_EM_AGE_AGING_RESOLUTIONf,  !fld_val));

  
  res = arad_pp_frwrd_mact_age_conf_default_get(
          unit,
          &default_conf
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_frwrd_mact_age_conf_write(
          unit,
          ARAD_PP_FRWRD_MACT_AGE_CONF_PTR_FOR_MAC,
          &default_conf
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  sys_phy_port = TRUE;
  if (pph_petra_a_compatible == TRUE)
  {
    switch(learning_mode)
    {
    case SOC_PPC_FRWRD_MACT_LEARNING_MODE_INGRESS_DISTRIBUTED:
    case SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_DISTRIBUTED:
      sys_lag_port = FALSE;
      is_centralized = FALSE;
      is_egr_independent = FALSE;
      break;

    case SOC_PPC_FRWRD_MACT_LEARNING_MODE_INGRESS_CENTRALIZED:
    case SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_CENTRALIZED:
      sys_lag_port = FALSE;
      is_centralized = TRUE;
      is_egr_independent = FALSE;
      break;

    case SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_INDEPENDENT:
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PPC_FRWRD_MACT_LEARNING_MODE_OUT_OF_RANGE_ERR, 30, exit);
    }
  }
  else
  {
     
    if ((SOC_IS_ARAD_B0_AND_ABOVE(unit)) && 
        SOC_IS_ARADPLUS_AND_BELOW(unit) && 
        (TRUE == SOC_DPP_CONFIG(unit)->tm.is_petrab_in_system)&&
        (
         (SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_DISTRIBUTED == learning_mode)||
         (SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_CENTRALIZED == learning_mode)||
         (SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_INDEPENDENT == learning_mode))
        )
    {
        uint32
            field,
            reg_val;
        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 32, exit, READ_IHP_REG_00CFr(unit, &reg_val));
        field = 0x2;
        soc_reg_field_set(unit, IHP_REG_00CFr, &reg_val, FIELD_5_6f, field);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 34, exit, WRITE_IHP_REG_00CFr(unit, reg_val));            
    }

    switch(learning_mode)
    {
    case SOC_PPC_FRWRD_MACT_LEARNING_MODE_INGRESS_DISTRIBUTED:
    case SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_DISTRIBUTED:
      sys_lag_port = TRUE;
      is_centralized = FALSE;
      is_egr_independent = FALSE;
      break;

    case SOC_PPC_FRWRD_MACT_LEARNING_MODE_INGRESS_CENTRALIZED:
    case SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_CENTRALIZED:
      sys_lag_port = TRUE;
      is_centralized = TRUE;
      is_egr_independent = FALSE;
    break;

    case SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_INDEPENDENT:
      sys_lag_port = TRUE;
      is_centralized = FALSE;
      is_egr_independent = TRUE;
    break;

    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PPC_FRWRD_MACT_LEARNING_MODE_OUT_OF_RANGE_ERR, 40, exit);
    }
  }

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, READ_PPDB_B_LARGE_EM_AGE_AGING_MODEr(unit, reg_above_64_val));

  
  
  age_key_ndx = 0 << 3 | 1 << 2; 
  last_age_key_ndx =  age_key_ndx + ARAD_PP_FRWRD_MACT_NOF_VSI_AGING_PROFILES;

  dyn_age_pointer_default[0] = 1;  
  dyn_age_pointer_default[1] = 1;
  dyn_age_pointer_default[2] = 2;
  dyn_age_pointer_default[3] = 3;

  
  for (; age_key_ndx < last_age_key_ndx; ++age_key_ndx)
  {
    fld_val = ARAD_PP_FRWRD_MACT_AGE_CONF_PTR_FOR_MAC;

    
	fld_val = dyn_age_pointer_default[age_key_ndx - 4]; 
	SHR_BITCOPY_RANGE(reg_above_64_val,age_key_ndx*ARAD_PP_FRWRD_MACT_AGE_AGING_CONF_SIZE, &fld_val, 0, 2);

    fld_val = sys_phy_port;
    SHR_BITCOPY_RANGE(reg_above_64_val,age_key_ndx*ARAD_PP_FRWRD_MACT_AGE_AGING_CONF_SIZE+2, &fld_val, 0, 1);
    fld_val = sys_lag_port;
    SHR_BITCOPY_RANGE(reg_above_64_val,age_key_ndx*ARAD_PP_FRWRD_MACT_AGE_AGING_CONF_SIZE+3, &fld_val, 0, 1);
  }
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, WRITE_PPDB_B_LARGE_EM_AGE_AGING_MODEr(unit, reg_above_64_val));

  
  fld_val = 0x1;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_LEARN_FILTER_PROPERTIESr, REG_PORT_ANY, 0, LARGE_EM_LEARN_FILTER_DROP_DUPLICATEf,  fld_val));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_LOOKUP_FILTER_PROPERTIESr, REG_PORT_ANY, 0, LARGE_EM_LOOKUP_FILTER_DROP_DUPLICATEf,  fld_val));

  
  if (is_egr_independent == TRUE)
  {
    fld_val = ARAD_PP_FRWRD_MACT_LEARN_FILTER_TTL_LOOPBACK;
  }
  else
  {
    fld_val = ARAD_PP_FRWRD_MACT_LEARN_FILTER_TTL_MSGS;
  }
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  105,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_LEARN_FILTER_PROPERTIESr, REG_PORT_ANY, 0, LARGE_EM_LEARN_FILTER_ENTRY_TTLf,  fld_val));


  
  fld_val = 0x0;

#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
      
      if ((oper_mode_info->learn_msgs_info.type == SOC_PPC_FRWRD_MACT_EVENT_PATH_TYPE_DIRECT_ACCESS) &&
          (is_centralized == TRUE)) {
          fld_val = 0x2;
      }
  }
#endif 

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  107,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_LOOKUP_FILTER_PROPERTIESr, REG_PORT_ANY, 0, LARGE_EM_LOOKUP_FILTER_RESf,  fld_val));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  109,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_LEARN_FILTER_PROPERTIESr, REG_PORT_ANY, 0, LARGE_EM_LEARN_FILTER_RESf,  fld_val));

  
  msg_type[ARAD_PP_FRWRD_MACT_FIFO_LEARNING] = oper_mode_info->learn_msgs_info.type;
  msg_type[ARAD_PP_FRWRD_MACT_FIFO_SHADOW] = oper_mode_info->shadow_msgs_info.type;

  
  if (oper_mode_info->learn_msgs_info.type == SOC_PPC_FRWRD_MACT_EVENT_PATH_TYPE_DIRECT_ACCESS)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 110, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit,PPDB_B_LARGE_EM_MNGMNT_REQ_CONFIGURATIONr,REG_PORT_ANY,0,  LARGE_EM_MNGMNT_REQ_OLP_REGISTER_INTERFACE_ENABLEf, 0x0));
    field = SOC_IS_JERICHO(unit) ? LARGE_EM_EVENT_FIFO_ENABLE_OLPf : MACT_EVENT_FIFO_ENABLE_OLPf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 111, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit,event_fifo_configuration_reg, REG_PORT_ANY,0, field, 0x0));
  }
  else
  {
    field = SOC_IS_JERICHO(unit) ? LARGE_EM_EVENT_FIFO_ENABLE_OLPf : MACT_EVENT_FIFO_ENABLE_OLPf;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 113, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit,event_fifo_configuration_reg, REG_PORT_ANY,0, field, 0x1));
  }

  for (reg_ndx = 0; reg_ndx < ARAD_PP_MACT_DSP_ENGINE_CONFIGURATION_REG_NOF_REGS; ++reg_ndx)
  {

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, READ_OLP_DSP_ENGINE_CONFIGURATIONr(unit,reg_ndx,&reg_val));

    switch(msg_type[0]) 
    {
    case SOC_PPC_FRWRD_MACT_EVENT_PATH_TYPE_MSG_SINGLE:
      dsp_generation_en = 0x1;
      dsp_nof_cmds = ARAD_PP_FRWRD_MACT_DSP_NOF_DSP_MSGS_IN_OLP_MSG_SINGLE;
      dsp_delay = 0x20;
      break;

    case SOC_PPC_FRWRD_MACT_EVENT_PATH_TYPE_MSG_AGGR:
      dsp_generation_en = 0x1;
      dsp_nof_cmds = ARAD_PP_FRWRD_MACT_DSP_NOF_DSP_MSGS_IN_OLP_MSG_AGGR;
      dsp_delay = 0x3F;
      break;

    case SOC_PPC_FRWRD_MACT_EVENT_PATH_TYPE_DIRECT_ACCESS:
    default:
      dsp_generation_en = 0x0;
      dsp_nof_cmds = 1; 
      dsp_delay = 0;
      break;
    }
    dsp_nof_cmds = dsp_nof_cmds - 1;

    soc_reg_field_set(unit, OLP_DSP_ENGINE_CONFIGURATIONr, &reg_val, DSP_GENERATION_ENf, dsp_generation_en);
    soc_reg_field_set(unit, OLP_DSP_ENGINE_CONFIGURATIONr, &reg_val, MAX_CMD_DELAYf, dsp_delay);
    soc_reg_field_set(unit, OLP_DSP_ENGINE_CONFIGURATIONr, &reg_val, MAX_DSP_CMDf, dsp_nof_cmds);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, WRITE_OLP_DSP_ENGINE_CONFIGURATIONr(unit,reg_ndx,reg_val));

    
    if ((oper_mode_info->learn_msgs_info.info.header_type != SOC_PPC_NOF_FRWRD_MACT_MSG_HDR_TYPES) && (reg_ndx == ARAD_PP_FRWRD_MACT_FIFO_LEARNING))
    {
      res = arad_pp_frwrd_mact_learn_msgs_distribution_info_set_unsafe(
              unit,
              &(oper_mode_info->learn_msgs_info.info)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
    }
    
    else if ((oper_mode_info->shadow_msgs_info.info.header_type != SOC_PPC_NOF_FRWRD_MACT_MSG_HDR_TYPES)  && (reg_ndx == ARAD_PP_FRWRD_MACT_FIFO_SHADOW))
    {
      
      res = arad_pp_frwrd_mact_shadow_msgs_distribution_info_set_unsafe(
              unit,
              &(oper_mode_info->shadow_msgs_info.info)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
    }
  }

  
  for (reg_ndx = 0; reg_ndx < ARAD_PP_MACT_DSP_ENGINE_CONFIGURATION_REG_NOF_REGS; ++reg_ndx)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 165, exit, READ_OLP_DSP_ENGINE_CONFIGURATIONr(unit,reg_ndx,&reg_val));
    fld_val = ARAD_PP_FRWRD_MACT_DSP_MSG_MIN_SIZE;
    soc_reg_field_set(unit, OLP_DSP_ENGINE_CONFIGURATIONr, &reg_val, MIN_DSPf, fld_val);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 166, exit, WRITE_OLP_DSP_ENGINE_CONFIGURATIONr(unit,reg_ndx,reg_val));
  }

   
  res = arad_pp_frwrd_mact_is_ingress_learning_get(
          unit,
          learning_mode,
          &is_ingress_learning
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 167, exit);  

  res = sw_state_access[unit].dpp.soc.arad.pp.oper_mode.authentication_enable.get(unit, &authentication_enable);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 168, exit);

  if (authentication_enable) 
  {
      sa_lookup_type = SOC_PPC_FRWRD_MACT_LOOKUP_TYPE_SA_AUTH;
  }
  else {
      sa_lookup_type = SOC_PPC_FRWRD_MACT_LOOKUP_TYPE_SA_LOOKUP;
  }

#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_RESILIENT_HASH_ENABLE, 0)) {
    sa_lookup_type = SOC_PPC_FRWRD_MACT_LOOKUP_TYPE_SLB_LOOKUP;
  }
#endif 


  res = arad_pp_frwrd_mact_lookup_type_set_unsafe(unit, sa_lookup_type, !oper_mode_info->disable_learning);
  SOC_SAND_CHECK_FUNC_RESULT(res, 168, exit);

  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 170, exit, READ_PPDB_B_LARGE_EM_LOOKUP_LOOKUP_MODEr(unit,&reg_val));
  
  for (fld_ndx = 0; fld_ndx < 4; ++fld_ndx)
  {
      
    if (fld_ndx == ARAD_PP_FRWRD_MACT_LEM_DB_MACT_ID(unit) || fld_ndx != ARAD_PP_FRWRD_MACT_LEM_DB_MACT_ID(unit))
    {
      if (is_ingress_learning == TRUE)
      {
        fld_val = ARAD_PP_FRWRD_MACT_LOOKUP_MODE_TRANSPLANT_FLD_VAL;
      }
      else
      {
        fld_val = ARAD_PP_FRWRD_MACT_LOOKUP_MODE_TRANSPLANT_AND_REFRESH_FLD_VAL;
      }


      
      SHR_BITCOPY_RANGE(&reg_val,fld_ndx*5, &fld_val, 0, 2);
      fld_val = 0;
      SHR_BITCOPY_RANGE(&reg_val,fld_ndx*5+2, &fld_val, 0, 2);
      fld_val = 0x1;
      SHR_BITCOPY_RANGE(&reg_val,fld_ndx*5+4, &fld_val, 0, 1);
    }
  }
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 210, exit, WRITE_PPDB_B_LARGE_EM_LOOKUP_LOOKUP_MODEr(unit,reg_val));

  
  
  for (handle_profile_ndx = 0; handle_profile_ndx < 1; ++handle_profile_ndx)
  {
    event_key.vsi_event_handle_profile = handle_profile_ndx;
    for (is_lag_ndx = 0; is_lag_ndx < SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY_NOF_IS_LAGS; ++is_lag_ndx)
    {
      event_key.is_lag = SOC_SAND_NUM2BOOL(is_lag_ndx);
      if (is_centralized == TRUE)
      {
        
        event_key.event_type = SOC_PPC_FRWRD_MACT_EVENT_TYPE_TRANSPLANT + SOC_PPC_FRWRD_MACT_EVENT_TYPE_LEARN
          + SOC_PPC_FRWRD_MACT_EVENT_TYPE_REQ_FAIL + SOC_PPC_FRWRD_MACT_EVENT_TYPE_ACK + SOC_PPC_FRWRD_MACT_EVENT_TYPE_AGED_OUT;
        handle_info.self_learning = FALSE;
        handle_info.send_to_learning_fifo = TRUE;
        handle_info.send_to_shadow_fifo = FALSE;
        handle_info.send_to_cpu_dma_fifo = FALSE;
        res = arad_pp_frwrd_mact_event_handle_info_set_unsafe(
                unit,
                &event_key,
                &handle_info
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 220, exit);
      }
      else if (is_egr_independent == TRUE)
      {
        
        event_key.event_type = SOC_PPC_FRWRD_MACT_EVENT_TYPE_TRANSPLANT + SOC_PPC_FRWRD_MACT_EVENT_TYPE_LEARN;
        if (is_lag_ndx)
        {
          event_key.event_type += SOC_PPC_FRWRD_MACT_EVENT_TYPE_REFRESH + SOC_PPC_FRWRD_MACT_EVENT_TYPE_AGED_OUT;
        }
        handle_info.self_learning = TRUE;
        handle_info.send_to_learning_fifo = SOC_SAND_NUM2BOOL(is_lag_ndx);
        handle_info.send_to_shadow_fifo = FALSE;
        handle_info.send_to_cpu_dma_fifo = FALSE;
        res = arad_pp_frwrd_mact_event_handle_info_set_unsafe(
                unit,
                &event_key,
                &handle_info
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit);
      }
      else 
      {
        
        event_key.event_type = SOC_PPC_FRWRD_MACT_EVENT_TYPE_TRANSPLANT + SOC_PPC_FRWRD_MACT_EVENT_TYPE_LEARN + SOC_PPC_FRWRD_MACT_EVENT_TYPE_REFRESH + SOC_PPC_FRWRD_MACT_EVENT_TYPE_REQ_FAIL;
        handle_info.self_learning = FALSE;
        handle_info.send_to_learning_fifo = TRUE;
        handle_info.send_to_shadow_fifo = FALSE;
        handle_info.send_to_cpu_dma_fifo = FALSE;
        res = arad_pp_frwrd_mact_event_handle_info_set_unsafe(
                unit,
                &event_key,
                &handle_info
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 240, exit);

        event_key.event_type = SOC_PPC_FRWRD_MACT_EVENT_TYPE_AGED_OUT;
        handle_info.self_learning = FALSE;
        handle_info.send_to_learning_fifo = FALSE;
        handle_info.send_to_shadow_fifo = FALSE;
        handle_info.send_to_cpu_dma_fifo = FALSE;
        res = arad_pp_frwrd_mact_event_handle_info_set_unsafe(
                unit,
                &event_key,
                &handle_info
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 240, exit);
      }
    }
  }
  
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_clear(&handle_info);
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY_clear(&event_key);
  for (handle_profile_ndx = 0; handle_profile_ndx < 1; ++handle_profile_ndx)
  {
    event_key.vsi_event_handle_profile = handle_profile_ndx;
    for (is_lag_ndx = 0; is_lag_ndx < SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY_NOF_IS_LAGS; ++is_lag_ndx)
    {
      event_key.is_lag = SOC_SAND_NUM2BOOL(is_lag_ndx);
      for (event_type_ndx = 0; event_type_ndx < SOC_PPC_NOF_FRWRD_MACT_EVENT_TYPES; ++event_type_ndx)
      {
          event_key.event_type = SOC_SAND_BIT(event_type_ndx);
        if ((event_key.event_type == SOC_PPC_FRWRD_MACT_EVENT_TYPE_TRANSPLANT)
           ||(event_key.event_type == SOC_PPC_FRWRD_MACT_EVENT_TYPE_AGED_OUT)
           ||(event_key.event_type == SOC_PPC_FRWRD_MACT_EVENT_TYPE_LEARN)
           )
        {
          res = arad_pp_frwrd_mact_event_handle_info_get_unsafe(
                  unit,
                  &event_key,
                  &handle_info
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 280, exit);

          
          if ((oper_mode_info->shadow_mode == SOC_PPC_FRWRD_MACT_SHADOW_MODE_NONE)
              || ((oper_mode_info->shadow_mode == SOC_PPC_FRWRD_MACT_SHADOW_MODE_LAG) && (is_lag_ndx == 0)))
          {
            handle_info.send_to_shadow_fifo = FALSE;
          }
          else
          {
            handle_info.send_to_shadow_fifo = TRUE;
          }
          if ((event_key.event_type == SOC_PPC_FRWRD_MACT_EVENT_TYPE_LEARN)
            && (oper_mode_info->shadow_mode == SOC_PPC_FRWRD_MACT_SHADOW_MODE_ARP))
          {
            handle_info.send_to_shadow_fifo = FALSE;
          }

          res = arad_pp_frwrd_mact_event_handle_info_set_unsafe(
                  unit,
                  &event_key,
                  &handle_info
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 290, exit);
        }
      }
    }
  }

  
  SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
      for (pp_port_ndx = 0; pp_port_ndx <= ARAD_PP_PORT_MAX; ++pp_port_ndx)
      {
        res = arad_pp_ihb_pinfo_flp_tbl_get_unsafe(
                unit,
                core,
                pp_port_ndx,
                &tbl_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 320, exit);
        tbl_data.learn_enable = TRUE;
        res = arad_pp_ihb_pinfo_flp_tbl_set_unsafe(
                unit,
                core,
                pp_port_ndx,
                &tbl_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 330, exit);
      }
  }
   
  fld_val = is_ingress_learning;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  160,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_LEARNING_CFGr, REG_PORT_ANY, 0, INGRESS_LEARNINGf,  fld_val));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_oper_mode_info_set_unsafe()", 0, 0);
}


soc_error_t
  arad_pp_frwrd_mact_cpu_counter_learn_limit_set(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint8  disable          
  )
{
  uint32
    field_val;    
   
  SOCDNX_INIT_FUNC_DEFS;

  if (disable) {
    field_val = 0;
  } else {
    field_val = 0x1;
  }

  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_COUNTER_LIMIT_CONFIGURATIONr, REG_PORT_ANY, 0,  LARGE_EM_FID_DB_LEARN_EVENT_CHECK_FID_LIMITf,  field_val));
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_COUNTER_LIMIT_CONFIGURATIONr, REG_PORT_ANY, 0,  LARGE_EM_FID_DB_LEARN_EVENT_CHECK_LARGE_EM_DB_LIMITf,  field_val));

exit:
  SOCDNX_FUNC_RETURN;
}


soc_error_t
  arad_pp_frwrd_mact_cpu_counter_learn_limit_get(
    SOC_SAND_IN  int    unit,
    SOC_SAND_OUT uint8* is_enabled
  )
{
  uint32
    mact_fid_db_learn_event_check_fid_limit;
  
  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(is_enabled);
  
  
  SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_COUNTER_LIMIT_CONFIGURATIONr, REG_PORT_ANY, 0,  LARGE_EM_FID_DB_LEARN_EVENT_CHECK_FID_LIMITf, &mact_fid_db_learn_event_check_fid_limit));

  (*is_enabled) = (!mact_fid_db_learn_event_check_fid_limit);
  
exit:
  SOCDNX_FUNC_RETURN;
}


uint32
  arad_pp_frwrd_mact_oper_mode_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_OPER_MODE_INFO           *oper_mode_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_OPER_MODE_INFO_SET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_MACT_OPER_MODE_INFO, oper_mode_info, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_oper_mode_info_set_verify()", 0, 0);
}


uint32
  arad_pp_frwrd_mact_oper_mode_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_OPER_MODE_INFO           *oper_mode_info
  )
{
  uint32
    dsp_generation_en,
    dsp_nof_cmds,
    res = SOC_SAND_OK;
  uint32
    reg_ndx,
    is_lag_ndx,
    handle_profile_ndx;
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY
    event_key;
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO
    handle_info;
  uint8
    is_shadow_always_false,
    is_shadow_always_false_for_other_events,
    is_shadow_aged_out_transplant_always_true,
    is_shadow_aged_out_transplant_lag_always_true,
    is_shadow_aged_out_transplant_not_lag_always_false,
    is_shadow_learned_always_false,
    is_shadow_learned_always_true,
    is_shadow_learned_not_lag_always_false,
    is_shadow_learned_lag_always_true,
    is_ingress_learning,
    is_enable_learning;
  SOC_PPC_FRWRD_MACT_LEARNING_MODE
    learning_mode;
  SOC_PPC_FRWRD_MACT_EVENT_PATH_TYPE
    msg_type[ARAD_PP_FRWRD_MACT_NOF_FIFOS];
  uint32
      reg_val;
  SOC_PPC_FRWRD_MACT_EVENT_TYPE
    event_type_ndx;
  uint8 cpu_enabled;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_OPER_MODE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(oper_mode_info);

  SOC_PPC_FRWRD_MACT_OPER_MODE_INFO_clear(oper_mode_info);

  

  
  res = sw_state_access[unit].dpp.soc.arad.pp.fwd_mact.learning_mode.get(unit, &oper_mode_info->learning_mode);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 3, exit);

  res = sw_state_access[unit].dpp.soc.arad.pp.fwd_mact.is_petra_a_compatible.get(unit, &(oper_mode_info->soc_petra_a_compatible));
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

  res = arad_pp_flp_ethernet_prog_learn_get(unit,&is_enable_learning);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  oper_mode_info->disable_learning = !is_enable_learning;

  for (reg_ndx = 0; reg_ndx < ARAD_PP_MACT_DSP_ENGINE_CONFIGURATION_REG_NOF_REGS; ++reg_ndx)
  {   
	SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, READ_OLP_DSP_ENGINE_CONFIGURATIONr(unit, reg_ndx, &reg_val));

    if (SOC_IS_JERICHO(unit)) 
    {
        
        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, arad_pp_frwrd_mact_is_cpu_mode_enabled(unit, &cpu_enabled));
        dsp_generation_en = !cpu_enabled;
    }
    else
    {
        dsp_generation_en = soc_reg_field_get(unit, OLP_DSP_ENGINE_CONFIGURATIONr,reg_val, DSP_GENERATION_ENf);
    }

    dsp_nof_cmds = soc_reg_field_get(unit, OLP_DSP_ENGINE_CONFIGURATIONr,reg_val, MAX_DSP_CMDf);
    if (dsp_generation_en == 0x0)
    {
      msg_type[reg_ndx] = SOC_PPC_FRWRD_MACT_EVENT_PATH_TYPE_DIRECT_ACCESS;
    }
    else
    {
      switch(dsp_nof_cmds)
      {
      case ARAD_PP_FRWRD_MACT_DSP_NOF_DSP_MSGS_IN_OLP_MSG_SINGLE - 1:
        msg_type[reg_ndx] = SOC_PPC_FRWRD_MACT_EVENT_PATH_TYPE_MSG_SINGLE;
        break;

      case ARAD_PP_FRWRD_MACT_DSP_NOF_DSP_MSGS_IN_OLP_MSG_AGGR - 1:
        msg_type[reg_ndx] = SOC_PPC_FRWRD_MACT_EVENT_PATH_TYPE_MSG_AGGR;
        break;

      default:
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_MACT_MGMT_TYPE_OUT_OF_RANGE_ERR, 30, exit);
      }
    }
  }

  oper_mode_info->learn_msgs_info.type = msg_type[ARAD_PP_FRWRD_MACT_FIFO_LEARNING];
  oper_mode_info->shadow_msgs_info.type = msg_type[ARAD_PP_FRWRD_MACT_FIFO_SHADOW];

  
  res = arad_pp_frwrd_mact_learn_msgs_distribution_info_get_unsafe(
          unit,
          &(oper_mode_info->learn_msgs_info.info)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  
  res = arad_pp_frwrd_mact_shadow_msgs_distribution_info_get_unsafe(
          unit,
          &(oper_mode_info->shadow_msgs_info.info)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_clear(&handle_info);
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY_clear(&event_key);
  
  res = sw_state_access[unit].dpp.soc.arad.pp.fwd_mact.learning_mode.get(unit, &learning_mode);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 55, exit);

  res = arad_pp_frwrd_mact_is_ingress_learning_get(
          unit,
          learning_mode,
          &is_ingress_learning
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  

  is_shadow_always_false = TRUE;
  is_shadow_always_false_for_other_events = TRUE;
  is_shadow_aged_out_transplant_always_true = TRUE;
  is_shadow_aged_out_transplant_lag_always_true = TRUE;
  is_shadow_aged_out_transplant_not_lag_always_false = TRUE;
  is_shadow_learned_always_false = TRUE;
  is_shadow_learned_always_true = TRUE;
  is_shadow_learned_not_lag_always_false = TRUE;
  is_shadow_learned_lag_always_true = TRUE;

  for (handle_profile_ndx = 0; handle_profile_ndx < 1; ++handle_profile_ndx)
  {
    event_key.vsi_event_handle_profile = handle_profile_ndx;
    for (is_lag_ndx = 0; is_lag_ndx < SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY_NOF_IS_LAGS; ++is_lag_ndx)
    {
      event_key.is_lag = SOC_SAND_NUM2BOOL(is_lag_ndx);
      for (event_type_ndx = 0; event_type_ndx < SOC_PPC_NOF_FRWRD_MACT_EVENT_TYPES; ++event_type_ndx)
      {
        if ((event_type_ndx == SOC_PPC_FRWRD_MACT_EVENT_TYPE_TRANSPLANT)
          ||(event_type_ndx == SOC_PPC_FRWRD_MACT_EVENT_TYPE_AGED_OUT)
          ||(event_type_ndx == SOC_PPC_FRWRD_MACT_EVENT_TYPE_LEARN)
          )
        {
          event_key.event_type = event_type_ndx;
          res = arad_pp_frwrd_mact_event_handle_info_get_unsafe(
                  unit,
                  &event_key,
                  &handle_info
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

          if ((event_type_ndx == SOC_PPC_FRWRD_MACT_EVENT_TYPE_TRANSPLANT)
             ||(event_type_ndx == SOC_PPC_FRWRD_MACT_EVENT_TYPE_AGED_OUT)
             )
          {
            if (handle_info.send_to_shadow_fifo == FALSE)
            {
              is_shadow_aged_out_transplant_always_true = FALSE;

            }
            if ((handle_info.send_to_shadow_fifo == FALSE) && (is_lag_ndx == TRUE))
            {
              is_shadow_aged_out_transplant_lag_always_true = FALSE;

            }
            else if ((handle_info.send_to_shadow_fifo == TRUE) && (is_lag_ndx == FALSE))
            {
              is_shadow_aged_out_transplant_not_lag_always_false = FALSE;
            }
          }
          else if (event_type_ndx == SOC_PPC_FRWRD_MACT_EVENT_TYPE_LEARN)
          {
            if ((handle_info.send_to_shadow_fifo == FALSE) && ((is_lag_ndx == TRUE)))
            {
              is_shadow_learned_lag_always_true = FALSE;

            }
            else if ((handle_info.send_to_shadow_fifo == TRUE) && ((is_lag_ndx == FALSE)))
            {
              is_shadow_learned_not_lag_always_false = FALSE;
            }
            if (handle_info.send_to_shadow_fifo == TRUE)
            {
              is_shadow_learned_always_false = FALSE;

            }
            else
            {
              is_shadow_learned_always_true = FALSE;
            }
          }

          if (handle_info.send_to_shadow_fifo == TRUE)
          {
            is_shadow_always_false = FALSE;
          }
        }
      }
    }
  }

  
  if (is_shadow_always_false == TRUE)
  {
    oper_mode_info->shadow_mode = SOC_PPC_FRWRD_MACT_SHADOW_MODE_NONE;
  }
  else if (is_shadow_always_false_for_other_events && is_shadow_aged_out_transplant_always_true
           &&  is_shadow_learned_always_false)
  {
    oper_mode_info->shadow_mode = SOC_PPC_FRWRD_MACT_SHADOW_MODE_ARP;
  }
  else if (is_shadow_always_false_for_other_events
            && is_shadow_aged_out_transplant_lag_always_true
            &&  is_shadow_aged_out_transplant_not_lag_always_false
            && is_shadow_learned_lag_always_true
            && is_shadow_learned_not_lag_always_false)
  {
    oper_mode_info->shadow_mode = SOC_PPC_FRWRD_MACT_SHADOW_MODE_LAG;
  }
  else if (is_shadow_always_false_for_other_events
            && is_shadow_aged_out_transplant_always_true
            && is_shadow_learned_always_true)
  {
    oper_mode_info->shadow_mode = SOC_PPC_FRWRD_MACT_SHADOW_MODE_ALL;
  }
  else
  {
    
    oper_mode_info->shadow_mode = SOC_PPC_NOF_FRWRD_MACT_SHADOW_MODES;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_oper_mode_info_get_unsafe()", 0, 0);
}


uint32
  arad_pp_frwrd_mact_aging_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_AGING_INFO               *aging_info
  )
{
  uint32
    fld_val,
    ticks_per_microsec,
    entry_decrease_in_microsec,
    entry_decrease_in_16_clks,
    max_age,
    res = SOC_SAND_OK;
   
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_AGING_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(aging_info);

  
  ticks_per_microsec = arad_chip_mega_ticks_per_sec_get(unit);

  
  res = arad_pp_frwrd_mact_max_age_default_get(
          unit,
          &max_age
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  entry_decrease_in_microsec = (aging_info->aging_time.mili_sec
    + aging_info->aging_time.sec * ARAD_PP_FRWRD_MACT_NOF_MS_IN_SEC)
    * ARAD_PP_FRWRD_MACT_NOF_MICROSEC_IN_MS / max_age;
  entry_decrease_in_16_clks = (entry_decrease_in_microsec / ARAD_PP_FRWRD_MACT_16_CLOCKS_RESOLUTION) * ticks_per_microsec;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_AGE_MACHINE_META_CYCLEr, REG_PORT_ANY, 0,  LARGE_EM_AGE_MACHINE_META_CYCLEf,  entry_decrease_in_16_clks));

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  25,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_AGE_MACHINE_CONFIGURATIONr, REG_PORT_ANY, 0,  LARGE_EM_AGE_MACHINE_ACCESS_SHAPERf,  64));

  
  if (aging_info->enable_aging == TRUE)
  {
    fld_val = 0x1;
  }
  else
  {
    fld_val = 0x0;
  }
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_AGE_MACHINE_CONFIGURATIONr, REG_PORT_ANY, 0,  LARGE_EM_AGE_AGING_ENABLEf,  fld_val));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_aging_info_set_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_mact_aging_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_AGING_INFO               *aging_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_AGING_INFO_SET_VERIFY);

  res = SOC_PPC_FRWRD_MACT_AGING_INFO_verify(unit, aging_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_aging_info_set_verify()", 0, 0);
}


uint32
  arad_pp_frwrd_mact_aging_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_AGING_INFO               *aging_info
  )
{
  uint32
    fld_val,
    ticks_per_microsec,
    entry_decrease_in_microsec,
    entry_decrease_in_16_clks,
    max_age,
    res = SOC_SAND_OK;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_AGING_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(aging_info);

  SOC_PPC_FRWRD_MACT_AGING_INFO_clear(aging_info);

  

  
  ticks_per_microsec = arad_chip_mega_ticks_per_sec_get(unit);

 
  res = arad_pp_frwrd_mact_max_age_default_get(
          unit,
          &max_age
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_AGE_MACHINE_META_CYCLEr, REG_PORT_ANY, 0,  LARGE_EM_AGE_MACHINE_META_CYCLEf, &entry_decrease_in_16_clks));
  entry_decrease_in_microsec =
    (entry_decrease_in_16_clks / ticks_per_microsec) * ARAD_PP_FRWRD_MACT_16_CLOCKS_RESOLUTION;
  aging_info->aging_time.sec = (entry_decrease_in_microsec * max_age)
    / (ARAD_PP_FRWRD_MACT_NOF_MICROSEC_IN_MS * ARAD_PP_FRWRD_MACT_NOF_MS_IN_SEC);
  aging_info->aging_time.mili_sec = ((entry_decrease_in_microsec * max_age) / ARAD_PP_FRWRD_MACT_NOF_MICROSEC_IN_MS)
    % ARAD_PP_FRWRD_MACT_NOF_MS_IN_SEC;

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_AGE_MACHINE_CONFIGURATIONr, REG_PORT_ANY, 0,  LARGE_EM_AGE_AGING_ENABLEf, &fld_val));
  if (fld_val == TRUE)
  {
    aging_info->enable_aging = TRUE;
  }
  else
  {
    aging_info->enable_aging = FALSE;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_aging_info_get_unsafe()", 0, 0);
}



uint32
  arad_pp_frwrd_mact_aging_one_pass_set_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_AGING_ONE_PASS_INFO   *pass_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE             *success
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
      fld_val;
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_AGING_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pass_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  5,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_AGE_MACHINE_CONFIGURATIONr, REG_PORT_ANY, 0,  LARGE_EM_AGE_AGING_ENABLEf, &fld_val));
  
  if(fld_val) {
      *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
      goto exit;
  }

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_AGE_MACHINE_CURRENT_INDEXr, REG_PORT_ANY, 0,  LARGE_EM_AGE_MACHINE_CURRENT_INDEXf, &fld_val));
  if(fld_val) {
      *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES_2;
      goto exit;
  }
  
  
  fld_val = DPP_PP_LEM_NOF_ENTRIES(unit); 
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_AGE_MACHINE_CURRENT_INDEXr, REG_PORT_ANY, 0,  LARGE_EM_AGE_MACHINE_CURRENT_INDEXf,  fld_val));
  *success = SOC_SAND_SUCCESS;

  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_aging_one_pass_set_unsafe()", 0, 0);
}



uint32
  arad_pp_frwrd_mact_aging_events_handle_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE            *aging_info
  )
{
  uint32
    age_ndx,
    fld_val,
    res = SOC_SAND_OK;
   
    
  ARAD_PP_FRWRD_MACT_AGING_MODIFICATION
    age_modif;
  ARAD_PP_FRWRD_MACT_AGING_EVENT_TABLE
    default_conf;
  uint8
    is_owned,
    is_age_resolution_low;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(aging_info);

  ARAD_PP_FRWRD_MACT_AGING_MODIFICATION_clear(&age_modif);
  ARAD_PP_FRWRD_MACT_AGING_EVENT_TABLE_clear(&default_conf);


 
  
  fld_val = (aging_info->clear_hit_bit == 0)? 0x2:1;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  28,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_DIAGNOSTICS_ACCESSED_MODEr, REG_PORT_ANY, 0,  LARGE_EM_DIAGNOSTICS_ACCESSED_MODEf,  fld_val));
  
  if (aging_info->event_when_aged_out == TRUE)
  {
    fld_val = 0x0;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_AGE_MACHINE_CONFIGURATIONr, REG_PORT_ANY, 0,  LARGE_EM_AGE_STAMPf,  fld_val));
  }

  
  res = arad_pp_frwrd_mact_age_modify_get(
          unit,
          &age_modif
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_frwrd_mact_age_conf_default_get(
          unit,
          &default_conf
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  for (is_owned = 0; is_owned < ARAD_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED; ++is_owned)
  {
    age_ndx = age_modif.age_delete[is_owned];
    default_conf.age_action[age_ndx][is_owned].deleted = aging_info->delete_internally;
    age_ndx = age_modif.age_aged_out[is_owned];
    default_conf.age_action[age_ndx][is_owned].aged_out = aging_info->event_when_aged_out;
    age_ndx = age_modif.age_refresh[is_owned];
    default_conf.age_action[age_ndx][is_owned].refreshed = aging_info->event_when_refreshed;
    res = arad_pp_frwrd_mact_is_age_resolution_low(unit, &is_age_resolution_low);
    SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
    if (is_age_resolution_low)
    {
      age_ndx = age_modif.age_delete[is_owned] + 1;
      default_conf.age_action[age_ndx][is_owned].deleted = aging_info->delete_internally;
      age_ndx = age_modif.age_aged_out[is_owned] + 1;
      default_conf.age_action[age_ndx][is_owned].aged_out = aging_info->event_when_aged_out;
      age_ndx = age_modif.age_refresh[is_owned] + 1;
      default_conf.age_action[age_ndx][is_owned].refreshed = aging_info->event_when_refreshed;
    }
  }

  
  res = arad_pp_frwrd_mact_age_conf_write(
          unit,
          ARAD_PP_FRWRD_MACT_AGE_CONF_PTR_FOR_MAC,
          &default_conf
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_aging_events_handle_info_set_unsafe()", 0, 0);
}
uint32
  arad_pp_frwrd_mact_aging_events_handle_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE            *aging_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE, aging_info, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_aging_events_handle_info_set_verify()", 0, 0);
}

soc_error_t arad_pp_aging_num_of_cycles_get (
    SOC_SAND_IN int     unit,
    SOC_SAND_IN uint32  aging_profile,
    SOC_SAND_OUT int     *fid_aging_cycles
)
{
    uint32 res;
    int32 age_ndx;
    ARAD_PP_IHP_MACT_AGING_CONFIGURATION_TABLE_TBL_DATA tbl_data;
    uint8 delete_event_sent, aged_out_event_sent;
    uint32 max_age;
    uint32 line_number;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_AGING_NUM_OF_CYCLES_GET);

    res = arad_pp_frwrd_mact_max_age_default_get(
            unit,
            &max_age
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    for (age_ndx = (ARAD_PP_FRWRD_MACT_ENTRY_NOF_AGES-2); age_ndx >= 0; --age_ndx)
    {
        arad_pp_frwrd_mact_sw_age_to_aging_config_line(unit,
                                                 &line_number,
                                                 age_ndx,
                                                 aging_profile,
                                                 1);

        res = arad_pp_ihp_mact_aging_configuration_table_tbl_get_unsafe(
              unit,
              line_number,
              &tbl_data
            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10 + age_ndx, exit);

        delete_event_sent = SOC_SAND_NUM2BOOL(tbl_data.aging_cfg_info_delete_entry);
        aged_out_event_sent = SOC_SAND_NUM2BOOL(tbl_data.aging_cfg_info_create_aged_out_event);

        if (delete_event_sent == TRUE ||
          aged_out_event_sent == TRUE) {
          break;
        }
    }

    
    if (age_ndx < 0) {
        *fid_aging_cycles = 0;
    }
    else
    {
        *fid_aging_cycles = max_age - age_ndx;
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_aging_num_of_cycles_get()", 0, 0);
}

uint32
  arad_pp_frwrd_mact_aging_events_handle_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE            *aging_info
  )
{
  uint32
    age_ndx,
    fld_val,
    res = SOC_SAND_OK;
  ARAD_PP_FRWRD_MACT_AGING_EVENT_TABLE
    conf;
  ARAD_PP_FRWRD_MACT_AGING_MODIFICATION
    age_modif;
  uint8
    is_owned;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(aging_info);
  SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE_clear(aging_info);

  ARAD_PP_FRWRD_MACT_AGING_EVENT_TABLE_clear(&conf);
  ARAD_PP_FRWRD_MACT_AGING_MODIFICATION_clear(&age_modif);


  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  28,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_DIAGNOSTICS_ACCESSED_MODEr, REG_PORT_ANY, 0,  LARGE_EM_DIAGNOSTICS_ACCESSED_MODEf, &fld_val));
  aging_info->clear_hit_bit = (fld_val == 2)? 0:1;
  
  res = arad_pp_frwrd_mact_age_conf_read(
          unit,
          &conf
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  res = arad_pp_frwrd_mact_age_modify_get(
          unit,
          &age_modif
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  is_owned = FALSE;
  age_ndx = age_modif.age_delete[is_owned];
  aging_info->delete_internally = conf.age_action[age_ndx][is_owned].deleted;
  age_ndx = age_modif.age_aged_out[is_owned];
  aging_info->event_when_aged_out = conf.age_action[age_ndx][is_owned].aged_out;
  age_ndx = age_modif.age_refresh[is_owned];
  aging_info->event_when_refreshed = conf.age_action[age_ndx][is_owned].refreshed;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_aging_events_handle_info_get_unsafe()", 0, 0);
}


uint32
  arad_pp_frwrd_mact_fid_profile_to_fid_map_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  fid_profile_ndx,
    SOC_SAND_IN  SOC_PPC_FID                                fid
  )
{
  uint32
    entry_offset,
    res = SOC_SAND_OK;
  ARAD_PP_IHP_FID_CLASS_2_FID_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET_UNSAFE);

  
  entry_offset = fid_profile_ndx;
  tbl_data.fid = fid;

  res = arad_pp_ihp_fid_class_2_fid_tbl_set_unsafe(
          unit,
          entry_offset,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_fid_profile_to_fid_map_set_unsafe()", fid_profile_ndx, 0);
}

uint32
  arad_pp_frwrd_mact_fid_profile_to_fid_map_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  fid_profile_ndx,
    SOC_SAND_IN  SOC_PPC_FID                                fid
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET_VERIFY);

  
  
  SOC_SAND_ERR_IF_OUT_OF_RANGE(fid_profile_ndx, ARAD_PP_FRWRD_MACT_FID_PROFILE_NDX_MIN, ARAD_PP_FRWRD_MACT_FID_PROFILE_NDX_MAX, ARAD_PP_FRWRD_MACT_FID_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(fid, SOC_DPP_DEFS_GET(unit, nof_fids), SOC_PPC_FID_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_fid_profile_to_fid_map_set_verify()", fid_profile_ndx, 0);
}

uint32
  arad_pp_frwrd_mact_fid_profile_to_fid_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  fid_profile_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET_VERIFY);

  
  
  SOC_SAND_ERR_IF_OUT_OF_RANGE(fid_profile_ndx, ARAD_PP_FRWRD_MACT_FID_PROFILE_NDX_MIN, ARAD_PP_FRWRD_MACT_FID_PROFILE_NDX_MAX, ARAD_PP_FRWRD_MACT_FID_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_fid_profile_to_fid_map_get_verify()", fid_profile_ndx, 0);
}


uint32
  arad_pp_frwrd_mact_fid_profile_to_fid_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  fid_profile_ndx,
    SOC_SAND_OUT SOC_PPC_FID                                 *fid
  )
{
  uint32
    entry_offset,
    res = SOC_SAND_OK;
  ARAD_PP_IHP_FID_CLASS_2_FID_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(fid);

  
  entry_offset = fid_profile_ndx;

  res = arad_pp_ihp_fid_class_2_fid_tbl_get_unsafe(
          unit,
          entry_offset,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *fid = tbl_data.fid;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_fid_profile_to_fid_map_get_unsafe()", fid_profile_ndx, 0);
}


uint32
  arad_pp_frwrd_mact_mac_limit_glbl_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO           *limit_info
  )
{
  uint32
    fld_val,
    reg_val,
    res = SOC_SAND_OK;
   

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(limit_info);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit,READ_PPDB_B_LARGE_EM_COUNTER_LIMIT_CONFIGURATIONr(unit, &reg_val));

  
  fld_val = limit_info->enable;
  soc_reg_field_set(unit, PPDB_B_LARGE_EM_COUNTER_LIMIT_CONFIGURATIONr, &reg_val,  LARGE_EM_FID_DB_CHECK_FID_LIMITf, fld_val);

  
  fld_val = limit_info->static_may_exceed;
  soc_reg_field_set(unit, PPDB_B_LARGE_EM_COUNTER_LIMIT_CONFIGURATIONr, &reg_val,  LARGE_EM_MNGMNT_REQ_ALLOW_STATIC_EXCEED_LARGE_EM_DB_LIMITf, fld_val);
  soc_reg_field_set(unit, PPDB_B_LARGE_EM_COUNTER_LIMIT_CONFIGURATIONr, &reg_val,  LARGE_EM_MNGMNT_REQ_ALLOW_STATIC_EXCEED_FID_LIMITf, fld_val);

  
  fld_val = limit_info->cpu_may_exceed;
  soc_reg_field_set(unit, PPDB_B_LARGE_EM_COUNTER_LIMIT_CONFIGURATIONr, &reg_val,  LARGE_EM_MNGMNT_REQ_ALLOW_CPU_EXCEED_LARGE_EM_DB_LIMITf, fld_val);
  soc_reg_field_set(unit, PPDB_B_LARGE_EM_COUNTER_LIMIT_CONFIGURATIONr, &reg_val,  LARGE_EM_MNGMNT_REQ_ALLOW_CPU_EXCEED_FID_LIMITf, fld_val);


  
  fld_val = SOC_SAND_BOOL2NUM(limit_info->glbl_limit != SOC_PPC_FRWRD_MACT_NO_GLOBAL_LIMIT);
  soc_reg_field_set(unit, PPDB_B_LARGE_EM_COUNTER_LIMIT_CONFIGURATIONr, &reg_val,  LARGE_EM_FID_DB_CHECK_LARGE_EM_DB_LIMITf, fld_val);

  
  if(limit_info->glbl_limit == SOC_PPC_FRWRD_MACT_NO_GLOBAL_LIMIT) {
    fld_val = SOC_DPP_DEFS_GET(unit, nof_lem_lines) - 1;
  } else {
    fld_val =  limit_info->glbl_limit;
  }
  soc_reg_field_set(unit, PPDB_B_LARGE_EM_COUNTER_LIMIT_CONFIGURATIONr, &reg_val,  LARGE_EM_MNGMNT_LARGE_EM_DB_ENTRIES_LIMITf, fld_val);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_PPDB_B_LARGE_EM_COUNTER_LIMIT_CONFIGURATIONr(unit, reg_val));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_mac_limit_glbl_info_set_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_mact_mac_limit_glbl_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO           *limit_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET_VERIFY);

  res = SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_verify(unit, limit_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_mac_limit_glbl_info_set_verify()", 0, 0);
}


uint32
  arad_pp_frwrd_mact_mac_limit_glbl_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO           *limit_info
  )
{
  uint32
    fld_val,
    reg_val,
    res = SOC_SAND_OK;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(limit_info);

  SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_clear(limit_info);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit,READ_PPDB_B_LARGE_EM_COUNTER_LIMIT_CONFIGURATIONr(unit, &reg_val));

  
  
  fld_val = soc_reg_field_get(unit, PPDB_B_LARGE_EM_COUNTER_LIMIT_CONFIGURATIONr, reg_val,  LARGE_EM_FID_DB_CHECK_FID_LIMITf);
  limit_info->enable = SOC_SAND_NUM2BOOL(fld_val);

  
  fld_val = soc_reg_field_get(unit, PPDB_B_LARGE_EM_COUNTER_LIMIT_CONFIGURATIONr, reg_val,  LARGE_EM_MNGMNT_REQ_ALLOW_STATIC_EXCEED_FID_LIMITf);
  limit_info->static_may_exceed = SOC_SAND_NUM2BOOL(fld_val);

  
  
  fld_val = soc_reg_field_get(unit, PPDB_B_LARGE_EM_COUNTER_LIMIT_CONFIGURATIONr, reg_val,  LARGE_EM_MNGMNT_REQ_ALLOW_CPU_EXCEED_FID_LIMITf);
  limit_info->cpu_may_exceed = SOC_SAND_NUM2BOOL(fld_val);;

  
  fld_val = soc_reg_field_get(unit, PPDB_B_LARGE_EM_COUNTER_LIMIT_CONFIGURATIONr, reg_val,  LARGE_EM_FID_DB_CHECK_LARGE_EM_DB_LIMITf);
  if(fld_val == 0)
  {
    limit_info->glbl_limit = SOC_PPC_FRWRD_MACT_NO_GLOBAL_LIMIT;
  }
  else
  {
    limit_info->glbl_limit = soc_reg_field_get(unit, PPDB_B_LARGE_EM_COUNTER_LIMIT_CONFIGURATIONr, reg_val,  LARGE_EM_MNGMNT_LARGE_EM_DB_ENTRIES_LIMITf);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_mac_limit_glbl_info_get_unsafe()", 0, 0);
}



uint32
  arad_pp_frwrd_mact_learn_profile_limit_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO   *limit_info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_IHP_MACT_FID_COUNTER_PROFILE_DB_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(limit_info);

  
  res = arad_pp_ihp_mact_fid_counter_profile_db_tbl_get_unsafe(
          unit,
          mac_learn_profile_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tbl_data.limit = limit_info->nof_entries;

  switch(limit_info->action_when_exceed)
  {
  case SOC_PPC_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_NONE:
    tbl_data.interrupt_en = FALSE;
    tbl_data.message_en = FALSE;
    break;

  case SOC_PPC_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_INTERRUPT:
    tbl_data.interrupt_en = TRUE;
    tbl_data.message_en = FALSE;
    break;

  case SOC_PPC_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_MSG:
    tbl_data.interrupt_en = FALSE;
    tbl_data.message_en = TRUE;
    break;
  
  case SOC_PPC_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_EVENT:
  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_MACT_ACTION_WHEN_EXCEED_OUT_OF_RANGE_ERR, 20, exit);
  }

  res = arad_pp_ihp_mact_fid_counter_profile_db_tbl_set_unsafe(
          unit,
          mac_learn_profile_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_learn_profile_limit_info_set_unsafe()", mac_learn_profile_ndx, 0);
}

uint32
  arad_pp_frwrd_mact_learn_profile_limit_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO   *limit_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mac_learn_profile_ndx, ARAD_PP_FRWRD_MACT_MAC_LEARN_PROFILE_NDX_MAX, ARAD_PP_FRWRD_MACT_MAC_LEARN_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  res = SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO_verify(unit, limit_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_learn_profile_limit_info_set_verify()", mac_learn_profile_ndx, 0);
}

uint32
  arad_pp_frwrd_mact_learn_profile_limit_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mac_learn_profile_ndx, ARAD_PP_FRWRD_MACT_MAC_LEARN_PROFILE_NDX_MAX, ARAD_PP_FRWRD_MACT_MAC_LEARN_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_learn_profile_limit_info_get_verify()", mac_learn_profile_ndx, 0);
}


uint32
  arad_pp_frwrd_mact_learn_profile_limit_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO   *limit_info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_IHP_MACT_FID_COUNTER_PROFILE_DB_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(limit_info);

  SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO_clear(limit_info);

  
  res = arad_pp_ihp_mact_fid_counter_profile_db_tbl_get_unsafe(
          unit,
          mac_learn_profile_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  limit_info->is_limited = TRUE;
  limit_info->nof_entries = tbl_data.limit;

  if (tbl_data.interrupt_en == TRUE)
  {
    limit_info->action_when_exceed = SOC_PPC_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_INTERRUPT;
  }
  else if (tbl_data.message_en == TRUE)
  {
    limit_info->action_when_exceed = SOC_PPC_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_MSG;
  }
  else
  {
    limit_info->action_when_exceed = SOC_PPC_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_NONE;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_learn_profile_limit_info_get_unsafe()", mac_learn_profile_ndx, 0);
}


uint32
  arad_pp_frwrd_mact_event_handle_profile_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_IN  uint32                                  event_handle_profile
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_IHP_MACT_FID_COUNTER_PROFILE_DB_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET_UNSAFE);

  
  res = arad_pp_ihp_mact_fid_counter_profile_db_tbl_get_unsafe(
          unit,
          mac_learn_profile_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tbl_data.event_forwarding_profile = event_handle_profile;

  res = arad_pp_ihp_mact_fid_counter_profile_db_tbl_set_unsafe(
          unit,
          mac_learn_profile_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_event_handle_profile_set_unsafe()", mac_learn_profile_ndx, 0);
}

uint32
  arad_pp_frwrd_mact_event_handle_profile_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_IN  uint32                                  event_handle_profile
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mac_learn_profile_ndx, ARAD_PP_FRWRD_MACT_MAC_LEARN_PROFILE_NDX_MAX, ARAD_PP_FRWRD_MACT_MAC_LEARN_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(event_handle_profile, ARAD_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_MAX, ARAD_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_event_handle_profile_set_verify()", mac_learn_profile_ndx, event_handle_profile);
}

uint32
  arad_pp_frwrd_mact_event_handle_profile_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mac_learn_profile_ndx, ARAD_PP_FRWRD_MACT_MAC_LEARN_PROFILE_NDX_MAX, ARAD_PP_FRWRD_MACT_MAC_LEARN_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_event_handle_profile_get_verify()", mac_learn_profile_ndx, 0);
}


uint32
  arad_pp_frwrd_mact_event_handle_profile_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_OUT uint32                                  *event_handle_profile
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_IHP_MACT_FID_COUNTER_PROFILE_DB_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(event_handle_profile);

  
  res = arad_pp_ihp_mact_fid_counter_profile_db_tbl_get_unsafe(
          unit,
          mac_learn_profile_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *event_handle_profile = tbl_data.event_forwarding_profile;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_event_handle_profile_get_unsafe()", mac_learn_profile_ndx, 0);
}



uint32
  arad_pp_frwrd_mact_fid_aging_profile_set_unsafe(
    SOC_SAND_IN  int            unit,
    SOC_SAND_IN  uint32         mac_learn_profile_ndx,
    SOC_SAND_IN  uint32         fid_aging_profile
  )
{
  uint32 res = SOC_SAND_OK;
  ARAD_PP_IHP_MACT_FID_COUNTER_PROFILE_DB_TBL_DATA tbl_data;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  res = arad_pp_ihp_mact_fid_counter_profile_db_tbl_get_unsafe(
          unit,
          mac_learn_profile_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);

  tbl_data.fid_aging_profile = fid_aging_profile;

  res = arad_pp_ihp_mact_fid_counter_profile_db_tbl_set_unsafe(
          unit,
          mac_learn_profile_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_fid_aging_profile_set_unsafe()", mac_learn_profile_ndx, 0);
}

uint32
  arad_pp_frwrd_mact_fid_aging_profile_set_verify(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32          mac_learn_profile_ndx,
    SOC_SAND_IN  uint32          fid_aging_profile
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_ERR_IF_ABOVE_MAX(mac_learn_profile_ndx, ARAD_PP_FRWRD_MACT_MAC_LEARN_PROFILE_NDX_MAX, ARAD_PP_FRWRD_MACT_MAC_LEARN_PROFILE_NDX_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(fid_aging_profile, ARAD_PP_FRWRD_MACT_FID_AGING_PROFILE_MAX, ARAD_PP_FRWRD_MACT_FID_AGING_PROFILE_OUT_OF_RANGE_ERR, 21, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_fid_aging_profile_set_verify()", mac_learn_profile_ndx, fid_aging_profile);
}

uint32
  arad_pp_frwrd_mact_fid_aging_profile_get_verify(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32       mac_learn_profile_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_ERR_IF_ABOVE_MAX(mac_learn_profile_ndx, ARAD_PP_FRWRD_MACT_MAC_LEARN_PROFILE_NDX_MAX, ARAD_PP_FRWRD_MACT_MAC_LEARN_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_fid_aging_profile_get_verify()", mac_learn_profile_ndx, 0);
}


uint32
  arad_pp_frwrd_mact_fid_aging_profile_get_unsafe(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint32    mac_learn_profile_ndx,
    SOC_SAND_OUT uint32   *fid_aging_profile
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_IHP_MACT_FID_COUNTER_PROFILE_DB_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(fid_aging_profile);

  
  res = arad_pp_ihp_mact_fid_counter_profile_db_tbl_get_unsafe(
          unit,
          mac_learn_profile_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *fid_aging_profile = tbl_data.fid_aging_profile;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_fid_aging_profile_get_unsafe()", mac_learn_profile_ndx, 0);
}


uint32
  arad_pp_frwrd_mact_event_get_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_EVENT_BUFFER            *event_buf
  )
{
  uint32
    fld_val,
    nof_event_fifos,
    event_fifo_ndx,
    res = SOC_SAND_OK;
  soc_reg_above_64_val_t
    reg_above_64_val;
  soc_reg_t
      event_ready_r,
      mact_event_r;
  soc_field_t
      event_ready_f;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_EVENT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(event_buf);

  SOC_PPC_FRWRD_MACT_EVENT_BUFFER_clear(event_buf);
  
  
  nof_event_fifos = SOC_IS_JERICHO(unit)? 2: 1;

  
  for (event_fifo_ndx = 0; event_fifo_ndx < nof_event_fifos; event_fifo_ndx++) {
      event_ready_r = SOC_IS_JERICHO(unit)? PPDB_B_LARGE_EM_INTERRUPT_REGISTER_TWOr: IHP_MACT_INTERRUPT_REGISTER_TWOr;
      event_ready_f = SOC_IS_JERICHO(unit)? ((event_fifo_ndx == 0)? LARGE_EM_MASTER_EVENT_READYf: LARGE_EM_SLAVE_EVENT_READYf): MACT_EVENT_READYf;
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, event_ready_r, REG_PORT_ANY, 0, event_ready_f, &fld_val));
      if (fld_val == 0x0)
      {
        event_buf->buff_len = 0;
      }
      else
      {
        
        mact_event_r = SOC_IS_JERICHO(unit)? ((event_fifo_ndx == 0)? PPDB_B_LARGE_EM_MASTER_EVENTr: PPDB_B_LARGE_EM_SLAVE_EVENTr): IHP_MACT_EVENTr;
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg_above_64_get(unit, mact_event_r, REG_PORT_ANY, 0, reg_above_64_val));
        event_buf->buff[0] = reg_above_64_val[0];
        event_buf->buff[1] = reg_above_64_val[1];
        event_buf->buff[2] = reg_above_64_val[2];
        event_buf->buff[3] = reg_above_64_val[3];
        event_buf->buff[4] = reg_above_64_val[4];
        event_buf->buff_len = 5;
        break; 
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_event_get_unsafe()", 0, 0);
}


uint32
  arad_pp_frwrd_mact_event_handle_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY               *event_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO              *handle_info
  )
{
  uint32
    line_indx;
  uint32
    res,
    build_val=0,
    entry_indx,
    indx,
    entry_val[2];
  SOC_PPC_FRWRD_MACT_EVENT_TYPE
    event_type_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(event_key);
  SOC_SAND_CHECK_NULL_INPUT(handle_info);

  
  
  line_indx = 0; 
  entry_indx = (event_key->is_lag << 2) | event_key->vsi_event_handle_profile;
  build_val = (handle_info->self_learning) | (handle_info->send_to_learning_fifo << 1) | (handle_info->send_to_shadow_fifo << 2) |
                (handle_info->send_to_cpu_dma_fifo << 3);
 
  for (event_type_ndx = 0; event_type_ndx < SOC_PPC_NOF_FRWRD_MACT_EVENT_TYPES; ++event_type_ndx)
  {
    if (SOC_SAND_GET_BIT(event_key->event_type, event_type_ndx) == 0x1)
    {
      indx = line_indx + (arad_pp_frwrd_mact_event_type_to_event_val(SOC_SAND_BIT(event_type_ndx)));

      res = READ_OLP_DSP_EVENT_ROUTEm(unit,MEM_BLOCK_ANY,indx,entry_val);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

      SHR_BITCOPY_RANGE(entry_val, entry_indx*(SOC_DPP_IMP_DEFS_GET(unit, dsp_event_route_entry_nof_bits)), &build_val, 0, SOC_DPP_IMP_DEFS_GET(unit, dsp_event_route_entry_nof_bits));

      res = WRITE_OLP_DSP_EVENT_ROUTEm(unit,MEM_BLOCK_ANY,indx,entry_val);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_event_handle_info_set_unsafe()", 0, 0);
}


soc_error_t arad_pp_frwrd_mact_event_handle_info_set_dma(SOC_SAND_IN int unit, SOC_SAND_IN int enable_dma)
{
  uint32
    line_indx,
    num_bits_per_profile;
  uint32
    res,
    build_val=0,
    entry_indx,
    indx,
    event_type_id,
    entry_val[2];
  uint32 dma_supported;
  SOC_PPC_FRWRD_MACT_EVENT_TYPE
    event_type_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
 
  res = arad_pp_frwrd_mact_is_dma_supported(unit, &dma_supported);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit); 

  if (!dma_supported) 
  {
      ARAD_PP_DO_NOTHING_AND_EXIT;
  }

  
  
  line_indx = 0; 

  build_val = (enable_dma << 3); 
  
  num_bits_per_profile = SOC_DPP_IMP_DEFS_GET(unit, dsp_event_route_entry_nof_bits);

 
  for (event_type_ndx = 0; event_type_ndx < SOC_PPC_NOF_FRWRD_MACT_EVENT_TYPES; ++event_type_ndx)
  {
    entry_val[0] = 0;
    entry_val[1] = 0;

    event_type_id = SOC_SAND_BIT(event_type_ndx);

    if(event_type_id==SOC_PPC_FRWRD_MACT_EVENT_TYPE_AGED_OUT ||
       event_type_id== SOC_PPC_FRWRD_MACT_EVENT_TYPE_LEARN ||
       event_type_id==SOC_PPC_FRWRD_MACT_EVENT_TYPE_TRANSPLANT)
    {
        for (entry_indx = 0; entry_indx < NUM_OF_EVENT_ROUTE_PROFILES; ++entry_indx)
        {
          SHR_BITCOPY_RANGE(entry_val, entry_indx * num_bits_per_profile, &build_val, 0, num_bits_per_profile);
        }
    }
    indx = line_indx + (arad_pp_frwrd_mact_event_type_to_event_val(event_type_id));

    res = WRITE_OLP_DSP_EVENT_ROUTEm(unit, MEM_BLOCK_ANY, indx, entry_val); 
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);  
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_event_handle_info_set_dma()", 0, 0);
}

soc_error_t arad_pp_frwrd_mact_transplant_static_get(SOC_SAND_IN int unit, SOC_SAND_OUT uint8 *is_enabled)
{
    uint32 res, reg_val, field_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
 
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,READ_IHP_SA_LOOKUP_RESULT_MAPPINGr(unit, 0, &reg_val));
    field_val = (uint8)soc_reg_field_get(unit, IHP_SA_LOOKUP_RESULT_MAPPINGr, reg_val, SA_LOOKUP_RESULT_MAPPING_4f);

    
    *is_enabled = (field_val < 8 ? 0 : 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_transplant_static_get()", 0, 0);
}

soc_error_t arad_pp_frwrd_mact_transplant_static_set(SOC_SAND_IN int unit, SOC_SAND_IN uint8 enabled)
{
    uint32 res, reg_val, field_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);


    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,READ_IHP_SA_LOOKUP_RESULT_MAPPINGr(unit, 0, &reg_val));   
    field_val = soc_reg_field_get(unit, IHP_SA_LOOKUP_RESULT_MAPPINGr, reg_val, SA_LOOKUP_RESULT_MAPPING_4f);
    field_val = (field_val & 0x7 ) | (((uint32)enabled) << 3);     
    ARAD_FLD_TO_REG(IHP_SA_LOOKUP_RESULT_MAPPINGr,
                    SA_LOOKUP_RESULT_MAPPING_4f,
                    field_val, reg_val, 50, exit);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHP_SA_LOOKUP_RESULT_MAPPINGr(unit, SOC_CORE_ALL, reg_val));

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,READ_PPDB_B_LARGE_EM_LOOKUP_LOOKUP_MODEr(unit, &reg_val));    
    field_val = (enabled ? 2 : 0);
    ARAD_FLD_TO_REG(PPDB_B_LARGE_EM_LOOKUP_LOOKUP_MODEr,
                    LARGE_EM_LOOKUP_ALLOWED_EVENTS_STATICf,
                    field_val, reg_val, 50, exit);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,WRITE_PPDB_B_LARGE_EM_LOOKUP_LOOKUP_MODEr(unit, reg_val));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_transplant_static_set()", 0, 0);
}

STATIC
  uint32
    arad_pp_frwrd_mact_event_handle_info_verify(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY               *event_key
    )
{
  uint32
    res;
  SOC_PPC_FRWRD_MACT_LEARNING_MODE
    learning_mode;
  uint8
    is_ingress_learning;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_VERIFY);

  
  res = sw_state_access[unit].dpp.soc.arad.pp.fwd_mact.learning_mode.get(unit, &learning_mode);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  
 res = arad_pp_frwrd_mact_is_ingress_learning_get(
          unit,
          learning_mode,
          &is_ingress_learning
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_event_handle_info_set_verify()", 0, 0);
}

uint32
  arad_pp_frwrd_mact_event_handle_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY               *event_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO              *handle_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_SET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY, event_key, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO, handle_info, 20, exit);

  res = arad_pp_frwrd_mact_event_handle_info_verify(
          unit,
          event_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_event_handle_info_set_verify()", 0, 0);
}

uint32
  arad_pp_frwrd_mact_event_handle_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY       *event_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_GET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY, event_key, 10, exit);

  res = arad_pp_frwrd_mact_event_handle_info_verify(
          unit,
          event_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_event_handle_info_get_verify()", 0, 0);
}


uint32
  arad_pp_frwrd_mact_event_handle_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY               *event_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO              *handle_info
  )
{
  uint32
    line_indx;
  uint32
    res,
    build_val=0,
    entry_val[2],
    entry_indx,
    indx;
  SOC_PPC_FRWRD_MACT_EVENT_TYPE
    event_type_ndx;
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(event_key);
  SOC_SAND_CHECK_NULL_INPUT(handle_info);
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_clear(handle_info);

  
  
  line_indx = 0;
  entry_indx = (event_key->is_lag << 2) | event_key->vsi_event_handle_profile;
 
 
  for (event_type_ndx = 0; event_type_ndx < SOC_PPC_NOF_FRWRD_MACT_EVENT_TYPES; ++event_type_ndx)
  {
    if (SOC_SAND_GET_BIT(event_key->event_type, event_type_ndx) == 0x1)
    {
      indx = line_indx + (arad_pp_frwrd_mact_event_type_to_event_val(SOC_SAND_BIT(event_type_ndx)));

      res = READ_OLP_DSP_EVENT_ROUTEm(unit,MEM_BLOCK_ANY,indx,entry_val);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

      build_val = 0;
      SHR_BITCOPY_RANGE(&build_val,0, entry_val, entry_indx*(SOC_DPP_IMP_DEFS_GET(unit, dsp_event_route_entry_nof_bits)), 
                        SOC_DPP_IMP_DEFS_GET(unit, dsp_event_route_entry_nof_bits));

      break;
    }
  }

  handle_info->self_learning = SOC_SAND_GET_BIT(build_val,0);
  handle_info->send_to_learning_fifo = SOC_SAND_GET_BIT(build_val,1);
  handle_info->send_to_shadow_fifo = SOC_SAND_GET_BIT(build_val,2);
  handle_info->send_to_cpu_dma_fifo = SOC_SAND_GET_BIT(build_val,3);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_event_handle_info_get_unsafe()", 0, 0);
}


STATIC
  uint32
    arad_pp_frwrd_mact_olp_msg_set(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  ARAD_PP_FRWRD_MACT_FIFO       distribution_fifo,
      SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO  *distribution_info
    )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PORTS_OTMH_EXTENSIONS_EN
    otmh_extenstion;
  uint32
    header_size = 0,
    dsp_header_type = 0,
    to_skip,
    header_type_lsb = 0,
    indx,
    header[ARAD_PP_FRWRD_MACT_MESSAGE_HEADER_SIZE],
    otmh_size=0,
    user_header_0_size,
    user_header_1_size,
    user_header_egress_pmf_offset_0,
    user_header_egress_pmf_offset_1, 
    itmh_header_size; 
  soc_reg_above_64_val_t
    reg_above_64_val;
  uint32
    reg_val;
  uint64 
    reg_64_val,
    fld_64_val;
  int
    core = 0,
    num_olp_tm_ports = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_LEARN_MSGS_OLP_MSG_SET);

  SOC_SAND_CHECK_NULL_INPUT(distribution_info);

  
  if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "injection_with_user_header_enable", 0 ) == 1)
  {
	  res = arad_pmf_db_fes_user_header_sizes_get(
                unit,
                &user_header_0_size,
                &user_header_1_size,
                &user_header_egress_pmf_offset_0,
                &user_header_egress_pmf_offset_1
              );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
	  user_header_0_size/=8;
	  user_header_1_size/=8;
	  itmh_header_size = ARAD_PP_FRWRD_MACT_ITMH_HEADER_SIZE + (user_header_0_size + user_header_1_size);
  }
  else
  {
	  itmh_header_size = ARAD_PP_FRWRD_MACT_ITMH_HEADER_SIZE;
  }
  
  SOC_DPP_CORES_ITER(SOC_CORE_ALL, core)
  {
      num_olp_tm_ports |= soc_property_suffix_num_get(unit, core, spn_NUM_OLP_TM_PORTS, "core", 0);
  }

  if(distribution_info->header_type == SOC_PPC_NOF_FRWRD_MACT_MSG_HDR_TYPES ||
		num_olp_tm_ports == 0){
      goto exit; 
  }

    if (distribution_info->olp_receive_header_type != SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_RAW &&
        distribution_info->olp_receive_header_type != SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_ETH_O_RAW) {
        res = arad_ports_otmh_extension_get_unsafe(
            unit,
            SOC_CORE_DEFAULT,
            ARAD_OLP_PORT_ID,
            &otmh_extenstion
         );
     SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

     
     otmh_size = ARAD_PP_FRWRD_MACT_OTMH_HEADER_SIZE;

     if (otmh_extenstion.outlif_ext_en)
     {
       otmh_size += 3;
     }
     if (otmh_extenstion.src_ext_en)
     {
       otmh_size += 2;
     }
  }


  
  SOC_SAND_ERR_IF_ABOVE_MAX(distribution_fifo, ARAD_PP_FRWRD_MACT_NOF_FIFOS-1, ARAD_PP_FRWRD_MACT_FIFO_OUT_OF_RANGE_ERR, 5, exit);

    
    switch (distribution_info->olp_receive_header_type) {
    case SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_RAW:
        to_skip = 0;
        break;

    case SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_ETH_O_RAW:
        to_skip = ARAD_PP_FRWRD_MACT_ETHERNET_HEADER_SIZE -ARAD_PP_FRWRD_MACT_ETHERNET_TYPE_SIZE;
        break;

    case SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_OTMH:
        to_skip = otmh_size;
        break;

    case SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_ETH_O_OTMH:
        to_skip = ARAD_PP_FRWRD_MACT_ETHERNET_HEADER_SIZE - ARAD_PP_FRWRD_MACT_ETHERNET_TYPE_SIZE + otmh_size;
        break;

    default:
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_MACT_HEADER_TYPE_OUT_OF_RANGE_ERR, 10, exit);
    }

    
    switch (distribution_info->header_type) {

    case SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_ITMH:
        header_size = itmh_header_size + ARAD_PP_FRWRD_MACT_ETHERNET_TYPE_SIZE;
        header_type_lsb = itmh_header_size;
        break;
    case SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_ETH_VLAN_O_ITMH:
        header_size=4; 
        to_skip += 4;
        header_type_lsb += 4;
    case SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_ETH_O_ITMH:
        header_type_lsb += itmh_header_size + ARAD_PP_FRWRD_MACT_ETHERNET_HEADER_SIZE - ARAD_PP_FRWRD_MACT_ETHERNET_TYPE_SIZE;
        header_size += ARAD_PP_FRWRD_MACT_ETHERNET_HEADER_SIZE + itmh_header_size;
        break;

  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_MACT_HEADER_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  }

  
  to_skip += ARAD_PP_FRWRD_MACT_ETHERNET_TYPE_SIZE;
  if(distribution_fifo == ARAD_PP_FRWRD_MACT_FIFO_LEARNING) {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  15,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, OLP_RX_CONFIGS_GENERALr, REG_PORT_ANY, 0, DSP_FIRST_CMD_OFFSETf,  to_skip));
  }

 
  soc_sand_os_memcpy(
    header,
    distribution_info->header,
    sizeof(uint32) * ARAD_PP_FRWRD_MACT_MESSAGE_HEADER_SIZE
  );

  header_type_lsb = (ARAD_PP_FRWRD_MACT_DSP_HEADER_SIZE * SOC_SAND_NOF_BITS_IN_UINT32) -
    ((header_type_lsb + ARAD_PP_FRWRD_MACT_ETHERNET_TYPE_SIZE) * SOC_SAND_NOF_BITS_IN_CHAR);

 
  if (distribution_info->header_type != SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_ETH_O_ITMH &&
      distribution_info->header_type != SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_ETH_VLAN_O_ITMH)
  {
    dsp_header_type = ARAD_PP_FRWRD_MACT_MESSAGE_DSP_TYPE;
  }
  else
  {
    res = soc_sand_bitstream_get_any_field(
            distribution_info->header,
            header_type_lsb,
            ARAD_PP_FRWRD_MACT_ETHERNET_TYPE_SIZE * SOC_SAND_NOF_BITS_IN_CHAR,
            &dsp_header_type
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  res = soc_sand_bitstream_set_any_field(
          &dsp_header_type,
          header_type_lsb,
          ARAD_PP_FRWRD_MACT_ETHERNET_TYPE_SIZE * SOC_SAND_NOF_BITS_IN_CHAR,
          header
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  

 
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, READ_OLP_DSP_ENGINE_CONFIGURATIONr(unit,distribution_fifo,&reg_val));
  soc_reg_field_set(unit, OLP_DSP_ENGINE_CONFIGURATIONr, &reg_val, DSP_HEADER_SIZEf, header_size);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, WRITE_OLP_DSP_ENGINE_CONFIGURATIONr(unit,distribution_fifo,reg_val));

  
  if(distribution_fifo == ARAD_PP_FRWRD_MACT_FIFO_LEARNING) 
  {  
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, READ_OLP_RX_CONFIGS_DSP_IDENTIFIERr(unit,&reg_64_val));
    
    to_skip -= ARAD_PP_FRWRD_MACT_ETHERNET_TYPE_SIZE;

    COMPILER_64_SET(fld_64_val,0,to_skip);
    soc_reg64_field_set(unit, OLP_RX_CONFIGS_DSP_IDENTIFIERr, &reg_64_val, DSP_IDENTIFIER_BYTES_TO_SKIPf, fld_64_val);
    
    COMPILER_64_SET(fld_64_val,0,1);
    soc_reg64_field_set(unit, OLP_RX_CONFIGS_DSP_IDENTIFIERr, &reg_64_val, DSP_IDENTIFIER_CHECKf, fld_64_val);
    
    COMPILER_64_SET(fld_64_val,0,0xFFFF);
    soc_reg64_field_set(unit, OLP_RX_CONFIGS_DSP_IDENTIFIERr, &reg_64_val, DSP_IDENTIFIER_MASKf, fld_64_val);
    
    COMPILER_64_SET(fld_64_val,0,dsp_header_type);
    soc_reg64_field_set(unit, OLP_RX_CONFIGS_DSP_IDENTIFIERr, &reg_64_val, DSP_IDENTIFIER_IDf, fld_64_val);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, WRITE_OLP_RX_CONFIGS_DSP_IDENTIFIERr(unit,reg_64_val));
  }

 
  for (indx = 0; indx < ARAD_PP_FRWRD_MACT_DSP_HEADER_SIZE; ++indx)
  {
    reg_above_64_val[indx] = header[indx];
  }
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 90, exit, WRITE_OLP_DSP_HEADERr(unit,distribution_fifo,reg_above_64_val));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_olp_msg_set()", 0, 0);
}

STATIC
  uint32
    arad_pp_frwrd_mact_olp_msg_get(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  ARAD_PP_FRWRD_MACT_FIFO       distribution_fifo,
      SOC_SAND_OUT SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO  *distribution_info
    )
{
  uint32
    reg_val,
    res = SOC_SAND_OK,
    indx,
    header_size,
    bytes_to_skip,
    user_header_0_size,
    user_header_1_size,
    user_header_egress_pmf_offset_0,
    user_header_egress_pmf_offset_1, 
    itmh_header_size; 
    uint64 reg_val64;
    soc_reg_above_64_val_t
    reg_above_64_val;
  uint64 bytes_to_skip64;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_LEARN_MSGS_OLP_MSG_GET);

  SOC_SAND_CHECK_NULL_INPUT(distribution_info);
  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO_clear(distribution_info);

    
    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "injection_with_user_header_enable", 0 ) == 1) {

        res = arad_pmf_db_fes_user_header_sizes_get(
            unit,
            &user_header_0_size,
            &user_header_1_size,
            &user_header_egress_pmf_offset_0,
            &user_header_egress_pmf_offset_1
            );

        user_header_0_size /= 8;
        user_header_1_size /= 8;
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        itmh_header_size = ARAD_PP_FRWRD_MACT_ITMH_HEADER_SIZE + (user_header_0_size + user_header_1_size);
    } else {
        itmh_header_size = ARAD_PP_FRWRD_MACT_ITMH_HEADER_SIZE;
    }

    
    SOC_SAND_ERR_IF_ABOVE_MAX(distribution_fifo, ARAD_PP_FRWRD_MACT_NOF_FIFOS-1, ARAD_PP_FRWRD_MACT_FIFO_OUT_OF_RANGE_ERR, 5, exit);

    

    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, READ_OLP_DSP_ENGINE_CONFIGURATIONr(unit,distribution_fifo,&reg_val));
    header_size = soc_reg_field_get(unit, OLP_DSP_ENGINE_CONFIGURATIONr, reg_val, DSP_HEADER_SIZEf);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, READ_OLP_RX_CONFIGS_DSP_IDENTIFIERr(unit,&reg_val64));
    bytes_to_skip64 = soc_reg64_field_get(unit, OLP_RX_CONFIGS_DSP_IDENTIFIERr, reg_val64, DSP_IDENTIFIER_BYTES_TO_SKIPf);
    bytes_to_skip = COMPILER_64_LO(bytes_to_skip64);

    
    if (header_size == ARAD_PP_FRWRD_MACT_ETHERNET_TYPE_SIZE)
    {
        distribution_info->header_type = SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_RAW;
    }
    else if (header_size == itmh_header_size + ARAD_PP_FRWRD_MACT_ETHERNET_TYPE_SIZE)
    {
        distribution_info->header_type = SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_ITMH;
    }
    else if (header_size == ARAD_PP_FRWRD_MACT_ETHERNET_HEADER_SIZE + itmh_header_size)
    {
        distribution_info->header_type = SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_ETH_O_ITMH;
    }
    else if (header_size == (ARAD_PP_FRWRD_MACT_ETHERNET_HEADER_SIZE + itmh_header_size + 4))
    {
        distribution_info->header_type = SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_ETH_VLAN_O_ITMH;
    }

    
    switch (bytes_to_skip)
    {
    case 0:
        distribution_info->olp_receive_header_type = SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_RAW;
        break;
    case ARAD_PP_FRWRD_MACT_ETHERNET_HEADER_SIZE - ARAD_PP_FRWRD_MACT_ETHERNET_TYPE_SIZE:
        distribution_info->olp_receive_header_type = SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_ETH_O_RAW;
        break;
    case ARAD_PP_FRWRD_MACT_ETHERNET_HEADER_SIZE - ARAD_PP_FRWRD_MACT_ETHERNET_TYPE_SIZE + ARAD_PP_FRWRD_MACT_OTMH_HEADER_SIZE:
    case ARAD_PP_FRWRD_MACT_ETHERNET_HEADER_SIZE - ARAD_PP_FRWRD_MACT_ETHERNET_TYPE_SIZE + ARAD_PP_FRWRD_MACT_OTMH_HEADER_SIZE + 2:
    case ARAD_PP_FRWRD_MACT_ETHERNET_HEADER_SIZE - ARAD_PP_FRWRD_MACT_ETHERNET_TYPE_SIZE + ARAD_PP_FRWRD_MACT_OTMH_HEADER_SIZE + 3:
        distribution_info->olp_receive_header_type = SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_ETH_O_OTMH;
        break;
    case ARAD_PP_FRWRD_MACT_OTMH_HEADER_SIZE:
    case ARAD_PP_FRWRD_MACT_OTMH_HEADER_SIZE + 2:
    case ARAD_PP_FRWRD_MACT_OTMH_HEADER_SIZE + 3:
        distribution_info->olp_receive_header_type = SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_OTMH;
        break;
    }

    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 90, exit, READ_OLP_DSP_HEADERr(unit, distribution_fifo, reg_above_64_val));
    for (indx = 0; indx < ARAD_PP_FRWRD_MACT_DSP_HEADER_SIZE; ++indx) {
        distribution_info->header[indx] = reg_above_64_val[indx];
    }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_olp_msg_get()", 0, 0);
}


uint32
  arad_pp_frwrd_mact_learn_msgs_distribution_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  )
{
  uint32
    res = SOC_SAND_OK;
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(distribution_info);

  res = arad_pp_frwrd_mact_olp_msg_set(
          unit,
          ARAD_PP_FRWRD_MACT_FIFO_LEARNING,
          distribution_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_learn_msgs_distribution_info_set_unsafe()", 0, 0);
}

uint32
arad_pp_frwrd_mact_learn_msgs_distribution_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO, distribution_info, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_learn_msgs_distribution_info_set_verify()", 0, 0);
}


uint32
  arad_pp_frwrd_mact_learn_msgs_distribution_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(distribution_info);
  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO_clear(distribution_info);

  res = arad_pp_frwrd_mact_olp_msg_get(
          unit,
          ARAD_PP_FRWRD_MACT_FIFO_LEARNING,
          distribution_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_learn_msgs_distribution_info_get_unsafe()", 0, 0);
}


uint32
  arad_pp_frwrd_mact_shadow_msgs_distribution_info_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO                     *distribution_info
  )
{
  uint32
    res = SOC_SAND_OK;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(distribution_info);

  

  res = arad_pp_frwrd_mact_olp_msg_set(
          unit,
          ARAD_PP_FRWRD_MACT_FIFO_SHADOW,
          distribution_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_shadow_msgs_distribution_info_set_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_mact_shadow_msgs_distribution_info_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO                     *distribution_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO, distribution_info, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_shadow_msgs_distribution_info_set_verify()", 0, 0);
}


uint32
  arad_pp_frwrd_mact_shadow_msgs_distribution_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO                     *distribution_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(distribution_info);
  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO_clear(distribution_info);

  res = arad_pp_frwrd_mact_olp_msg_get(
          unit,
          ARAD_PP_FRWRD_MACT_FIFO_SHADOW,
          distribution_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_shadow_msgs_distribution_info_get_unsafe()", 0, 0);
}


uint32
  arad_pp_frwrd_mact_mac_limit_exceeded_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO  *exceed_info
  )
{
  uint32
    res,
    fld_val;
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(exceed_info);

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_MNGMNT_REQ_EXCEED_LIMIT_FIDr, REG_PORT_ANY, 0,  LARGE_EM_MNGMNT_REQ_EXCEED_LIMIT_FIDf, &fld_val));
  exceed_info->fid_fail = fld_val;

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_MNGMNT_REQ_EXCEED_LIMIT_STATIC_ALLOWED_FIDr, REG_PORT_ANY, 0,  LARGE_EM_MNGMNT_REQ_EXCEED_LIMIT_STATIC_ALLOWED_FIDf, &fld_val));
  exceed_info->fid_allowed = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_mac_limit_exceeded_info_get_unsafe()", 0, 0);
}


int soc_interrupt_clear_fid_limit_reached_event(int unit, int block_instance, int interrupt_id)
{

  uint32
    res = SOC_E_NONE,
    fld_val;


    res = soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_MNGMNT_REQ_EXCEED_LIMIT_FIDr, REG_PORT_ANY, 0, LARGE_EM_MNGMNT_REQ_EXCEED_LIMIT_FIDf, &fld_val);
    if (res != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(res)));
    }

    return res;
}


uint32
  arad_pp_frwrd_mact_mac_limit_range_map_info_get_unsafe(
    SOC_SAND_IN  int                                         unit,
    SOC_SAND_IN  int8                                           range_num,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_MAC_LIMIT_RANGE_MAP_INFO    *map_info
  )
{
  soc_reg_above_64_val_t
    reg_data, tmp_lif_ptr_range_map_values, field_val;
  uint32 res;
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(map_info);

  
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_RANGE_MAP_INFO_clear(map_info);

  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit,
      soc_reg_above_64_get(unit, PPDB_B_LARGE_EM_COUNTER_DB_LIF_PTR_RANGE_MAPr, REG_PORT_ANY, 0, reg_data));
  SOC_REG_ABOVE_64_CLEAR(tmp_lif_ptr_range_map_values);
  SHR_BITCOPY_RANGE(tmp_lif_ptr_range_map_values, 0, reg_data, range_num * RANGE_MAP_SIZE, RANGE_MAP_SIZE);

  
  soc_reg_above_64_field_get(unit, PPDB_B_LARGE_EM_COUNTER_DB_LIF_PTR_RANGE_MAPr, tmp_lif_ptr_range_map_values,  LARGE_EM_LIF_ZERO_MSBSf, field_val);
  map_info->zero_msbs = field_val[0];

  soc_reg_above_64_field_get(unit, PPDB_B_LARGE_EM_COUNTER_DB_LIF_PTR_RANGE_MAPr, tmp_lif_ptr_range_map_values,  LARGE_EM_LIF_SHIFT_RIGHTf, field_val);
  map_info->shift_right_bits = field_val[0];

  soc_reg_above_64_field_get(unit, PPDB_B_LARGE_EM_COUNTER_DB_LIF_PTR_RANGE_MAPr, tmp_lif_ptr_range_map_values,  LARGE_EM_LIF_BASEf, field_val);
  map_info->base_add_val = field_val[0];

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_mac_limit_range_map_info_get_unsafe()", 0, 0);
}



uint32
  arad_pp_frwrd_mact_mac_limit_range_map_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int8                                   range_num
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_ERR_IF_ABOVE_MAX(range_num, SOC_PPC_MAX_NOF_MACT_LIMIT_LIF_RANGES, ARAD_PP_GENERAL_VAL_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_MSG_STR("error in arad_pp_frwrd_mact_mac_limit_range_map_info_get_verify() for range num %d"), range_num));
}



uint32
  arad_pp_frwrd_mact_mac_limit_mapping_info_get_unsafe(
    SOC_SAND_IN  int                                         unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_MAC_LIMIT_MAPPING_INFO      *map_info
  )
{
  uint32 field_val, res;
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_MAPPING_INFO_clear(map_info);

  
  soc_sand_os_memset(&field_val, 0x0, sizeof(field_val));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_COUNTER_DB_CNTR_PTR_CONFIGURATIONr, REG_PORT_ANY, 0, LARGE_EM_INVALID_LIF_CNTR_PTRf, &field_val));
  soc_sand_os_memcpy(&(map_info->invalid_ptr), &field_val, sizeof(map_info->invalid_ptr));

  
  soc_sand_os_memset(&field_val, 0x0, sizeof(field_val));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  110,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_COUNTER_DB_CNTR_PTR_CONFIGURATIONr, REG_PORT_ANY, 0, LARGE_EM_LIF_RANGE_0f, &field_val));
  soc_sand_os_memcpy(&(map_info->range_end[0]), &field_val, sizeof(map_info->range_end[0]));

  soc_sand_os_memset(&field_val, 0x0, sizeof(field_val));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  120,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_COUNTER_DB_CNTR_PTR_CONFIGURATIONr, REG_PORT_ANY, 0, LARGE_EM_LIF_RANGE_1f, &field_val));
  soc_sand_os_memcpy(&(map_info->range_end[1]), &field_val, sizeof(map_info->range_end[1]));

  soc_sand_os_memset(&field_val, 0x0, sizeof(field_val));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  130,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_read(unit, PPDB_B_LARGE_EM_COUNTER_DB_CNTR_PTR_CONFIGURATIONr, REG_PORT_ANY, 0, LARGE_EM_LIF_RANGE_2f, &field_val));
  soc_sand_os_memcpy(&(map_info->range_end[2]), &field_val, sizeof(map_info->range_end[2]));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_mac_limit_mapping_info_get_unsafe()", 0, 0);
}



uint32
  arad_pp_frwrd_mact_limit_mapped_val_to_table_index_get_unsafe(
      SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN  uint32                                 mapped_val,
      SOC_SAND_OUT uint32                                 *limit_tbl_idx,
      SOC_SAND_OUT uint32                                 *is_reserved
    )
{
  uint32
    res = SOC_SAND_OK, zero_msbs_mask;
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_MAPPING_INFO
    map_info;
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_RANGE_MAP_INFO
    map_range_info;
  int8
    selected_range;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  res = soc_ppd_frwrd_mact_mac_limit_mapping_info_get(
          unit,
          &map_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  if (mapped_val < map_info.range_end[0]) {
    selected_range = 0;
  } else if (mapped_val < map_info.range_end[1]) {
    selected_range = 1;
  } else {
    selected_range = (mapped_val < map_info.range_end[2]) ? 2 : 3;
  }

  
  res = soc_ppd_frwrd_mact_mac_limit_range_map_info_get(
          unit,
          selected_range,
          &map_range_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  *limit_tbl_idx = mapped_val;

  zero_msbs_mask = LIF_MASK(unit) >> map_range_info.zero_msbs;
  *limit_tbl_idx &= zero_msbs_mask;
  *limit_tbl_idx >>= map_range_info.shift_right_bits;

  *limit_tbl_idx += map_range_info.base_add_val;

  
  *is_reserved = (*limit_tbl_idx == map_info.invalid_ptr) ? TRUE : FALSE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_limit_mapped_val_to_table_index_get_unsafe()", 0, 0);
}


uint32
  arad_pp_frwrd_mact_port_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_PORT_INFO                *port_info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_IHB_PINFO_FLP_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_PORT_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  
  res = arad_pp_ihb_pinfo_flp_tbl_get_unsafe(
          unit,
          core_id,
          local_port_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tbl_data.action_profile_sa_not_found_index = port_info->sa_unknown_action_profile;
  tbl_data.action_profile_da_not_found_index = port_info->da_unknown_action_profile;
  tbl_data.action_profile_sa_drop_index = port_info->sa_drop_action_profile;

  res = arad_pp_ihb_pinfo_flp_tbl_set_unsafe(
          unit,
          core_id,
          local_port_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_port_info_set_unsafe()", local_port_ndx, 0);
}

uint32
  arad_pp_frwrd_mact_port_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_PORT_INFO                *port_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_PORT_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_MACT_PORT_INFO, port_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_port_info_set_verify()", local_port_ndx, 0);
}

uint32
  arad_pp_frwrd_mact_port_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_PORT_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(local_port_ndx, ARAD_PP_PORT_MAX, SOC_PPC_PORT_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_port_info_get_verify()", local_port_ndx, 0);
}


uint32
  arad_pp_frwrd_mact_port_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_PORT_INFO                *port_info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_IHB_PINFO_FLP_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_PORT_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  SOC_PPC_FRWRD_MACT_PORT_INFO_clear(port_info);

  
  res = arad_pp_ihb_pinfo_flp_tbl_get_unsafe(
          unit,
          core_id,
          local_port_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  port_info->sa_unknown_action_profile = tbl_data.action_profile_sa_not_found_index;
  port_info->da_unknown_action_profile = tbl_data.action_profile_da_not_found_index;
  port_info->sa_drop_action_profile = tbl_data.action_profile_sa_drop_index;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_port_info_get_unsafe()", local_port_ndx, 0);
}


uint32
  arad_pp_frwrd_mact_trap_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAP_TYPE              trap_type_ndx,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_PPC_ACTION_PROFILE                    *action_profile
  )
{
  uint32 res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_TRAP_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(action_profile);

  
  switch(trap_type_ndx)
  {
  case SOC_PPC_FRWRD_MACT_TRAP_TYPE_SA_DROP:
    switch(port_profile_ndx)
    {
    case 0:
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_ACTION_PROFILE_SA_DROP_MAPr, REG_PORT_ANY, 0, ACTION_PROFILE_SA_DROP_0_FWDf,  action_profile->frwrd_action_strength));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_ACTION_PROFILE_SA_DROP_MAPr, REG_PORT_ANY, 0, ACTION_PROFILE_SA_DROP_0_SNPf,  action_profile->snoop_action_strength));
    break;
    case 1:
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  21,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_ACTION_PROFILE_SA_DROP_MAPr, REG_PORT_ANY, 0, ACTION_PROFILE_SA_DROP_1_FWDf,  action_profile->frwrd_action_strength));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  31,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_ACTION_PROFILE_SA_DROP_MAPr, REG_PORT_ANY, 0, ACTION_PROFILE_SA_DROP_1_SNPf,  action_profile->snoop_action_strength));
    break;
    case 2:
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  22,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_ACTION_PROFILE_SA_DROP_MAPr, REG_PORT_ANY, 0, ACTION_PROFILE_SA_DROP_2_FWDf,  action_profile->frwrd_action_strength));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  32,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_ACTION_PROFILE_SA_DROP_MAPr, REG_PORT_ANY, 0, ACTION_PROFILE_SA_DROP_2_SNPf,  action_profile->snoop_action_strength));
    break;
    case 3:
    default:
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  23,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_ACTION_PROFILE_SA_DROP_MAPr, REG_PORT_ANY, 0, ACTION_PROFILE_SA_DROP_3_FWDf,  action_profile->frwrd_action_strength));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  33,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_ACTION_PROFILE_SA_DROP_MAPr, REG_PORT_ANY, 0, ACTION_PROFILE_SA_DROP_3_SNPf,  action_profile->snoop_action_strength));
    break;
    }
    break;

  case SOC_PPC_FRWRD_MACT_TRAP_TYPE_SA_UNKNOWN:
    switch(port_profile_ndx)
    {
    case 0:
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_ACTION_PROFILE_SA_NOT_FOUND_MAPr, REG_PORT_ANY, 0, ACTION_PROFILE_SA_NOT_FOUND_0_FWDf,  action_profile->frwrd_action_strength));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_ACTION_PROFILE_SA_NOT_FOUND_MAPr, REG_PORT_ANY, 0, ACTION_PROFILE_SA_NOT_FOUND_0_SNPf,  action_profile->snoop_action_strength));
    break;
    case 1:
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  21,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_ACTION_PROFILE_SA_NOT_FOUND_MAPr, REG_PORT_ANY, 0, ACTION_PROFILE_SA_NOT_FOUND_1_FWDf,  action_profile->frwrd_action_strength));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  31,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_ACTION_PROFILE_SA_NOT_FOUND_MAPr, REG_PORT_ANY, 0, ACTION_PROFILE_SA_NOT_FOUND_1_SNPf,  action_profile->snoop_action_strength));
    break;
    case 2:
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  22,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_ACTION_PROFILE_SA_NOT_FOUND_MAPr, REG_PORT_ANY, 0, ACTION_PROFILE_SA_NOT_FOUND_2_FWDf,  action_profile->frwrd_action_strength));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  32,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_ACTION_PROFILE_SA_NOT_FOUND_MAPr, REG_PORT_ANY, 0, ACTION_PROFILE_SA_NOT_FOUND_2_SNPf,  action_profile->snoop_action_strength));
    break;
    case 3:
    default:
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  23,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_ACTION_PROFILE_SA_NOT_FOUND_MAPr, REG_PORT_ANY, 0, ACTION_PROFILE_SA_NOT_FOUND_3_FWDf,  action_profile->frwrd_action_strength));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  33,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_ACTION_PROFILE_SA_NOT_FOUND_MAPr, REG_PORT_ANY, 0, ACTION_PROFILE_SA_NOT_FOUND_3_SNPf,  action_profile->snoop_action_strength));
    break;
    }
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PPC_FRWRD_MACT_TRAP_TYPE_NDX_OUT_OF_RANGE_ERR, 80, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_trap_info_set_unsafe()", 0, port_profile_ndx);
}

uint32
  arad_pp_frwrd_mact_trap_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAP_TYPE                trap_type_ndx,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_PPC_ACTION_PROFILE                      *action_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_TRAP_INFO_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(trap_type_ndx, SOC_PPC_FRWRD_MACT_TRAP_TYPE_NDX_MAX, SOC_PPC_FRWRD_MACT_TRAP_TYPE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(port_profile_ndx, ARAD_PP_FRWRD_MACT_PORT_PROFILE_NDX_MAX, ARAD_PP_FRWRD_MACT_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR, 20, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_ACTION_PROFILE, action_profile, 30, exit);

  if (trap_type_ndx == SOC_PPC_FRWRD_MACT_TRAP_TYPE_SA_UNKNOWN &&
    (action_profile->trap_code != (SOC_PPC_TRAP_CODE)(SOC_PPC_TRAP_CODE_SA_NOT_FOUND_0 + port_profile_ndx)))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_MACT_ACTION_TRAP_CODE_LSB_INVALID_ERR, 50, exit);
  }

  if (trap_type_ndx == SOC_PPC_FRWRD_MACT_TRAP_TYPE_SA_DROP &&
    (action_profile->trap_code != (SOC_PPC_TRAP_CODE)(SOC_PPC_TRAP_CODE_SA_DROP_0 + port_profile_ndx)))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_MACT_ACTION_TRAP_CODE_LSB_INVALID_ERR, 60, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_trap_info_set_verify()", 0, port_profile_ndx);
}



uint32
  arad_pp_frwrd_mact_trap_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAP_TYPE                trap_type_ndx,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_OUT SOC_PPC_ACTION_PROFILE                      *action_profile
  )
{
  uint32 res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_TRAP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(action_profile);

  SOC_PPC_ACTION_PROFILE_clear(action_profile);

  

  
  switch(trap_type_ndx)
  {
  case SOC_PPC_FRWRD_MACT_TRAP_TYPE_SA_DROP:
    switch(port_profile_ndx)
    {
    case 0:
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_ACTION_PROFILE_SA_DROP_MAPr, REG_PORT_ANY, 0, ACTION_PROFILE_SA_DROP_0_FWDf, &action_profile->frwrd_action_strength));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_ACTION_PROFILE_SA_DROP_MAPr, REG_PORT_ANY, 0, ACTION_PROFILE_SA_DROP_0_SNPf, &action_profile->snoop_action_strength));
    break;
    case 1:
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  21,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_ACTION_PROFILE_SA_DROP_MAPr, REG_PORT_ANY, 0, ACTION_PROFILE_SA_DROP_1_FWDf, &action_profile->frwrd_action_strength));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  31,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_ACTION_PROFILE_SA_DROP_MAPr, REG_PORT_ANY, 0, ACTION_PROFILE_SA_DROP_1_SNPf, &action_profile->snoop_action_strength));
    break;
    case 2:
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  22,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_ACTION_PROFILE_SA_DROP_MAPr, REG_PORT_ANY, 0, ACTION_PROFILE_SA_DROP_2_FWDf, &action_profile->frwrd_action_strength));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  32,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_ACTION_PROFILE_SA_DROP_MAPr, REG_PORT_ANY, 0, ACTION_PROFILE_SA_DROP_2_SNPf, &action_profile->snoop_action_strength));
    break;
    case 3:
    default:
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  23,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_ACTION_PROFILE_SA_DROP_MAPr, REG_PORT_ANY, 0, ACTION_PROFILE_SA_DROP_3_FWDf, &action_profile->frwrd_action_strength));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  33,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_ACTION_PROFILE_SA_DROP_MAPr, REG_PORT_ANY, 0, ACTION_PROFILE_SA_DROP_3_SNPf, &action_profile->snoop_action_strength));
    break;
    }
    break;

  case SOC_PPC_FRWRD_MACT_TRAP_TYPE_SA_UNKNOWN:
    switch(port_profile_ndx)
    {
    case 0:
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_ACTION_PROFILE_SA_NOT_FOUND_MAPr, REG_PORT_ANY, 0, ACTION_PROFILE_SA_NOT_FOUND_0_FWDf, &action_profile->frwrd_action_strength));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_ACTION_PROFILE_SA_NOT_FOUND_MAPr, REG_PORT_ANY, 0, ACTION_PROFILE_SA_NOT_FOUND_0_SNPf, &action_profile->snoop_action_strength));
    break;
    case 1:
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  21,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_ACTION_PROFILE_SA_NOT_FOUND_MAPr, REG_PORT_ANY, 0, ACTION_PROFILE_SA_NOT_FOUND_1_FWDf, &action_profile->frwrd_action_strength));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  31,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_ACTION_PROFILE_SA_NOT_FOUND_MAPr, REG_PORT_ANY, 0, ACTION_PROFILE_SA_NOT_FOUND_1_SNPf, &action_profile->snoop_action_strength));
    break;
    case 2:
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  22,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_ACTION_PROFILE_SA_NOT_FOUND_MAPr, REG_PORT_ANY, 0, ACTION_PROFILE_SA_NOT_FOUND_2_FWDf, &action_profile->frwrd_action_strength));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  32,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_ACTION_PROFILE_SA_NOT_FOUND_MAPr, REG_PORT_ANY, 0, ACTION_PROFILE_SA_NOT_FOUND_2_SNPf, &action_profile->snoop_action_strength));
    break;
    case 3:
    default:
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  23,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_ACTION_PROFILE_SA_NOT_FOUND_MAPr, REG_PORT_ANY, 0, ACTION_PROFILE_SA_NOT_FOUND_3_FWDf, &action_profile->frwrd_action_strength));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  33,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_ACTION_PROFILE_SA_NOT_FOUND_MAPr, REG_PORT_ANY, 0, ACTION_PROFILE_SA_NOT_FOUND_3_SNPf, &action_profile->snoop_action_strength));
    break;
    }
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PPC_FRWRD_MACT_TRAP_TYPE_NDX_OUT_OF_RANGE_ERR, 80, exit);
  }

  if (trap_type_ndx == SOC_PPC_FRWRD_MACT_TRAP_TYPE_SA_UNKNOWN)
  {
    action_profile->trap_code = SOC_PPC_TRAP_CODE_SA_NOT_FOUND_0 + port_profile_ndx;
  }

  if (trap_type_ndx == SOC_PPC_FRWRD_MACT_TRAP_TYPE_SA_DROP)
  {
    action_profile->trap_code = SOC_PPC_TRAP_CODE_SA_DROP_0 + port_profile_ndx;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_trap_info_get_unsafe()", 0, port_profile_ndx);
}


uint32
  arad_pp_frwrd_mact_ip_compatible_mc_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO    *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  
  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_ip_compatible_mc_info_set_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_mact_ip_compatible_mc_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO    *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO, info, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_ip_compatible_mc_info_set_verify()", 0, 0);
}


uint32
  arad_pp_frwrd_mact_ip_compatible_mc_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO    *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_clear(info);

  

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_ip_compatible_mc_info_get_unsafe()", 0, 0);
}



uint32
  arad_pp_frwrd_mact_event_parse_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_BUFFER            *event_buf,
    SOC_SAND_OUT SOC_PPC_FRWRD_MACT_EVENT_INFO              *mact_event
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_LEM_ACCESS_BUFFER
    buffer;
  ARAD_PP_LEM_ACCESS_OUTPUT
    output;
  ARAD_PP_LEM_ACCESS_ACK_STATUS ack_status;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_EVENT_PARSE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(event_buf);
  SOC_SAND_CHECK_NULL_INPUT(mact_event);

  ARAD_PP_LEM_ACCESS_BUFFER_clear(&buffer);
  ARAD_PP_LEM_ACCESS_OUTPUT_clear(&output);

  ARAD_PP_LEM_ACCESS_ACK_STATUS_clear(&ack_status);
  

  res = arad_pp_lem_access_parse_only(
          unit,
          ARAD_PP_LEM_ACCESS_LEARN_EVENT,
          event_buf->buff,
          event_buf->buff_len,
          &output,
          &ack_status
        );

  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  
  switch (output.request.command)
  {
  case ARAD_PP_LEM_ACCESS_CMD_DELETE:
    mact_event->type = SOC_PPC_FRWRD_MACT_EVENT_TYPE_AGED_OUT;
    break;

  case ARAD_PP_LEM_ACCESS_CMD_LEARN:
    mact_event->type = SOC_PPC_FRWRD_MACT_EVENT_TYPE_LEARN;
    break;

  case ARAD_PP_LEM_ACCESS_CMD_TRANSPLANT:
    mact_event->type = SOC_PPC_FRWRD_MACT_EVENT_TYPE_TRANSPLANT;
    break;

  case ARAD_PP_LEM_ACCESS_CMD_REFRESH:
    mact_event->type = SOC_PPC_FRWRD_MACT_EVENT_TYPE_REFRESH;
    break;

  case ARAD_PP_LEM_ACCESS_CMD_ACK:
    mact_event->type = SOC_PPC_FRWRD_MACT_EVENT_TYPE_ACK;
    break;

  case ARAD_PP_LEM_ACCESS_CMD_ERROR:
    mact_event->type = SOC_PPC_FRWRD_MACT_EVENT_TYPE_REQ_FAIL;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_LEM_ACCESS_CMD_OUT_OF_RANGE_ERR, 15, exit);
  }

  
  res = arad_pp_frwrd_mact_key_parse(
          unit,
          &output.request.key,
          &mact_event->key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  
  res = arad_pp_frwrd_mact_payload_convert(
          unit,
          &output.payload,
          &mact_event->value
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  
  mact_event->lag.is_lag = output.is_part_of_lag;
  mact_event->lag.id = output.stamp;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_event_parse_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_mact_event_parse_verify(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_BUFFER            *event_buf
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_EVENT_PARSE_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_MACT_EVENT_BUFFER, event_buf, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_event_parse_verify()", 0, 0);
}
void
  ARAD_PP_FRWRD_MACT_AGING_MODIFICATION_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_AGING_MODIFICATION  *info
  )
{
  uint8
    is_owned;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_PP_FRWRD_MACT_AGING_MODIFICATION));
  for (is_owned = 0; is_owned < ARAD_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED; ++is_owned)
  {
    info->age_delete[is_owned] = 8;
    info->age_aged_out[is_owned] = 8;
    info->age_refresh[is_owned] = 8;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

STATIC void
  ARAD_PP_FRWRD_MACT_AGING_EVENT_clear(
      SOC_SAND_OUT ARAD_PP_FRWRD_MACT_AGING_EVENT  *info
    )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_PP_FRWRD_MACT_AGING_EVENT));
  info->deleted = 8;
  info->aged_out = 8;
  info->refreshed = 8;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_FRWRD_MACT_AGING_EVENT_TABLE_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_MACT_AGING_EVENT_TABLE  *info
  )
{
  uint8
    is_owned;
  uint32
    age_ndx;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_PP_FRWRD_MACT_AGING_EVENT_TABLE));
  for (age_ndx = 0; age_ndx < ARAD_PP_FRWRD_MACT_ENTRY_NOF_AGES; ++age_ndx)
  {
    for (is_owned = 0; is_owned < ARAD_PP_FRWRD_MACT_ENTRY_NOF_IS_OWNED; ++is_owned)
    {
      ARAD_PP_FRWRD_MACT_AGING_EVENT_clear(&(info->age_action[age_ndx][is_owned]));
    }
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32
arad_pp_frwrd_mact_learn_msg_conf_get_unsafe(
    SOC_SAND_IN   int                                unit,
    SOC_SAND_IN   SOC_PPC_FRWRD_MACT_LEARN_MSG          *learn_msg,
    SOC_SAND_OUT  SOC_PPC_FRWRD_MACT_LEARN_MSG_CONF     *learn_msg_conf
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO
    distr_info;
  SOC_PPC_TRAP_PACKET_INFO
    pkt_info;
  soc_pkt_t   dnx_pkt;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  res = soc_ppd_trap_packet_parse(unit, learn_msg->msg_buffer,learn_msg->msg_len,&pkt_info, &dnx_pkt);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  learn_msg_conf->header_size = pkt_info.ntwrk_header_ptr;

  
  res = arad_pp_frwrd_mact_olp_msg_get(
          unit,
          ARAD_PP_FRWRD_MACT_FIFO_LEARNING,
          &distr_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  if (distr_info.header_type == SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_ETH_O_ITMH)
  {
    learn_msg_conf->header_size += ARAD_PP_FRWRD_MACT_ETHERNET_HEADER_SIZE - ARAD_PP_FRWRD_MACT_ETHERNET_TYPE_SIZE;
  }
    
  
  learn_msg_conf->recv_mode = SOC_TMC_PKT_PACKET_RECV_MODE_LSB_TO_MSB;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_learn_msg_conf_get_unsafe()", 0, 0);
}



uint32
  arad_pp_frwrd_mact_learn_msg_parse_unsafe(
      SOC_SAND_IN  int                                         unit,
      SOC_SAND_IN   SOC_PPC_FRWRD_MACT_LEARN_MSG                  *learn_msg,
      SOC_SAND_IN   SOC_PPC_FRWRD_MACT_LEARN_MSG_CONF             *learn_msg_conf,
      SOC_SAND_OUT  SOC_PPC_FRWRD_MACT_LEARN_MSG_PARSE_INFO       *learn_events
    )
{
  uint32
    res = SOC_SAND_OK,
    i,
    event_index,
    skip_bytes,
    offset,
    fld_val;  
  uint8
    msg_buffer[SOC_PPC_FRWRD_MACT_LEARN_MSG_MAX_LEN],
    *buff;
  SOC_PPC_FRWRD_MACT_EVENT_INFO
    *c_event;
  uint32
    skipped_events = 0;
  ARAD_PP_LEM_ACCESS_KEY_ENCODED
      lem_key_in_buffer;
  ARAD_PP_LEM_ACCESS_KEY
      lem_key;
  uint32                    
      lem_payload_data[ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S];
  ARAD_PP_LEM_ACCESS_PAYLOAD     
      lem_payload;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_MACT_LEARN_MSG_PARSE_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(learn_msg);
  SOC_SAND_CHECK_NULL_INPUT(learn_msg_conf);
  SOC_SAND_CHECK_NULL_INPUT(learn_events);

  SOC_PPC_FRWRD_MACT_LEARN_MSG_PARSE_INFO_clear(learn_events);
  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&lem_payload);

  sal_memset(lem_payload_data,0,sizeof(uint32)*ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S);
  sal_memset(msg_buffer, 0, sizeof(msg_buffer));

  

  skip_bytes = learn_msg_conf->header_size;

  
  if (learn_msg_conf->recv_mode == SOC_TMC_PKT_PACKET_RECV_MODE_MSB_TO_LSB)
  {
    for (i = 0; i < learn_msg->msg_len - skip_bytes; i++)
    {
      msg_buffer[i] = learn_msg->msg_buffer[learn_msg->msg_len - skip_bytes - i - 1];
    }
  }
  else
  {
    if(learn_msg->msg_len - skip_bytes > SOC_PPC_FRWRD_MACT_LEARN_MSG_MAX_LEN)
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_MACT_MSG_LEN_ERR, 20, exit);
    }
    soc_sand_os_memcpy(msg_buffer,learn_msg->msg_buffer + skip_bytes,learn_msg->msg_len - skip_bytes);
  }

  
  
    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
            unit,
            msg_buffer,
            0,
            ARAD_PP_FRWRD_MACT_ETHERNET_TYPE_SIZE*8,
            &fld_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (fld_val != ARAD_PP_FRWRD_MACT_MESSAGE_DSP_TYPE)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_MACT_MSG_IS_NOT_LEARN_MSG_ERR, 15, exit);
  }
  fld_val = 0;

  
  learn_events->nof_events_in_msg = 0;
  
  offset = ARAD_PP_FRWRD_MACT_ETHERNET_TYPE_SIZE;
  for(event_index = 0; event_index < learn_msg->max_nof_events ; event_index++)
  {    
    if(offset > learn_msg->msg_len)
    {
      
      break;
    }

    
    buff = msg_buffer + offset;
    offset += SOC_PPC_FRWRD_MACT_LEARN_MSG_EVENT_LEN;

    
    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
            unit,
            buff,
            SOC_PPC_FRWRD_MACT_LEARN_MSG_VALID_INDEX,
            SOC_PPC_FRWRD_MACT_LEARN_MSG_VALID_LEN,
            &fld_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    if(fld_val != 1)
    {
      
      skipped_events++;
      continue;
    }

    learn_events->nof_events_in_msg++;
    
    c_event = &learn_events->events[learn_events->nof_parsed_events];
    learn_events->nof_parsed_events++;

    
    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
            unit,
            buff,
            SOC_PPC_FRWRD_MACT_LEARN_MSG_TYPE_INDEX,
            SOC_PPC_FRWRD_MACT_LEARN_MSG_TYPE_LEN,
            &fld_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    switch (fld_val)
    {
      case ARAD_PP_LEM_ACCESS_CMD_DELETE:
        c_event->type = SOC_PPC_FRWRD_MACT_EVENT_TYPE_AGED_OUT;
        break;

      case ARAD_PP_LEM_ACCESS_CMD_LEARN:
        c_event->type = SOC_PPC_FRWRD_MACT_EVENT_TYPE_LEARN;
        break;

      case ARAD_PP_LEM_ACCESS_CMD_TRANSPLANT:
        c_event->type = SOC_PPC_FRWRD_MACT_EVENT_TYPE_TRANSPLANT;
        break;

      case ARAD_PP_LEM_ACCESS_CMD_REFRESH:
        c_event->type = SOC_PPC_FRWRD_MACT_EVENT_TYPE_REFRESH;
        break;

      case ARAD_PP_LEM_ACCESS_CMD_ACK:
        c_event->type = SOC_PPC_FRWRD_MACT_EVENT_TYPE_ACK;
        break;

      case ARAD_PP_LEM_ACCESS_CMD_ERROR:
        c_event->type = SOC_PPC_FRWRD_MACT_EVENT_TYPE_REQ_FAIL;
        break;

      default:
        skipped_events++;
        continue;
    }

    
    
    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
      unit,
      buff,
      SOC_PPC_FRWRD_MACT_LEARN_MSG_ASD_1_INDEX,
      SOC_PPC_FRWRD_MACT_LEARN_MSG_ASD_1_LEN,
      &fld_val
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
    sal_memset(lem_payload_data,0,sizeof(uint32)*ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S);
    SHR_BITCOPY_RANGE(lem_payload_data,SOC_PPC_FRWRD_MACT_LEARN_MSG_ASD_2_LEN,&fld_val,0,SOC_PPC_FRWRD_MACT_LEARN_MSG_ASD_1_LEN);

    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
      unit,
      buff,
      SOC_PPC_FRWRD_MACT_LEARN_MSG_ASD_2_INDEX,
      SOC_PPC_FRWRD_MACT_LEARN_MSG_ASD_2_LEN,
      &fld_val
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
    lem_payload_data[0] |= fld_val;

    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
      unit,
      buff,
      SOC_PPC_FRWRD_MACT_LEARN_MSG_ASD_3_INDEX,
      SOC_PPC_FRWRD_MACT_LEARN_MSG_ASD_3_LEN,
      &fld_val
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

    lem_payload_data[1] |= fld_val << SOC_PPC_FRWRD_MACT_LEARN_MSG_ASD_2_LEN;


    
    sal_memset(&lem_payload,0,sizeof(lem_payload));
    
    res = arad_pp_lem_access_payload_parse(
            unit,
            lem_payload_data,
            ARAD_PP_LEM_ACCESS_NOF_KEY_TYPES,
            &lem_payload
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 153, exit);

    lem_payload.flags |= ARAD_PP_FWD_DECISION_PARSE_IS_LEARN;

    
    res = arad_pp_frwrd_mact_payload_convert(
            unit,
            &lem_payload,
            &(c_event->value)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 153, exit);

    
    c_event->value.aging_info.age_status = 0;

    

    
    res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
            unit,
            buff,
            SOC_PPC_FRWRD_MACT_LEARN_MSG_MFF_IS_KEY_INDEX,
            SOC_PPC_FRWRD_MACT_LEARN_MSG_MFF_IS_KEY_LEN,
            &fld_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    
    if(fld_val == 1)
    {
        res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
                unit,
                buff,
                SOC_PPC_FRWRD_MACT_LEARN_MSG_KEY_1_INDEX,
                SOC_PPC_FRWRD_MACT_LEARN_MSG_KEY_1_LEN,
                &fld_val
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
        lem_key_in_buffer.buffer[2] = fld_val;

        res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
                unit,
                buff,
                SOC_PPC_FRWRD_MACT_LEARN_MSG_KEY_2_INDEX,
                SOC_PPC_FRWRD_MACT_LEARN_MSG_KEY_2_LEN,
                &fld_val
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        lem_key_in_buffer.buffer[1] = fld_val;

        res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
                unit,
                buff,
                SOC_PPC_FRWRD_MACT_LEARN_MSG_KEY_3_INDEX,
                SOC_PPC_FRWRD_MACT_LEARN_MSG_KEY_3_LEN,
                &fld_val
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        lem_key_in_buffer.buffer[0] = fld_val;

            
        res = arad_pp_lem_key_encoded_parse(
                unit,
                &lem_key_in_buffer,
                &lem_key
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 151, exit);

        res = arad_pp_frwrd_mact_key_parse(
                unit,
                &lem_key,
                &(c_event->key)
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 152, exit);
    }
    else{
        
    }

    
    if(c_event->type != SOC_PPC_FRWRD_MACT_EVENT_TYPE_ACK)
    {

        res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
                unit,
                buff,
                SOC_PPC_FRWRD_MACT_LEARN_MSG_IS_LAG_INDEX,
                SOC_PPC_FRWRD_MACT_LEARN_MSG_IS_LAG_LEN,
                &fld_val
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
        c_event->lag.id = 0;
        if(fld_val) { 
            res = soc_sand_bitsteam_u8_ms_byte_first_get_field(
                      unit,
                      buff,
                      SOC_PPC_FRWRD_MACT_LEARN_MSG_LAG_INDEX,
                      SOC_PPC_FRWRD_MACT_LEARN_MSG_LAG_LEN,
                      &fld_val
                    );
            SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
            c_event->lag.id = fld_val;
        }
    }
    else
    {
      c_event->lag.is_lag = 0;
      c_event->lag.id = 0;
    }

  }

  
  for(; ; event_index++)
  {    
    if(offset > learn_msg->msg_len)
    {
      
      break;
    }
    learn_events->nof_events_in_msg++;

    
    buff = msg_buffer + offset;

    offset += SOC_PPC_FRWRD_MACT_LEARN_MSG_EVENT_LEN;
  }

  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_frwrd_mact_learn_msg_parse()", 0, 0);
}


uint32
  arad_pp_frwrd_mact_mim_init_set_unsafe(
      SOC_SAND_IN   int                                   unit,
      SOC_SAND_IN   uint8                                    mim_initialized
    )
{
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

   
  if (SOC_DPP_CONFIG(unit)->pp.test2 || SOC_DPP_CONFIG(unit)->pp.test1) {
      if (SOC_IS_ARADPLUS_AND_BELOW(unit)){
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 29, exit, soc_reg_field32_modify(unit, EPNI_EEDB_OUTLIF_BASEr, REG_PORT_ANY, EEDB_OUTLIF_BASEf, SOC_PPC_OUTLIF_BASE));
      } else {
          SOC_SAND_SOC_IF_ERROR_RETURN(res, 29, exit, soc_reg_field32_modify(unit, EPNI_EEDB_OUTLIF_BASEr, REG_PORT_ANY, EEDB_OUTLIF_BASEf, SOC_DPP_CONFIG(unit)->pp.mim_local_out_lif_base));
      }
  } 
  res = sw_state_access[unit].dpp.soc.arad.pp.oper_mode.mim_initialized.set(unit, mim_initialized);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_mim_init_set_unsafe()", 0, 0);
}



uint32
  arad_pp_frwrd_mact_mim_init_get_unsafe(
    SOC_SAND_IN    int                           unit,
    SOC_SAND_OUT   uint8                            *mim_initialized
  )
{
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = sw_state_access[unit].dpp.soc.arad.pp.oper_mode.mim_initialized.get(unit, mim_initialized);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_mim_init_get_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_mact_routed_learning_set_unsafe(
    SOC_SAND_IN  int                unit, 
    SOC_SAND_IN  uint32                appFlags
  )
{
  uint32
    app_idx,
    reg_val,
    field_val,
    res = SOC_SAND_OK;
  uint8
    learn_enable,
    is_ingress_learning,
    result_mapping_learn_enable;
  SOC_PPC_FRWRD_MACT_LEARNING_MODE
    learning_mode;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  result_mapping_learn_enable = appFlags ? 1:0;
  res = sw_state_access[unit].dpp.soc.arad.pp.fwd_mact.learning_mode.get(unit, &learning_mode);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

  res = arad_pp_frwrd_mact_is_ingress_learning_get(
          unit,
          learning_mode,
          &is_ingress_learning
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_frwrd_mact_opport_mode_set(unit, 2, is_ingress_learning, 1);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit)

  for (app_idx = 0; app_idx < ARAD_PP_FRWRD_MACT_NOF_ROUTED_LEARNING_APPS; app_idx++) {
    learn_enable = 0;
    if (appFlags & app_to_flp_prog_map[app_idx].app_type) {
      learn_enable = 1;
    }
    
    res = arad_pp_flp_prog_learn_set(
            unit,
            app_to_flp_prog_map[app_idx].fwd_code,
            learn_enable
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
 
  if (SOC_IS_ARADPLUS(unit)) {
     
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,READ_IHP_SA_LOOKUP_RESULT_MAPPINGr(unit, 0, &reg_val));

    field_val = 5 | ((result_mapping_learn_enable) << 3); 

    ARAD_FLD_TO_REG(IHP_SA_LOOKUP_RESULT_MAPPINGr,
                    SA_LOOKUP_RESULT_MAPPING_4f,
                    field_val, reg_val, 50, exit);


    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,WRITE_IHP_SA_LOOKUP_RESULT_MAPPINGr(unit, SOC_CORE_ALL, reg_val));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_routed_learning_set_unsafe()", 0, 0);

}

uint32
  arad_pp_frwrd_mact_routed_learning_get_unsafe(
    SOC_SAND_IN  int                unit, 
    SOC_SAND_OUT uint32                *appFlags
  )
{
  uint32
    app_idx, 
    app_types = 0,
    res = SOC_SAND_OK;
  uint8
    learn_enable;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  for (app_idx = 0; app_idx < ARAD_PP_FRWRD_MACT_NOF_ROUTED_LEARNING_APPS; app_idx++) {
    learn_enable = 0;
    
    res = arad_pp_flp_prog_learn_get(
              unit,
              app_to_flp_prog_map[app_idx].fwd_code,
              &learn_enable
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      if (learn_enable) {
        app_types |= app_to_flp_prog_map[app_idx].app_type;
      }
  }

  *appFlags = app_types;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_routed_learning_get_unsafe()", 0, 0);
}


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_frwrd_mact_mgmt_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_frwrd_mact_mgmt;
}

CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_frwrd_mact_mgmt_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_frwrd_mact_mgmt;
}
uint32
  SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_MACT_AGING_INFO_verify(
    SOC_SAND_IN   int                                   unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_AGING_INFO *info
  )
{
  uint32
    ticks_per_microsec,
     max_time,
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  ticks_per_microsec = arad_chip_mega_ticks_per_sec_get(unit);
  
  max_time = SOC_PPC_FRWRD_MACT_TIME_SEC_MAX * (ARAD_DFLT_TICKS_PER_SEC / (1000*1000)) / ticks_per_microsec;
  

  if (SOC_IS_QUX(unit)){
    max_time = max_time + 1;
  }

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_MACT_TIME, &(info->aging_time), 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->aging_time.sec, max_time, ARAD_PP_FRWRD_MACT_MGMT_SEC_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_MACT_AGING_INFO_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->event_type, SOC_PPC_FRWRD_MACT_EVENT_TYPE_MAX, SOC_PPC_FRWRD_MACT_EVENT_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->vsi_event_handle_profile, ARAD_PP_FRWRD_MACT_EVENT_HANDLE_PROFILE_MAX, ARAD_PP_FRWRD_MACT_VSI_EVENT_HANDLE_PROFILE_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE_verify()",0,0);
}
uint32
  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  
  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->header_type, ARAD_PP_FRWRD_MACT_HEADER_TYPE_MAX, ARAD_PP_FRWRD_MACT_HEADER_TYPE_OUT_OF_RANGE_ERR, 10, exit);
 
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_MACT_EVENT_PROCESSING_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_PROCESSING_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, ARAD_PP_FRWRD_MACT_TYPE_MAX, ARAD_PP_FRWRD_MACT_MGMT_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO, &(info->info), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_MACT_EVENT_PROCESSING_INFO_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_MACT_OPER_MODE_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_OPER_MODE_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  
  SOC_SAND_CHECK_NULL_INPUT(info);
  
  SOC_SAND_ERR_IF_ABOVE_MAX(info->learning_mode, SOC_PPC_FRWRD_MACT_LEARNING_MODE_MAX, SOC_PPC_FRWRD_MACT_LEARNING_MODE_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_MACT_EVENT_PROCESSING_INFO, &(info->learn_msgs_info), 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->shadow_mode, SOC_PPC_FRWRD_MACT_SHADOW_MODE_MAX, SOC_PPC_FRWRD_MACT_SHADOW_MODE_OUT_OF_RANGE_ERR, 13, exit);
  if (info->shadow_mode != SOC_PPC_FRWRD_MACT_SHADOW_MODE_NONE)
  {
    ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_MACT_EVENT_PROCESSING_INFO, &(info->shadow_msgs_info), 14, exit);
  }

  if ((info->learning_mode == SOC_PPC_FRWRD_MACT_LEARNING_MODE_EGRESS_INDEPENDENT) && (info->soc_petra_a_compatible == TRUE))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PPC_FRWRD_MACT_LEARNING_MODE_OUT_OF_RANGE_ERR, 20, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_MACT_OPER_MODE_INFO_verify()",0,0);
}
uint32
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO_verify(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(info);
  
  SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_entries, SOC_DPP_DEFS_GET(unit, max_mact_limit), ARAD_PP_FRWRD_MACT_FID_LIMIT_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->action_when_exceed, ARAD_PP_FRWRD_MACT_ACTION_WHEN_EXCEED_MAX, ARAD_PP_FRWRD_MACT_ACTION_WHEN_EXCEED_OUT_OF_RANGE_ERR, 12, exit);

  if (info->is_limited == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_MACT_FID_LIMIT_DIS_ERR, 14, exit);
  }

  if (info->action_when_exceed == SOC_PPC_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_EVENT)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_MACT_ACTION_WHEN_EXCEED_OUT_OF_RANGE_ERR, 40, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_verify(
    SOC_SAND_IN  int                                        unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->fid_fail, ARAD_PP_FRWRD_MACT_FID_FAIL_MAX, ARAD_PP_FRWRD_MACT_FID_FAIL_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(info->fid_allowed, SOC_DPP_DEFS_GET(unit, nof_fids), SOC_PPC_FID_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_verify(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->fid_base)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->fid_base, 0, SOC_PPC_FID_OUT_OF_RANGE_ERR, 13, exit);
  }
  if (info->generate_event == TRUE)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_MACT_MAC_LIMIT_GENERATE_EVENT_OUT_OF_RANGE_ERR, 20, exit);
  }

  if(info->glbl_limit != SOC_PPC_FRWRD_MACT_NO_GLOBAL_LIMIT) {    
    SOC_SAND_ERR_IF_ABOVE_NOF(info->glbl_limit, DPP_PP_LEM_NOF_ENTRIES(unit), ARAD_PP_FRWRD_MACT_GLBL_LIMIT_OUT_OF_RANGE_ERR, 21, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_RANGE_MAP_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MAC_LIMIT_RANGE_MAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->zero_msbs, 0, ARAD_PP_FRWRD_MACT_LIMIT_RANGE_MAP_ZERO_MSBS_MAX, ARAD_PP_GENERAL_VAL_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->shift_right_bits, 0, ARAD_PP_FRWRD_MACT_LIMIT_RANGE_MAP_SHIFT_RIGHT_MAX, ARAD_PP_GENERAL_VAL_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_BELOW_MIN(info->base_add_val, 0, ARAD_PP_GENERAL_VAL_OUT_OF_RANGE_ERR, 30, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_MACT_MAC_LIMIT_RANGE_MAP_INFO_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_MACT_MAC_LIMIT_MAPPING_INFO_verify(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_MAC_LIMIT_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->invalid_ptr, SOC_DPP_FRWRD_MACT_LIMIT_INV_MAP_PTR_MAX(unit), ARAD_PP_GENERAL_VAL_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->range_end[0], 0, ARAD_PP_FRWRD_MACT_LIMIT_RANGE_END_MAX, ARAD_PP_GENERAL_VAL_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->range_end[1], 0, ARAD_PP_FRWRD_MACT_LIMIT_RANGE_END_MAX, ARAD_PP_GENERAL_VAL_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->range_end[2], 0, ARAD_PP_FRWRD_MACT_LIMIT_RANGE_END_MAX, ARAD_PP_GENERAL_VAL_OUT_OF_RANGE_ERR, 40, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_MACT_MAC_LIMIT_RANGE_MAP_INFO_verify()",0,0);
}

uint32
  arad_pp_frwrd_mact_trap_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_TRAP_TYPE                trap_type_ndx,
    SOC_SAND_IN  uint32                                  port_profile_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_MACT_TRAP_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(trap_type_ndx, SOC_PPC_FRWRD_MACT_TRAP_TYPE_NDX_MAX, SOC_PPC_FRWRD_MACT_TRAP_TYPE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(port_profile_ndx, ARAD_PP_FRWRD_MACT_PORT_PROFILE_NDX_MAX, ARAD_PP_FRWRD_MACT_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_mact_trap_info_get_verify()", 0, port_profile_ndx);
}

uint32
  SOC_PPC_FRWRD_MACT_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->sa_drop_action_profile, ARAD_PP_FRWRD_MACT_SA_DROP_ACTION_PROFILE_MAX, ARAD_PP_FRWRD_MACT_SA_DROP_ACTION_PROFILE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->sa_unknown_action_profile, ARAD_PP_FRWRD_MACT_SA_UNKNOWN_ACTION_PROFILE_MAX, ARAD_PP_FRWRD_MACT_SA_UNKNOWN_ACTION_PROFILE_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->da_unknown_action_profile, ARAD_PP_FRWRD_MACT_DA_UNKNOWN_ACTION_PROFILE_MAX, ARAD_PP_FRWRD_MACT_DA_UNKNOWN_ACTION_PROFILE_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_MACT_PORT_INFO_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_MACT_EVENT_LAG_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_LAG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->id, ARAD_PP_FRWRD_MACT_ID_MAX, ARAD_PP_FRWRD_MACT_ID_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_MACT_EVENT_LAG_INFO_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_MACT_EVENT_INFO_verify(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, ARAD_PP_FRWRD_MACT_TYPE_MAX, ARAD_PP_FRWRD_MACT_MGMT_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  res = SOC_PPC_FRWRD_MACT_ENTRY_KEY_verify(unit, &(info->key));
  SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_MACT_ENTRY_VALUE, &(info->value), 12, exit);
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_MACT_EVENT_LAG_INFO, &(info->lag), 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_MACT_EVENT_INFO_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_MACT_EVENT_BUFFER_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_MACT_EVENT_BUFFER *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_PPC_FRWRD_MACT_EVENT_BUFF_MAX_SIZE; ++ind)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->buff[ind], ARAD_PP_FRWRD_MACT_MGMT_BUFF_MAX, ARAD_PP_FRWRD_MACT_MGMT_BUFF_OUT_OF_RANGE_ERR, 10, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->buff_len, ARAD_PP_FRWRD_MACT_MGMT_BUFF_LEN_MAX, ARAD_PP_FRWRD_MACT_MGMT_BUFF_LEN_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_MACT_EVENT_BUFFER_verify()",0,0);
}

STATIC
  void
    jer_mact_fifo_thread(void *param)
{
    int             unit = PTR_TO_INT(param);
    int             channel_number= -1;
    int             ch = 0;
    int             cmc = 0;
    uint32          ret;
    char            thread_name[SAL_THREAD_NAME_MAX_LEN];
    sal_thread_t    thread;

    SOCDNX_INIT_FUNC_DEFS;
    thread = sal_thread_self();
    thread_name[0] = 0;
    sal_thread_name(thread, thread_name, sizeof (thread_name));

    bsl_printf("%s - Entered, unit=%d \n", __func__, unit);
    ret = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_mgmt_dma_fifo_channel_get, (unit, dma_fifo_channel_src_olp, &channel_number));
    if ((ret != BCM_E_NONE) ||(channel_number == -1))
        SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("%s - Cannot get FIFO Channel Number, unit=%d - rc=%d \n"), __func__, unit, ret));

    if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc)) {
        cmc = channel_number / NOF_DMA_FIFO_PER_CMC;
        ch = channel_number % NOF_DMA_FIFO_PER_CMC;
    }
    bsl_printf("%s - channel=0x%x, cmc=%d, ch=%d \n", __func__, channel_number, cmc, ch);

    while (jer_mact_fifo_terminate[unit] == 0)
    {
        (void) sal_sem_take(jer_mact_fifo_sem[unit], sal_sem_FOREVER);
        arad_pp_frwrd_mact_learning_dma_event_handler(INT_TO_PTR(unit), 0, INT_TO_PTR(cmc), INT_TO_PTR(ch), 0);
    }

exit:
    soc_cmicm_cmcx_intr0_disable(unit, cmc, IRQ_CMCx_FIFO_CH_DMA(ch));
    sal_sem_destroy(jer_mact_fifo_sem[unit]);
    jer_mact_fifo_sem[unit] = NULL;
    jer_mact_fifo_terminate[unit] = 0;
    jer_mact_fifo_tid[unit] = SAL_THREAD_ERROR;
    bsl_printf("%s - Thread Exiting ... \n", __func__);
    sal_thread_exit(0);
       SOCDNX_FUNC_RETURN_VOID; 
    return;
}

STATIC
  uint32
    jer_mact_fifo_cleanup(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    bsl_printf("%s - Entered, unit=%d \n", __func__, unit);
    
    if ((jer_mact_fifo_tid[unit] != NULL) && (jer_mact_fifo_tid[unit] != SAL_THREAD_ERROR)) {
        jer_mact_fifo_terminate[unit] = 1;
        sal_sem_give(jer_mact_fifo_sem[unit]);
    } else {
        if (jer_mact_fifo_sem[unit]) {
             sal_sem_destroy(jer_mact_fifo_sem[unit]);
            jer_mact_fifo_sem[unit] = NULL;
       }
    }

    SOCDNX_FUNC_RETURN;
}

STATIC
  uint32
    jer_mact_fifo_init(int unit)
{
  char                        thread_name[SAL_THREAD_NAME_MAX_LEN];

  SOCDNX_INIT_FUNC_DEFS;
  sal_snprintf(thread_name, sizeof(thread_name), "MACT_fifo_thread.%d", unit);
  jer_mact_fifo_sem[unit] = sal_sem_create("MACT_fifo_sem", sal_sem_BINARY, 1);
  
  if(jer_mact_fifo_sem[unit] == NULL) {
    jer_mact_fifo_cleanup(unit);
    SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Sempahore allocation failure (MACT_fifo_sem.%d"), unit));
  }
  jer_mact_fifo_terminate[unit] = 0;

  jer_mact_fifo_tid[unit] = sal_thread_create(thread_name,
                                              SAL_THREAD_STKSZ,
                                              50 ,
                                              jer_mact_fifo_thread,
                                              INT_TO_PTR(unit));

  if ((jer_mact_fifo_tid[unit] == NULL) || (jer_mact_fifo_tid[unit] == SAL_THREAD_ERROR)) {
    jer_mact_fifo_cleanup(unit);
    SOCDNX_EXIT_WITH_ERR(SOC_SAND_GEN_ERR, (_BSL_SOCDNX_MSG("%s create failed. \n"), thread_name));
  }
  bsl_printf("%s - Task %s Craeted - %p \n", __func__, thread_name, (void *)jer_mact_fifo_tid[unit]);

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t arad_pp_frwrd_mact_learning_dma_set( SOC_SAND_IN  int unit )
{
    uint32 res;
    uint32 reg, dma_threshold;
    uint32 host_mem_num_entries , timeout;
    uint32 host_mem_size_in_bytes;
    uint32 size_of_learn_event;
    int dont, care, endian; 
    int channel_number = -1;
    int ch,cmc;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);


    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_mgmt_dma_fifo_channel_get, (unit, dma_fifo_channel_src_olp, &channel_number)));         
    
    if(channel_number == -1)
    {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_mgmt_dma_fifo_channel_free_find, (unit, FALSE, &channel_number)));        
        if (channel_number == -1) {
            
            LOG_ERROR(BSL_LS_SOC_OTHER,(BSL_META_U(unit, "unit %d Cannot find free DMA-channel."),unit));
            SOC_SAND_SET_ERROR_CODE(ARAD_PP_FRWRD_MACT_FIFO_OUT_OF_RANGE_ERR, 31, exit);
        }           
    }  
    if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc)) {
        cmc = channel_number / NOF_DMA_FIFO_PER_CMC;
        ch = channel_number % NOF_DMA_FIFO_PER_CMC;    
    } else {
        cmc = SOC_PCI_CMC(unit);
        ch = channel_number % NOF_DMA_FIFO_PER_CMC;  
    }

    g_next_dma_interrupt_message_num[unit] = 0;
    dma_threshold = soc_property_get(unit, spn_LEARNING_FIFO_DMA_THRESHOLD, 0x20);
    host_mem_size_in_bytes = soc_property_get(unit, spn_LEARNING_FIFO_DMA_BUFFER_SIZE, 0);  
    timeout = soc_property_get(unit, spn_LEARNING_FIFO_DMA_TIMEOUT, 0);

    res = sw_state_access[unit].dpp.soc.arad.pp.fwd_mact.size_of_learn_event.get(unit, &size_of_learn_event);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    
    
    host_mem_num_entries = (host_mem_size_in_bytes / size_of_learn_event) + 1;

    
    dma_threshold = (dma_threshold > LARGEST_NUMBER_OF_ENTRIES_FOR_DMA_HOST_MEMORY)? LARGEST_NUMBER_OF_ENTRIES_FOR_DMA_HOST_MEMORY: dma_threshold;

    
    host_mem_num_entries =  (host_mem_num_entries < dma_threshold) ? dma_threshold : host_mem_num_entries; 

    ROUND_UP_TO_NEAREST_POWER_OF_2(host_mem_num_entries);
    
    host_mem_num_entries = (host_mem_num_entries < SMALLEST_NUMBER_OF_ENTRIES_FOR_DMA_HOST_MEMORY)? SMALLEST_NUMBER_OF_ENTRIES_FOR_DMA_HOST_MEMORY : host_mem_num_entries;
    host_mem_num_entries = (host_mem_num_entries > LARGEST_NUMBER_OF_ENTRIES_FOR_DMA_HOST_MEMORY)? LARGEST_NUMBER_OF_ENTRIES_FOR_DMA_HOST_MEMORY : host_mem_num_entries;

    res = sw_state_access[unit].dpp.soc.arad.pp.fwd_mact.num_entries_in_dma_host_memory.set(unit, host_mem_num_entries);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    host_mem_size_in_bytes = host_mem_num_entries * size_of_learn_event;

    res = sw_state_access[unit].dpp.soc.arad.pp.fwd_mact.host_memory_allocated_bytes.set(unit, host_mem_size_in_bytes);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    g_dma_host_memory[unit] = soc_cm_salloc(unit, host_mem_size_in_bytes, "learning DMA");

    LOG_VERBOSE(BSL_LS_SOC_L2,
                (BSL_META_U(unit,
                            "Allocating %d memory \n"),host_mem_size_in_bytes ));
    if (!g_dma_host_memory[unit]) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("Memory allocation failed!")));
    }

    if (!SAL_BOOT_PLISIM)
    {
        
        res = _soc_mem_sbus_fifo_dma_stop(unit, channel_number);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 31, exit);
    }

    res = _soc_mem_sbus_fifo_dma_start_memreg(unit, channel_number,
                                             FALSE , 0, OLP_HEAD_OF_CPU_DSPG_CMD_FIFOr, MEM_BLOCK_ALL,
                                             0, host_mem_num_entries, g_dma_host_memory[unit]);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 31, exit);

    
    soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_HOSTMEM_THRESHOLD_OFFSET(cmc, ch),dma_threshold);

    reg=0;
    reg = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, ch));

    
    MICROSECONDS_TO_DMA_CFG__TIMEOUT_COUNT(timeout);
    timeout &= 0x3fff;

    soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_CFGr, &reg, TIMEOUT_COUNTf, timeout);

    soc_endian_get(unit, &dont,&care,&endian);
    soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_CFGr, &reg, ENDIANESSf, endian!=0);
    soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc, ch), reg);

    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_mgmt_dma_fifo_channel_set, (unit, channel_number, dma_fifo_channel_src_olp)));                     

    res = jer_mact_fifo_init(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

    
    soc_cmicm_cmcx_intr0_enable(unit, cmc, IRQ_CMCx_FIFO_CH_DMA(ch));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong in arad_pp_frwrd_mact_learning_dma_set")));
}


soc_error_t arad_pp_frwrd_mact_learning_dma_unset(SOC_SAND_IN int unit)
{
    uint32 res;
    int channel_number = -1;
    int ch,cmc;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_mgmt_dma_fifo_channel_get, (unit, dma_fifo_channel_src_olp, &channel_number)));         
    if(channel_number != -1)
    {
        if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc)) {
            cmc = channel_number / NOF_DMA_FIFO_PER_CMC;
            ch = channel_number % NOF_DMA_FIFO_PER_CMC;    
        } else {
            cmc = SOC_PCI_CMC(unit);
            ch = channel_number % NOF_DMA_FIFO_PER_CMC;  
        }    
        
        
        if (!SAL_BOOT_PLISIM)
        {
            res = _soc_mem_sbus_fifo_dma_stop(unit, channel_number);
            SOC_SAND_CHECK_FUNC_RESULT(res, 234 ,exit);
        }
        
        
        if (g_dma_host_memory[unit] != NULL) {
            soc_cm_sfree(unit, g_dma_host_memory[unit]);
        }
        
        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 31, exit, MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_mgmt_dma_fifo_channel_set, (unit, channel_number, dma_fifo_channel_src_reserved)));                 
        
        soc_cmicm_cmcx_intr0_disable(unit, cmc, IRQ_CMCx_FIFO_CH_DMA(ch));        

        res = jer_mact_fifo_cleanup(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}



void arad_pp_frwrd_mact_learning_dma_event_handler(
      SOC_SAND_INOUT  void  *unit_ptr,
      SOC_SAND_INOUT  void  *event_type_ptr,
      SOC_SAND_INOUT  void  *cmc_ptr,
      SOC_SAND_INOUT  void  *ch_ptr,
      SOC_SAND_INOUT  void  *unused4)
{
    int unit = BSL_UNIT_UNKNOWN;    
    uint32 res = SOC_SAND_OK;
    uint32 field_32;
    uint32 learn_event[_ARAD_PP_LEARNING_SIZE_OF_EVENT_IN_WORDS]={0};
    int num_events_in_host_memory=0;
    int num_handled_events=0;
    uint32 num_entries_in_dma_host_memory;
    uint32 size_of_learn_event;
    uint8 *next_event_in_host_mem;
    int ch,cmc;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    unit = PTR_TO_INT(unit_ptr);
    cmc = PTR_TO_INT(cmc_ptr);
    ch = PTR_TO_INT(ch_ptr);
    res = sw_state_access[unit].dpp.soc.arad.pp.fwd_mact.num_entries_in_dma_host_memory.get(unit, &num_entries_in_dma_host_memory);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    res = sw_state_access[unit].dpp.soc.arad.pp.fwd_mact.size_of_learn_event.get(unit, &size_of_learn_event);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    
    field_32 = 0;
    soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STAT_CLRr, &field_32, HOSTMEM_TIMEOUTf, 1);
    soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STAT_CLRr, &field_32, HOSTMEM_OVERFLOWf, 1);
    soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_CLR_OFFSET(cmc, ch), field_32);

    
    res = _soc_mem_sbus_fifo_dma_get_num_entries(unit, (cmc*NOF_DMA_FIFO_PER_CMC+ch), &num_events_in_host_memory);

    
    if (res != SOC_E_EMPTY)
    {
        for (num_handled_events=0; num_handled_events < num_events_in_host_memory; num_handled_events++) 
        {            
            
            if (g_next_dma_interrupt_message_num[unit] + num_handled_events >= num_entries_in_dma_host_memory) 
            {
                g_next_dma_interrupt_message_num[unit] -= num_entries_in_dma_host_memory;
            }

            next_event_in_host_mem = g_dma_host_memory[unit] + (g_next_dma_interrupt_message_num[unit] + num_handled_events)*size_of_learn_event;
            sal_memcpy(learn_event, next_event_in_host_mem, size_of_learn_event); 
            arad_pp_frwrd_mact_handle_learn_event(unit, learn_event); 
        }

        g_next_dma_interrupt_message_num[unit] = g_next_dma_interrupt_message_num[unit] + num_handled_events;

        
        res = _soc_mem_sbus_fifo_dma_set_entries_read(unit, (cmc*NOF_DMA_FIFO_PER_CMC+ch), num_events_in_host_memory);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 2134, exit);
    }


exit:
    
    soc_cmicm_cmcx_intr0_enable(unit, cmc, IRQ_CMCx_FIFO_CH_DMA(ch));

    SOC_SAND_VOID_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong in arad_pp_frwrd_mact_learning_dma_event_handler")));
}


soc_error_t arad_pp_frwrd_mact_clear_access_bit(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 fid,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS             *mac
  )
{
    soc_reg_above_64_val_t key = {0};
    uint32 access_mode_1 = 1;
    uint32 access_mode_2 = 2;
    uint32 lookup = 1;
    uint32 res = SOC_SAND_OK;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
    
    
    key[1]=(fid<<16)+((mac->address[5]&0xff)<<8)+((mac->address[4]&0xff));
    key[0]=((mac->address[3]&0xff)<<24)+((mac->address[2]&0xff)<<16)+((mac->address[1]&0xff)<<8)+((mac->address[0]&0xff));
    
	
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1200, exit, WRITE_PPDB_B_LARGE_EM_DIAGNOSTICS_KEYr(unit, key));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  28,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_DIAGNOSTICS_ACCESSED_MODEr, REG_PORT_ANY, 0,  LARGE_EM_DIAGNOSTICS_ACCESSED_MODEf,  access_mode_1));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_DIAGNOSTICSr, REG_PORT_ANY, 0, LARGE_EM_DIAGNOSTICS_LOOKUPf,  lookup));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  28,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_DIAGNOSTICS_ACCESSED_MODEr, REG_PORT_ANY, 0,  LARGE_EM_DIAGNOSTICS_ACCESSED_MODEf,  access_mode_2));
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong in arad_pp_frwrd_mact_clear_access_bit")));
}



soc_error_t arad_pp_frwrd_mact_opport_mode_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT  uint32                *opport
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_LEARNING_CFGr, REG_PORT_ANY, 0, OPPORTUNISTIC_LEARNINGf,  &fld_val));

  *opport = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_mact_opport_mode_set()", opport,0);
}




#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 

