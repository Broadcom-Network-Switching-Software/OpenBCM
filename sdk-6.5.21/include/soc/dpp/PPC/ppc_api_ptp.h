/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPC_API_PTP_INCLUDED__

#define __SOC_PPC_API_PTP_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>













enum {

	SOC_PPC_PTP_ACTION_FIELD_ACTION_INDEX_TRAP  = 0,
	SOC_PPC_PTP_ACTION_FIELD_ACTION_INDEX_DROP  = 1,
	SOC_PPC_PTP_ACTION_FIELD_ACTION_INDEX_FWD   = 2
};

typedef enum
{
  SOC_PPC_PTP_IN_PP_PORT_PROFILE_0    = 0,
  SOC_PPC_PTP_IN_PP_PORT_PROFILE_1    = 1,
  SOC_PPC_PTP_IN_PP_PORT_PROFILE_2    = 2,
  SOC_PPC_PTP_IN_PP_PORT_PROFILE_3    = 3,

  SOC_PPC_PTP_IN_PP_PORT_PROFILES_NUM

}SOC_PPC_PTP_IN_PP_PORT_PROFILE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 ptp_enabled;

#define SOC_PPC_PTP_PORT_TIMESYNC_ONE_STEP_TIMESTAMP 0x00000004 
#define SOC_PPC_PTP_PORT_TIMESYNC_TWO_STEP_TIMESTAMP 0x00000008 
  
    uint32 flags;

#define SOC_PPC_PTP_1588_PKT_SYNC                 0x0 
#define SOC_PPC_PTP_1588_PKT_DELAY_REQ            0x1 
#define SOC_PPC_PTP_1588_PKT_PDELAY_REQ           0x2 
#define SOC_PPC_PTP_1588_PKT_PDELAY_RESP          0x3 
#define SOC_PPC_PTP_1588_PKT_FOLLOWUP             0x8 
                                                  
#define SOC_PPC_PTP_1588_PKT_DELAY_RESP           0x9 
#define SOC_PPC_PTP_1588_PKT_PDELAY_RESP_FOLLOWUP 0xa 
#define SOC_PPC_PTP_1588_PKT_ANNOUNCE             0xb 
#define SOC_PPC_PTP_1588_PKT_SIGNALLING           0xc 
#define SOC_PPC_PTP_1588_PKT_MANAGMENT            0xd 
                                                  
  
  uint32 pkt_drop;
  uint32 pkt_tocpu;
    
  int user_trap_id;

} SOC_PPC_PTP_PORT_INFO;









void
  SOC_PPC_PTP_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_PTP_PORT_INFO *info
  );

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_PTP_IN_PP_PORT_PROFILE_to_string(
    SOC_SAND_IN  SOC_PPC_PTP_IN_PP_PORT_PROFILE enum_val
  );

void
  SOC_PPC_PTP_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPC_PTP_PORT_INFO *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
