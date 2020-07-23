/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_TMC_API_PMF_LOW_LEVEL_PGM_INCLUDED__

#define __SOC_TMC_API_PMF_LOW_LEVEL_PGM_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/TMC/tmc_api_ports.h>






#define  SOC_TMC_PMF_NOF_PGMS (32)


#define  SOC_TMC_PMF_NOF_CYCLES (2)









typedef enum
{
  
  SOC_TMC_PMF_PGM_SEL_TYPE_EEI_OUTLIF_15_8 = 0,
  
  SOC_TMC_PMF_PGM_SEL_TYPE_EEI_OUTLIF_7_0 = 1,
  
  SOC_TMC_PMF_PGM_SEL_TYPE_SEM_NDX_7_0 = 2,
  
  SOC_TMC_PMF_PGM_SEL_TYPE_PFQ_SEM_NDX_13_8 = 3,
  
  SOC_TMC_PMF_PGM_SEL_TYPE_FWDING_TTC_CODE = 4,
  
  SOC_TMC_PMF_PGM_SEL_TYPE_LOOKUPS = 5,
  
  SOC_TMC_PMF_PGM_SEL_TYPE_PARSER_PMF_PRO = 6,
  
  SOC_TMC_PMF_PGM_SEL_TYPE_LLVP_PFC = 7,
  
  SOC_TMC_NOF_PMF_PGM_SEL_TYPES = 8
}SOC_TMC_PMF_PGM_SEL_TYPE;

typedef enum
{
  
  SOC_TMC_PMF_PGM_BYTES_TO_RMV_HDR_START = 0,
  
  SOC_TMC_PMF_PGM_BYTES_TO_RMV_HDR_1ST = 1,
  
  SOC_TMC_PMF_PGM_BYTES_TO_RMV_HDR_FWDING = 2,
  
  SOC_TMC_PMF_PGM_BYTES_TO_RMV_HDR_POST_FWDING = 3,
  
  SOC_TMC_NOF_PMF_PGM_BYTES_TO_RMV_HDRS = 4
}SOC_TMC_PMF_PGM_BYTES_TO_RMV_HDR;

typedef enum
{
  
  SOC_TMC_PMF_PGM_HEADER_PROFILE_TM_MULTICAST = 0,
  
  SOC_TMC_PMF_PGM_HEADER_PROFILE_TM_OUTLIF = 5,
  
  SOC_TMC_PMF_PGM_HEADER_PROFILE_TM_UNICAST = 7,
  
  SOC_TMC_PMF_PGM_HEADER_PROFILE_STACKING = 8,
  
  SOC_TMC_PMF_PGM_HEADER_PROFILE_ETHERNET = 10,
  
  SOC_TMC_PMF_PGM_HEADER_PROFILE_ETHERNET_DSP = 12,
  
  SOC_TMC_PMF_PGM_HEADER_PROFILE_ETHERNET_LEARN = 13,
   
  SOC_TMC_PMF_PGM_HEADER_PROFILE_ETHERNET_LATENCY = 14,
  
  SOC_TMC_PMF_PGM_HEADER_PROFILE_ETHERNET_LEARN_DISABLE = 15

}SOC_TMC_PMF_PGM_HEADER_PROFILE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 pfq;
  
  uint32 sem_13_8_ndx;

} SOC_TMC_PMF_PGM_SEL_VAL_PFQ_SEM;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_PKT_FRWRD_TYPE fwd;
  
  SOC_TMC_TUNNEL_TERM_CODE ttc;

} SOC_TMC_PMF_PGM_SEL_VAL_FWD_TTC;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 prsr;
  
  uint32 port_pmf;

} SOC_TMC_PMF_PGM_SEL_VAL_PRSR_PMF;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 lem_1st_found;
  
  uint8 lem_2nd_found;
  
  uint8 sem_1st_found;
  
  uint8 sem_2nd_found;
  
  uint8 lpm_1st_not_dflt;
  
  uint8 lpm_2nd_not_dflt;
  
  uint8 tcam_found;
  
  uint8 elk_found;

} SOC_TMC_PMF_PGM_SEL_VAL_LKP;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 llvp;
  
  uint32 pmf_pro;

} SOC_TMC_PMF_PGM_SEL_VAL_LLVP_PFC;

typedef union
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 eei_outlif_15_8;
  
  uint32 eei_outlif_7_0;
  
  uint32 sem_7_0_ndx;
  
  SOC_TMC_PMF_PGM_SEL_VAL_PFQ_SEM pfq_sem;
  
  SOC_TMC_PMF_PGM_SEL_VAL_FWD_TTC fwd_ttc;
  
  SOC_TMC_PMF_PGM_SEL_VAL_PRSR_PMF prsr_pmf;
  
  SOC_TMC_PMF_PGM_SEL_VAL_LKP lkp;
  
  SOC_TMC_PMF_PGM_SEL_VAL_LLVP_PFC llvp_pfc;

} SOC_TMC_PMF_PGM_SEL_VAL;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_PMF_PGM_SEL_TYPE type;
  
  SOC_TMC_PMF_PGM_SEL_VAL val;

} SOC_TMC_PMF_PGM_SELECTION_ENTRY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 is_pgm_valid[SOC_TMC_PMF_NOF_PGMS];

} SOC_TMC_PMF_PGM_VALIDITY_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_PMF_PGM_BYTES_TO_RMV_HDR header_type;
  
  uint32 nof_bytes;

} SOC_TMC_PMF_PGM_BYTES_TO_RMV;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 lkp_profile_id[SOC_TMC_PMF_NOF_CYCLES];
  
  uint32 tag_profile_id;
  
  SOC_TMC_PORT_HEADER_TYPE header_type;
  
  SOC_TMC_PMF_PGM_BYTES_TO_RMV bytes_to_rmv;
  
  uint32 copy_pgm_var;
  
  SOC_TMC_PORTS_FC_TYPE fc_type;
  
  int header_profile;

} SOC_TMC_PMF_PGM_INFO;












void
  SOC_TMC_PMF_PGM_SEL_VAL_PFQ_SEM_clear(
    SOC_SAND_OUT SOC_TMC_PMF_PGM_SEL_VAL_PFQ_SEM *info
  );

void
  SOC_TMC_PMF_PGM_SEL_VAL_FWD_TTC_clear(
    SOC_SAND_OUT SOC_TMC_PMF_PGM_SEL_VAL_FWD_TTC *info
  );

void
  SOC_TMC_PMF_PGM_SEL_VAL_PRSR_PMF_clear(
    SOC_SAND_OUT SOC_TMC_PMF_PGM_SEL_VAL_PRSR_PMF *info
  );

void
  SOC_TMC_PMF_PGM_SEL_VAL_LKP_clear(
    SOC_SAND_OUT SOC_TMC_PMF_PGM_SEL_VAL_LKP *info
  );

void
  SOC_TMC_PMF_PGM_SEL_VAL_LLVP_PFC_clear(
    SOC_SAND_OUT SOC_TMC_PMF_PGM_SEL_VAL_LLVP_PFC *info
  );

void
  SOC_TMC_PMF_PGM_SEL_VAL_clear(
    SOC_SAND_IN  SOC_TMC_PMF_PGM_SEL_TYPE sel_type,
    SOC_SAND_OUT SOC_TMC_PMF_PGM_SEL_VAL *info
  );

void
  SOC_TMC_PMF_PGM_SELECTION_ENTRY_clear(
    SOC_SAND_OUT SOC_TMC_PMF_PGM_SELECTION_ENTRY *info
  );

void
  SOC_TMC_PMF_PGM_VALIDITY_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PMF_PGM_VALIDITY_INFO *info
  );

void
  SOC_TMC_PMF_PGM_BYTES_TO_RMV_clear(
    SOC_SAND_OUT SOC_TMC_PMF_PGM_BYTES_TO_RMV *info
  );

void
  SOC_TMC_PMF_PGM_INFO_clear(
    SOC_SAND_OUT SOC_TMC_PMF_PGM_INFO *info
  );

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_PMF_PGM_SEL_TYPE_to_string(
    SOC_SAND_IN  SOC_TMC_PMF_PGM_SEL_TYPE enum_val
  );

const char*
  SOC_TMC_PMF_PGM_BYTES_TO_RMV_HDR_to_string(
    SOC_SAND_IN  SOC_TMC_PMF_PGM_BYTES_TO_RMV_HDR enum_val
  );

void
  SOC_TMC_PMF_PGM_SEL_VAL_PFQ_SEM_print(
    SOC_SAND_IN  SOC_TMC_PMF_PGM_SEL_VAL_PFQ_SEM *info
  );

void
  SOC_TMC_PMF_PGM_SEL_VAL_FWD_TTC_print(
    SOC_SAND_IN  SOC_TMC_PMF_PGM_SEL_VAL_FWD_TTC *info
  );

void
  SOC_TMC_PMF_PGM_SEL_VAL_PRSR_PMF_print(
    SOC_SAND_IN  SOC_TMC_PMF_PGM_SEL_VAL_PRSR_PMF *info
  );

void
  SOC_TMC_PMF_PGM_SEL_VAL_LKP_print(
    SOC_SAND_IN  SOC_TMC_PMF_PGM_SEL_VAL_LKP *info
  );

void
  SOC_TMC_PMF_PGM_SEL_VAL_LLVP_PFC_print(
    SOC_SAND_IN  SOC_TMC_PMF_PGM_SEL_VAL_LLVP_PFC *info
  );

void
  SOC_TMC_PMF_PGM_SEL_VAL_print(
    SOC_SAND_IN  SOC_TMC_PMF_PGM_SEL_VAL *info
  );

void
  SOC_TMC_PMF_PGM_SELECTION_ENTRY_print(
    SOC_SAND_IN  SOC_TMC_PMF_PGM_SELECTION_ENTRY *info
  );

void
  SOC_TMC_PMF_PGM_VALIDITY_INFO_print(
    SOC_SAND_IN  SOC_TMC_PMF_PGM_VALIDITY_INFO *info
  );

void
  SOC_TMC_PMF_PGM_BYTES_TO_RMV_print(
    SOC_SAND_IN  SOC_TMC_PMF_PGM_BYTES_TO_RMV *info
  );

void
  SOC_TMC_PMF_PGM_INFO_print(
    SOC_SAND_IN  SOC_TMC_PMF_PGM_INFO *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

