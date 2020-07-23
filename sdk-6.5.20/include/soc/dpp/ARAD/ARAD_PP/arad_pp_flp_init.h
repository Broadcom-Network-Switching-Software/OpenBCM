

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/

#ifndef __ARAD_PP_FLP_INIT_INCLUDED__

#define __ARAD_PP_FLP_INIT_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_ce_instruction.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_trap_mgmt.h>
#include <soc/dpp/PPC/ppc_api_fp.h>














#define ARAD_PP_FLP_32B_INST_P6_GLOBAL_IN_LIF_D                                                          (0x8A28)



#define ARAD_PP_FLP_16B_INST_P6_FORWARDING_ACTION_TC_D                                                   (0x2af8)
#define ARAD_PP_FLP_16B_INST_P6_VT_LOOKUP0_PAYLOAD_D                                                     (0xf118)
#define ARAD_PP_FLP_16B_INST_P6_IN_PORT_D                                                                (0x7ec8)


#define ARAD_PP_FLP_16B_INST_P6_TT_LOOKUP0_PAYLOAD_D                                      ((SOC_IS_JERICHO(unit) || SOC_IS_QAX(unit))? 0xfcac: 0xf068)
#define ARAD_PP_FLP_16B_INST_P6_IN_LIF_D                                                  ((SOC_IS_JERICHO(unit) || SOC_IS_QAX(unit))? 0xfaac: 0xf688)



#define ARAD_PP_FLP_16B_INST_N_ZEROS(_n)                                                             (((_n-1) << 12) | (SOC_IS_JERICHO(unit)? 0x0fc8: 0x0fc8))
#define ARAD_PP_FLP_16B_INST_8_ZEROS                                                                 (ARAD_PP_FLP_16B_INST_N_ZEROS(8))
#define ARAD_PP_FLP_16B_INST_12_ZEROS                                                                (ARAD_PP_FLP_16B_INST_N_ZEROS(12))
#define ARAD_PP_FLP_16B_INST_15_ZEROS                                                                (ARAD_PP_FLP_16B_INST_N_ZEROS(15))

#define ARAD_PP_FLP_32B_INST_N_ZEROS(_n)                                                             (((_n-1) << 11) | (SOC_IS_JERICHO(unit)? 0x07c8: 0x07c8))

#define ARAD_PP_FLP_16B_INST_0_PROGRAM_KEY_GEN_VAR(_n)                                               (((_n-1) << 12)|(SOC_IS_JERICHO(unit)? 0x0248: 0x04e8))
#define ARAD_PP_FLP_16B_INST_P6_IN_PORT_KEY_GEN_VAR(_n)                                              (((_n-1) << 12)|(SOC_IS_JERICHO(unit)? 0x0e88: 0x0e88))
#define ARAD_PP_FLP_16B_INST_P6_IN_PORT_KEY_GEN_VAR_D_13_BITS                                        (ARAD_PP_FLP_16B_INST_P6_IN_PORT_KEY_GEN_VAR(13))
#define ARAD_PP_FLP_16B_INST_P6_IN_PORT_KEY_GEN_VAR_D_3_BITS                                         (ARAD_PP_FLP_16B_INST_P6_IN_PORT_KEY_GEN_VAR(3))
#define ARAD_PP_FLP_16B_INST_P6_VSI(_n)                                                              (((_n-1) << 12)|(SOC_IS_JERICHO(unit)? 0x08c8: 0x0a18))

#define ARAD_PP_FLP_16B_INST_P6_TT_LOOKUP0_PAYLOAD_D_BITS(_n)                                        (((_n-1) << 12)|(SOC_IS_JERICHO(unit)? 0x0cac : 0x0068))




#define ARAD_PP_FLP_VFT_NOF_BITS (13)

 
#define ARAD_PP_FLP_32B_INST_ARAD_FC_D_ID                   arad_pp_ce_instruction_composer(24,2,0,ARAD_PP_CE_IS_CE32);
#define ARAD_PP_FLP_32B_INST_ARAD_FC_S_ID                   arad_pp_ce_instruction_composer(24,2,4*8,ARAD_PP_CE_IS_CE32);



#define ARAD_PP_FLP_32B_INST_ARAD_FC_WITH_VFT_D_ID          arad_pp_ce_instruction_composer(24,3,0,ARAD_PP_CE_IS_CE32);
#define ARAD_PP_FLP_32B_INST_ARAD_FC_WITH_VFT_S_ID          arad_pp_ce_instruction_composer(24,3,4*8,ARAD_PP_CE_IS_CE32);



#define ARAD_PP_FLP_16B_INST_ARAD_FC_D_ID_8_MSB             arad_pp_ce_instruction_composer(8,2,0,ARAD_PP_CE_IS_CE16);
#define ARAD_PP_FLP_16B_INST_ARAD_FC_S_ID_8_MSB             arad_pp_ce_instruction_composer(8,2,4*8,ARAD_PP_CE_IS_CE16);



 
#define ARAD_IHP_FLP_16B_INST_ARAD_FC_WITH_VFT_D_ID_8_MSB   arad_pp_ce_instruction_composer(8,3,0,ARAD_PP_CE_IS_CE16);
#define ARAD_IHP_FLP_16B_INST_ARAD_FC_WITH_VFT_S_ID_8_MSB   arad_pp_ce_instruction_composer(8,3,4*8,ARAD_PP_CE_IS_CE16);



#define ARAD_PP_FLP_16B_INST_ARAD_FC_WITH_VFT_VFT_ID        arad_pp_ce_instruction_composer(ARAD_PP_FLP_VFT_NOF_BITS,2,16,ARAD_PP_CE_IS_CE16);








#define ARAD_PP_FLP_ETH_KEY_OR_MASK(unit)   ((SOC_IS_ARAD_B0_AND_ABOVE(unit)) ? 0x0 : 0x1)
#define ARAD_PP_FLP_DUMMY_KEY_OR_MASK(unit)   ((SOC_IS_ARAD_B0_AND_ABOVE(unit)) ? 0x1 : 0x0)

#define ARAD_PP_FLP_B_ETH_KEY_OR_MASK(unit) ((SOC_IS_ARAD_B0_AND_ABOVE(unit)) ? ARAD_PP_LEM_ACCESS_KEY_TYPE_PREFIX_SA_AUTH : 0x0)
#define ARAD_PP_FLP_IPV4_KEY_OR_MASK                0x2
#define ARAD_PP_FLP_LSR_KEY_OR_MASK                 0x3
#define ARAD_PP_FLP_P2P_KEY_OR_MASK                 0x4
#define ARAD_PP_FLP_IP_SPOOF_DHCP_KEY_OR_MASK       0x8 
#define ARAD_PP_FLP_IPV4_COMP_KEY_OR_MASK           0x6
#define ARAD_PP_FLP_MAC_IN_MAC_TUNNEL_KEY_OR_MASK   0x7
#define ARAD_PP_FLP_IPV4_SPOOF_STATIC_KEY_OR_MASK   0x5
#define ARAD_PP_FLP_TRILL_KEY_OR_MASK               0xa
#define ARAD_PP_FLP_TRILL_KEY_OR_MASK_MC            0xb


#define ARAD_PP_FLP_LKP_KEY_SELECT_FID_FWD_MAC_KEY_HW_VAL  (SOC_IS_JERICHO(unit)? 0x7: 0x3)


#define ARAD_PP_FLP_FC_KEY_OR_MASK                  0xc
#define ARAD_PP_FLP_FC_ZONING_KEY_OR_MASK           0xd
#define ARAD_PP_FLP_FC_REMOTE_KEY_OR_MASK           0xf
#define ARAD_PP_FLP_IPMC_BIDIR_KEY_OR_MASK          0x10
#define ARAD_PP_FLP_OMAC_2_VMAC_KEY_OR_MASK         0x11
#define ARAD_PP_FLP_VMAC_KEY_OR_MASK                0x12
#define ARAD_PP_FLP_VMAC_2_OMAC_KEY_OR_MASK         0x13
#define ARAD_PP_FLP_TRILL_ADJ_KEY_OR_MASK           0x14
#define ARAD_PP_FLP_SLB_KEY_OR_MASK                 0x15
#define ARAD_PP_FLP_GLOBAL_IPV4_KEY_OR_MASK         0x18 
#define ARAD_PP_FLP_FC_N_PORT_KEY_OR_MASK           0x19

#define ARAD_PP_FLP_IPV4_MC_SSM_KEY_OR_MASK         0x1a 
#define ARAD_PP_FLP_IPV6_MC_SSM_EUI_KEY_OR_MASK         0x1c

#define ARAD_PP_FLP_BFD_SINGLE_HOP_KEY_OR_MASK		0x20
#define ARAD_PP_FLP_IP6_SPOOF_STATIC_KEY_OR_MASK           0x22
#define ARAD_PP_FLP_IP6_COMPRESSION_DIP_KEY_OR_MASK        0x23
#define ARAD_PP_FLP_BFD_STATISTICS_KEY_OR_MASK      0x24
#define ARAD_PP_FLP_IP6_64_IN_LEM_KEY_OR_MASK       0x25 
#define ARAD_PP_FLP_ETHERNET_ING_IVL_LEARN_KEY_OR_MASK          0x28


#define ARAD_PP_FLP_DYNAMIC_DBAL_KEY_OR_MASK_BASE               0x29
#define ARAD_PP_FLP_BFD_ECHO_KEY_OR_MASK                        0x30
#define ARAD_PP_FLP_OAM_OUTLIF_MAP_KEY_OR_MASK                  0x31
#define ARAD_PP_FLP_MC_ENCAP_MAP_KEY_OR_MASK                    0x32
#define ARAD_PP_FLP_LSR_CNT_KEY_OR_MASK            0x1b  

#define ARAD_PP_FLP_DYNAMIC_DBAL_KEY_OR_MASK_END                0x5b
#define ARAD_PP_FLP_KEY_OR_MASK_LAST                0x5c 





#define ARAD_PP_FLP_PORT_PROFILE_DEFAULT      (0)
#define ARAD_PP_FLP_PORT_PROFILE_FC_N_PORT     (1)
#define ARAD_PP_FLP_PORT_PROFILE_PBP          (2)
#define ARAD_PP_FLP_PORT_PROFILE_CES          (4)


#define ARAD_PP_FLP_INSTRUCTIONS_NOF (SOC_DPP_IMP_DEFS_GET(unit, flp_instructions_nof))

#define ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_PBP     (1)
#define ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_NPV     (2)
#define ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_ALL     (0)
#define ARAD_PP_FLP_PORT_PROFILE_MASK_CONSIDER_NONE    (SOC_IS_JERICHO(unit)? 0x1F: 0x3)


#define ARAD_PP_FLP_KBP_MAX_ROP_PAYLOAD_SIZE_IN_BITS           	120
#define ARAD_PP_FLP_KBP_MAX_ROP_PAYLOAD_BIT_IN_BUFFER      		728
#define ARAD_PP_FLP_KBP_MAX_ROP_PAYLOAD_LSB_SIZE_IN_BITS       	128
#define ARAD_PP_FLP_KBP_MAX_ROP_PAYLOAD_LSB_BIT_IN_BUFFER      	856
#define ARAD_PP_FLP_KBP_MAX_RESULTS_LENGTH_IN_32b_ALIGNMENT		((ARAD_PP_FLP_KBP_MAX_ROP_PAYLOAD_SIZE_IN_BITS+ARAD_PP_FLP_KBP_MAX_ROP_PAYLOAD_LSB_SIZE_IN_BITS+31)/32)
#define ARAD_PP_FLP_KBP_MAX_NUMBER_OF_RESULTS                  	8
#define ARAD_PP_FLP_KBP_ROP_HEADERS_SIZE_IN_BITS 	            24


#define PROG_FLP_TM                                 0x00
#define PROG_FLP_ETHERNET_ING_LEARN                 0x01
#define PROG_FLP_IPV4UC_PUBLIC                      0x02
#define PROG_FLP_IPV4UC_RPF                         0x03
#define PROG_FLP_IPV4UC                             0x04
#define PROG_FLP_IPV6UC                             0x05
#define PROG_FLP_P2P                                0x06
#define PROG_FLP_IPV6MC                             0x07
#define PROG_FLP_LSR                                0x08
#define PROG_FLP_TRILL_UC                           0x0a 
#define PROG_FLP_ETHERNET_TK_EPON_UNI_V4_STATIC     0x0a 
#define PROG_FLP_PPPOE_IPV4UC                       0x0a 
#define PROG_FLP_TRILL_MC_ONE_TAG                   0x0b 
#define PROG_FLP_ETHERNET_TK_EPON_UNI_V6_STATIC     0x0b 
#define PROG_FLP_PPPOE_IPV6UC                       0x0b 
#define PROG_FLP_TRILL_MC_TWO_TAGS                  0x0c 
#define PROG_FLP_MAC_IN_MAC_AFTER_TERMINATIOM       0x0c
#define PROG_FLP_ETHERNET_TK_EPON_UNI_V4_DHCP       0x0d 
#define PROG_FLP_VPWS_TAGGED_SINGLE_TAG             0x0d 
#define PROG_FLP_ETHERNET_TK_EPON_UNI_V6_DHCP       0x0e 
#define PROG_FLP_VPWS_TAGGED_DOUBLE_TAG             0x0e 
#define PROG_FLP_ETHERNET_MAC_IN_MAC                0x0f
#define PROG_FLP_IP4UC_CUSTOM_ROUTE                 0x10 
#define PROG_FLP_TRILL_AFTER_TERMINATION            0x10 
#define PROG_FLP_IPV4MC_BRIDGE                      0x11 
#define PROG_FLP_IPV4COMPMC_WITH_RPF                0x12 
#define PROG_FLP_IPV4_DC                            0x13
#define PROG_FLP_MPLS_CONTROL_WORD_TRAP             0x14
#define PROG_FLP_BIDIR                              0x19
#define PROG_FLP_IPV6UC_RPF                         0x20
#define PROG_FLP_IP6UC_CUSTOM_ROUTE                 0x22 
#define PROG_FLP_VPLSOGRE                           0x22 
#define PROG_FLP_VMAC_UPSTREAM                      0x23 
#define PROG_FLP_VMAC_DOWNSTREAM                    0x24 
#define PROG_FLP_ETHERNET_PON_LOCAL_ROUTE           0x25 
#define PROG_FLP_ETHERNET_PON_DEFAULT_UPSTREAM      0x26 
#define PROG_FLP_ETHERNET_PON_DEFAULT_DOWNSTREAM    0x27 
#define PROG_FLP_IPV4MC_BRIDGE_USER_MODE            0X28 
#define PROG_FLP_IPV4MC_BRIDGE_USER_MODE_UNTAGGED   0X29 

#define PROG_FLP_GLOBAL_IPV4COMPMC_WITH_RPF         0x2a 
#define PROG_FLP_PON_ARP_DOWNSTREAM                 0x2b
#define PROG_FLP_PON_ARP_UPSTREAM                   0x2c
#define PROG_FLP_FC_TRANSIT                         0x2d 
#define PROG_FLP_IPV6UC_WITH_RPF_2PASS              0x2e
#define PROG_FLP_BFD_IPV4_SINGLE_HOP                0x2f
#define PROG_FLP_BFD_IPV6_SINGLE_HOP                0x30
#define PROG_FLP_OAM_STATISTICS                     0x31
#define PROG_FLP_BFD_STATISTICS                     0x32
#define PROG_FLP_OAM_DOWN_UNTAGGED_STATISTICS       0x33
#define PROG_FLP_OAM_SINGLE_TAG_STATISTICS          0x34
#define PROG_FLP_OAM_DOUBLE_TAG_STATISTICS          0x35
#define PROG_FLP_BFD_MPLS_STATISTICS                0x36
#define PROG_FLP_BFD_PWE_STATISTICS                 0x37
#define PROG_FLP_ETHERNET_ING_IVL_LEARN             0x38
#define PROG_FLP_ETHERNET_ING_IVL_INNER_LEARN       0x39
#define PROG_FLP_ETHERNET_ING_IVL_FWD_OUTER_LEARN   0x3a
#define PROG_FLP_IPV4UC_PUBLIC_RPF                  0x3b
#define PROG_FLP_FC_REMOTE                          0x3c 
#define PROG_FLP_FC_WITH_VFT_REMOTE                 0x3d 
#define PROG_FLP_FC                                 0x3e 
#define PROG_FLP_FC_WITH_VFT                        0x3f 
#define PROG_FLP_FC_N_PORT                          0x40 
#define PROG_FLP_FC_WITH_VFT_N_PORT                 0x41 
#define PROG_FLP_IPV6UC_PUBLIC                      0x42
#define PROG_FLP_IPV6UC_PUBLIC_RPF                  0x43
#define PROG_FLP_MYMAC_IP_DISABLED_TRAP             0x44

#define PROG_FLP_IPV4_MC_SSM                        0x45
#define PROG_FLP_IPV6_MC_SSM_EUI                    0x46
#define PROG_FLP_MPLS_EXP_NULL_TTL_REJECT           0x47
#define PROG_FLP_OAM_OUTLIF_L2G                     0x48
#define PROG_FLP_IPV4COMPMC_WITH_MIM_LEARNING       0x49
#define PROG_FLP_TWO_PASS_MC_ENCAP_TO_DEST          0x4a
#define PROG_FLP_IPV6_MC_SSM                        0x4b
#define PROG_FLP_IPV6_MC_SSM_COMPRESS_SIP           0x4c
#define PROG_FLP_LAST                               (PROG_FLP_IPV6_MC_SSM_COMPRESS_SIP)

#define ARAD_PP_FLP_MAP_PROG_NOT_SET                (0xff)


#define ARAD_PP_FLP_NUMBER_OF_FCOE_FCF_PROGRAMS                 4

#define ARAD_PP_FLP_DEFAULT_FWD_RES_SIZE            48
#define ARAD_PP_FLP_DEFAULT_RPF_RES_SIZE            24


#define ARAD_PP_FLP_PROGRAM_FWD_PROCESS_PROFILE_REPLACE_FWD_CODE    (0x01)



#define ARAD_PP_FLP_TCAM_LKP_KEY_SELECT_KEY_C_HW_VAL  (SOC_IS_JERICHO(unit)? 0x2: 0x1)


#define ARAD_PP_FLP_IPV6_DIP_SIP_SHARING_IS_PROG_VALID(opcode) ( \
    opcode == PROG_FLP_IPV6UC || \
    opcode == PROG_FLP_IPV6UC_RPF || \
    opcode == PROG_FLP_IPV6MC || \
    opcode == PROG_FLP_IPV6UC_PUBLIC || \
    opcode == PROG_FLP_IPV6UC_PUBLIC_RPF \
) \


#define ARAD_PP_FLP_IPV6_DIP_SIP_SHARING_IS_PROG_UC_VALID(opcode) ( \
    opcode == PROG_FLP_IPV6UC || \
    opcode == PROG_FLP_IPV6UC_PUBLIC \
) \

#define ARAD_PP_FLP_PON_PROG_LOAD() ((SOC_DPP_CONFIG(unit)->pp.pon_application_enable) || (is_pon_dpp_support))

#define INVALID_FLP_HW_PROG 0xff
#define MAX_PROG_IN_NS      64




typedef enum
{
   
    ARAD_PP_FLP_L3_MC_USE_TCAM_DISABLE = 0,
   
    ARAD_PP_FLP_L3_MC_USE_TCAM_ENABLE = 1,
    
    ARAD_PP_FLP_L3_MC_USE_TCAM_NO_IPV4_VRF = 2
} ARAD_PP_FLP_L3_MC_USE_TCAM;


typedef enum
{
    ARAD_PP_FLP_PROG_ETH = 0,
    ARAD_PP_FLP_PROG_IPV4 = 1,
    ARAD_PP_FLP_PROG_IPV6 = 2,
    ARAD_PP_FLP_PROG_MPLS = 3,
    ARAD_PP_FLP_PROG_NOP,
    ARAD_PP_FLP_PROG_NOF
} ARAD_PP_FLP_PROG_NS;


typedef struct 
{
    SOC_SAND_MAGIC_NUM_VAR
    
    uint16 count;
    
    uint8 hw_prog[MAX_PROG_IN_NS];
} ARAD_PP_FLP_PROG_NS_INFO;




















#if defined(INCLUDE_KBP)

uint32
    arad_pp_flp_elk_prog_config_max_key_size_get(
       SOC_SAND_IN  int  unit,
       SOC_SAND_IN  uint32  prog_id,
	   SOC_SAND_IN  uint32  zone_id,
       SOC_SAND_OUT uint32   *max_key_size_in_bits
    );

uint32
    arad_pp_flp_elk_prog_config(
       SOC_SAND_IN  int  unit,
       SOC_SAND_IN  uint32  prog_id,
       SOC_SAND_IN  uint32  opcode,
       SOC_SAND_IN  int     nof_shared_quals,
       SOC_SAND_IN  uint32  key_size_msb,
       SOC_SAND_IN  uint32  key_size
    );

#endif


uint32
   arad_pp_flp_prog_sel_cam_key_program_tm(
     int unit
   );
uint32
   arad_pp_flp_process_key_program_tm(
     int unit
   );

#if defined(INCLUDE_KBP)
uint32
   arad_pp_flp_elk_result_configure(int unit, int prog_id, int fwd_res_size,  int fwd_res_found_bit_offset, int fec_size,
                                    int fec_res_data_start, int fec_res_found_bit_offset );
#endif


uint32
  arad_pp_flp_init(
     int unit,
     uint8 ingress_learn_enable, 
     uint8 ingress_learn_oppurtunistic, 
     uint32  sa_lookup_type 
   );


uint32
  arad_pp_flp_ethernet_prog_update(
     int unit,
     uint8 learn_enable,
     uint8 ingress_learn_enable, 
     uint8 ingress_learn_oppurtunistic, 
     uint32  sa_lookup_type 
   );

uint32
  arad_pp_flp_ethernet_prog_learn_get(
     int unit,
     uint8 *learn_enable 
   );



uint32
  arad_pp_flp_trap_config_update(
     int unit,
     SOC_PPC_TRAP_CODE_INTERNAL trap_code_internal,
     int trap_strength,  
     int snoop_strength
   );

uint32
  arad_pp_flp_trap_config_get(
     int unit,
     SOC_PPC_TRAP_CODE_INTERNAL trap_code_internal,
     uint32 *trap_strength, 
     uint32  *snoop_strength
   );


uint32
    arad_pp_flp_prog_learn_set(
       int unit,
       int32  fwd_code,
       uint8  learn_enable
    );


uint32
  arad_pp_flp_prog_learn_get(
     int unit,
     int32  fwd_code,
     uint8  *learn_enable
  );


uint32
   arad_pp_ipmc_not_found_proc_update(
     int unit,
     uint8  flood
   );

uint32
   arad_pp_ipmc_not_found_proc_get(
     int unit,
     uint8  *flood
   );


uint32
   arad_pp_ipv4mc_bridge_lookup_update(
     int unit,
     uint8  mode 
   );

uint32
   arad_pp_ipv4mc_bridge_lookup_get(
     int unit,
     uint8  *mode
   );



uint32
   arad_pp_flp_key_const_lsr(
     int unit,
     uint8  in_port,
     uint8  in_rif,
     uint8  in_exp
   );

uint32
   arad_pp_flp_key_const_pwe_gre(
     int unit,
     uint8  in_rif,
     uint8  in_exp
   );

uint32
   arad_pp_flp_lookups_tcam_profile_set(
     int unit,
     uint32 tcam_access_profile_ndx,
     uint32 tcam_access_profile_id,
     uint32 prog_id
   );


uint32
    arad_pp_flp_lookups_TRILL_mc_update(int unit,
     uint32 is_ingress_learn);


uint32
   arad_pp_flp_lookups_TRILL_mc(
     int unit,
     uint32 is_ingress_learn,
     uint32 tcam_access_profile_id
   );

uint32
   arad_pp_flp_lookups_ipv4mc_with_rpf(
     int unit,
     uint8 ingress_learn_enable, 
     uint8 ingress_learn_oppurtunistic 
   );


uint32
   arad_pp_flp_lookups_ipv4compmc_with_rpf(
     int unit,
     uint32 tcam_access_profile_id
   );

uint32
   arad_pp_flp_lookups_global_ipv4compmc_with_rpf(
     int    unit,
     int prog_id,
     uint32 tcam_access_profile_id
   );

uint32
   arad_pp_flp_lookups_ipv6uc(
     int unit,
     uint32 tcam_access_profile_id
   );

uint32
   arad_pp_flp_lookups_ipv6mc(
     int unit,
     uint32 tcam_access_profile_id
   );

uint32
   arad_pp_flp_lookups_oam(
     int unit,
     uint32 tcam_access_profile_id_0,
     uint32 tcam_access_profile_id_1,
     uint32 flp_key_program
   );

uint32
   arad_pp_flp_lookups_ethernet_tk_epon_uni_v6(
     int unit,
     uint32 tcam_access_profile_id,
     uint8 ingress_learn_enable,
     uint8 ingress_learn_oppurtunistic
   );

uint32
   arad_pp_flp_lookups_ethernet_pon_default_downstream(
     int unit,
     uint32 tcam_access_profile_id,
     uint8 ingress_learn_enable, 
     uint8 ingress_learn_oppurtunistic, 
     int32 prog_id
   );


uint32
   arad_pp_flp_tk_epon_uni_v6_ing_learn_get(
     int unit,
     uint8 *ingress_learn_enable,
     uint8 *ingress_learn_oppurtunistic
   );


char*
  arad_pp_flp_prog_id_to_prog_name(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 prog_id
);

uint32
  arad_pp_flp_access_print_all_programs_data(
    SOC_SAND_IN  int unit
  );


uint32 arad_pp_flp_app_to_prog_index_get(
   int unit,
   uint32 app_id,
   uint8  *prog_index
);

uint32 arad_pp_prog_index_to_flp_app_get(
   int unit,
   uint32 prog_index,
   uint8  *app_id
);

uint32
arad_pp_flp_fcoe_zoning_set(
     int unit,
     int enable
   );

uint32
arad_pp_flp_fcoe_vsan_mode_set(
     int unit,
     int is_vsan_from_vsi
   );

uint32
   arad_pp_flp_n_port_programs_disable(
     int unit
   );

uint32
   arad_pp_flp_npv_programs_init(
     int unit
   );

uint32
arad_pp_flp_fcoe_is_zoning_enabled(
     int unit,
     int* is_enabled
   );

uint32
arad_pp_flp_fcoe_is_vsan_from_vsi_mode(
     int unit,
     int* is_vsan_from_vsi
   );
uint32   arad_pp_flp_lsr_stat_init(int unit);


uint32
  arad_pp_flp_not_found_trap_config_set(
     int unit,
     int hw_trap_id
   );


uint32
  arad_pp_flp_not_found_trap_config_get(
     int unit,
     int *hw_trap_id
   );


uint32
  arad_pp_flp_access_print_last_programs_data(
    SOC_SAND_IN   int                 unit,
    SOC_SAND_IN   int                 core_id,
    SOC_SAND_IN   int                 to_print,
    SOC_SAND_OUT  int                 *prog_id,
    SOC_SAND_OUT  int                 *sec_prog_id
  );


uint32 
    arad_pp_flp_program_get_namespace_info(
        SOC_SAND_IN int unit,
        SOC_SAND_IN int ns_id,
        SOC_SAND_OUT ARAD_PP_FLP_PROG_NS_INFO *ns_info
    );


uint32 arad_pp_flp_update_programs_pem_contex(int unit);




#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

