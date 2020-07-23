/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_OAMP_PE_INCLUDED__

#define __ARAD_PP_OAMP_PE_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>





#define ARAD_PP_OAMP_MEP_PE_PROFILE_OUTLIF_OFFSET    0
#define ARAD_PP_OAMP_MEP_PE_PROFILE_COUNT_SLM_OFFSET       2
#define ARAD_PP_OAMP_MEP_PE_PROFILE_MAID48_OFFSET    3


#define ARAD_PP_OAMP_MEP_PE_PROFILE_SLM_OFFSET       4

typedef enum
{
    ARAD_PP_OAMP_PE_PROGS_DEFAULT, 
    ARAD_PP_OAMP_PE_PROGS_DEFAULT_CCM, 
    ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER,
    ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER_JER,
    ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH_JER, 
    ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER_CCM_JER, 
    ARAD_PP_OAMP_PE_PROGS_1DM, 
    ARAD_PP_OAMP_PE_PROGS_1DM_DOWN, 
    ARAD_PP_OAMP_PE_PROGS_ETH_TLV_ON_SERVER,  
    ARAD_PP_OAMP_PE_PROGS_BFD_ECHO,
    ARAD_PP_OAMP_PE_PROGS_BFD_UDP_CHECKSUM, 
    ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC, 
    ARAD_PP_OAMP_PE_PROGS_DOWN_MEP_TLV_FIX, 
    ARAD_PP_OAMP_PE_PROGS_BFD_IPV4_SINGLE_HOP, 
    ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV, 
    ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV_UDH, 
    ARAD_PP_OAMP_PE_PROGS_MAID_11B_END_TLV_ON_SERVER, 
    ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER_MAID_11B_JER, 
    ARAD_PP_OAMP_PE_PROGS_OAMTS_DOWN_MEP, 
    ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH, 
    ARAD_PP_OAMP_PE_PROGS_PPH_ADD_UDH_CCM_JER, 
    ARAD_PP_OAMP_PE_PROGS_UP_MEP_UDH_DEFAULT_JER, 
    ARAD_PP_OAMP_PE_PROGS_UP_MEP_UDH_MAID_48,
    ARAD_PP_OAMP_PE_PROGS_UDH_PUNT_JER, 
    ARAD_PP_OAMP_PE_PROGS_MHOP_SHOP_BFD, 
    ARAD_PP_OAMP_PE_PROGS_SEAMLESS_BFD,
    ARAD_PP_OAMP_PE_PROGS_DPORT_AND_MYDISCR_UPDATE_BFD, 
    ARAD_PP_OAMP_PE_PROGS_CHANGE_DPORT_TO_SHOP_BFD, 
    ARAD_PP_OAMP_PE_PROGS_MICRO_BFD, 
    ARAD_PP_OAMP_PE_PROGS_BFD_PWE_BOS_FIX, 
    ARAD_PP_OAMP_PE_PROGS_RFC_6374_PWE_BOS_FIX, 
    ARAD_PP_OAMP_PE_PROGS_MAID_48, 
    ARAD_PP_OAMP_PE_PROGS_LMM_DMM_FLEXIBLE_DA_QAX,
    ARAD_PP_OAMP_PE_PROGS_SLM_MAID_48,
    ARAD_PP_OAMP_PE_PROGS_MAID_48_UDH, 
    ARAD_PP_OAMP_PE_PROGS_UP_MEP_MAC_MC, 
    ARAD_PP_OAMP_PE_PROGS_CONSTRUCT_1711_PWE,
    ARAD_PP_OAMP_PE_PROGS_CONSTRUCT_1711_MPLS_TP,
    ARAD_PP_OAMP_PE_PROGS_CONSTRUCT_1711_PWE_DIS_COUNT,
    ARAD_PP_OAMP_PE_PROGS_CONSTRUCT_1711_MPLS_TP_DIS_COUNT,
    ARAD_PP_OAMP_PE_PROGS_SLM_DOWN, 
    ARAD_PP_OAMP_PE_PROGS_SLM_MAID_11, 
    ARAD_PP_OAMP_PE_PROGS_SLM_POINTER, 
    ARAD_PP_OAMP_PE_PROGS_EGR_INJ, 
    ARAD_PP_OAMP_PE_PROGS_SLM_EGR_INJ, 
    ARAD_PP_OAMP_PE_PROGS_LMR_DMR_EGR_INJ, 
    ARAD_PP_OAMP_PE_PROGS_LM_DM_EGR_INJ, 
    ARAD_PP_OAMP_PE_PROGS_EGR_INJ_48_MAID_QAX,
    ARAD_PP_OAMP_PE_PROGS_SLM_EGR_INJ_48_MAID_QAX,
    
    ARAD_PP_OAMP_PE_PROGS_DM_COUNT_QAX,
    ARAD_PP_OAMP_PE_PROGS_CCM_COUNT_QAX,
    ARAD_PP_OAMP_PE_PROGS_SLM_CCM_COUNT_QAX, 
   
    ARAD_PP_OAMP_PE_PROGS_CCM_COUNT_48_MAID_QAX,
    ARAD_PP_OAMP_PE_PROGS_SLM_CCM_COUNT_48_MAID_QAX, 
    ARAD_PP_OAMP_PE_PROGS_JUMBO_DM_TLV, 
    ARAD_PP_OAMP_PE_PROGS_JUMBO_DM_TLV_11B_MAID, 
    ARAD_PP_OAMP_PE_PROGS_JUMBO_DM_TLV_48B_MAID, 
    ARAD_PP_OAMP_PE_PROGS_MPLS_TP_VCCV, 
    
    ARAD_PP_OAMP_PE_PROGS_NOF_PROGS
} ARAD_PP_OAMP_PE_PROGRAMS;



typedef enum
{
    ARAD_PP_OAMP_PE_IN_FIFO_RD_BITS = 0     ,
    ARAD_PP_OAMP_PE_FEM1_SEL_BITS           ,
    ARAD_PP_OAMP_PE_FEM2_SEL_BITS           ,
    ARAD_PP_OAMP_PE_MUX1_SRC_BITS           ,
    ARAD_PP_OAMP_PE_MERGE1_INST_BITS        ,
    ARAD_PP_OAMP_PE_SHIFT1_SRC_BITS         ,
    ARAD_PP_OAMP_PE_SHIFT2_SRC_BITS         ,
    ARAD_PP_OAMP_PE_FDBK_FF_WR_BIT          ,
    ARAD_PP_OAMP_PE_BUFF_WR_BIT             ,
    ARAD_PP_OAMP_PE_BUFF_WR_BITS            ,
    ARAD_PP_OAMP_PE_BUFF_SIZE_SRC_BITS      ,
    ARAD_PP_OAMP_PE_BUFF1_SIZE_SRC_BITS     ,
    ARAD_PP_OAMP_PE_OP1_SEL_BITS            ,
    ARAD_PP_OAMP_PE_OP2_SEL_BITS            ,
    ARAD_PP_OAMP_PE_ALU_ACT_BITS            ,
    ARAD_PP_OAMP_PE_CMP1_ACT_BITS           ,
    ARAD_PP_OAMP_PE_ALU_DST_BITS            ,
    ARAD_PP_OAMP_PE_BUF_EOP_BIT             ,
    ARAD_PP_OAMP_PE_BUF_EOP_BITS            ,
    ARAD_PP_OAMP_PE_BUF_EOP_FRC_BIT         ,
    ARAD_PP_OAMP_PE_INST_CONST_BITS         ,
    ARAD_PP_OAMP_PE_FDBK_FF_RD_BIT          ,
    ARAD_PP_OAMP_PE_OP3_SEL_BITS            ,
    ARAD_PP_OAMP_PE_CMP2_ACT_BITS           ,
    ARAD_PP_OAMP_PE_INST_SRC_BITS           ,
    ARAD_PP_OAMP_PE_MUX2_SRC_BITS           ,
    ARAD_PP_OAMP_PE_BUFF2_SIZE_BITS         ,
    ARAD_PP_OAMP_PE_MERGE2_INST_BITS        ,

    ARAD_PP_OAMP_PE_INSTRUCTION_FIELDS_NOF
} ARAD_PP_OAMP_PE_INSTRUCTION_FIELDS;










#define _OAMP_PE_TCAM_KEY_PROTECTION_PACKET 0x7












uint32
  arad_pp_oamp_pe_unsafe(
    SOC_SAND_IN  int                                 unit
  );


uint32
    arad_pp_oamp_pe_seamless_bfd_src_port_set(
      SOC_SAND_IN   int                                 unit,
      SOC_SAND_IN  uint32                               src_port
    );

uint32
    arad_pp_oamp_pe_seamless_bfd_src_port_get(
      SOC_SAND_IN   int                                 unit,
      SOC_SAND_OUT  uint32                              *src_port
    );


void
  arad_pp_oamp_pe_program_profile_get(
    SOC_SAND_IN   int                                 unit,
    SOC_SAND_IN   ARAD_PP_OAMP_PE_PROGRAMS     program_id,
    SOC_SAND_OUT  uint32                                 *program_profile
  );



soc_error_t arad_pp_oam_oamp_pe_gen_mem_set(int unit, int gen_mem_index, int gen_mem_data);


soc_error_t arad_pp_oam_oamp_pe_gen_mem_get(int unit, int gen_mem_index, uint32 *gen_mem_data);



uint32
  arad_pp_oamp_pe_print_all_programs_data(int unit);
  

uint32
  arad_pp_oamp_pe_print_last_program_data(int unit);



int soc_arad_pp_set_mep_data_in_gen_mem(int unit,uint32 mep_id, uint16 data_1,uint16 data_2);


int soc_arad_pp_get_mep_data_in_gen_mem(int unit,uint32 mep_id, uint32 *data_1,uint32 *data_2);

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif




