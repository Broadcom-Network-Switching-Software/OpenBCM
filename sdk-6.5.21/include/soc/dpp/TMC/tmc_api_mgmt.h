/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __SOC_TMC_API_MGMT_INCLUDED__

#define __SOC_TMC_API_MGMT_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/SAND_FM/sand_chip_defines.h>
#include <soc/dpp/TMC/tmc_api_general.h>
#include <soc/dpp/TMC/tmc_api_ports.h>

















#define SOC_TMC_MGMT_PCKT_SIZE_EXTERN_NO_LIMIT 0




#define SOC_TMC_MGMT_OCB_VOQ_NOF_THRESHOLDS  (2) 

#define SOC_TMC_MGMT_OCB_PRM_EN_TH_DEFAULT   (0x17f)   


#define SOC_TMC_FAP_CREDIT_VALUE_LOCAL 0
#define SOC_TMC_FAP_CREDIT_VALUE_REMOTE 1
#define SOC_TMC_FAP_CREDIT_VALUE_MAX 1



#define SOC_TMC_CREDIT_SIZE_BYTES_MIN 1


#define SOC_TMC_CREDIT_SIZE_BYTES_MAX ((8 * 1024) - 1)
















typedef enum
{
  
  SOC_TMC_INIT_PDM_MODE_SIMPLE = 0,
  
  SOC_TMC_INIT_PDM_MODE_REDUCED = 1,
  
  SOC_TMC_INIT_PDM_NOF_MODES = 2
} SOC_TMC_INIT_PDM_MODE;

typedef enum
{
  SOC_TMC_MGMT_CORE_CLK_RATE_250MHZ = 250,
  SOC_TMC_MGMT_CORE_CLK_RATE_300MHZ = 300
}SOC_TMC_MGMT_CORE_CLK_RATE;

typedef enum
{
  
  SOC_TMC_MGMT_FABRIC_HDR_TYPE_PETRA = 0,
  
  SOC_TMC_MGMT_FABRIC_HDR_TYPE_FAP20 = 1,
  
  SOC_TMC_MGMT_FABRIC_HDR_TYPE_FAP10M = 2,
  
  SOC_TMC_MGMT_NOF_FABRIC_HDR_TYPES = 3
}SOC_TMC_MGMT_FABRIC_HDR_TYPE;

typedef enum
{
  
  SOC_TMC_MGMT_TDM_MODE_PACKET = 0,
  
  SOC_TMC_MGMT_TDM_MODE_TDM_OPT = 1,
  
  SOC_TMC_MGMT_TDM_MODE_TDM_STA = 2,
  
  SOC_TMC_MGMT_NOF_TDM_MODES = 3
}SOC_TMC_MGMT_TDM_MODE;

typedef enum
{
  
  SOC_TMC_MGMT_PCKT_SIZE_CONF_MODE_EXTERN = 0,
  
  SOC_TMC_MGMT_PCKT_SIZE_CONF_MODE_INTERN = 1,
  
  SOC_TMC_MGMT_NOF_PCKT_SIZE_CONF_MODES = 2
}SOC_TMC_MGMT_PCKT_SIZE_CONF_MODE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 min;
  
  uint32 max;
} SOC_TMC_MGMT_PCKT_SIZE;

typedef struct{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 voq_eligible;
  
  uint8 dram_admission_exempt;
  
  uint32 th_words[SOC_TMC_MGMT_OCB_VOQ_NOF_THRESHOLDS];
  
  uint32 th_buffers[SOC_TMC_MGMT_OCB_VOQ_NOF_THRESHOLDS];
}SOC_TMC_MGMT_OCB_VOQ_INFO;











void
  SOC_TMC_MGMT_PCKT_SIZE_clear(
    SOC_SAND_OUT SOC_TMC_MGMT_PCKT_SIZE *info
  );

void
  SOC_TMC_MGMT_OCB_VOQ_INFO_clear(
    SOC_SAND_OUT SOC_TMC_MGMT_OCB_VOQ_INFO *info
  );

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_MGMT_FABRIC_HDR_TYPE_to_string(
    SOC_SAND_IN SOC_TMC_MGMT_FABRIC_HDR_TYPE enum_val
  );

const char*
  SOC_TMC_MGMT_TDM_MODE_to_string(
    SOC_SAND_IN  SOC_TMC_MGMT_TDM_MODE enum_val
  );

const char*
  SOC_TMC_MGMT_PCKT_SIZE_CONF_MODE_to_string(
    SOC_SAND_IN  SOC_TMC_MGMT_PCKT_SIZE_CONF_MODE enum_val
  );

void
  SOC_TMC_MGMT_PCKT_SIZE_print(
    SOC_SAND_IN  SOC_TMC_MGMT_PCKT_SIZE *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
