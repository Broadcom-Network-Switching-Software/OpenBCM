/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __SOC_TMC_API_STACK_INCLUDED__

#define __SOC_TMC_API_STACK_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/TMC/tmc_api_general.h>






#define  SOC_TMC_STACK_PRUN_BMP_LEN (1)









typedef enum
{
  
  SOC_TMC_STACK_MAX_NOF_TM_DOMAINS_1 = 1,
  
  SOC_TMC_STACK_MAX_NOF_TM_DOMAINS_8 = 8,
  
  SOC_TMC_STACK_MAX_NOF_TM_DOMAINS_16 = 16
}SOC_TMC_STACK_MAX_NOF_TM_DOMAINS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_STACK_MAX_NOF_TM_DOMAINS max_nof_tm_domains;
  
  uint32 my_tm_domain;

} SOC_TMC_STACK_GLBL_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 peer_tm_domain;
  
  uint32 prun_bmp[SOC_TMC_STACK_PRUN_BMP_LEN];

} SOC_TMC_STACK_PORT_DISTR_INFO;



typedef enum
{
  
  SOC_TMC_STACK_EGR_PROG_TM_PORT_PROFILE_STACK1 = 1,
  
  SOC_TMC_STACK_EGR_PROG_TM_PORT_PROFILE_STACK2 = 2,
  
  SOC_TMC_STACK_EGR_PROG_TM_PORT_PROFILE_STACK_ALL = 3
}SOC_TMC_STACK_EGR_PROG_TM_PORT_PROFILE_STACK;










void
  SOC_TMC_STACK_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_TMC_STACK_GLBL_INFO *info
  );

void
  SOC_TMC_STACK_PORT_DISTR_INFO_clear(
    SOC_SAND_OUT SOC_TMC_STACK_PORT_DISTR_INFO *info
  );

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_STACK_MAX_NOF_TM_DOMAINS_to_string(
    SOC_SAND_IN  SOC_TMC_STACK_MAX_NOF_TM_DOMAINS enum_val
  );

void
  SOC_TMC_STACK_GLBL_INFO_print(
    SOC_SAND_IN  SOC_TMC_STACK_GLBL_INFO *info
  );

void
  SOC_TMC_STACK_PORT_DISTR_INFO_print(
    SOC_SAND_IN  SOC_TMC_STACK_PORT_DISTR_INFO *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

