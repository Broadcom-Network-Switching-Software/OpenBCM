/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPC_API_COUNTING_INCLUDED__

#define __SOC_PPC_API_COUNTING_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_64cnt.h>

#include <soc/dpp/PPC/ppc_api_general.h>















typedef enum
{
  
  SOC_PPC_CNT_COUNTER_TYPE_BYTES = 0,
  
  SOC_PPC_CNT_COUNTER_TYPE_PACKETS = 1,
  
  SOC_PPC_CNT_COUNTER_TYPE_BYTES_AND_PACKETS = 2,
  
  SOC_PPC_CNT_COUNTER_TYPE_PER_COLOR = 3,
  
  SOC_PPC_NOF_CNT_COUNTER_TYPES = 4
}SOC_PPC_CNT_COUNTER_TYPE;

typedef enum
{
  
  SOC_PPC_CNT_TYPE_INLIF = 0,
  
  SOC_PPC_CNT_TYPE_OUTLIF = 1,
  
  SOC_PPC_CNT_TYPE_IN_AC = 2,
  
  SOC_PPC_CNT_TYPE_OUT_AC = 3,
  
  SOC_PPC_CNT_TYPE_VSID = 4,
  
  SOC_PPC_CNT_TYPE_IN_PWE = 5,
  
  SOC_PPC_CNT_TYPE_OUT_PWE = 6,
  
  SOC_PPC_CNT_TYPE_IN_LABEL = 7,
  
  SOC_PPC_CNT_TYPE_OUT_LABEL = 8,
  
  SOC_PPC_CNT_TYPE_FEC_ID = 9,
  
  SOC_PPC_NOF_CNT_TYPES = 10
}SOC_PPC_CNT_TYPE;

typedef enum
{
  
  SOC_PPC_CNT_COUNTER_STAGE_INGRESS = 0,
  
  SOC_PPC_CNT_COUNTER_STAGE_EGRESS = 1,
  
  SOC_PPC_NOF_CNT_COUNTER_STAGES = 2
}SOC_PPC_CNT_COUNTER_STAGE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_CNT_COUNTER_STAGE stage;
  
  uint32 group;
  
  uint32 id;

} SOC_PPC_CNT_COUNTER_ID;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_CNT_TYPE type;
  
  uint32 id;

} SOC_PPC_CNT_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_64CNT pkt_cnt;
  
  SOC_SAND_64CNT byte_cnt;

} SOC_PPC_CNT_RESULT;










void
  SOC_PPC_CNT_COUNTER_ID_clear(
    SOC_SAND_OUT SOC_PPC_CNT_COUNTER_ID *info
  );

void
  SOC_PPC_CNT_KEY_clear(
    SOC_SAND_OUT SOC_PPC_CNT_KEY *info
  );

void
  SOC_PPC_CNT_RESULT_clear(
    SOC_SAND_OUT SOC_PPC_CNT_RESULT *info
  );

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_CNT_COUNTER_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_CNT_COUNTER_TYPE enum_val
  );

const char*
  SOC_PPC_CNT_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_CNT_TYPE enum_val
  );

const char*
  SOC_PPC_CNT_COUNTER_STAGE_to_string(
    SOC_SAND_IN  SOC_PPC_CNT_COUNTER_STAGE enum_val
  );

void
  SOC_PPC_CNT_COUNTER_ID_print(
    SOC_SAND_IN  SOC_PPC_CNT_COUNTER_ID *info
  );

void
  SOC_PPC_CNT_KEY_print(
    SOC_SAND_IN  SOC_PPC_CNT_KEY *info
  );

void
  SOC_PPC_CNT_RESULT_print(
    SOC_SAND_IN  SOC_PPC_CNT_RESULT *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

