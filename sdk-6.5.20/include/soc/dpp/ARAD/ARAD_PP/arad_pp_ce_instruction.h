
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_CE_INST_INCLUDED__

#define __ARAD_PP_CE_INST_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>





#define ARAD_PP_CE_IS_CE16                                          (0)
#define ARAD_PP_CE_IS_CE32                                          (1)



#define ARAD_PP_CE_HEADER_0         0
#define ARAD_PP_CE_HEADER_1         1
#define ARAD_PP_CE_HEADER_2         2
#define ARAD_PP_CE_HEADER_3         3
#define ARAD_PP_CE_HEADER_4         4
#define ARAD_PP_CE_HEADER_5         5
#define ARAD_PP_CE_HEADER_FWD       6
#define ARAD_PP_CE_HEADER_AFTER_FWD 7

#define ARAD_PP_CE_IPV4_DIP_HDR2_HEADER arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_2,128,ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_IPV4_SIP_HDR2_HEADER_31_16 arad_pp_ce_instruction_composer(16,ARAD_PP_CE_HEADER_2,96,ARAD_PP_CE_IS_CE16);
#define ARAD_PP_CE_IPV4_SIP_HDR2_HEADER_15_0 arad_pp_ce_instruction_composer(16,ARAD_PP_CE_HEADER_2,112,ARAD_PP_CE_IS_CE16);
#define ARAD_PP_CE_IPV4_SIP_HDR2_HEADER arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_2,96,ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_IPV6_DIP_HDR2_HEADER_127_96 arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_2,(128+96),ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_IPV6_DIP_HDR2_HEADER_95_64 arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_2,(128+128),ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_IPV6_DIP_HDR2_HEADER_63_32 arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_2,(128+160),ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_IPV6_DIP_HDR2_HEADER_31_0 arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_2,(128+192),ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_IPV6_SIP_HDR2_HEADER_127_96 arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_2,64,ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_IPV6_SIP_HDR2_HEADER_95_64 arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_2,96,ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_IPV6_SIP_HDR2_HEADER_63_32 arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_2,128,ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_IPV6_SIP_HDR2_HEADER_31_0 arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_2,160,ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_IPV6_SIP_HDR2_HEADER_111_80 arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_2,80,ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_IPV6_SIP_HDR2_HEADER_127_112 arad_pp_ce_instruction_composer(16,ARAD_PP_CE_HEADER_2,64,ARAD_PP_CE_IS_CE16);
#define ARAD_PP_CE_IPV6_SIP_HDR2_HEADER_79_64 arad_pp_ce_instruction_composer(16,ARAD_PP_CE_HEADER_2,96,ARAD_PP_CE_IS_CE32);

#define ARAD_PP_CE_DIP_IPV4_HDR2_HEADER arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_2,128,ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_SIP_IPV4_HDR2_HEADER arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_2,96,ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_DIP_IPV4_HDR2_HEADER_27_0 arad_pp_ce_instruction_composer(28,ARAD_PP_CE_HEADER_2,128,ARAD_PP_CE_IS_CE32);

#define ARAD_PP_CE_DIP_IPV6_HDR2_HEADER_127_96  arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_2,(128+64),ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_DIP_IPV6_HDR2_HEADER_95_64   arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_2,(128+96),ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_DIP_IPV6_HDR2_HEADER_63_32   arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_2,(128+128),ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_DIP_IPV6_HDR2_HEADER_31_0    arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_2,(128+160),ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_DIP_IPV6_HDR2_HEADER_63_56_CB16   arad_pp_ce_instruction_composer(8,ARAD_PP_CE_HEADER_2,(128+120),ARAD_PP_CE_IS_CE16);
#define ARAD_PP_CE_DIP_IPV6_HDR2_HEADER_55_32   arad_pp_ce_instruction_composer(24,ARAD_PP_CE_HEADER_2,(128+128),ARAD_PP_CE_IS_CE32);

#define ARAD_PP_CE_IPV4_NEXT_PROTOCOL_HDR2_HEADER_CB16 arad_pp_ce_instruction_composer(8,ARAD_PP_CE_HEADER_2,64,ARAD_PP_CE_IS_CE16);
#define ARAD_PP_CE_IPV4_NEXT_PROTOCOL_HDR2_HEADER_CB32 arad_pp_ce_instruction_composer(8,ARAD_PP_CE_HEADER_2,48,ARAD_PP_CE_IS_CE32);
        
#define ARAD_PP_CE_UDP_HDR3_L4SP_L4DP_CB32 arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_3,0,ARAD_PP_CE_IS_CE32);
        
#define ARAD_PP_CE_MPLS_1_19_16   arad_pp_ce_instruction_composer(4,ARAD_PP_CE_HEADER_2,-12,ARAD_PP_CE_IS_CE16)
#define ARAD_PP_CE_MPLS_1_15_0    arad_pp_ce_instruction_composer(16,ARAD_PP_CE_HEADER_2,4,ARAD_PP_CE_IS_CE16)
#define ARAD_PP_CE_MPLS_1_BOS_16  arad_pp_ce_instruction_composer(1,ARAD_PP_CE_HEADER_2,8,ARAD_PP_CE_IS_CE16)
#define ARAD_PP_CE_MPLS_2_19_16   arad_pp_ce_instruction_composer(4,ARAD_PP_CE_HEADER_2,20,ARAD_PP_CE_IS_CE16)
#define ARAD_PP_CE_MPLS_2_15_0    arad_pp_ce_instruction_composer(16,ARAD_PP_CE_HEADER_2,36,ARAD_PP_CE_IS_CE16)
#define ARAD_PP_CE_MPLS_2_BOS_16  arad_pp_ce_instruction_composer(1,ARAD_PP_CE_HEADER_2,40,ARAD_PP_CE_IS_CE16)
#define ARAD_PP_CE_MPLS_3_19_16   arad_pp_ce_instruction_composer(4,ARAD_PP_CE_HEADER_2,52,ARAD_PP_CE_IS_CE16)
#define ARAD_PP_CE_MPLS_3_15_0    arad_pp_ce_instruction_composer(16,ARAD_PP_CE_HEADER_2,68,ARAD_PP_CE_IS_CE16)
#define ARAD_PP_CE_MPLS_3_BOS_16  arad_pp_ce_instruction_composer(1,ARAD_PP_CE_HEADER_2,72,ARAD_PP_CE_IS_CE16)
#define ARAD_PP_CE_MPLS_4_19_16   arad_pp_ce_instruction_composer(4,ARAD_PP_CE_HEADER_2,84,ARAD_PP_CE_IS_CE16)
#define ARAD_PP_CE_MPLS_4_15_0    arad_pp_ce_instruction_composer(16,ARAD_PP_CE_HEADER_2,100,ARAD_PP_CE_IS_CE16)
#define ARAD_PP_CE_MPLS_4_BOS_16  arad_pp_ce_instruction_composer(1,ARAD_PP_CE_HEADER_2,104,ARAD_PP_CE_IS_CE16)
#define ARAD_PP_CE_MPLS_1_31_0    arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_2,0,ARAD_PP_CE_IS_CE32)
#define ARAD_PP_CE_MPLS_2_31_0    arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_2,32,ARAD_PP_CE_IS_CE32)
#define ARAD_PP_CE_MPLS_3_31_0    arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_2,64,ARAD_PP_CE_IS_CE32)
#define ARAD_PP_CE_MPLS_4_31_0    arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_2,96,ARAD_PP_CE_IS_CE32)
#define ARAD_PP_CE_MPLS_1_19_0    arad_pp_ce_instruction_composer(20,ARAD_PP_CE_HEADER_2,0,ARAD_PP_CE_IS_CE32)
#define ARAD_PP_CE_MPLS_1_BOS_32  arad_pp_ce_instruction_composer(1,ARAD_PP_CE_HEADER_2,-8,ARAD_PP_CE_IS_CE32)
#define ARAD_PP_CE_MPLS_2_19_0    arad_pp_ce_instruction_composer(20,ARAD_PP_CE_HEADER_2,20,ARAD_PP_CE_IS_CE32)
#define ARAD_PP_CE_MPLS_2_BOS_32  arad_pp_ce_instruction_composer(1,ARAD_PP_CE_HEADER_2,24,ARAD_PP_CE_IS_CE32)
#define ARAD_PP_CE_MPLS_3_19_0    arad_pp_ce_instruction_composer(20,ARAD_PP_CE_HEADER_2,64,ARAD_PP_CE_IS_CE32)
#define ARAD_PP_CE_MPLS_3_BOS_32  arad_pp_ce_instruction_composer(1,ARAD_PP_CE_HEADER_2,56,ARAD_PP_CE_IS_CE32)
#define ARAD_PP_CE_MPLS_4_19_0    arad_pp_ce_instruction_composer(20,ARAD_PP_CE_HEADER_2,96,ARAD_PP_CE_IS_CE32)
#define ARAD_PP_CE_MPLS_4_BOS_32  arad_pp_ce_instruction_composer(1,ARAD_PP_CE_HEADER_2,88,ARAD_PP_CE_IS_CE32)
                                                                                             
#define ARAD_PP_CE_ETH_TYPE_32B   arad_pp_ce_instruction_composer(16,ARAD_PP_CE_HEADER_1,96,ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_ETH_TYPE_16B   arad_pp_ce_instruction_composer(16,ARAD_PP_CE_HEADER_1,96,ARAD_PP_CE_IS_CE16);

#define ARAD_PP_CE_MPLS_FWD_HEADER_19_16 arad_pp_ce_instruction_composer(4,ARAD_PP_CE_HEADER_FWD,4,ARAD_PP_CE_IS_CE16);
#define ARAD_PP_CE_MPLS_FWD_HEADER_15_0 arad_pp_ce_instruction_composer(16,ARAD_PP_CE_HEADER_FWD,0,ARAD_PP_CE_IS_CE16);
#define ARAD_PP_CE_MPLS_FWD_HEADER_BOS_BIT_23 arad_pp_ce_instruction_composer(1,ARAD_PP_CE_HEADER_FWD,8,ARAD_PP_CE_IS_CE16);
#define ARAD_PP_CE_MPLS_AFTER_FWD_HEADER_19_16 arad_pp_ce_instruction_composer(4,ARAD_PP_CE_HEADER_AFTER_FWD,4,ARAD_PP_CE_IS_CE16);
#define ARAD_PP_CE_MPLS_AFTER_FWD_HEADER_15_0 arad_pp_ce_instruction_composer(16,ARAD_PP_CE_HEADER_AFTER_FWD,0,ARAD_PP_CE_IS_CE16);

#define ARAD_PP_CE_DIP_FWD_HEADER arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_FWD,128,ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_DIP_FWD_HEADER_27_0 arad_pp_ce_instruction_composer(28,ARAD_PP_CE_HEADER_FWD,128,ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_SIP_FWD_HEADER arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_FWD,96,ARAD_PP_CE_IS_CE32);

#define ARAD_PP_CE_SIP_IPV6_FWD_HEADER_127_112 arad_pp_ce_instruction_composer(16,ARAD_PP_CE_HEADER_FWD,64,ARAD_PP_CE_IS_CE16);
#define ARAD_PP_CE_SIP_IPV6_FWD_HEADER_111_96  arad_pp_ce_instruction_composer(16,ARAD_PP_CE_HEADER_FWD,80,ARAD_PP_CE_IS_CE16);
#define ARAD_PP_CE_SIP_IPV6_FWD_HEADER_95_64   arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_FWD,96,ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_SIP_IPV6_FWD_HEADER_79_48   arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_FWD,112,ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_SIP_IPV6_FWD_HEADER_63_32   arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_FWD,128,ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_SIP_IPV6_FWD_HEADER_31_0    arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_FWD,160,ARAD_PP_CE_IS_CE32);

#define ARAD_PP_CE_SIP_IPV6_FWD_HEADER_119_112 arad_pp_ce_instruction_composer(8,ARAD_PP_CE_HEADER_FWD,64,ARAD_PP_CE_IS_CE16);
#define ARAD_PP_CE_SIP_IPV6_FWD_HEADER_127_96 arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_FWD,64, ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_SIP_IPV6_FWD_HEADER_95_80  arad_pp_ce_instruction_composer(16,ARAD_PP_CE_HEADER_FWD,96,ARAD_PP_CE_IS_CE16);
#define ARAD_PP_CE_SIP_IPV6_FWD_HEADER_63_48  arad_pp_ce_instruction_composer(16,ARAD_PP_CE_HEADER_FWD,128,ARAD_PP_CE_IS_CE16);
#define ARAD_PP_CE_SIP_IPV6_FWD_HEADER_47_32  arad_pp_ce_instruction_composer(16,ARAD_PP_CE_HEADER_FWD,144,ARAD_PP_CE_IS_CE16);

#define ARAD_PP_CE_DIP_IPV6_FWD_HEADER_127_112 arad_pp_ce_instruction_composer(16,ARAD_PP_CE_HEADER_FWD,128+64,ARAD_PP_CE_IS_CE16); 
#define ARAD_PP_CE_DIP_IPV6_FWD_HEADER_111_96  arad_pp_ce_instruction_composer(16,ARAD_PP_CE_HEADER_FWD,128+80,ARAD_PP_CE_IS_CE16); 
#define ARAD_PP_CE_DIP_IPV6_FWD_HEADER_95_64   arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_FWD,128+96,ARAD_PP_CE_IS_CE32); 
#define ARAD_PP_CE_DIP_IPV6_FWD_HEADER_63_32   arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_FWD,128+128,ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_DIP_IPV6_FWD_HEADER_31_0    arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_FWD,128+160,ARAD_PP_CE_IS_CE32);

#define ARAD_PP_CE_DIP_IPV6_FWD_HEADER_119_112 arad_pp_ce_instruction_composer(8,ARAD_PP_CE_HEADER_FWD,128+64,ARAD_PP_CE_IS_CE16); 

#define ARAD_PP_CE_ETH_HEADER_OUTER_TAG        arad_pp_ce_instruction_composer(12,ARAD_PP_CE_HEADER_1,112,ARAD_PP_CE_IS_CE16);
#define ARAD_PP_CE_ETH_HEADER_INNER_TAG        arad_pp_ce_instruction_composer(12,ARAD_PP_CE_HEADER_1,144,ARAD_PP_CE_IS_CE16);

#define ARAD_PP_CE_ETH_HEADER_OUTER_TAG_CE32   arad_pp_ce_instruction_composer(12,ARAD_PP_CE_HEADER_1,96,ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_ETH_HEADER_INNER_TAG_CE32   arad_pp_ce_instruction_composer(12,ARAD_PP_CE_HEADER_1,128,ARAD_PP_CE_IS_CE32);

#define ARAD_PP_CE_DA_32MSB                    arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_1,0,ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_DA_24MSB                    arad_pp_ce_instruction_composer(24,ARAD_PP_CE_HEADER_1,-8,ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_SA_24MSB                    arad_pp_ce_instruction_composer(24,ARAD_PP_CE_HEADER_1,40,ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_SA_24LSB                    arad_pp_ce_instruction_composer(24,ARAD_PP_CE_HEADER_1,64,ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_SA_8MSB_CB16                arad_pp_ce_instruction_composer(8,ARAD_PP_CE_HEADER_1,40,ARAD_PP_CE_IS_CE16)

#define ARAD_PP_CE_SA_FWD_HEADER_16_MSB_CB32 arad_pp_ce_instruction_composer(16,ARAD_PP_CE_HEADER_FWD,32,ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_SA_FWD_HEADER_7_MSB_CB16  arad_pp_ce_instruction_composer(7,ARAD_PP_CE_HEADER_FWD,48,ARAD_PP_CE_IS_CE16);
#define ARAD_PP_CE_SA_FWD_HEADER_40_32_CB16  arad_pp_ce_instruction_composer(9,ARAD_PP_CE_HEADER_FWD,48,ARAD_PP_CE_IS_CE16);
#define ARAD_PP_CE_SA_FWD_HEADER_16_MSB  arad_pp_ce_instruction_composer(16,ARAD_PP_CE_HEADER_FWD,48,ARAD_PP_CE_IS_CE16);
#define ARAD_PP_CE_SA_FWD_HEADER_32_LSB  arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_FWD,64,ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_DA_FWD_HEADER_16_MSB  arad_pp_ce_instruction_composer(16,ARAD_PP_CE_HEADER_FWD,0,ARAD_PP_CE_IS_CE16); 
#define ARAD_PP_CE_DA_FWD_HEADER_32_LSB  arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_FWD,16,ARAD_PP_CE_IS_CE32);


#define ARAD_PP_CE_ETH_HEADER_ISID arad_pp_ce_instruction_composer(24,ARAD_PP_CE_HEADER_2,-32,ARAD_PP_CE_IS_CE32);


#define ARAD_PP_CE_ETH_HEADER_ISID_16_LSB arad_pp_ce_instruction_composer(16,ARAD_PP_CE_HEADER_2,-32,ARAD_PP_CE_IS_CE32);

#define ARAD_PP_CE_ARP_SPA_HEADER_2 arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_2,112,ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_ARP_TPA_HEADER_2 arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_2,192,ARAD_PP_CE_IS_CE32);

#define ARAD_PP_CE_TRILL_EGRESS_NICK  arad_pp_ce_instruction_composer(16,ARAD_PP_CE_HEADER_2,16,ARAD_PP_CE_IS_CE16);
#define ARAD_PP_CE_TRILL_INGRESS_NICK arad_pp_ce_instruction_composer(16,ARAD_PP_CE_HEADER_2,32,ARAD_PP_CE_IS_CE16);
#define ARAD_PP_CE_NATIVE_INNER_VLAN_VSI arad_pp_ce_instruction_composer(12,ARAD_PP_CE_HEADER_3,144,ARAD_PP_CE_IS_CE16);
#define ARAD_PP_CE_TRILL_DIST_TREE_NICK_16  arad_pp_ce_instruction_composer(16,ARAD_PP_CE_HEADER_2,16,ARAD_PP_CE_IS_CE16);
#define ARAD_PP_CE_TRILL_DIST_TREE_NICK_32  arad_pp_ce_instruction_composer(16,ARAD_PP_CE_HEADER_2,0,ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_TRILL_NATIVE_SA_32MSB 0xf867
#define ARAD_PP_CE_TRILL_NATIVE_SA_16LSB 0x7887
#define ARAD_PP_CE_TRILL_NATIVE_DA_32MSB arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_3,0,ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_TRILL_NATIVE_DA_16LSB arad_pp_ce_instruction_composer(16,ARAD_PP_CE_HEADER_3,16,ARAD_PP_CE_IS_CE32);

#define ARAD_PP_CE_TRILL_DA_16MSB arad_pp_ce_instruction_composer(16,ARAD_PP_CE_HEADER_1,0,ARAD_PP_CE_IS_CE16);
#define ARAD_PP_CE_TRILL_DA_32LSB arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_1,16,ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_TRILL_SA_16MSB arad_pp_ce_instruction_composer(16,ARAD_PP_CE_HEADER_1,48,ARAD_PP_CE_IS_CE16);
#define ARAD_PP_CE_TRILL_SA_32LSB arad_pp_ce_instruction_composer(32,ARAD_PP_CE_HEADER_1,64,ARAD_PP_CE_IS_CE32);

#define ARAD_PP_CE_NATIVE_VLAN_VSI  arad_pp_ce_instruction_composer(12,ARAD_PP_CE_HEADER_3,112,ARAD_PP_CE_IS_CE16);
#define ARAD_PP_CE_GRE_8B_CRKS_BITS_CE16 arad_pp_ce_instruction_composer(4,ARAD_PP_CE_HEADER_2,148,ARAD_PP_CE_IS_CE16);
#define ARAD_PP_CE_GRE_8B_KEY_31_8_BITS_CE32 arad_pp_ce_instruction_composer(24,ARAD_PP_CE_HEADER_2,184,ARAD_PP_CE_IS_CE32);
#define ARAD_PP_CE_VXLAN_VNI_BITS_CE32 arad_pp_ce_instruction_composer(24,ARAD_PP_CE_HEADER_3,-40,ARAD_PP_CE_IS_CE32); 

#define ARAD_PP_CE_TRILL_NATIVE_OUTER_TAG_CE16 arad_pp_ce_instruction_composer(12,ARAD_PP_CE_HEADER_3,(14*8),ARAD_PP_CE_IS_CE16);
#define ARAD_PP_CE_TRILL_NATIVE_INNER_TAG_CE16 arad_pp_ce_instruction_composer(12,ARAD_PP_CE_HEADER_3,(18*8),ARAD_PP_CE_IS_CE16);

#define ARAD_PP_CE_YOUR_DESCRIMINATOR arad_pp_ce_instruction_composer((4*8),ARAD_PP_CE_HEADER_4,(8*8),ARAD_PP_CE_IS_CE32);




   









uint32 
   arad_pp_ce_instruction_composer (
     uint32 field_size_in_bits,
	   uint32 header_index,
	   uint32 offest_from_header_start_in_bits,
	   uint32 ce_16_or_32
	 );


#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

