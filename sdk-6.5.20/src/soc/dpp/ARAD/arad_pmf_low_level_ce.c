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

#define _ERR_MSG_MODULE_NAME BSL_SOC_FP



#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>

#include <soc/dcmn/error.h>
#include <soc/dpp/drv.h>
#include <soc/mem.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_framework.h>
#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/ARAD/arad_api_mgmt.h>
#include <soc/dpp/ARAD/arad_pmf_low_level_ce.h>
#include <soc/dpp/ARAD/arad_pmf_low_level_fem_tag.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_parser.h>

#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_reg_access.h>






#define ARAD_PMF_CE_32_BITS_INDEX_FIRST         (ARAD_PMF_LOW_LEVEL_NOF_CE_16_IN_PROG_GROUP)
#define ARAD_PMF_CE_16_BITS_INDEX_SECOND        (2 * ARAD_PMF_LOW_LEVEL_NOF_CE_16_IN_PROG_GROUP)
#define ARAD_PMF_CE_32_BITS_INDEX_SECOND        (3 * ARAD_PMF_LOW_LEVEL_NOF_CE_16_IN_PROG_GROUP)


#define ARAD_PMF_CE_TBL_2ND_KEY_GROUP_FIRST_LINE            (ARAD_PMF_LOW_LEVEL_PMF_PGM_NDX_MAX + 1)

#define ARAD_PMF_CE_TABLE_LENGTH_IN_UINT32S            (164/32 + 1)

#define ARAD_PMF_CE_TOTAL_BUFFER_LENGTH_IN_BITS    (1024)

#define ARAD_PMF_CE_BUFFER_SHIFT_IN_BITS        (4)

#define ARAD_PMF_CE_NOF_GROUP_TABLES    (4)

#define ARAD_PMF_CE_INTERNAL_TABLE_LINE_NOT_FOUND   (0xFFFFFFFF)


#define ARAD_PMF_KEY_PTCH2_OPAQUE_MSB   (0)
#define ARAD_PMF_KEY_PTCH2_OPAQUE_LSB   (3)
#define ARAD_PMF_KEY_ITMH_PPH_TYPE_MSB  (16)
#define ARAD_PMF_KEY_ITMH_PPH_TYPE_LSB  (19)


#define ARAD_PMF_KEY_BFD_PPH_FWD_CODE_MSB               (76)
#define ARAD_PMF_KEY_BFD_PPH_FWD_CODE_LSB               (79)
#define ARAD_PMF_KEY_BFD_1ST_NIBBLE_AFTER_LABEL_MSB     (160)
#define ARAD_PMF_KEY_BFD_1ST_NIBBLE_AFTER_LABEL_LSB     (163)


#define ARAD_PMF_KEY_OAM_ETHERTYPE_MSB          (144)
#define ARAD_PMF_KEY_OAM_ETHERTYPE_LSB          (159)
#define ARAD_PMF_KEY_OAM_2ND_ETHERTYPE_MSB      (176)
#define ARAD_PMF_KEY_OAM_2ND_ETHERTYPE_LSB      (191)


#define ARAD_PMF_KEY_VLAN_TAG_ID_MSB                   (4)
#define ARAD_PMF_KEY_VLAN_TAG_ID_LSB                   (15)
#define ARAD_PMF_KEY_VLAN_TAG_CFI_MSB                  (3)
#define ARAD_PMF_KEY_VLAN_TAG_CFI_LSB                  (3)
#define ARAD_PMF_KEY_VLAN_TAG_PRI_MSB                  (0)
#define ARAD_PMF_KEY_VLAN_TAG_PRI_LSB                  (2)


#define ARAD_PMF_KEY_DA_MSB                            (0)
#define ARAD_PMF_KEY_DA_LSB                            (47)
#define ARAD_PMF_KEY_SA_MSB                            (48)
#define ARAD_PMF_KEY_SA_LSB                            (95)
#define ARAD_PMF_KEY_VLAN_TAG_TPID_1ST_MSB             (96)
#define ARAD_PMF_KEY_VLAN_TAG_TPID_1ST_LSB             (111)
#define ARAD_PMF_KEY_VLAN_TAG_1ST_MSB                  (112)
#define ARAD_PMF_KEY_VLAN_TAG_1ST_LSB                  (127)
#define ARAD_PMF_KEY_VLAN_TAG_ID_1ST_MSB               (ARAD_PMF_KEY_VLAN_TAG_1ST_MSB + ARAD_PMF_KEY_VLAN_TAG_ID_MSB)
#define ARAD_PMF_KEY_VLAN_TAG_ID_1ST_LSB               (ARAD_PMF_KEY_VLAN_TAG_1ST_MSB + ARAD_PMF_KEY_VLAN_TAG_ID_LSB)
#define ARAD_PMF_KEY_VLAN_TAG_CFI_1ST_MSB              (ARAD_PMF_KEY_VLAN_TAG_1ST_MSB + ARAD_PMF_KEY_VLAN_TAG_CFI_MSB)
#define ARAD_PMF_KEY_VLAN_TAG_CFI_1ST_LSB              (ARAD_PMF_KEY_VLAN_TAG_1ST_MSB + ARAD_PMF_KEY_VLAN_TAG_CFI_LSB)
#define ARAD_PMF_KEY_VLAN_TAG_PRI_1ST_MSB              (ARAD_PMF_KEY_VLAN_TAG_1ST_MSB + ARAD_PMF_KEY_VLAN_TAG_PRI_MSB)
#define ARAD_PMF_KEY_VLAN_TAG_PRI_1ST_LSB              (ARAD_PMF_KEY_VLAN_TAG_1ST_MSB + ARAD_PMF_KEY_VLAN_TAG_PRI_LSB)
#define ARAD_PMF_KEY_VLAN_TAG_TPID_2ND_MSB             (128)
#define ARAD_PMF_KEY_VLAN_TAG_TPID_2ND_LSB             (143)
#define ARAD_PMF_KEY_VLAN_TAG_2ND_MSB                  (144)
#define ARAD_PMF_KEY_VLAN_TAG_2ND_LSB                  (159)
#define ARAD_PMF_KEY_VLAN_TAG_ID_2ND_MSB               (ARAD_PMF_KEY_VLAN_TAG_2ND_MSB + ARAD_PMF_KEY_VLAN_TAG_ID_MSB)
#define ARAD_PMF_KEY_VLAN_TAG_ID_2ND_LSB               (ARAD_PMF_KEY_VLAN_TAG_2ND_MSB + ARAD_PMF_KEY_VLAN_TAG_ID_LSB)
#define ARAD_PMF_KEY_VLAN_TAG_CFI_2ND_MSB              (ARAD_PMF_KEY_VLAN_TAG_2ND_MSB + ARAD_PMF_KEY_VLAN_TAG_CFI_MSB)
#define ARAD_PMF_KEY_VLAN_TAG_CFI_2ND_LSB              (ARAD_PMF_KEY_VLAN_TAG_2ND_MSB + ARAD_PMF_KEY_VLAN_TAG_CFI_LSB)
#define ARAD_PMF_KEY_VLAN_TAG_PRI_2ND_MSB              (ARAD_PMF_KEY_VLAN_TAG_2ND_MSB + ARAD_PMF_KEY_VLAN_TAG_PRI_MSB)
#define ARAD_PMF_KEY_VLAN_TAG_PRI_2ND_LSB              (ARAD_PMF_KEY_VLAN_TAG_2ND_MSB + ARAD_PMF_KEY_VLAN_TAG_PRI_LSB)
#define ARAD_PMF_KEY_ETHERTYPE_MSB                     (96) 
#define ARAD_PMF_KEY_ETHERTYPE_LSB                     (111)
#define ARAD_PMF_KEY_ONE_TAG_ETHERTYPE_MSB             (128) 
#define ARAD_PMF_KEY_ONE_TAG_ETHERTYPE_LSB             (143)
#define ARAD_PMF_KEY_DOUBLE_TAG_ETHERTYPE_MSB          (160) 
#define ARAD_PMF_KEY_DOUBLE_TAG_ETHERTYPE_LSB          (175)
#define ARAD_PMF_KEY_ETHERTYPE_H2_MSB                  (1008) 
#define ARAD_PMF_KEY_ETHERTYPE_H2_LSB                  (1023)
#define ARAD_PMF_KEY_PPPOE_SESSION_ID_H2_MSB           (-48) 
#define ARAD_PMF_KEY_PPPOE_SESSION_ID_H2_LSB           (-33)


#define ARAD_PMF_KEY_IPV4_NEXT_PRTCL_MSB                    (72)
#define ARAD_PMF_KEY_IPV4_NEXT_PRTCL_LSB                    (79)
#define ARAD_PMF_KEY_IPV4_FLAGS_MSB                         (48)
#define ARAD_PMF_KEY_IPV4_FLAGS_LSB                         (50)
#define ARAD_PMF_KEY_IPV4_DF_MSB                            (49)
#define ARAD_PMF_KEY_IPV4_DF_LSB                            (49)
#define ARAD_PMF_KEY_IPV4_MF_MSB                            (50)
#define ARAD_PMF_KEY_IPV4_MF_LSB                            (50)
#define ARAD_PMF_KEY_IPV4_SIP_MSB                           (96)
#define ARAD_PMF_KEY_IPV4_SIP_LSB                           (127)
#define ARAD_PMF_KEY_IPV4_DIP_MSB                           (128)
#define ARAD_PMF_KEY_IPV4_DIP_LSB                           (159)
#define ARAD_PMF_KEY_IPV4_TOS_MSB                           (8)
#define ARAD_PMF_KEY_IPV4_TOS_LSB                           (15)
#define ARAD_PMF_KEY_IPV4_TTL_MSB                           (64)
#define ARAD_PMF_KEY_IPV4_TTL_LSB                           (71)
#define ARAD_PMF_KEY_IPV4_TCP_CTL_MSB                       (106)
#define ARAD_PMF_KEY_IPV4_TCP_CTL_LSB                       (111)

#define ARAD_PMF_KEY_IPV4_SRC_PORT_MSB                      (0) 
#define ARAD_PMF_KEY_IPV4_SRC_PORT_LSB                      (15)
#define ARAD_PMF_KEY_IPV4_DEST_PORT_MSB                     (16)
#define ARAD_PMF_KEY_IPV4_DEST_PORT_LSB                     (31)


#define ARAD_PMF_KEY_GRE_CRKS_MSB                      (160)
#define ARAD_PMF_KEY_GRE_CRKS_LSB                      (163)
#define ARAD_PMF_KEY_GRE_PROTOCOL_TYPE_MSB             (176)
#define ARAD_PMF_KEY_GRE_PROTOCOL_TYPE_LSB             (191)
#define ARAD_PMF_KEY_GRE_KEY_MSB                       (192)
#define ARAD_PMF_KEY_GRE_KEY_LSB                       (215)


#define ARAD_PMF_QUAL_HDR_IPV6_TC_MSB                           (4)
#define ARAD_PMF_QUAL_HDR_IPV6_TC_LSB                           (11)
#define ARAD_PMF_QUAL_HDR_IPV6_FLOW_LABEL_MSB                   (12)
#define ARAD_PMF_QUAL_HDR_IPV6_FLOW_LABEL_LSB                   (31)
#define ARAD_PMF_QUAL_HDR_IPV6_HOP_LIMIT_MSB                    (56)
#define ARAD_PMF_QUAL_HDR_IPV6_HOP_LIMIT_LSB                    (63)
#define ARAD_PMF_QUAL_HDR_IPV6_SIP_HIGH_MSB                     (64)
#define ARAD_PMF_QUAL_HDR_IPV6_SIP_HIGH_LSB                     (127)
#define ARAD_PMF_QUAL_HDR_IPV6_SIP_LOW_MSB                      (128)
#define ARAD_PMF_QUAL_HDR_IPV6_SIP_LOW_LSB                      (191)
#define ARAD_PMF_QUAL_HDR_IPV6_DIP_HIGH_MSB                     (192)
#define ARAD_PMF_QUAL_HDR_IPV6_DIP_HIGH_LSB                     (255)
#define ARAD_PMF_QUAL_HDR_IPV6_DIP_LOW_MSB                      (256)
#define ARAD_PMF_QUAL_HDR_IPV6_DIP_LOW_LSB                      (319)
#define ARAD_PMF_QUAL_HDR_IPV6_NEXT_PRTCL_MSB                   (48)
#define ARAD_PMF_QUAL_HDR_IPV6_NEXT_PRTCL_LSB                   (55)
#define ARAD_PMF_QUAL_HDR_IPV6_TCP_CTL_MSB                      (ARAD_PMF_KEY_IPV4_TCP_CTL_MSB)
#define ARAD_PMF_QUAL_HDR_IPV6_TCP_CTL_LSB                      (ARAD_PMF_KEY_IPV4_TCP_CTL_LSB)
#define ARAD_PMF_QUAL_HDR_IPV6_EXT_NEXT_HDR_MSB                 (320)
#define ARAD_PMF_QUAL_HDR_IPV6_EXT_NEXT_HDR_LSB                 (327)


#define ARAD_PMF_KEY_MPLS_LABEL_MSB                          (0)
#define ARAD_PMF_KEY_MPLS_LABEL_LSB                          (31)
#define ARAD_PMF_KEY_MPLS_LABEL_ID_MSB                       (0)
#define ARAD_PMF_KEY_MPLS_LABEL_ID_LSB                       (19)
#define ARAD_PMF_KEY_MPLS_EXP_MSB                            (20)
#define ARAD_PMF_KEY_MPLS_EXP_LSB                            (22)
#define ARAD_PMF_KEY_MPLS_TTL_MSB                            (24)
#define ARAD_PMF_KEY_MPLS_TTL_LSB                            (31)
#define ARAD_PMF_KEY_MPLS_BOS_MSB                            (23)
#define ARAD_PMF_KEY_MPLS_BOS_LSB                            (23)

#define ARAD_PMF_KEY_MPLS_LABEL_SIZE_IN_BITS                 (32)
#define ARAD_PMF_KEY_MPLS_2_OFFSET                           (ARAD_PMF_KEY_MPLS_LABEL_SIZE_IN_BITS)
#define ARAD_PMF_KEY_MPLS_3_OFFSET                           (ARAD_PMF_KEY_MPLS_2_OFFSET + ARAD_PMF_KEY_MPLS_LABEL_SIZE_IN_BITS)
#define ARAD_PMF_KEY_MPLS_4_OFFSET                           (ARAD_PMF_KEY_MPLS_3_OFFSET + ARAD_PMF_KEY_MPLS_LABEL_SIZE_IN_BITS)
#define ARAD_PMF_KEY_MPLS_5_OFFSET                           (ARAD_PMF_KEY_MPLS_4_OFFSET + ARAD_PMF_KEY_MPLS_LABEL_SIZE_IN_BITS)

#define	ARAD_PMF_QUAL_HDR_HIGIG_FRC_MSB                      (0)
#define	ARAD_PMF_QUAL_HDR_HIGIG_FRC_LSB                      (63)
#define	ARAD_PMF_QUAL_HDR_HIGIG_PPD_EXT_MSB                  (128)
#define	ARAD_PMF_QUAL_HDR_HIGIG_PPD_EXT_LSB                  (191)
#define	ARAD_PMF_QUAL_HDR_HIGIG_PPD_MSB                      (64)
#define	ARAD_PMF_QUAL_HDR_HIGIG_PPD_LSB                      (127)

#define    ARAD_PMF_QUAL_HDR_PTCH_RESERVE_LSB_MSB               (7)
#define    ARAD_PMF_QUAL_HDR_PTCH_RESERVE_LSB_LSB               (7)

#define    ARAD_PMF_QUAL_HDR_MH_FLOW_MSB						(8)
#define    ARAD_PMF_QUAL_HDR_MH_FLOW_LSB						(23)
#define    ARAD_PMF_QUAL_HDR_MH_TC2_MSB 						(24)
#define    ARAD_PMF_QUAL_HDR_MH_TC2_LSB 						(31)
#define    ARAD_PMF_QUAL_HDR_MH_DP0_MSB 						(40)
#define    ARAD_PMF_QUAL_HDR_MH_DP0_LSB 						(47)
#define    ARAD_PMF_QUAL_HDR_MH_CAST_MSB						(56)
#define    ARAD_PMF_QUAL_HDR_MH_CAST_LSB						(63)
#define    ARAD_PMF_QUAL_HDR_MH_DP1_MSB 						(152)
#define    ARAD_PMF_QUAL_HDR_MH_DP1_LSB 						(159)
#define    ARAD_PMF_QUAL_HDR_MH_TC10_MSB						(160)
#define    ARAD_PMF_QUAL_HDR_MH_TC10_LSB						(167)


#define    ARAD_PMF_QUAL_HDR_INPHEADER_UC_MSB						(16)
#define    ARAD_PMF_QUAL_HDR_INPHEADER_UC_LSB						(16)
#define    ARAD_PMF_QUAL_HDR_INPHEADER_TB_MSB						(8)
#define    ARAD_PMF_QUAL_HDR_INPHEADER_TB_LSB						(15)
#define    ARAD_PMF_QUAL_HDR_INPHEADER_UC_TC_MSB				    (25)
#define    ARAD_PMF_QUAL_HDR_INPHEADER_UC_TC_LSB					(27)
#define    ARAD_PMF_QUAL_HDR_INPHEADER_MC_TC_MSB					(69)
#define    ARAD_PMF_QUAL_HDR_INPHEADER_MC_TC_LSB					(71)
#define    ARAD_PMF_QUAL_HDR_INPHEADER_DP_MSB						(19)
#define    ARAD_PMF_QUAL_HDR_INPHEADER_DP_LSB						(20)

#define	ARAD_PMF_QUAL_EID_MSB                      (157)
#define	ARAD_PMF_QUAL_EID_LSB                      (159)

#define ARAD_PMF_QUAL_HDR_TUNNEL_ID_MSB                  (1013) 
#define ARAD_PMF_QUAL_HDR_TUNNEL_ID_LSB                  (1023)


#define ARAD_PMF_QUAL_HDR_ARP_SENDER_MSB                 (112) 
#define ARAD_PMF_QUAL_HDR_ARP_SENDER_LSB                 (143)
#define ARAD_PMF_QUAL_HDR_ARP_TARGET_MSB                 (192) 
#define ARAD_PMF_QUAL_HDR_ARP_TARGET_LSB                 (223)
#define ARAD_PMF_QUAL_HDR_ARP_OPCODE_MSB                 (48) 
#define ARAD_PMF_QUAL_HDR_ARP_OPCODE_LSB                 (63)


#define ARAD_PMF_QUAL_ETH_OAM_HEADER_BITS_0_31_MSB        (0)
#define ARAD_PMF_QUAL_ETH_OAM_HEADER_BITS_0_31_LSB        (31)
#define ARAD_PMF_QUAL_ETH_OAM_HEADER_BITS_32_63_MSB       (32)
#define ARAD_PMF_QUAL_ETH_OAM_HEADER_BITS_32_63_LSB       (63)
#define ARAD_PMF_QUAL_MPLS_OAM_HEADER_BITS_0_31_MSB       (0)
#define ARAD_PMF_QUAL_MPLS_OAM_HEADER_BITS_0_32_LSB       (31)
#define ARAD_PMF_QUAL_MPLS_OAM_HEADER_BITS_32_63_MSB      (32)
#define ARAD_PMF_QUAL_MPLS_OAM_HEADER_BITS_32_63_LSB      (63)
#define ARAD_PMF_QUAL_OAM_HEADER_BITS_0_31_MSB            (0)
#define ARAD_PMF_QUAL_OAM_HEADER_BITS_0_31_LSB            (31)
#define ARAD_PMF_QUAL_OAM_HEADER_BITS_31_63_MSB           (32)
#define ARAD_PMF_QUAL_OAM_HEADER_BITS_31_63_LSB           (63)
#define ARAD_PMF_QUAL_QUAL_MPLS_OAM_ACH_MSB               (-32)
#define ARAD_PMF_QUAL_QUAL_MPLS_OAM_ACH_LSB               (-1)
#define ARAD_PMF_QUAL_OAM_OPCODE_MSB                      (8)
#define ARAD_PMF_QUAL_OAM_OPCODE_LSB                      (15)
#define ARAD_PMF_QUAL_OAM_MD_LEVEL_UNTAGGED_MSB           (112)
#define ARAD_PMF_QUAL_OAM_MD_LEVEL_UNTAGGED_LSB           (115)
#define ARAD_PMF_QUAL_OAM_MD_LEVEL_SINGLE_TAG_MSB         (144)
#define ARAD_PMF_QUAL_OAM_MD_LEVEL_SINGLE_TAG_LSB         (147)
#define ARAD_PMF_QUAL_OAM_MD_LEVEL_DOUBLE_TAG_MSB         (176)
#define ARAD_PMF_QUAL_OAM_MD_LEVEL_DOUBLE_TAG_LSB         (179)
#define ARAD_PMF_QUAL_OAM_MD_LEVEL_MSB                    (0)
#define ARAD_PMF_QUAL_OAM_MD_LEVEL_LSB                    (3)
#define ARAD_PMF_QUAL_TM_OUTER_TAG_MSB                (112)
#define ARAD_PMF_QUAL_TM_OUTER_TAG_LSB                (127)
#define ARAD_PMF_QUAL_TM_INNER_TAG_MSB                (144)
#define ARAD_PMF_QUAL_TM_INNER_TAG_LSB                (159)

#define ARAD_PMF_QUAL_MY_DISCR_IPV4_MSB                (312)
#define ARAD_PMF_QUAL_MY_DISCR_IPV4_LSB                (343)
#define ARAD_PMF_QUAL_MY_DISCR_MPLS_MSB                (344)
#define ARAD_PMF_QUAL_MY_DISCR_MPLS_LSB                (375)
#define ARAD_PMF_QUAL_MY_DISCR_PWE_MSB                (152)
#define ARAD_PMF_QUAL_MY_DISCR_PWE_LSB                (183)


#define ARAD_PMF_QUAL_HDR_TRILL_EGRESS_NICK_MSB  (16)
#define ARAD_PMF_QUAL_HDR_TRILL_EGRESS_NICK_LSB  (31)
#define ARAD_PMF_QUAL_HDR_TRILL_INGRESS_NICK_MSB (32)
#define ARAD_PMF_QUAL_HDR_TRILL_INGRESS_NICK_LSB (47)

#define ARAD_PMF_QUAL_ETH_HEADER_ISID_MSB        (-24)
#define ARAD_PMF_QUAL_ETH_HEADER_ISID_LSB        (-1)

#define ARAD_PMF_QUAL_VXLAN_VNI_MSB              (-32)
#define ARAD_PMF_QUAL_VXLAN_VNI_LSB              (-9)



#define ARAD_PMF_QUAL_PORT_EXTENDER_ETAG_MSB     (-64)
#define ARAD_PMF_QUAL_PORT_EXTENDER_ETAG_LSB     (-49)
 
#define ARAD_PMF_QUAL_PORT_EXTENDER_ECID_MSB     (-28)
#define ARAD_PMF_QUAL_PORT_EXTENDER_ECID_LSB     (-17)


#define ARAD_PMF_KEY_FC_D_ID_MSB             (8)
#define ARAD_PMF_KEY_FC_D_ID_LSB             (15)
#define ARAD_PMF_KEY_FC_WITH_VFT_VFT_ID_MSB  (19)
#define ARAD_PMF_KEY_FC_WITH_VFT_VFT_ID_LSB  (31)


#define	ARAD_PMF_QUAL_L2TP_TUNNEL_MSB		(-64) 
#define ARAD_PMF_QUAL_L2TP_TUNNEL_LSB		(-49) 
#define ARAD_PMF_QUAL_L2TP_SESSION_MSB		(-48) 
#define	ARAD_PMF_QUAL_L2TP_SESSION_LSB		(-33) 

#define ARAD_PMF_EGQ_SYSTEM_HEADERS_FORMAT_CODE_NUM_OF_PROFILES   (128 )


#define ARAD_PMF_QUAL_IPV6_COMPRESSED_SIP_MSB        (0) 
#define ARAD_PMF_QUAL_IPV6_COMPRESSED_SIP_LSB        (15) 










typedef struct
{
  uint32 ftmh_present;
  uint32 dsp_ext_present;
  uint32 pph_base;
  uint32 fhei_size; 
  uint32 eei_present;
  uint32 learn_ext_present;
}ARAD_PMF_LOW_LEVEL_HEADER_FORMAT_PROFILE_USUAL;






static 
    uint32
        Arad_pmf_ce_sub_header_type_encoding[ARAD_NOF_PMF_CE_SUB_HEADERS][2] = {
            {ARAD_PMF_CE_SUB_HEADER_0, 0},
            {ARAD_PMF_CE_SUB_HEADER_1, 1},
            {ARAD_PMF_CE_SUB_HEADER_2, 2},
            {ARAD_PMF_CE_SUB_HEADER_3, 3},
            {ARAD_PMF_CE_SUB_HEADER_4, 4},
            {ARAD_PMF_CE_SUB_HEADER_5, 5},
            {ARAD_PMF_CE_SUB_HEADER_FWD, 6},
            {ARAD_PMF_CE_SUB_HEADER_FWD_POST, 7}
        };



 static const 
    ARAD_PMF_CE_HEADER_QUAL_INFO
      Arad_pmf_ce_header_info[] =
 {
   {SOC_PPC_FP_QUAL_HDR_ITMH,                                                           0,                 31, ARAD_PMF_CE_SUB_HEADER_1,  ARAD_PMF_CE_SUB_HEADER_1},
   {SOC_PPC_FP_QUAL_HDR_ITMH_EXT,                                                      (32),  (32+23), ARAD_PMF_CE_SUB_HEADER_1,  ARAD_PMF_CE_SUB_HEADER_1},
    
   {SOC_PPC_FP_QUAL_HDR_ITMH_DEST_FWD,                                                 -20,           -1, ARAD_PMF_CE_SUB_HEADER_FWD_POST,  ARAD_PMF_CE_SUB_HEADER_FWD_POST}, 
#if (0)
   
   {SOC_PPC_FP_QUAL_HDR_ITMH_DEST_FWD,                                                 -20-8,           -1-8, ARAD_PMF_CE_SUB_HEADER_FWD_POST,  ARAD_PMF_CE_SUB_HEADER_FWD_POST}, 
#endif
   {SOC_PPC_FP_QUAL_HDR_ITMH_PMF_HDR_EXT,                                               0,           159, ARAD_PMF_CE_SUB_HEADER_FWD_POST,  ARAD_PMF_CE_SUB_HEADER_FWD_POST}, 
   {SOC_PPC_FP_QUAL_HDR_PTCH2_OPAQUE,                   ARAD_PMF_KEY_PTCH2_OPAQUE_MSB,                  ARAD_PMF_KEY_PTCH2_OPAQUE_LSB,          ARAD_PMF_CE_SUB_HEADER_0,   0},
   {SOC_PPC_FP_QUAL_HDR_ITMH_PPH_TYPE,                  ARAD_PMF_KEY_ITMH_PPH_TYPE_MSB,                 ARAD_PMF_KEY_ITMH_PPH_TYPE_LSB,         ARAD_PMF_CE_SUB_HEADER_0,   0},
   {SOC_PPC_FP_QUAL_HDR_OAM_ETHERTYPE,                  ARAD_PMF_KEY_OAM_ETHERTYPE_MSB,                 ARAD_PMF_KEY_OAM_ETHERTYPE_LSB,         ARAD_PMF_CE_SUB_HEADER_0,   0},
   {SOC_PPC_FP_QUAL_HDR_OAM_2ND_ETHERTYPE,              ARAD_PMF_KEY_OAM_2ND_ETHERTYPE_MSB,             ARAD_PMF_KEY_OAM_2ND_ETHERTYPE_LSB,     ARAD_PMF_CE_SUB_HEADER_0,   0},
   {SOC_PPC_FP_QUAL_HDR_BFD_PPH_FWD_CODE,               ARAD_PMF_KEY_BFD_PPH_FWD_CODE_MSB,              ARAD_PMF_KEY_BFD_PPH_FWD_CODE_LSB,      ARAD_PMF_CE_SUB_HEADER_0,   0},
   {SOC_PPC_FP_QUAL_HDR_BFD_1ST_NIBBLE_AFTER_LABEL,     ARAD_PMF_KEY_BFD_1ST_NIBBLE_AFTER_LABEL_MSB,    ARAD_PMF_KEY_BFD_1ST_NIBBLE_AFTER_LABEL_LSB,             ARAD_PMF_CE_SUB_HEADER_0,   0},
   {SOC_PPC_FP_QUAL_HDR_FTMH,                                                           0,                 71,  ARAD_PMF_CE_SUB_HEADER_1,  ARAD_PMF_CE_SUB_HEADER_1},
   {SOC_PPC_FP_QUAL_HDR_FTMH_LB_KEY_EXT_AFTER_FTMH, 72 ,                                79,                     ARAD_PMF_CE_SUB_HEADER_1,  ARAD_PMF_CE_SUB_HEADER_1},
   {SOC_PPC_FP_QUAL_HDR_FTMH_LB_KEY_START_OF_PACKET, 0,                                 7,                     ARAD_PMF_CE_SUB_HEADER_0,  ARAD_PMF_CE_SUB_HEADER_0},
   {SOC_PPC_FP_QUAL_HDR_DSP_EXTENSION_AFTER_FTMH,   72 , 87 ,  ARAD_PMF_CE_SUB_HEADER_1,  ARAD_PMF_CE_SUB_HEADER_1},
   {SOC_PPC_FP_QUAL_HDR_STACKING_EXT_AFTER_DSP_EXT,                                     88,                103, ARAD_PMF_CE_SUB_HEADER_1,  ARAD_PMF_CE_SUB_HEADER_1},
   {SOC_PPC_FP_QUAL_HDR_STACKING_EXT_AFTER_DSP_EXT_PETRA,                               64,                79, ARAD_PMF_CE_SUB_HEADER_1,  ARAD_PMF_CE_SUB_HEADER_1},
   {SOC_PPC_FP_QUAL_HDR_INNER_VLAN_TAG,          ARAD_PMF_KEY_VLAN_TAG_1ST_MSB, ARAD_PMF_KEY_VLAN_TAG_1ST_LSB,                                         ARAD_PMF_CE_SUB_HEADER_2,  ARAD_PMF_CE_SUB_HEADER_3},
   {SOC_PPC_FP_QUAL_HDR_INNER_VLAN_TAG_ID,       ARAD_PMF_KEY_VLAN_TAG_ID_1ST_MSB,ARAD_PMF_KEY_VLAN_TAG_ID_1ST_LSB,                                    ARAD_PMF_CE_SUB_HEADER_2,  ARAD_PMF_CE_SUB_HEADER_3},
   {SOC_PPC_FP_QUAL_HDR_INNER_VLAN_TAG_CFI,      ARAD_PMF_KEY_VLAN_TAG_CFI_1ST_MSB,ARAD_PMF_KEY_VLAN_TAG_CFI_1ST_LSB,                                  ARAD_PMF_CE_SUB_HEADER_2,  ARAD_PMF_CE_SUB_HEADER_3},
   {SOC_PPC_FP_QUAL_HDR_INNER_VLAN_TAG_PRI,      ARAD_PMF_KEY_VLAN_TAG_PRI_1ST_MSB,ARAD_PMF_KEY_VLAN_TAG_PRI_1ST_LSB,                                  ARAD_PMF_CE_SUB_HEADER_2,  ARAD_PMF_CE_SUB_HEADER_3},
   {SOC_PPC_FP_QUAL_HDR_INNER_SA,                ARAD_PMF_KEY_SA_MSB, ARAD_PMF_KEY_SA_LSB,                                                             ARAD_PMF_CE_SUB_HEADER_2,  ARAD_PMF_CE_SUB_HEADER_3},
   {SOC_PPC_FP_QUAL_HDR_INNER_DA,                ARAD_PMF_KEY_DA_MSB,  ARAD_PMF_KEY_DA_LSB,                                                            ARAD_PMF_CE_SUB_HEADER_2,  ARAD_PMF_CE_SUB_HEADER_3},
   {SOC_PPC_FP_QUAL_HDR_INNER_ETHERTYPE,         ARAD_PMF_KEY_ETHERTYPE_H2_MSB, ARAD_PMF_KEY_ETHERTYPE_H2_LSB,                          ARAD_PMF_CE_SUB_HEADER_3,  ARAD_PMF_CE_SUB_HEADER_4},
   {SOC_PPC_FP_QUAL_HDR_INNER_2ND_VLAN_TAG,      ARAD_PMF_KEY_VLAN_TAG_2ND_MSB,ARAD_PMF_KEY_VLAN_TAG_2ND_LSB,                                          ARAD_PMF_CE_SUB_HEADER_2,  ARAD_PMF_CE_SUB_HEADER_3},
   {SOC_PPC_FP_QUAL_HDR_INNER_2ND_VLAN_TAG_ID,   ARAD_PMF_KEY_VLAN_TAG_ID_2ND_MSB,ARAD_PMF_KEY_VLAN_TAG_ID_2ND_LSB,                                          ARAD_PMF_CE_SUB_HEADER_2,  ARAD_PMF_CE_SUB_HEADER_3},
   {SOC_PPC_FP_QUAL_HDR_INNER_2ND_VLAN_TAG_CFI,  ARAD_PMF_KEY_VLAN_TAG_CFI_2ND_MSB,ARAD_PMF_KEY_VLAN_TAG_CFI_2ND_LSB,                                          ARAD_PMF_CE_SUB_HEADER_2,  ARAD_PMF_CE_SUB_HEADER_3},
   {SOC_PPC_FP_QUAL_HDR_INNER_2ND_VLAN_TAG_PRI,  ARAD_PMF_KEY_VLAN_TAG_PRI_2ND_MSB,ARAD_PMF_KEY_VLAN_TAG_PRI_2ND_LSB,                                          ARAD_PMF_CE_SUB_HEADER_2,  ARAD_PMF_CE_SUB_HEADER_3},
   {SOC_PPC_FP_QUAL_HDR_VLAN_TAG,                ARAD_PMF_KEY_VLAN_TAG_1ST_MSB,ARAD_PMF_KEY_VLAN_TAG_1ST_LSB,                                          ARAD_PMF_CE_SUB_HEADER_1,                       0},
   {SOC_PPC_FP_QUAL_HDR_VLAN_TAG_ID,             ARAD_PMF_KEY_VLAN_TAG_ID_1ST_MSB,ARAD_PMF_KEY_VLAN_TAG_ID_1ST_LSB,                                    ARAD_PMF_CE_SUB_HEADER_1,                       0},
   {SOC_PPC_FP_QUAL_HDR_VLAN_TAG_CFI,            ARAD_PMF_KEY_VLAN_TAG_CFI_1ST_MSB,ARAD_PMF_KEY_VLAN_TAG_CFI_1ST_LSB,                                  ARAD_PMF_CE_SUB_HEADER_1,                       0},
   {SOC_PPC_FP_QUAL_HDR_VLAN_TAG_PRI,            ARAD_PMF_KEY_VLAN_TAG_PRI_1ST_MSB,ARAD_PMF_KEY_VLAN_TAG_PRI_1ST_LSB,                                  ARAD_PMF_CE_SUB_HEADER_1,                       0},
   {SOC_PPC_FP_QUAL_HDR_VLAN_TAG_PRI_CFI,        ARAD_PMF_KEY_VLAN_TAG_PRI_1ST_MSB,ARAD_PMF_KEY_VLAN_TAG_CFI_1ST_LSB,                                  ARAD_PMF_CE_SUB_HEADER_1,                       0},
   {SOC_PPC_FP_QUAL_HDR_VLAN_TAG_TPID,           ARAD_PMF_KEY_VLAN_TAG_TPID_1ST_MSB,ARAD_PMF_KEY_VLAN_TAG_TPID_1ST_LSB,                                ARAD_PMF_CE_SUB_HEADER_1,                       0},
   {SOC_PPC_FP_QUAL_HDR_SA,                      ARAD_PMF_KEY_SA_MSB,ARAD_PMF_KEY_SA_LSB,                                                              ARAD_PMF_CE_SUB_HEADER_1,                       0},
   {SOC_PPC_FP_QUAL_HDR_DA,                      ARAD_PMF_KEY_DA_MSB, ARAD_PMF_KEY_DA_LSB,                                                             ARAD_PMF_CE_SUB_HEADER_1,                       0},
   {SOC_PPC_FP_QUAL_HDR_ETHERTYPE,               ARAD_PMF_KEY_ETHERTYPE_H2_MSB, ARAD_PMF_KEY_ETHERTYPE_H2_LSB,                                         ARAD_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PPC_FP_QUAL_HDR_PPPOE_SESSION_ID,        ARAD_PMF_KEY_PPPOE_SESSION_ID_H2_MSB, ARAD_PMF_KEY_PPPOE_SESSION_ID_H2_LSB,                           ARAD_PMF_CE_SUB_HEADER_2,                       0},   
   {SOC_PPC_FP_QUAL_HDR_2ND_VLAN_TAG,            ARAD_PMF_KEY_VLAN_TAG_2ND_MSB,ARAD_PMF_KEY_VLAN_TAG_2ND_LSB,                                          ARAD_PMF_CE_SUB_HEADER_1,                       0},
   {SOC_PPC_FP_QUAL_HDR_2ND_VLAN_TAG_ID,         ARAD_PMF_KEY_VLAN_TAG_ID_2ND_MSB,ARAD_PMF_KEY_VLAN_TAG_ID_2ND_LSB,                                    ARAD_PMF_CE_SUB_HEADER_1,                       0},
   {SOC_PPC_FP_QUAL_HDR_2ND_VLAN_TAG_CFI,        ARAD_PMF_KEY_VLAN_TAG_CFI_2ND_MSB,ARAD_PMF_KEY_VLAN_TAG_CFI_2ND_LSB,                                  ARAD_PMF_CE_SUB_HEADER_1,                       0},
   {SOC_PPC_FP_QUAL_HDR_2ND_VLAN_TAG_PRI,        ARAD_PMF_KEY_VLAN_TAG_PRI_2ND_MSB,ARAD_PMF_KEY_VLAN_TAG_PRI_2ND_LSB,                                  ARAD_PMF_CE_SUB_HEADER_1,                       0},
   {SOC_PPC_FP_QUAL_HDR_2ND_VLAN_TAG_TPID,       ARAD_PMF_KEY_VLAN_TAG_TPID_2ND_MSB,ARAD_PMF_KEY_VLAN_TAG_TPID_2ND_LSB,                                ARAD_PMF_CE_SUB_HEADER_1,                       0},
   {SOC_PPC_FP_QUAL_UNTAG_HDR_ETHERTYPE,         ARAD_PMF_KEY_ETHERTYPE_MSB           , ARAD_PMF_KEY_ETHERTYPE_LSB,                                    ARAD_PMF_CE_SUB_HEADER_1,                       0},
   {SOC_PPC_FP_QUAL_ONE_TAG_HDR_ETHERTYPE,       ARAD_PMF_KEY_ONE_TAG_ETHERTYPE_MSB   , ARAD_PMF_KEY_ONE_TAG_ETHERTYPE_LSB,                            ARAD_PMF_CE_SUB_HEADER_1,                       0},
   {SOC_PPC_FP_QUAL_DOUBLE_TAG_HDR_ETHERTYPE,    ARAD_PMF_KEY_DOUBLE_TAG_ETHERTYPE_MSB, ARAD_PMF_KEY_DOUBLE_TAG_ETHERTYPE_LSB,                         ARAD_PMF_CE_SUB_HEADER_1,                       0},
   {SOC_PPC_FP_QUAL_HDR_INNER_IPV4_NEXT_PRTCL,   ARAD_PMF_KEY_IPV4_NEXT_PRTCL_MSB,  ARAD_PMF_KEY_IPV4_NEXT_PRTCL_LSB,                                  ARAD_PMF_CE_SUB_HEADER_3,                       0},
   {SOC_PPC_FP_QUAL_HDR_INNER_IPV4_DF,           ARAD_PMF_KEY_IPV4_DF_MSB, ARAD_PMF_KEY_IPV4_DF_LSB,                                                   ARAD_PMF_CE_SUB_HEADER_3,                       0},
   {SOC_PPC_FP_QUAL_HDR_INNER_IPV4_MF,           ARAD_PMF_KEY_IPV4_MF_MSB,  ARAD_PMF_KEY_IPV4_MF_LSB,                                                  ARAD_PMF_CE_SUB_HEADER_3,                       0},
   {SOC_PPC_FP_QUAL_HDR_INNER_IPV4_SIP,          ARAD_PMF_KEY_IPV4_SIP_MSB, ARAD_PMF_KEY_IPV4_SIP_LSB,                                                 ARAD_PMF_CE_SUB_HEADER_3,                       0},
   {SOC_PPC_FP_QUAL_HDR_INNER_IPV4_DIP,          ARAD_PMF_KEY_IPV4_DIP_MSB, ARAD_PMF_KEY_IPV4_DIP_LSB,                                                 ARAD_PMF_CE_SUB_HEADER_3,                       0},
   {SOC_PPC_FP_QUAL_HDR_INNER_IPV4_SRC_PORT,     ARAD_PMF_KEY_IPV4_SRC_PORT_MSB, ARAD_PMF_KEY_IPV4_SRC_PORT_LSB,                                       ARAD_PMF_CE_SUB_HEADER_4,                       0},
   {SOC_PPC_FP_QUAL_HDR_INNER_IPV4_DEST_PORT,    ARAD_PMF_KEY_IPV4_DEST_PORT_MSB, ARAD_PMF_KEY_IPV4_DEST_PORT_LSB,                                     ARAD_PMF_CE_SUB_HEADER_4,                       0},
   {SOC_PPC_FP_QUAL_HDR_INNER_IPV4_TOS,          ARAD_PMF_KEY_IPV4_TOS_MSB, ARAD_PMF_KEY_IPV4_TOS_LSB,                                                 ARAD_PMF_CE_SUB_HEADER_3,                       0},
   {SOC_PPC_FP_QUAL_HDR_INNER_IPV4_TTL,          ARAD_PMF_KEY_IPV4_TTL_MSB, ARAD_PMF_KEY_IPV4_TTL_LSB,                                                 ARAD_PMF_CE_SUB_HEADER_3,                       0},
   {SOC_PPC_FP_QUAL_HDR_INNER_IPV4_TCP_CTL,      ARAD_PMF_KEY_IPV4_TCP_CTL_MSB, ARAD_PMF_KEY_IPV4_TCP_CTL_LSB,                                         ARAD_PMF_CE_SUB_HEADER_4,                       0},
   {SOC_PPC_FP_QUAL_HDR_IPV4_NEXT_PRTCL,         ARAD_PMF_KEY_IPV4_NEXT_PRTCL_MSB, ARAD_PMF_KEY_IPV4_NEXT_PRTCL_LSB,                                   ARAD_PMF_CE_SUB_HEADER_2,  ARAD_PMF_CE_SUB_HEADER_3},
   {SOC_PPC_FP_QUAL_HDR_IPV4_FLAGS,              ARAD_PMF_KEY_IPV4_FLAGS_MSB, ARAD_PMF_KEY_IPV4_FLAGS_LSB,                                             ARAD_PMF_CE_SUB_HEADER_2,  ARAD_PMF_CE_SUB_HEADER_3},
   {SOC_PPC_FP_QUAL_HDR_IPV4_DF,                 ARAD_PMF_KEY_IPV4_DF_MSB, ARAD_PMF_KEY_IPV4_DF_LSB,                                                   ARAD_PMF_CE_SUB_HEADER_2,  ARAD_PMF_CE_SUB_HEADER_3},
   {SOC_PPC_FP_QUAL_HDR_IPV4_MF,                 ARAD_PMF_KEY_IPV4_MF_MSB, ARAD_PMF_KEY_IPV4_MF_LSB,                                                   ARAD_PMF_CE_SUB_HEADER_2,  ARAD_PMF_CE_SUB_HEADER_3},
   {SOC_PPC_FP_QUAL_HDR_IPV4_SIP,                ARAD_PMF_KEY_IPV4_SIP_MSB, ARAD_PMF_KEY_IPV4_SIP_LSB,                                                 ARAD_PMF_CE_SUB_HEADER_2,  ARAD_PMF_CE_SUB_HEADER_3},
   {SOC_PPC_FP_QUAL_HDR_IPV4_DIP,                ARAD_PMF_KEY_IPV4_DIP_MSB, ARAD_PMF_KEY_IPV4_DIP_LSB,                                                 ARAD_PMF_CE_SUB_HEADER_2,  ARAD_PMF_CE_SUB_HEADER_3},
   {SOC_PPC_FP_QUAL_HDR_IPV4_SRC_PORT,           ARAD_PMF_KEY_IPV4_SRC_PORT_MSB, ARAD_PMF_KEY_IPV4_SRC_PORT_LSB,                                       ARAD_PMF_CE_SUB_HEADER_3,  ARAD_PMF_CE_SUB_HEADER_4},
   {SOC_PPC_FP_QUAL_HDR_IPV4_DEST_PORT,          ARAD_PMF_KEY_IPV4_DEST_PORT_MSB, ARAD_PMF_KEY_IPV4_DEST_PORT_LSB,                                     ARAD_PMF_CE_SUB_HEADER_3,  ARAD_PMF_CE_SUB_HEADER_4},
   {SOC_PPC_FP_QUAL_HDR_ELK_RANGE_IPV4_SRC_PORT, ARAD_PMF_KEY_IPV4_SRC_PORT_MSB, ARAD_PMF_KEY_IPV4_SRC_PORT_LSB,                                       ARAD_PMF_CE_SUB_HEADER_3,  ARAD_PMF_CE_SUB_HEADER_4},
   {SOC_PPC_FP_QUAL_HDR_ELK_RANGE_IPV4_DEST_PORT, ARAD_PMF_KEY_IPV4_DEST_PORT_MSB, ARAD_PMF_KEY_IPV4_DEST_PORT_LSB,                                     ARAD_PMF_CE_SUB_HEADER_3,  ARAD_PMF_CE_SUB_HEADER_4},
   {SOC_PPC_FP_QUAL_HDR_IPV4_SRC_DEST_PORT,      ARAD_PMF_KEY_IPV4_SRC_PORT_MSB, ARAD_PMF_KEY_IPV4_DEST_PORT_LSB,                                     ARAD_PMF_CE_SUB_HEADER_3,  0                        },
   {SOC_PPC_FP_QUAL_HDR_IPV4_TOS,                ARAD_PMF_KEY_IPV4_TOS_MSB, ARAD_PMF_KEY_IPV4_TOS_LSB,                                                 ARAD_PMF_CE_SUB_HEADER_2,  ARAD_PMF_CE_SUB_HEADER_3},
   {SOC_PPC_FP_QUAL_HDR_IPV4_TTL,                ARAD_PMF_KEY_IPV4_TTL_MSB, ARAD_PMF_KEY_IPV4_TTL_LSB,                                                 ARAD_PMF_CE_SUB_HEADER_2,  ARAD_PMF_CE_SUB_HEADER_3},
   {SOC_PPC_FP_QUAL_HDR_IPV4_TCP_CTL,            ARAD_PMF_KEY_IPV4_TCP_CTL_MSB, ARAD_PMF_KEY_IPV4_TCP_CTL_LSB,                                         ARAD_PMF_CE_SUB_HEADER_3,  ARAD_PMF_CE_SUB_HEADER_4},
   {SOC_PPC_FP_QUAL_HDR_AFTER_FWD_IPV4_SIP,      ARAD_PMF_KEY_IPV4_SIP_MSB, ARAD_PMF_KEY_IPV4_SIP_LSB,                                                 ARAD_PMF_CE_SUB_HEADER_FWD_POST,                  0},
   {SOC_PPC_FP_QUAL_HDR_AFTER_FWD_IPV4_DIP,      ARAD_PMF_KEY_IPV4_DIP_MSB, ARAD_PMF_KEY_IPV4_DIP_LSB,                                                 ARAD_PMF_CE_SUB_HEADER_FWD_POST,                  0},
   {SOC_PPC_FP_QUAL_HDR_IPV6_SIP_HIGH,           ARAD_PMF_QUAL_HDR_IPV6_SIP_HIGH_MSB, ARAD_PMF_QUAL_HDR_IPV6_SIP_HIGH_LSB,                             ARAD_PMF_CE_SUB_HEADER_2,  ARAD_PMF_CE_SUB_HEADER_3},
   {SOC_PPC_FP_QUAL_HDR_IPV6_SIP_LOW,            ARAD_PMF_QUAL_HDR_IPV6_SIP_LOW_MSB, ARAD_PMF_QUAL_HDR_IPV6_SIP_LOW_LSB,                               ARAD_PMF_CE_SUB_HEADER_2,  ARAD_PMF_CE_SUB_HEADER_3},
   {SOC_PPC_FP_QUAL_HDR_IPV6_DIP_HIGH,           ARAD_PMF_QUAL_HDR_IPV6_DIP_HIGH_MSB, ARAD_PMF_QUAL_HDR_IPV6_DIP_HIGH_LSB,                             ARAD_PMF_CE_SUB_HEADER_2,  ARAD_PMF_CE_SUB_HEADER_3},
   {SOC_PPC_FP_QUAL_HDR_IPV6_DIP_LOW,            ARAD_PMF_QUAL_HDR_IPV6_DIP_LOW_MSB, ARAD_PMF_QUAL_HDR_IPV6_DIP_LOW_LSB,                               ARAD_PMF_CE_SUB_HEADER_2,  ARAD_PMF_CE_SUB_HEADER_3},
   {SOC_PPC_FP_QUAL_HDR_IPV6_NEXT_PRTCL,         ARAD_PMF_QUAL_HDR_IPV6_NEXT_PRTCL_MSB, ARAD_PMF_QUAL_HDR_IPV6_NEXT_PRTCL_LSB,                         ARAD_PMF_CE_SUB_HEADER_2,  ARAD_PMF_CE_SUB_HEADER_3},
   {SOC_PPC_FP_QUAL_HDR_IPV6_TCP_CTL,            ARAD_PMF_QUAL_HDR_IPV6_TCP_CTL_MSB, ARAD_PMF_QUAL_HDR_IPV6_TCP_CTL_LSB,                               ARAD_PMF_CE_SUB_HEADER_3,  ARAD_PMF_CE_SUB_HEADER_4},
   {SOC_PPC_FP_QUAL_HDR_IPV6_TC,                 ARAD_PMF_QUAL_HDR_IPV6_TC_MSB, ARAD_PMF_QUAL_HDR_IPV6_TC_LSB,                                         ARAD_PMF_CE_SUB_HEADER_2,  ARAD_PMF_CE_SUB_HEADER_3},
   {SOC_PPC_FP_QUAL_HDR_IPV6_FLOW_LABEL,         ARAD_PMF_QUAL_HDR_IPV6_FLOW_LABEL_MSB, ARAD_PMF_QUAL_HDR_IPV6_FLOW_LABEL_LSB,                         ARAD_PMF_CE_SUB_HEADER_2,  ARAD_PMF_CE_SUB_HEADER_3},
   {SOC_PPC_FP_QUAL_HDR_IPV6_HOP_LIMIT,          ARAD_PMF_QUAL_HDR_IPV6_HOP_LIMIT_MSB, ARAD_PMF_QUAL_HDR_IPV6_HOP_LIMIT_LSB,                           ARAD_PMF_CE_SUB_HEADER_2,  ARAD_PMF_CE_SUB_HEADER_3},
   {SOC_PPC_FP_QUAL_EXTENSION_HEADER_TYPE,       ARAD_PMF_QUAL_HDR_IPV6_EXT_NEXT_HDR_MSB, ARAD_PMF_QUAL_HDR_IPV6_EXT_NEXT_HDR_LSB,                     ARAD_PMF_CE_SUB_HEADER_2,  ARAD_PMF_CE_SUB_HEADER_3},
   {SOC_PPC_FP_QUAL_HDR_MPLS_LABEL_FWD,          ARAD_PMF_KEY_MPLS_LABEL_MSB, ARAD_PMF_KEY_MPLS_LABEL_LSB,                                            ARAD_PMF_CE_SUB_HEADER_FWD,                     0},
   {SOC_PPC_FP_QUAL_HDR_MPLS_LABEL_ID_FWD,       ARAD_PMF_KEY_MPLS_LABEL_ID_MSB, ARAD_PMF_KEY_MPLS_LABEL_ID_LSB,                                            ARAD_PMF_CE_SUB_HEADER_FWD,                     0},
   {SOC_PPC_FP_QUAL_HDR_MPLS_EXP_FWD,            ARAD_PMF_KEY_MPLS_EXP_MSB, ARAD_PMF_KEY_MPLS_EXP_LSB,                                                ARAD_PMF_CE_SUB_HEADER_FWD,                     0},
   {SOC_PPC_FP_QUAL_HDR_MPLS_TTL_FWD,            ARAD_PMF_KEY_MPLS_TTL_MSB, ARAD_PMF_KEY_MPLS_TTL_LSB,                                                ARAD_PMF_CE_SUB_HEADER_FWD,                     0},
   {SOC_PPC_FP_QUAL_HDR_MPLS_BOS_FWD,            ARAD_PMF_KEY_MPLS_BOS_MSB, ARAD_PMF_KEY_MPLS_BOS_LSB,                                                ARAD_PMF_CE_SUB_HEADER_FWD,                     0},
   {SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1,             ARAD_PMF_KEY_MPLS_LABEL_MSB, ARAD_PMF_KEY_MPLS_LABEL_LSB,                                            ARAD_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1_ID,          ARAD_PMF_KEY_MPLS_LABEL_ID_MSB, ARAD_PMF_KEY_MPLS_LABEL_ID_LSB,                                            ARAD_PMF_CE_SUB_HEADER_2,                     0},
   {SOC_PPC_FP_QUAL_HDR_MPLS_EXP1,               ARAD_PMF_KEY_MPLS_EXP_MSB, ARAD_PMF_KEY_MPLS_EXP_LSB,                                                ARAD_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PPC_FP_QUAL_HDR_MPLS_TTL1,               ARAD_PMF_KEY_MPLS_TTL_MSB, ARAD_PMF_KEY_MPLS_TTL_LSB,                                                ARAD_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PPC_FP_QUAL_HDR_MPLS_BOS1,               ARAD_PMF_KEY_MPLS_BOS_MSB, ARAD_PMF_KEY_MPLS_BOS_LSB,                                                ARAD_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PPC_FP_QUAL_HDR_MPLS_LABEL2,             ARAD_PMF_KEY_MPLS_2_OFFSET + ARAD_PMF_KEY_MPLS_LABEL_MSB, ARAD_PMF_KEY_MPLS_2_OFFSET + ARAD_PMF_KEY_MPLS_LABEL_LSB,                                            ARAD_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PPC_FP_QUAL_HDR_MPLS_LABEL2_ID,          ARAD_PMF_KEY_MPLS_2_OFFSET + ARAD_PMF_KEY_MPLS_LABEL_ID_MSB, ARAD_PMF_KEY_MPLS_2_OFFSET + ARAD_PMF_KEY_MPLS_LABEL_ID_LSB,                                            ARAD_PMF_CE_SUB_HEADER_2,                     0},
   {SOC_PPC_FP_QUAL_HDR_MPLS_EXP2,               ARAD_PMF_KEY_MPLS_2_OFFSET + ARAD_PMF_KEY_MPLS_EXP_MSB, ARAD_PMF_KEY_MPLS_2_OFFSET + ARAD_PMF_KEY_MPLS_EXP_LSB,      ARAD_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PPC_FP_QUAL_HDR_MPLS_TTL2,               ARAD_PMF_KEY_MPLS_2_OFFSET + ARAD_PMF_KEY_MPLS_TTL_MSB, ARAD_PMF_KEY_MPLS_2_OFFSET + ARAD_PMF_KEY_MPLS_TTL_LSB,      ARAD_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PPC_FP_QUAL_HDR_MPLS_BOS2,               ARAD_PMF_KEY_MPLS_2_OFFSET + ARAD_PMF_KEY_MPLS_BOS_MSB, ARAD_PMF_KEY_MPLS_2_OFFSET + ARAD_PMF_KEY_MPLS_BOS_LSB,      ARAD_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PPC_FP_QUAL_HDR_MPLS_LABEL3,             ARAD_PMF_KEY_MPLS_3_OFFSET + ARAD_PMF_KEY_MPLS_LABEL_MSB, ARAD_PMF_KEY_MPLS_3_OFFSET + ARAD_PMF_KEY_MPLS_LABEL_LSB,                                            ARAD_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PPC_FP_QUAL_HDR_MPLS_LABEL3_ID,          ARAD_PMF_KEY_MPLS_3_OFFSET + ARAD_PMF_KEY_MPLS_LABEL_ID_MSB, ARAD_PMF_KEY_MPLS_3_OFFSET + ARAD_PMF_KEY_MPLS_LABEL_ID_LSB,                                            ARAD_PMF_CE_SUB_HEADER_2,                     0},
   {SOC_PPC_FP_QUAL_HDR_MPLS_EXP3,               ARAD_PMF_KEY_MPLS_3_OFFSET + ARAD_PMF_KEY_MPLS_EXP_MSB, ARAD_PMF_KEY_MPLS_3_OFFSET + ARAD_PMF_KEY_MPLS_EXP_LSB,      ARAD_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PPC_FP_QUAL_HDR_MPLS_TTL3,               ARAD_PMF_KEY_MPLS_3_OFFSET + ARAD_PMF_KEY_MPLS_TTL_MSB, ARAD_PMF_KEY_MPLS_3_OFFSET + ARAD_PMF_KEY_MPLS_TTL_LSB,      ARAD_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PPC_FP_QUAL_HDR_MPLS_BOS3,               ARAD_PMF_KEY_MPLS_3_OFFSET + ARAD_PMF_KEY_MPLS_BOS_MSB, ARAD_PMF_KEY_MPLS_3_OFFSET + ARAD_PMF_KEY_MPLS_BOS_LSB,      ARAD_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PPC_FP_QUAL_HDR_MPLS_LABEL4,             ARAD_PMF_KEY_MPLS_4_OFFSET + ARAD_PMF_KEY_MPLS_LABEL_MSB, ARAD_PMF_KEY_MPLS_4_OFFSET + ARAD_PMF_KEY_MPLS_LABEL_LSB,                                            ARAD_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PPC_FP_QUAL_HDR_MPLS_LABEL4_ID,          ARAD_PMF_KEY_MPLS_4_OFFSET + ARAD_PMF_KEY_MPLS_LABEL_ID_MSB, ARAD_PMF_KEY_MPLS_4_OFFSET + ARAD_PMF_KEY_MPLS_LABEL_ID_LSB,                                            ARAD_PMF_CE_SUB_HEADER_2,                     0},
   {SOC_PPC_FP_QUAL_HDR_MPLS_EXP4,               ARAD_PMF_KEY_MPLS_4_OFFSET + ARAD_PMF_KEY_MPLS_EXP_MSB, ARAD_PMF_KEY_MPLS_4_OFFSET + ARAD_PMF_KEY_MPLS_EXP_LSB,      ARAD_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PPC_FP_QUAL_HDR_MPLS_TTL4,               ARAD_PMF_KEY_MPLS_4_OFFSET + ARAD_PMF_KEY_MPLS_TTL_MSB, ARAD_PMF_KEY_MPLS_4_OFFSET + ARAD_PMF_KEY_MPLS_TTL_LSB,      ARAD_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PPC_FP_QUAL_HDR_MPLS_BOS4,               ARAD_PMF_KEY_MPLS_4_OFFSET + ARAD_PMF_KEY_MPLS_BOS_MSB, ARAD_PMF_KEY_MPLS_4_OFFSET + ARAD_PMF_KEY_MPLS_BOS_LSB,      ARAD_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PPC_FP_QUAL_HDR_MPLS_LABEL5,             ARAD_PMF_KEY_MPLS_5_OFFSET + ARAD_PMF_KEY_MPLS_LABEL_MSB, ARAD_PMF_KEY_MPLS_5_OFFSET + ARAD_PMF_KEY_MPLS_LABEL_LSB,                                            ARAD_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PPC_FP_QUAL_HDR_MPLS_EXP5,               ARAD_PMF_KEY_MPLS_5_OFFSET + ARAD_PMF_KEY_MPLS_EXP_MSB, ARAD_PMF_KEY_MPLS_5_OFFSET + ARAD_PMF_KEY_MPLS_EXP_LSB,      ARAD_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PPC_FP_QUAL_HDR_MPLS_TTL5,               ARAD_PMF_KEY_MPLS_5_OFFSET + ARAD_PMF_KEY_MPLS_TTL_MSB, ARAD_PMF_KEY_MPLS_5_OFFSET + ARAD_PMF_KEY_MPLS_TTL_LSB,      ARAD_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PPC_FP_QUAL_HDR_MPLS_BOS5,               ARAD_PMF_KEY_MPLS_5_OFFSET + ARAD_PMF_KEY_MPLS_BOS_MSB, ARAD_PMF_KEY_MPLS_5_OFFSET + ARAD_PMF_KEY_MPLS_BOS_LSB,      ARAD_PMF_CE_SUB_HEADER_2,                       0},
   {SOC_PPC_FP_QUAL_HDR_HIGIG_FRC,                ARAD_PMF_QUAL_HDR_HIGIG_FRC_MSB,                                ARAD_PMF_QUAL_HDR_HIGIG_FRC_LSB,                                  0, 0},
   {SOC_PPC_FP_QUAL_HDR_HIGIG_PPD_EXT,            ARAD_PMF_QUAL_HDR_HIGIG_PPD_EXT_MSB,                ARAD_PMF_QUAL_HDR_HIGIG_PPD_EXT_LSB,                  ARAD_PMF_CE_SUB_HEADER_1, ARAD_PMF_CE_SUB_HEADER_1},
   {SOC_PPC_FP_QUAL_HDR_HIGIG_PPD,                ARAD_PMF_QUAL_HDR_HIGIG_PPD_MSB,                    ARAD_PMF_QUAL_HDR_HIGIG_PPD_LSB,                      ARAD_PMF_CE_SUB_HEADER_1, ARAD_PMF_CE_SUB_HEADER_1},
   {SOC_PPC_FP_QUAL_HDR_PTCH_RESERVE_LSB,        ARAD_PMF_QUAL_HDR_PTCH_RESERVE_LSB_MSB,   ARAD_PMF_QUAL_HDR_PTCH_RESERVE_LSB_LSB,                      ARAD_PMF_CE_SUB_HEADER_0, ARAD_PMF_CE_SUB_HEADER_0},
   {SOC_PPC_FP_QUAL_HDR_MH_FLOW,      			ARAD_PMF_QUAL_HDR_MH_FLOW_MSB,   			ARAD_PMF_QUAL_HDR_MH_FLOW_LSB,                      	ARAD_PMF_CE_SUB_HEADER_0, ARAD_PMF_CE_SUB_HEADER_0},
   {SOC_PPC_FP_QUAL_HDR_MH_TC2,      			ARAD_PMF_QUAL_HDR_MH_TC2_MSB,   			ARAD_PMF_QUAL_HDR_MH_TC2_LSB,                      		ARAD_PMF_CE_SUB_HEADER_0, ARAD_PMF_CE_SUB_HEADER_0},
   {SOC_PPC_FP_QUAL_HDR_MH_DP0,      			ARAD_PMF_QUAL_HDR_MH_DP0_MSB,   			ARAD_PMF_QUAL_HDR_MH_DP0_LSB,                      		ARAD_PMF_CE_SUB_HEADER_0, ARAD_PMF_CE_SUB_HEADER_0},
   {SOC_PPC_FP_QUAL_HDR_MH_CAST,      			ARAD_PMF_QUAL_HDR_MH_CAST_MSB,   			ARAD_PMF_QUAL_HDR_MH_CAST_LSB,                      	ARAD_PMF_CE_SUB_HEADER_0, ARAD_PMF_CE_SUB_HEADER_0},
   {SOC_PPC_FP_QUAL_HDR_MH_DP1,      			ARAD_PMF_QUAL_HDR_MH_DP1_MSB,   			ARAD_PMF_QUAL_HDR_MH_DP1_LSB,                      		ARAD_PMF_CE_SUB_HEADER_0, ARAD_PMF_CE_SUB_HEADER_0},
   {SOC_PPC_FP_QUAL_HDR_MH_TC10,      			ARAD_PMF_QUAL_HDR_MH_TC10_MSB,   			ARAD_PMF_QUAL_HDR_MH_TC10_LSB,                      	ARAD_PMF_CE_SUB_HEADER_0, ARAD_PMF_CE_SUB_HEADER_0},
   {SOC_PPC_FP_QUAL_HDR_INPHEADER_UC,      			ARAD_PMF_QUAL_HDR_INPHEADER_UC_MSB,   			ARAD_PMF_QUAL_HDR_INPHEADER_UC_LSB,                      	ARAD_PMF_CE_SUB_HEADER_1, ARAD_PMF_CE_SUB_HEADER_1},
   {SOC_PPC_FP_QUAL_HDR_INPHEADER_TB,      			ARAD_PMF_QUAL_HDR_INPHEADER_TB_MSB,   			ARAD_PMF_QUAL_HDR_INPHEADER_TB_LSB,                      	ARAD_PMF_CE_SUB_HEADER_1, ARAD_PMF_CE_SUB_HEADER_1},
   {SOC_PPC_FP_QUAL_HDR_INPHEADER_UC_TC,      			ARAD_PMF_QUAL_HDR_INPHEADER_UC_TC_MSB,   			ARAD_PMF_QUAL_HDR_INPHEADER_UC_TC_LSB,              ARAD_PMF_CE_SUB_HEADER_1, ARAD_PMF_CE_SUB_HEADER_1},
   {SOC_PPC_FP_QUAL_HDR_INPHEADER_MC_TC,      			ARAD_PMF_QUAL_HDR_INPHEADER_MC_TC_MSB,   			ARAD_PMF_QUAL_HDR_INPHEADER_MC_TC_LSB,              ARAD_PMF_CE_SUB_HEADER_1, ARAD_PMF_CE_SUB_HEADER_1},
   {SOC_PPC_FP_QUAL_HDR_INPHEADER_DP,      			ARAD_PMF_QUAL_HDR_INPHEADER_DP_MSB,   			ARAD_PMF_QUAL_HDR_INPHEADER_DP_LSB,                      	ARAD_PMF_CE_SUB_HEADER_1, ARAD_PMF_CE_SUB_HEADER_1},
   {SOC_PPC_FP_QUAL_TUNNEL_ID,                  ARAD_PMF_QUAL_HDR_TUNNEL_ID_MSB,   		ARAD_PMF_QUAL_HDR_TUNNEL_ID_LSB,                      	ARAD_PMF_CE_SUB_HEADER_1, ARAD_PMF_CE_SUB_HEADER_1},
   {SOC_PPC_FP_QUAL_ARP_SENDER_IP4,             ARAD_PMF_QUAL_HDR_ARP_SENDER_MSB,   ARAD_PMF_QUAL_HDR_ARP_SENDER_LSB,                              ARAD_PMF_CE_SUB_HEADER_2, ARAD_PMF_CE_SUB_HEADER_2},
   {SOC_PPC_FP_QUAL_ARP_TARGET_IP4,             ARAD_PMF_QUAL_HDR_ARP_TARGET_MSB,   ARAD_PMF_QUAL_HDR_ARP_TARGET_LSB,                              ARAD_PMF_CE_SUB_HEADER_2, ARAD_PMF_CE_SUB_HEADER_2},
   {SOC_PPC_FP_QUAL_ARP_OPCODE_IP4,             ARAD_PMF_QUAL_HDR_ARP_OPCODE_MSB,   ARAD_PMF_QUAL_HDR_ARP_OPCODE_LSB,                              ARAD_PMF_CE_SUB_HEADER_2, ARAD_PMF_CE_SUB_HEADER_2},
   {SOC_PPC_FP_QUAL_ETH_OAM_HEADER_BITS_0_31 ,               ARAD_PMF_QUAL_ETH_OAM_HEADER_BITS_0_31_MSB,   ARAD_PMF_QUAL_ETH_OAM_HEADER_BITS_0_31_LSB ,                                    ARAD_PMF_CE_SUB_HEADER_2,0},
   {SOC_PPC_FP_QUAL_ETH_OAM_HEADER_BITS_32_63 ,              ARAD_PMF_QUAL_ETH_OAM_HEADER_BITS_32_63_MSB,  ARAD_PMF_QUAL_ETH_OAM_HEADER_BITS_32_63_LSB,                                    ARAD_PMF_CE_SUB_HEADER_2,0},
   {SOC_PPC_FP_QUAL_MPLS_OAM_HEADER_BITS_0_31 ,              ARAD_PMF_QUAL_MPLS_OAM_HEADER_BITS_0_31_MSB,  ARAD_PMF_QUAL_MPLS_OAM_HEADER_BITS_0_32_LSB,                                    ARAD_PMF_CE_SUB_HEADER_3,0},
   {SOC_PPC_FP_QUAL_MPLS_OAM_HEADER_BITS_32_63 ,             ARAD_PMF_QUAL_MPLS_OAM_HEADER_BITS_32_63_MSB, ARAD_PMF_QUAL_MPLS_OAM_HEADER_BITS_32_63_LSB,                                   ARAD_PMF_CE_SUB_HEADER_3,0},
   {SOC_PPC_FP_QUAL_MPLS_OAM_ACH  ,                          ARAD_PMF_QUAL_QUAL_MPLS_OAM_ACH_MSB,          ARAD_PMF_QUAL_QUAL_MPLS_OAM_ACH_LSB,                   ARAD_PMF_CE_SUB_HEADER_3,  ARAD_PMF_CE_SUB_HEADER_4},
   {SOC_PPC_FP_QUAL_OAM_HEADER_BITS_0_31 ,                   ARAD_PMF_QUAL_OAM_HEADER_BITS_0_31_MSB,       ARAD_PMF_QUAL_OAM_HEADER_BITS_0_31_LSB,                                         ARAD_PMF_CE_SUB_HEADER_2,0},
   {SOC_PPC_FP_QUAL_OAM_HEADER_BITS_32_63,                   ARAD_PMF_QUAL_OAM_HEADER_BITS_31_63_MSB,      ARAD_PMF_QUAL_OAM_HEADER_BITS_31_63_LSB,                                        ARAD_PMF_CE_SUB_HEADER_2,0},
   {SOC_PPC_FP_QUAL_OAM_OPCODE ,                             ARAD_PMF_QUAL_OAM_OPCODE_MSB,                 ARAD_PMF_QUAL_OAM_OPCODE_LSB ,                                                  ARAD_PMF_CE_SUB_HEADER_2,0},
   {SOC_PPC_FP_QUAL_OAM_MD_LEVEL_UNTAGGED ,                  ARAD_PMF_QUAL_OAM_MD_LEVEL_UNTAGGED_MSB,      ARAD_PMF_QUAL_OAM_MD_LEVEL_UNTAGGED_LSB ,                   ARAD_PMF_CE_SUB_HEADER_FWD_POST,	ARAD_PMF_CE_SUB_HEADER_FWD_POST},
   {SOC_PPC_FP_QUAL_TM_OUTER_TAG ,                  ARAD_PMF_QUAL_TM_OUTER_TAG_MSB,      ARAD_PMF_QUAL_TM_OUTER_TAG_LSB ,                   ARAD_PMF_CE_SUB_HEADER_FWD_POST,	ARAD_PMF_CE_SUB_HEADER_FWD_POST},
   {SOC_PPC_FP_QUAL_TM_INNER_TAG ,                  ARAD_PMF_QUAL_TM_INNER_TAG_MSB,      ARAD_PMF_QUAL_TM_INNER_TAG_LSB ,                   ARAD_PMF_CE_SUB_HEADER_FWD_POST,	ARAD_PMF_CE_SUB_HEADER_FWD_POST},
   {SOC_PPC_FP_QUAL_OAM_MD_LEVEL_SINGLE_TAG ,                  ARAD_PMF_QUAL_OAM_MD_LEVEL_SINGLE_TAG_MSB,      ARAD_PMF_QUAL_OAM_MD_LEVEL_SINGLE_TAG_LSB ,                   ARAD_PMF_CE_SUB_HEADER_FWD_POST,	ARAD_PMF_CE_SUB_HEADER_FWD_POST},
   {SOC_PPC_FP_QUAL_OAM_MD_LEVEL_DOUBLE_TAG ,                  ARAD_PMF_QUAL_OAM_MD_LEVEL_DOUBLE_TAG_MSB,      ARAD_PMF_QUAL_OAM_MD_LEVEL_DOUBLE_TAG_LSB ,                   ARAD_PMF_CE_SUB_HEADER_FWD_POST,	ARAD_PMF_CE_SUB_HEADER_FWD_POST},
   {SOC_PPC_FP_QUAL_OAM_MD_LEVEL,                  ARAD_PMF_QUAL_OAM_MD_LEVEL_MSB,      ARAD_PMF_QUAL_OAM_MD_LEVEL_LSB ,                   ARAD_PMF_CE_SUB_HEADER_2,	ARAD_PMF_CE_SUB_HEADER_2},

   {SOC_PPC_FP_QUAL_MY_DISCR_IPV4 ,                  ARAD_PMF_QUAL_MY_DISCR_IPV4_MSB,      ARAD_PMF_QUAL_MY_DISCR_IPV4_LSB ,                   ARAD_PMF_CE_SUB_HEADER_FWD_POST,	ARAD_PMF_CE_SUB_HEADER_FWD_POST},
   {SOC_PPC_FP_QUAL_MY_DISCR_MPLS ,                  ARAD_PMF_QUAL_MY_DISCR_MPLS_MSB,      ARAD_PMF_QUAL_MY_DISCR_MPLS_LSB ,                   ARAD_PMF_CE_SUB_HEADER_FWD_POST,	ARAD_PMF_CE_SUB_HEADER_FWD_POST},
   {SOC_PPC_FP_QUAL_MY_DISCR_PWE ,                  ARAD_PMF_QUAL_MY_DISCR_PWE_MSB,      ARAD_PMF_QUAL_MY_DISCR_PWE_LSB ,                   ARAD_PMF_CE_SUB_HEADER_FWD_POST,	ARAD_PMF_CE_SUB_HEADER_FWD_POST},

   
   {SOC_PPC_FP_QUAL_TRAP_QUALIFIER_FHEI, 131 ,                                143,                     ARAD_PMF_CE_SUB_HEADER_0,  ARAD_PMF_CE_SUB_HEADER_0},
   {SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP,          ARAD_PMF_KEY_IPV4_SIP_MSB, ARAD_PMF_KEY_IPV4_SIP_LSB,  ARAD_PMF_CE_SUB_HEADER_FWD,    0},
   {SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP,          ARAD_PMF_KEY_IPV4_DIP_MSB, ARAD_PMF_KEY_IPV4_DIP_LSB,  ARAD_PMF_CE_SUB_HEADER_FWD,    0},
   {SOC_PPC_FP_QUAL_HDR_FWD_VLAN_TAG_ID,       ARAD_PMF_KEY_VLAN_TAG_ID_1ST_MSB, ARAD_PMF_KEY_VLAN_TAG_ID_1ST_LSB, ARAD_PMF_CE_SUB_HEADER_FWD, 0},
   {SOC_PPC_FP_QUAL_HDR_FWD_SA,                ARAD_PMF_KEY_SA_MSB,                     ARAD_PMF_KEY_SA_LSB,                    ARAD_PMF_CE_SUB_HEADER_FWD, 0},
   {SOC_PPC_FP_QUAL_HDR_FWD_DA,                ARAD_PMF_KEY_DA_MSB,                     ARAD_PMF_KEY_DA_LSB,                    ARAD_PMF_CE_SUB_HEADER_FWD, 0},
   {SOC_PPC_FP_QUAL_HDR_FWD_INNERMOST_VLAN_TAG_ID,     -(12+16),       -(1+16),                                ARAD_PMF_CE_SUB_HEADER_FWD_POST,  0},
   {SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH,     ARAD_PMF_QUAL_HDR_IPV6_SIP_HIGH_MSB,     ARAD_PMF_QUAL_HDR_IPV6_SIP_HIGH_LSB,    ARAD_PMF_CE_SUB_HEADER_FWD,  0},
   {SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_LOW,      ARAD_PMF_QUAL_HDR_IPV6_SIP_LOW_MSB,      ARAD_PMF_QUAL_HDR_IPV6_SIP_LOW_LSB,     ARAD_PMF_CE_SUB_HEADER_FWD,  0},
   {SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH,     ARAD_PMF_QUAL_HDR_IPV6_DIP_HIGH_MSB,     ARAD_PMF_QUAL_HDR_IPV6_DIP_HIGH_LSB,    ARAD_PMF_CE_SUB_HEADER_FWD,  0},
   {SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW,      ARAD_PMF_QUAL_HDR_IPV6_DIP_LOW_MSB,      ARAD_PMF_QUAL_HDR_IPV6_DIP_LOW_LSB,     ARAD_PMF_CE_SUB_HEADER_FWD,  0},   
   {SOC_PPC_FP_QUAL_HDR_FWD_VLAN_2ND_TAG_ID,   ARAD_PMF_KEY_VLAN_TAG_ID_2ND_MSB, ARAD_PMF_KEY_VLAN_TAG_ID_2ND_LSB, ARAD_PMF_CE_SUB_HEADER_FWD, 0},

   
   {SOC_PPC_FP_QUAL_TRILL_INGRESS_NICK,          ARAD_PMF_QUAL_HDR_TRILL_INGRESS_NICK_MSB, ARAD_PMF_QUAL_HDR_TRILL_INGRESS_NICK_LSB , ARAD_PMF_CE_SUB_HEADER_2, 0},
   {SOC_PPC_FP_QUAL_TRILL_EGRESS_NICK ,          ARAD_PMF_QUAL_HDR_TRILL_EGRESS_NICK_MSB , ARAD_PMF_QUAL_HDR_TRILL_EGRESS_NICK_LSB  , ARAD_PMF_CE_SUB_HEADER_2, 0},
   {SOC_PPC_FP_QUAL_TRILL_NATIVE_VLAN_VSI,       ARAD_PMF_KEY_VLAN_TAG_ID_1ST_MSB        , ARAD_PMF_KEY_VLAN_TAG_ID_1ST_LSB         , ARAD_PMF_CE_SUB_HEADER_3, 0},
   {SOC_PPC_FP_QUAL_TRILL_NATIVE_ETH_INNER_TPID, ARAD_PMF_KEY_VLAN_TAG_TPID_2ND_MSB      , ARAD_PMF_KEY_VLAN_TAG_TPID_2ND_LSB       , ARAD_PMF_CE_SUB_HEADER_3, 0},
   {SOC_PPC_FP_QUAL_TRILL_NATIVE_INNER_VLAN_VSI, ARAD_PMF_KEY_VLAN_TAG_ID_2ND_MSB        , ARAD_PMF_KEY_VLAN_TAG_ID_2ND_LSB         , ARAD_PMF_CE_SUB_HEADER_3, 0},


   
   {SOC_PPC_FP_QUAL_GRE_CRKS               , ARAD_PMF_KEY_GRE_CRKS_MSB          , ARAD_PMF_KEY_GRE_CRKS_LSB          , ARAD_PMF_CE_SUB_HEADER_2, 0},
   {SOC_PPC_FP_QUAL_GRE_KEY                , ARAD_PMF_KEY_GRE_KEY_MSB           , ARAD_PMF_KEY_GRE_KEY_LSB           , ARAD_PMF_CE_SUB_HEADER_2, 0},
   {SOC_PPC_FP_QUAL_GRE_PROTOCOL_TYPE      , ARAD_PMF_KEY_GRE_PROTOCOL_TYPE_MSB , ARAD_PMF_KEY_GRE_PROTOCOL_TYPE_LSB , ARAD_PMF_CE_SUB_HEADER_2, 0},

   {SOC_PPC_FP_QUAL_NATIVE_VLAN_VSI        , ARAD_PMF_KEY_VLAN_TAG_ID_1ST_MSB   , ARAD_PMF_KEY_VLAN_TAG_ID_1ST_LSB   , ARAD_PMF_CE_SUB_HEADER_3, 0},

   {SOC_PPC_FP_QUAL_ETH_HEADER_ISID        , ARAD_PMF_QUAL_ETH_HEADER_ISID_MSB  , ARAD_PMF_QUAL_ETH_HEADER_ISID_LSB  , ARAD_PMF_CE_SUB_HEADER_2, 0},
   {SOC_PPC_FP_QUAL_EID,      ARAD_PMF_QUAL_EID_MSB,      ARAD_PMF_QUAL_EID_LSB,     ARAD_PMF_CE_SUB_HEADER_1,  0},   


   {SOC_PPC_FP_QUAL_VXLAN_VNI              , ARAD_PMF_QUAL_VXLAN_VNI_MSB        , ARAD_PMF_QUAL_VXLAN_VNI_LSB        , ARAD_PMF_CE_SUB_HEADER_3, 0},

   
   {SOC_PPC_FP_QUAL_PORT_EXTENDER_ETAG     , ARAD_PMF_QUAL_PORT_EXTENDER_ETAG_MSB, ARAD_PMF_QUAL_PORT_EXTENDER_ETAG_LSB, ARAD_PMF_CE_SUB_HEADER_1, 0},
   {SOC_PPC_FP_QUAL_PORT_EXTENDER_ECID     , ARAD_PMF_QUAL_PORT_EXTENDER_ECID_MSB, ARAD_PMF_QUAL_PORT_EXTENDER_ECID_LSB, ARAD_PMF_CE_SUB_HEADER_1, 0},

   {SOC_PPC_FP_QUAL_CUSTOM_PP_HEADER_OUTPUT_FP     , 44, 63, ARAD_PMF_CE_SUB_HEADER_0, 0},
   {SOC_PPC_FP_QUAL_CUSTOM_PP_HEADER_OUTPUT_LSP_ECMP_FP, 96, 127, ARAD_PMF_CE_SUB_HEADER_0, 0},

   
   {SOC_PPC_FP_QUAL_FC_WITH_VFT_D_ID      , ARAD_PMF_KEY_FC_D_ID_MSB             , ARAD_PMF_KEY_FC_D_ID_LSB                , ARAD_PMF_CE_SUB_HEADER_3    , 0},
   {SOC_PPC_FP_QUAL_FC_D_ID               , ARAD_PMF_KEY_FC_D_ID_MSB             , ARAD_PMF_KEY_FC_D_ID_LSB                , ARAD_PMF_CE_SUB_HEADER_2    , 0},
   {SOC_PPC_FP_QUAL_FC_WITH_VFT_VFT_ID    , ARAD_PMF_KEY_FC_WITH_VFT_VFT_ID_MSB  , ARAD_PMF_KEY_FC_WITH_VFT_VFT_ID_LSB     , ARAD_PMF_CE_SUB_HEADER_2    , 0},

   
   {SOC_PPC_FP_QUAL_L2TP_TUNNEL_SESSION	  , ARAD_PMF_QUAL_L2TP_TUNNEL_MSB	     , ARAD_PMF_QUAL_L2TP_SESSION_LSB		   , ARAD_PMF_CE_SUB_HEADER_3, 0},
   {SOC_PPC_FP_QUAL_L2TP_TUNNEL		 	  , ARAD_PMF_QUAL_L2TP_TUNNEL_MSB	     , ARAD_PMF_QUAL_L2TP_TUNNEL_LSB	  	   , ARAD_PMF_CE_SUB_HEADER_3, 0},
   {SOC_PPC_FP_QUAL_L2TP_SESSION	 	  , ARAD_PMF_QUAL_L2TP_SESSION_MSB       , ARAD_PMF_QUAL_L2TP_SESSION_LSB		   , ARAD_PMF_CE_SUB_HEADER_3, 0},

   
   {SOC_PPC_FP_QUAL_IPV6_COMPRESSED_SIP   , ARAD_PMF_QUAL_IPV6_COMPRESSED_SIP_MSB, ARAD_PMF_QUAL_IPV6_COMPRESSED_SIP_LSB,  ARAD_PMF_CE_SUB_HEADER_0, 0}

 };





CONST STATIC 
    ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES
        Arad_pmf_ce_irpp_flp_info[] = {
               {SOC_PPC_FP_QUAL_IRPP_INVALID,1,1,0,0},
               {SOC_PPC_FP_QUAL_FWD_PRCESSING_PROFILE,1,1,0,3}, 
               {SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES,1,1,0,32},
               {SOC_PPC_FP_QUAL_IRPP_ALL_ONES,1,1,32,32},
               {SOC_PPC_FP_QUAL_IRPP_SRC_PP_PORT,1,1,64,8},
               {SOC_PPC_FP_QUAL_IRPP_IN_LIF_DATA,1,1,0,0}, 
               {SOC_PPC_FP_QUAL_IRPP_SRC_TM_PORT,1,1,72,8},
               {SOC_PPC_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR,1,1,80,16}, 
               {SOC_PPC_FP_QUAL_CPU2FLP_C_INTERNAL_FIELDS_DATA,1,1,96,32},
               {SOC_PPC_FP_QUAL_PACKET_HEADER_SIZE,1,1,128,7},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET0,1,1,136,7},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET1,1,1,144,7},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET2,1,1,152,7},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET3,1,1,160,7},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET4,1,1,168,7},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET5,1,1,176,7},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET_0_UNTIL_5,1,0,184-3,(8*6)}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1,1,1,184,11},
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_NEXT_PROTOCOL,1,1,184+7,4}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_ENCAPSULATION,1,1,184+5,2}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_OUTER_TAG    ,1,1,184+3,2}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_PRIORITY     ,1,1,184+2,1}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_INNER_TAG    ,1,1,184+0,2}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2,1,1,196,11},
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_BOS,1,1,196,1} ,
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_NEXT_PROTOCOL,1,1,196+7,4}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_IP_FRAGMENTED,1,1,196+6,1}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_IP_HAS_OPTIONS,1,1,196+4,1}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER3,1,1,208,11},
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER3_IP_FRAGMENTED,1,1,208+6,1}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER4,1,1,220,11},
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER5,1,1,232,11},
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER0,1,1,244,3},
               {SOC_PPC_FP_QUAL_IRPP_PKT_HDR_TYPE,1,1,248,6},
               {SOC_PPC_FP_QUAL_IRPP_SRC_SYST_PORT,1,1,256,16},
               {SOC_PPC_FP_QUAL_IRPP_PARSER_LEAF_CONTEXT,1,1,272,4},
               {SOC_PPC_FP_QUAL_SERVICE_TYPE,1,1,276,3},
               {SOC_PPC_FP_QUAL_IRPP_ETH_TAG_FORMAT,1,1,280,4},
               {SOC_PPC_FP_QUAL_FORWARDING_ACTION_STRENGTH,1,1,284,3},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DEST,1,1,288,19},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_TC,1,1,308,3},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DP,1,1,312,2},
               {SOC_PPC_FP_QUAL_FORWARDING_ACTION_METER_TRAFFIC_CLASS,1,1,316,2},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_CODE,1,1,320,8},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_QUAL,1,1,328,16},
               {SOC_PPC_FP_QUAL_IRPP_SNOOP_CODE,1,1,344,8},
               {SOC_PPC_FP_QUAL_SNOOP_STRENGTH,1,1,352,2},
               {SOC_PPC_FP_QUAL_IRPP_LL_MIRROR_CMD,1,1,356,4},
               {SOC_PPC_FP_QUAL_IRPP_UP,1,1,360,3},
               {SOC_PPC_FP_QUAL_IRPP_SYSTEM_VSI,1,1,364,16},
               {SOC_PPC_FP_QUAL_VSI_PROFILE,1,1,380+2,2}, 
               {SOC_PPC_FP_QUAL_VSI_UNKNOWN_DA_DESTINATION,1,1,384,19},
               {SOC_PPC_FP_QUAL_FID,1,1,404,15},
               {SOC_PPC_FP_QUAL_IRPP_ORIENTATION_IS_HUB,1,1,420,1},
               {SOC_PPC_FP_QUAL_I_SID,1,1,424,24},
               {SOC_PPC_FP_QUAL_IRPP_VLAN_ID,1,0,448,6},
               {SOC_PPC_FP_QUAL_IRPP_VLAN_DEI,1,0,454,1},
               {SOC_PPC_FP_QUAL_IRPP_VLAN_PCP,1,0,455,3},
               {SOC_PPC_FP_QUAL_VLAN_EDIT_CMD_VID2,1,0,458,12},
               {SOC_PPC_FP_QUAL_VLAN_EDIT_CMD_VID1,1,0,470,12},
               {SOC_PPC_FP_QUAL_TT_LEARN_ENABLE,1,1,484,1},
               {SOC_PPC_FP_QUAL_TT_LEARN_DATA,1,1,488,40},
               {SOC_PPC_FP_QUAL_IRPP_STP_STATE,1,1,528,2},
               {SOC_PPC_FP_QUAL_IRPP_FWD_TYPE,1,1,532,4},
               {SOC_PPC_FP_QUAL_IRPP_SUB_HEADER_NDX,1,1,536,3},
               {SOC_PPC_FP_QUAL_FORWARDING_OFFSET_EXTENSION,1,1,540,2},
               {SOC_PPC_FP_QUAL_IRPP_TERM_TYPE,1,1,544,4},
               {SOC_PPC_FP_QUAL_EEI,1,1,552,24},
               {SOC_PPC_FP_QUAL_OUT_LIF,1,1,576,16},
               {SOC_PPC_FP_QUAL_IRPP_IN_LIF,1,1,592,16},
               {SOC_PPC_FP_QUAL_IN_LIF_PROFILE,1,1,608,4},
               {SOC_PPC_FP_QUAL_IN_LIF_PROFILE_SAME_IF,1,1,608,1}, 
               {SOC_PPC_FP_QUAL_IN_LIF_UNKNOWN_DA_PROFILE,1,1,612,2},
               {SOC_PPC_FP_QUAL_IRPP_IN_RIF,1,1,616,12},
               {SOC_PPC_FP_QUAL_IN_RIF_UC_RPF_ENABLE,1,1,628,1},
               {SOC_PPC_FP_QUAL_IRPP_VRF,1,1,632,12},
               {SOC_PPC_FP_QUAL_L3VPN_DEFAULT_ROUTING,1,1,644,1},
               {SOC_PPC_FP_QUAL_IRPP_PCKT_IS_COMP_MC,1,1,648,1},
               {SOC_PPC_FP_QUAL_TERMINATED_TTL_VALID,1,1,652,1},
               {SOC_PPC_FP_QUAL_TERMINATED_TTL,1,1,656,8},
               {SOC_PPC_FP_QUAL_TERMINATED_DSCP_EXP,1,1,664,8},
               {SOC_PPC_FP_QUAL_IRPP_IN_DSCP_EXP,1,1,672,8},
               {SOC_PPC_FP_QUAL_IRPP_IN_TTL,1,1,680,8},
               {SOC_PPC_FP_QUAL_IPR2DSP_6EQ7_ESADI,1,1,688,1},
               {SOC_PPC_FP_QUAL_IPR2DSP_6EQ7_MPLS_EXP,1,1,692,3},
               {SOC_PPC_FP_QUAL_IRPP_PMF_PGM_KEY_GEN_VAR,1,1,696,32},
               {SOC_PPC_FP_QUAL_TERMINATED_PROTOCOL,1,1,728,2},
               {SOC_PPC_FP_QUAL_COS_PROFILE,1,1,732,6},
               {SOC_PPC_FP_QUAL_COUNTER_POINTER_A,1,1,744,21},
               {SOC_PPC_FP_QUAL_COUNTER_UPDATE_A,1,1,768,1},
               {SOC_PPC_FP_QUAL_COUNTER_POINTER_B,1,1,776,21},
               {SOC_PPC_FP_QUAL_COUNTER_UPDATE_B,1,1,800,1},
               {SOC_PPC_FP_QUAL_VTT_OAM_LIF_VALID,1,1,804,1},
               {SOC_PPC_FP_QUAL_VTT_OAM_LIF,1,1,808,16},
               {SOC_PPC_FP_QUAL_PROGRAM_INDEX,1,1,824,5},
               {SOC_PPC_FP_QUAL_LL_LEM_1ST_LOOKUP_FOUND,1,1,832,1},
               {SOC_PPC_FP_QUAL_LL_LEM_1ST_LOOKUP_RESULT,1,1,840,43},
               {SOC_PPC_FP_QUAL_VT_ISA_KEY,1,1,888,41},
               {SOC_PPC_FP_QUAL_VT_PROCESSING_PROFILE,1,1,932,3},
               {SOC_PPC_FP_QUAL_VT_LOOKUP0_FOUND,1,1,936,1},
               {SOC_PPC_FP_QUAL_VT_LOOKUP0_PAYLOAD,1,1,940,16},
               {SOC_PPC_FP_QUAL_VT_LOOKUP1_FOUND,1,1,956,1},
               {SOC_PPC_FP_QUAL_VT_LOOKUP1_PAYLOAD,1,1,960,16},
               {SOC_PPC_FP_QUAL_TT_PROCESSING_PROFILE,1,1,976,3},
               {SOC_PPC_FP_QUAL_TT_LOOKUP0_FOUND,1,1,980,1},
               {SOC_PPC_FP_QUAL_TT_LOOKUP0_PAYLOAD,1,1,984,16},
               {SOC_PPC_FP_QUAL_TT_LOOKUP1_FOUND,1,1,1000,1},
               {SOC_PPC_FP_QUAL_TT_LOOKUP1_PAYLOAD,1,1,1004,16},
               {SOC_PPC_FP_QUAL_VTT_OAM_LIF_VALID, 1,1,804,1},
               {SOC_PPC_FP_QUAL_VTT_OAM_LIF,1,1,808,16},
               {SOC_PPC_FP_QUAL_IRPP_IN_RIF_PROFILE,1,1,1,1}, 
        };


CONST STATIC 
    ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES
        Jericho_ce_irpp_flp_info[] = {
               {SOC_PPC_FP_QUAL_IRPP_INVALID,1,1,0,0},
               {SOC_PPC_FP_QUAL_FWD_PRCESSING_PROFILE,1,1,0,3}, 
               {SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES,1,0,0,32},
               {SOC_PPC_FP_QUAL_IRPP_ALL_ONES,1,0,32,32},
               
               {SOC_PPC_FP_QUAL_IRPP_SRC_TM_PORT,1,0,72,8},
               {SOC_PPC_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR,1,0,80,16},
               {SOC_PPC_FP_QUAL_IRPP_SRC_PP_PORT,1,0,96,9},   
               {SOC_PPC_FP_QUAL_CPU2FLP_C_INTERNAL_FIELDS_DATA,1,0,112,32},
               {SOC_PPC_FP_QUAL_PACKET_HEADER_SIZE,1,0,144,7},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET0,1,0,152,7},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET1,1,0,160,7},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET2,1,0,168,7},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET3,1,0,176,7},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET4,1,0,184,7},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET5,1,0,192,7},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET_0_UNTIL_5,1,0,200-3,(8*6)}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1,1,0,200,11},
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_NEXT_PROTOCOL,1,0,200+7,4}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_ENCAPSULATION,1,0,200+5,2}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2,1,0,216,11},
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_BOS,1,0,216,1} ,
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_NEXT_PROTOCOL,1,0,216+7,4}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_IP_FRAGMENTED,1,0,216+6,1}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_IP_HAS_OPTIONS,1,0,216+4,1}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER3,1,0,232,11},
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER3_IP_FRAGMENTED,1,0,232+6,1}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER4,1,0,248,11},
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER5,1,0,264,11},
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER0,1,0,280,3},
               {SOC_PPC_FP_QUAL_IRPP_PKT_HDR_TYPE,1,0,288,6},
               {SOC_PPC_FP_QUAL_IRPP_SRC_SYST_PORT,1,0,296,16},
               {SOC_PPC_FP_QUAL_IRPP_PARSER_LEAF_CONTEXT,1,0,312,4},
               {SOC_PPC_FP_QUAL_SERVICE_TYPE,1,0,320,3},
               {SOC_PPC_FP_QUAL_IRPP_ETH_TAG_FORMAT,1,0,328,4},
               {SOC_PPC_FP_QUAL_FORWARDING_ACTION_STRENGTH,1,0,336,3},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DEST,1,0,344,19},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_TC,1,0,368,3},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DP,1,0,376,2},
               {SOC_PPC_FP_QUAL_FORWARDING_ACTION_METER_TRAFFIC_CLASS,1,0,384,2},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_CODE,1,0,392,8},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_QUAL,1,0,400,16},
               {SOC_PPC_FP_QUAL_IRPP_SNOOP_CODE,1,0,416,8},
               {SOC_PPC_FP_QUAL_SNOOP_STRENGTH,1,0,424,2},
               {SOC_PPC_FP_QUAL_IRPP_LL_MIRROR_CMD,1,0,432,4},
               {SOC_PPC_FP_QUAL_IRPP_UP,1,0,440,3},
               {SOC_PPC_FP_QUAL_IRPP_SYSTEM_VSI,1,0,448,16},
               {SOC_PPC_FP_QUAL_VSI_PROFILE,1,0,464+2,2}, 
               {SOC_PPC_FP_QUAL_VSI_UNKNOWN_DA_DESTINATION,1,0,472,19},
               {SOC_PPC_FP_QUAL_FID,1,0,496,15},
               {SOC_PPC_FP_QUAL_IRPP_ORIENTATION_IS_HUB,1,0,512,1},
               {SOC_PPC_FP_QUAL_I_SID,1,0,520,24},
               {SOC_PPC_FP_QUAL_IRPP_VLAN_ID,1,0,572,6},
               {SOC_PPC_FP_QUAL_IRPP_VLAN_PCP,1,0,569,3},
               {SOC_PPC_FP_QUAL_IRPP_VLAN_DEI,1,0,568,1},
               {SOC_PPC_FP_QUAL_VLAN_EDIT_CMD_VID2,1,0,556,12},
               {SOC_PPC_FP_QUAL_VLAN_EDIT_CMD_VID1,1,0,544,12},
               {SOC_PPC_FP_QUAL_TT_LEARN_ENABLE,1,0,584,1},
               {SOC_PPC_FP_QUAL_TT_LEARN_DATA,1,0,592,40},
               {SOC_PPC_FP_QUAL_IRPP_STP_STATE,1,0,632,2},
               {SOC_PPC_FP_QUAL_IRPP_FWD_TYPE,1,0,640,4},
               {SOC_PPC_FP_QUAL_IRPP_SUB_HEADER_NDX,1,0,648,3},
               {SOC_PPC_FP_QUAL_FORWARDING_OFFSET_EXTENSION,1,0,656,2},
               {SOC_PPC_FP_QUAL_IRPP_TERM_TYPE,1,0,664,4},
               {SOC_PPC_FP_QUAL_EEI,1,0,672,24},
               {SOC_PPC_FP_QUAL_OUT_LIF,1,0,696,18},
               {SOC_PPC_FP_QUAL_IRPP_IN_LIF,1,0,720,18},
               {SOC_PPC_FP_QUAL_IN_LIF_PROFILE,1,0,744,4},
               {SOC_PPC_FP_QUAL_IN_LIF_PROFILE_SAME_IF,1,0,744,1}, 
               {SOC_PPC_FP_QUAL_IN_LIF_UNKNOWN_DA_PROFILE,1,0,752,2},
               {SOC_PPC_FP_QUAL_IRPP_IN_RIF,1,0,760,15},
               {SOC_PPC_FP_QUAL_IRPP_IN_RIF_PROFILE,1,0,776,6},
               {SOC_PPC_FP_QUAL_IRPP_VRF,1,0,784,14},
               {SOC_PPC_FP_QUAL_IRPP_PCKT_IS_COMP_MC,1,0,800,1},
               {SOC_PPC_FP_QUAL_TERMINATED_TTL_VALID,1,0,808,1},
               {SOC_PPC_FP_QUAL_TERMINATED_TTL,1,0,816,8},
               {SOC_PPC_FP_QUAL_TERMINATED_DSCP_EXP,1,0,824,8},
               {SOC_PPC_FP_QUAL_IRPP_IN_DSCP_EXP,1,0,832,8},
               {SOC_PPC_FP_QUAL_IRPP_IN_TTL,1,0,840,8},
               {SOC_PPC_FP_QUAL_IPR2DSP_6EQ7_ESADI,1,0,848,1},
               {SOC_PPC_FP_QUAL_IPR2DSP_6EQ7_MPLS_EXP,1,0,856,3},
               {SOC_PPC_FP_QUAL_IRPP_PMF_PGM_KEY_GEN_VAR,1,0,864,32},
               {SOC_PPC_FP_QUAL_TERMINATED_PROTOCOL,1,0,896,2},
               {SOC_PPC_FP_QUAL_COS_PROFILE,1,0,904,6},
               {SOC_PPC_FP_QUAL_COUNTER_POINTER_A,1,0,912,21},
               {SOC_PPC_FP_QUAL_COUNTER_UPDATE_A,1,0,936,1},
               {SOC_PPC_FP_QUAL_COUNTER_POINTER_B,1,0,944,21},
               {SOC_PPC_FP_QUAL_COUNTER_UPDATE_B,1,0,968,1},
               {SOC_PPC_FP_QUAL_VTT_OAM_LIF_VALID,1,0,976,1},
               {SOC_PPC_FP_QUAL_VTT_OAM_LIF,1,0,984,18},
               {SOC_PPC_FP_QUAL_PROGRAM_INDEX,1,0,1008,5},
               {SOC_PPC_FP_QUAL_LL_LEM_1ST_LOOKUP_FOUND,0,1,0,1},
               {SOC_PPC_FP_QUAL_LL_LEM_1ST_LOOKUP_RESULT,0,1,8,45},
               {SOC_PPC_FP_QUAL_VT_ISA_KEY,0,1,56,50},
               {SOC_PPC_FP_QUAL_VT_PROCESSING_PROFILE,0,1,112,3},
               {SOC_PPC_FP_QUAL_VT_LOOKUP0_FOUND,0,1,120,1},
               {SOC_PPC_FP_QUAL_VT_LOOKUP0_PAYLOAD,0,1,128,17},
               {SOC_PPC_FP_QUAL_VT_LOOKUP1_FOUND,0,1,152,1},
               {SOC_PPC_FP_QUAL_VT_LOOKUP1_PAYLOAD,0,1,160,17},
               {SOC_PPC_FP_QUAL_TT_PROCESSING_PROFILE,0,1,184,3},
               {SOC_PPC_FP_QUAL_TT_LOOKUP0_FOUND,0,1,192,1},
               {SOC_PPC_FP_QUAL_TT_LOOKUP0_PAYLOAD,0,1,200,17},
               {SOC_PPC_FP_QUAL_TT_LOOKUP1_FOUND,0,1,224,1},
               {SOC_PPC_FP_QUAL_TT_LOOKUP1_PAYLOAD,0,1,232,17},
               {SOC_PPC_FP_QUAL_IRPP_IN_LIF_DATA,0,1,256,58},
               {SOC_PPC_FP_QUAL_IRPP_IN_LIF_DATA_INDEX,0,1,320,2},
               {SOC_PPC_FP_QUAL_IRPP_LOCAL_IN_LIF,0,1,328,17},
               {SOC_PPC_FP_QUAL_VTT_OAM_LIF_VALID, 1,0,976,1},
               {SOC_PPC_FP_QUAL_VTT_OAM_LIF,1,0,984,18},
        };


CONST STATIC 
    ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES
        Qax_ce_irpp_flp_info[] = {
               {SOC_PPC_FP_QUAL_IRPP_INVALID,1,1,0,0},
               {SOC_PPC_FP_QUAL_FWD_PRCESSING_PROFILE,1,1,0,3}, 
               {SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES,1,0,0,32},
               {SOC_PPC_FP_QUAL_IRPP_ALL_ONES,1,0,32,32},
               
               {SOC_PPC_FP_QUAL_IRPP_SRC_TM_PORT,1,0,72,8},
               {SOC_PPC_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR,1,0,80,16},
               {SOC_PPC_FP_QUAL_IRPP_SRC_PP_PORT,1,0,96,9},   
               {SOC_PPC_FP_QUAL_CPU2FLP_C_INTERNAL_FIELDS_DATA,1,0,112,32},
               {SOC_PPC_FP_QUAL_PACKET_HEADER_SIZE,1,0,144,7},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET0,1,0,152,7},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET1,1,0,160,7},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET2,1,0,168,7},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET3,1,0,176,7},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET4,1,0,184,7},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET5,1,0,192,7},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET_0_UNTIL_5,1,0,200-3,(8*6)}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1,1,0,200,11},
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_NEXT_PROTOCOL,1,0,200+7,4}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_ENCAPSULATION,1,0,200+5,2}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2,1,0,216,11},
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_BOS,1,0,216,1} ,
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_NEXT_PROTOCOL,1,0,216+7,4}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_IP_FRAGMENTED,1,0,216+6,1}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_IP_HAS_OPTIONS,1,0,216+4,1}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER3,1,0,232,11},
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER3_IP_FRAGMENTED,1,0,232+6,1}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER4,1,0,248,11},
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER5,1,0,264,11},
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER0,1,0,280,3},
               {SOC_PPC_FP_QUAL_IRPP_PKT_HDR_TYPE,1,0,288,6},
               {SOC_PPC_FP_QUAL_IRPP_SRC_SYST_PORT,1,0,296,16},
               {SOC_PPC_FP_QUAL_IRPP_PARSER_LEAF_CONTEXT,1,0,312,4},
               {SOC_PPC_FP_QUAL_SERVICE_TYPE,1,0,320,3},
               {SOC_PPC_FP_QUAL_IRPP_ETH_TAG_FORMAT,1,0,328,4},
               {SOC_PPC_FP_QUAL_FORWARDING_ACTION_STRENGTH,1,0,336,3},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DEST,1,0,344,19},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_TC,1,0,368,3},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DP,1,0,376,2},
               {SOC_PPC_FP_QUAL_FORWARDING_ACTION_METER_TRAFFIC_CLASS,1,0,384,2},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_CODE,1,0,392,8},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_QUAL,1,0,400,16},
               {SOC_PPC_FP_QUAL_IRPP_SNOOP_CODE,1,0,416,8},
               {SOC_PPC_FP_QUAL_SNOOP_STRENGTH,1,0,424,2},
               {SOC_PPC_FP_QUAL_IRPP_LL_MIRROR_CMD,1,0,432,4},
               {SOC_PPC_FP_QUAL_IRPP_UP,1,0,440,3},
               {SOC_PPC_FP_QUAL_IRPP_SYSTEM_VSI,1,0,448,16},
               {SOC_PPC_FP_QUAL_VSI_PROFILE,1,0,464+2,2}, 
               {SOC_PPC_FP_QUAL_VSI_UNKNOWN_DA_DESTINATION,1,0,472,19},
               {SOC_PPC_FP_QUAL_FID,1,0,496,15},
               {SOC_PPC_FP_QUAL_IRPP_ORIENTATION_IS_HUB,1,0,512,1},
               {SOC_PPC_FP_QUAL_I_SID,1,0,520,24},
               {SOC_PPC_FP_QUAL_IRPP_VLAN_ID,1,0,572,6},
               {SOC_PPC_FP_QUAL_IRPP_VLAN_PCP,1,0,569,3},
               {SOC_PPC_FP_QUAL_IRPP_VLAN_DEI,1,0,568,1},
               {SOC_PPC_FP_QUAL_VLAN_EDIT_CMD_VID2,1,0,556,12},
               {SOC_PPC_FP_QUAL_VLAN_EDIT_CMD_VID1,1,0,544,12},
               {SOC_PPC_FP_QUAL_TT_LEARN_ENABLE,1,0,584,1},
               {SOC_PPC_FP_QUAL_TT_LEARN_DATA,1,0,592,40},
               {SOC_PPC_FP_QUAL_IRPP_STP_STATE,1,0,632,2},
               {SOC_PPC_FP_QUAL_IRPP_FWD_TYPE,1,0,640,4},
               {SOC_PPC_FP_QUAL_IRPP_SUB_HEADER_NDX,1,0,648,3},
               {SOC_PPC_FP_QUAL_FORWARDING_OFFSET_EXTENSION,1,0,656,2},
               {SOC_PPC_FP_QUAL_IRPP_TERM_TYPE,1,0,664,4},
               {SOC_PPC_FP_QUAL_EEI,1,0,672,24},
               {SOC_PPC_FP_QUAL_OUT_LIF,1,0,696,18},
               {SOC_PPC_FP_QUAL_IRPP_IN_LIF,1,0,720,18},
               {SOC_PPC_FP_QUAL_IN_LIF_PROFILE,1,0,744,4},
               {SOC_PPC_FP_QUAL_IN_LIF_PROFILE_SAME_IF,1,0,744,1}, 
               {SOC_PPC_FP_QUAL_IN_LIF_UNKNOWN_DA_PROFILE,1,0,752,2},
               {SOC_PPC_FP_QUAL_IRPP_IN_RIF,1,0,760,15},
               {SOC_PPC_FP_QUAL_IRPP_IN_RIF_PROFILE,1,0,776,6},
               {SOC_PPC_FP_QUAL_IRPP_VRF,1,0,784,14},
               {SOC_PPC_FP_QUAL_IRPP_PCKT_IS_COMP_MC,1,0,800,1},
               {SOC_PPC_FP_QUAL_TERMINATED_TTL_VALID,1,0,808,1},
               {SOC_PPC_FP_QUAL_TERMINATED_TTL,1,0,816,8},
               {SOC_PPC_FP_QUAL_TERMINATED_DSCP_EXP,1,0,824,8},
               {SOC_PPC_FP_QUAL_IRPP_IN_DSCP_EXP,1,0,832,8},
               {SOC_PPC_FP_QUAL_IRPP_IN_TTL,1,0,840,8},
               {SOC_PPC_FP_QUAL_IPR2DSP_6EQ7_ESADI,1,0,848,1},
               {SOC_PPC_FP_QUAL_IPR2DSP_6EQ7_MPLS_EXP,1,0,856,3},
               {SOC_PPC_FP_QUAL_IRPP_PMF_PGM_KEY_GEN_VAR,1,0,864,32},
               {SOC_PPC_FP_QUAL_TERMINATED_PROTOCOL,1,0,896,2},
               {SOC_PPC_FP_QUAL_COS_PROFILE,1,0,904,6},
               {SOC_PPC_FP_QUAL_COUNTER_POINTER_A,1,0,912,21},
               {SOC_PPC_FP_QUAL_COUNTER_UPDATE_A,1,0,936,1},
               {SOC_PPC_FP_QUAL_COUNTER_POINTER_B,1,0,944,21},
               {SOC_PPC_FP_QUAL_COUNTER_UPDATE_B,1,0,968,1},
               {SOC_PPC_FP_QUAL_VTT_OAM_LIF_VALID,1,0,976,1},
               {SOC_PPC_FP_QUAL_VTT_OAM_LIF,1,0,984,18},
               {SOC_PPC_FP_QUAL_PROGRAM_INDEX,1,0,1008,5},
               {SOC_PPC_FP_QUAL_LL_LEM_1ST_LOOKUP_FOUND,0,1,0,1},
               {SOC_PPC_FP_QUAL_LL_LEM_1ST_LOOKUP_RESULT,0,1,8,45},
               {SOC_PPC_FP_QUAL_VT_ISA_KEY,0,1,56,50},
               {SOC_PPC_FP_QUAL_VT_PROCESSING_PROFILE,0,1,112,3},
               {SOC_PPC_FP_QUAL_VT_LOOKUP0_FOUND,0,1,120,1},
               {SOC_PPC_FP_QUAL_VT_LOOKUP0_PAYLOAD,0,1,128,17},
               {SOC_PPC_FP_QUAL_VT_LOOKUP1_FOUND,0,1,152,1},
               {SOC_PPC_FP_QUAL_VT_LOOKUP1_PAYLOAD,0,1,160,17},
               {SOC_PPC_FP_QUAL_TT_PROCESSING_PROFILE,0,1,184,3},
               {SOC_PPC_FP_QUAL_TT_LOOKUP0_FOUND,0,1,192,1},
               {SOC_PPC_FP_QUAL_TT_LOOKUP0_PAYLOAD,0,1,200,17},
               {SOC_PPC_FP_QUAL_TT_LOOKUP1_FOUND,0,1,224,1},
               {SOC_PPC_FP_QUAL_TT_LOOKUP1_PAYLOAD,0,1,232,17},
               {SOC_PPC_FP_QUAL_IRPP_IN_LIF_DATA,0,1,256,58},
               {SOC_PPC_FP_QUAL_IRPP_IN_LIF_DATA_INDEX,0,1,320,2},
               {SOC_PPC_FP_QUAL_IRPP_LOCAL_IN_LIF,0,1,328,17},
               {SOC_PPC_FP_QUAL_VTT_OAM_LIF_VALID, 1,0,976,1},
               {SOC_PPC_FP_QUAL_VTT_OAM_LIF,1,0,984,18},
        };

CONST STATIC 
    ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES
        Arad_plus_pmf_ce_irpp_slb_info[] = {
               {SOC_PPC_FP_QUAL_IRPP_INVALID,1,1,1,1},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET_0_UNTIL_5,1,0,0,(8*6)}, 
               {SOC_PPC_FP_QUAL_IRPP_SUB_HEADER_NDX,1,0,0,3},         
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_NEXT_PROTOCOL,1,0,148+7,4}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_NEXT_PROTOCOL,1,0,160+7,4}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER_NEXT_PROTOCOL_PLUS_ONE,1,0,160+7,4}, 
               {SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES,1,0,0,32},
               {SOC_PPC_FP_QUAL_IRPP_ALL_ONES,1,0,32,32},
               {SOC_PPC_FP_QUAL_IRPP_SRC_PP_PORT,1,0,64,8},
               {SOC_PPC_FP_QUAL_IRPP_PKT_HDR_TYPE,1,0,72,6},
               {SOC_PPC_FP_QUAL_IRPP_SRC_TM_PORT,1,0,80,8},
               {SOC_PPC_FP_QUAL_PACKET_HEADER_SIZE,1,0,88,7},
               {SOC_PPC_FP_QUAL_IRPP_SRC_SYST_PORT,1,0,96,16},
               {SOC_PPC_FP_QUAL_IRPP_PARSER_LEAF_CONTEXT,1,0,112,4},
               {SOC_PPC_FP_QUAL_IRPP_ETH_TAG_FORMAT,1,0,120,4},
               {SOC_PPC_FP_QUAL_FORWARDING_ACTION_STRENGTH,1,0,128,3},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DEST,1,0,136,19},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_TC,1,0,160,3},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DP,1,0,168,2},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_CODE,1,0,176,8},
               {SOC_PPC_FP_QUAL_FORWARDING_ACTION_METER_TRAFFIC_CLASS,1,0,184,2},
               {SOC_PPC_FP_QUAL_IRPP_SNOOP_CODE,1,0,192,8},
               {SOC_PPC_FP_QUAL_SNOOP_STRENGTH,1,0,200,2},
               {SOC_PPC_FP_QUAL_IRPP_LL_MIRROR_CMD,1,0,208,4},
               {SOC_PPC_FP_QUAL_IRPP_SYSTEM_VSI,1,0,216,16},
               {SOC_PPC_FP_QUAL_VSI_PROFILE,1,0,232+2,2}, 
               {SOC_PPC_FP_QUAL_FID,1,0,240,15},
               {SOC_PPC_FP_QUAL_IRPP_ORIENTATION_IS_HUB,1,0,256,1},
               {SOC_PPC_FP_QUAL_IRPP_STP_STATE,1,0,264,2},
               {SOC_PPC_FP_QUAL_IRPP_FWD_TYPE,1,0,272,4},
               {SOC_PPC_FP_QUAL_EEI,1,0,280,24},
               {SOC_PPC_FP_QUAL_OUT_LIF,1,0,304,16},
               {SOC_PPC_FP_QUAL_IRPP_IN_RIF,1,0,320,12},
               {SOC_PPC_FP_QUAL_IRPP_IN_LIF,1,0,336,16},
               {SOC_PPC_FP_QUAL_IN_LIF_PROFILE,1,0,352,4},
               {SOC_PPC_FP_QUAL_IN_LIF_PROFILE_SAME_IF,1,0,352,1}, 
               {SOC_PPC_FP_QUAL_LEM_2ND_LOOKUP_FOUND,1,0,360,1},
               {SOC_PPC_FP_QUAL_LEM_2ND_LOOKUP_RESULT,1,0,368,43},
               {SOC_PPC_FP_QUAL_LPM_1ST_LOOKUP_FOUND,1,0,416,1},
               {SOC_PPC_FP_QUAL_LPM_1ST_LOOKUP_RESULT,1,0,424,15},
               {SOC_PPC_FP_QUAL_LPM_2ND_LOOKUP_FOUND,1,0,440,1},
               {SOC_PPC_FP_QUAL_LPM_2ND_LOOKUP_RESULT,1,0,448,15},
               {SOC_PPC_FP_QUAL_TCAM_MATCH,1,0,464,1},
               {SOC_PPC_FP_QUAL_TCAM_RESULT,1,0,472,40},
               {SOC_PPC_FP_QUAL_TCAM_TRAPS0_MATCH,1,0,512,1},
               {SOC_PPC_FP_QUAL_TCAM_TRAPS0_RESULT,1,0,520,40},
               {SOC_PPC_FP_QUAL_TCAM_TRAPS1_MATCH,1,0,560,1},
               {SOC_PPC_FP_QUAL_TCAM_TRAPS1_RESULT,1,0,568,40},
               {SOC_PPC_FP_QUAL_TT_PROCESSING_PROFILE,1,0,608,3},
               {SOC_PPC_FP_QUAL_TT_LOOKUP0_FOUND,1,0,616,1},
               {SOC_PPC_FP_QUAL_TT_LOOKUP0_PAYLOAD,1,0,624,16},
               {SOC_PPC_FP_QUAL_TT_LOOKUP1_FOUND,1,0,640,1},
               {SOC_PPC_FP_QUAL_TT_LOOKUP1_PAYLOAD,1,0,648,16},
               {SOC_PPC_FP_QUAL_VT_PROCESSING_PROFILE,1,0,664,3},
               {SOC_PPC_FP_QUAL_VT_LOOKUP0_FOUND,1,0,672,1},
               {SOC_PPC_FP_QUAL_VT_LOOKUP0_PAYLOAD,1,0,680,16},
               {SOC_PPC_FP_QUAL_VT_LOOKUP1_FOUND,1,0,696,1},
               {SOC_PPC_FP_QUAL_VT_LOOKUP1_PAYLOAD,1,0,704,16},
               {SOC_PPC_FP_QUAL_IRPP_PMF_PGM_KEY_GEN_VAR,1,0,720,16},
               {SOC_PPC_FP_QUAL_PACKET_IS_IEEE1588,1,0,736,1},
               {SOC_PPC_FP_QUAL_IEEE1588_ENCAPSULATION,1,0,744,1},
               {SOC_PPC_FP_QUAL_IEEE1588_COMPENSATE_TIME_STAMP,1,0,752,1},
               {SOC_PPC_FP_QUAL_IEEE1588_COMMAND,1,0,760,2},
               {SOC_PPC_FP_QUAL_IEEE1588_HEADER_OFFSET,1,0,768,7},
               {SOC_PPC_FP_QUAL_OAM_UP_MEP,1,0,776,1},
               {SOC_PPC_FP_QUAL_OAM_SUB_TYPE,1,0,784,3},
               {SOC_PPC_FP_QUAL_OAM_OFFSET,1,0,792,7},
               {SOC_PPC_FP_QUAL_OAM_STAMP_OFFSET,1,0,800,7},
               {SOC_PPC_FP_QUAL_OAM_METER_DISABLE,1,0,808,1},
               {SOC_PPC_FP_QUAL_OAM_ID,1,0,816,17},
               {SOC_PPC_FP_QUAL_IRPP_VRF,1,0,840,12},
               {SOC_PPC_FP_QUAL_IRPP_IN_TTL,1,0,856,8},
               {SOC_PPC_FP_QUAL_IRPP_IN_DSCP_EXP,1,0,864,8},
               {SOC_PPC_FP_QUAL_UNKNOWN_ADDR,1,0,872,1},
               {SOC_PPC_FP_QUAL_FWD_PRCESSING_PROFILE,1,0,880,3},
               {SOC_PPC_FP_QUAL_ELK_ERROR,1,0,888,1},
               {SOC_PPC_FP_QUAL_ELK_LKP_PAYLOAD,1,0,896,128},
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_0,1,0,896+127,1},
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_1,1,0,896+126,1},
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_2,1,0,896+125,1},
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_3,1,0,896+124,1},
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_4,1,1,1,1}, 
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_5,1,1,1,1}, 
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_6,1,1,1,1}, 
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_7,1,1,1,1}, 
			    {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_0,1,0,896+120-48,48},
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_1,1,0,896+120-48-32,32},
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_2,1,0,896+120-48-32-16,16},
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_3,1,0,896+120-48-16-32-24,24},
               {SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES,0,1,0,32},
               {SOC_PPC_FP_QUAL_IRPP_ALL_ONES,0,1,32,32},
               {SOC_PPC_FP_QUAL_IRPP_SRC_PP_PORT,0,1,64,8},
               {SOC_PPC_FP_QUAL_IRPP_PKT_HDR_TYPE,0,1,72,6},
               {SOC_PPC_FP_QUAL_IRPP_SRC_TM_PORT,0,1,80,8},
               {SOC_PPC_FP_QUAL_IRPP_SRC_SYST_PORT,0,1,88,16},
               {SOC_PPC_FP_QUAL_IRPP_PARSER_LEAF_CONTEXT,0,1,104,4},
               {SOC_PPC_FP_QUAL_IRPP_ETH_TAG_FORMAT,0,1,112,4},
               {SOC_PPC_FP_QUAL_FORWARDING_ACTION_STRENGTH,0,1,120,3},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DEST,0,1,128,19},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_TC,0,1,152,3},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DP,0,1,160,2},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_CODE,0,1,168,8},
               {SOC_PPC_FP_QUAL_IRPP_SNOOP_CODE,0,1,176,8},
               {SOC_PPC_FP_QUAL_SNOOP_STRENGTH,0,1,184,2},
               {SOC_PPC_FP_QUAL_IRPP_SYSTEM_VSI,0,1,192,16},
               {SOC_PPC_FP_QUAL_VSI_PROFILE,0,1,208+2,2}, 
               {SOC_PPC_FP_QUAL_IRPP_FWD_TYPE,0,1,216,4},
               {SOC_PPC_FP_QUAL_EEI,0,1,224,24},
               {SOC_PPC_FP_QUAL_OUT_LIF,0,1,248,16},
               {SOC_PPC_FP_QUAL_IRPP_IN_RIF,0,1,264,12},
               {SOC_PPC_FP_QUAL_IRPP_IN_LIF,0,1,280,16},
               {SOC_PPC_FP_QUAL_IN_LIF_PROFILE,0,1,296,4},
               {SOC_PPC_FP_QUAL_IN_LIF_PROFILE_SAME_IF,0,1,296,1}, 
               {SOC_PPC_FP_QUAL_LEM_2ND_LOOKUP_FOUND,0,1,304,1},
               {SOC_PPC_FP_QUAL_LEM_2ND_LOOKUP_RESULT,0,1,312,43},
               {SOC_PPC_FP_QUAL_LPM_1ST_LOOKUP_FOUND,0,1,360,1},
               {SOC_PPC_FP_QUAL_LPM_1ST_LOOKUP_RESULT,0,1,368,15},
               {SOC_PPC_FP_QUAL_LPM_2ND_LOOKUP_FOUND,0,1,384,1},
               {SOC_PPC_FP_QUAL_LPM_2ND_LOOKUP_RESULT,0,1,392,15},
               {SOC_PPC_FP_QUAL_TCAM_MATCH,0,1,408,1},
               {SOC_PPC_FP_QUAL_TCAM_RESULT,0,1,416,40},
               {SOC_PPC_FP_QUAL_TCAM_TRAPS0_MATCH,0,1,456,1},
               {SOC_PPC_FP_QUAL_TCAM_TRAPS0_RESULT,0,1,464,40},
               {SOC_PPC_FP_QUAL_TCAM_TRAPS1_MATCH,0,1,504,1},
               {SOC_PPC_FP_QUAL_TCAM_TRAPS1_RESULT,1,0,512,40},
               {SOC_PPC_FP_QUAL_TT_PROCESSING_PROFILE,1,0,552,3},
               {SOC_PPC_FP_QUAL_TT_LOOKUP0_FOUND,0,1,560,1},
               {SOC_PPC_FP_QUAL_TT_LOOKUP0_PAYLOAD,0,1,568,16},
               {SOC_PPC_FP_QUAL_TT_LOOKUP1_FOUND,0,1,584,1},
               {SOC_PPC_FP_QUAL_TT_LOOKUP1_PAYLOAD,0,1,592,16},
               {SOC_PPC_FP_QUAL_VT_PROCESSING_PROFILE,0,1,608,3},
               {SOC_PPC_FP_QUAL_VT_LOOKUP0_FOUND,0,1,616,1},
               {SOC_PPC_FP_QUAL_VT_LOOKUP0_PAYLOAD,0,1,624,16},
               {SOC_PPC_FP_QUAL_VT_LOOKUP1_FOUND,0,1,640,1},
               {SOC_PPC_FP_QUAL_VT_LOOKUP1_PAYLOAD,0,1,648,16},
               {SOC_PPC_FP_QUAL_IRPP_PMF_PGM_KEY_GEN_VAR,0,1,664,16},
               {SOC_PPC_FP_QUAL_KEY_AFTER_HASHING,0,1,680,160},
               {SOC_PPC_FP_QUAL_IS_FEC_DEST_14_0,0,1,840,16},
               {SOC_PPC_FP_QUAL_IRPP_VRF,0,1,856,12},
               {SOC_PPC_FP_QUAL_IRPP_IN_DSCP_EXP,0,1,872,8},
               {SOC_PPC_FP_QUAL_FWD_PRCESSING_PROFILE,0,1,880,3},
               {SOC_PPC_FP_QUAL_ELK_ERROR,0,1,888,1},
               {SOC_PPC_FP_QUAL_ELK_LKP_PAYLOAD,0,1,896,128},
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_4,1,1,1,1}, 
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_5,1,1,1,1}, 
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_6,1,1,1,1}, 
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_7,1,1,1,1}, 
               {SOC_PPC_FP_QUAL_IRPP_RPF_NATIVE_VSI,1,1,1,15}, 
               {SOC_PPC_FP_QUAL_OAM_UP_MEP,1,0,776,1},
               {SOC_PPC_FP_QUAL_OAM_SUB_TYPE,1,0,784,3},
               {SOC_PPC_FP_QUAL_OAM_OFFSET,1,0,792,7},
               {SOC_PPC_FP_QUAL_OAM_STAMP_OFFSET,1,0,800,7},
               {SOC_PPC_FP_QUAL_OAM_ID,1,0,816,17},
               {SOC_PPC_FP_QUAL_OAM_METER_DISABLE,1,0,808,1},
               {SOC_PPC_FP_QUAL_IRPP_IN_RIF_PROFILE,1,1,1,1}, 
        };

CONST STATIC 
    ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES
        Jericho_pmf_ce_irpp_slb_info[] = {
              {SOC_PPC_FP_QUAL_IRPP_INVALID,1,1,1,1},

              {SOC_PPC_FP_QUAL_HEADER_OFFSET_0_UNTIL_5,1,0,0,(8*6)}, 
              {SOC_PPC_FP_QUAL_IRPP_SUB_HEADER_NDX,1,0,0,3},         
              {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_NEXT_PROTOCOL,1,0,148+7,4}, 
              {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_NEXT_PROTOCOL,1,0,160+7,4}, 
              {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER_NEXT_PROTOCOL_PLUS_ONE,1,0,160+7,4}, 

              {SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES,1,0,0,32},
              {SOC_PPC_FP_QUAL_IRPP_ALL_ONES,1,0,32,32},
              
              {SOC_PPC_FP_QUAL_IRPP_SRC_PP_PORT,1,0,72,9},    
              {SOC_PPC_FP_QUAL_IRPP_PKT_HDR_TYPE,1,0,84,6},
              {SOC_PPC_FP_QUAL_IRPP_SRC_TM_PORT,1,0,92,8},
              {SOC_PPC_FP_QUAL_PACKET_HEADER_SIZE,1,0,100,7},
              {SOC_PPC_FP_QUAL_IRPP_SRC_SYST_PORT,1,0,108,16},
              {SOC_PPC_FP_QUAL_IRPP_PARSER_LEAF_CONTEXT,1,0,124,4},
              {SOC_PPC_FP_QUAL_IRPP_ETH_TAG_FORMAT,1,0,128,4},
              {SOC_PPC_FP_QUAL_FORWARDING_ACTION_STRENGTH,1,0,132,3},
              {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DEST,1,0,136,19},
              {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_TC,1,0,156,3},
              {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DP,1,0,160,2},
              {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_CODE,1,0,164,8},
              {SOC_PPC_FP_QUAL_FORWARDING_ACTION_METER_TRAFFIC_CLASS,1,0,172,2},
              {SOC_PPC_FP_QUAL_IRPP_SNOOP_CODE,1,0,176,8},
              {SOC_PPC_FP_QUAL_SNOOP_STRENGTH,1,0,184,2},
              {SOC_PPC_FP_QUAL_IRPP_LL_MIRROR_CMD,1,0,188,4},
              {SOC_PPC_FP_QUAL_IRPP_SYSTEM_VSI,1,0,192,16},
              {SOC_PPC_FP_QUAL_VSI_PROFILE,1,0,208,4},
              {SOC_PPC_FP_QUAL_FID,1,0,212,15},
              {SOC_PPC_FP_QUAL_IRPP_ORIENTATION_IS_HUB,1,0,228,1},
              {SOC_PPC_FP_QUAL_IRPP_STP_STATE,1,0,232,2},
              {SOC_PPC_FP_QUAL_IRPP_FWD_TYPE,1,0,236,4},
              {SOC_PPC_FP_QUAL_EEI,1,0,240,24},
              {SOC_PPC_FP_QUAL_OUT_LIF,1,0,264,18},
              {SOC_PPC_FP_QUAL_IRPP_IN_RIF,1,0,284,15},
              {SOC_PPC_FP_QUAL_IRPP_IN_LIF,1,0,304,18},
              {SOC_PPC_FP_QUAL_IN_LIF_PROFILE,1,0,324,4},
              {SOC_PPC_FP_QUAL_IN_LIF_PROFILE_SAME_IF,1,0,324,1}, 
              {SOC_PPC_FP_QUAL_LEM_2ND_LOOKUP_FOUND,1,0,328,1},
              {SOC_PPC_FP_QUAL_LEM_2ND_LOOKUP_RESULT,1,0,336,45},
              {SOC_PPC_FP_QUAL_LPM_1ST_LOOKUP_FOUND,1,0,384,1},
              {SOC_PPC_FP_QUAL_LPM_1ST_LOOKUP_RESULT,1,0,392,20},
              {SOC_PPC_FP_QUAL_LPM_2ND_LOOKUP_FOUND,1,0,412,1},
              {SOC_PPC_FP_QUAL_LPM_2ND_LOOKUP_RESULT,1,0,416,20},
              {SOC_PPC_FP_QUAL_IRPP_TCAM0_MATCH,1,0,436,1},
              {SOC_PPC_FP_QUAL_TCAM_MATCH,1,0,436,1},
              {SOC_PPC_FP_QUAL_IRPP_TCAM0_RESULT,1,0,440,48},
              {SOC_PPC_FP_QUAL_TCAM_RESULT,1,0,440,48},
              {SOC_PPC_FP_QUAL_TCAM_TRAPS0_MATCH,1,0,488,1},
              {SOC_PPC_FP_QUAL_TCAM_TRAPS0_RESULT,1,0,496,48},
              {SOC_PPC_FP_QUAL_TCAM_TRAPS1_MATCH,1,0,544,1},
              {SOC_PPC_FP_QUAL_TCAM_TRAPS1_RESULT,1,0,552,48},
              {SOC_PPC_FP_QUAL_TT_PROCESSING_PROFILE,1,0,600,3},
              {SOC_PPC_FP_QUAL_TT_LOOKUP0_FOUND,1,0,604,1},
              {SOC_PPC_FP_QUAL_TT_LOOKUP0_PAYLOAD,1,0,608,17},
              {SOC_PPC_FP_QUAL_TT_LOOKUP1_FOUND,1,0,628,1},
              {SOC_PPC_FP_QUAL_TT_LOOKUP1_PAYLOAD,1,0,632,17},
              {SOC_PPC_FP_QUAL_VT_PROCESSING_PROFILE,1,0,652,3},
              {SOC_PPC_FP_QUAL_VT_LOOKUP0_FOUND,1,0,656,1},
              {SOC_PPC_FP_QUAL_VT_LOOKUP0_PAYLOAD,1,0,664,17},
              {SOC_PPC_FP_QUAL_VT_LOOKUP1_FOUND,1,0,684,1},
              {SOC_PPC_FP_QUAL_VT_LOOKUP1_PAYLOAD,1,0,688,17},
              {SOC_PPC_FP_QUAL_IRPP_CONSISTENT_HASHING_PGM_KEY_GEN_VAR,1,0,708,16},
              {SOC_PPC_FP_QUAL_PACKET_IS_IEEE1588,1,0,724,1},
              {SOC_PPC_FP_QUAL_IEEE1588_ENCAPSULATION,1,0,728,1},
              {SOC_PPC_FP_QUAL_IEEE1588_COMPENSATE_TIME_STAMP,1,0,732,1},
              {SOC_PPC_FP_QUAL_IEEE1588_COMMAND,1,0,736,2},
              {SOC_PPC_FP_QUAL_IEEE1588_HEADER_OFFSET,1,0,740,7},
              {SOC_PPC_FP_QUAL_OAM_UP_MEP,1,0,748,1},
              {SOC_PPC_FP_QUAL_OAM_SUB_TYPE,1,0,752,3},
              {SOC_PPC_FP_QUAL_OAM_OFFSET,1,0,756,7},
              {SOC_PPC_FP_QUAL_OAM_STAMP_OFFSET,1,0,764,7},
              {SOC_PPC_FP_QUAL_OAM_METER_DISABLE,1,0,772,1},
              {SOC_PPC_FP_QUAL_OAM_ID,1,0,776,17},
              {SOC_PPC_FP_QUAL_IRPP_VRF,1,0,796,14},
              {SOC_PPC_FP_QUAL_IRPP_IN_TTL,1,0,812,8},
              {SOC_PPC_FP_QUAL_IRPP_IN_DSCP_EXP,1,0,820,8},
              {SOC_PPC_FP_QUAL_UNKNOWN_ADDR,1,0,828,1},
              {SOC_PPC_FP_QUAL_FWD_PRCESSING_PROFILE,1,0,832,3},
              {SOC_PPC_FP_QUAL_ELK_ERROR,1,0,836,1},
              {SOC_PPC_FP_QUAL_ELK_LKP_PAYLOAD,1,0,840,128},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_0,1,0,840+127,1},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_1,1,0,840+126,1},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_2,1,0,840+125,1},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_3,1,0,840+124,1},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_4,1,0,840+123,1},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_5,1,0,840+122,1},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_6,1,0,840+121,1},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_7,1,0,840+120,1},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_0,1,0,840+120-48,48},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_1,1,0,840+120-48-32,32},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_2,1,0,840+120-48-32-16,16},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_3,1,0,840+120-48-16-32-24,24},
              {SOC_PPC_FP_QUAL_IRPP_TCAM1_MATCH,1,0,968,1},
              {SOC_PPC_FP_QUAL_IRPP_TCAM1_RESULT,1,0,976,48},
              {SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES,0,1,0,32},
              {SOC_PPC_FP_QUAL_IRPP_ALL_ONES,0,1,32,32},
              {SOC_PPC_FP_QUAL_IRPP_SRC_PP_PORT,0,1,64,8},
              {SOC_PPC_FP_QUAL_IRPP_PKT_HDR_TYPE,0,1,72,6},
              {SOC_PPC_FP_QUAL_IRPP_SRC_TM_PORT,0,1,80,8},
              {SOC_PPC_FP_QUAL_IRPP_SRC_SYST_PORT,0,1,88,16},
              {SOC_PPC_FP_QUAL_IRPP_PARSER_LEAF_CONTEXT,0,1,104,4},
              {SOC_PPC_FP_QUAL_IRPP_ETH_TAG_FORMAT,0,1,108,4},
              {SOC_PPC_FP_QUAL_FORWARDING_ACTION_STRENGTH,0,1,112,3},
              {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DEST,0,1,120,19},
              {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_TC,0,1,140,3},
              {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DP,0,1,144,2},
              {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_CODE,0,1,148,8},
              {SOC_PPC_FP_QUAL_IRPP_SNOOP_CODE,0,1,156,8},
              {SOC_PPC_FP_QUAL_SNOOP_STRENGTH,0,1,164,2},
              {SOC_PPC_FP_QUAL_IRPP_SYSTEM_VSI,0,1,168,16},
              {SOC_PPC_FP_QUAL_VSI_PROFILE,0,1,184+2,2}, 
              {SOC_PPC_FP_QUAL_IRPP_FWD_TYPE,0,1,188,4},
              {SOC_PPC_FP_QUAL_EEI,0,1,192,24},
              {SOC_PPC_FP_QUAL_OUT_LIF,0,1,216,18},
              {SOC_PPC_FP_QUAL_IRPP_IN_RIF,0,1,236,15},
              {SOC_PPC_FP_QUAL_IRPP_IN_LIF,0,1,256,18},
              {SOC_PPC_FP_QUAL_IN_LIF_PROFILE,0,1,276,4},
              {SOC_PPC_FP_QUAL_IN_LIF_PROFILE_SAME_IF,0,1,276,1}, 
              {SOC_PPC_FP_QUAL_LEM_2ND_LOOKUP_FOUND,0,1,280,1},
              {SOC_PPC_FP_QUAL_LEM_2ND_LOOKUP_RESULT,0,1,288,45},
              {SOC_PPC_FP_QUAL_LPM_1ST_LOOKUP_FOUND,0,1,336,1},
              {SOC_PPC_FP_QUAL_LPM_1ST_LOOKUP_RESULT,0,1,344,20},
              {SOC_PPC_FP_QUAL_LPM_2ND_LOOKUP_FOUND,0,1,364,1},
              {SOC_PPC_FP_QUAL_LPM_2ND_LOOKUP_RESULT,0,1,368,20},
              {SOC_PPC_FP_QUAL_IRPP_TCAM0_MATCH,0,1,388,1},
              {SOC_PPC_FP_QUAL_TCAM_MATCH,0,1,388,1},
              {SOC_PPC_FP_QUAL_IRPP_TCAM0_RESULT,0,1,392,48},
              {SOC_PPC_FP_QUAL_TCAM_RESULT,0,1,392,48},
              {SOC_PPC_FP_QUAL_TCAM_TRAPS0_MATCH,0,1,440,1},
              {SOC_PPC_FP_QUAL_TCAM_TRAPS0_RESULT,0,1,448,48},
              {SOC_PPC_FP_QUAL_TCAM_TRAPS1_MATCH,0,1,496,1},
              {SOC_PPC_FP_QUAL_TCAM_TRAPS1_RESULT,0,1,504,48},
              {SOC_PPC_FP_QUAL_TT_PROCESSING_PROFILE,0,1,552,3},
              {SOC_PPC_FP_QUAL_TT_LOOKUP0_FOUND,0,1,556,1},
              {SOC_PPC_FP_QUAL_TT_LOOKUP0_PAYLOAD,0,1,560,17},
              {SOC_PPC_FP_QUAL_TT_LOOKUP1_FOUND,0,1,580,1},
              {SOC_PPC_FP_QUAL_TT_LOOKUP1_PAYLOAD,0,1,584,17},
              {SOC_PPC_FP_QUAL_VT_PROCESSING_PROFILE,0,1,604,3},
              {SOC_PPC_FP_QUAL_VT_LOOKUP0_FOUND,0,1,608,1},
              {SOC_PPC_FP_QUAL_VT_LOOKUP0_PAYLOAD,0,1,616,17},
              {SOC_PPC_FP_QUAL_VT_LOOKUP1_FOUND,0,1,636,1},
              {SOC_PPC_FP_QUAL_VT_LOOKUP1_PAYLOAD,0,1,640,17},
              {SOC_PPC_FP_QUAL_IRPP_CONSISTENT_HASHING_PGM_KEY_GEN_VAR,0,1,660,16},
              {SOC_PPC_FP_QUAL_KEY_AFTER_HASHING,0,1,680,160},
              {SOC_PPC_FP_QUAL_IS_FEC_DEST_14_0,0,1,840,17},
              {SOC_PPC_FP_QUAL_IRPP_VRF,0,1,860,14},
              {SOC_PPC_FP_QUAL_IRPP_IN_DSCP_EXP,0,1,876,8},
              {SOC_PPC_FP_QUAL_FWD_PRCESSING_PROFILE,0,1,884,3},
              {SOC_PPC_FP_QUAL_ELK_ERROR,0,1,888,1},
              {SOC_PPC_FP_QUAL_IRPP_ELK_LKP_PAYLOAD_LSB,0,1,896,128},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_4,0,1,896+128-32,32},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_5,0,1,896+128-32-32,32},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_6,0,1,896+128-32-32-24,24},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_7,0,1,896+128-32-32-24-16,16},
              {SOC_PPC_FP_QUAL_OAM_UP_MEP,1,0,748,1},
              {SOC_PPC_FP_QUAL_OAM_SUB_TYPE,1,0,752,3},
              {SOC_PPC_FP_QUAL_OAM_OFFSET,1,0,756,7},
              {SOC_PPC_FP_QUAL_OAM_STAMP_OFFSET,1,0,764,7},
              {SOC_PPC_FP_QUAL_OAM_ID,1,0,776,17},
              {SOC_PPC_FP_QUAL_OAM_METER_DISABLE,1,0,772,1},

        };

CONST STATIC 
    ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES
        Qax_pmf_ce_irpp_slb_info[] = {
              {SOC_PPC_FP_QUAL_IRPP_INVALID,1,1,1,1},
              {SOC_PPC_FP_QUAL_HEADER_OFFSET_0_UNTIL_5,1,0,0,(8*6)}, 
              {SOC_PPC_FP_QUAL_IRPP_SUB_HEADER_NDX,1,0,0,3},         
              {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_NEXT_PROTOCOL,1,0,148+7,4}, 
              {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_NEXT_PROTOCOL,1,0,160+7,4}, 
              {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER_NEXT_PROTOCOL_PLUS_ONE,1,0,160+7,4}, 
              {SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES,1,0,0,32},
              {SOC_PPC_FP_QUAL_IRPP_ALL_ONES,1,0,32,32},
              
              {SOC_PPC_FP_QUAL_IRPP_SRC_PP_PORT,1,0,72,9},    
              {SOC_PPC_FP_QUAL_IRPP_PKT_HDR_TYPE,1,0,84,6},
              {SOC_PPC_FP_QUAL_IRPP_SRC_TM_PORT,1,0,92,8},
              {SOC_PPC_FP_QUAL_PACKET_HEADER_SIZE,1,0,100,7},
              {SOC_PPC_FP_QUAL_IRPP_SRC_SYST_PORT,1,0,108,16},
              {SOC_PPC_FP_QUAL_IRPP_PARSER_LEAF_CONTEXT,1,0,124,4},
              {SOC_PPC_FP_QUAL_IRPP_ETH_TAG_FORMAT,1,0,128,4},
              {SOC_PPC_FP_QUAL_FORWARDING_ACTION_STRENGTH,1,0,132,3},
              {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DEST,1,0,136,19},
              {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_TC,1,0,156,3},
              {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DP,1,0,160,2},
              {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_CODE,1,0,164,8},
              {SOC_PPC_FP_QUAL_FORWARDING_ACTION_METER_TRAFFIC_CLASS,1,0,172,2},
              {SOC_PPC_FP_QUAL_IRPP_SNOOP_CODE,1,0,176,8},
              {SOC_PPC_FP_QUAL_SNOOP_STRENGTH,1,0,184,2},
              {SOC_PPC_FP_QUAL_IRPP_LL_MIRROR_CMD,1,0,188,4},
              {SOC_PPC_FP_QUAL_IRPP_SYSTEM_VSI,1,0,192,16},
              {SOC_PPC_FP_QUAL_VSI_PROFILE,1,0,208,4},
              {SOC_PPC_FP_QUAL_FID,1,0,212,15},
              {SOC_PPC_FP_QUAL_IRPP_ORIENTATION_IS_HUB,1,0,228,1},
              {SOC_PPC_FP_QUAL_IRPP_STP_STATE,1,0,232,2},
              {SOC_PPC_FP_QUAL_IRPP_FWD_TYPE,1,0,236,4},
              {SOC_PPC_FP_QUAL_EEI,1,0,240,24},
              {SOC_PPC_FP_QUAL_OUT_LIF,1,0,264,18},
              {SOC_PPC_FP_QUAL_IRPP_IN_RIF,1,0,284,15},
              {SOC_PPC_FP_QUAL_IRPP_IN_LIF,1,0,304,18},
              {SOC_PPC_FP_QUAL_IN_LIF_PROFILE,1,0,324,4},
              {SOC_PPC_FP_QUAL_IN_LIF_PROFILE_SAME_IF,1,0,324,1}, 
              {SOC_PPC_FP_QUAL_LEM_2ND_LOOKUP_FOUND,1,0,328,1},
              {SOC_PPC_FP_QUAL_LEM_2ND_LOOKUP_RESULT,1,0,336,45},
              {SOC_PPC_FP_QUAL_LPM_1ST_LOOKUP_FOUND,1,0,384,1},
              {SOC_PPC_FP_QUAL_LPM_1ST_LOOKUP_RESULT,1,0,392,20},
              {SOC_PPC_FP_QUAL_LPM_2ND_LOOKUP_FOUND,1,0,412,1},
              {SOC_PPC_FP_QUAL_LPM_2ND_LOOKUP_RESULT,1,0,416,20},
              {SOC_PPC_FP_QUAL_IRPP_TCAM0_MATCH,1,0,436,1},
              {SOC_PPC_FP_QUAL_TCAM_MATCH,1,0,436,1},
              {SOC_PPC_FP_QUAL_IRPP_TCAM0_RESULT,1,0,440,48},
              {SOC_PPC_FP_QUAL_TCAM_RESULT,1,0,440,48},
              {SOC_PPC_FP_QUAL_TCAM_TRAPS0_MATCH,1,0,488,1},
              {SOC_PPC_FP_QUAL_TCAM_TRAPS0_RESULT,1,0,496,48},
              {SOC_PPC_FP_QUAL_TCAM_TRAPS1_MATCH,1,0,544,1},
              {SOC_PPC_FP_QUAL_TCAM_TRAPS1_RESULT,1,0,552,48},
              {SOC_PPC_FP_QUAL_TT_PROCESSING_PROFILE,1,0,600,3},
              {SOC_PPC_FP_QUAL_TT_LOOKUP0_FOUND,1,0,604,1},
              {SOC_PPC_FP_QUAL_TT_LOOKUP0_PAYLOAD,1,0,608,17},
              {SOC_PPC_FP_QUAL_TT_LOOKUP1_FOUND,1,0,628,1},
              {SOC_PPC_FP_QUAL_TT_LOOKUP1_PAYLOAD,1,0,632,17},
              {SOC_PPC_FP_QUAL_VT_PROCESSING_PROFILE,1,0,652,3},
              {SOC_PPC_FP_QUAL_VT_LOOKUP0_FOUND,1,0,656,1},
              {SOC_PPC_FP_QUAL_VT_LOOKUP0_PAYLOAD,1,0,664,17},
              {SOC_PPC_FP_QUAL_VT_LOOKUP1_FOUND,1,0,684,1},
              {SOC_PPC_FP_QUAL_VT_LOOKUP1_PAYLOAD,1,0,688,17},
              {SOC_PPC_FP_QUAL_IRPP_CONSISTENT_HASHING_PGM_KEY_GEN_VAR,1,0,708,16},
              {SOC_PPC_FP_QUAL_PACKET_IS_IEEE1588,1,0,724,1},
              {SOC_PPC_FP_QUAL_IEEE1588_ENCAPSULATION,1,0,728,1},
              {SOC_PPC_FP_QUAL_IEEE1588_COMPENSATE_TIME_STAMP,1,0,732,1},
              {SOC_PPC_FP_QUAL_IEEE1588_COMMAND,1,0,736,2},
              {SOC_PPC_FP_QUAL_IEEE1588_HEADER_OFFSET,1,0,740,7},
              {SOC_PPC_FP_QUAL_OAM_UP_MEP,1,0,748,1},
              {SOC_PPC_FP_QUAL_OAM_SUB_TYPE,1,0,752,3},
              {SOC_PPC_FP_QUAL_OAM_OFFSET,1,0,756,7},
              {SOC_PPC_FP_QUAL_OAM_STAMP_OFFSET,1,0,764,7},
              {SOC_PPC_FP_QUAL_OAM_METER_DISABLE,1,0,772,1},
              {SOC_PPC_FP_QUAL_OAM_ID,1,0,776,17},
              {SOC_PPC_FP_QUAL_IRPP_VRF,1,0,796,14},
              {SOC_PPC_FP_QUAL_IRPP_IN_TTL,1,0,812,8},
              {SOC_PPC_FP_QUAL_IRPP_IN_DSCP_EXP,1,0,820,8},
              {SOC_PPC_FP_QUAL_UNKNOWN_ADDR,1,0,828,1},
              {SOC_PPC_FP_QUAL_FWD_PRCESSING_PROFILE,1,0,832,3},
              {SOC_PPC_FP_QUAL_ELK_ERROR,1,0,836,1},
              {SOC_PPC_FP_QUAL_ELK_LKP_PAYLOAD,1,0,840,128},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_0,1,0,840+127,1},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_1,1,0,840+126,1},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_2,1,0,840+125,1},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_3,1,0,840+124,1},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_4,1,0,840+123,1},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_5,1,0,840+122,1},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_6,1,0,840+121,1},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_7,1,0,840+120,1},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_0,1,0,840+120-48,48},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_1,1,0,840+120-48-32,32},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_2,1,0,840+120-48-32-16,16},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_3,1,0,840+120-48-16-32-24,24},
              {SOC_PPC_FP_QUAL_IRPP_TCAM1_MATCH,1,0,968,1},
              {SOC_PPC_FP_QUAL_IRPP_TCAM1_RESULT,1,0,976,48},
              {SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES,0,1,0,32},
              {SOC_PPC_FP_QUAL_IRPP_ALL_ONES,0,1,32,32},
              {SOC_PPC_FP_QUAL_IRPP_SRC_PP_PORT,0,1,64,8},
              {SOC_PPC_FP_QUAL_IRPP_PKT_HDR_TYPE,0,1,72,6},
              {SOC_PPC_FP_QUAL_IRPP_SRC_TM_PORT,0,1,80,8},
              {SOC_PPC_FP_QUAL_IRPP_SRC_SYST_PORT,0,1,88,16},
              {SOC_PPC_FP_QUAL_IRPP_PARSER_LEAF_CONTEXT,0,1,104,4},
              {SOC_PPC_FP_QUAL_IRPP_ETH_TAG_FORMAT,0,1,108,4},
              {SOC_PPC_FP_QUAL_FORWARDING_ACTION_STRENGTH,0,1,112,3},
              {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DEST,0,1,120,19},
              {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_TC,0,1,140,3},
              {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DP,0,1,144,2},
              {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_CODE,0,1,148,8},
              {SOC_PPC_FP_QUAL_IRPP_SNOOP_CODE,0,1,156,8},
              {SOC_PPC_FP_QUAL_SNOOP_STRENGTH,0,1,164,2},
              {SOC_PPC_FP_QUAL_IRPP_SYSTEM_VSI,0,1,168,16},
              {SOC_PPC_FP_QUAL_VSI_PROFILE,0,1,184+2,2}, 
              {SOC_PPC_FP_QUAL_IRPP_FWD_TYPE,0,1,188,4},
              {SOC_PPC_FP_QUAL_EEI,0,1,192,24},
              {SOC_PPC_FP_QUAL_OUT_LIF,0,1,216,18},
              {SOC_PPC_FP_QUAL_IRPP_IN_RIF,0,1,236,15},
              {SOC_PPC_FP_QUAL_IRPP_IN_LIF,0,1,256,18},
              {SOC_PPC_FP_QUAL_IN_LIF_PROFILE,0,1,276,4},
              {SOC_PPC_FP_QUAL_IN_LIF_PROFILE_SAME_IF,0,1,276,1}, 
              {SOC_PPC_FP_QUAL_LEM_2ND_LOOKUP_FOUND,0,1,280,1},
              {SOC_PPC_FP_QUAL_LEM_2ND_LOOKUP_RESULT,0,1,288,45},
              {SOC_PPC_FP_QUAL_LPM_1ST_LOOKUP_FOUND,0,1,336,1},
              {SOC_PPC_FP_QUAL_LPM_1ST_LOOKUP_RESULT,0,1,344,20},
              {SOC_PPC_FP_QUAL_LPM_2ND_LOOKUP_FOUND,0,1,364,1},
              {SOC_PPC_FP_QUAL_LPM_2ND_LOOKUP_RESULT,0,1,368,20},
              {SOC_PPC_FP_QUAL_IRPP_TCAM0_MATCH,0,1,388,1},
              {SOC_PPC_FP_QUAL_TCAM_MATCH,0,1,388,1},
              {SOC_PPC_FP_QUAL_IRPP_TCAM0_RESULT,0,1,392,48},
              {SOC_PPC_FP_QUAL_TCAM_RESULT,0,1,392,48},
              {SOC_PPC_FP_QUAL_TCAM_TRAPS0_MATCH,0,1,440,1},
              {SOC_PPC_FP_QUAL_TCAM_TRAPS0_RESULT,0,1,448,48},
              {SOC_PPC_FP_QUAL_TCAM_TRAPS1_MATCH,0,1,496,1},
              {SOC_PPC_FP_QUAL_TCAM_TRAPS1_RESULT,0,1,504,48},
              {SOC_PPC_FP_QUAL_TT_PROCESSING_PROFILE,0,1,552,3},
              {SOC_PPC_FP_QUAL_TT_LOOKUP0_FOUND,0,1,556,1},
              {SOC_PPC_FP_QUAL_TT_LOOKUP0_PAYLOAD,0,1,560,17},
              {SOC_PPC_FP_QUAL_TT_LOOKUP1_FOUND,0,1,580,1},
              {SOC_PPC_FP_QUAL_TT_LOOKUP1_PAYLOAD,0,1,584,17},
              {SOC_PPC_FP_QUAL_VT_PROCESSING_PROFILE,0,1,604,3},
              {SOC_PPC_FP_QUAL_VT_LOOKUP0_FOUND,0,1,608,1},
              {SOC_PPC_FP_QUAL_VT_LOOKUP0_PAYLOAD,0,1,616,17},
              {SOC_PPC_FP_QUAL_VT_LOOKUP1_FOUND,0,1,636,1},
              {SOC_PPC_FP_QUAL_VT_LOOKUP1_PAYLOAD,0,1,640,17},
              {SOC_PPC_FP_QUAL_IRPP_CONSISTENT_HASHING_PGM_KEY_GEN_VAR,0,1,660,16},
              {SOC_PPC_FP_QUAL_KEY_AFTER_HASHING,0,1,680,160},
              {SOC_PPC_FP_QUAL_IS_FEC_DEST_14_0,0,1,840,17},
              {SOC_PPC_FP_QUAL_IRPP_VRF,0,1,860,14},
              {SOC_PPC_FP_QUAL_IRPP_IN_DSCP_EXP,0,1,876,8},
              {SOC_PPC_FP_QUAL_FWD_PRCESSING_PROFILE,0,1,884,3},
              {SOC_PPC_FP_QUAL_ELK_ERROR,0,1,888,1},
              {SOC_PPC_FP_QUAL_IRPP_ELK_LKP_PAYLOAD_LSB,0,1,896,128},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_4,0,1,896+128-32,32},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_5,0,1,896+128-32-32,32},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_6,0,1,896+128-32-32-24,24},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_7,0,1,896+128-32-32-24-16,16},
              {SOC_PPC_FP_QUAL_OAM_UP_MEP,1,0,748,1},
              {SOC_PPC_FP_QUAL_OAM_SUB_TYPE,1,0,752,3},
              {SOC_PPC_FP_QUAL_OAM_OFFSET,1,0,756,7},
              {SOC_PPC_FP_QUAL_OAM_STAMP_OFFSET,1,0,764,7},
              {SOC_PPC_FP_QUAL_OAM_ID,1,0,776,17},
              {SOC_PPC_FP_QUAL_OAM_METER_DISABLE,1,0,772,1},

        };



CONST STATIC 
    ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES
        Arad_pmf_ce_irpp_pmf_info[] = {
               {SOC_PPC_FP_QUAL_IRPP_INVALID,1,1,0,0},
	       {SOC_PPC_FP_QUAL_OUT_LIF_RANGE,1,1,0,2}, 
               {SOC_PPC_FP_QUAL_IS_EQUAL,1,1,0,4}, 
               {SOC_PPC_FP_QUAL_KEY_AFTER_HASHING,0,1,0,80}, 
               {SOC_PPC_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR_PS,1,1,0,3}, 
               {SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES,1,0,0,32},
               {SOC_PPC_FP_QUAL_IRPP_KEY_CHANGED,1,0,0,32}, 
               {SOC_PPC_FP_QUAL_IRPP_ALL_ONES,1,0,32,32},
               {SOC_PPC_FP_QUAL_IRPP_SRC_PP_PORT,1,0,64,8},
               {SOC_PPC_FP_QUAL_IRPP_PKT_HDR_TYPE,1,0,72,6},
               {SOC_PPC_FP_QUAL_IRPP_SRC_TM_PORT,1,0,80,8},
               {SOC_PPC_FP_QUAL_PACKET_HEADER_SIZE,1,0,88,7},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET0,1,0,96,7},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET1,1,0,104,7},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET2,1,0,112,7},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET3,1,0,120,7},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET4,1,0,128,7},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET5,1,0,136,7},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET_0_UNTIL_5,1,0,96,(8*6)},
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER0,1,0,144,3},
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1,1,0,148,11},
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_NEXT_PROTOCOL,1,0,148+7,4}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_ENCAPSULATION,1,0,148+5,2}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2,1,0,160,11},
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_BOS,1,0,160,1} ,
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_NEXT_PROTOCOL,1,0,160+7,4}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_IP_FRAGMENTED,1,0,160+6,1}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_IP_HAS_OPTIONS,1,0,160+4,1}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER3,1,0,172,11},
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER3_IP_FRAGMENTED,1,0,172+6,1}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER4,1,0,184,11},
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER5,1,0,196,11},
               {SOC_PPC_FP_QUAL_IRPP_SRC_SYST_PORT,1,0,208,16},
               {SOC_PPC_FP_QUAL_IRPP_PARSER_LEAF_CONTEXT,1,0,224,4},
               {SOC_PPC_FP_QUAL_IRPP_ETH_TAG_FORMAT,1,0,228,4},
               {SOC_PPC_FP_QUAL_FORWARDING_ACTION_STRENGTH,1,0,232,3},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DEST,1,0,240,19},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_TC,1,0,260,3},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DP,1,0,264,2},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_CODE,1,0,268,8},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_QUAL,1,0,276,16},
               {SOC_PPC_FP_QUAL_FORWARDING_ACTION_METER_TRAFFIC_CLASS,1,0,292,2},
               {SOC_PPC_FP_QUAL_IRPP_UP,1,0,296,3},
               {SOC_PPC_FP_QUAL_IRPP_SNOOP_CODE,1,0,300,8},
               {SOC_PPC_FP_QUAL_SNOOP_STRENGTH,1,0,308,2},
               {SOC_PPC_FP_QUAL_IRPP_LL_MIRROR_CMD,1,0,312,4},
               {SOC_PPC_FP_QUAL_IRPP_SYSTEM_VSI,1,0,316,16},
               {SOC_PPC_FP_QUAL_VSI_PROFILE,1,0,332+2,2}, 
               {SOC_PPC_FP_QUAL_FID,1,0,336,15},
               {SOC_PPC_FP_QUAL_IRPP_ORIENTATION_IS_HUB,1,0,352,1},
               {SOC_PPC_FP_QUAL_VLAN_EDIT_CMD_COMMAND,1,0,356,6},
               {SOC_PPC_FP_QUAL_IRPP_VLAN_DEI,1,0,364,1},
               {SOC_PPC_FP_QUAL_IRPP_VLAN_PCP,1,0,365,3},
               {SOC_PPC_FP_QUAL_VLAN_EDIT_CMD_VID2,1,0,368,12},
               {SOC_PPC_FP_QUAL_VLAN_EDIT_CMD_VID1,1,0,380,12},
               {SOC_PPC_FP_QUAL_IRPP_STP_STATE,1,0,392,2},
               {SOC_PPC_FP_QUAL_IRPP_FWD_TYPE,1,0,396,4},
               {SOC_PPC_FP_QUAL_IRPP_SUB_HEADER_NDX,1,0,400,3},
               {SOC_PPC_FP_QUAL_FORWARDING_OFFSET_EXTENSION,1,0,404,2},
               {SOC_PPC_FP_QUAL_IRPP_TERM_TYPE,1,0,408,4},
               {SOC_PPC_FP_QUAL_FORWARDING_HEADER_ENCAPSULATION,1,0,412,2},
               {SOC_PPC_FP_QUAL_IGNORE_CP,1,0,416,1},
               {SOC_PPC_FP_QUAL_SEQUENCE_NUMBER_TAG,1,0,420,16},
               {SOC_PPC_FP_QUAL_EEI,1,0,440,24},
               {SOC_PPC_FP_QUAL_OUT_LIF,1,0,464,16},
               {SOC_PPC_FP_QUAL_IRPP_IN_RIF,1,0,480,12},
               {SOC_PPC_FP_QUAL_IRPP_VRF,1,0,492,12},
               {SOC_PPC_FP_QUAL_IRPP_PCKT_IS_COMP_MC,1,0,504,1},
               {SOC_PPC_FP_QUAL_IRPP_IN_TTL,1,0,508,8},
               {SOC_PPC_FP_QUAL_IRPP_IN_DSCP_EXP,1,0,516,8},
               {SOC_PPC_FP_QUAL_RPF_DESTINATION,1,0,528,19},
               {SOC_PPC_FP_QUAL_RPF_DESTINATION_VALID,1,0,548,1},
               {SOC_PPC_FP_QUAL_INGRESS_LEARN_ENABLE,1,0,552,1},
               {SOC_PPC_FP_QUAL_EGRESS_LEARN_ENABLE,1,0,556,1},
               {SOC_PPC_FP_QUAL_LEARN_KEY,1,0,560,74},
               {SOC_PPC_FP_QUAL_LEARN_KEY_MAC,1,0,560,48},
               {SOC_PPC_FP_QUAL_LEARN_KEY_VLAN,1,0,560+48,16}, 
               {SOC_PPC_FP_QUAL_IRPP_LEARN_DECISION_DEST,1,0,640,19},
               {SOC_PPC_FP_QUAL_IRPP_LEARN_ADD_INFO,1,0,659,21},
               {SOC_PPC_FP_QUAL_IRPP_IN_LIF,1,0,680,16},
               {SOC_PPC_FP_QUAL_IN_LIF_PROFILE,1,0,696,4},
               {SOC_PPC_FP_QUAL_IN_LIF_PROFILE_SAME_IF,1,0,696,1}, 
               {SOC_PPC_FP_QUAL_LEARN_OR_TRANSPLANT,1,0,700,1},
               {SOC_PPC_FP_QUAL_PACKET_IS_BOOTP_DHCP,1,0,704,1},
               {SOC_PPC_FP_QUAL_UNKNOWN_ADDR,1,0,708,1},
               {SOC_PPC_FP_QUAL_FWD_PRCESSING_PROFILE,1,0,712,3},
               {SOC_PPC_FP_QUAL_ELK_ERROR,1,0,716,1},
               {SOC_PPC_FP_QUAL_ELK_LKP_PAYLOAD,1,0,720,128},
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_0,1,0,720+127,1},
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_1,1,0,720+126,1},
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_2,1,0,720+125,1},
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_3,1,0,720+124,1},
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_4,1,1,1,1}, 
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_5,1,1,1,1}, 
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_6,1,1,1,1}, 
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_7,1,1,1,1}, 
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_0,1,0,720+120-48,48},
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_1,1,0,720+120-48-16,16},
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_2,1,0,720+120-48-16-32,32},
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_3,1,0,720+120-48-16-32-24,24},
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_4,1,1,1,1}, 
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_5,1,1,1,1}, 
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_6,1,1,1,1}, 
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_7,1,1,1,1}, 
               {SOC_PPC_FP_QUAL_IRPP_RPF_NATIVE_VSI,1,1,1,15}, 
			   {SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND,1,0,848,1},
               {SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_RESULT,1,0,856,43},
               {SOC_PPC_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR,1,0,904,64},
               {SOC_PPC_FP_QUAL_IRPP_PMF_PGM_KEY_GEN_VAR,1,0,968,32},
               {SOC_PPC_FP_QUAL_IRPP_PTC_KEY_GEN_VAR,1,0,1000,16},
               {SOC_PPC_FP_QUAL_IRPP_PACKET_SIZE_RANGE,1,0,1016,2},
               {SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES,0,1,0,32},
               {SOC_PPC_FP_QUAL_IRPP_ALL_ONES,0,1,32,32},
               {SOC_PPC_FP_QUAL_IRPP_SRC_PP_PORT,0,1,64,8},
               {SOC_PPC_FP_QUAL_IRPP_PKT_HDR_TYPE,0,1,72,6},
               {SOC_PPC_FP_QUAL_IRPP_SRC_TM_PORT,0,1,80,8},
               {SOC_PPC_FP_QUAL_PACKET_HEADER_SIZE,0,1,88,7},
               {SOC_PPC_FP_QUAL_IRPP_SRC_SYST_PORT,0,1,96,16},
               {SOC_PPC_FP_QUAL_IRPP_PARSER_LEAF_CONTEXT,0,1,112,4},
               {SOC_PPC_FP_QUAL_IRPP_ETH_TAG_FORMAT,0,1,116,4},
               {SOC_PPC_FP_QUAL_FORWARDING_ACTION_STRENGTH,0,1,120,3},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DEST,0,1,128,19},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_TC,0,1,148,3},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DP,0,1,152,2},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_CODE,0,1,156,8},
               {SOC_PPC_FP_QUAL_FORWARDING_ACTION_METER_TRAFFIC_CLASS,0,1,164,2},
               {SOC_PPC_FP_QUAL_COUNTER_UPDATE_A,0,1,168,1},
               {SOC_PPC_FP_QUAL_COUNTER_POINTER_A,0,1,176,21},
               {SOC_PPC_FP_QUAL_COUNTER_UPDATE_B,0,1,200,1},
               {SOC_PPC_FP_QUAL_COUNTER_POINTER_B,0,1,208,21},
               {SOC_PPC_FP_QUAL_IRPP_SNOOP_CODE,0,1,232,8},
               {SOC_PPC_FP_QUAL_SNOOP_STRENGTH,0,1,240,2},
               {SOC_PPC_FP_QUAL_IRPP_LL_MIRROR_CMD,0,1,244,4},
               {SOC_PPC_FP_QUAL_IRPP_SYSTEM_VSI,0,1,248,16},
               {SOC_PPC_FP_QUAL_VSI_PROFILE,0,1,264+2,2}, 
               {SOC_PPC_FP_QUAL_FID,0,1,268,15},
               {SOC_PPC_FP_QUAL_IRPP_ORIENTATION_IS_HUB,0,1,284,1},
               {SOC_PPC_FP_QUAL_VLAN_EDIT_CMD_COMMAND,0,1,288,6},
               {SOC_PPC_FP_QUAL_IRPP_VLAN_DEI,0,1,296,1},
               {SOC_PPC_FP_QUAL_IRPP_VLAN_PCP,0,1,299,3},
               {SOC_PPC_FP_QUAL_VLAN_EDIT_CMD_VID2,0,1,300,12},
               {SOC_PPC_FP_QUAL_VLAN_EDIT_CMD_VID1,0,1,312,12},
               {SOC_PPC_FP_QUAL_IRPP_STP_STATE,0,1,324,2},
               {SOC_PPC_FP_QUAL_IRPP_FWD_TYPE,0,1,328,4},
               {SOC_PPC_FP_QUAL_IRPP_SUB_HEADER_NDX,0,1,332,3},
               {SOC_PPC_FP_QUAL_FORWARDING_OFFSET_EXTENSION,0,1,336,2},
               {SOC_PPC_FP_QUAL_IRPP_TERM_TYPE,0,1,340,4},
               {SOC_PPC_FP_QUAL_FORWARDING_HEADER_ENCAPSULATION,0,1,344,2},
               {SOC_PPC_FP_QUAL_IGNORE_CP,0,1,348,1},
               {SOC_PPC_FP_QUAL_PROGRAM_INDEX,0,1,352,5},
               {SOC_PPC_FP_QUAL_EEI,0,1,360,24},
               {SOC_PPC_FP_QUAL_OUT_LIF,0,1,384,16},
               {SOC_PPC_FP_QUAL_IRPP_IN_RIF,0,1,400,12},
               {SOC_PPC_FP_QUAL_LEARN_DATA,0,1,416,40},
               {SOC_PPC_FP_QUAL_IRPP_IN_LIF,0,1,456,16},
               {SOC_PPC_FP_QUAL_IN_LIF_PROFILE,0,1,472,4},
               {SOC_PPC_FP_QUAL_IN_LIF_PROFILE_SAME_IF,0,1,472,1}, 
               {SOC_PPC_FP_QUAL_LEM_2ND_LOOKUP_FOUND,0,1,476,1},
               {SOC_PPC_FP_QUAL_LEM_2ND_LOOKUP_RESULT,0,1,480,43}, 
               {SOC_PPC_FP_QUAL_IRPP_LEM_2ND_LKUP_ASD,0,1,480,43}, 
               {SOC_PPC_FP_QUAL_LPM_1ST_LOOKUP_FOUND,0,1,524,1},
               {SOC_PPC_FP_QUAL_LPM_1ST_LOOKUP_RESULT,0,1,528,15},
               {SOC_PPC_FP_QUAL_LPM_2ND_LOOKUP_FOUND,0,1,544,1},
               {SOC_PPC_FP_QUAL_LPM_2ND_LOOKUP_RESULT,0,1,548,15},
               {SOC_PPC_FP_QUAL_TCAM_MATCH,0,1,564,1},
               {SOC_PPC_FP_QUAL_TCAM_RESULT,0,1,568,40},
               {SOC_PPC_FP_QUAL_TCAM_TRAPS0_MATCH,0,1,608,1},
               {SOC_PPC_FP_QUAL_TCAM_TRAPS0_RESULT,0,1,616,40},
               {SOC_PPC_FP_QUAL_TCAM_TRAPS1_MATCH,0,1,656,1},
               {SOC_PPC_FP_QUAL_TCAM_TRAPS1_RESULT,0,1,664,40},
               {SOC_PPC_FP_QUAL_TT_PROCESSING_PROFILE,0,1,704,3},
               {SOC_PPC_FP_QUAL_TT_LOOKUP0_FOUND,0,1,708,1},
               {SOC_PPC_FP_QUAL_TT_LOOKUP0_PAYLOAD,0,1,712,16},
               {SOC_PPC_FP_QUAL_TT_LOOKUP1_FOUND,0,1,728,1},
               {SOC_PPC_FP_QUAL_TT_LOOKUP1_PAYLOAD,0,1,732,16},
               {SOC_PPC_FP_QUAL_VT_PROCESSING_PROFILE,0,1,748,3},
               {SOC_PPC_FP_QUAL_VT_LOOKUP0_FOUND,0,1,752,1},
               {SOC_PPC_FP_QUAL_VT_LOOKUP0_PAYLOAD,0,1,756,16},
               {SOC_PPC_FP_QUAL_VT_LOOKUP1_FOUND,0,1,772,1},
               {SOC_PPC_FP_QUAL_VT_LOOKUP1_PAYLOAD,0,1,776,16},
               {SOC_PPC_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR,0,1,792, 64},
               {SOC_PPC_FP_QUAL_IRPP_PMF_PGM_KEY_GEN_VAR,0,1,856,32},
               {SOC_PPC_FP_QUAL_IRPP_PTC_KEY_GEN_VAR,0,1,888,16},
               {SOC_PPC_FP_QUAL_IRPP_PACKET_SIZE_RANGE,0,1,904,2},
               {SOC_PPC_FP_QUAL_CPU_TRAP_CODE_PROFILE,0,1,908,1},
               {SOC_PPC_FP_QUAL_VID_VALID,0,1,912,1},
               {SOC_PPC_FP_QUAL_DA_IS_BPDU,0,1,916,1},
               {SOC_PPC_FP_QUAL_HDR_IPV4_L4OPS_LOW,0,1,920,16}, 
               {SOC_PPC_FP_QUAL_HDR_IPV4_L4OPS_HI,0,1,936,8}, 
               {SOC_PPC_FP_QUAL_HDR_IPV6_L4OPS,0,1,920,24}, 
               {SOC_PPC_FP_QUAL_PACKET_IS_IEEE1588,0,1,944,1},
               {SOC_PPC_FP_QUAL_IEEE1588_ENCAPSULATION,0,1,948,1},
               {SOC_PPC_FP_QUAL_IEEE1588_COMPENSATE_TIME_STAMP,0,1,952,1},
               {SOC_PPC_FP_QUAL_IEEE1588_COMMAND,0,1,956,2},
               {SOC_PPC_FP_QUAL_IEEE1588_HEADER_OFFSET,0,1,960,7},
               {SOC_PPC_FP_QUAL_OAM_UP_MEP,0,1,968,1},
               {SOC_PPC_FP_QUAL_OAM_SUB_TYPE,0,1,972,3},
               {SOC_PPC_FP_QUAL_OAM_OFFSET,0,1,976,7},
               {SOC_PPC_FP_QUAL_OAM_STAMP_OFFSET,0,1,984,7},
               {SOC_PPC_FP_QUAL_OAM_METER_DISABLE,0,1,992,1},
               {SOC_PPC_FP_QUAL_OAM_ID,0,1,1000,17},
               {SOC_PPC_FP_QUAL_OAM_UP_MEP,0,1,968,1},
               {SOC_PPC_FP_QUAL_OAM_SUB_TYPE,0,1,972,3},
               {SOC_PPC_FP_QUAL_OAM_OFFSET,0,1,976,7},
               {SOC_PPC_FP_QUAL_OAM_STAMP_OFFSET,0,1,984,7},
               {SOC_PPC_FP_QUAL_OAM_ID,0,1,1000,17},
               {SOC_PPC_FP_QUAL_OAM_METER_DISABLE,0,1,992,1},
               {SOC_PPC_FP_QUAL_IRPP_IN_LIF_DATA,0,1,0,58},
               {SOC_PPC_FP_QUAL_VLAN_DOMAIN, 1, 1, 0, 8 }, 
               {SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID, 1, 1, 0, 12 }, 
               {SOC_PPC_FP_QUAL_CMPRSD_INNER_VID, 1, 1, 0, 12 },
               {SOC_PPC_FP_QUAL_INITIAL_VID, 1, 1, 0, 12 },
               {SOC_PPC_FP_QUAL_IRPP_PRESEL_ID,1,0,0,32}, 
               {SOC_PPC_FP_QUAL_IRPP_PRESEL_PROFILE0,1,0,0,2},
               {SOC_PPC_FP_QUAL_IRPP_PRESEL_PROFILE1,1,0,0,2},
               {SOC_PPC_FP_QUAL_IRPP_PRESEL_PROFILE2,1,0,0,2},
               {SOC_PPC_FP_QUAL_IRPP_PRESEL_PROFILE3,1,0,0,2},
               {SOC_PPC_FP_QUAL_IRPP_PRESEL_PROFILE_KAPS,1,0,0,2},
               {SOC_PPC_FP_QUAL_IRPP_PTC_KEY_GEN_VAR_PS ,1,1,0,3}, 
               {SOC_PPC_FP_QUAL_IRPP_IN_RIF_PROFILE,1,1,1,1}, 
               {SOC_PPC_FP_QUAL_IRPP_PEM_GENERAL_DATA,0,1,0,64}, 
               {SOC_PPC_FP_QUAL_IRPP_IN_LIF_DATA_INDEX,1,1,1,2}, 
        };

        
CONST STATIC 
    ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES
        Arad_plus_pmf_ce_irpp_pmf_info[] = {
               {SOC_PPC_FP_QUAL_IRPP_INVALID,1,1,0,0},
	       {SOC_PPC_FP_QUAL_OUT_LIF_RANGE,1,1,0,2}, 
               {SOC_PPC_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR_PS,1,1,0,3}, 
               {SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES,1,0,0,32},
               {SOC_PPC_FP_QUAL_IRPP_KEY_CHANGED,1,0,0,32}, 
               {SOC_PPC_FP_QUAL_IS_EQUAL,0,1,0,4}, 
               {SOC_PPC_FP_QUAL_IRPP_ALL_ONES,1,0,32,32},
               {SOC_PPC_FP_QUAL_IRPP_SRC_PP_PORT,1,0,64,8},
               {SOC_PPC_FP_QUAL_IRPP_PKT_HDR_TYPE,1,0,72,6},
               {SOC_PPC_FP_QUAL_IRPP_SRC_TM_PORT,1,0,80,8},
               {SOC_PPC_FP_QUAL_PACKET_HEADER_SIZE,1,0,88,7},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET0,1,0,96,7},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET1,1,0,104,7},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET2,1,0,112,7},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET3,1,0,120,7},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET4,1,0,128,7},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET5,1,0,136,7},
               {SOC_PPC_FP_QUAL_HEADER_OFFSET_0_UNTIL_5,1,0,96,(8*6)},
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER0,1,0,144,3},
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1,1,0,148,11},
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_NEXT_PROTOCOL,1,0,148+7,4}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_ENCAPSULATION,1,0,148+5,2}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2,1,0,160,11},
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_BOS,1,0,160,1} ,
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_NEXT_PROTOCOL,1,0,160+7,4}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_IP_FRAGMENTED,1,0,160+6,1}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_IP_HAS_OPTIONS,1,0,160+4,1}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER3,1,0,172,11},
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER3_IP_FRAGMENTED,1,0,172+6,1}, 
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER4,1,0,184,11},
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER5,1,0,196,11},
               {SOC_PPC_FP_QUAL_IRPP_SRC_SYST_PORT,1,0,208,16},
               {SOC_PPC_FP_QUAL_IRPP_PARSER_LEAF_CONTEXT,1,0,224,4},
               {SOC_PPC_FP_QUAL_IRPP_ETH_TAG_FORMAT,1,0,228,4},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DEST,1,0,232,19},
               {SOC_PPC_FP_QUAL_FORWARDING_ACTION_STRENGTH,1,0,252,3},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_TC,1,0,256,3},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DP,1,0,260,2},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_CODE,1,0,264,8},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_QUAL,1,0,272,16},
               {SOC_PPC_FP_QUAL_FORWARDING_ACTION_METER_TRAFFIC_CLASS,1,0,288,2},
               {SOC_PPC_FP_QUAL_IRPP_UP,1,0,292,3},
               {SOC_PPC_FP_QUAL_IRPP_SNOOP_CODE,1,0,296,8},
               {SOC_PPC_FP_QUAL_SNOOP_STRENGTH,1,0,304,2},
               {SOC_PPC_FP_QUAL_IRPP_LL_MIRROR_CMD,1,0,308,4},
               {SOC_PPC_FP_QUAL_IRPP_SYSTEM_VSI,1,0,312,16},
               {SOC_PPC_FP_QUAL_VSI_PROFILE,1,0,328+2,2}, 
               {SOC_PPC_FP_QUAL_FID,1,0,332,15},
               {SOC_PPC_FP_QUAL_IRPP_ORIENTATION_IS_HUB,1,0,348,1},
               {SOC_PPC_FP_QUAL_VLAN_EDIT_CMD_COMMAND,1,0,352,6},
               {SOC_PPC_FP_QUAL_IRPP_VLAN_DEI,1,0,360,1},
               {SOC_PPC_FP_QUAL_IRPP_VLAN_PCP,1,0,361,3},
               {SOC_PPC_FP_QUAL_VLAN_EDIT_CMD_VID2,1,0,364,12},
               {SOC_PPC_FP_QUAL_VLAN_EDIT_CMD_VID1,1,0,376,12},
               {SOC_PPC_FP_QUAL_IRPP_STP_STATE,1,0,388,2},
               {SOC_PPC_FP_QUAL_IRPP_FWD_TYPE,1,0,392,4},
               {SOC_PPC_FP_QUAL_FORWARDING_OFFSET_EXTENSION,1,0,396,2},
               {SOC_PPC_FP_QUAL_IRPP_SUB_HEADER_NDX,1,0,400,3},
               {SOC_PPC_FP_QUAL_IRPP_TERM_TYPE,1,0,404,4},
               {SOC_PPC_FP_QUAL_FORWARDING_HEADER_ENCAPSULATION,1,0,408,2},
               {SOC_PPC_FP_QUAL_IGNORE_CP,1,0,412,1},
               {SOC_PPC_FP_QUAL_SEQUENCE_NUMBER_TAG,1,0,416,16},
               {SOC_PPC_FP_QUAL_EEI,1,0,432,24},
               {SOC_PPC_FP_QUAL_OUT_LIF,1,0,456,16},
               {SOC_PPC_FP_QUAL_IRPP_IN_RIF,1,0,472,12},
               {SOC_PPC_FP_QUAL_IRPP_VRF,1,0,484,12},
               {SOC_PPC_FP_QUAL_IRPP_PCKT_IS_COMP_MC,1,0,496,1},
               {SOC_PPC_FP_QUAL_IRPP_IN_TTL,1,0,500,8},
               {SOC_PPC_FP_QUAL_IRPP_IN_DSCP_EXP,1,0,508,8},
               {SOC_PPC_FP_QUAL_RPF_DESTINATION_VALID,1,0,516,1},
               {SOC_PPC_FP_QUAL_RPF_DESTINATION,1,0,520,19},
               {SOC_PPC_FP_QUAL_INGRESS_LEARN_ENABLE,1,0,540,1},
               {SOC_PPC_FP_QUAL_EGRESS_LEARN_ENABLE,1,0,544,1},
               {SOC_PPC_FP_QUAL_LEARN_OR_TRANSPLANT,1,0,548,1},
               {SOC_PPC_FP_QUAL_IRPP_LEARN_DECISION_DEST,1,0,552,19},
               {SOC_PPC_FP_QUAL_IRPP_LEARN_ADD_INFO,1,0,571,21},
               {SOC_PPC_FP_QUAL_LEARN_KEY,1,0,592,74},
               {SOC_PPC_FP_QUAL_LEARN_KEY_MAC,1,0,592,48},
               {SOC_PPC_FP_QUAL_LEARN_KEY_VLAN,1,0,592+48,16}, 
               {SOC_PPC_FP_QUAL_IRPP_IN_LIF,1,0,668,16},
               {SOC_PPC_FP_QUAL_IN_LIF_PROFILE,1,0,684,4},
               {SOC_PPC_FP_QUAL_IN_LIF_PROFILE_SAME_IF,1,0,684,1}, 
               {SOC_PPC_FP_QUAL_PACKET_IS_BOOTP_DHCP,1,0,688,1},
               {SOC_PPC_FP_QUAL_UNKNOWN_ADDR,1,0,692,1},
               {SOC_PPC_FP_QUAL_FWD_PRCESSING_PROFILE,1,0,696,3},
               {SOC_PPC_FP_QUAL_ELK_ERROR,1,0,700,1},
               {SOC_PPC_FP_QUAL_ELK_LKP_PAYLOAD,1,0,704,128},
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_0,1,0,704+127,1},
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_1,1,0,704+126,1},
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_2,1,0,704+125,1},
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_3,1,0,704+124,1},
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_4,1,1,1,1}, 
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_5,1,1,1,1}, 
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_6,1,1,1,1}, 
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_7,1,1,1,1}, 
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_0,1,0,704+120-48,48},
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_1,1,0,704+120-48-32,32},
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_2,1,0,704+120-48-32-16,16},
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_3,1,0,704+120-48-32-16-24,24},
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_4,1,1,1,1}, 
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_5,1,1,1,1}, 
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_6,1,1,1,1}, 
                {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_7,1,1,1,1}, 
               {SOC_PPC_FP_QUAL_IRPP_RPF_NATIVE_VSI,1,1,1,15}, 
               {SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_RESULT,1,0,832,43},
               {SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND,1,0,876,1},
               {SOC_PPC_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR,1,0,880,64},
               {SOC_PPC_FP_QUAL_IRPP_PMF_PGM_KEY_GEN_VAR,1,0,944,32},
               {SOC_PPC_FP_QUAL_IRPP_PTC_KEY_GEN_VAR,1,0,976,16},
               {SOC_PPC_FP_QUAL_IRPP_PACKET_SIZE_RANGE,1,0,992,2},
               {SOC_PPC_FP_QUAL_STAMP_NATIVE_VSI,1,0,996,1},
               {SOC_PPC_FP_QUAL_NATIVE_VSI,1,0,1000,12},
               {SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES,0,1,0,32},
               {SOC_PPC_FP_QUAL_IRPP_ALL_ONES,0,1,32,32},
               {SOC_PPC_FP_QUAL_IRPP_SRC_PP_PORT,0,1,64,8},
               {SOC_PPC_FP_QUAL_IRPP_PKT_HDR_TYPE,0,1,72,6},
               {SOC_PPC_FP_QUAL_IRPP_SRC_TM_PORT,0,1,80,8},
               {SOC_PPC_FP_QUAL_PACKET_HEADER_SIZE,0,1,88,7},
               {SOC_PPC_FP_QUAL_IRPP_SRC_SYST_PORT,0,1,96,16},
               {SOC_PPC_FP_QUAL_IRPP_PARSER_LEAF_CONTEXT,0,1,112,4},
               {SOC_PPC_FP_QUAL_IRPP_ETH_TAG_FORMAT,0,1,116,4},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DEST,0,1,120,19},
               {SOC_PPC_FP_QUAL_FORWARDING_ACTION_STRENGTH,0,1,140,3},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_TC,0,1,144,3},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DP,0,1,148,2},
               {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_CODE,0,1,152,8},
               {SOC_PPC_FP_QUAL_FORWARDING_ACTION_METER_TRAFFIC_CLASS,0,1,160,2},
               {SOC_PPC_FP_QUAL_COUNTER_UPDATE_A,0,1,164,1},
               {SOC_PPC_FP_QUAL_COUNTER_POINTER_A,0,1,168,21},
               {SOC_PPC_FP_QUAL_CPU_TRAP_CODE_PROFILE,0,1,192,1},
               {SOC_PPC_FP_QUAL_COUNTER_UPDATE_B,0,1,196,1},
               {SOC_PPC_FP_QUAL_COUNTER_POINTER_B,0,1,200,21},
               {SOC_PPC_FP_QUAL_IRPP_SNOOP_CODE,0,1,224,8},
               {SOC_PPC_FP_QUAL_SNOOP_STRENGTH,0,1,232,2},
               {SOC_PPC_FP_QUAL_IRPP_LL_MIRROR_CMD,0,1,236,4},
               {SOC_PPC_FP_QUAL_IRPP_SYSTEM_VSI,0,1,240,16},
               {SOC_PPC_FP_QUAL_VSI_PROFILE,0,1,256+2,2}, 
               {SOC_PPC_FP_QUAL_FID,0,1,260,15},
               {SOC_PPC_FP_QUAL_IRPP_ORIENTATION_IS_HUB,0,1,276,1},
               {SOC_PPC_FP_QUAL_VLAN_EDIT_CMD_COMMAND,0,1,280,6},
               {SOC_PPC_FP_QUAL_IRPP_VLAN_DEI,0,1,288,1},
               {SOC_PPC_FP_QUAL_IRPP_VLAN_PCP,0,1,289,3},
               {SOC_PPC_FP_QUAL_VLAN_EDIT_CMD_VID2,0,1,292,12},
               {SOC_PPC_FP_QUAL_VLAN_EDIT_CMD_VID1,0,1,304,12},
               {SOC_PPC_FP_QUAL_IRPP_STP_STATE,0,1,316,2},
               {SOC_PPC_FP_QUAL_IRPP_FWD_TYPE,0,1,320,4},
               {SOC_PPC_FP_QUAL_IRPP_SUB_HEADER_NDX,0,1,324,3},
               {SOC_PPC_FP_QUAL_FORWARDING_OFFSET_EXTENSION,0,1,328,2},
               {SOC_PPC_FP_QUAL_IRPP_TERM_TYPE,0,1,332,4},
               {SOC_PPC_FP_QUAL_FORWARDING_HEADER_ENCAPSULATION,0,1,336,2},
               {SOC_PPC_FP_QUAL_IGNORE_CP,0,1,340,1},
               {SOC_PPC_FP_QUAL_PROGRAM_INDEX,0,1,344,5},
               {SOC_PPC_FP_QUAL_EEI,0,1,352,24},
               {SOC_PPC_FP_QUAL_OUT_LIF,0,1,376,16},
               {SOC_PPC_FP_QUAL_IRPP_IN_RIF,0,1,392,12},
               {SOC_PPC_FP_QUAL_VID_VALID,0,1,404,1},
               {SOC_PPC_FP_QUAL_LEARN_DATA,0,1,408,40},
               {SOC_PPC_FP_QUAL_IRPP_IN_LIF,0,1,448,16},
               {SOC_PPC_FP_QUAL_IN_LIF_PROFILE,0,1,464,4},
               {SOC_PPC_FP_QUAL_IN_LIF_PROFILE_SAME_IF,0,1,464,1}, 
               {SOC_PPC_FP_QUAL_LEM_2ND_LOOKUP_FOUND,0,1,468,1},
               {SOC_PPC_FP_QUAL_LEM_2ND_LOOKUP_RESULT,0,1,472,43}, 
               {SOC_PPC_FP_QUAL_IRPP_LEM_2ND_LKUP_ASD,0,1,472,43}, 
               {SOC_PPC_FP_QUAL_LPM_1ST_LOOKUP_FOUND,0,1,516,1},
               {SOC_PPC_FP_QUAL_LPM_1ST_LOOKUP_RESULT,0,1,520,15},
               {SOC_PPC_FP_QUAL_LPM_2ND_LOOKUP_FOUND,0,1,536,1},
               {SOC_PPC_FP_QUAL_LPM_2ND_LOOKUP_RESULT,0,1,540,15},
               {SOC_PPC_FP_QUAL_TCAM_MATCH,0,1,556,1},
               {SOC_PPC_FP_QUAL_TCAM_RESULT,0,1,560,40},
               {SOC_PPC_FP_QUAL_TCAM_TRAPS0_MATCH,0,1,600,1},
               {SOC_PPC_FP_QUAL_TCAM_TRAPS0_RESULT,0,1,608,40},
               {SOC_PPC_FP_QUAL_TCAM_TRAPS1_MATCH,0,1,648,1},
               {SOC_PPC_FP_QUAL_TCAM_TRAPS1_RESULT,0,1,656,40},
               {SOC_PPC_FP_QUAL_TT_PROCESSING_PROFILE,0,1,696,3},
               {SOC_PPC_FP_QUAL_TT_LOOKUP0_FOUND,0,1,700,1},
               {SOC_PPC_FP_QUAL_TT_LOOKUP0_PAYLOAD,0,1,704,16},
               {SOC_PPC_FP_QUAL_TT_LOOKUP1_FOUND,0,1,720,1},
               {SOC_PPC_FP_QUAL_TT_LOOKUP1_PAYLOAD,0,1,724,16},
               {SOC_PPC_FP_QUAL_VT_PROCESSING_PROFILE,0,1,740,3},
               {SOC_PPC_FP_QUAL_VT_LOOKUP0_FOUND,0,1,744,1},
               {SOC_PPC_FP_QUAL_VT_LOOKUP0_PAYLOAD,0,1,748,16},
               {SOC_PPC_FP_QUAL_VT_LOOKUP1_FOUND,0,1,764,1},
               {SOC_PPC_FP_QUAL_VT_LOOKUP1_PAYLOAD,0,1,768,16},
               {SOC_PPC_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR,0,1,784,64},
               {SOC_PPC_FP_QUAL_IRPP_PMF_PGM_KEY_GEN_VAR,0,1,848,32},
               {SOC_PPC_FP_QUAL_IRPP_PTC_KEY_GEN_VAR,0,1,880,16},
               {SOC_PPC_FP_QUAL_IRPP_PACKET_SIZE_RANGE,0,1,896,2},
               {SOC_PPC_FP_QUAL_DA_IS_BPDU,0,1,900,1},
               {SOC_PPC_FP_QUAL_HDR_IPV4_L4OPS_LOW,0,1,904,16}, 
               {SOC_PPC_FP_QUAL_HDR_IPV4_L4OPS_HI,0,1,920,8}, 
               {SOC_PPC_FP_QUAL_HDR_IPV6_L4OPS,0,1,904,24}, 
               {SOC_PPC_FP_QUAL_PACKET_IS_IEEE1588,0,1,928,1},
               {SOC_PPC_FP_QUAL_IEEE1588_ENCAPSULATION,0,1,932,1},
               {SOC_PPC_FP_QUAL_IEEE1588_COMPENSATE_TIME_STAMP,0,1,936,1},
               {SOC_PPC_FP_QUAL_IEEE1588_COMMAND,0,1,940,2},
               {SOC_PPC_FP_QUAL_IEEE1588_HEADER_OFFSET,0,1,944,7},
               {SOC_PPC_FP_QUAL_OAM_UP_MEP,0,1,952,1},
               {SOC_PPC_FP_QUAL_OAM_SUB_TYPE,0,1,956,3},
               {SOC_PPC_FP_QUAL_OAM_OFFSET,0,1,960,7},
               {SOC_PPC_FP_QUAL_OAM_STAMP_OFFSET,0,1,968,7},
               {SOC_PPC_FP_QUAL_OAM_ID,0,1,976,17},
               {SOC_PPC_FP_QUAL_OAM_METER_DISABLE,0,1,996,1},
               {SOC_PPC_FP_QUAL_STAMP_NATIVE_VSI,0,1,1000,1},
               {SOC_PPC_FP_QUAL_NATIVE_VSI,0,1,1004,12},
               {SOC_PPC_FP_QUAL_OAM_UP_MEP,0,1,952,1},
               {SOC_PPC_FP_QUAL_OAM_SUB_TYPE,0,1,956,3},
               {SOC_PPC_FP_QUAL_OAM_OFFSET,0,1,960,7},
               {SOC_PPC_FP_QUAL_OAM_STAMP_OFFSET,0,1,968,7},
               {SOC_PPC_FP_QUAL_OAM_ID,0,1,976,17},
               {SOC_PPC_FP_QUAL_OAM_METER_DISABLE,0,1,996,1},
               {SOC_PPC_FP_QUAL_IRPP_IN_LIF_DATA,0,1,0,58},
               {SOC_PPC_FP_QUAL_VLAN_DOMAIN, 1, 1, 0, 8 },
               {SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID, 1, 1, 0, 12 },
               {SOC_PPC_FP_QUAL_CMPRSD_INNER_VID, 1, 1, 0, 12 },
               {SOC_PPC_FP_QUAL_INITIAL_VID, 1, 1, 0, 12 },
               {SOC_PPC_FP_QUAL_IRPP_PRESEL_ID,1,0,0,32}, 
               {SOC_PPC_FP_QUAL_IRPP_PRESEL_PROFILE0,1,0,0,2},
               {SOC_PPC_FP_QUAL_IRPP_PRESEL_PROFILE1,1,0,0,2},
               {SOC_PPC_FP_QUAL_IRPP_PRESEL_PROFILE2,1,0,0,2},
               {SOC_PPC_FP_QUAL_IRPP_PRESEL_PROFILE3,1,0,0,2},
               {SOC_PPC_FP_QUAL_IRPP_PRESEL_PROFILE_KAPS,1,0,0,2},
               {SOC_PPC_FP_QUAL_IRPP_PTC_KEY_GEN_VAR_PS ,1,1,0,3}, 
               {SOC_PPC_FP_QUAL_KEY_AFTER_HASHING,0,1,0,80},
               {SOC_PPC_FP_QUAL_IRPP_IN_RIF_PROFILE,1,1,1,1}, 
               {SOC_PPC_FP_QUAL_IRPP_PEM_GENERAL_DATA,0,1,0,64}, 
               {SOC_PPC_FP_QUAL_IRPP_IN_LIF_DATA_INDEX,1,1,1,2}, 
        };

CONST STATIC 
    ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES
        Jericho_pmf_ce_irpp_pmf_info[] = {
              {SOC_PPC_FP_QUAL_IRPP_INVALID,1,1,0,0},
              {SOC_PPC_FP_QUAL_OUT_LIF_RANGE,1,1,0,2}, 
	      {SOC_PPC_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR_PS,1,1,0,3}, 
              {SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES,1,0,0,32},
              {SOC_PPC_FP_QUAL_IRPP_KEY_CHANGED,1,0,0,32}, 
              {SOC_PPC_FP_QUAL_IS_EQUAL,0,1,0,4}, 
              {SOC_PPC_FP_QUAL_IRPP_ALL_ONES,1,0,32,32},
              
              {SOC_PPC_FP_QUAL_IRPP_PKT_HDR_TYPE,1,0,72,6},
              {SOC_PPC_FP_QUAL_IRPP_SRC_TM_PORT,1,0,80,8},
              {SOC_PPC_FP_QUAL_PACKET_HEADER_SIZE,1,0,88,7},
              {SOC_PPC_FP_QUAL_HEADER_OFFSET0,1,0,96,7},
              {SOC_PPC_FP_QUAL_HEADER_OFFSET1,1,0,104,7},
              {SOC_PPC_FP_QUAL_HEADER_OFFSET2,1,0,112,7},
              {SOC_PPC_FP_QUAL_HEADER_OFFSET3,1,0,120,7},
              {SOC_PPC_FP_QUAL_HEADER_OFFSET4,1,0,128,7},
              {SOC_PPC_FP_QUAL_HEADER_OFFSET5,1,0,136,7},
              {SOC_PPC_FP_QUAL_HEADER_OFFSET_0_UNTIL_5,1,0,96,(8*6)},
              {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER0,1,0,144,3},
              {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1,1,0,148,11},
              {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_NEXT_PROTOCOL,1,0,148+7,4}, 
              {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_ENCAPSULATION,1,0,148+5,2}, 
              {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2,1,0,160,11},
              {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_BOS,1,0,160,1} ,
              {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_NEXT_PROTOCOL,1,0,160+7,4}, 
              {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_IP_FRAGMENTED,1,0,160+6,1}, 
              {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_IP_HAS_OPTIONS,1,0,160+4,1}, 
              {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER3,1,0,172,11},
              {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER3_IP_FRAGMENTED,1,0,172+6,1}, 
              {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER4,1,0,184,11},
              {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER5,1,0,196,11},
              {SOC_PPC_FP_QUAL_IRPP_SRC_SYST_PORT,1,0,208,16},
              {SOC_PPC_FP_QUAL_IRPP_PARSER_LEAF_CONTEXT,1,0,224,4},
              {SOC_PPC_FP_QUAL_IRPP_ETH_TAG_FORMAT,1,0,228,4},
              {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DEST,1,0,232,19},
              {SOC_PPC_FP_QUAL_FORWARDING_ACTION_STRENGTH,1,0,252,3},
              {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_TC,1,0,256,3},
              {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DP,1,0,260,2},
              {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_CODE,1,0,264,8},
              {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_QUAL,1,0,272,16},
              {SOC_PPC_FP_QUAL_FORWARDING_ACTION_METER_TRAFFIC_CLASS,1,0,288,2},
              {SOC_PPC_FP_QUAL_IRPP_UP,1,0,292,3},
              {SOC_PPC_FP_QUAL_IRPP_SNOOP_CODE,1,0,296,8},
              {SOC_PPC_FP_QUAL_SNOOP_STRENGTH,1,0,304,2},
              {SOC_PPC_FP_QUAL_IRPP_LL_MIRROR_CMD,1,0,308,4},
              {SOC_PPC_FP_QUAL_IRPP_SYSTEM_VSI,1,0,312,16},
              {SOC_PPC_FP_QUAL_VSI_PROFILE,1,0,328+2,2}, 
              {SOC_PPC_FP_QUAL_FID,1,0,332,15},
              {SOC_PPC_FP_QUAL_IRPP_ORIENTATION_IS_HUB,1,0,348,1},
              {SOC_PPC_FP_QUAL_VLAN_EDIT_CMD_COMMAND,1,0,352,6},
              {SOC_PPC_FP_QUAL_IRPP_VLAN_DEI,1,0,360,1},
              {SOC_PPC_FP_QUAL_IRPP_VLAN_PCP,1,0,361,3},
              {SOC_PPC_FP_QUAL_VLAN_EDIT_CMD_VID2,1,0,364,12},
              {SOC_PPC_FP_QUAL_VLAN_EDIT_CMD_VID1,1,0,376,12},
              {SOC_PPC_FP_QUAL_IRPP_STP_STATE,1,0,388,2},
              {SOC_PPC_FP_QUAL_IRPP_FWD_TYPE,1,0,392,4},
              {SOC_PPC_FP_QUAL_FORWARDING_OFFSET_EXTENSION,1,0,396,2},
              {SOC_PPC_FP_QUAL_IRPP_SUB_HEADER_NDX,1,0,400,3},
              {SOC_PPC_FP_QUAL_IRPP_TERM_TYPE,1,0,404,4},
              {SOC_PPC_FP_QUAL_FORWARDING_HEADER_ENCAPSULATION,1,0,408,2},
              {SOC_PPC_FP_QUAL_IGNORE_CP,1,0,412,1},
              {SOC_PPC_FP_QUAL_SEQUENCE_NUMBER_TAG,1,0,416,16},
              {SOC_PPC_FP_QUAL_EEI,1,0,432,24},
              {SOC_PPC_FP_QUAL_OUT_LIF,1,0,456,18},
              {SOC_PPC_FP_QUAL_IRPP_IN_RIF,1,0,476,15},
              {SOC_PPC_FP_QUAL_IRPP_VRF,1,0,492,14},
              {SOC_PPC_FP_QUAL_IRPP_PCKT_IS_COMP_MC,1,0,508,1},
              {SOC_PPC_FP_QUAL_IRPP_IN_TTL,1,0,512,8},
              {SOC_PPC_FP_QUAL_IRPP_IN_DSCP_EXP,1,0,520,8},
              {SOC_PPC_FP_QUAL_RPF_DESTINATION_VALID,1,0,528,1},
              {SOC_PPC_FP_QUAL_RPF_DESTINATION,1,0,536,19},
              {SOC_PPC_FP_QUAL_INGRESS_LEARN_ENABLE,1,0,556,1},
              {SOC_PPC_FP_QUAL_EGRESS_LEARN_ENABLE,1,0,560,1},
              {SOC_PPC_FP_QUAL_LEARN_OR_TRANSPLANT,1,0,564,1},
              {SOC_PPC_FP_QUAL_LEARN_DATA,1,0,568,40},
              {SOC_PPC_FP_QUAL_IRPP_LEARN_DECISION_DEST,1,0,568,19},
              {SOC_PPC_FP_QUAL_IRPP_LEARN_ADD_INFO,1,0,587,21},
              {SOC_PPC_FP_QUAL_LEARN_KEY,1,0,608,74},
              {SOC_PPC_FP_QUAL_LEARN_KEY_MAC,1,0,608,48},
              {SOC_PPC_FP_QUAL_LEARN_KEY_VLAN,1,0,608+48,16}, 
              {SOC_PPC_FP_QUAL_IRPP_IN_LIF,1,0,688,18},
              {SOC_PPC_FP_QUAL_IN_LIF_PROFILE,1,0,708,4},
              {SOC_PPC_FP_QUAL_IN_LIF_PROFILE_SAME_IF,1,0,708,1}, 
              {SOC_PPC_FP_QUAL_PACKET_IS_BOOTP_DHCP,1,0,712,1},
              {SOC_PPC_FP_QUAL_UNKNOWN_ADDR,1,0,716,1},
              {SOC_PPC_FP_QUAL_FWD_PRCESSING_PROFILE,1,0,720,3},
              {SOC_PPC_FP_QUAL_ELK_ERROR,1,0,724,1},
              {SOC_PPC_FP_QUAL_ELK_LKP_PAYLOAD,1,0,728,128},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_0,1,0,728+127,1},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_1,1,0,728+126,1},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_2,1,0,728+125,1},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_3,1,0,728+124,1},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_4,1,0,728+123,1},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_5,1,0,728+122,1},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_6,1,0,728+121,1},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_7,1,0,728+120,1},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_0,1,0,728+120-48,48},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_1,1,0,728+120-48-32,32},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_2,1,0,728+120-48-32-24,24},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_3,1,0,728+120-48-32-24-16,16},
              {SOC_PPC_FP_QUAL_IRPP_ELK_LKP_PAYLOAD_LSB,1,0,856,128},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_4,1,0,856+128-32,32},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_5,1,0,856+128-32-32,32},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_6,1,0,856+128-32-32-24,24},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_7,1,0,856+128-32-32-24-16,16},
              {SOC_PPC_FP_QUAL_IRPP_IN_RIF_PROFILE,1,0,984,6},
              {SOC_PPC_FP_QUAL_IRPP_RPF_STAMP_NATIVE_VSI,1,0,992,1},
              {SOC_PPC_FP_QUAL_IRPP_RPF_NATIVE_VSI,1,0,996,15},
              {SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_RESULT,0,1,0,45},
              {SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND,0,1,48,1},
              {SOC_PPC_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR,0,1,56,64},
              {SOC_PPC_FP_QUAL_IRPP_SRC_PP_PORT,0,1,120,9},    
              {SOC_PPC_FP_QUAL_IRPP_PMF_PGM_KEY_GEN_VAR,0,1,136,32},
              {SOC_PPC_FP_QUAL_IRPP_PTC_KEY_GEN_VAR,0,1,168,16},
              {SOC_PPC_FP_QUAL_IRPP_PACKET_SIZE_RANGE,0,1,184,2},
              {SOC_PPC_FP_QUAL_STAMP_NATIVE_VSI,0,1,192,1},
              {SOC_PPC_FP_QUAL_NATIVE_VSI,0,1,200,15},
              {SOC_PPC_FP_QUAL_COUNTER_UPDATE_A,0,1,216,1},
              {SOC_PPC_FP_QUAL_COUNTER_POINTER_A,0,1,224,21},
              {SOC_PPC_FP_QUAL_CPU_TRAP_CODE_PROFILE,0,1,248,1},
              {SOC_PPC_FP_QUAL_COUNTER_UPDATE_B,0,1,256,1},
              {SOC_PPC_FP_QUAL_COUNTER_POINTER_B,0,1,264,21},
              {SOC_PPC_FP_QUAL_PROGRAM_INDEX,0,1,288,5},
              {SOC_PPC_FP_QUAL_VID_VALID,0,1,296,1},
              {SOC_PPC_FP_QUAL_LEM_2ND_LOOKUP_FOUND,0,1,304,1},
              {SOC_PPC_FP_QUAL_LEM_2ND_LOOKUP_RESULT,0,1,312,45},
              {SOC_PPC_FP_QUAL_IRPP_LEM_2ND_LKUP_ASD,0,1,312,45}, 
              {SOC_PPC_FP_QUAL_LPM_1ST_LOOKUP_RESULT,0,1,360,20},
              {SOC_PPC_FP_QUAL_LPM_1ST_LOOKUP_FOUND,0,1,380,1},
              {SOC_PPC_FP_QUAL_LPM_2ND_LOOKUP_RESULT,0,1,384,20},
              {SOC_PPC_FP_QUAL_LPM_2ND_LOOKUP_FOUND,0,1,404,1},
              {SOC_PPC_FP_QUAL_IRPP_TCAM0_MATCH,0,1,408,1},
              {SOC_PPC_FP_QUAL_TCAM_MATCH,0,1,408,1},
              {SOC_PPC_FP_QUAL_IRPP_TCAM0_RESULT,0,1,416,48},
              {SOC_PPC_FP_QUAL_TCAM_RESULT,0,1,416,48},
              {SOC_PPC_FP_QUAL_IRPP_TCAM1_MATCH,0,1,464,1},
              {SOC_PPC_FP_QUAL_IRPP_TCAM1_RESULT,0,1,472,48},
              {SOC_PPC_FP_QUAL_TCAM_TRAPS0_MATCH,0,1,520,1},
              {SOC_PPC_FP_QUAL_TCAM_TRAPS0_RESULT,0,1,528,48},
              {SOC_PPC_FP_QUAL_TCAM_TRAPS1_MATCH,0,1,576,1},
              {SOC_PPC_FP_QUAL_TCAM_TRAPS1_RESULT,0,1,584,48},
              {SOC_PPC_FP_QUAL_TT_PROCESSING_PROFILE,0,1,632,3},
              {SOC_PPC_FP_QUAL_TT_LOOKUP0_FOUND,0,1,640,1},
              {SOC_PPC_FP_QUAL_TT_LOOKUP0_PAYLOAD,0,1,648,17},
              {SOC_PPC_FP_QUAL_TT_LOOKUP1_FOUND,0,1,672,1},
              {SOC_PPC_FP_QUAL_TT_LOOKUP1_PAYLOAD,0,1,680,17},
              {SOC_PPC_FP_QUAL_VT_PROCESSING_PROFILE,0,1,704,3},
              {SOC_PPC_FP_QUAL_VT_LOOKUP0_FOUND,0,1,712,1},
              {SOC_PPC_FP_QUAL_VT_LOOKUP0_PAYLOAD,0,1,720,17},
              {SOC_PPC_FP_QUAL_VT_LOOKUP1_FOUND,0,1,744,1},
              {SOC_PPC_FP_QUAL_VT_LOOKUP1_PAYLOAD,0,1,752,17},
              {SOC_PPC_FP_QUAL_DA_IS_BPDU,0,1,776,1},
              {SOC_PPC_FP_QUAL_HDR_IPV4_L4OPS_LOW,0,1,784,16}, 
              {SOC_PPC_FP_QUAL_HDR_IPV4_L4OPS_HI,0,1,800,8}, 
              {SOC_PPC_FP_QUAL_HDR_IPV6_L4OPS,0,1,784,24}, 
              {SOC_PPC_FP_QUAL_PACKET_IS_IEEE1588,0,1,808,1},
              {SOC_PPC_FP_QUAL_IEEE1588_ENCAPSULATION,0,1,816,1},
              {SOC_PPC_FP_QUAL_IEEE1588_COMPENSATE_TIME_STAMP,0,1,824,1},
              {SOC_PPC_FP_QUAL_IEEE1588_COMMAND,0,1,832,2},
              {SOC_PPC_FP_QUAL_IEEE1588_HEADER_OFFSET,0,1,840,7},
              {SOC_PPC_FP_QUAL_OAM_UP_MEP,0,1,848,1},
              {SOC_PPC_FP_QUAL_OAM_SUB_TYPE,0,1,856,3},
              {SOC_PPC_FP_QUAL_OAM_OFFSET,0,1,864,7},
              {SOC_PPC_FP_QUAL_OAM_STAMP_OFFSET,0,1,872,7},
              {SOC_PPC_FP_QUAL_OAM_ID,0,1,880,17},
              {SOC_PPC_FP_QUAL_OAM_METER_DISABLE,0,1,904,1},
              {SOC_PPC_FP_QUAL_IRPP_IN_LIF_DATA,0,1,912,58},
              {SOC_PPC_FP_QUAL_IRPP_IN_LIF_DATA_INDEX,0,1,976,2},
              {SOC_PPC_FP_QUAL_IRPP_LOCAL_IN_LIF,0,1,984,17},
              {SOC_PPC_FP_QUAL_IRPP_PACKET_FORMAT_CODE_ACL,0,1,1008,6},
              {SOC_PPC_FP_QUAL_OAM_UP_MEP,0,1,848,1},
              {SOC_PPC_FP_QUAL_OAM_SUB_TYPE,0,1,856,3},
              {SOC_PPC_FP_QUAL_OAM_OFFSET,0,1,864,7},
              {SOC_PPC_FP_QUAL_OAM_STAMP_OFFSET,0,1,872,7},
              {SOC_PPC_FP_QUAL_OAM_ID,0,1,880,17},
              {SOC_PPC_FP_QUAL_OAM_METER_DISABLE,0,1,904,1},
              {SOC_PPC_FP_QUAL_VLAN_DOMAIN, 1, 1, 0, 9 },
              {SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID, 1, 1, 0, 12 },
              {SOC_PPC_FP_QUAL_CMPRSD_INNER_VID, 1, 1, 0, 12 },
              {SOC_PPC_FP_QUAL_INITIAL_VID, 1, 1, 0, 12 },
              {SOC_PPC_FP_QUAL_IRPP_PRESEL_ID,1,0,0,32}, 
              {SOC_PPC_FP_QUAL_IRPP_PRESEL_PROFILE0,1,0,0,2},
              {SOC_PPC_FP_QUAL_IRPP_PRESEL_PROFILE1,1,0,0,2},
              {SOC_PPC_FP_QUAL_IRPP_PRESEL_PROFILE2,1,0,0,2},
              {SOC_PPC_FP_QUAL_IRPP_PRESEL_PROFILE3,1,0,0,2},
              {SOC_PPC_FP_QUAL_IRPP_PRESEL_PROFILE_KAPS,1,0,0,2},
              {SOC_PPC_FP_QUAL_IRPP_PTC_KEY_GEN_VAR_PS ,1,1,0,3}, 
              {SOC_PPC_FP_QUAL_KEY_AFTER_HASHING,0,1,0,80},
              {SOC_PPC_FP_QUAL_IRPP_PEM_GENERAL_DATA,0,1,0,64}, 
        };


CONST STATIC 
    ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES
        Qax_pmf_ce_irpp_pmf_info[] = {
              {SOC_PPC_FP_QUAL_IRPP_INVALID,1,1,0,0},
	      {SOC_PPC_FP_QUAL_OUT_LIF_RANGE,1,1,0,2}, 
              {SOC_PPC_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR_PS,1,1,0,3}, 
              {SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES,1,0,0,32},
              {SOC_PPC_FP_QUAL_IRPP_KEY_CHANGED,1,0,0,32}, 
              {SOC_PPC_FP_QUAL_IRPP_PRESEL_ID,1,0,0,32}, 
              {SOC_PPC_FP_QUAL_IRPP_PRESEL_PROFILE0,1,0,0,2},
              {SOC_PPC_FP_QUAL_IRPP_PRESEL_PROFILE1,1,0,0,2},
              {SOC_PPC_FP_QUAL_IRPP_PRESEL_PROFILE2,1,0,0,2},
              {SOC_PPC_FP_QUAL_IRPP_PRESEL_PROFILE3,1,0,0,2},
              {SOC_PPC_FP_QUAL_IRPP_PRESEL_PROFILE_KAPS,1,0,0,2},
              {SOC_PPC_FP_QUAL_IS_EQUAL,0,1,0,4}, 
              {SOC_PPC_FP_QUAL_IRPP_ALL_ONES,1,0,32,32},
              
              {SOC_PPC_FP_QUAL_IRPP_PKT_HDR_TYPE,1,0,72,6},
              {SOC_PPC_FP_QUAL_IRPP_SRC_TM_PORT,1,0,80,8},
              {SOC_PPC_FP_QUAL_PACKET_HEADER_SIZE,1,0,88,7},
              {SOC_PPC_FP_QUAL_HEADER_OFFSET0,1,0,96,7},
              {SOC_PPC_FP_QUAL_HEADER_OFFSET1,1,0,104,7},
              {SOC_PPC_FP_QUAL_HEADER_OFFSET2,1,0,112,7},
              {SOC_PPC_FP_QUAL_HEADER_OFFSET3,1,0,120,7},
              {SOC_PPC_FP_QUAL_HEADER_OFFSET4,1,0,128,7},
              {SOC_PPC_FP_QUAL_HEADER_OFFSET5,1,0,136,7},
              {SOC_PPC_FP_QUAL_HEADER_OFFSET_0_UNTIL_5,1,0,96,(8*6)},
              {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER0,1,0,144,3},
              {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1,1,0,148,11},
              {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_NEXT_PROTOCOL,1,0,148+7,4}, 
              {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_ENCAPSULATION,1,0,148+5,2}, 
              {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2,1,0,160,11},
              {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_BOS,1,0,160,1} ,
              {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_NEXT_PROTOCOL,1,0,160+7,4}, 
              {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_IP_FRAGMENTED,1,0,160+6,1}, 
              {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_IP_HAS_OPTIONS,1,0,160+4,1}, 
              {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER3,1,0,172,11},
              {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER3_IP_FRAGMENTED,1,0,172+6,1}, 
              {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER4,1,0,184,11},
              {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER5,1,0,196,11},
              {SOC_PPC_FP_QUAL_IRPP_SRC_SYST_PORT,1,0,208,16},
              {SOC_PPC_FP_QUAL_IRPP_PARSER_LEAF_CONTEXT,1,0,224,4},
              {SOC_PPC_FP_QUAL_IRPP_ETH_TAG_FORMAT,1,0,228,4},
              {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DEST,1,0,232,19},
              {SOC_PPC_FP_QUAL_FORWARDING_ACTION_STRENGTH,1,0,252,3},
              {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_TC,1,0,256,3},
              {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_DP,1,0,260,2},
              {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_CODE,1,0,264,8},
              {SOC_PPC_FP_QUAL_IRPP_FWD_DEC_CPU_TRAP_QUAL,1,0,272,16},
              {SOC_PPC_FP_QUAL_FORWARDING_ACTION_METER_TRAFFIC_CLASS,1,0,288,2},
              {SOC_PPC_FP_QUAL_IRPP_UP,1,0,292,3},
              {SOC_PPC_FP_QUAL_IRPP_SNOOP_CODE,1,0,296,8},
              {SOC_PPC_FP_QUAL_SNOOP_STRENGTH,1,0,304,2},
              {SOC_PPC_FP_QUAL_IRPP_LL_MIRROR_CMD,1,0,308,4},
              {SOC_PPC_FP_QUAL_IRPP_SYSTEM_VSI,1,0,312,16},
              {SOC_PPC_FP_QUAL_VSI_PROFILE,1,0,328+2,2}, 
              {SOC_PPC_FP_QUAL_FID,1,0,332,15},
              {SOC_PPC_FP_QUAL_IRPP_ORIENTATION_IS_HUB,1,0,348,1},
              {SOC_PPC_FP_QUAL_VLAN_EDIT_CMD_COMMAND,1,0,352,6},
              {SOC_PPC_FP_QUAL_IRPP_VLAN_DEI,1,0,360,1},
              {SOC_PPC_FP_QUAL_IRPP_VLAN_PCP,1,0,361,3},
              {SOC_PPC_FP_QUAL_VLAN_EDIT_CMD_VID2,1,0,364,12},
              {SOC_PPC_FP_QUAL_VLAN_EDIT_CMD_VID1,1,0,376,12},
              {SOC_PPC_FP_QUAL_IRPP_STP_STATE,1,0,388,2},
              {SOC_PPC_FP_QUAL_IRPP_FWD_TYPE,1,0,392,4},
              {SOC_PPC_FP_QUAL_FORWARDING_OFFSET_EXTENSION,1,0,396,2},
              {SOC_PPC_FP_QUAL_IRPP_SUB_HEADER_NDX,1,0,400,3},
              {SOC_PPC_FP_QUAL_IRPP_TERM_TYPE,1,0,404,4},
              {SOC_PPC_FP_QUAL_FORWARDING_HEADER_ENCAPSULATION,1,0,408,2},
              {SOC_PPC_FP_QUAL_IGNORE_CP,1,0,412,1},
              {SOC_PPC_FP_QUAL_SEQUENCE_NUMBER_TAG,1,0,416,16},
              {SOC_PPC_FP_QUAL_EEI,1,0,432,24},
              {SOC_PPC_FP_QUAL_OUT_LIF,1,0,456,18},
              {SOC_PPC_FP_QUAL_IRPP_IN_RIF,1,0,476,15},
              {SOC_PPC_FP_QUAL_IRPP_VRF,1,0,492,14},
              {SOC_PPC_FP_QUAL_IRPP_PCKT_IS_COMP_MC,1,0,508,1},
              {SOC_PPC_FP_QUAL_IRPP_IN_TTL,1,0,512,8},
              {SOC_PPC_FP_QUAL_IRPP_IN_DSCP_EXP,1,0,520,8},
              {SOC_PPC_FP_QUAL_RPF_DESTINATION_VALID,1,0,528,1},
              {SOC_PPC_FP_QUAL_RPF_DESTINATION,1,0,536,19},
              {SOC_PPC_FP_QUAL_INGRESS_LEARN_ENABLE,1,0,556,1},
              {SOC_PPC_FP_QUAL_EGRESS_LEARN_ENABLE,1,0,560,1},
              {SOC_PPC_FP_QUAL_LEARN_OR_TRANSPLANT,1,0,564,1},
              {SOC_PPC_FP_QUAL_LEARN_DATA,1,0,568,40},
              {SOC_PPC_FP_QUAL_IRPP_LEARN_DECISION_DEST,1,0,568,19},
              {SOC_PPC_FP_QUAL_IRPP_LEARN_ADD_INFO,1,0,587,21},
              {SOC_PPC_FP_QUAL_LEARN_KEY,1,0,608,74},
              {SOC_PPC_FP_QUAL_LEARN_KEY_MAC,1,0,608,48},
              {SOC_PPC_FP_QUAL_LEARN_KEY_VLAN,1,0,608+48,16}, 
              {SOC_PPC_FP_QUAL_IRPP_IN_LIF,1,0,688,18},
              {SOC_PPC_FP_QUAL_IN_LIF_PROFILE,1,0,708,4},
              {SOC_PPC_FP_QUAL_IN_LIF_PROFILE_SAME_IF,1,0,708,1}, 
              {SOC_PPC_FP_QUAL_PACKET_IS_BOOTP_DHCP,1,0,712,1},
              {SOC_PPC_FP_QUAL_UNKNOWN_ADDR,1,0,716,1},
              {SOC_PPC_FP_QUAL_FWD_PRCESSING_PROFILE,1,0,720,3},
              {SOC_PPC_FP_QUAL_ELK_ERROR,1,0,724,1},
              {SOC_PPC_FP_QUAL_ELK_LKP_PAYLOAD,1,0,728,128},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_0,1,0,728+127,1},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_1,1,0,728+126,1},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_2,1,0,728+125,1},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_3,1,0,728+124,1},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_4,1,0,728+123,1},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_5,1,0,728+122,1},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_6,1,0,728+121,1},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_FOUND_7,1,0,728+120,1},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_0,1,0,728+120-48,48},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_1,1,0,728+120-48-32,32},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_2,1,0,728+120-48-32-16,16},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_3,1,0,728+120-48-32-16-24,24},
              {SOC_PPC_FP_QUAL_IRPP_ELK_LKP_PAYLOAD_LSB,1,0,856,128},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_4,1,0,856+128-32,32},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_5,1,0,856+128-32-32,32},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_6,1,0,856+128-32-32-24,24},
               {SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_7,1,0,856+128-32-32-24-16,16},
              {SOC_PPC_FP_QUAL_IRPP_IN_RIF_PROFILE,1,0,984,6},
              {SOC_PPC_FP_QUAL_IRPP_RPF_STAMP_NATIVE_VSI,1,0,992,1},
              {SOC_PPC_FP_QUAL_IRPP_RPF_NATIVE_VSI,1,0,996,15},
              {SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_RESULT,0,1,0,45},
              {SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND,0,1,48,1},
              {SOC_PPC_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR,0,1,56,64},
              {SOC_PPC_FP_QUAL_IRPP_SRC_PP_PORT,0,1,120,9},    
              {SOC_PPC_FP_QUAL_IRPP_PMF_PGM_KEY_GEN_VAR,0,1,136,32},
              {SOC_PPC_FP_QUAL_IRPP_PTC_KEY_GEN_VAR,0,1,168,16},
              {SOC_PPC_FP_QUAL_IRPP_PACKET_SIZE_RANGE,0,1,184,2},
              {SOC_PPC_FP_QUAL_STAMP_NATIVE_VSI,0,1,188,1},
              {SOC_PPC_FP_QUAL_NATIVE_VSI,0,1,192,15},
              {SOC_PPC_FP_QUAL_COUNTER_UPDATE_A,0,1,208,1},
              {SOC_PPC_FP_QUAL_COUNTER_POINTER_A,0,1,216,21},
              {SOC_PPC_FP_QUAL_CPU_TRAP_CODE_PROFILE,0,1,240,1},
              {SOC_PPC_FP_QUAL_COUNTER_UPDATE_B,0,1,244,1},
              {SOC_PPC_FP_QUAL_COUNTER_POINTER_B,0,1,248,21},
              {SOC_PPC_FP_QUAL_PROGRAM_INDEX,0,1,272,5},
              {SOC_PPC_FP_QUAL_VID_VALID,0,1,280,1},
              {SOC_PPC_FP_QUAL_LEM_2ND_LOOKUP_FOUND,0,1,284,1},
              {SOC_PPC_FP_QUAL_LEM_2ND_LOOKUP_RESULT,0,1,288,45},
              {SOC_PPC_FP_QUAL_IRPP_LEM_2ND_LKUP_ASD,0,1,288,45}, 
              {SOC_PPC_FP_QUAL_LPM_1ST_LOOKUP_RESULT,0,1,336,20},
              {SOC_PPC_FP_QUAL_LPM_1ST_LOOKUP_FOUND,0,1,356,1},
              {SOC_PPC_FP_QUAL_LPM_2ND_LOOKUP_RESULT,0,1,360,20},
              {SOC_PPC_FP_QUAL_LPM_2ND_LOOKUP_FOUND,0,1,380,1},
              {SOC_PPC_FP_QUAL_IRPP_TCAM0_MATCH,0,1,384,1},
              {SOC_PPC_FP_QUAL_TCAM_MATCH,0,1,384,1},
              {SOC_PPC_FP_QUAL_IRPP_TCAM0_RESULT,0,1,392,48},
              {SOC_PPC_FP_QUAL_TCAM_RESULT,0,1,392,48},
              {SOC_PPC_FP_QUAL_IRPP_TCAM1_MATCH,0,1,440,1},
              {SOC_PPC_FP_QUAL_IRPP_TCAM1_RESULT,0,1,448,48},
              {SOC_PPC_FP_QUAL_TCAM_TRAPS0_MATCH,0,1,496,1},
              {SOC_PPC_FP_QUAL_TCAM_TRAPS0_RESULT,0,1,504,48},
              {SOC_PPC_FP_QUAL_TCAM_TRAPS1_MATCH,0,1,552,1},
              {SOC_PPC_FP_QUAL_TCAM_TRAPS1_RESULT,0,1,560,48},
              {SOC_PPC_FP_QUAL_TT_PROCESSING_PROFILE,0,1,608,3},
              {SOC_PPC_FP_QUAL_TT_LOOKUP0_FOUND,0,1,612,1},
              {SOC_PPC_FP_QUAL_TT_LOOKUP0_PAYLOAD,0,1,616,17},
              {SOC_PPC_FP_QUAL_TT_LOOKUP1_FOUND,0,1,636,1},
              {SOC_PPC_FP_QUAL_TT_LOOKUP1_PAYLOAD,0,1,640,17},
              {SOC_PPC_FP_QUAL_VT_PROCESSING_PROFILE,0,1,660,3},
              {SOC_PPC_FP_QUAL_VT_LOOKUP0_FOUND,0,1,664,1},
              {SOC_PPC_FP_QUAL_VT_LOOKUP0_PAYLOAD,0,1,672,17},
              {SOC_PPC_FP_QUAL_VT_LOOKUP1_FOUND,0,1,692,1},
              {SOC_PPC_FP_QUAL_VT_LOOKUP1_PAYLOAD,0,1,696,17},
              {SOC_PPC_FP_QUAL_DA_IS_BPDU,0,1,716,1},
              {SOC_PPC_FP_QUAL_HDR_IPV4_L4OPS_LOW,0,1,720,16}, 
              {SOC_PPC_FP_QUAL_HDR_IPV4_L4OPS_HI,0,1,736,8}, 
              {SOC_PPC_FP_QUAL_HDR_IPV6_L4OPS,0,1,720,24}, 
              {SOC_PPC_FP_QUAL_PACKET_IS_IEEE1588,0,1,744,1},
              {SOC_PPC_FP_QUAL_IEEE1588_ENCAPSULATION,0,1,748,1},
              {SOC_PPC_FP_QUAL_IEEE1588_COMPENSATE_TIME_STAMP,0,1,752,1},
              {SOC_PPC_FP_QUAL_IEEE1588_COMMAND,0,1,756,2},
              {SOC_PPC_FP_QUAL_IEEE1588_HEADER_OFFSET,0,1,760,7},
              {SOC_PPC_FP_QUAL_OAM_UP_MEP,0,1,768,1},
              {SOC_PPC_FP_QUAL_OAM_SUB_TYPE,0,1,772,3},
              {SOC_PPC_FP_QUAL_OAM_OFFSET,0,1,776,7},
              {SOC_PPC_FP_QUAL_OAM_STAMP_OFFSET,0,1,784,7},
              {SOC_PPC_FP_QUAL_OAM_ID,0,1,792,17},
              {SOC_PPC_FP_QUAL_OAM_METER_DISABLE,0,1,812,1},
              {SOC_PPC_FP_QUAL_IRPP_IN_LIF_DATA,0,1,816,58},
              {SOC_PPC_FP_QUAL_IRPP_IN_LIF_DATA_INDEX,0,1,876,2},
              {SOC_PPC_FP_QUAL_IRPP_LOCAL_IN_LIF,0,1,880,17},
              {SOC_PPC_FP_QUAL_IRPP_PACKET_FORMAT_CODE_ACL,0,1,900,6},
              {SOC_PPC_FP_QUAL_IRPP_PEM_GENERAL_DATA,0,1,912,64}, 
              {SOC_PPC_FP_QUAL_OAM_UP_MEP,0,1,768,1},
              {SOC_PPC_FP_QUAL_OAM_SUB_TYPE,0,1,772,3},
              {SOC_PPC_FP_QUAL_OAM_OFFSET,0,1,776,7},
              {SOC_PPC_FP_QUAL_OAM_STAMP_OFFSET,0,1,784,7},
              {SOC_PPC_FP_QUAL_OAM_ID,0,1,792,17},
              {SOC_PPC_FP_QUAL_OAM_METER_DISABLE,0,1,812,1},
              {SOC_PPC_FP_QUAL_VLAN_DOMAIN, 1, 1, 0, 9 },
              {SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID, 1, 1, 0, 12 },
              {SOC_PPC_FP_QUAL_CMPRSD_INNER_VID, 1, 1, 0, 12 },
              {SOC_PPC_FP_QUAL_INITIAL_VID, 1, 1, 0, 12 },
              {SOC_PPC_FP_QUAL_IRPP_PTC_KEY_GEN_VAR_PS ,1,1,0,3}, 
              {SOC_PPC_FP_QUAL_KEY_AFTER_HASHING,0,1,0,80},
              {SOC_PPC_FP_QUAL_IRPP_TCAM_0_RESULT,0,0,0 ,48}, 
              {SOC_PPC_FP_QUAL_IRPP_TCAM_1_RESULT,0,0,48 ,48}, 
              {SOC_PPC_FP_QUAL_IRPP_TCAM_2_RESULT,0,0,96 ,48}, 
              {SOC_PPC_FP_QUAL_IRPP_TCAM_3_RESULT,0,0,144 ,48}, 
              
              {SOC_PPC_FP_QUAL_IRPP_KAPS_PASS1_PAYLOAD,1,1,320 ,64},
              

        };


CONST STATIC 
    ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES
        Arad_pmf_ce_egress_pmf_info[] = {
           {SOC_PPC_FP_QUAL_IRPP_INVALID, 1, 1, 0, 0},
           {SOC_PPC_FP_QUAL_ERPP_OUT_PP_PORT_PMF_DATA_PS, 1, 1, 0, 3}, 
           {SOC_PPC_FP_QUAL_ERPP_ONES, 1, 1, 0, 80},
           {SOC_PPC_FP_QUAL_ERPP_ZEROES, 1, 1, 80, 80},
           {SOC_PPC_FP_QUAL_ERPP_OAM_TS, 1, 1, 160, 48},

           {SOC_PPC_FP_QUAL_ERPP_LEARN_EXT_SRC_PORT, 1, 1, 208, 16},
           {SOC_PPC_FP_QUAL_ERPP_LEARN_EXT_IN_VPORT, 1, 1, 224, 24},
           {SOC_PPC_FP_QUAL_ERPP_FHEI, 1, 1, 248, 40},
           {SOC_PPC_FP_QUAL_ERPP_FHEI_IPV4_TTL, 1, 1, 248, 8}, 
           {SOC_PPC_FP_QUAL_ERPP_FHEI_DSCP, 1, 1, 248+8, 8}, 
           {SOC_PPC_FP_QUAL_ERPP_FHEI_EXP, 1, 1, 248+8, 3}, 
           {SOC_PPC_FP_QUAL_ERPP_OUT_TM_PORT_PMF_DATA, 1, 1, 288, 32},
           {SOC_PPC_FP_QUAL_ERPP_OUT_PP_PORT_PMF_DATA, 1, 1, 320, 32},
           {SOC_PPC_FP_QUAL_ERPP_EEI, 1, 1, 352, 24},
           {SOC_PPC_FP_QUAL_ERPP_EXT_IN_LIF, 1, 1, 376, 24},
           {SOC_PPC_FP_QUAL_ERPP_EXT_OUT_LIF, 1, 1, 400, 24},
           {SOC_PPC_FP_QUAL_ERPP_STACKING_ROUTE_HISTORY_BITMAP, 1, 1, 424, 16},
           {SOC_PPC_FP_QUAL_ERPP_DSP_EXT, 1, 1, 440, 16},
           {SOC_PPC_FP_QUAL_ERPP_PACKET_SIZE, 1, 1, 456, 16},
           {SOC_PPC_FP_QUAL_ERPP_DST_SYSTEM_PORT, 1, 1, 472, 16},
           {SOC_PPC_FP_QUAL_ERPP_SRC_SYSTEM_PORT, 1, 1, 488, 16},
           {SOC_PPC_FP_QUAL_ERPP_VSI_OR_VRF, 1, 1, 504, 16},
           {SOC_PPC_FP_QUAL_ERPP_VSI_OR_VRF_ORIG, 1, 1, 520, 16},
           {SOC_PPC_FP_QUAL_ERPP_FWD_OFFSET, 1, 1, 536, 8},
           {SOC_PPC_FP_QUAL_ERPP_ETH_TAG_FORMAT, 1, 1, 544, 5},
           {SOC_PPC_FP_QUAL_ERPP_SYS_VALUE1, 1, 1, 552, 8},
           {SOC_PPC_FP_QUAL_ERPP_SYS_VALUE2, 1, 1, 560, 8},
           {SOC_PPC_FP_QUAL_ERPP_DSP_PTR_ORIG, 1, 1, 584, 8},
           {SOC_PPC_FP_QUAL_ERPP_DSP_PTR, 1, 1, 592, 8},
           {SOC_PPC_FP_QUAL_ERPP_OUT_TM_PORT, 1, 1, 600, 8},
           {SOC_PPC_FP_QUAL_ERPP_OUT_PP_PORT, 1, 1, 608, 8},
           {SOC_PPC_FP_QUAL_ERPP_LB_KEY, 1, 1, 616, 8},
           {SOC_PPC_FP_QUAL_ERPP_TC, 1, 1, 624, 4},
           {SOC_PPC_FP_QUAL_ERPP_FORMAT_CODE, 1, 1, 628, 4},
           {SOC_PPC_FP_QUAL_ERPP_FWD_CODE, 1, 1, 632, 4},
           {SOC_PPC_FP_QUAL_ERPP_FWD_CODE_ORIG, 1, 1, 636, 4},
           {SOC_PPC_FP_QUAL_ERPP_ACTION_PROFILE, 1, 1, 640, 4},
           {SOC_PPC_FP_QUAL_ERPP_HEADER_CODE, 1, 1, 644, 4},
           {SOC_PPC_FP_QUAL_ERPP_ETH_TYPE_CODE, 1, 1, 648, 4},
           {SOC_PPC_FP_QUAL_ERPP_IN_LIF_ORIENTATION, 1, 1, 652, 2},
           {SOC_PPC_FP_QUAL_ERPP_SNOOP_CPU_CODE, 1, 1, 654, 2},
           {SOC_PPC_FP_QUAL_ERPP_FTMH_RESERVED, 1, 1, 656, 1},
           {SOC_PPC_FP_QUAL_ERPP_ECN_CAPABLE, 1, 1, 657, 1},
           {SOC_PPC_FP_QUAL_ERPP_PPH_TYPE, 1, 1, 658, 2},
           {SOC_PPC_FP_QUAL_ERPP_TM_ACTION_TYPE, 1, 1, 660, 2},
           {SOC_PPC_FP_QUAL_ERPP_DP, 1, 1, 662, 2},
           {SOC_PPC_FP_QUAL_ERPP_FHEI_CODE, 1, 1, 664, 2},
           {SOC_PPC_FP_QUAL_ERPP_LEARN_ALLOWED, 1, 1, 666, 1},
           {SOC_PPC_FP_QUAL_ERPP_UNKNOWN_ADDR, 1, 1, 667, 1},
           {SOC_PPC_FP_QUAL_ERPP_LEARN_EXT_VALID, 1, 1, 668, 1},
           {SOC_PPC_FP_QUAL_ERPP_BYPASS_FILTERING, 1, 1, 669, 1},
           {SOC_PPC_FP_QUAL_ERPP_EEI_VALID, 1, 1, 670, 1},
           {SOC_PPC_FP_QUAL_ERPP_CNI, 1, 1, 671, 1},
           {SOC_PPC_FP_QUAL_ERPP_DSP_EXT_VALID, 1, 1, 672, 1},
           {SOC_PPC_FP_QUAL_ERPP_SYSTEM_MC, 1, 1, 673, 1},
           {SOC_PPC_FP_QUAL_ERPP_OUT_MIRROR_DISABLE, 1, 1, 674, 1},
           {SOC_PPC_FP_QUAL_ERPP_EXCLUDE_SRC, 1, 1, 675, 1},
           {SOC_PPC_FP_QUAL_ERPP_DISCARD, 1, 1, 676, 1},
           {SOC_PPC_FP_QUAL_ERPP_FABRIC_OR_EGRESS_MC, 1, 1, 677, 1},
           {SOC_PPC_FP_QUAL_ERPP_RESEVED, 1, 1, 678, 2},
           {SOC_PPC_FP_QUAL_ERPP_FIRST_COPY, 1, 1, 992, 1}, 
           {SOC_PPC_FP_QUAL_ERPP_LAST_COPY, 1, 1, 994, 1}, 
           {SOC_PPC_FP_QUAL_ERPP_START_BUFFER, 1, 1, 996, 14},
           {SOC_PPC_FP_QUAL_ERPP_CONTEXT, 1, 1, 1010, 14},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_DA, 1, 1, 680, 48},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_SA, 1, 1, 728, 48},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_DATA, 1, 1, 792, 80},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_ADDITIONAL_DATA, 1, 1, 776, 3},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_CPID0, 1, 1, 780, 4},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_CPID1, 1, 1, 784, 4},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_CPID2, 1, 1, 788, 4},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TPID, 1, 1, 792+16+32+16, 16},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TAG, 1, 1, 792+16+32, 16},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TAG_ID, 1, 1, 792+16+32+15-ARAD_PMF_KEY_VLAN_TAG_ID_LSB, ARAD_PMF_KEY_VLAN_TAG_ID_LSB - ARAD_PMF_KEY_VLAN_TAG_ID_MSB + 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TAG_PRI, 1, 1, 792+16+32+15-ARAD_PMF_KEY_VLAN_TAG_PRI_LSB, ARAD_PMF_KEY_VLAN_TAG_PRI_LSB - ARAD_PMF_KEY_VLAN_TAG_PRI_MSB + 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TAG_CFI, 1, 1, 792+16+32+15-ARAD_PMF_KEY_VLAN_TAG_CFI_LSB, ARAD_PMF_KEY_VLAN_TAG_CFI_LSB - ARAD_PMF_KEY_VLAN_TAG_CFI_MSB + 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TAG_PRI_CFI, 1, 1, 792+16+32+15-ARAD_PMF_KEY_VLAN_TAG_PRI_LSB, ARAD_PMF_KEY_VLAN_TAG_CFI_LSB - ARAD_PMF_KEY_VLAN_TAG_PRI_MSB + 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_INNER_TPID, 1, 1, 792+16+16, 16},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_INNER_TAG, 1, 1, 792+16, 16},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_INNER_TAG_ID, 1, 1, 792+16+15-ARAD_PMF_KEY_VLAN_TAG_ID_LSB, ARAD_PMF_KEY_VLAN_TAG_ID_LSB - ARAD_PMF_KEY_VLAN_TAG_ID_MSB + 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_INNER_TAG_PRI, 1, 1, 792+16+15-ARAD_PMF_KEY_VLAN_TAG_PRI_LSB, ARAD_PMF_KEY_VLAN_TAG_PRI_LSB - ARAD_PMF_KEY_VLAN_TAG_PRI_MSB + 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_INNER_TAG_CFI, 1, 1, 792+16+15-ARAD_PMF_KEY_VLAN_TAG_CFI_LSB, ARAD_PMF_KEY_VLAN_TAG_CFI_LSB - ARAD_PMF_KEY_VLAN_TAG_CFI_MSB + 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_OUTER_TAG, 1, 1, 248, 16}, 
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_OUTER_TAG_ID, 1, 1, 248+15-ARAD_PMF_KEY_VLAN_TAG_ID_LSB, ARAD_PMF_KEY_VLAN_TAG_ID_LSB - ARAD_PMF_KEY_VLAN_TAG_ID_MSB + 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_OUTER_TAG_PRI, 1, 1, 248+15-ARAD_PMF_KEY_VLAN_TAG_PRI_LSB, ARAD_PMF_KEY_VLAN_TAG_PRI_LSB - ARAD_PMF_KEY_VLAN_TAG_PRI_MSB + 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_OUTER_TAG_CFI, 1, 1, 248+15-ARAD_PMF_KEY_VLAN_TAG_CFI_LSB, ARAD_PMF_KEY_VLAN_TAG_CFI_LSB - ARAD_PMF_KEY_VLAN_TAG_CFI_MSB + 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_INNER_TAG, 1, 1, 248+24, 16}, 
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_INNER_TAG_ID, 1, 1, 248+24+15-ARAD_PMF_KEY_VLAN_TAG_ID_LSB, ARAD_PMF_KEY_VLAN_TAG_ID_LSB - ARAD_PMF_KEY_VLAN_TAG_ID_MSB + 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_INNER_TAG_PRI, 1, 1, 248+24+15-ARAD_PMF_KEY_VLAN_TAG_PRI_LSB, ARAD_PMF_KEY_VLAN_TAG_PRI_LSB - ARAD_PMF_KEY_VLAN_TAG_PRI_MSB + 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_INNER_TAG_CFI, 1, 1, 248+24+15-ARAD_PMF_KEY_VLAN_TAG_CFI_LSB, ARAD_PMF_KEY_VLAN_TAG_CFI_LSB - ARAD_PMF_KEY_VLAN_TAG_CFI_MSB + 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_BRIDGED_FORWARDING_DATA_MSB, 1, 1, 56+872, 64},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_BRIDGED_FORWARDING_DATA_LSB, 1, 1, 872, 56},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_BRIDGED_ACL_DATA_HIGH, 1, 1, 80, 40},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_BRIDGED_ACL_DATA_LOW, 1, 1, 0, 32},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_OPTIONS_PRESENT, 1, 1, 116+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_TOTAL_LEN_ERROR, 1, 1, 115+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_HEADER_LEN_ERROR, 1, 1, 114+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_CHECKSUM_ERROR, 1, 1, 113+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_VERSION_ERROR, 1, 1, 112+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_TOS, 1, 1, 104+872, 8},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_SIP, 1, 1, 72+872, 32},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_DIP, 1, 1, 40+872, 32},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_PROTOCOL, 1, 1, 32+872, 8},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_L4_DEST_PORT, 1, 1, 0+872, 16},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_L4_SRC_PORT, 1, 1, 16+872, 16},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_MSBS_TC, 1, 1, 116+872-ARAD_PMF_QUAL_HDR_IPV6_TC_LSB, 8},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_MSBS_HOP_LIMIT, 1, 1, 116+872-ARAD_PMF_QUAL_HDR_IPV6_HOP_LIMIT_LSB, 8},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_MSBS_FLOW_LABEL, 1, 1, 116+872-ARAD_PMF_QUAL_HDR_IPV6_FLOW_LABEL_LSB, 20},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_MSBS_NEXT_PROTOCOL, 1, 1, 116+872-ARAD_PMF_QUAL_HDR_IPV6_NEXT_PRTCL_LSB, 8},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_MC_DST, 1, 1, 12+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_IPV4_MAPPED_DST, 1, 1, 11+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_IPV4_CMP_DST, 1, 1, 10+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_SITE_LOCAL_SRC, 1, 1, 9+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_LINK_LOCAL_SRC, 1, 1, 8+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_SITE_LOCAL_DST, 1, 1, 7+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_LINK_LOCAL_DST, 1, 1, 6+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_NEXT_HEADER_IS_ZERO, 1, 1, 5+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_LOOPBACK_SRC_OR_DST, 1, 1, 4+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_SIP_IS_ALL_ZEROES, 1, 1, 3+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_DIP_IS_ALL_ZEROES, 1, 1, 2+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_SIP_IS_MC, 1, 1, 1+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_VERSION_ERROR, 1, 1, 0+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_MPLS_EXTRA_DATA, 1, 1, 32+872, 85},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_MPLS_HDR, 1, 1, 0+872, 32},
           {SOC_PPC_FP_QUAL_HDR_MPLS_LABEL_FWD, 1, 1, 0+872+31-ARAD_PMF_KEY_MPLS_LABEL_LSB, 32},
           {SOC_PPC_FP_QUAL_HDR_MPLS_LABEL_ID_FWD, 1, 1, 0+872+31-ARAD_PMF_KEY_MPLS_LABEL_ID_LSB, 20},
           {SOC_PPC_FP_QUAL_HDR_MPLS_TTL_FWD, 1, 1, 0+872+31-ARAD_PMF_KEY_MPLS_TTL_LSB, 8},
           {SOC_PPC_FP_QUAL_HDR_MPLS_BOS_FWD, 1, 1, 0+872+31-ARAD_PMF_KEY_MPLS_BOS_LSB, 1},
           {SOC_PPC_FP_QUAL_HDR_MPLS_EXP_FWD, 1, 1, 0+872+31-ARAD_PMF_KEY_MPLS_EXP_LSB, 3},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_TRILL_EXTRA_DATA, 1, 1, 48+872, 69},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_TRILL_HDR, 1, 1, 0+872, 48},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_TRILL_VERSION, 1, 1, 0+872, 2},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_TRILL_MULTI_DESTINATION, 1, 1, 0+872+4, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_TRILL_OP_LENGTH, 1, 1, 0+872+5, 5},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_TRILL_HOP_COUNT, 1, 1, 0+872+10, 6},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_TRILL_EGRESS_RBRIDGE, 1, 1, 0+872+16, 16},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_TRILL_INGRESS_RBRIDGE, 1, 1, 0+872+32, 16},
           {SOC_PPC_FP_QUAL_ERPP_OAM_TS,1,1,160,48},

        };
CONST STATIC 
    ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES
        Qax_pmf_ce_egress_pmf_info[] = {
           {SOC_PPC_FP_QUAL_IRPP_INVALID, 1, 1, 0, 0},
           {SOC_PPC_FP_QUAL_ERPP_OUT_PP_PORT_PMF_DATA_PS, 1, 1, 0, 3}, 
           {SOC_PPC_FP_QUAL_ERPP_ONES, 1, 1, 0, 80},
           {SOC_PPC_FP_QUAL_ERPP_ZEROES, 1, 1, 80, 80},
           {SOC_PPC_FP_QUAL_ERPP_OAM_TS, 1, 1, 160, 48},

           {SOC_PPC_FP_QUAL_ERPP_LEARN_EXT_SRC_PORT, 1, 1, 208, 16},
           {SOC_PPC_FP_QUAL_ERPP_LEARN_EXT_IN_VPORT, 1, 1, 224, 24},
           {SOC_PPC_FP_QUAL_ERPP_FHEI, 1, 1, 248, 40},
           {SOC_PPC_FP_QUAL_ERPP_FHEI_IPV4_TTL, 1, 1, 248, 8}, 
           {SOC_PPC_FP_QUAL_ERPP_FHEI_DSCP, 1, 1, 248+8, 8}, 
           {SOC_PPC_FP_QUAL_ERPP_FHEI_EXP, 1, 1, 248+8, 3}, 
           {SOC_PPC_FP_QUAL_ERPP_OUT_TM_PORT_PMF_DATA, 1, 1, 288, 32},
           {SOC_PPC_FP_QUAL_ERPP_OUT_PP_PORT_PMF_DATA, 1, 1, 320, 32},
           {SOC_PPC_FP_QUAL_ERPP_EEI, 1, 1, 352, 24},
           {SOC_PPC_FP_QUAL_ERPP_EXT_IN_LIF, 1, 1, 376, 24},
           {SOC_PPC_FP_QUAL_ERPP_EXT_OUT_LIF, 1, 1, 400, 24},
           {SOC_PPC_FP_QUAL_ERPP_STACKING_ROUTE_HISTORY_BITMAP, 1, 1, 424, 16},
           {SOC_PPC_FP_QUAL_ERPP_DSP_EXT, 1, 1, 440, 16},
           {SOC_PPC_FP_QUAL_ERPP_PACKET_SIZE, 1, 1, 456, 16},
           {SOC_PPC_FP_QUAL_ERPP_DST_SYSTEM_PORT, 1, 1, 472, 16},
           {SOC_PPC_FP_QUAL_ERPP_SRC_SYSTEM_PORT, 1, 1, 488, 16},
           {SOC_PPC_FP_QUAL_ERPP_VSI_OR_VRF, 1, 1, 504, 16},
           {SOC_PPC_FP_QUAL_ERPP_VSI_OR_VRF_ORIG, 1, 1, 520, 16},
           {SOC_PPC_FP_QUAL_ERPP_FWD_OFFSET, 1, 1, 536, 8},
           {SOC_PPC_FP_QUAL_ERPP_ETH_TAG_FORMAT, 1, 1, 544, 7},
           {SOC_PPC_FP_QUAL_ERPP_SYS_VALUE1, 1, 1, 552, 8},
           {SOC_PPC_FP_QUAL_ERPP_SYS_VALUE2, 1, 1, 560, 8},
           {SOC_PPC_FP_QUAL_ERPP_DSP_PTR_ORIG, 1, 1, 584, 8},
           {SOC_PPC_FP_QUAL_ERPP_DSP_PTR, 1, 1, 592, 8},
           {SOC_PPC_FP_QUAL_ERPP_OUT_TM_PORT, 1, 1, 600, 8},
           {SOC_PPC_FP_QUAL_ERPP_OUT_PP_PORT, 1, 1, 608, 8},
           {SOC_PPC_FP_QUAL_ERPP_LB_KEY, 1, 1, 616, 8},
           {SOC_PPC_FP_QUAL_ERPP_TC, 1, 1, 624, 4},
           {SOC_PPC_FP_QUAL_ERPP_FORMAT_CODE, 1, 1, 628, 4},
           {SOC_PPC_FP_QUAL_ERPP_FWD_CODE, 1, 1, 632, 4},
           {SOC_PPC_FP_QUAL_ERPP_FWD_CODE_ORIG, 1, 1, 636, 4},
           {SOC_PPC_FP_QUAL_ERPP_ACTION_PROFILE, 1, 1, 640, 4},
           {SOC_PPC_FP_QUAL_ERPP_HEADER_CODE, 1, 1, 644, 4},
           {SOC_PPC_FP_QUAL_ERPP_ETH_TYPE_CODE, 1, 1, 648, 4},
           {SOC_PPC_FP_QUAL_ERPP_IN_LIF_ORIENTATION, 1, 1, 652, 2},
           {SOC_PPC_FP_QUAL_ERPP_SNOOP_CPU_CODE, 1, 1, 654, 2},
           {SOC_PPC_FP_QUAL_ERPP_FTMH_RESERVED, 1, 1, 656, 1},
           {SOC_PPC_FP_QUAL_ERPP_ECN_CAPABLE, 1, 1, 657, 1},
           {SOC_PPC_FP_QUAL_ERPP_PPH_TYPE, 1, 1, 658, 2},
           {SOC_PPC_FP_QUAL_ERPP_TM_ACTION_TYPE, 1, 1, 660, 2},
           {SOC_PPC_FP_QUAL_ERPP_DP, 1, 1, 662, 2},
           {SOC_PPC_FP_QUAL_ERPP_FHEI_CODE, 1, 1, 664, 2},
           {SOC_PPC_FP_QUAL_ERPP_LEARN_ALLOWED, 1, 1, 666, 1},
           {SOC_PPC_FP_QUAL_ERPP_UNKNOWN_ADDR, 1, 1, 667, 1},
           {SOC_PPC_FP_QUAL_ERPP_LEARN_EXT_VALID, 1, 1, 668, 1},
           {SOC_PPC_FP_QUAL_ERPP_BYPASS_FILTERING, 1, 1, 669, 1},
           {SOC_PPC_FP_QUAL_ERPP_EEI_VALID, 1, 1, 670, 1},
           {SOC_PPC_FP_QUAL_ERPP_CNI, 1, 1, 671, 1},
           {SOC_PPC_FP_QUAL_ERPP_DSP_EXT_VALID, 1, 1, 672, 1},
           {SOC_PPC_FP_QUAL_ERPP_SYSTEM_MC, 1, 1, 673, 1},
           {SOC_PPC_FP_QUAL_ERPP_OUT_MIRROR_DISABLE, 1, 1, 674, 1},
           {SOC_PPC_FP_QUAL_ERPP_EXCLUDE_SRC, 1, 1, 675, 1},
           {SOC_PPC_FP_QUAL_ERPP_DISCARD, 1, 1, 676, 1},
           {SOC_PPC_FP_QUAL_ERPP_FABRIC_OR_EGRESS_MC, 1, 1, 677, 1},
           {SOC_PPC_FP_QUAL_ERPP_RESEVED, 1, 1, 678, 2},
           {SOC_PPC_FP_QUAL_ERPP_FIRST_COPY, 1, 1, 992, 1}, 
           {SOC_PPC_FP_QUAL_ERPP_LAST_COPY, 1, 1, 994, 1}, 
           {SOC_PPC_FP_QUAL_ERPP_START_BUFFER, 1, 1, 996, 14},
           {SOC_PPC_FP_QUAL_ERPP_CONTEXT, 1, 1, 1010, 14},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_DA, 1, 1, 680, 48},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_SA, 1, 1, 728, 48},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_DATA, 1, 1, 792, 80},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_ADDITIONAL_DATA, 1, 1, 776, 3},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_CPID0, 1, 1, 780, 4},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_CPID1, 1, 1, 784, 4},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_CPID2, 1, 1, 788, 4},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TPID, 1, 1, 792+16+32+16, 16},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TAG, 1, 1, 792+16+32, 16},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TAG_ID, 1, 1, 792+16+32+15-ARAD_PMF_KEY_VLAN_TAG_ID_LSB, ARAD_PMF_KEY_VLAN_TAG_ID_LSB - ARAD_PMF_KEY_VLAN_TAG_ID_MSB + 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TAG_PRI, 1, 1, 792+16+32+15-ARAD_PMF_KEY_VLAN_TAG_PRI_LSB, ARAD_PMF_KEY_VLAN_TAG_PRI_LSB - ARAD_PMF_KEY_VLAN_TAG_PRI_MSB + 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TAG_CFI, 1, 1, 792+16+32+15-ARAD_PMF_KEY_VLAN_TAG_CFI_LSB, ARAD_PMF_KEY_VLAN_TAG_CFI_LSB - ARAD_PMF_KEY_VLAN_TAG_CFI_MSB + 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TAG_PRI_CFI, 1, 1, 792+16+32+15-ARAD_PMF_KEY_VLAN_TAG_PRI_LSB, ARAD_PMF_KEY_VLAN_TAG_CFI_LSB - ARAD_PMF_KEY_VLAN_TAG_PRI_MSB + 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_INNER_TPID, 1, 1, 792+16+16, 16},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_INNER_TAG, 1, 1, 792+16, 16},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_INNER_TAG_ID, 1, 1, 792+16+15-ARAD_PMF_KEY_VLAN_TAG_ID_LSB, ARAD_PMF_KEY_VLAN_TAG_ID_LSB - ARAD_PMF_KEY_VLAN_TAG_ID_MSB + 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_INNER_TAG_PRI, 1, 1, 792+16+15-ARAD_PMF_KEY_VLAN_TAG_PRI_LSB, ARAD_PMF_KEY_VLAN_TAG_PRI_LSB - ARAD_PMF_KEY_VLAN_TAG_PRI_MSB + 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_INNER_TAG_CFI, 1, 1, 792+16+15-ARAD_PMF_KEY_VLAN_TAG_CFI_LSB, ARAD_PMF_KEY_VLAN_TAG_CFI_LSB - ARAD_PMF_KEY_VLAN_TAG_CFI_MSB + 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_OUTER_TAG, 1, 1, 248, 16}, 
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_OUTER_TAG_ID, 1, 1, 248+15-ARAD_PMF_KEY_VLAN_TAG_ID_LSB, ARAD_PMF_KEY_VLAN_TAG_ID_LSB - ARAD_PMF_KEY_VLAN_TAG_ID_MSB + 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_OUTER_TAG_PRI, 1, 1, 248+15-ARAD_PMF_KEY_VLAN_TAG_PRI_LSB, ARAD_PMF_KEY_VLAN_TAG_PRI_LSB - ARAD_PMF_KEY_VLAN_TAG_PRI_MSB + 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_OUTER_TAG_CFI, 1, 1, 248+15-ARAD_PMF_KEY_VLAN_TAG_CFI_LSB, ARAD_PMF_KEY_VLAN_TAG_CFI_LSB - ARAD_PMF_KEY_VLAN_TAG_CFI_MSB + 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_INNER_TAG, 1, 1, 248+24, 16}, 
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_INNER_TAG_ID, 1, 1, 248+24+15-ARAD_PMF_KEY_VLAN_TAG_ID_LSB, ARAD_PMF_KEY_VLAN_TAG_ID_LSB - ARAD_PMF_KEY_VLAN_TAG_ID_MSB + 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_INNER_TAG_PRI, 1, 1, 248+24+15-ARAD_PMF_KEY_VLAN_TAG_PRI_LSB, ARAD_PMF_KEY_VLAN_TAG_PRI_LSB - ARAD_PMF_KEY_VLAN_TAG_PRI_MSB + 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_INNER_TAG_CFI, 1, 1, 248+24+15-ARAD_PMF_KEY_VLAN_TAG_CFI_LSB, ARAD_PMF_KEY_VLAN_TAG_CFI_LSB - ARAD_PMF_KEY_VLAN_TAG_CFI_MSB + 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_BRIDGED_FORWARDING_DATA_MSB, 1, 1, 56+872, 64},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_BRIDGED_FORWARDING_DATA_LSB, 1, 1, 872, 56},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_BRIDGED_ACL_DATA_HIGH, 1, 1, 80, 40},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_BRIDGED_ACL_DATA_LOW, 1, 1, 0, 32},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_OPTIONS_PRESENT, 1, 1, 116+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_TOTAL_LEN_ERROR, 1, 1, 115+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_HEADER_LEN_ERROR, 1, 1, 114+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_CHECKSUM_ERROR, 1, 1, 113+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_VERSION_ERROR, 1, 1, 112+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_TOS, 1, 1, 104+872, 8},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_SIP, 1, 1, 72+872, 32},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_DIP, 1, 1, 40+872, 32},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_PROTOCOL, 1, 1, 32+872, 8},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_L4_DEST_PORT, 1, 1, 0+872, 16},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_L4_SRC_PORT, 1, 1, 16+872, 16},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_MSBS_TC, 1, 1, 116+872-ARAD_PMF_QUAL_HDR_IPV6_TC_LSB, 8},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_MSBS_HOP_LIMIT, 1, 1, 116+872-ARAD_PMF_QUAL_HDR_IPV6_HOP_LIMIT_LSB, 8},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_MSBS_FLOW_LABEL, 1, 1, 116+872-ARAD_PMF_QUAL_HDR_IPV6_FLOW_LABEL_LSB, 20},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_MSBS_NEXT_PROTOCOL, 1, 1, 116+872-ARAD_PMF_QUAL_HDR_IPV6_NEXT_PRTCL_LSB, 8},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_MC_DST, 1, 1, 12+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_IPV4_MAPPED_DST, 1, 1, 11+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_IPV4_CMP_DST, 1, 1, 10+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_SITE_LOCAL_SRC, 1, 1, 9+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_LINK_LOCAL_SRC, 1, 1, 8+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_SITE_LOCAL_DST, 1, 1, 7+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_LINK_LOCAL_DST, 1, 1, 6+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_NEXT_HEADER_IS_ZERO, 1, 1, 5+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_LOOPBACK_SRC_OR_DST, 1, 1, 4+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_SIP_IS_ALL_ZEROES, 1, 1, 3+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_DIP_IS_ALL_ZEROES, 1, 1, 2+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_SIP_IS_MC, 1, 1, 1+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_VERSION_ERROR, 1, 1, 0+872, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_MPLS_EXTRA_DATA, 1, 1, 32+872, 85},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_MPLS_HDR, 1, 1, 0+872, 32},
           {SOC_PPC_FP_QUAL_HDR_MPLS_LABEL_FWD, 1, 1, 0+872+31-ARAD_PMF_KEY_MPLS_LABEL_LSB, 32},
           {SOC_PPC_FP_QUAL_HDR_MPLS_LABEL_ID_FWD, 1, 1, 0+872+31-ARAD_PMF_KEY_MPLS_LABEL_ID_LSB, 20},
           {SOC_PPC_FP_QUAL_HDR_MPLS_TTL_FWD, 1, 1, 0+872+31-ARAD_PMF_KEY_MPLS_TTL_LSB, 8},
           {SOC_PPC_FP_QUAL_HDR_MPLS_BOS_FWD, 1, 1, 0+872+31-ARAD_PMF_KEY_MPLS_BOS_LSB, 1},
           {SOC_PPC_FP_QUAL_HDR_MPLS_EXP_FWD, 1, 1, 0+872+31-ARAD_PMF_KEY_MPLS_EXP_LSB, 3},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_TRILL_EXTRA_DATA, 1, 1, 48+872, 69},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_TRILL_HDR, 1, 1, 0+872, 48},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_TRILL_VERSION, 1, 1, 0+872, 2},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_TRILL_MULTI_DESTINATION, 1, 1, 0+872+4, 1},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_TRILL_OP_LENGTH, 1, 1, 0+872+5, 5},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_TRILL_HOP_COUNT, 1, 1, 0+872+10, 6},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_TRILL_EGRESS_RBRIDGE, 1, 1, 0+872+16, 16},
           {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_TRILL_INGRESS_RBRIDGE, 1, 1, 0+872+32, 16},
           {SOC_PPC_FP_QUAL_ERPP_OAM_TS,1,1,160,48},

        };


CONST STATIC 
    ARAD_PMF_CE_IRPP_QUALIFIER_SIGNAL
        Arad_pmf_ce_egress_pmf_signal[] = {
               {SOC_PPC_FP_QUAL_IRPP_INVALID, 0, 0, 0, 0},
               {SOC_PPC_FP_QUAL_ERPP_OUT_PP_PORT_PMF_DATA_PS, 0, 0, 0, 0}, 
               {SOC_PPC_FP_QUAL_ERPP_ONES, 0, 0, 0, 0},
               {SOC_PPC_FP_QUAL_ERPP_ZEROES, 0, 0, 0, 0},
               {SOC_PPC_FP_QUAL_ERPP_OAM_TS, 208, 161, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_LEARN_EXT_SRC_PORT, 16, 1, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_LEARN_EXT_IN_VPORT, 40, 17, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_FHEI, 104, 65, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_FHEI_IPV4_TTL, 65+8-1, 65, 4, 0}, 
               {SOC_PPC_FP_QUAL_ERPP_FHEI_DSCP, 65+8-1+8, 65+8, 4, 0}, 
               {SOC_PPC_FP_QUAL_ERPP_FHEI_EXP, 65+3-1+8, 65+8, 4, 0}, 
               {SOC_PPC_FP_QUAL_ERPP_OUT_TM_PORT_PMF_DATA, 40, 33, 1, 0}, 
               {SOC_PPC_FP_QUAL_ERPP_OUT_PP_PORT_PMF_DATA, 48, 41, 1, 0}, 
               {SOC_PPC_FP_QUAL_ERPP_EEI, 64, 41, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_EXT_IN_LIF, 122, 105, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_EXT_OUT_LIF, 32, 17, 1, 0},
               {SOC_PPC_FP_QUAL_ERPP_STACKING_ROUTE_HISTORY_BITMAP, 223, 208, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_DSP_EXT, 240, 225, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_PACKET_SIZE, 64, 51, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_DST_SYSTEM_PORT, 114, 99, 1, 0},
               {SOC_PPC_FP_QUAL_ERPP_SRC_SYSTEM_PORT, 47, 32, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_VSI_OR_VRF, 15, 0, 1, 0},
               {SOC_PPC_FP_QUAL_ERPP_VSI_OR_VRF_ORIG, 138, 123, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_FWD_OFFSET, 152, 146, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_ETH_TAG_FORMAT, 98, 94, 1, 0},
               {SOC_PPC_FP_QUAL_ERPP_SYS_VALUE1, 75, 68, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_SYS_VALUE2, 83, 76, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_DSP_PTR_ORIG, 31, 24, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_DSP_PTR, 122, 115, 1, 0},
               {SOC_PPC_FP_QUAL_ERPP_OUT_TM_PORT, 40, 33, 1, 0},
               {SOC_PPC_FP_QUAL_ERPP_OUT_PP_PORT, 48, 41, 1, 0},
               {SOC_PPC_FP_QUAL_ERPP_LB_KEY, 248, 241, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_TC, 50, 48, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_FORMAT_CODE, 67, 65, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_FWD_CODE, 88, 85, 1, 0},
               {SOC_PPC_FP_QUAL_ERPP_FWD_CODE_ORIG, 156, 153, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_ACTION_PROFILE, 126, 123, 1, 0},
               {SOC_PPC_FP_QUAL_ERPP_HEADER_CODE, 84, 81, 1, 0},
               {SOC_PPC_FP_QUAL_ERPP_ETH_TYPE_CODE, 93, 90, 1, 0},
               {SOC_PPC_FP_QUAL_ERPP_IN_LIF_ORIENTATION, 142, 141, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_SNOOP_CPU_CODE, 144, 143, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_FTMH_RESERVED, 249, 249, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_ECN_CAPABLE, 250, 250, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_PPH_TYPE, 19, 18, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_TM_ACTION_TYPE, 21, 20, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_DP, 23, 22, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_FHEI_CODE, 158, 157, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_LEARN_ALLOWED, 139, 139, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_UNKNOWN_ADDR, 140, 140, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_LEARN_EXT_VALID, 159, 159, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_BYPASS_FILTERING, 145, 145, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_EEI_VALID, 160, 160, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_CNI, 251, 251, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_DSP_EXT_VALID, 252, 252, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_SYSTEM_MC, 16, 16, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_OUT_MIRROR_DISABLE, 17, 17, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_EXCLUDE_SRC, 84, 84, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_DISCARD, 65, 65, 1, 0},
               {SOC_PPC_FP_QUAL_ERPP_FABRIC_OR_EGRESS_MC, 89, 89, 1, 0},
               {SOC_PPC_FP_QUAL_ERPP_RESEVED, 0, 0, 0},
               {SOC_PPC_FP_QUAL_ERPP_FIRST_COPY, 63, 63, 1, 0},
               {SOC_PPC_FP_QUAL_ERPP_LAST_COPY, 64, 64, 1, 0},
               {SOC_PPC_FP_QUAL_ERPP_START_BUFFER, 62, 49, 1, 0},
               {SOC_PPC_FP_QUAL_ERPP_CONTEXT, 79, 66, 1, 0},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_DA, 137, 90, 4, 2},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_SA, 89, 42, 4, 2},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_DATA, 255, 218, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_ADDITIONAL_DATA, 217, 215, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_CPID0, 214, 211, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_CPID1, 210, 207, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_CPID2, 206, 203, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TPID, 41, 26, 4, 2},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TAG, 25, 10, 4, 2},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TAG_ID, 25-ARAD_PMF_KEY_VLAN_TAG_ID_MSB, 25-ARAD_PMF_KEY_VLAN_TAG_ID_LSB, 4, 2},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TAG_PRI, 25-ARAD_PMF_KEY_VLAN_TAG_PRI_MSB, 25-ARAD_PMF_KEY_VLAN_TAG_PRI_LSB, 4, 2},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TAG_CFI, 25-ARAD_PMF_KEY_VLAN_TAG_CFI_MSB, 25-ARAD_PMF_KEY_VLAN_TAG_CFI_LSB, 4, 2},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TAG_PRI_CFI, 25-ARAD_PMF_KEY_VLAN_TAG_PRI_MSB, 25-ARAD_PMF_KEY_VLAN_TAG_CFI_LSB, 4, 2},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_INNER_TPID, 0, 0, 0, 0},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_INNER_TAG, 249, 234, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_INNER_TAG_ID, 249-ARAD_PMF_KEY_VLAN_TAG_ID_MSB, 249-ARAD_PMF_KEY_VLAN_TAG_ID_LSB, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_INNER_TAG_PRI, 249-ARAD_PMF_KEY_VLAN_TAG_PRI_MSB, 249-ARAD_PMF_KEY_VLAN_TAG_PRI_LSB, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_INNER_TAG_CFI, 249-ARAD_PMF_KEY_VLAN_TAG_CFI_MSB, 249-ARAD_PMF_KEY_VLAN_TAG_CFI_LSB, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_OUTER_TAG, 65+15, 65+0, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_OUTER_TAG_ID, 65+15-ARAD_PMF_KEY_VLAN_TAG_ID_MSB, 65+15-ARAD_PMF_KEY_VLAN_TAG_ID_LSB, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_OUTER_TAG_PRI, 65+15-ARAD_PMF_KEY_VLAN_TAG_PRI_MSB, 65+15-ARAD_PMF_KEY_VLAN_TAG_PRI_LSB, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_OUTER_TAG_CFI, 65+15-ARAD_PMF_KEY_VLAN_TAG_CFI_MSB, 65+15-ARAD_PMF_KEY_VLAN_TAG_CFI_LSB, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_INNER_TAG, 65+39, 65+24, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_INNER_TAG_ID, 65+39-ARAD_PMF_KEY_VLAN_TAG_ID_MSB, 65+39-ARAD_PMF_KEY_VLAN_TAG_ID_LSB, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_INNER_TAG_PRI, 65+39-ARAD_PMF_KEY_VLAN_TAG_PRI_MSB, 65+39-ARAD_PMF_KEY_VLAN_TAG_PRI_LSB, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_INNER_TAG_CFI, 65+39-ARAD_PMF_KEY_VLAN_TAG_CFI_MSB, 65+39-ARAD_PMF_KEY_VLAN_TAG_CFI_LSB, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_BRIDGED_FORWARDING_DATA_MSB, 226, 163, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_BRIDGED_FORWARDING_DATA_LSB, 162, 107, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_BRIDGED_ACL_DATA_HIGH, 120, 80, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_BRIDGED_ACL_DATA_LOW, 32, 0, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_OPTIONS_PRESENT, 202, 202, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_TOTAL_LEN_ERROR, 201, 201, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_HEADER_LEN_ERROR, 200, 200, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_CHECKSUM_ERROR, 199, 199, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_VERSION_ERROR, 198, 198, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_TOS, 197, 190, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_SIP, 189, 158, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_DIP, 157, 126, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_PROTOCOL, 125, 118, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_L4_DEST_PORT, 101, 86, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_L4_SRC_PORT, 117, 102, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_MSBS_TC, 202-ARAD_PMF_QUAL_HDR_IPV6_TC_MSB, 202-ARAD_PMF_QUAL_HDR_IPV6_TC_LSB, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_MSBS_HOP_LIMIT, 202-ARAD_PMF_QUAL_HDR_IPV6_HOP_LIMIT_MSB, 202-ARAD_PMF_QUAL_HDR_IPV6_HOP_LIMIT_LSB, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_MSBS_FLOW_LABEL, 202-ARAD_PMF_QUAL_HDR_IPV6_FLOW_LABEL_MSB, 202-ARAD_PMF_QUAL_HDR_IPV6_FLOW_LABEL_LSB, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_MSBS_NEXT_PROTOCOL, 202-ARAD_PMF_QUAL_HDR_IPV6_NEXT_PRTCL_MSB, 202-ARAD_PMF_QUAL_HDR_IPV6_NEXT_PRTCL_LSB, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_MC_DST, 98, 98, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_IPV4_MAPPED_DST, 97, 97, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_IPV4_CMP_DST, 96, 96, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_SITE_LOCAL_SRC, 95, 95, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_LINK_LOCAL_SRC, 94, 94, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_SITE_LOCAL_DST, 93, 93, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_LINK_LOCAL_DST, 92, 92, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_NEXT_HEADER_IS_ZERO, 91, 91, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_LOOPBACK_SRC_OR_DST, 90, 90, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_SIP_IS_ALL_ZEROES, 89, 89, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_DIP_IS_ALL_ZEROES, 88, 88, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_SIP_IS_MC, 87, 87, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_VERSION_ERROR, 86, 86, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_MPLS_EXTRA_DATA, 202, 118, 4, 1},
               {SOC_PPC_FP_QUAL_HDR_MPLS_LABEL_FWD, 117, 86, 4, 1},
               {SOC_PPC_FP_QUAL_HDR_MPLS_TTL_FWD, 117-ARAD_PMF_KEY_MPLS_TTL_MSB, 117-ARAD_PMF_KEY_MPLS_TTL_LSB, 4, 1},
               {SOC_PPC_FP_QUAL_HDR_MPLS_BOS_FWD, 117-ARAD_PMF_KEY_MPLS_BOS_MSB, 117-ARAD_PMF_KEY_MPLS_BOS_LSB, 4, 1},
               {SOC_PPC_FP_QUAL_HDR_MPLS_LABEL_ID_FWD, 117-ARAD_PMF_KEY_MPLS_LABEL_ID_MSB, 117-ARAD_PMF_KEY_MPLS_LABEL_ID_LSB, 4, 1},
               {SOC_PPC_FP_QUAL_HDR_MPLS_EXP_FWD, 117-ARAD_PMF_KEY_MPLS_EXP_MSB, 117-ARAD_PMF_KEY_MPLS_EXP_LSB, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_TRILL_EXTRA_DATA, 202, 134, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_TRILL_HDR, 133, 86, 4, 1},
        };

CONST STATIC 
    ARAD_PMF_CE_IRPP_QUALIFIER_SIGNAL
        Jericho_pmf_ce_egress_pmf_signal[] = {
               {SOC_PPC_FP_QUAL_IRPP_INVALID, 0, 0, 0, 0},
               {SOC_PPC_FP_QUAL_ERPP_OUT_PP_PORT_PMF_DATA_PS, 0, 0, 0, 0}, 
               {SOC_PPC_FP_QUAL_ERPP_ONES,0,0, 0, 0},
               {SOC_PPC_FP_QUAL_ERPP_ZEROES,0,0, 0, 0},
               {SOC_PPC_FP_QUAL_ERPP_OAM_TS,208,161, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_LEARN_EXT_SRC_PORT, 16, 1, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_LEARN_EXT_IN_VPORT, 40, 17, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_FHEI, 104, 65, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_FHEI_IPV4_TTL, 65+8-1, 65, 4, 0}, 
               {SOC_PPC_FP_QUAL_ERPP_FHEI_DSCP, 65+8-1+8, 65+8, 4, 0}, 
               {SOC_PPC_FP_QUAL_ERPP_FHEI_EXP, 65+3-1+8, 65+8, 4, 0}, 
               {SOC_PPC_FP_QUAL_ERPP_OUT_TM_PORT_PMF_DATA,42,35, 1, 0}, 
               {SOC_PPC_FP_QUAL_ERPP_OUT_PP_PORT_PMF_DATA,50,43, 1, 0}, 
               {SOC_PPC_FP_QUAL_ERPP_EEI,64,41, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_EXT_IN_LIF,122,105, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_EXT_OUT_LIF,34,17, 1, 0},
               {SOC_PPC_FP_QUAL_ERPP_STACKING_ROUTE_HISTORY_BITMAP,224,209, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_DSP_EXT,240,225, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_PACKET_SIZE,64,51, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_DST_SYSTEM_PORT,124,109, 1, 0},
               {SOC_PPC_FP_QUAL_ERPP_SRC_SYSTEM_PORT,47,32, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_VSI_OR_VRF,15,0, 1, 0},
               {SOC_PPC_FP_QUAL_ERPP_VSI_OR_VRF_ORIG,138,123, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_FWD_OFFSET,152,146, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_ETH_TAG_FORMAT,108,104, 1, 0},
               {SOC_PPC_FP_QUAL_ERPP_SYS_VALUE1,75,68, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_SYS_VALUE2,83,76, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_DSP_PTR_ORIG,31,24, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_DSP_PTR,132,125, 1, 0},
               {SOC_PPC_FP_QUAL_ERPP_OUT_TM_PORT,42,35, 1, 0},
               {SOC_PPC_FP_QUAL_ERPP_OUT_PP_PORT,50,43, 1, 0},
               {SOC_PPC_FP_QUAL_ERPP_LB_KEY,248,241, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_TC,50,48, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_FORMAT_CODE,67,65, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_FWD_CODE,98,95, 1, 0},
               {SOC_PPC_FP_QUAL_ERPP_FWD_CODE_ORIG,156,153, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_ACTION_PROFILE,136,133, 1, 0},
               {SOC_PPC_FP_QUAL_ERPP_HEADER_CODE,94,91, 1, 0},
               {SOC_PPC_FP_QUAL_ERPP_ETH_TYPE_CODE,103,100, 1, 0},
               {SOC_PPC_FP_QUAL_ERPP_IN_LIF_ORIENTATION,142,141, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_SNOOP_CPU_CODE,144,143, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_FTMH_RESERVED, 249, 249, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_ECN_CAPABLE, 250, 250, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_PPH_TYPE,19,18, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_TM_ACTION_TYPE,21,20, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_DP,23,22, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_FHEI_CODE,158,157, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_LEARN_ALLOWED,139,139, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_UNKNOWN_ADDR,140,140, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_LEARN_EXT_VALID,159,159, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_BYPASS_FILTERING,145,145, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_EEI_VALID,160,160, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_CNI,251,251, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_DSP_EXT_VALID,252,252, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_SYSTEM_MC,16,16, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_OUT_MIRROR_DISABLE,17,17, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_EXCLUDE_SRC,84,84, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_DISCARD,57,57, 3, 0},
               {SOC_PPC_FP_QUAL_ERPP_FABRIC_OR_EGRESS_MC,99,99, 1, 0},
               {SOC_PPC_FP_QUAL_ERPP_RESEVED,0,0, 0, 0},
               {SOC_PPC_FP_QUAL_ERPP_FIRST_COPY,53,53, 3, 0},
               {SOC_PPC_FP_QUAL_ERPP_LAST_COPY,52,52, 3, 0},
               {SOC_PPC_FP_QUAL_ERPP_START_BUFFER,19,6, 3, 0},
               {SOC_PPC_FP_QUAL_ERPP_CONTEXT,167,148, 3, 0},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_DA,161,114, 4, 2},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_SA,113,66, 4, 2},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_DATA, 255, 242, 4, 1}, 
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_ADDITIONAL_DATA,241,239, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_CPID0,238,235, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_CPID1,234,231, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_CPID2,230,227, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TPID, 65, 50, 4, 2},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TAG, 49, 34, 4, 2},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TAG_ID, 49-ARAD_PMF_KEY_VLAN_TAG_ID_MSB, 49-ARAD_PMF_KEY_VLAN_TAG_ID_LSB, 4, 2},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TAG_PRI, 49-ARAD_PMF_KEY_VLAN_TAG_PRI_MSB, 49-ARAD_PMF_KEY_VLAN_TAG_PRI_LSB, 4, 2},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TAG_CFI, 49-ARAD_PMF_KEY_VLAN_TAG_CFI_MSB, 49-ARAD_PMF_KEY_VLAN_TAG_CFI_LSB, 4, 2},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_OUTER_TAG_PRI_CFI, 49-ARAD_PMF_KEY_VLAN_TAG_PRI_MSB, 49-ARAD_PMF_KEY_VLAN_TAG_CFI_LSB, 4, 2},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_INNER_TPID, 33, 18, 4, 2},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_INNER_TAG, 17, 2, 4, 2},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_INNER_TAG_ID, 17-ARAD_PMF_KEY_VLAN_TAG_ID_MSB, 17-ARAD_PMF_KEY_VLAN_TAG_ID_LSB, 4, 2},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_INNER_TAG_PRI, 17-ARAD_PMF_KEY_VLAN_TAG_PRI_MSB, 17-ARAD_PMF_KEY_VLAN_TAG_PRI_LSB, 4, 2},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_ETH_INNER_TAG_CFI, 17-ARAD_PMF_KEY_VLAN_TAG_CFI_MSB, 17-ARAD_PMF_KEY_VLAN_TAG_CFI_LSB, 4, 2},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_OUTER_TAG, 65+15, 65+0, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_OUTER_TAG_ID, 65+15-ARAD_PMF_KEY_VLAN_TAG_ID_MSB, 65+15-ARAD_PMF_KEY_VLAN_TAG_ID_LSB, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_OUTER_TAG_PRI, 65+15-ARAD_PMF_KEY_VLAN_TAG_PRI_MSB, 65+15-ARAD_PMF_KEY_VLAN_TAG_PRI_LSB, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_OUTER_TAG_CFI, 65+15-ARAD_PMF_KEY_VLAN_TAG_CFI_MSB, 65+15-ARAD_PMF_KEY_VLAN_TAG_CFI_LSB, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_INNER_TAG, 65+39, 65+24, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_INNER_TAG_ID, 65+39-ARAD_PMF_KEY_VLAN_TAG_ID_MSB, 65+39-ARAD_PMF_KEY_VLAN_TAG_ID_LSB, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_INNER_TAG_PRI, 65+39-ARAD_PMF_KEY_VLAN_TAG_PRI_MSB, 65+39-ARAD_PMF_KEY_VLAN_TAG_PRI_LSB, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_FHEI_ETH_INNER_TAG_CFI, 65+39-ARAD_PMF_KEY_VLAN_TAG_CFI_MSB, 65+39-ARAD_PMF_KEY_VLAN_TAG_CFI_LSB, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_BRIDGED_FORWARDING_DATA_MSB, 226, 163, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_BRIDGED_FORWARDING_DATA_LSB, 162, 107, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_BRIDGED_ACL_DATA_HIGH, 120, 80, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_BRIDGED_ACL_DATA_LOW, 32, 0, 4, 0},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_OPTIONS_PRESENT, 226, 226, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_TOTAL_LEN_ERROR, 225, 225, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_HEADER_LEN_ERROR, 224, 224, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_CHECKSUM_ERROR, 223, 223, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_VERSION_ERROR, 222, 222, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_TOS, 221, 214, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_SIP, 213, 182, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_DIP, 181, 150, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_PROTOCOL, 149, 142, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_L4_DEST_PORT, 125, 110, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV4_L4_SRC_PORT, 141, 126, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_MSBS_TC, 226-ARAD_PMF_QUAL_HDR_IPV6_TC_MSB, 226-ARAD_PMF_QUAL_HDR_IPV6_TC_LSB, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_MSBS_HOP_LIMIT, 226-ARAD_PMF_QUAL_HDR_IPV6_HOP_LIMIT_MSB, 226-ARAD_PMF_QUAL_HDR_IPV6_HOP_LIMIT_LSB, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_MSBS_FLOW_LABEL, 226-ARAD_PMF_QUAL_HDR_IPV6_FLOW_LABEL_MSB, 226-ARAD_PMF_QUAL_HDR_IPV6_FLOW_LABEL_LSB, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_MSBS_NEXT_PROTOCOL, 226-ARAD_PMF_QUAL_HDR_IPV6_NEXT_PRTCL_MSB, 226-ARAD_PMF_QUAL_HDR_IPV6_NEXT_PRTCL_LSB, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_MC_DST, 122, 122, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_IPV4_MAPPED_DST, 121, 121, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_IPV4_CMP_DST, 120, 120, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_SITE_LOCAL_SRC, 119, 119, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_LINK_LOCAL_SRC, 118, 118, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_SITE_LOCAL_DST, 117, 117, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_LINK_LOCAL_DST, 116, 116, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_NEXT_HEADER_IS_ZERO, 115, 115, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_LOOPBACK_SRC_OR_DST, 114, 114, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_SIP_IS_ALL_ZEROES, 113, 113, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_DIP_IS_ALL_ZEROES, 112, 112, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_SIP_IS_MC, 111, 111, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_IPV6_VERSION_ERROR, 110, 110, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_MPLS_EXTRA_DATA, 226, 142, 4, 1},
               {SOC_PPC_FP_QUAL_HDR_MPLS_LABEL_FWD, 141, 110, 4, 1},
               {SOC_PPC_FP_QUAL_HDR_MPLS_TTL_FWD, 141-ARAD_PMF_KEY_MPLS_TTL_MSB, 141-ARAD_PMF_KEY_MPLS_TTL_LSB, 4, 1},
               {SOC_PPC_FP_QUAL_HDR_MPLS_BOS_FWD, 141-ARAD_PMF_KEY_MPLS_BOS_MSB, 141-ARAD_PMF_KEY_MPLS_BOS_LSB, 4, 1},
               {SOC_PPC_FP_QUAL_HDR_MPLS_LABEL_ID_FWD, 141-ARAD_PMF_KEY_MPLS_LABEL_ID_MSB, 141-ARAD_PMF_KEY_MPLS_LABEL_ID_LSB, 4, 1},
               {SOC_PPC_FP_QUAL_HDR_MPLS_EXP_FWD, 141-ARAD_PMF_KEY_MPLS_EXP_MSB, 141-ARAD_PMF_KEY_MPLS_EXP_LSB, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_TRILL_EXTRA_DATA, 226, 158, 4, 1},
               {SOC_PPC_FP_QUAL_ERPP_NWK_RCRD_TRILL_HDR, 157, 110, 4, 1},
        };

CONST STATIC
    ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES
    Arad_pmf_ce_vt_info[] = {
               {SOC_PPC_FP_QUAL_CMPRSD_INNER_VID, 1, 1, 0, 12 },
               {SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID, 1, 1, 16, 12 },
               {SOC_PPC_FP_QUAL_INITIAL_VID, 1, 1, 32, 12 },
               {SOC_PPC_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR, 1, 1, 48, 16 },
               {SOC_PPC_FP_QUAL_MPLS_LABEL_RANGE_BASE_LIF2, 1, 1, 64, 16 },
               {SOC_PPC_FP_QUAL_MPLS_LABEL_RANGE_BASE_LIF1, 1, 1, 80, 16 },
               {SOC_PPC_FP_QUAL_MPLS_LABEL_RANGE_BASE_LIF0, 1, 1, 96, 16 },
               {SOC_PPC_FP_QUAL_LABEL3_IDX, 1, 1, 112, 16 },
               {SOC_PPC_FP_QUAL_LABEL2_IDX, 1, 1, 128, 16 },
               {SOC_PPC_FP_QUAL_LABEL1_IDX, 1, 1, 144, 16 },
               {SOC_PPC_FP_QUAL_VLAN_DOMAIN, 1, 1, 160, 8 },
               {SOC_PPC_FP_QUAL_IRPP_SRC_PP_PORT, 1, 1, 168, 8 },
               {SOC_PPC_FP_QUAL_IRPP_SRC_TM_PORT, 1, 1, 176, 8 },
               {SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND_LEM_DOMAIN, 1, 1, 184, 6 },
                  {SOC_PPC_FP_QUAL_MACT_DOMAIN, 1, 1, 184, 5 },
                  {SOC_PPC_FP_QUAL_VT_LEM_1ST_LOOKUP_NOT_FOUND, 1, 1, 189, 1 },
               {SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND_LEM_SA_DROP, 1, 1, 192, 2 },
                  {SOC_PPC_FP_QUAL_MACT_SA_DROP, 1, 1, 192, 1 },
                  {SOC_PPC_FP_QUAL_VT_LEM_1ST_LOOKUP_NOT_FOUND, 1, 1, 193, 1 },
               {SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND_LEM_IS_LEARN_LIF, 1, 1, 200, 2 },
                  {SOC_PPC_FP_QUAL_MACT_IS_LEARN, 1, 1, 200, 1 },
                  {SOC_PPC_FP_QUAL_VT_LEM_1ST_LOOKUP_NOT_FOUND, 1, 1, 201, 1 },
               {SOC_PPC_FP_QUAL_LEM_DYNAMIC_LEM_1ST_LOOKUP_FOUND_LEM, 1, 1, 208, 2 },
                  {SOC_PPC_FP_QUAL_VT_LEM_1ST_LOOKUP_FOUND, 1, 1, 208, 1 },
                  {SOC_PPC_FP_QUAL_MACT_DYNAMIC, 1, 1, 209, 1 },
               {SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND_DESTINATION, 1, 1, 216, 20 },
                  {SOC_PPC_FP_QUAL_MACT_DESTINATION, 1, 1, 216, 19 },
                  {SOC_PPC_FP_QUAL_VT_LEM_1ST_LOOKUP_NOT_FOUND, 1, 1, 235, 1 },
               {SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES, 1, 1, 240, 32 },
               {SOC_PPC_FP_QUAL_IRPP_ALL_ONES, 1, 1, 272, 32 },
               {SOC_PPC_FP_QUAL_IRPP_PROG_VAR, 1, 1, 304, 16 }
};

CONST STATIC
    ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES
    Jericho_pmf_ce_vt_info[] = {
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER0, 1, 1, 0, 11 },
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1, 1, 1, 16, 11 },
                  {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_INNER_TAG, 1, 1, 16, 2 },
                  {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_PRIORITY, 1, 1, 18, 1 },
                  {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_OUTER_TAG, 1, 1, 19, 2 },
                  {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_ENCAPSULATION, 1, 1, 21, 2 },
                  {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_NEXT_PROTOCOL, 1, 1, 23, 4 },
               {SOC_PPC_FP_QUAL_MPLS_KEY3, 1, 1, 32, 46 },
                  {SOC_PPC_FP_QUAL_KEY3_16_INST0, 1, 1, 32, 16 },
                  {SOC_PPC_FP_QUAL_KEY3_16_INST1, 1, 1, 48, 16 },
                  {SOC_PPC_FP_QUAL_KEY3_16_INST2, 1, 1, 64, 14 },
                  {SOC_PPC_FP_QUAL_KEY3_32_INST0, 1, 1, 32, 32 },
                  {SOC_PPC_FP_QUAL_KEY3_32_INST1, 1, 1, 64, 14 },
                  {SOC_PPC_FP_QUAL_KEY3_LABEL, 1, 1, 32, 20 },
                  {SOC_PPC_FP_QUAL_KEY3_OUTER_VID, 1, 1, 52, 12 },
                  {SOC_PPC_FP_QUAL_KEY3_OUTER_VID_VALID, 1, 1, 64, 1 },
                  {SOC_PPC_FP_QUAL_KEY3_EVPN_BOS_EXPECTED, 1, 1, 64, 1 },
                  {SOC_PPC_FP_QUAL_KEY3_INNER_VID, 1, 1, 65, 12 },
                  {SOC_PPC_FP_QUAL_KEY3_INNER_VID_VALID, 1, 1, 77, 1 },
               {SOC_PPC_FP_QUAL_MPLS_KEY2, 1, 1, 80, 46 },
                  {SOC_PPC_FP_QUAL_KEY2_16_INST0, 1, 1, 80, 16 },
                  {SOC_PPC_FP_QUAL_KEY2_16_INST1, 1, 1, 96, 16 },
                  {SOC_PPC_FP_QUAL_KEY2_16_INST2, 1, 1, 112, 14 },
                  {SOC_PPC_FP_QUAL_KEY2_32_INST0, 1, 1, 80, 32 },
                  {SOC_PPC_FP_QUAL_KEY2_32_INST1, 1, 1, 112, 14 },
                  {SOC_PPC_FP_QUAL_KEY2_LABEL, 1, 1, 80, 20 },
                  {SOC_PPC_FP_QUAL_KEY2_OUTER_VID, 1, 1, 100, 12 },
                  {SOC_PPC_FP_QUAL_KEY2_OUTER_VID_VALID, 1, 1, 112, 1 },
                  {SOC_PPC_FP_QUAL_KEY2_EVPN_BOS_EXPECTED, 1, 1, 112, 1 },
                  {SOC_PPC_FP_QUAL_KEY2_INNER_VID, 1, 1, 113, 12 },
                  {SOC_PPC_FP_QUAL_KEY2_INNER_VID_VALID, 1, 1, 125, 1 },
               {SOC_PPC_FP_QUAL_MPLS_KEY1, 1, 1, 128, 46 },
                  {SOC_PPC_FP_QUAL_KEY1_16_INST0, 1, 1, 128, 16 },
                  {SOC_PPC_FP_QUAL_KEY1_16_INST1, 1, 1, 144, 16 },
                  {SOC_PPC_FP_QUAL_KEY1_16_INST2, 1, 1, 150, 14 },
                  {SOC_PPC_FP_QUAL_KEY1_32_INST0, 1, 1, 128, 32 },
                  {SOC_PPC_FP_QUAL_KEY1_32_INST1, 1, 1, 150, 14 },
                  {SOC_PPC_FP_QUAL_KEY1_LABEL, 1, 1, 128, 20 },
                  {SOC_PPC_FP_QUAL_KEY1_OUTER_VID, 1, 1, 148, 12 },
                  {SOC_PPC_FP_QUAL_KEY1_OUTER_VID_VALID, 1, 1, 160, 1 },
                  {SOC_PPC_FP_QUAL_KEY1_EVPN_BOS_EXPECTED, 1, 1, 160, 1 },
                  {SOC_PPC_FP_QUAL_KEY1_INNER_VID, 1, 1, 161, 12 },
                  {SOC_PPC_FP_QUAL_KEY1_INNER_VID_VALID, 1, 1, 173, 1 },
               {SOC_PPC_FP_QUAL_MPLS_KEY0, 1, 1, 176, 46 },
                  {SOC_PPC_FP_QUAL_KEY0_16_INST0, 1, 1, 176, 16 },
                  {SOC_PPC_FP_QUAL_KEY0_16_INST1, 1, 1, 192, 16 },
                  {SOC_PPC_FP_QUAL_KEY0_16_INST2, 1, 1, 208, 14 },
                  {SOC_PPC_FP_QUAL_KEY0_32_INST0, 1, 1, 176, 32 },
                  {SOC_PPC_FP_QUAL_KEY0_32_INST1, 1, 1, 208, 14 },
                  {SOC_PPC_FP_QUAL_KEY0_LABEL, 1, 1, 176, 20 },
                  {SOC_PPC_FP_QUAL_KEY0_OUTER_VID, 1, 1, 196, 12 },
                  {SOC_PPC_FP_QUAL_KEY0_OUTER_VID_VALID, 1, 1, 208, 1 },
                  {SOC_PPC_FP_QUAL_KEY0_EVPN_BOS_EXPECTED, 1, 1, 208, 1 },
                  {SOC_PPC_FP_QUAL_KEY0_INNER_VID, 1, 1, 209, 12 },
                  {SOC_PPC_FP_QUAL_KEY0_INNER_VID_VALID, 1, 1, 221, 1 },
               {SOC_PPC_FP_QUAL_IRPP_SRC_SYST_PORT, 1, 1, 224, 16 },
               {SOC_PPC_FP_QUAL_CMPRSD_INNER_VID, 1, 1, 240, 12 },
               {SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID, 1, 1, 256, 12 },
               {SOC_PPC_FP_QUAL_INITIAL_VID, 1, 1, 272, 12 },
               {SOC_PPC_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR, 1, 1, 288, 16 },
               {SOC_PPC_FP_QUAL_MPLS_LABEL_RANGE_BASE_LIF2, 1, 1, 304, 16 },
               {SOC_PPC_FP_QUAL_MPLS_LABEL_RANGE_BASE_LIF1, 1, 1, 320, 16 },
               {SOC_PPC_FP_QUAL_MPLS_LABEL_RANGE_BASE_LIF0, 1, 1, 336, 16 },
               {SOC_PPC_FP_QUAL_LABEL3_IDX, 1, 1, 352, 16 },
               {SOC_PPC_FP_QUAL_LABEL2_IDX, 1, 1, 368, 16 },
               {SOC_PPC_FP_QUAL_LABEL1_IDX, 1, 1, 384, 16 },
               {SOC_PPC_FP_QUAL_IRPP_SRC_PP_PORT, 1, 1, 400, 9 }, 
               {SOC_PPC_FP_QUAL_VLAN_DOMAIN, 1, 1, 416, 9 },
               
               {SOC_PPC_FP_QUAL_IRPP_SRC_TM_PORT, 1, 1, 440, 8 },
               {SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND_LEM_DOMAIN, 1, 1, 448, 6 },
                  {SOC_PPC_FP_QUAL_MACT_DOMAIN, 1, 1, 448, 5 },
                  {SOC_PPC_FP_QUAL_VT_LEM_1ST_LOOKUP_NOT_FOUND, 1, 1, 453, 1 },
               {SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND_LEM_SA_DROP, 1, 1, 456, 2 },
                  {SOC_PPC_FP_QUAL_MACT_SA_DROP, 1, 1, 456, 1 },
                  {SOC_PPC_FP_QUAL_VT_LEM_1ST_LOOKUP_NOT_FOUND, 1, 1, 457, 1 },
               {SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND_LEM_IS_LEARN_LIF, 1, 1, 464, 2 },
                  {SOC_PPC_FP_QUAL_MACT_IS_LEARN, 1, 1, 464, 1 },
                  {SOC_PPC_FP_QUAL_VT_LEM_1ST_LOOKUP_NOT_FOUND, 1, 1, 465, 1 },
               {SOC_PPC_FP_QUAL_LEM_DYNAMIC_LEM_1ST_LOOKUP_FOUND_LEM, 1, 1, 472, 2 },
                  {SOC_PPC_FP_QUAL_VT_LEM_1ST_LOOKUP_FOUND, 1, 1, 472, 1 },
                  {SOC_PPC_FP_QUAL_MACT_DYNAMIC, 1, 1, 473, 1 },
               {SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND_DESTINATION, 1, 1, 480, 20 },
                  {SOC_PPC_FP_QUAL_MACT_DESTINATION, 1, 1, 480, 19 },
                  {SOC_PPC_FP_QUAL_VT_LEM_1ST_LOOKUP_NOT_FOUND, 1, 1, 499, 1 },
               {SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES, 1, 1, 504, 32 },
               {SOC_PPC_FP_QUAL_IRPP_ALL_ONES, 1, 1, 536, 32 },
               {SOC_PPC_FP_QUAL_IRPP_PROG_VAR, 1, 1, 568, 16 }
        };

CONST STATIC
    ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES
    Qax_pmf_ce_vt_info[] = {
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER0, 1, 1, 0, 11 },
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1, 1, 1, 16, 11 },
                  {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_INNER_TAG, 1, 1, 16, 2 },
                  {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_PRIORITY, 1, 1, 18, 1 },
                  {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_OUTER_TAG, 1, 1, 19, 2 },
                  {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_ENCAPSULATION, 1, 1, 21, 2 },
                  {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1_NEXT_PROTOCOL, 1, 1, 23, 4 },
               {SOC_PPC_FP_QUAL_MPLS_KEY3, 1, 1, 32, 47 },
                  {SOC_PPC_FP_QUAL_KEY3_16_INST0, 1, 1, 32, 16 },
                  {SOC_PPC_FP_QUAL_KEY3_16_INST1, 1, 1, 48, 16 },
                  {SOC_PPC_FP_QUAL_KEY3_16_INST2, 1, 1, 64, 14 },
                  {SOC_PPC_FP_QUAL_KEY3_32_INST0, 1, 1, 32, 32 },
                  {SOC_PPC_FP_QUAL_KEY3_32_INST1, 1, 1, 64, 14 },
                  {SOC_PPC_FP_QUAL_KEY3_LABEL, 1, 1, 32, 20 },
                  {SOC_PPC_FP_QUAL_KEY3_OUTER_VID, 1, 1, 52, 12 },
                  {SOC_PPC_FP_QUAL_KEY3_OUTER_VID_VALID, 1, 1, 64, 1 },
                  {SOC_PPC_FP_QUAL_KEY3_EVPN_BOS_EXPECTED, 1, 1, 64, 1 },
                  {SOC_PPC_FP_QUAL_KEY3_INNER_VID, 1, 1, 65, 12 },
                  {SOC_PPC_FP_QUAL_KEY3_INNER_VID_VALID, 1, 1, 77, 1 },
               {SOC_PPC_FP_QUAL_MPLS_KEY2, 1, 1, 80, 47 },
                  {SOC_PPC_FP_QUAL_KEY2_16_INST0, 1, 1, 80, 16 },
                  {SOC_PPC_FP_QUAL_KEY2_16_INST1, 1, 1, 96, 16 },
                  {SOC_PPC_FP_QUAL_KEY2_16_INST2, 1, 1, 112, 14 },
                  {SOC_PPC_FP_QUAL_KEY2_32_INST0, 1, 1, 80, 32 },
                  {SOC_PPC_FP_QUAL_KEY2_32_INST1, 1, 1, 112, 14 },
                  {SOC_PPC_FP_QUAL_KEY2_LABEL, 1, 1, 80, 20 },
                  {SOC_PPC_FP_QUAL_KEY2_OUTER_VID, 1, 1, 100, 12 },
                  {SOC_PPC_FP_QUAL_KEY2_OUTER_VID_VALID, 1, 1, 112, 1 },
                  {SOC_PPC_FP_QUAL_KEY2_EVPN_BOS_EXPECTED, 1, 1, 112, 1 },
                  {SOC_PPC_FP_QUAL_KEY2_INNER_VID, 1, 1, 113, 12 },
                  {SOC_PPC_FP_QUAL_KEY2_INNER_VID_VALID, 1, 1, 125, 1 },
               {SOC_PPC_FP_QUAL_MPLS_KEY1, 1, 1, 128, 47 },
                  {SOC_PPC_FP_QUAL_KEY1_16_INST0, 1, 1, 128, 16 },
                  {SOC_PPC_FP_QUAL_KEY1_16_INST1, 1, 1, 144, 16 },
                  {SOC_PPC_FP_QUAL_KEY1_16_INST2, 1, 1, 150, 14 },
                  {SOC_PPC_FP_QUAL_KEY1_32_INST0, 1, 1, 128, 32 },
                  {SOC_PPC_FP_QUAL_KEY1_32_INST1, 1, 1, 150, 14 },
                  {SOC_PPC_FP_QUAL_KEY1_LABEL, 1, 1, 128, 20 },
                  {SOC_PPC_FP_QUAL_KEY1_OUTER_VID, 1, 1, 148, 12 },
                  {SOC_PPC_FP_QUAL_KEY1_OUTER_VID_VALID, 1, 1, 160, 1 },
                  {SOC_PPC_FP_QUAL_KEY1_EVPN_BOS_EXPECTED, 1, 1, 160, 1 },
                  {SOC_PPC_FP_QUAL_KEY1_INNER_VID, 1, 1, 161, 12 },
                  {SOC_PPC_FP_QUAL_KEY1_INNER_VID_VALID, 1, 1, 173, 1 },
               {SOC_PPC_FP_QUAL_MPLS_KEY0, 1, 1, 176, 47 },
                  {SOC_PPC_FP_QUAL_KEY0_16_INST0, 1, 1, 176, 16 },
                  {SOC_PPC_FP_QUAL_KEY0_16_INST1, 1, 1, 192, 16 },
                  {SOC_PPC_FP_QUAL_KEY0_16_INST2, 1, 1, 208, 14 },
                  {SOC_PPC_FP_QUAL_KEY0_32_INST0, 1, 1, 176, 32 },
                  {SOC_PPC_FP_QUAL_KEY0_32_INST1, 1, 1, 208, 14 },
                  {SOC_PPC_FP_QUAL_KEY0_LABEL, 1, 1, 176, 20 },
                  {SOC_PPC_FP_QUAL_KEY0_OUTER_VID, 1, 1, 196, 12 },
                  {SOC_PPC_FP_QUAL_KEY0_OUTER_VID_VALID, 1, 1, 208, 1 },
                  {SOC_PPC_FP_QUAL_KEY0_EVPN_BOS_EXPECTED, 1, 1, 208, 1 },
                  {SOC_PPC_FP_QUAL_KEY0_INNER_VID, 1, 1, 209, 12 },
                  {SOC_PPC_FP_QUAL_KEY0_INNER_VID_VALID, 1, 1, 221, 1 },
               {SOC_PPC_FP_QUAL_IRPP_SRC_SYST_PORT, 1, 1, 224, 16 },
               {SOC_PPC_FP_QUAL_CMPRSD_INNER_VID, 1, 1, 240, 12 },
               {SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID, 1, 1, 256, 12 },
               {SOC_PPC_FP_QUAL_INITIAL_VID, 1, 1, 272, 12 },
               {SOC_PPC_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR, 1, 1, 288, 16 },
               {SOC_PPC_FP_QUAL_MPLS_LABEL_RANGE_BASE_LIF2, 1, 1, 304, 16 },
               {SOC_PPC_FP_QUAL_MPLS_LABEL_RANGE_BASE_LIF1, 1, 1, 320, 16 },
               {SOC_PPC_FP_QUAL_MPLS_LABEL_RANGE_BASE_LIF0, 1, 1, 336, 16 },
               {SOC_PPC_FP_QUAL_LABEL3_IDX, 1, 1, 352, 16 },
               {SOC_PPC_FP_QUAL_LABEL2_IDX, 1, 1, 368, 16 },
               {SOC_PPC_FP_QUAL_LABEL1_IDX, 1, 1, 384, 16 },
               {SOC_PPC_FP_QUAL_IRPP_SRC_PP_PORT, 1, 1, 400, 9 }, 
               {SOC_PPC_FP_QUAL_VLAN_DOMAIN, 1, 1, 416, 9 },
               
               {SOC_PPC_FP_QUAL_IRPP_SRC_TM_PORT, 1, 1, 440, 8 },
               {SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND_LEM_DOMAIN, 1, 1, 448, 6 },
                  {SOC_PPC_FP_QUAL_MACT_DOMAIN, 1, 1, 448, 5 },
                  {SOC_PPC_FP_QUAL_VT_LEM_1ST_LOOKUP_NOT_FOUND, 1, 1, 453, 1 },
               {SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND_LEM_SA_DROP, 1, 1, 456, 2 },
                  {SOC_PPC_FP_QUAL_MACT_SA_DROP, 1, 1, 456, 1 },
                  {SOC_PPC_FP_QUAL_VT_LEM_1ST_LOOKUP_NOT_FOUND, 1, 1, 457, 1 },
               {SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND_LEM_IS_LEARN_LIF, 1, 1, 464, 2 },
                  {SOC_PPC_FP_QUAL_MACT_IS_LEARN, 1, 1, 464, 1 },
                  {SOC_PPC_FP_QUAL_VT_LEM_1ST_LOOKUP_NOT_FOUND, 1, 1, 465, 1 },
               {SOC_PPC_FP_QUAL_LEM_DYNAMIC_LEM_1ST_LOOKUP_FOUND_LEM, 1, 1, 472, 2 },
                  {SOC_PPC_FP_QUAL_VT_LEM_1ST_LOOKUP_FOUND, 1, 1, 472, 1 },
                  {SOC_PPC_FP_QUAL_MACT_DYNAMIC, 1, 1, 473, 1 },
               {SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND_DESTINATION, 1, 1, 480, 20 },
                  {SOC_PPC_FP_QUAL_MACT_DESTINATION, 1, 1, 480, 19 },
                  {SOC_PPC_FP_QUAL_VT_LEM_1ST_LOOKUP_NOT_FOUND, 1, 1, 499, 1 },
               {SOC_PPC_FP_QUAL_IRPP_PEM_GENERAL_DATA, 1, 1, 504, 64 },
               {SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES, 1, 1, 568, 32 },
               {SOC_PPC_FP_QUAL_IRPP_ALL_ONES, 1, 1, 600, 32 },
               {SOC_PPC_FP_QUAL_IRPP_PROG_VAR, 1, 1, 632, 16 }
        };

CONST STATIC
    ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES
    Arad_pmf_ce_tt_info[] = {
               {SOC_PPC_FP_QUAL_INITIAL_VSI, 1, 1, 0, 16 },
               {SOC_PPC_FP_QUAL_IN_RIF_VALID_VRF, 1, 1, 16, 13 },
                  {SOC_PPC_FP_QUAL_IN_RIF_VALID, 1, 1, 16, 1 },
                  {SOC_PPC_FP_QUAL_IRPP_VRF, 1, 1, 17, 12 },
               {SOC_PPC_FP_QUAL_VT_LOOKUP0_FOUND, 1, 1, 32, 1 },
               {SOC_PPC_FP_QUAL_IN_RIF_VALID_IN_RIF, 1, 1, 40, 13 },
                  {SOC_PPC_FP_QUAL_IN_RIF_VALID, 1, 1, 40, 1 },
                  {SOC_PPC_FP_QUAL_IRPP_IN_RIF, 1, 1, 41, 12 },
               {SOC_PPC_FP_QUAL_CMPRSD_INNER_VID, 1, 1, 56, 12 },
               {SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID, 1, 1, 72, 12 },
               {SOC_PPC_FP_QUAL_INITIAL_VID, 1, 1, 88, 12 },
               {SOC_PPC_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR, 1, 1, 104, 16 },
               {SOC_PPC_FP_QUAL_MPLS_LABEL_RANGE_BASE_LIF2, 1, 1, 120, 16 },
               {SOC_PPC_FP_QUAL_MPLS_LABEL_RANGE_BASE_LIF1, 1, 1, 136, 16 },
               {SOC_PPC_FP_QUAL_MPLS_LABEL_RANGE_BASE_LIF0, 1, 1, 152, 16 },
               {SOC_PPC_FP_QUAL_LABEL3_IDX, 1, 1, 168, 16 },
               {SOC_PPC_FP_QUAL_LABEL2_IDX, 1, 1, 184, 16 },
               {SOC_PPC_FP_QUAL_LABEL1_IDX, 1, 1, 200, 16 },
               {SOC_PPC_FP_QUAL_VLAN_DOMAIN, 1, 1, 216, 8 },
               {SOC_PPC_FP_QUAL_IRPP_SRC_PP_PORT, 1, 1, 224, 8 },
               {SOC_PPC_FP_QUAL_IRPP_SRC_TM_PORT, 1, 1, 232, 8 },
               {SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND_LEM_DOMAIN, 1, 1, 240, 6 },
                  {SOC_PPC_FP_QUAL_MACT_DOMAIN, 1, 1, 240, 5 },
                  {SOC_PPC_FP_QUAL_VT_LEM_1ST_LOOKUP_NOT_FOUND, 1, 1, 245, 1 },
               {SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND_LEM_SA_DROP, 1, 1, 248, 2 },
                  {SOC_PPC_FP_QUAL_MACT_SA_DROP, 1, 1, 248, 1 },
                  {SOC_PPC_FP_QUAL_VT_LEM_1ST_LOOKUP_NOT_FOUND, 1, 1, 249, 1 },
               {SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND_LEM_IS_LEARN_LIF, 1, 1, 256, 2 },
                  {SOC_PPC_FP_QUAL_MACT_IS_LEARN, 1, 1, 256, 1 },
                  {SOC_PPC_FP_QUAL_VT_LEM_1ST_LOOKUP_NOT_FOUND, 1, 1, 257, 1 },
               {SOC_PPC_FP_QUAL_LEM_DYNAMIC_LEM_1ST_LOOKUP_FOUND_LEM, 1, 1, 264, 2 },
                  {SOC_PPC_FP_QUAL_VT_LEM_1ST_LOOKUP_FOUND, 1, 1, 264, 1 },
                  {SOC_PPC_FP_QUAL_MACT_DYNAMIC, 1, 1, 265, 1 },
               {SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND_DESTINATION, 1, 1, 272, 20 },
                  {SOC_PPC_FP_QUAL_MACT_DESTINATION, 1, 1, 272, 19 },
                  {SOC_PPC_FP_QUAL_VT_LEM_1ST_LOOKUP_NOT_FOUND, 1, 1, 291, 1 },
               {SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES, 1, 1, 296, 32 },
               {SOC_PPC_FP_QUAL_IRPP_ALL_ONES, 1, 1, 328, 32 },
               {SOC_PPC_FP_QUAL_IRPP_PROG_VAR, 1, 1, 360, 16 }
    };

CONST STATIC
    ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES
    Jericho_pmf_ce_tt_info[] = {
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER3, 1, 1, 0, 11 },
                  {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER3_IP_FRAGMENTED, 1, 1, 6, 1 },
               {SOC_PPC_FP_QUAL_ISB_FOUND_IN_LIF_IDX, 1, 1, 16, 18 },                  
                  {SOC_PPC_FP_QUAL_ISB_FOUND, 1, 1, 16, 1 },
                  {SOC_PPC_FP_QUAL_ISB_IN_LIF_IDX, 1, 1, 17, 17 },
               {SOC_PPC_FP_QUAL_ISA_FOUND_IN_LIF_IDX, 1, 1, 40, 18 },                  
                  {SOC_PPC_FP_QUAL_ISA_FOUND, 1, 1, 40, 1 },
                  {SOC_PPC_FP_QUAL_ISA_IN_LIF_IDX, 1, 1, 41, 17 },
               {SOC_PPC_FP_QUAL_VT_TCAM_MATCH_IN_LIF_IDX, 1, 1, 64, 18 },
                  {SOC_PPC_FP_QUAL_VT_TCAM_IN_LIF_IDX, 1, 1, 64, 17 },
                  {SOC_PPC_FP_QUAL_VT_TCAM_MATCH, 1, 1, 81, 1 },
               {SOC_PPC_FP_QUAL_IN_LIF_PROFILE, 1, 1, 88, 4 },
               {SOC_PPC_FP_QUAL_IRPP_IN_LIF, 1, 1, 96, 18 },
               {SOC_PPC_FP_QUAL_INITIAL_VSI, 1, 1, 120, 16 },
               {SOC_PPC_FP_QUAL_IN_RIF_VALID_VRF, 1, 1, 136, 15 },
                  {SOC_PPC_FP_QUAL_IN_RIF_VALID, 1, 1, 136, 1 },
                  {SOC_PPC_FP_QUAL_IRPP_VRF, 1, 1, 137, 14 },
               {SOC_PPC_FP_QUAL_IN_RIF_VALID_RIF_PROFILE, 1, 1, 152, 7 },
                  {SOC_PPC_FP_QUAL_IN_RIF_VALID, 1, 1, 152, 1 },
                  {SOC_PPC_FP_QUAL_IRPP_IN_RIF_PROFILE, 1, 1, 153, 6 },
               {SOC_PPC_FP_QUAL_VT_LOOKUP0_FOUND, 1, 1, 160, 1 },
               {SOC_PPC_FP_QUAL_IN_RIF_VALID_IN_RIF, 1, 1, 168, 16 },
                  {SOC_PPC_FP_QUAL_IN_RIF_VALID, 1, 1, 168, 1 },
                  {SOC_PPC_FP_QUAL_IRPP_IN_RIF, 1, 1, 169, 15 },
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2, 1, 1, 184, 11 },
                  {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_BOS, 1, 1, 184, 1 },
                  {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_GRE_PARSED, 1, 1, 184, 4 }, 
                  {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_IP_HAS_OPTIONS, 1, 1, 188, 1 },
                  {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_IP_FRAGMENTED, 1, 1, 190, 1 },
                  {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_NEXT_PROTOCOL, 1, 1, 191, 4 },
               {SOC_PPC_FP_QUAL_MPLS_KEY3, 1, 1, 200, 46 },
                  {SOC_PPC_FP_QUAL_KEY3_16_INST0, 1, 1, 200, 16 },
                  {SOC_PPC_FP_QUAL_KEY3_16_INST1, 1, 1, 216, 16 },
                  {SOC_PPC_FP_QUAL_KEY3_16_INST2, 1, 1, 232, 14 },
                  {SOC_PPC_FP_QUAL_KEY3_32_INST0, 1, 1, 200, 32 },
                  {SOC_PPC_FP_QUAL_KEY3_32_INST1, 1, 1, 232, 14 },
                  {SOC_PPC_FP_QUAL_KEY3_LABEL, 1, 1, 200, 20 },
                  {SOC_PPC_FP_QUAL_KEY3_OUTER_VID, 1, 1, 220, 12 },
                  {SOC_PPC_FP_QUAL_KEY3_OUTER_VID_VALID, 1, 1, 232, 1 },
                  {SOC_PPC_FP_QUAL_KEY3_EVPN_BOS_EXPECTED, 1, 1, 232, 1 },
                  {SOC_PPC_FP_QUAL_KEY3_INNER_VID, 1, 1, 233, 12 },
                  {SOC_PPC_FP_QUAL_KEY3_INNER_VID_VALID, 1, 1, 245, 1 },
               {SOC_PPC_FP_QUAL_MPLS_KEY2, 1, 1, 248, 46 },
                  {SOC_PPC_FP_QUAL_KEY2_16_INST0, 1, 1, 248, 16 },
                  {SOC_PPC_FP_QUAL_KEY2_16_INST1, 1, 1, 264, 16 },
                  {SOC_PPC_FP_QUAL_KEY2_16_INST2, 1, 1, 280, 14 },
                  {SOC_PPC_FP_QUAL_KEY2_32_INST0, 1, 1, 248, 32 },
                  {SOC_PPC_FP_QUAL_KEY2_32_INST1, 1, 1, 280, 14 },
                  {SOC_PPC_FP_QUAL_KEY2_LABEL, 1, 1, 248, 20 },
                  {SOC_PPC_FP_QUAL_KEY2_OUTER_VID, 1, 1, 268, 12 },
                  {SOC_PPC_FP_QUAL_KEY2_OUTER_VID_VALID, 1, 1, 280, 1 },
                  {SOC_PPC_FP_QUAL_KEY2_EVPN_BOS_EXPECTED, 1, 1, 280, 1},
                  {SOC_PPC_FP_QUAL_KEY2_INNER_VID, 1, 1, 281, 12 },
                  {SOC_PPC_FP_QUAL_KEY2_INNER_VID_VALID, 1, 1, 293, 1},
               {SOC_PPC_FP_QUAL_MPLS_KEY1, 1, 1, 296, 46 },
                  {SOC_PPC_FP_QUAL_KEY1_16_INST0, 1, 1, 296, 16 },
                  {SOC_PPC_FP_QUAL_KEY1_16_INST1, 1, 1, 312, 16 },
                  {SOC_PPC_FP_QUAL_KEY1_16_INST2, 1, 1, 328, 14 },  
                  {SOC_PPC_FP_QUAL_KEY1_32_INST0, 1, 1, 296, 32 },
                  {SOC_PPC_FP_QUAL_KEY1_32_INST1, 1, 1, 328, 14 },
                  {SOC_PPC_FP_QUAL_KEY1_LABEL, 1, 1, 296, 20 },
                  {SOC_PPC_FP_QUAL_KEY1_OUTER_VID, 1, 1, 316, 12 },
                  {SOC_PPC_FP_QUAL_KEY1_OUTER_VID_VALID, 1, 1, 328, 1 },
                  {SOC_PPC_FP_QUAL_KEY1_EVPN_BOS_EXPECTED, 1, 1, 328, 1},
                  {SOC_PPC_FP_QUAL_KEY1_INNER_VID, 1, 1, 329, 12 },
                  {SOC_PPC_FP_QUAL_KEY1_INNER_VID_VALID, 1, 1, 341, 1 },
               {SOC_PPC_FP_QUAL_MPLS_KEY0, 1, 1, 344, 46 },
                  {SOC_PPC_FP_QUAL_KEY0_16_INST0, 1, 1, 344, 16 },
                  {SOC_PPC_FP_QUAL_KEY0_16_INST1, 1, 1, 360, 16 },
                  {SOC_PPC_FP_QUAL_KEY0_16_INST2, 1, 1, 376, 14 },
                  {SOC_PPC_FP_QUAL_KEY0_32_INST0, 1, 1, 344, 32 },
                  {SOC_PPC_FP_QUAL_KEY0_32_INST1, 1, 1, 376, 14 },
                  {SOC_PPC_FP_QUAL_KEY0_LABEL, 1, 1, 344, 20 },
                  {SOC_PPC_FP_QUAL_KEY0_OUTER_VID, 1, 1, 364, 12 },
                  {SOC_PPC_FP_QUAL_KEY0_OUTER_VID_VALID, 1, 1, 376, 1 },
                  {SOC_PPC_FP_QUAL_KEY0_EVPN_BOS_EXPECTED, 1, 1, 376, 1},
                  {SOC_PPC_FP_QUAL_KEY0_INNER_VID, 1, 1, 377, 12 },
                  {SOC_PPC_FP_QUAL_KEY0_INNER_VID_VALID, 1, 1, 389, 1 },
               {SOC_PPC_FP_QUAL_IRPP_SRC_SYST_PORT, 1, 1, 392, 16 },
               {SOC_PPC_FP_QUAL_CMPRSD_INNER_VID, 1, 1, 408, 12 },
               {SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID, 1, 1, 424, 12 },
               {SOC_PPC_FP_QUAL_INITIAL_VID, 1, 1, 440, 12 },
               {SOC_PPC_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR, 1, 1, 456, 16 },
               {SOC_PPC_FP_QUAL_MPLS_LABEL_RANGE_BASE_LIF2, 1, 1, 472, 16 },
               {SOC_PPC_FP_QUAL_MPLS_LABEL_RANGE_BASE_LIF1, 1, 1, 488, 16 },
               {SOC_PPC_FP_QUAL_MPLS_LABEL_RANGE_BASE_LIF0, 1, 1, 504, 16 },
               {SOC_PPC_FP_QUAL_LABEL3_IDX, 1, 1, 520, 16 },
               {SOC_PPC_FP_QUAL_LABEL2_IDX, 1, 1, 536, 16 },
               {SOC_PPC_FP_QUAL_LABEL1_IDX, 1, 1, 552, 16 },
               {SOC_PPC_FP_QUAL_IRPP_SRC_PP_PORT, 1, 1, 568, 9 },  
               {SOC_PPC_FP_QUAL_VLAN_DOMAIN, 1, 1, 584, 9 },
               
               {SOC_PPC_FP_QUAL_IRPP_SRC_TM_PORT, 1, 1, 608, 8 },
               {SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND_LEM_DOMAIN, 1, 1, 616, 6 },
                  {SOC_PPC_FP_QUAL_MACT_DOMAIN, 1, 1, 616, 5 },
                  {SOC_PPC_FP_QUAL_VT_LEM_1ST_LOOKUP_NOT_FOUND, 1, 1, 621, 1 },
               {SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND_LEM_SA_DROP, 1, 1, 624, 2 },
                  {SOC_PPC_FP_QUAL_MACT_SA_DROP, 1, 1, 624, 1 },
                  {SOC_PPC_FP_QUAL_VT_LEM_1ST_LOOKUP_NOT_FOUND, 1, 1, 625, 1 },
               {SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND_LEM_IS_LEARN_LIF, 1, 1, 632, 2 },
                  {SOC_PPC_FP_QUAL_MACT_IS_LEARN, 1, 1, 632, 1 },
                  {SOC_PPC_FP_QUAL_VT_LEM_1ST_LOOKUP_NOT_FOUND, 1, 1, 633, 1 },
               {SOC_PPC_FP_QUAL_LEM_DYNAMIC_LEM_1ST_LOOKUP_FOUND_LEM, 1, 1, 640, 2 },
                  {SOC_PPC_FP_QUAL_VT_LEM_1ST_LOOKUP_FOUND, 1, 1, 640, 1 },
                  {SOC_PPC_FP_QUAL_MACT_DYNAMIC, 1, 1, 641, 1 },
               {SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND_DESTINATION, 1, 1, 648, 20 },
                  {SOC_PPC_FP_QUAL_MACT_DESTINATION, 1, 1, 648, 19 },
                  {SOC_PPC_FP_QUAL_VT_LEM_1ST_LOOKUP_NOT_FOUND, 1, 1, 667, 1 },
               {SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES, 1, 1, 672, 32 },
               {SOC_PPC_FP_QUAL_IRPP_ALL_ONES, 1, 1, 704, 32 },
               {SOC_PPC_FP_QUAL_IRPP_PROG_VAR, 1, 1, 736, 16 }
};

CONST STATIC
    ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES
    Qax_pmf_ce_tt_info[] = {
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER3, 1, 1, 0, 11 },
                  {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER3_IP_FRAGMENTED, 1, 1, 6, 1 },
               {SOC_PPC_FP_QUAL_ISB_FOUND_IN_LIF_IDX, 1, 1, 16, 18 },
                   {SOC_PPC_FP_QUAL_ISB_FOUND, 1, 1, 16, 1 },
                   {SOC_PPC_FP_QUAL_ISB_IN_LIF_IDX, 1, 1, 17, 17 },
               {SOC_PPC_FP_QUAL_ISA_FOUND_IN_LIF_IDX, 1, 1, 40, 18 },
                   {SOC_PPC_FP_QUAL_ISA_FOUND, 1, 1, 40, 1 },
                   {SOC_PPC_FP_QUAL_ISA_IN_LIF_IDX, 1, 1, 41, 17 },                  
               {SOC_PPC_FP_QUAL_VT_TCAM_MATCH_IN_LIF_IDX, 1, 1, 64, 18 },
                  {SOC_PPC_FP_QUAL_VT_TCAM_IN_LIF_IDX, 1, 1, 64, 17 },
                  {SOC_PPC_FP_QUAL_VT_TCAM_MATCH, 1, 1, 81, 1 },
               {SOC_PPC_FP_QUAL_IN_LIF_PROFILE, 1, 1, 88, 4 },
               {SOC_PPC_FP_QUAL_IRPP_IN_LIF, 1, 1, 96, 18 },
               {SOC_PPC_FP_QUAL_INITIAL_VSI, 1, 1, 120, 16 },
               {SOC_PPC_FP_QUAL_IN_RIF_VALID_VRF, 1, 1, 136, 15 },
                  {SOC_PPC_FP_QUAL_IN_RIF_VALID, 1, 1, 136, 1 },
                  {SOC_PPC_FP_QUAL_IRPP_VRF, 1, 1, 137, 14 },
               {SOC_PPC_FP_QUAL_IN_RIF_VALID_RIF_PROFILE, 1, 1, 152, 7 },
                  {SOC_PPC_FP_QUAL_IN_RIF_VALID, 1, 1, 152, 1 },
                  {SOC_PPC_FP_QUAL_IRPP_IN_RIF_PROFILE, 1, 1, 153, 6 },
               {SOC_PPC_FP_QUAL_VT_LOOKUP0_FOUND, 1, 1, 160, 1 },
               {SOC_PPC_FP_QUAL_IN_RIF_VALID_IN_RIF, 1, 1, 168, 16 },
                  {SOC_PPC_FP_QUAL_IN_RIF_VALID, 1, 1, 168, 1 },
                  {SOC_PPC_FP_QUAL_IRPP_IN_RIF, 1, 1, 169, 15 },
               {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2, 1, 1, 184, 11 },
                  {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_BOS, 1, 1, 184, 1 },
                  {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_GRE_PARSED, 1, 1, 184, 4 }, 
                  {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_IP_HAS_OPTIONS, 1, 1, 188, 1 },
                  {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_IP_FRAGMENTED, 1, 1, 190, 1 },
                  {SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_NEXT_PROTOCOL, 1, 1, 191, 4 },
               {SOC_PPC_FP_QUAL_MPLS_KEY3, 1, 1, 200, 47 },
                  {SOC_PPC_FP_QUAL_KEY3_16_INST0, 1, 1, 200, 16 },
                  {SOC_PPC_FP_QUAL_KEY3_16_INST1, 1, 1, 216, 16 },
                  {SOC_PPC_FP_QUAL_KEY3_16_INST2, 1, 1, 232, 14 },
                  {SOC_PPC_FP_QUAL_KEY3_32_INST0, 1, 1, 200, 32 },
                  {SOC_PPC_FP_QUAL_KEY3_32_INST1, 1, 1, 232, 14 },
                  {SOC_PPC_FP_QUAL_KEY3_LABEL, 1, 1, 200, 20 },
                  {SOC_PPC_FP_QUAL_KEY3_OUTER_VID, 1, 1, 220, 12 },
                  {SOC_PPC_FP_QUAL_KEY3_OUTER_VID_VALID, 1, 1, 232, 1 },
                  {SOC_PPC_FP_QUAL_KEY3_EVPN_BOS_EXPECTED, 1, 1, 232, 1 },
                  {SOC_PPC_FP_QUAL_KEY3_INNER_VID, 1, 1, 233, 12 },
                  {SOC_PPC_FP_QUAL_KEY3_INNER_VID_VALID, 1, 1, 245, 1 },
               {SOC_PPC_FP_QUAL_MPLS_KEY2, 1, 1, 248, 47 },
                  {SOC_PPC_FP_QUAL_KEY2_16_INST0, 1, 1, 248, 16 },
                  {SOC_PPC_FP_QUAL_KEY2_16_INST1, 1, 1, 264, 16 },
                  {SOC_PPC_FP_QUAL_KEY2_16_INST2, 1, 1, 280, 14 },
                  {SOC_PPC_FP_QUAL_KEY2_32_INST0, 1, 1, 248, 32 },
                  {SOC_PPC_FP_QUAL_KEY2_32_INST1, 1, 1, 280, 14 },
                  {SOC_PPC_FP_QUAL_KEY2_LABEL, 1, 1, 248, 20 },
                  {SOC_PPC_FP_QUAL_KEY2_OUTER_VID, 1, 1, 268, 12 },
                  {SOC_PPC_FP_QUAL_KEY2_OUTER_VID_VALID, 1, 1, 280, 1 },
                  {SOC_PPC_FP_QUAL_KEY2_EVPN_BOS_EXPECTED, 1, 1, 280, 1},
                  {SOC_PPC_FP_QUAL_KEY2_INNER_VID, 1, 1, 281, 12 },
                  {SOC_PPC_FP_QUAL_KEY2_INNER_VID_VALID, 1, 1, 293, 1 },
               {SOC_PPC_FP_QUAL_MPLS_KEY1, 1, 1, 296, 47 },
                  {SOC_PPC_FP_QUAL_KEY1_16_INST0, 1, 1, 296, 16 },
                  {SOC_PPC_FP_QUAL_KEY1_16_INST1, 1, 1, 312, 16 },
                  {SOC_PPC_FP_QUAL_KEY1_16_INST2, 1, 1, 328, 14 },
                  {SOC_PPC_FP_QUAL_KEY1_32_INST0, 1, 1, 296, 32 },
                  {SOC_PPC_FP_QUAL_KEY1_32_INST1, 1, 1, 328, 14 },
                  {SOC_PPC_FP_QUAL_KEY1_LABEL, 1, 1, 296, 20 },
                  {SOC_PPC_FP_QUAL_KEY1_OUTER_VID, 1, 1, 316, 12 },
                  {SOC_PPC_FP_QUAL_KEY1_OUTER_VID_VALID, 1, 1, 328, 1 },
                  {SOC_PPC_FP_QUAL_KEY1_EVPN_BOS_EXPECTED, 1, 1, 328, 1},
                  {SOC_PPC_FP_QUAL_KEY1_INNER_VID, 1, 1, 329, 12 },
                  {SOC_PPC_FP_QUAL_KEY1_INNER_VID_VALID, 1, 1, 341, 1 },
               {SOC_PPC_FP_QUAL_MPLS_KEY0, 1, 1, 344, 47 },
                  {SOC_PPC_FP_QUAL_KEY0_16_INST0, 1, 1, 344, 16 },
                  {SOC_PPC_FP_QUAL_KEY0_16_INST1, 1, 1, 360, 16 },
                  {SOC_PPC_FP_QUAL_KEY0_16_INST2, 1, 1, 376, 14 },
                  {SOC_PPC_FP_QUAL_KEY0_32_INST0, 1, 1, 344, 32 },
                  {SOC_PPC_FP_QUAL_KEY0_32_INST1, 1, 1, 376, 14 },
                  {SOC_PPC_FP_QUAL_KEY0_LABEL, 1, 1, 344, 20 },
                  {SOC_PPC_FP_QUAL_KEY0_OUTER_VID, 1, 1, 364, 12 },
                  {SOC_PPC_FP_QUAL_KEY0_OUTER_VID_VALID, 1, 1, 376, 1 },
                  {SOC_PPC_FP_QUAL_KEY0_EVPN_BOS_EXPECTED, 1, 1, 376, 1},
                  {SOC_PPC_FP_QUAL_KEY0_INNER_VID, 1, 1, 377, 12 },
                  {SOC_PPC_FP_QUAL_KEY0_INNER_VID_VALID, 1, 1, 389, 1 },
               {SOC_PPC_FP_QUAL_IRPP_SRC_SYST_PORT, 1, 1, 392, 16 },
               {SOC_PPC_FP_QUAL_CMPRSD_INNER_VID, 1, 1, 408, 12 },
               {SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID, 1, 1, 424, 12 },
               {SOC_PPC_FP_QUAL_INITIAL_VID, 1, 1, 440, 12 },
               {SOC_PPC_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR, 1, 1, 456, 16 },
               {SOC_PPC_FP_QUAL_MPLS_LABEL_RANGE_BASE_LIF2, 1, 1, 472, 16 },
               {SOC_PPC_FP_QUAL_MPLS_LABEL_RANGE_BASE_LIF1, 1, 1, 488, 16 },
               {SOC_PPC_FP_QUAL_MPLS_LABEL_RANGE_BASE_LIF0, 1, 1, 504, 16 },
               {SOC_PPC_FP_QUAL_LABEL3_IDX, 1, 1, 520, 16 },
               {SOC_PPC_FP_QUAL_LABEL2_IDX, 1, 1, 536, 16 },
               {SOC_PPC_FP_QUAL_LABEL1_IDX, 1, 1, 552, 16 },
               {SOC_PPC_FP_QUAL_IRPP_SRC_PP_PORT, 1, 1, 568, 9 },  
               {SOC_PPC_FP_QUAL_VLAN_DOMAIN, 1, 1, 584, 9 },
               
               {SOC_PPC_FP_QUAL_IRPP_SRC_TM_PORT, 1, 1, 608, 8 },
               {SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND_LEM_DOMAIN, 1, 1, 616, 6 },
                  {SOC_PPC_FP_QUAL_MACT_DOMAIN, 1, 1, 616, 5 },
                  {SOC_PPC_FP_QUAL_VT_LEM_1ST_LOOKUP_NOT_FOUND, 1, 1, 621, 1 },
               {SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND_LEM_SA_DROP, 1, 1, 624, 2 },
                  {SOC_PPC_FP_QUAL_MACT_SA_DROP, 1, 1, 624, 1 },
                  {SOC_PPC_FP_QUAL_VT_LEM_1ST_LOOKUP_NOT_FOUND, 1, 1, 625, 1 },
               {SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND_LEM_IS_LEARN_LIF, 1, 1, 632, 2 },
                  {SOC_PPC_FP_QUAL_MACT_IS_LEARN, 1, 1, 632, 1 },
                  {SOC_PPC_FP_QUAL_VT_LEM_1ST_LOOKUP_NOT_FOUND, 1, 1, 633, 1 },
               {SOC_PPC_FP_QUAL_LEM_DYNAMIC_LEM_1ST_LOOKUP_FOUND_LEM, 1, 1, 640, 2 },
                  {SOC_PPC_FP_QUAL_VT_LEM_1ST_LOOKUP_FOUND, 1, 1, 640, 1 },
                  {SOC_PPC_FP_QUAL_MACT_DYNAMIC, 1, 1, 641, 1 },
               {SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND_DESTINATION, 1, 1, 648, 20 },
                  {SOC_PPC_FP_QUAL_MACT_DESTINATION, 1, 1, 648, 19 },
                  {SOC_PPC_FP_QUAL_VT_LEM_1ST_LOOKUP_NOT_FOUND, 1, 1, 667, 1 },
               {SOC_PPC_FP_QUAL_IRPP_PEM_GENERAL_DATA, 1, 1, 672, 64 },
               {SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES, 1, 1, 736, 32 },
               {SOC_PPC_FP_QUAL_IRPP_ALL_ONES, 1, 1, 768, 32 },
               {SOC_PPC_FP_QUAL_IRPP_PROG_VAR, 1, 1, 800, 16 }
    };







uint8
  arad_pmf_low_level_ce_is_32b_ce(
     SOC_SAND_IN  int                            unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32               ce_ndx
  )
{
    if (ARAD_PMF_LOW_LEVEL_PROG_GROUP_IS_ALL_32) {
        return TRUE;
    }
    else {
        if (SOC_IS_JERICHO(unit)) {
                if ((stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT) || (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT)) {
                    if ((ce_ndx == 4) || (ce_ndx == 5) || (ce_ndx == 10) || (ce_ndx == 11)) {
                        return TRUE;
                    } else {
                        return FALSE;
                    }
                }
            }
    }

    
    
    return (((ce_ndx >= ARAD_PMF_CE_32_BITS_INDEX_FIRST) && (ce_ndx < ARAD_PMF_CE_16_BITS_INDEX_SECOND)) 
                || ((ce_ndx >= ARAD_PMF_CE_32_BITS_INDEX_SECOND) && (ce_ndx <= ARAD_PMF_LOW_LEVEL_CE_NDX_MAX))
                || ((ce_ndx >= ARAD_PMF_CE_32_BITS_INDEX_FIRST+ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_LSB) && (ce_ndx < ARAD_PMF_CE_16_BITS_INDEX_SECOND+ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_LSB))
                || ((ce_ndx >= ARAD_PMF_CE_32_BITS_INDEX_SECOND+ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_LSB) && (ce_ndx <= ARAD_PMF_LOW_LEVEL_CE_NDX_MAX+ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_LSB))
               );
}

uint8
  arad_pmf_low_level_ce_is_second_group(
     SOC_SAND_IN  int                            unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint32               ce_ndx
  )
{
    uint8
        ce_group_ndx = (ce_ndx >= ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_GROUP)? 0x1:0x0;

    return ce_group_ndx;
}

uint32
  arad_pmf_ce_header_info_find(
	  SOC_SAND_IN  int                           unit,
	  SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE            header_field,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
	  SOC_SAND_OUT uint8                           *is_found,
	  SOC_SAND_OUT ARAD_PMF_CE_HEADER_QUAL_INFO	      *qual_info
  )
{
  uint32
	  table_line,
      res = SOC_SAND_OK;
  uint8
      found = FALSE;
  ARAD_PMF_CE_QUAL_INFO general_qual_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PMF_CE_HEADER_INFO_FIND);

  sal_memset(qual_info, 0x0, sizeof(ARAD_PMF_CE_HEADER_QUAL_INFO));

  *is_found = FALSE;
  
  if (stage == SOC_PPC_FP_DATABASE_STAGE_EGRESS) {
      ARAD_DO_NOTHING_AND_EXIT;
  }

  
  if(SOC_PPC_FP_IS_QUAL_TYPE_USER_DEFINED(header_field)) {

      res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fp_info.udf.get(
                unit,
                stage,
                header_field - SOC_PPC_FP_QUAL_HDR_USER_DEF_0,
                &general_qual_info
            );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

	  *qual_info = general_qual_info.header_qual_info;
	  if (general_qual_info.is_header_qual == TRUE) {
		  *is_found = TRUE;
		  goto exit;
	  }
  }

  found = FALSE;
  for (table_line = 0; (table_line < sizeof(Arad_pmf_ce_header_info) / sizeof(ARAD_PMF_CE_HEADER_QUAL_INFO)) 
                            && (!found); ++table_line) {
      if (header_field == Arad_pmf_ce_header_info[table_line].qual_type) {
              found = TRUE;
              break;
      }
  }

  *is_found = found;
  if (found) {
      sal_memcpy(qual_info, &(Arad_pmf_ce_header_info[table_line]), sizeof(ARAD_PMF_CE_HEADER_QUAL_INFO));

      
      if ((header_field == SOC_PPC_FP_QUAL_HDR_DSP_EXTENSION_AFTER_FTMH) 
          || (header_field == SOC_PPC_FP_QUAL_HDR_STACKING_EXT_AFTER_DSP_EXT) || (header_field == SOC_PPC_FP_QUAL_TRAP_QUALIFIER_FHEI)) {
          if ((SOC_DPP_CONFIG(unit)->arad->init.fabric.ftmh_lb_ext_mode == ARAD_MGMT_FTMH_LB_EXT_MODE_ENABLED)
#ifdef BCM_88660_A0
               || (SOC_DPP_CONFIG(unit)->arad->init.fabric.ftmh_lb_ext_mode == ARAD_MGMT_FTMH_LB_EXT_MODE_STANDBY_MC_LB)
               || (SOC_DPP_CONFIG(unit)->arad->init.fabric.ftmh_lb_ext_mode == ARAD_MGMT_FTMH_LB_EXT_MODE_FULL_HASH)
#endif 
          ) {
              qual_info->msb += 8;
              qual_info->lsb += 8;
          }
      }

      
      if ((_BCM_PPD_PON_PP_PORT_MAPPING_BY_PASS_IN_JER(unit) || (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "gpon_application_enable", 0))) &&
          (header_field == SOC_PPC_FP_QUAL_TUNNEL_ID)) {
          qual_info->msb -= 5;
      }
  }

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_ce_header_info_find()", header_field, *is_found);
}





STATIC
uint32
  arad_pmf_ce_internal_field_table_size_find(
      SOC_SAND_IN  int                           unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint8                            is_signal_loop,
      SOC_SAND_OUT uint32                           *table_size
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PMF_CE_INTERNAL_FIELD_TABLE_SIZE_FIND);

  switch (stage) {
  case SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP:
      if (!is_signal_loop) {
          if (SOC_IS_JERICHO_PLUS(unit)) {
              *table_size = sizeof(Qax_ce_irpp_flp_info) / sizeof(ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES);
          } else if (SOC_IS_JERICHO(unit)) {
              *table_size = sizeof(Jericho_ce_irpp_flp_info) / sizeof(ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES);
          } else {
              *table_size = sizeof(Arad_pmf_ce_irpp_flp_info) / sizeof(ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES);
          }
      }
      else {
          *table_size = 0; 
      }
      break;
  case SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB:
      if (!is_signal_loop) {
          if (SOC_IS_JERICHO_PLUS(unit)) {
              *table_size = sizeof(Qax_pmf_ce_irpp_slb_info) / sizeof(ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES);
          } else if (SOC_IS_JERICHO(unit)) {
              *table_size = sizeof(Jericho_pmf_ce_irpp_slb_info) / sizeof(ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES);
          } else {
              *table_size = sizeof(Arad_plus_pmf_ce_irpp_slb_info) / sizeof(ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES);
          }
      }
      else {
          *table_size = 0; 
      }
      break;
  case SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF:
      if (!is_signal_loop) {
          if (SOC_IS_JERICHO_PLUS(unit)) {
              *table_size = sizeof(Qax_pmf_ce_irpp_pmf_info) / sizeof(ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES);
          } else if (SOC_IS_JERICHO(unit)) {
              *table_size = sizeof(Jericho_pmf_ce_irpp_pmf_info) / sizeof(ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES);
          } else if (SOC_IS_ARADPLUS(unit) && (!(SOC_IS_ARDON(unit)))) {
              *table_size = sizeof(Arad_plus_pmf_ce_irpp_pmf_info) / sizeof(ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES);
          } else {
              *table_size = sizeof(Arad_pmf_ce_irpp_pmf_info) / sizeof(ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES);
          }
      }
      else {
          *table_size = 0; 
      }
      break;
  case SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT:
      if (!is_signal_loop) {
          if (SOC_IS_JERICHO_PLUS(unit)) {
              *table_size = sizeof(Qax_pmf_ce_vt_info) / sizeof(ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES);
          }
          else
          {
              if (SOC_IS_JERICHO(unit)) {
                  *table_size = sizeof(Jericho_pmf_ce_vt_info) / sizeof(ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES);
              } else {
                  *table_size = sizeof(Arad_pmf_ce_vt_info) / sizeof(ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES);
              }
          }
      }
      else {
          *table_size = 0; 
      }
      break;
  case SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT:
      if (!is_signal_loop) {
          if (SOC_IS_JERICHO_PLUS(unit)) {
              *table_size = sizeof(Qax_pmf_ce_tt_info) / sizeof(ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES);
          }
          else
          {
              if (SOC_IS_JERICHO(unit)) {
                  *table_size = sizeof(Jericho_pmf_ce_tt_info) / sizeof(ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES);
              } else {
                  *table_size = sizeof(Arad_pmf_ce_tt_info) / sizeof(ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES);
              }
          }
      }
      else {
          *table_size = 0; 
      }
      break;
  case SOC_PPC_FP_DATABASE_STAGE_EGRESS: 
      if (!is_signal_loop) {
          *table_size = sizeof(Arad_pmf_ce_egress_pmf_info) / sizeof(ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES);
      }
      else {
          if (SOC_IS_JERICHO(unit)) {
              *table_size =  sizeof(Jericho_pmf_ce_egress_pmf_signal) / sizeof(ARAD_PMF_CE_IRPP_QUALIFIER_SIGNAL);
          } else {
              *table_size =  sizeof(Arad_pmf_ce_egress_pmf_signal) / sizeof(ARAD_PMF_CE_IRPP_QUALIFIER_SIGNAL);
          }
      }
      break;
  default:
      
      *table_size = 0;
      break;
  }

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_ce_internal_field_table_size_find()", 0, 0);
}

STATIC
uint32
  arad_pmf_ce_internal_field_table_line_find(
      SOC_SAND_IN  int                           unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE           stage,
      SOC_SAND_IN  uint8                            is_signal_loop,
      SOC_SAND_IN  uint32                           table_line,
      SOC_SAND_OUT ARAD_PMF_CE_IRPP_QUALIFIER_INFO  *ce_qualifier_info,
      SOC_SAND_IN  uint8                            in_init
  )
{
    uint32
        res = SOC_SAND_OK;
    uint8
        fp_key_changed_size;
    CONST ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES
        *pmf_ce_irpp_flp_pmf_info;
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PMF_CE_INTERNAL_FIELD_TABLE_LINE_FIND);

  
  switch (stage) {
  case SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP:
  case SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB:
  case SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF:
  case SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT:
  case SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT:
      if (!is_signal_loop) {
          if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP) {
              if (SOC_IS_JERICHO_PLUS(unit)) {
                  pmf_ce_irpp_flp_pmf_info = &(Qax_ce_irpp_flp_info[table_line]);
              } else if (SOC_IS_JERICHO(unit)) {
                  pmf_ce_irpp_flp_pmf_info = &(Jericho_ce_irpp_flp_info[table_line]);
              } else {
                  pmf_ce_irpp_flp_pmf_info = &(Arad_pmf_ce_irpp_flp_info[table_line]);
              }
          }
          else if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB) {
              if (SOC_IS_JERICHO_PLUS(unit)) {
                  pmf_ce_irpp_flp_pmf_info = &(Qax_pmf_ce_irpp_slb_info[table_line]);
              } else if (SOC_IS_JERICHO(unit)) {
                  pmf_ce_irpp_flp_pmf_info = &(Jericho_pmf_ce_irpp_slb_info[table_line]);
              } else {
                  pmf_ce_irpp_flp_pmf_info = &(Arad_plus_pmf_ce_irpp_slb_info[table_line]);
              }
          }
          else if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT) {
              if (SOC_IS_JERICHO_PLUS(unit)) {
                  pmf_ce_irpp_flp_pmf_info = &(Qax_pmf_ce_vt_info[table_line]);
              }
              else
              {
                  if (SOC_IS_JERICHO(unit)) {
                      pmf_ce_irpp_flp_pmf_info = &(Jericho_pmf_ce_vt_info[table_line]);
                  } else {
                      pmf_ce_irpp_flp_pmf_info = &(Arad_pmf_ce_vt_info[table_line]);
                  }
              }
          }
          else if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT) {
              if (SOC_IS_JERICHO_PLUS(unit)) {
                  pmf_ce_irpp_flp_pmf_info = &(Qax_pmf_ce_tt_info[table_line]);
              }
              else
              {
                  if (SOC_IS_JERICHO(unit)) {
                      pmf_ce_irpp_flp_pmf_info = &(Jericho_pmf_ce_tt_info[table_line]);
                  } else {
                      pmf_ce_irpp_flp_pmf_info = &(Arad_pmf_ce_tt_info[table_line]);
                  }
              }
          }
          else {
              if (SOC_IS_JERICHO_PLUS(unit)) {
                  pmf_ce_irpp_flp_pmf_info = &(Qax_pmf_ce_irpp_pmf_info[table_line]);
              } else if (SOC_IS_JERICHO(unit)) {
                  pmf_ce_irpp_flp_pmf_info = &(Jericho_pmf_ce_irpp_pmf_info[table_line]);
              } else if (SOC_IS_ARADPLUS(unit) && (!(SOC_IS_ARDON(unit)))) {
                  pmf_ce_irpp_flp_pmf_info = &(Arad_plus_pmf_ce_irpp_pmf_info[table_line]);
              } else {
                  pmf_ce_irpp_flp_pmf_info = &(Arad_pmf_ce_irpp_pmf_info[table_line]);
              }
          }

          sal_memcpy(&(ce_qualifier_info->info), pmf_ce_irpp_flp_pmf_info, sizeof(ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES));
          
          ce_qualifier_info->hw_buffer_jericho = 0;
          if (SOC_IS_JERICHO(unit) && 
              ((stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF)||(stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP))) {
              ce_qualifier_info->info.is_lsb = 1;
              ce_qualifier_info->info.is_msb = 1;
              
			  ce_qualifier_info->hw_buffer_jericho = pmf_ce_irpp_flp_pmf_info->is_msb? 1 :0;
          }

            
          if (pmf_ce_irpp_flp_pmf_info->irpp_field == SOC_PPC_FP_QUAL_IRPP_KEY_CHANGED
              || (SOC_IS_JERICHO_PLUS(unit) && 
             (pmf_ce_irpp_flp_pmf_info->irpp_field == SOC_PPC_FP_QUAL_IRPP_TCAM_0_RESULT ||
              pmf_ce_irpp_flp_pmf_info->irpp_field == SOC_PPC_FP_QUAL_IRPP_TCAM_1_RESULT ||
              pmf_ce_irpp_flp_pmf_info->irpp_field == SOC_PPC_FP_QUAL_IRPP_TCAM_2_RESULT ||
              pmf_ce_irpp_flp_pmf_info->irpp_field == SOC_PPC_FP_QUAL_IRPP_TCAM_3_RESULT || 
              pmf_ce_irpp_flp_pmf_info->irpp_field == SOC_PPC_FP_QUAL_IRPP_KAPS_PASS1_PAYLOAD 
              ))) 
          {
              res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fp_info.key_change_size.get(unit, stage, &fp_key_changed_size);
              SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
              ce_qualifier_info->info.qual_nof_bits = fp_key_changed_size;
          }

#ifdef BCM_88660_A0
          
          
          if (SOC_IS_ARADPLUS(unit) 
              && (pmf_ce_irpp_flp_pmf_info->irpp_field == SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_FOUND || pmf_ce_irpp_flp_pmf_info->irpp_field == SOC_PPC_FP_QUAL_LEM_1ST_LOOKUP_RESULT)
              && (!in_init)) 
          {
              if (soc_property_get(unit, spn_RESILIENT_HASH_ENABLE, 0)) {
                  SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_STR("Qualifiers that use the 1st LEM lookup cannot be used with SLB.\n")));
              }
          }

          
          if (SOC_IS_ARADPLUS(unit)){
              if ((pmf_ce_irpp_flp_pmf_info->irpp_field == SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_1) &&
                 (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_rpf_fwd_parallel", 0) == 1)){
                  ce_qualifier_info->info.buffer_lsb += 16;
                  ce_qualifier_info->info.qual_nof_bits = 16;
              }
              if ((pmf_ce_irpp_flp_pmf_info->irpp_field == SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_2) && 
                 (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_rpf_fwd_parallel", 0) == 1)){
                  
                  ce_qualifier_info->info.qual_nof_bits = 32;
              }                  

#if defined(INCLUDE_KBP)
              if ((pmf_ce_irpp_flp_pmf_info->irpp_field == SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_0) || 
                  (pmf_ce_irpp_flp_pmf_info->irpp_field == SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_1) || 
                  (pmf_ce_irpp_flp_pmf_info->irpp_field == SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_2) ||
				  (pmf_ce_irpp_flp_pmf_info->irpp_field == SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_3) || 
                  (pmf_ce_irpp_flp_pmf_info->irpp_field == SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_4) ||
                  (pmf_ce_irpp_flp_pmf_info->irpp_field == SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_5) || 
                  (pmf_ce_irpp_flp_pmf_info->irpp_field == SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_6) ||
                  (pmf_ce_irpp_flp_pmf_info->irpp_field == SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_7)){
                  uint8 is_dynamic_res_enabled;

                  res = sw_state_access[unit].dpp.soc.arad.tm.kbp_info.Arad_pmf_ce_dynamic_kbp_qualifiers_enabled.get(unit,
                                        &is_dynamic_res_enabled); 
                  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

                      if (is_dynamic_res_enabled) {
                          ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES kbp_qualifiers_values;
                          int pos = pmf_ce_irpp_flp_pmf_info->irpp_field - SOC_PPC_FP_QUAL_ELK_LOOKUP_RESULT_0;

                          res = sw_state_access[unit].dpp.soc.arad.tm.kbp_info.Arad_pmf_ce_dynamic_kbp_qualifiers_values.get(unit, pos,
                                        &kbp_qualifiers_values); 
                          SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

                          ce_qualifier_info->info.qual_nof_bits = kbp_qualifiers_values.qual_nof_bits;
                          ce_qualifier_info->info.buffer_lsb    = kbp_qualifiers_values.buffer_lsb;      
						  ce_qualifier_info->info.is_lsb 		= kbp_qualifiers_values.is_lsb;
                          ce_qualifier_info->info.is_msb    	= kbp_qualifiers_values.is_msb;    
						  ce_qualifier_info->info.irpp_field 	= kbp_qualifiers_values.irpp_field;

						  LOG_DEBUG(BSL_LS_SOC_FP,
								   (BSL_META_U(unit,"pmf_ce_irpp_flp_pmf_info->irpp_field = %d, ce_qualifier_info->info.qual_nof_bits=%d, ce_qualifier_info->info.buffer_lsb=%d\n"),
													 pmf_ce_irpp_flp_pmf_info->irpp_field, ce_qualifier_info->info.qual_nof_bits, ce_qualifier_info->info.buffer_lsb));
                      }
              }
#endif
          }
#endif 
            
          
          if( ((stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF) || (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP)) &&
              (soc_property_get((unit), spn_PMF_VSI_PROFILE_FULL_RANGE , 0))&&
              (pmf_ce_irpp_flp_pmf_info->irpp_field == SOC_PPC_FP_QUAL_VSI_PROFILE))
          {
               ce_qualifier_info->info.qual_nof_bits  = 4;
               ce_qualifier_info->info.buffer_lsb = (ce_qualifier_info->info.buffer_lsb - 2);
          }
      }
      
      break;
  case SOC_PPC_FP_DATABASE_STAGE_EGRESS: 
      if (!is_signal_loop) {
          if (SOC_IS_JERICHO_PLUS(unit)) {
              sal_memcpy(&(ce_qualifier_info->info), &(Qax_pmf_ce_egress_pmf_info[table_line]), sizeof(ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES));
          } else {
              
              sal_memcpy(&(ce_qualifier_info->info), &(Arad_pmf_ce_egress_pmf_info[table_line]), sizeof(ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES));
          }
      }
      else { 
          if (SOC_IS_JERICHO(unit)) {
              sal_memcpy(&(ce_qualifier_info->signal), &(Jericho_pmf_ce_egress_pmf_signal[table_line]), sizeof(ARAD_PMF_CE_IRPP_QUALIFIER_SIGNAL));
          } else {
              sal_memcpy(&(ce_qualifier_info->signal), &(Arad_pmf_ce_egress_pmf_signal[table_line]), sizeof(ARAD_PMF_CE_IRPP_QUALIFIER_SIGNAL));
          }
      }
      break;
  default:
      
      SOC_SAND_ERR_IF_ABOVE_MAX(stage, SOC_PPC_NOF_FP_DATABASE_STAGES-1, ARAD_PMF_LOW_LEVEL_STAGE_OUT_OF_RANGE_ERR, 32, exit);
  }

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_ce_internal_field_table_line_find()", 0, 0);
}



uint32
  arad_pmf_ce_internal_field_info_find(
      SOC_SAND_IN  int                           unit,
      SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE            irpp_field,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint8                           is_msb,
      SOC_SAND_OUT uint8                           *is_found,
      SOC_SAND_OUT ARAD_PMF_CE_IRPP_QUALIFIER_INFO     *qual_info
  )
{
  uint32
      res = SOC_SAND_OK,
      table_line;
  uint8
      is_signal_loop = FALSE;
  ARAD_PMF_CE_IRPP_QUALIFIER_INFO
      ce_qualifier_info;
  ARAD_PMF_CE_QUAL_INFO
	  general_qual_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PMF_CE_INTERNAL_FIELD_INFO_FIND);

  
  *is_found = FALSE;

  
  if(SOC_PPC_FP_IS_QUAL_TYPE_USER_DEFINED(irpp_field)) {

      res = sw_state_access[unit].dpp.soc.arad.tm.pmf.fp_info.udf.get(
                unit,
                stage,
                irpp_field - SOC_PPC_FP_QUAL_HDR_USER_DEF_0,
                &general_qual_info
            );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 8, exit);

	  *qual_info = is_msb ? general_qual_info.irpp_qual_info[1] : general_qual_info.irpp_qual_info[0];
	  if (qual_info->info.qual_nof_bits && (!(SOC_PPC_FP_IS_QUAL_TYPE_USER_DEFINED(qual_info->info.irpp_field)))) {
		  *is_found = TRUE;
		  goto exit;
	  }
  }

  for (is_signal_loop = FALSE; is_signal_loop <= (stage == SOC_PPC_FP_DATABASE_STAGE_EGRESS); is_signal_loop++) {

      sal_memset(&ce_qualifier_info, 0x0, sizeof(ARAD_PMF_CE_IRPP_QUALIFIER_INFO));

      
      if (is_signal_loop) {
          res = sw_state_access[unit].dpp.soc.arad.tm.pmf.ce_signals_table.get(unit, stage, irpp_field, &table_line);
      }
      else {
          res = sw_state_access[unit].dpp.soc.arad.tm.pmf.ce_internal_table.get(unit, stage, irpp_field, is_msb, &table_line);
      }
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

      
      if ( table_line != ARAD_PMF_CE_INTERNAL_TABLE_LINE_NOT_FOUND ) 
      {
          arad_pmf_ce_internal_field_table_line_find(
                unit,
                stage,
                is_signal_loop,
                table_line,
                &ce_qualifier_info,
                TRUE
              );

          if (!is_signal_loop) {
              sal_memcpy(&(qual_info->info), &(ce_qualifier_info.info), sizeof(ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES));
              qual_info->hw_buffer_jericho = ce_qualifier_info.hw_buffer_jericho;
              *is_found = TRUE; 
          }
          else { 
              sal_memcpy(&(qual_info->signal), &(ce_qualifier_info.signal), sizeof(ARAD_PMF_CE_IRPP_QUALIFIER_SIGNAL));
          }
      }
  }

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_ce_internal_field_info_find()", 0, 0);
}


uint32
  arad_pmf_ce_internal_field_offset_compute(
      SOC_SAND_IN  int                           unit,
      SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE            irpp_field,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint8                           is_msb,
      SOC_SAND_IN  uint32		                   nof_bits,
      SOC_SAND_IN  uint32		                   qual_lsb,
      SOC_SAND_IN  uint32                             lost_bits, 
      SOC_SAND_IN  uint8		                   is_32b_ce,
      SOC_SAND_OUT uint8                           *is_found,
      SOC_SAND_OUT uint32						   *ce_offset,
      SOC_SAND_OUT uint32						   *nof_bits_for_ce,
      SOC_SAND_OUT uint32                          *hw_buffer_jericho 
  )
{
  uint32
      real_lsb,
      real_nof_bits,
      nof_bits_lost_at_lsb,
      real_lsb_after_resolution,
      max_offset,
      resolution_ce_in_bits,
    res = SOC_SAND_OK;
  uint8
	  found = FALSE;
  ARAD_PMF_CE_IRPP_QUALIFIER_INFO     
	  qual_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PMF_CE_INTERNAL_FIELD_OFFSET_COMPUTE);

  
  res = arad_pmf_ce_internal_field_info_find(
            unit,
            irpp_field,
            stage,
            is_msb,
            &found,
            &qual_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *is_found = FALSE;
  *ce_offset = 0;
  *nof_bits_for_ce = 0;
  
  if (!found) {
	  ARAD_DO_NOTHING_AND_EXIT;
  }

  *is_found = TRUE;

  
  real_nof_bits = (nof_bits)? nof_bits: (qual_info.info.qual_nof_bits - qual_lsb);

  
  real_lsb = qual_info.info.buffer_lsb - lost_bits + qual_lsb;

  resolution_ce_in_bits = (is_32b_ce)? ARAD_PMF_CE_INSTR_RESOLUTION_32B_BYTES:ARAD_PMF_CE_INSTR_RESOLUTION_16B_NIBBLES;
  LOG_DEBUG(BSL_LS_SOC_FP,
             (BSL_META_U(unit,
                         "real_nof_bits %d nof_bits %d qual_info.info.qual_nof_bits %d qual_lsb %d real_lsb %d=%d-%d+%d "
                         "                        \n\r"),
            		 real_nof_bits, nof_bits , qual_info.info.qual_nof_bits,qual_lsb, real_lsb, qual_info.info.buffer_lsb, lost_bits, qual_lsb));

  if(stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT)
  {
      max_offset = (SOC_DPP_DEFS_GET(unit, tt_ce_buffer_length)/ resolution_ce_in_bits) - ARAD_PMF_CE_BUFFER_SHIFT_IN_BITS;
  }else if(stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT)
  {
      max_offset = (SOC_DPP_DEFS_GET(unit, vt_ce_buffer_length)/ resolution_ce_in_bits) - ARAD_PMF_CE_BUFFER_SHIFT_IN_BITS;
  }else
  {
      max_offset = (ARAD_PMF_CE_TOTAL_BUFFER_LENGTH_IN_BITS / resolution_ce_in_bits) - ARAD_PMF_CE_BUFFER_SHIFT_IN_BITS;
  }


  
  real_lsb_after_resolution = real_lsb / resolution_ce_in_bits;
  nof_bits_lost_at_lsb = real_lsb - (real_lsb_after_resolution * resolution_ce_in_bits);
  *ce_offset = max_offset - real_lsb_after_resolution;
  *nof_bits_for_ce = nof_bits_lost_at_lsb + real_nof_bits;
  *hw_buffer_jericho = qual_info.hw_buffer_jericho;

  LOG_DEBUG(BSL_LS_SOC_FP,
             (BSL_META_U(unit,
                         "max_offset %d real_lsb_after_resolution %d/%d=%d nof_bits_lost_at_lsb %d *ce_offset %d *nof_bits_for_ce %d\n\r"
                         "                        \n\r"),
            		 max_offset, real_lsb , resolution_ce_in_bits,real_lsb_after_resolution, nof_bits_lost_at_lsb, *ce_offset, *nof_bits_for_ce));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_ce_internal_field_offset_compute()", 0, 0);
}



uint32
  arad_pmf_ce_internal_field_offset_qual_find(
      SOC_SAND_IN  int                           unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
      SOC_SAND_IN  uint8                           is_msb,
      SOC_SAND_IN  uint32                           nof_bits,
      SOC_SAND_IN  uint32                           ce_offset,
      SOC_SAND_IN  uint8                           is_32b_ce,
      SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE         irpp_field_in, 
      SOC_SAND_OUT uint8                           *is_found,
      SOC_SAND_OUT  SOC_PPC_FP_QUAL_TYPE           *irpp_field,
      SOC_SAND_OUT  uint32                           *qual_lsb,
      SOC_SAND_OUT  uint32                           *qual_nof_bits
 )
{
  uint32
      res,
      table_size,
      table_line,
      real_lsb,
      real_msb,
      real_lsb_after_resolution,
      max_offset,
      resolution_ce_in_bits;
  uint8
      is_signal_loop = 0, 
      is_qual_lsb_inside = FALSE,
      found = FALSE;
  ARAD_PMF_CE_IRPP_QUALIFIER_INFO
      ce_qualifier_info;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PMF_CE_INTERNAL_FIELD_OFFSET_QUAL_FIND);


  
  resolution_ce_in_bits = (is_32b_ce)? ARAD_PMF_CE_INSTR_RESOLUTION_32B_BYTES:ARAD_PMF_CE_INSTR_RESOLUTION_16B_NIBBLES;

  if(stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT)
  {
      max_offset = (SOC_DPP_DEFS_GET(unit, tt_ce_buffer_length)/ resolution_ce_in_bits) - ARAD_PMF_CE_BUFFER_SHIFT_IN_BITS;
  }else if(stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT)
  {
      max_offset = (SOC_DPP_DEFS_GET(unit, vt_ce_buffer_length)/ resolution_ce_in_bits) - ARAD_PMF_CE_BUFFER_SHIFT_IN_BITS;
  }else
  {
      max_offset = (ARAD_PMF_CE_TOTAL_BUFFER_LENGTH_IN_BITS / resolution_ce_in_bits) - ARAD_PMF_CE_BUFFER_SHIFT_IN_BITS;
  }

  real_lsb_after_resolution = max_offset - ce_offset;
  real_lsb =  real_lsb_after_resolution * resolution_ce_in_bits;
  real_msb = real_lsb + (nof_bits-1);

  res = arad_pmf_ce_internal_field_table_size_find(
            unit,
            stage,
            is_signal_loop,
            &table_size
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  found = FALSE;
  for (table_line = 0; (table_line < table_size) && (!found); ++table_line) {
      
      res = arad_pmf_ce_internal_field_table_line_find(
                unit,
                stage,
                is_signal_loop,
                table_line,
                &ce_qualifier_info,
                FALSE
              );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      
      if((BCM_FIELD_ENTRY_INVALID != irpp_field_in)
          && (SOC_PPC_FP_QUAL_IRPP_INVALID != irpp_field_in))
      {
          if(ce_qualifier_info.info.irpp_field == irpp_field_in
             && ((is_msb && ce_qualifier_info.info.is_msb) 
                 || (!is_msb && ce_qualifier_info.info.is_lsb))) {
              found = TRUE;
              *irpp_field = ce_qualifier_info.info.irpp_field;
              break;
          }
      }
      else if ((real_msb >= ce_qualifier_info.info.buffer_lsb)
          && (real_msb <= (ce_qualifier_info.info.buffer_lsb + (ce_qualifier_info.info.qual_nof_bits - 1)))) {
          if ((is_msb == ce_qualifier_info.info.is_msb) 
              
              
              && ((!is_msb) == ce_qualifier_info.info.is_lsb)){
              found = TRUE;
              *irpp_field = ce_qualifier_info.info.irpp_field;
              break;
          }
      }
  }

  
  if (!found) {
      ARAD_DO_NOTHING_AND_EXIT;
  }

  *is_found = TRUE;

  
  is_qual_lsb_inside = (real_lsb <= ce_qualifier_info.info.buffer_lsb)? TRUE:FALSE;
  *qual_lsb = is_qual_lsb_inside? 0 : real_lsb - ce_qualifier_info.info.buffer_lsb;
  
  
  *qual_nof_bits = real_msb - (ce_qualifier_info.info.buffer_lsb  + (*qual_lsb)) + 1;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_ce_internal_field_offset_qual_find()", 0, 0);
}



uint32
  arad_pmf_ce_nof_real_bits_compute_header(
      SOC_SAND_IN  int                           unit,
      SOC_SAND_IN  ARAD_PMF_CE_PACKET_HEADER_INFO      *info, 
      SOC_SAND_IN  uint8                           is_msb,
      SOC_SAND_IN  uint8                           is_32b_ce,
      SOC_SAND_OUT uint32                           *nof_bits_for_ce 
  )
{
  uint32
      resolution_ce_in_bits,
      nof_lost_lsb_bits;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PMF_CE_NOF_REAL_BITS_COMPUTE_HEADER);

  resolution_ce_in_bits = (is_32b_ce)? ARAD_PMF_CE_INSTR_RESOLUTION_32B_BYTES:ARAD_PMF_CE_INSTR_RESOLUTION_16B_NIBBLES;

  
  nof_lost_lsb_bits = (-(info->offset + info->nof_bits)) % resolution_ce_in_bits;
  *nof_bits_for_ce = info->nof_bits + nof_lost_lsb_bits;
  LOG_DEBUG(BSL_LS_SOC_FP,
             (BSL_META_U(unit,
                         "info->offset  %d + info->nof_bits %d MOD resolution_ce_in_bits %d = nof_lost_lsb_bits %d\n\r"
                         "info->nof_bits %d + nof_lost_lsb_bits %d = nof_bits_for_ce =%d "
                         "                        \n\r"),
            		 info->offset , info->nof_bits , resolution_ce_in_bits, nof_lost_lsb_bits, info->nof_bits, nof_lost_lsb_bits, *nof_bits_for_ce));

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_ce_nof_real_bits_compute_header()", 0, 0);
}


uint32
  arad_pmf_low_level_ce_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
    int 
        i,j;
    uint32
        res,
        table_line,
        tbl_data[ARAD_PMF_CE_TABLE_LENGTH_IN_UINT32S],
        group_id;
    soc_mem_t 
        mem_arr_ihb_pmf_pass_key_gen[ARAD_PMF_CE_NOF_GROUP_TABLES] = {
            IHB_PMF_PASS_1_KEY_GEN_LSBm, IHB_PMF_PASS_1_KEY_GEN_MSBm,
            IHB_PMF_PASS_2_KEY_GEN_LSBm, IHB_PMF_PASS_2_KEY_GEN_MSBm};
    uint32
        ftmh_lb_key_enable,
        ftmh_stacking_ext_mode,
        user_header_0_size,
        user_header_1_size,
        user_header_egress_pmf_offset_0,
        user_header_egress_pmf_offset_1,
        header_size,
        header_code_ndx,
        header_code_id,
        header_offset,
        header_format_code_key,
        table_size=0;
    ARAD_PMF_LOW_LEVEL_HEADER_FORMAT_PROFILE_USUAL
        format_code_usual[] = {
            
            {    1,     0,          1,        0,            0,           0 },
            {    1,     1,          1,        0,            0,           0 },
            {    1,     0,          1,        1,            0,           0 },
            {    1,     1,          1,        1,            0,           0 },
            {    1,     0,          1,        2,            0,           0 },
            {    1,     1,          1,        2,            0,           0 },
            {    1,     0,          1,        0,            1,           0 },
            {    1,     1,          1,        0,            1,           0 },
            {    1,     0,          1,        1,            1,           0 },
            {    1,     1,          1,        1,            1,           0 },
            {    1,     0,          1,        2,            1,           0 },
            {    1,     1,          1,        2,            1,           0 },
        };
    uint8 
        profile_found,
        is_signal_loop = FALSE;
    soc_reg_above_64_val_t
        reg_above64_val;
    soc_reg_t
      global_f = SOC_IS_JERICHO(unit)? ECI_GLOBAL_SYS_HEADER_CFGr: ECI_GLOBALFr;
    uint8
        stage;
    const ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES
        *pmf_ce_irpp_flp_pmf_info = NULL;
    const ARAD_PMF_CE_IRPP_QUALIFIER_SIGNAL
        *pmf_ce_signals_info = NULL;

    uint32 number_of_profiles = SOC_DPP_DEFS_GET(unit, sys_hdrs_format_code_profiles);
    uint32 value_1_offset = SOC_DPP_DEFS_GET(unit, value_1_offset_bits);
    uint32 value_2_offset = SOC_DPP_DEFS_GET(unit, value_2_offset_bits);
    uint32 number_of_format_header_codes = SOC_DPP_DEFS_GET(unit, sys_hdrs_format_code_bits);

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PMF_LOW_LEVEL_CE_INIT_UNSAFE);

  sal_memset(tbl_data, 0x0, ARAD_PMF_CE_TABLE_LENGTH_IN_UINT32S * sizeof(uint32));


  
  for (group_id = 0; group_id < ARAD_PMF_CE_NOF_GROUP_TABLES; ++group_id) { 
      res = arad_fill_table_with_entry(unit, mem_arr_ihb_pmf_pass_key_gen[group_id] , MEM_BLOCK_ANY, tbl_data); 
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  }

  
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, global_f, REG_PORT_ANY, 0, FTMH_LB_KEY_EXT_ENf, &ftmh_lb_key_enable));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, global_f, REG_PORT_ANY, 0, FTMH_STACKING_EXT_ENABLEf, &ftmh_stacking_ext_mode));
  res = arad_pmf_db_fes_user_header_sizes_get(
            unit,
            &user_header_0_size,
            &user_header_1_size,
            &user_header_egress_pmf_offset_0,
            &user_header_egress_pmf_offset_1
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if ((user_header_0_size + user_header_1_size) != 0) 
  {
  for (table_line = 0; table_line < (sizeof(format_code_usual) / sizeof(ARAD_PMF_LOW_LEVEL_HEADER_FORMAT_PROFILE_USUAL)); ++table_line)
  {
      
      if ( soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "cascaded_eg_learning",0) ) 
      {
          format_code_usual[table_line].learn_ext_present = TRUE;
      }

      
      header_size = (user_header_egress_pmf_offset_0 / 8)  +
          (ftmh_lb_key_enable * 1) + (ftmh_stacking_ext_mode * 2) +
          (format_code_usual[table_line].ftmh_present * 9) + (format_code_usual[table_line].dsp_ext_present * 2) + 
          (format_code_usual[table_line].pph_base * 7) + 
          ((format_code_usual[table_line].fhei_size == 0)? 0: ((format_code_usual[table_line].fhei_size == 1)? 3: 5)) + 
          (format_code_usual[table_line].eei_present * 3) + (format_code_usual[table_line].learn_ext_present * 5);

#ifdef BCM_88660
      
      if (SOC_IS_ARADPLUS((unit))) {
          if (soc_property_get((unit), spn_SCHEDULER_COMPENSATION_ENABLE, 0)) {
              header_size += 1;
          }
      }
#endif 

      
      SOC_REG_ABOVE_64_CLEAR(reg_above64_val);  
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR,READ_EGQ_SYSTEM_HEADERS_VALUE_1_OFFSETr(unit, SOC_CORE_ALL, reg_above64_val));
      profile_found = FALSE;
      for (header_code_ndx = 0; (header_code_ndx <  number_of_profiles ) && (!profile_found) ; ++header_code_ndx)
      {
          header_offset = 0;
          SHR_BITCOPY_RANGE(&header_offset, 0, reg_above64_val, (value_1_offset * header_code_ndx),value_1_offset);
          if (header_offset == header_size) {
              
              profile_found = TRUE;
              header_code_id = header_code_ndx;
              break;
          }
          else if (header_offset == 0) {
              
              profile_found = TRUE;
              header_code_id = header_code_ndx;
              break;
          }
      }
      if (!profile_found) {
          LOG_ERROR(BSL_LS_SOC_FP,
                    (BSL_META_U(unit,
                                "Unit %d table_line %d : no profile found for this line.\n\r"), unit, table_line));
          SOC_SAND_SET_ERROR_CODE(ARAD_PMF_LOW_LEVEL_SUB_HEADER_OUT_OF_RANGE_ERR, 80, exit);
      }

      
      SHR_BITCOPY_RANGE(reg_above64_val, (value_1_offset * header_code_id), &header_size, 0,value_1_offset);
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR,WRITE_EGQ_SYSTEM_HEADERS_VALUE_1_OFFSETr(unit, SOC_CORE_ALL,  reg_above64_val));

      
      header_size = header_size + ((user_header_egress_pmf_offset_1 - user_header_egress_pmf_offset_0) / 8) ;
      SOC_REG_ABOVE_64_CLEAR(reg_above64_val); 
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR,READ_EGQ_SYSTEM_HEADERS_VALUE_2_OFFSETr(unit, SOC_CORE_ALL, reg_above64_val));
      SHR_BITCOPY_RANGE(reg_above64_val, (value_2_offset * header_code_id), &header_size, 0, value_2_offset);
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  102,  exit, ARAD_REG_ACCESS_ERR,WRITE_EGQ_SYSTEM_HEADERS_VALUE_2_OFFSETr(unit, SOC_CORE_ALL,  reg_above64_val)); 
   
      SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_EGQ_SYSTEM_HEADERS_FORMAT_CODEr(unit, REG_PORT_ANY, reg_above64_val));
      header_format_code_key = (format_code_usual[table_line].dsp_ext_present << 6) + 
          (format_code_usual[table_line].pph_base << 4) + 
          (format_code_usual[table_line].fhei_size << 2) + 
          (format_code_usual[table_line].eei_present << 1) + (format_code_usual[table_line].learn_ext_present << 0);
      SHR_BITCOPY_RANGE(reg_above64_val, (number_of_format_header_codes * header_format_code_key), &header_code_id, 0, number_of_format_header_codes);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 120, exit, WRITE_EGQ_SYSTEM_HEADERS_FORMAT_CODEr(unit, REG_PORT_ANY, reg_above64_val));
      }
  }

  else
  {
      
      
        SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_EGQ_SYSTEM_HEADERS_FORMAT_CODEr(unit, REG_PORT_ANY, reg_above64_val));
        for (header_format_code_key = 0; header_format_code_key < ARAD_PMF_EGQ_SYSTEM_HEADERS_FORMAT_CODE_NUM_OF_PROFILES; header_format_code_key++ )
        {
            header_code_id = (header_format_code_key & 0x1 ) ? 1 : 0;
            SHR_BITCOPY_RANGE(reg_above64_val, (number_of_format_header_codes * header_format_code_key), &header_code_id, 0, number_of_format_header_codes);
        }
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 120, exit, WRITE_EGQ_SYSTEM_HEADERS_FORMAT_CODEr(unit, REG_PORT_ANY, reg_above64_val));
  }

  
  for (i=0; i< SOC_PPC_NOF_FP_DATABASE_STAGES; i++) {
      for (j=0; j< SOC_PPC_NOF_FP_QUAL_TYPES; j++) {
          sw_state_access[unit].dpp.soc.arad.tm.pmf.ce_internal_table.set(unit, i, j, FALSE, ARAD_PMF_CE_INTERNAL_TABLE_LINE_NOT_FOUND);
          sw_state_access[unit].dpp.soc.arad.tm.pmf.ce_internal_table.set(unit, i, j, TRUE, ARAD_PMF_CE_INTERNAL_TABLE_LINE_NOT_FOUND);
          sw_state_access[unit].dpp.soc.arad.tm.pmf.ce_signals_table.set(unit, i, j, ARAD_PMF_CE_INTERNAL_TABLE_LINE_NOT_FOUND);
      }
  }
  for (stage = SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF ;  stage < SOC_PPC_NOF_FP_DATABASE_STAGES ; stage++ ) 
  {
      
      switch (stage) {
      case SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP:
          if (SOC_IS_JERICHO_PLUS(unit)) { 
              pmf_ce_irpp_flp_pmf_info = &(Qax_ce_irpp_flp_info[0]);
          } else if (SOC_IS_JERICHO(unit)) { 
              pmf_ce_irpp_flp_pmf_info = &(Jericho_ce_irpp_flp_info[0]);
          } else {
              pmf_ce_irpp_flp_pmf_info = &(Arad_pmf_ce_irpp_flp_info[0]);
          }
          break;
      case SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB:
          if (SOC_IS_JERICHO_PLUS(unit)) { 
              pmf_ce_irpp_flp_pmf_info = &(Qax_pmf_ce_irpp_slb_info[0]);
          } else if (SOC_IS_JERICHO(unit)) {           
              pmf_ce_irpp_flp_pmf_info = &(Jericho_pmf_ce_irpp_slb_info[0]);
          } else {
              pmf_ce_irpp_flp_pmf_info = &(Arad_plus_pmf_ce_irpp_slb_info[0]);
          }
          break;
      case SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF:
          if (SOC_IS_JERICHO_PLUS(unit)) { 
              pmf_ce_irpp_flp_pmf_info = &(Qax_pmf_ce_irpp_pmf_info[0]);
          } else if (SOC_IS_JERICHO(unit)) {   
              pmf_ce_irpp_flp_pmf_info = &(Jericho_pmf_ce_irpp_pmf_info[0]);
          } else if (SOC_IS_ARADPLUS(unit) && (!(SOC_IS_ARDON(unit)))) {
              pmf_ce_irpp_flp_pmf_info = &(Arad_plus_pmf_ce_irpp_pmf_info[0]);
          } else {
              pmf_ce_irpp_flp_pmf_info = &(Arad_pmf_ce_irpp_pmf_info[0]);
          }
          break;
      case SOC_PPC_FP_DATABASE_STAGE_EGRESS: 
          if (SOC_IS_JERICHO_PLUS(unit)) {
              pmf_ce_irpp_flp_pmf_info = &(Qax_pmf_ce_egress_pmf_info[0]);
          } else {
              pmf_ce_irpp_flp_pmf_info = &(Arad_pmf_ce_egress_pmf_info[0]); 
          }
          if (SOC_IS_JERICHO(unit)) {
              pmf_ce_signals_info = &(Jericho_pmf_ce_egress_pmf_signal[0]); 
          } else {
              pmf_ce_signals_info = &(Arad_pmf_ce_egress_pmf_signal[0]); 
          }
          break;
      case SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT:
          if (SOC_IS_JERICHO_PLUS(unit)) {
              pmf_ce_irpp_flp_pmf_info = &(Qax_pmf_ce_tt_info[0]);
          }
          else
          {
              if (SOC_IS_JERICHO(unit)) {
                  pmf_ce_irpp_flp_pmf_info = &(Jericho_pmf_ce_tt_info[0]);
              } else {
                  pmf_ce_irpp_flp_pmf_info = &(Arad_pmf_ce_tt_info[0]);
              }
          }
          break;
      case SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT:
          if (SOC_IS_JERICHO_PLUS(unit)) {
              pmf_ce_irpp_flp_pmf_info = &(Qax_pmf_ce_vt_info[0]);
          }
          else
          {
              if (SOC_IS_JERICHO(unit)) {
                  pmf_ce_irpp_flp_pmf_info = &(Jericho_pmf_ce_vt_info[0]);
              } else {
                  pmf_ce_irpp_flp_pmf_info = &(Arad_pmf_ce_vt_info[0]);
              }
          }
           break;
      }

      
      for (is_signal_loop = FALSE; is_signal_loop <= (stage == SOC_PPC_FP_DATABASE_STAGE_EGRESS); is_signal_loop++) 
      {
          
          res = arad_pmf_ce_internal_field_table_size_find(
                    unit,
                    stage,
                    is_signal_loop,
                    &table_size
                  );
          SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

          
          for (table_line = 0; (table_line < table_size) ; ++table_line) 
          {
             
             if (!is_signal_loop)
             {
                 if (SOC_IS_JERICHO(unit)) {
                     sw_state_access[unit].dpp.soc.arad.tm.pmf.ce_internal_table.set(unit, stage, pmf_ce_irpp_flp_pmf_info[table_line].irpp_field, TRUE, table_line);
                     sw_state_access[unit].dpp.soc.arad.tm.pmf.ce_internal_table.set(unit, stage, pmf_ce_irpp_flp_pmf_info[table_line].irpp_field, FALSE, table_line);
                  } else {
                    if ( pmf_ce_irpp_flp_pmf_info[table_line].is_msb )
                      {
                          sw_state_access[unit].dpp.soc.arad.tm.pmf.ce_internal_table.set(unit, stage, pmf_ce_irpp_flp_pmf_info[table_line].irpp_field, TRUE, table_line);
                      }
                      if ( pmf_ce_irpp_flp_pmf_info[table_line].is_lsb ) 
                      {
                          sw_state_access[unit].dpp.soc.arad.tm.pmf.ce_internal_table.set(unit, stage, pmf_ce_irpp_flp_pmf_info[table_line].irpp_field, FALSE, table_line);
                      }
                  }
             }
             else
             {
                 sw_state_access[unit].dpp.soc.arad.tm.pmf.ce_signals_table.set(unit, stage, pmf_ce_signals_info[table_line].irpp_field, table_line);
             }
          }
      }
  }

  ARAD_DO_NOTHING_AND_EXIT;


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_low_level_ce_init_unsafe()", 0, 0);
}


soc_mem_t
  arad_pmf_ce_table_get(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE    stage,
    SOC_SAND_IN  uint8                     is_ce_in_msb,
    SOC_SAND_IN  uint8                     is_second_lookup,
    SOC_SAND_IN  uint8                     is_update_key,
    SOC_SAND_IN  uint8                     ce_ndx
  )
{
    soc_mem_t
        ce_table;

    
    if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP) {
        ce_table = SOC_IS_JERICHO(unit) ? ((is_ce_in_msb)? IHP_FLP_KEY_CONSTRUCTION_MSBm : IHP_FLP_KEY_CONSTRUCTION_LSBm) : IHB_FLP_KEY_CONSTRUCTIONm;
    }
    else if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT){
        ce_table = (ce_ndx < ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_GROUP) ?  IHP_VTT_1ST_LOOKUP_PROGRAM_0m : IHP_VTT_1ST_LOOKUP_PROGRAM_1m;
    }
    else if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT){
        ce_table = (ce_ndx < ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_GROUP) ?  IHP_VTT_2ND_LOOKUP_PROGRAM_0m : IHP_VTT_2ND_LOOKUP_PROGRAM_1m;
    }
#ifdef BCM_88660_A0
    else if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB) {
        ce_table = IHP_FLP_CONSISTENT_HASHING_KEY_GENm;
    }
#endif 
    else if (stage == SOC_PPC_FP_DATABASE_STAGE_EGRESS) {
        ce_table = (is_ce_in_msb)? EGQ_PMF_KEY_GEN_MSBm: EGQ_PMF_KEY_GEN_LSBm;
    }
    else 
    {
        if(SOC_IS_JERICHO_PLUS(unit) && is_update_key)
        {
            
            ce_table = is_ce_in_msb ? IHB_PMF_UPDATE_KEY_GEN_MSBm : IHB_PMF_UPDATE_KEY_GEN_LSBm;
        }
        else if (is_second_lookup)
        {
            ce_table = is_ce_in_msb ? IHB_PMF_PASS_2_KEY_GEN_MSBm : IHB_PMF_PASS_2_KEY_GEN_LSBm;
        }
        else
        {
            ce_table = is_ce_in_msb ? IHB_PMF_PASS_1_KEY_GEN_MSBm : IHB_PMF_PASS_1_KEY_GEN_LSBm;
        }
    }

    return ce_table;
}

static
 uint32
  arad_pmf_ce_line_get(
      SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE           stage,
    SOC_SAND_IN  uint8                            is_ce_in_msb,
    SOC_SAND_IN  uint32                           pmf_pgm_ndx,
    SOC_SAND_IN  uint32                           ce_ndx
  )
{
    uint32
        line_ndx;

#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit) && (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB)) {
        
        line_ndx = pmf_pgm_ndx + (is_ce_in_msb? ARAD_PMF_CE_TBL_2ND_KEY_GROUP_FIRST_LINE: 0); 
    } else
#endif 
    if((stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT) || (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT))
    {
        line_ndx = pmf_pgm_ndx;
    }
    else
    {
        line_ndx = pmf_pgm_ndx + ((ce_ndx >= ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_GROUP)? ARAD_PMF_CE_TBL_2ND_KEY_GROUP_FIRST_LINE: 0); 
    }

    return line_ndx;
}

static
    uint32
        arad_pmf_ce_valid_fld_bit_location_get(
            SOC_SAND_IN  int                         unit,
            SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE           stage,
            SOC_SAND_IN  uint32                           ce_group_ndx,
            SOC_SAND_IN  uint32                           ce_ndx
  )
{
    uint32
        valid_fld_bit_ndx;

#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit) && (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB)) {
        
        valid_fld_bit_ndx = ce_ndx; 
    } else
#endif 
    {
        valid_fld_bit_ndx = ce_ndx - (ce_group_ndx * ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_GROUP); 
    }

    return valid_fld_bit_ndx;
}


soc_field_t
  arad_pmf_ce_instruction_fld_get(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE           stage,
    SOC_SAND_IN  uint32                           ce_ndx
  )
{
    soc_field_t   instruction_fld_ingress_flp[]     =
                                                      { INSTRUCTION_0_16Bf, INSTRUCTION_1_16Bf, INSTRUCTION_2_16Bf,  INSTRUCTION_3_32Bf,
                                                        INSTRUCTION_4_32Bf, INSTRUCTION_5_32Bf, INSTRUCTION_6_16Bf,  INSTRUCTION_7_16Bf,
                                                        INSTRUCTION_8_16Bf, INSTRUCTION_9_32Bf, INSTRUCTION_10_32Bf, INSTRUCTION_11_32Bf };

    soc_field_t   instruction_fld_ingress_flp_jer[] =
                                                      { INSTRUCTION_0_16Bf,  INSTRUCTION_1_16Bf,  INSTRUCTION_2_16Bf,  INSTRUCTION_3_16Bf,
                                                        INSTRUCTION_4_32Bf,  INSTRUCTION_5_32Bf,  INSTRUCTION_6_32Bf,  INSTRUCTION_7_32Bf,
                                                        INSTRUCTION_8_16Bf,  INSTRUCTION_9_16Bf,  INSTRUCTION_10_16Bf, INSTRUCTION_11_16Bf,
                                                        INSTRUCTION_12_32Bf, INSTRUCTION_13_32Bf, INSTRUCTION_14_32Bf, INSTRUCTION_15_32Bf};

    soc_field_t   instruction_fld_ingress_slb[]     =
                                                      { INITIAL_INSTRUCTION_0_32Bf, INITIAL_INSTRUCTION_1_32Bf, INITIAL_INSTRUCTION_2_32Bf, INITIAL_INSTRUCTION_3_32Bf,
                                                        INITIAL_INSTRUCTION_4_32Bf, INITIAL_INSTRUCTION_5_32Bf, INITIAL_INSTRUCTION_6_32Bf, INITIAL_INSTRUCTION_7_32Bf };

    soc_field_t   instruction_fld_ingress_pmf[]     =
                                                      { INSTRUCTION_0_16Bf,  INSTRUCTION_1_16Bf,  INSTRUCTION_2_16Bf,  INSTRUCTION_3_16Bf,
                                                        INSTRUCTION_4_32Bf,  INSTRUCTION_5_32Bf,  INSTRUCTION_6_32Bf,  INSTRUCTION_7_32Bf,
                                                        INSTRUCTION_8_16Bf,  INSTRUCTION_9_16Bf,  INSTRUCTION_10_16Bf, INSTRUCTION_11_16Bf,
                                                        INSTRUCTION_12_32Bf, INSTRUCTION_13_32Bf, INSTRUCTION_14_32Bf, INSTRUCTION_15_32Bf };

    soc_field_t   instruction_fld_egress_pmf[]      =
                                                      { INSTRUCTION_1f,     INSTRUCTION_2f,     INSTRUCTION_3f,     INSTRUCTION_4f,
                                                        INSTRUCTION_5_16Bf, INSTRUCTION_6_16Bf, INSTRUCTION_7_32Bf, INSTRUCTION_8_32Bf };

    soc_field_t   instruction_fld_ingress_vt[]      =
                                                      { KEY_16B_INST_0f, KEY_16B_INST_1f, KEY_32B_INST_0f, KEY_32B_INST_1f,
                                                        KEY_16B_INST_2f, KEY_16B_INST_3f, KEY_32B_INST_2f, KEY_32B_INST_3f };

    soc_field_t   instruction_fld_ingress_vt_jer[]  =
                                                      { KEY_16B_INST_0f, KEY_16B_INST_1f, KEY_16B_INST_2f, KEY_16B_INST_3f,
                                                        KEY_32B_INST_0f, KEY_32B_INST_1f, KEY_16B_INST_4f, KEY_16B_INST_5f,
                                                        KEY_16B_INST_6f, KEY_16B_INST_7f, KEY_32B_INST_2f, KEY_32B_INST_3f };

    soc_field_t   instruction_fld_ingress_tt[]      =
                                                      { KEY_16B_INST_0f, KEY_16B_INST_1f, KEY_32B_INST_0f, KEY_32B_INST_1f,
                                                        KEY_16B_INST_2f, KEY_16B_INST_3f, KEY_32B_INST_2f, KEY_32B_INST_3f };

    soc_field_t   instruction_fld_ingress_tt_jer[]  =
                                                      { KEY_16B_INST_0f, KEY_16B_INST_1f, KEY_16B_INST_2f, KEY_16B_INST_3f,
                                                        KEY_32B_INST_0f, KEY_32B_INST_1f, KEY_16B_INST_4f, KEY_16B_INST_5f,
                                                        KEY_16B_INST_6f, KEY_16B_INST_7f, KEY_32B_INST_2f, KEY_32B_INST_3f };

    soc_field_t
        instruction_field;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PMF_CE_INSTRUCTION_FLD_GET);

    
  SOC_SAND_ERR_IF_ABOVE_MAX(ce_ndx, ARAD_PMF_LOW_LEVEL_CE_NDX_MAX,ARAD_PMF_LOW_LEVEL_CE_NDX_OUT_OF_RANGE_ERR,20,exit);
    if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP) {
        if(SOC_IS_JERICHO(unit))
            instruction_field = instruction_fld_ingress_flp_jer[ce_ndx];
        else
            instruction_field = instruction_fld_ingress_flp[ce_ndx];
    }
    else if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT) {
        if(SOC_IS_JERICHO(unit))
            instruction_field = instruction_fld_ingress_vt_jer[ce_ndx];
        else
            instruction_field = instruction_fld_ingress_vt[ce_ndx];
    }
    else if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT) {
        if(SOC_IS_JERICHO(unit))
            instruction_field = instruction_fld_ingress_tt_jer[ce_ndx];
        else
            instruction_field = instruction_fld_ingress_tt[ce_ndx];
    }
#ifdef BCM_88660_A0
    else if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB) {
        instruction_field = instruction_fld_ingress_slb[ce_ndx];
    }
#endif 
    else if (stage == SOC_PPC_FP_DATABASE_STAGE_EGRESS) {
        instruction_field = instruction_fld_egress_pmf[ce_ndx];
    }
    else {
        instruction_field = instruction_fld_ingress_pmf[ce_ndx];
    }

    return instruction_field;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_ce_instruction_fld_get()", 0, 0);
}

soc_field_t
  arad_pmf_ce_valid_fld_group_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE   stage,
    SOC_SAND_IN  uint32                   ce_group_ndx,
    SOC_SAND_IN  uint32                   pmf_key
  )
{
    soc_field_t valid_fld_ingress_flp[][ARAD_PMF_CE_NOF_GROUP_PER_LSB] =
                                                                 {{KEY_A_INST_0_TO_5_VALIDf, KEY_A_INST_6_TO_11_VALIDf},
                                                                  {KEY_B_INST_0_TO_5_VALIDf, KEY_B_INST_6_TO_11_VALIDf},
                                                                  {KEY_C_INST_0_TO_5_VALIDf, KEY_C_INST_6_TO_11_VALIDf}};

    soc_field_t valid_fld_ingress_flp_jer[][ARAD_PMF_CE_NOF_GROUP_PER_LSB] =
                                                                 {{KEY_A_INST_0_TO_7_VALIDf, KEY_A_INST_8_TO_15_VALIDf},
                                                                  {KEY_B_INST_0_TO_7_VALIDf, KEY_B_INST_8_TO_15_VALIDf},
                                                                  {KEY_C_INST_0_TO_7_VALIDf, KEY_C_INST_8_TO_15_VALIDf},
                                                                  {KEY_D_INST_0_TO_7_VALIDf, KEY_D_INST_8_TO_15_VALIDf}};

    soc_field_t valid_fld_ingress_slb[] = {KEY_INITIAL_INST_0_TO_7_VALIDf};

    soc_field_t valid_fld_ingress_pmf[][ARAD_PMF_CE_NOF_GROUP_PER_LSB] =
                                                                 {{KEY_A_INST_0_TO_7_VALIDf, KEY_A_INST_8_TO_15_VALIDf},
                                                                  {KEY_B_INST_0_TO_7_VALIDf, KEY_B_INST_8_TO_15_VALIDf},
                                                                  {KEY_C_INST_0_TO_7_VALIDf, KEY_C_INST_8_TO_15_VALIDf},
                                                                  {KEY_D_INST_0_TO_7_VALIDf, KEY_D_INST_8_TO_15_VALIDf}};

    soc_field_t valid_fld_egress_pmf[][ARAD_PMF_CE_NOF_GROUP_PER_LSB] =
                                                                 {{INSTRUCTIONS_1234_KEYA_VALIDSf, INSTRUCTIONS_5678_KEYA_VALIDSf},
                                                                  {INSTRUCTIONS_1234_KEYB_VALIDSf, INSTRUCTIONS_5678_KEYB_VALIDSf}};

    soc_field_t valid_fld_ingress_vtt[] = {ISA_KEY_VALIDSf ,ISB_KEY_VALIDSf, TCAM_KEY_VALIDSf};

    soc_field_t
        valid_field;

    
    if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP) {
        if(SOC_IS_JERICHO(unit))
            valid_field = valid_fld_ingress_flp_jer[pmf_key][ce_group_ndx];
        else
            valid_field = valid_fld_ingress_flp[pmf_key][ce_group_ndx];
    }
    else if ((stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT) || (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT)) {
            valid_field = valid_fld_ingress_vtt[pmf_key];
    }
#ifdef BCM_88660_A0
    else if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB) {
        valid_field = valid_fld_ingress_slb[pmf_key];
    }
#endif 
    else if (stage == SOC_PPC_FP_DATABASE_STAGE_EGRESS) {
        valid_field = valid_fld_egress_pmf[pmf_key][ce_group_ndx];
    }
    else {
        valid_field = valid_fld_ingress_pmf[pmf_key][ce_group_ndx];
    }

    return valid_field;
}


STATIC
soc_field_t
  arad_pmf_ce_valid_fld_get(
     SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE           stage,
    SOC_SAND_IN  uint32                           ce_ndx,
    SOC_SAND_IN  uint32                           pmf_key
  )
{
    return arad_pmf_ce_valid_fld_group_get(
             unit,
             stage,
             arad_pmf_low_level_ce_is_second_group(unit, stage, ce_ndx),
             pmf_key
           );
}

uint32  arad_pmf_ce_entry_verify(   SOC_SAND_IN  int                         unit,
                                    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE      stage,
                                    SOC_SAND_IN  uint32                      pmf_pgm_ndx,
                                    SOC_SAND_IN  uint32                      pmf_key,
                                    SOC_SAND_IN  uint32                      ce_ndx,
                                    SOC_SAND_IN  uint8                       is_ce_in_msb,
                                    SOC_SAND_IN  uint8                       is_second_lookup )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PMF_CE_ENTRY_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(pmf_pgm_ndx, ARAD_PMF_LOW_LEVEL_PMF_PGM_NDX_MAX, ARAD_PMF_LOW_LEVEL_PMF_PGM_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(pmf_key, ARAD_PMF_LOW_LEVEL_PMF_KEY_MAX, ARAD_PMF_LOW_LEVEL_PMF_KEY_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(ce_ndx, ARAD_PMF_LOW_LEVEL_CE_NDX_MAX, ARAD_PMF_LOW_LEVEL_CE_NDX_OUT_OF_RANGE_ERR, 30, exit);

  if (((stage == SOC_PPC_FP_DATABASE_STAGE_EGRESS) || (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP)) && (is_second_lookup)) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PMF_LOW_LEVEL_STAGE_OUT_OF_RANGE_ERR, 5, exit);
  }
  else if((SOC_IS_ARADPLUS_AND_BELOW(unit)) && (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP) && (is_ce_in_msb != 0)){
      
      SOC_SAND_SET_ERROR_CODE(ARAD_PMF_LOW_LEVEL_STAGE_OUT_OF_RANGE_ERR, 6, exit);
  }
  else if ((stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT)||(stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT)) {
      if ((is_ce_in_msb != 0) || (is_second_lookup != 0)) {
          
          SOC_SAND_SET_ERROR_CODE(ARAD_PMF_LOW_LEVEL_STAGE_OUT_OF_RANGE_ERR, 7, exit);
      }
  }
#ifdef BCM_88660_A0
  else if (stage == SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB) {
      if (is_second_lookup != 0) {
          
          SOC_SAND_SET_ERROR_CODE(ARAD_PMF_LOW_LEVEL_STAGE_OUT_OF_RANGE_ERR, 8, exit);
      }
  }
#endif 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_ce_entry_verify()", pmf_pgm_ndx, pmf_key);
}

uint32
  arad_pmf_ce_for_key_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE      stage,
    SOC_SAND_IN  uint32                         pmf_pgm_ndx,
    SOC_SAND_IN  uint32                         ce_ndx,
    SOC_SAND_IN  uint8                          is_second_lookup,
    SOC_SAND_OUT SOC_PPC_FP_CE_INFO             *info
  )
{
    uint32
        ce_group_ndx = arad_pmf_low_level_ce_is_second_group(unit, stage, ce_ndx),
        line_ndx,
        fld_val,
        ce_instr_encoding = 0,
        nof_bits_in_instr_encoded_lsb,
        offset_in_instr_encoded_msb,
        two_s_comp_msb,
        resolution_ce_in_bits,
        max_nof_bits_in_ce,
        ce_offset,
        key_idx,
        res = SOC_SAND_OK,
        tbl_data[ARAD_PMF_CE_TABLE_LENGTH_IN_UINT32S];
    uint8
        is_ce_in_msb;
    soc_mem_t
        ce_table;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    is_ce_in_msb = ce_ndx >= ARAD_PMF_LOW_LEVEL_NOF_CE_IN_PROG_LSB;

    res = arad_pmf_ce_entry_verify(unit, stage, pmf_pgm_ndx, 0, ce_ndx, is_ce_in_msb, is_second_lookup);
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

    sal_memset(tbl_data, 0x0, sizeof(tbl_data));

    
    ce_table =  arad_pmf_ce_table_get(unit, stage, is_ce_in_msb, is_second_lookup, FALSE , ce_ndx);
    line_ndx = arad_pmf_ce_line_get(unit, stage, is_ce_in_msb, pmf_pgm_ndx, ce_ndx);
    res = soc_mem_read(unit, ce_table, MEM_BLOCK_ANY, line_ndx, tbl_data);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

        
     
     
    for (key_idx = 0; key_idx < ARAD_PMF_LOW_LEVEL_NOF_KEYS; key_idx++) {
        fld_val = soc_mem_field32_get(unit, ce_table, tbl_data, arad_pmf_ce_valid_fld_get(unit, stage, ce_ndx, key_idx));
        fld_val = SOC_SAND_GET_BIT(fld_val, arad_pmf_ce_valid_fld_bit_location_get(unit, stage, ce_group_ndx, ce_ndx));
        if (fld_val == TRUE) {
            info->keys_valid |= (1 << key_idx);
        }
    }

    
    info->is_32b_ce = arad_pmf_low_level_ce_is_32b_ce(unit, stage, ce_ndx);
    nof_bits_in_instr_encoded_lsb = info->is_32b_ce ? ARAD_PMF_CE_INSTR_ENCODING_NOF_BITS_LSB_32 : ARAD_PMF_CE_INSTR_ENCODING_NOF_BITS_LSB_16;
    ce_instr_encoding = soc_mem_field32_get(unit, ce_table, tbl_data, arad_pmf_ce_instruction_fld_get(unit, stage, ce_ndx));
    fld_val = SOC_SAND_GET_BITS_RANGE(ce_instr_encoding, ARAD_PMF_CE_INSTR_ENCODING_NOF_BITS_MSB, nof_bits_in_instr_encoded_lsb);
    info->nof_bits = fld_val + 1;

    offset_in_instr_encoded_msb = info->is_32b_ce ? ARAD_PMF_CE_INSTR_ENCODING_OFFSET_MSB_32 : ARAD_PMF_CE_INSTR_ENCODING_OFFSET_MSB_16;
    ce_offset = SOC_SAND_GET_BITS_RANGE(ce_instr_encoding, offset_in_instr_encoded_msb, ARAD_PMF_CE_INSTR_ENCODING_OFFSET_LSB);

    
    fld_val = SOC_SAND_GET_BITS_RANGE(ce_instr_encoding, ARAD_PMF_CE_INSTR_ENCODING_IS_HEADER_MSB, ARAD_PMF_CE_INSTR_ENCODING_IS_HEADER_LSB);
    info->is_header = (fld_val == ARAD_PMF_CE_INSTR_ENCODING_VALUE_IS_HEADER);

    fld_val = SOC_SAND_GET_BITS_RANGE(ce_instr_encoding, ARAD_PMF_CE_INSTR_ENCODING_SUB_HEADER_MSB, ARAD_PMF_CE_INSTR_ENCODING_SUB_HEADER_LSB);
    if (info->is_header == TRUE) {
        info->base = fld_val;

        max_nof_bits_in_ce = info->is_32b_ce ? ARAD_PMF_CE_INSTR_RESOLUTION_32B_NOF_BITS : ARAD_PMF_CE_INSTR_RESOLUTION_16B_NOF_BITS;
        resolution_ce_in_bits = info->is_32b_ce ? ARAD_PMF_CE_INSTR_RESOLUTION_32B_BYTES : ARAD_PMF_CE_INSTR_RESOLUTION_16B_NIBBLES;
        two_s_comp_msb = info->is_32b_ce ? ARAD_PMF_CE_INSTR_2S_COMP_VALUE_32B_BYTES_BIT_NDX : ARAD_PMF_CE_INSTR_2S_COMP_VALUE_16B_NIBBLES_BIT_NDX;

        
        if (SOC_SAND_GET_BIT(ce_offset, two_s_comp_msb) == 0x0) {
            ce_offset = ce_offset * resolution_ce_in_bits;
        } else {
            
            ce_offset = (SOC_SAND_GET_BITS_RANGE(ce_offset, two_s_comp_msb - 1, 0) - (1 << two_s_comp_msb)) * resolution_ce_in_bits;
        }

        
        ce_offset -= (info->nof_bits - max_nof_bits_in_ce);
    } else {
        info->base = SOC_SAND_GET_BIT(fld_val, ARAD_PMF_CE_HW_BUFFER_SUB_HEADER_BIT);

        res = arad_pmf_ce_internal_field_offset_qual_find(
                unit,
                stage,
                SOC_IS_JERICHO(unit) ? info->base : is_ce_in_msb,
                info->nof_bits,
                ce_offset,
                info->is_32b_ce,
                BCM_FIELD_ENTRY_INVALID,
                &info->qual_found,
                &info->qual_type,
                &info->qual_lsb,
                &info->qual_nof_bits
              );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
    }

    info->ce_offset = ce_offset;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_ce_for_key_get_unsafe()", pmf_pgm_ndx, ce_ndx);
}


uint32
  arad_pmf_ce_packet_header_entry_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  uint8                            is_ce_in_msb,
    SOC_SAND_IN  uint8                            is_second_lookup,
    SOC_SAND_IN  ARAD_PMF_CE_PACKET_HEADER_INFO      *info
  )
{
  uint32
      ce_group_ndx = arad_pmf_low_level_ce_is_second_group(unit, stage, ce_ndx),
      table_line,
      line_ndx,
    fld_val,
      ce_instr_encoding = 0,
      nof_bits_in_instr_encoded_lsb,
      offset_in_instr_encoded_msb,
      resolution_ce_in_bits,
      max_nof_bits_in_ce,
      two_s_comp_msb,
    res = SOC_SAND_OK,
    tbl_data[ARAD_PMF_CE_TABLE_LENGTH_IN_UINT32S];
  uint8
    is_32b_ce,
      found;
  int32
    offset;
  soc_mem_t
      ce_table;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PMF_CE_PACKET_HEADER_ENTRY_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(tbl_data, 0x0, ARAD_PMF_CE_TABLE_LENGTH_IN_UINT32S * sizeof(uint32));


 LOG_DEBUG(BSL_LS_SOC_FP,
           (BSL_META_U(unit,
                       "     "
                       "CE: Header set, "
                       "Stage: %s, "
                       "CE-ID: %d, "
                       "PMF-Program: %d, "
                       "Key: %d, "
                       "2nd-lookup: %d, "
                       "Is-MSB: %d\n\r"),
            SOC_PPC_FP_DATABASE_STAGE_to_string(stage), ce_ndx, pmf_pgm_ndx, pmf_key, is_second_lookup, is_ce_in_msb));

 LOG_DEBUG(BSL_LS_SOC_FP,
           (BSL_META_U(unit,
                       "     "
                       "CE info:"
                       "sub_header %s, "
                       "offset: %d, "
                       "nof_bits: %d\n\r"), ARAD_PMF_CE_SUB_HEADER_to_string(info->sub_header), info->offset, info->nof_bits));

  res = arad_pmf_ce_entry_verify( unit,
                                  stage,
                                  pmf_pgm_ndx,
                                  pmf_key,
                                  ce_ndx,
                                  is_ce_in_msb,
                                  is_second_lookup );

  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = ARAD_PMF_CE_PACKET_HEADER_INFO_verify(unit, stage, info, ce_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  
  ce_table =  arad_pmf_ce_table_get(unit, stage, is_ce_in_msb, is_second_lookup, FALSE , ce_ndx);
  line_ndx = arad_pmf_ce_line_get(unit, stage, is_ce_in_msb, pmf_pgm_ndx, ce_ndx); 

  res = soc_mem_read(
          unit,
          ce_table,
          MEM_BLOCK_ANY,
          line_ndx,
          tbl_data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  
  
  fld_val = soc_mem_field32_get(unit, ce_table, tbl_data, arad_pmf_ce_valid_fld_get(unit, stage, ce_ndx, pmf_key));
  SOC_SAND_SET_BIT(fld_val, 0x1, arad_pmf_ce_valid_fld_bit_location_get(unit, stage, ce_group_ndx, ce_ndx));



  soc_mem_field32_set(unit, ce_table, tbl_data, arad_pmf_ce_valid_fld_get(unit, stage, ce_ndx, pmf_key), fld_val);


  
  fld_val = 0;
  found = FALSE;
  for (table_line = 0; (table_line < ARAD_NOF_PMF_CE_SUB_HEADERS) && (!found); ++table_line) {
      if (info->sub_header == Arad_pmf_ce_sub_header_type_encoding[table_line][0]) {
          fld_val = Arad_pmf_ce_sub_header_type_encoding[table_line][1];
          found = TRUE;
          break;
      }
  }
  if (!found) {
      LOG_ERROR(BSL_LS_SOC_FP,
                (BSL_META_U(unit,
                            "Unit %d Sub header %d : No encoding to sub-header.\n\r"), unit, info->sub_header));
      SOC_SAND_SET_ERROR_CODE(ARAD_PMF_LOW_LEVEL_SUB_HEADER_OUT_OF_RANGE_ERR, 20, exit);
  }

  ce_instr_encoding |= SOC_SAND_SET_BITS_RANGE(fld_val, ARAD_PMF_CE_INSTR_ENCODING_SUB_HEADER_MSB, ARAD_PMF_CE_INSTR_ENCODING_SUB_HEADER_LSB);

  
  ce_instr_encoding |= SOC_SAND_SET_BITS_RANGE(ARAD_PMF_CE_INSTR_ENCODING_VALUE_IS_HEADER, ARAD_PMF_CE_INSTR_ENCODING_IS_HEADER_MSB, ARAD_PMF_CE_INSTR_ENCODING_IS_HEADER_LSB);


  
  fld_val = 0;
  is_32b_ce = arad_pmf_low_level_ce_is_32b_ce(unit, stage, ce_ndx);
  max_nof_bits_in_ce = (is_32b_ce)? ARAD_PMF_CE_INSTR_RESOLUTION_32B_NOF_BITS:ARAD_PMF_CE_INSTR_RESOLUTION_16B_NOF_BITS;
  resolution_ce_in_bits = (is_32b_ce)? ARAD_PMF_CE_INSTR_RESOLUTION_32B_BYTES:ARAD_PMF_CE_INSTR_RESOLUTION_16B_NIBBLES;
  offset = info->offset + info->nof_bits - max_nof_bits_in_ce;
  if (offset >= 0)
  {
    fld_val = (offset / resolution_ce_in_bits);
  }
  else
  {
      two_s_comp_msb = (is_32b_ce)? ARAD_PMF_CE_INSTR_2S_COMP_VALUE_32B_BYTES_BIT_NDX:ARAD_PMF_CE_INSTR_2S_COMP_VALUE_16B_NIBBLES_BIT_NDX;
    SOC_SAND_SET_BIT(fld_val, 0x1, two_s_comp_msb);
    fld_val += (1 << two_s_comp_msb) - ((-offset) / resolution_ce_in_bits);
  }
  offset_in_instr_encoded_msb = (is_32b_ce)? ARAD_PMF_CE_INSTR_ENCODING_OFFSET_MSB_32 : ARAD_PMF_CE_INSTR_ENCODING_OFFSET_MSB_16;
  ce_instr_encoding |= SOC_SAND_SET_BITS_RANGE(fld_val, offset_in_instr_encoded_msb, ARAD_PMF_CE_INSTR_ENCODING_OFFSET_LSB);

  
  fld_val = info->nof_bits - 1; 
  nof_bits_in_instr_encoded_lsb = (is_32b_ce)? ARAD_PMF_CE_INSTR_ENCODING_NOF_BITS_LSB_32 : ARAD_PMF_CE_INSTR_ENCODING_NOF_BITS_LSB_16;
  ce_instr_encoding |= SOC_SAND_SET_BITS_RANGE(fld_val, ARAD_PMF_CE_INSTR_ENCODING_NOF_BITS_MSB, nof_bits_in_instr_encoded_lsb);
  soc_mem_field32_set(unit, ce_table, tbl_data, arad_pmf_ce_instruction_fld_get(unit, stage, ce_ndx), ce_instr_encoding);


  res = soc_mem_write(
          unit,
          ce_table,
          MEM_BLOCK_ANY,
          line_ndx,
          tbl_data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_ce_packet_header_entry_set_unsafe()", pmf_pgm_ndx, pmf_key);
}



uint32
  arad_pmf_ce_packet_header_entry_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  uint8                            is_ce_in_msb,
    SOC_SAND_IN  uint8                            is_second_lookup,
    SOC_SAND_OUT ARAD_PMF_CE_PACKET_HEADER_INFO      *info
  )
{
    uint32
        ce_group_ndx = arad_pmf_low_level_ce_is_second_group(unit, stage, ce_ndx),
        table_line,
        line_ndx,
      fld_val,
        offset,
        ce_instr_encoding = 0,
        nof_bits_in_instr_encoded_lsb,
        offset_in_instr_encoded_msb,
        two_s_comp_msb,
        resolution_ce_in_bits,
        max_nof_bits_in_ce,
      res = SOC_SAND_OK,
      tbl_data[ARAD_PMF_CE_TABLE_LENGTH_IN_UINT32S];
    uint8
      is_32b_ce,
        found;
    soc_mem_t
        ce_table;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PMF_CE_PACKET_HEADER_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_PMF_CE_PACKET_HEADER_INFO_clear(info);

  res = arad_pmf_ce_entry_verify( unit,
                                  stage,
                                  pmf_pgm_ndx,
                                  pmf_key,
                                  ce_ndx,
                                  is_ce_in_msb,
                                  is_second_lookup );

  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  sal_memset(tbl_data, 0x0, ARAD_PMF_CE_TABLE_LENGTH_IN_UINT32S * sizeof(uint32));

  
  ce_table =  arad_pmf_ce_table_get(unit, stage, is_ce_in_msb, is_second_lookup, FALSE , ce_ndx);
  line_ndx = arad_pmf_ce_line_get(unit, stage, is_ce_in_msb, pmf_pgm_ndx, ce_ndx);
  res = soc_mem_read(
          unit,
          ce_table,
          MEM_BLOCK_ANY,
          line_ndx,
          tbl_data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);


  
  

  fld_val = soc_mem_field32_get(unit, ce_table, tbl_data, arad_pmf_ce_valid_fld_get(unit, stage, ce_ndx, pmf_key));
  fld_val = SOC_SAND_GET_BIT(fld_val, arad_pmf_ce_valid_fld_bit_location_get(unit, stage, ce_group_ndx, ce_ndx));
  if (fld_val == FALSE) {
      ARAD_DO_NOTHING_AND_EXIT;
  }

  
  is_32b_ce = arad_pmf_low_level_ce_is_32b_ce(unit, stage, ce_ndx);
  nof_bits_in_instr_encoded_lsb = (is_32b_ce)? ARAD_PMF_CE_INSTR_ENCODING_NOF_BITS_LSB_32 : ARAD_PMF_CE_INSTR_ENCODING_NOF_BITS_LSB_16;
  ce_instr_encoding = soc_mem_field32_get(unit, ce_table, tbl_data, arad_pmf_ce_instruction_fld_get(unit, stage, ce_ndx));
  fld_val = SOC_SAND_GET_BITS_RANGE(ce_instr_encoding, ARAD_PMF_CE_INSTR_ENCODING_NOF_BITS_MSB, nof_bits_in_instr_encoded_lsb);
  info->nof_bits = fld_val + 1;

  
  fld_val = SOC_SAND_GET_BITS_RANGE(ce_instr_encoding, ARAD_PMF_CE_INSTR_ENCODING_IS_HEADER_MSB, ARAD_PMF_CE_INSTR_ENCODING_IS_HEADER_LSB);
  if (fld_val != ARAD_PMF_CE_INSTR_ENCODING_VALUE_IS_HEADER) {
      ARAD_DO_NOTHING_AND_EXIT;
  }

  
  fld_val = SOC_SAND_GET_BITS_RANGE(ce_instr_encoding, ARAD_PMF_CE_INSTR_ENCODING_SUB_HEADER_MSB, ARAD_PMF_CE_INSTR_ENCODING_SUB_HEADER_LSB);
  found = FALSE;
  for (table_line = 0; (table_line < ARAD_NOF_PMF_CE_SUB_HEADERS) && (!found); ++table_line) {
      if (fld_val == Arad_pmf_ce_sub_header_type_encoding[table_line][1]) {
          info->sub_header = Arad_pmf_ce_sub_header_type_encoding[table_line][0];
          found = TRUE;
          break;
      }
  }
  if (!found) {
     LOG_ERROR(BSL_LS_SOC_FP,
               (BSL_META_U(unit,
                           "Unit %d Sub header code %d - The sub header is not supported.\n\r"), unit, fld_val));
      SOC_SAND_SET_ERROR_CODE(ARAD_PMF_LOW_LEVEL_SUB_HEADER_OUT_OF_RANGE_ERR, 20, exit);
  }

  
  is_32b_ce = arad_pmf_low_level_ce_is_32b_ce(unit, stage, ce_ndx);
  max_nof_bits_in_ce = (is_32b_ce)? ARAD_PMF_CE_INSTR_RESOLUTION_32B_NOF_BITS:ARAD_PMF_CE_INSTR_RESOLUTION_16B_NOF_BITS;
  resolution_ce_in_bits = (is_32b_ce)? ARAD_PMF_CE_INSTR_RESOLUTION_32B_BYTES:ARAD_PMF_CE_INSTR_RESOLUTION_16B_NIBBLES;
  two_s_comp_msb = (is_32b_ce)? ARAD_PMF_CE_INSTR_2S_COMP_VALUE_32B_BYTES_BIT_NDX:ARAD_PMF_CE_INSTR_2S_COMP_VALUE_16B_NIBBLES_BIT_NDX;
  offset_in_instr_encoded_msb = (is_32b_ce)? ARAD_PMF_CE_INSTR_ENCODING_OFFSET_MSB_32 : ARAD_PMF_CE_INSTR_ENCODING_OFFSET_MSB_16;
  offset = SOC_SAND_GET_BITS_RANGE(ce_instr_encoding, offset_in_instr_encoded_msb, ARAD_PMF_CE_INSTR_ENCODING_OFFSET_LSB);

    
    if (SOC_SAND_GET_BIT(offset, two_s_comp_msb) == 0x0)
    {
        info->offset = offset * resolution_ce_in_bits;
    }
    else
    {
      
        info->offset = (SOC_SAND_GET_BITS_RANGE(offset, two_s_comp_msb-1, 0)- (1<< two_s_comp_msb))
                          * resolution_ce_in_bits;
    }
    
    info->offset = info->offset - (info->nof_bits - max_nof_bits_in_ce);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_ce_packet_header_entry_get_unsafe()", pmf_pgm_ndx, pmf_key);
}

uint32
  arad_pmf_ce_for_key_dump(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  uint8                            is_ce_in_msb,
    SOC_SAND_IN  uint8                            is_second_lookup
  )
{
    uint32
        ce_group_ndx = arad_pmf_low_level_ce_is_second_group(unit, stage, ce_ndx),
        table_line,
        line_ndx,
        fld_val,
        offset,
        ce_instr_encoding = 0,
        nof_bits_in_instr_encoded_lsb,
        offset_in_instr_encoded_msb,
        two_s_comp_msb,
        resolution_ce_in_bits,
        max_nof_bits_in_ce,
        res = SOC_SAND_OK,
        tbl_data[ARAD_PMF_CE_TABLE_LENGTH_IN_UINT32S];

    uint8       is_32b_ce, found;
    soc_mem_t   ce_table;

    ARAD_PMF_CE_PACKET_HEADER_INFO  info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PMF_CE_PACKET_HEADER_ENTRY_GET_UNSAFE);

    ARAD_PMF_CE_PACKET_HEADER_INFO_clear(&info);

    res = arad_pmf_ce_entry_verify( unit, stage, pmf_pgm_ndx, pmf_key, ce_ndx, is_ce_in_msb, is_second_lookup );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

    sal_memset(tbl_data, 0x0, ARAD_PMF_CE_TABLE_LENGTH_IN_UINT32S * sizeof(uint32));

    
    ce_table =  arad_pmf_ce_table_get(unit, stage, is_ce_in_msb, is_second_lookup, FALSE , ce_ndx);
    line_ndx = arad_pmf_ce_line_get(unit, stage, is_ce_in_msb, pmf_pgm_ndx, ce_ndx);
    res = soc_mem_read(unit, ce_table, MEM_BLOCK_ANY, line_ndx, tbl_data);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    
    fld_val = soc_mem_field32_get(unit, ce_table, tbl_data, arad_pmf_ce_valid_fld_get(unit, stage, ce_ndx, pmf_key));
    fld_val = SOC_SAND_GET_BIT(fld_val, arad_pmf_ce_valid_fld_bit_location_get(unit, stage, ce_group_ndx, ce_ndx));
    if (fld_val == FALSE) {
      
      ARAD_DO_NOTHING_AND_EXIT;
    }  

    
    is_32b_ce = arad_pmf_low_level_ce_is_32b_ce(unit, stage, ce_ndx);
    nof_bits_in_instr_encoded_lsb = (is_32b_ce)? ARAD_PMF_CE_INSTR_ENCODING_NOF_BITS_LSB_32 : ARAD_PMF_CE_INSTR_ENCODING_NOF_BITS_LSB_16;
    ce_instr_encoding = soc_mem_field32_get(unit, ce_table, tbl_data, arad_pmf_ce_instruction_fld_get(unit, stage, ce_ndx));
    fld_val = SOC_SAND_GET_BITS_RANGE(ce_instr_encoding, ARAD_PMF_CE_INSTR_ENCODING_NOF_BITS_MSB, nof_bits_in_instr_encoded_lsb);
    info.nof_bits = fld_val + 1;

    
    fld_val = SOC_SAND_GET_BITS_RANGE(ce_instr_encoding, ARAD_PMF_CE_INSTR_ENCODING_IS_HEADER_MSB, ARAD_PMF_CE_INSTR_ENCODING_IS_HEADER_LSB);
    if (fld_val != ARAD_PMF_CE_INSTR_ENCODING_VALUE_IS_HEADER) {
      {
          uint32                       qual_lsb = 0;
          uint32                       nof_bits = 0;
          SOC_PPC_FP_QUAL_TYPE     irpp_field = SOC_PPC_FP_QUAL_IRPP_INVALID;
          uint32                       offset_in_instr_encoded_msb;
          uint32                       ce_offset;
          found = 0;

          offset_in_instr_encoded_msb = (is_32b_ce)? ARAD_PMF_CE_INSTR_ENCODING_OFFSET_MSB_32 : ARAD_PMF_CE_INSTR_ENCODING_OFFSET_MSB_16;
          ce_offset = SOC_SAND_GET_BITS_RANGE(ce_instr_encoding, offset_in_instr_encoded_msb, ARAD_PMF_CE_INSTR_ENCODING_OFFSET_LSB);
                    
          

          arad_pmf_ce_internal_field_offset_qual_find(
                    unit,
                    stage,
                    is_ce_in_msb,
                    info.nof_bits,
                    ce_offset,
                    is_32b_ce,
                    BCM_FIELD_ENTRY_INVALID,
                    &found,
                    &irpp_field,
                    &qual_lsb,
                    &nof_bits
                );
          
          if (found) {
              LOG_CLI((BSL_META("\tCE %d is used, nof bits=%02d type: Internal buffer, field=%s, qual_lsb=%d \n"), (ce_ndx+(is_ce_in_msb*(ARAD_PMF_LOW_LEVEL_CE_NDX_MAX-1)) ),  info.nof_bits, SOC_PPC_FP_QUAL_TYPE_to_string(irpp_field), qual_lsb));
          }else{
              LOG_CLI((BSL_META("\tCE %d is used, nof bits=%02d type: Internal buffer, no more info found\n"), (ce_ndx+(is_ce_in_msb*(ARAD_PMF_LOW_LEVEL_CE_NDX_MAX-1)) ),  info.nof_bits));
          }
      }
    }else{
      fld_val = SOC_SAND_GET_BITS_RANGE(ce_instr_encoding, ARAD_PMF_CE_INSTR_ENCODING_SUB_HEADER_MSB, ARAD_PMF_CE_INSTR_ENCODING_SUB_HEADER_LSB);
      found = FALSE;
      for (table_line = 0; (table_line < ARAD_NOF_PMF_CE_SUB_HEADERS) && (!found); ++table_line) {
          if (fld_val == Arad_pmf_ce_sub_header_type_encoding[table_line][1]) {
              info.sub_header = Arad_pmf_ce_sub_header_type_encoding[table_line][0];
              found = TRUE;
              break;
          }
      }
      if (!found) {
         LOG_ERROR(BSL_LS_SOC_FP,(BSL_META_U(unit,"Unit %d Sub header code %d - The sub header is not supported.\n\r"), unit, fld_val));
          SOC_SAND_SET_ERROR_CODE(ARAD_PMF_LOW_LEVEL_SUB_HEADER_OUT_OF_RANGE_ERR, 20, exit);
      } else {
          max_nof_bits_in_ce = (is_32b_ce) ? ARAD_PMF_CE_INSTR_RESOLUTION_32B_NOF_BITS : ARAD_PMF_CE_INSTR_RESOLUTION_16B_NOF_BITS;
          resolution_ce_in_bits = (is_32b_ce) ? ARAD_PMF_CE_INSTR_RESOLUTION_32B_BYTES : ARAD_PMF_CE_INSTR_RESOLUTION_16B_NIBBLES;
          two_s_comp_msb = (is_32b_ce) ? ARAD_PMF_CE_INSTR_2S_COMP_VALUE_32B_BYTES_BIT_NDX : ARAD_PMF_CE_INSTR_2S_COMP_VALUE_16B_NIBBLES_BIT_NDX;
          offset_in_instr_encoded_msb = (is_32b_ce) ? ARAD_PMF_CE_INSTR_ENCODING_OFFSET_MSB_32 : ARAD_PMF_CE_INSTR_ENCODING_OFFSET_MSB_16;
          offset = SOC_SAND_GET_BITS_RANGE(ce_instr_encoding, offset_in_instr_encoded_msb, ARAD_PMF_CE_INSTR_ENCODING_OFFSET_LSB);

          
          if (SOC_SAND_GET_BIT(offset, two_s_comp_msb) == 0x0) {
              info.offset = offset * resolution_ce_in_bits;
          } else {
              
              info.offset = (SOC_SAND_GET_BITS_RANGE(offset, two_s_comp_msb - 1, 0) - (1 << two_s_comp_msb)) * resolution_ce_in_bits;
          }
          
          info.offset = info.offset - (info.nof_bits - max_nof_bits_in_ce);

          LOG_CLI((BSL_META("\tCE %d is used, nof bits=%02d type: Header (%s) offset in header=%d\n"), (ce_ndx + (is_ce_in_msb * ARAD_PMF_LOW_LEVEL_CE_NDX_MAX - 1)),  info.nof_bits, ARAD_PMF_CE_SUB_HEADER_to_string(info.sub_header), info.offset));
      }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_ce_packet_header_entry_get_unsafe()", pmf_pgm_ndx, pmf_key);
}







uint32
  arad_pmf_ce_internal_info_entry_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  uint8                            is_ce_in_msb,
    SOC_SAND_IN  uint8                            is_second_lookup,
    SOC_SAND_IN  uint8                            is_update_key,
    SOC_SAND_IN  uint32                             qual_lsb,
    SOC_SAND_IN  uint32                             lost_bits, 
    SOC_SAND_IN  uint32                             nof_bits,
    SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE            irpp_field
  )
{
    uint32
        ce_group_ndx = arad_pmf_low_level_ce_is_second_group(unit, stage, ce_ndx),
        line_ndx,
      fld_val,
        ce_instr_encoding = 0,
        nof_bits_in_instr_encoded_lsb,
        offset_in_instr_encoded_msb,
        nof_bits_fields,
        hw_buffer_jericho,
      res = SOC_SAND_OK,
        ce_offset,
      tbl_data[ARAD_PMF_CE_TABLE_LENGTH_IN_UINT32S],
      nof_bits_real,
      ce_nof_bits_max;
    uint8
      is_32b_ce,
        found;
    soc_mem_t
        ce_table;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PMF_CE_IRPP_INFO_ENTRY_SET_UNSAFE);



 LOG_DEBUG(BSL_LS_SOC_FP,
           (BSL_META_U(unit,
                       "     "
                       "CE: internal field set, "
                       "CE-ID: %d, "
                       "Stage: %s, "
                       "PMF-Program: %d, "
                       "Key: %d, "
                       "2nd-lookup: %d, "
                       "Is-MSB: %d\n\r"), 
            ce_ndx, SOC_PPC_FP_DATABASE_STAGE_to_string(stage), pmf_pgm_ndx, pmf_key, is_second_lookup, is_ce_in_msb));

 LOG_DEBUG(BSL_LS_SOC_FP,
           (BSL_META_U(unit,
                       "     "
                       "Internal Field: %s, "
                       "Qual_lsb: %d, "
                       "lost_bits: %d, "
                       "Nof_bits: %d\n\r"),
            SOC_PPC_FP_QUAL_TYPE_to_string(irpp_field), qual_lsb, lost_bits, nof_bits));
 LOG_DEBUG(BSL_LS_SOC_FP,
           (BSL_META_U(unit,
                       "\n\r")));

  res = arad_pmf_ce_entry_verify( unit,
                                  stage,
                                  pmf_pgm_ndx,
                                  pmf_key,
                                  ce_ndx,
                                  is_ce_in_msb,
                                  is_second_lookup );

  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  is_32b_ce = arad_pmf_low_level_ce_is_32b_ce(unit, stage, ce_ndx);
  ce_nof_bits_max = (is_32b_ce)? ARAD_PMF_LOW_LEVEL_NOF_BITS_CE_32B_MAX : ARAD_PMF_LOW_LEVEL_NOF_BITS_CE_16B_MAX;
  
  res = arad_pmf_ce_internal_field_offset_compute(
            unit,
            irpp_field,
            stage,
            is_ce_in_msb,
            nof_bits,
            qual_lsb,
            lost_bits,
            is_32b_ce,
            &found,
            &ce_offset,
            &nof_bits_real,
            &hw_buffer_jericho
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  if (!found) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PMF_CE_INTERNAL_FIELD_NOT_FOUND_ERR, 32, exit);
  }

  SOC_SAND_ERR_IF_ABOVE_MAX(nof_bits_real, ce_nof_bits_max, ARAD_PMF_LOW_LEVEL_NOF_BITS_OUT_OF_RANGE_ERR, 35, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(irpp_field, SOC_PPC_NOF_FP_QUAL_TYPES, ARAD_PMF_LOW_LEVEL_IRPP_FIELD_OUT_OF_RANGE_ERR, 40, exit);

  
  ce_table =  arad_pmf_ce_table_get(unit, stage, is_ce_in_msb, is_second_lookup, is_update_key, ce_ndx);
  line_ndx = arad_pmf_ce_line_get(unit, stage, is_ce_in_msb, pmf_pgm_ndx, ce_ndx);
  res = soc_mem_read(
          unit,
          ce_table,
          MEM_BLOCK_ANY,
          line_ndx,
          tbl_data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  
  
  if(SOC_IS_JERICHO_PLUS(unit) && is_update_key)
  {
      
      fld_val = (tbl_data[4] >> (8 * pmf_key)) & 0xffffffff;
  }
  else
  {
      fld_val = soc_mem_field32_get(unit, ce_table, tbl_data, arad_pmf_ce_valid_fld_get(unit, stage, ce_ndx, pmf_key));
  }

  if (irpp_field == SOC_PPC_FP_QUAL_IRPP_INVALID) {
      SOC_SAND_SET_BIT(fld_val, 0x0, arad_pmf_ce_valid_fld_bit_location_get(unit, stage, ce_group_ndx, ce_ndx));
  }
  else {
      SOC_SAND_SET_BIT(fld_val, 0x1, arad_pmf_ce_valid_fld_bit_location_get(unit, stage, ce_group_ndx, ce_ndx));
  }

  if(SOC_IS_JERICHO_PLUS(unit) && is_update_key)
  {
      
      tbl_data[4] |= (fld_val << (8 * pmf_key)) & 0xffffffff;
  }
  else
  {
      soc_mem_field32_set(unit, ce_table, tbl_data, arad_pmf_ce_valid_fld_get(unit, stage, ce_ndx, pmf_key), fld_val);
  }

  if (irpp_field == SOC_PPC_FP_QUAL_IRPP_INVALID) {
      LOG_ERROR(BSL_LS_SOC_FP,
                (BSL_META_U(unit,
                            "Unit %d The IRPP field is invalid.\n\r"), unit));
      goto exit_invalid;
  }


  
  ce_instr_encoding |= SOC_SAND_SET_BITS_RANGE(ARAD_PMF_CE_INSTR_ENCODING_VALUE_IS_INTERNAL_FIELD, 
                                           ARAD_PMF_CE_INSTR_ENCODING_IS_HEADER_MSB, ARAD_PMF_CE_INSTR_ENCODING_IS_HEADER_LSB);

  is_32b_ce = arad_pmf_low_level_ce_is_32b_ce(unit, stage, ce_ndx);

  res = arad_pmf_ce_internal_field_offset_compute(
            unit,
            irpp_field,
            stage,
            is_ce_in_msb,
            nof_bits,
            qual_lsb,
            lost_bits,
            is_32b_ce,
            &found,
            &ce_offset,
            &nof_bits_fields,
            &hw_buffer_jericho
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  if (!found) {
      LOG_ERROR(BSL_LS_SOC_FP,
                (BSL_META_U(unit,
                            "Unit %d, Stage %s, IRPP field %s, in msb %s, Number of bits %d, Qual lsb %d, Lost bits %d, is 32-bits Ce %s.\n\r"),
                            unit, SOC_PPC_FP_DATABASE_STAGE_to_string(stage), SOC_PPC_FP_QUAL_TYPE_to_string(irpp_field), is_ce_in_msb ? "TRUE" : "FALSE",
                 nof_bits, qual_lsb, lost_bits, is_32b_ce ? "TRUE" : "FALSE"));
      SOC_SAND_SET_ERROR_CODE(ARAD_PMF_CE_INTERNAL_FIELD_NOT_FOUND_ERR, 32, exit);
  }

  
  fld_val = SOC_IS_JERICHO(unit)? (hw_buffer_jericho << ARAD_PMF_CE_HW_BUFFER_SUB_HEADER_BIT): 0;
  ce_instr_encoding |= SOC_SAND_SET_BITS_RANGE(fld_val, ARAD_PMF_CE_INSTR_ENCODING_SUB_HEADER_MSB, ARAD_PMF_CE_INSTR_ENCODING_SUB_HEADER_LSB);

  fld_val = nof_bits_fields - 1;
  nof_bits_in_instr_encoded_lsb = (is_32b_ce)? ARAD_PMF_CE_INSTR_ENCODING_NOF_BITS_LSB_32 : ARAD_PMF_CE_INSTR_ENCODING_NOF_BITS_LSB_16;
  ce_instr_encoding |= SOC_SAND_SET_BITS_RANGE(fld_val, ARAD_PMF_CE_INSTR_ENCODING_NOF_BITS_MSB, nof_bits_in_instr_encoded_lsb);
  offset_in_instr_encoded_msb = (is_32b_ce)? ARAD_PMF_CE_INSTR_ENCODING_OFFSET_MSB_32 : ARAD_PMF_CE_INSTR_ENCODING_OFFSET_MSB_16;
  ce_instr_encoding |= SOC_SAND_SET_BITS_RANGE(ce_offset, offset_in_instr_encoded_msb, ARAD_PMF_CE_INSTR_ENCODING_OFFSET_LSB);
  if(SOC_IS_JERICHO_PLUS(unit) && is_update_key)
  {
      

      
      tbl_data[ce_ndx/2] &= ~(0xffff << (16 * (ce_ndx % 2)));
      tbl_data[ce_ndx/2] |= (ce_instr_encoding & 0xffff) << (16 * (ce_ndx % 2));
  }
  else
  {
      soc_mem_field32_set(unit, ce_table, tbl_data, arad_pmf_ce_instruction_fld_get(unit, stage, ce_ndx), ce_instr_encoding);
  }
  LOG_DEBUG(BSL_LS_SOC_FP,
            (BSL_META_U(unit,
                        "Stage %s, IRPP field %s, %s, Number of bits %d, nof_bits_fields %d Qual lsb %d, Lost bits %d, is 32-bits Ce %s. ce_offset %d, ce_instr_encoding %X\n\r"),
                        SOC_PPC_FP_DATABASE_STAGE_to_string(stage), SOC_PPC_FP_QUAL_TYPE_to_string(irpp_field), is_ce_in_msb ? "MSB" : "LSB",
             nof_bits, nof_bits_fields, qual_lsb, lost_bits, is_32b_ce ? "TRUE" : "FALSE", ce_offset, ce_instr_encoding));

 exit_invalid:
  res = soc_mem_write(
          unit,
          ce_table,
          MEM_BLOCK_ANY,
          line_ndx,
          tbl_data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_ce_internal_info_entry_set_unsafe()", pmf_pgm_ndx, pmf_key);
}



uint32
  arad_pmf_ce_internal_info_entry_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  uint8                            is_ce_in_msb,
    SOC_SAND_IN  uint8                            is_second_lookup,
    SOC_SAND_IN  SOC_PPC_FP_QUAL_TYPE         irpp_field_in,
    SOC_SAND_OUT uint32                             *qual_lsb,
    SOC_SAND_OUT uint32                             *nof_bits,
    SOC_SAND_OUT SOC_PPC_FP_QUAL_TYPE               *irpp_field
  )
{
    uint32
        ce_group_ndx = arad_pmf_low_level_ce_is_second_group(unit, stage, ce_ndx),
        line_ndx,
      fld_val,
        ce_instr_encoding = 0,
        nof_bits_lcl,
        nof_bits_in_instr_encoded_lsb,
        offset_in_instr_encoded_msb,
        ce_offset,
      res = SOC_SAND_OK,
      tbl_data[ARAD_PMF_CE_TABLE_LENGTH_IN_UINT32S];
    uint8
      is_32b_ce,
        found;
    soc_mem_t
        ce_table;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PMF_CE_IRPP_INFO_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(irpp_field);

  res = arad_pmf_ce_entry_verify( unit,
		                          stage,
                                  pmf_pgm_ndx,
                                  pmf_key,
                                  ce_ndx,
                                  is_ce_in_msb,
                                  is_second_lookup );

  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  sal_memset(tbl_data, 0x0, ARAD_PMF_CE_TABLE_LENGTH_IN_UINT32S * sizeof(uint32));

  
  ce_table =  arad_pmf_ce_table_get(unit, stage, is_ce_in_msb, is_second_lookup, FALSE , ce_ndx);
  line_ndx = arad_pmf_ce_line_get(unit, stage, is_ce_in_msb, pmf_pgm_ndx, ce_ndx);
  res = soc_mem_read(
          unit,
          ce_table,
          MEM_BLOCK_ANY,
          line_ndx,
          tbl_data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  
  *irpp_field = SOC_PPC_FP_QUAL_IRPP_INVALID;
  *nof_bits = 0;
  *qual_lsb = 0;


  
  
  fld_val = soc_mem_field32_get(unit, ce_table, tbl_data, arad_pmf_ce_valid_fld_get(unit, stage, ce_ndx, pmf_key));
  fld_val = SOC_SAND_GET_BIT(fld_val, arad_pmf_ce_valid_fld_bit_location_get(unit, stage, ce_group_ndx, ce_ndx));
  if (fld_val == FALSE) {
      ARAD_DO_NOTHING_AND_EXIT;
  }

  
  is_32b_ce = arad_pmf_low_level_ce_is_32b_ce(unit, stage, ce_ndx);
  nof_bits_in_instr_encoded_lsb = (is_32b_ce)? ARAD_PMF_CE_INSTR_ENCODING_NOF_BITS_LSB_32 : ARAD_PMF_CE_INSTR_ENCODING_NOF_BITS_LSB_16;
  ce_instr_encoding = soc_mem_field32_get(unit, ce_table, tbl_data, arad_pmf_ce_instruction_fld_get(unit, stage, ce_ndx));
  fld_val = SOC_SAND_GET_BITS_RANGE(ce_instr_encoding, ARAD_PMF_CE_INSTR_ENCODING_NOF_BITS_MSB, nof_bits_in_instr_encoded_lsb);
  nof_bits_lcl = fld_val + 1;
  offset_in_instr_encoded_msb = (is_32b_ce)? ARAD_PMF_CE_INSTR_ENCODING_OFFSET_MSB_32 : ARAD_PMF_CE_INSTR_ENCODING_OFFSET_MSB_16;
  ce_offset = SOC_SAND_GET_BITS_RANGE(ce_instr_encoding, offset_in_instr_encoded_msb, ARAD_PMF_CE_INSTR_ENCODING_OFFSET_LSB);

  
  fld_val = SOC_SAND_GET_BITS_RANGE(ce_instr_encoding, ARAD_PMF_CE_INSTR_ENCODING_IS_HEADER_MSB, ARAD_PMF_CE_INSTR_ENCODING_IS_HEADER_LSB);
  if (fld_val != ARAD_PMF_CE_INSTR_ENCODING_VALUE_IS_INTERNAL_FIELD) {
      ARAD_DO_NOTHING_AND_EXIT;
  }

  
  fld_val = SOC_SAND_GET_BITS_RANGE(ce_instr_encoding, ARAD_PMF_CE_INSTR_ENCODING_SUB_HEADER_MSB, ARAD_PMF_CE_INSTR_ENCODING_SUB_HEADER_LSB);
  if ( (!(SOC_IS_JERICHO(unit)) && fld_val != 0) || ((SOC_IS_JERICHO(unit)) && ((fld_val & 0x3 ) != 0 )) ) {
      ARAD_DO_NOTHING_AND_EXIT;
  }
 
  res = arad_pmf_ce_internal_field_offset_qual_find(
            unit,
            stage,
            is_ce_in_msb,
            nof_bits_lcl,
            ce_offset,
            is_32b_ce,
            irpp_field_in,
            &found,
            irpp_field,
            qual_lsb,
            nof_bits
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (!found) {
      LOG_ERROR(BSL_LS_SOC_FP,
                (BSL_META_U(unit,
                            "Unit %d, Stage %s, IRPP field %s, in msb %s, is 32-bits Ce %s.\n\r"),
                            unit, SOC_PPC_FP_DATABASE_STAGE_to_string(stage), SOC_PPC_FP_QUAL_TYPE_to_string(irpp_field_in), is_ce_in_msb ? "TRUE" : "FALSE",
                 is_32b_ce ? "TRUE" : "FALSE"));
      SOC_SAND_SET_ERROR_CODE(ARAD_PMF_CE_INTERNAL_FIELD_NOT_FOUND_ERR, 32, exit);
  }



exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_ce_internal_info_entry_get_unsafe()", pmf_pgm_ndx, pmf_key);
}


uint32
  arad_pmf_ce_nop_entry_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  uint8                            is_ce_in_msb,
    SOC_SAND_IN  uint8                            is_second_lookup,
    SOC_SAND_IN  uint8                            is_ce_not_valid
  )
{
    uint32
        ce_group_ndx = arad_pmf_low_level_ce_is_second_group(unit, stage, ce_ndx),
        line_ndx,
      fld_val,
      res = SOC_SAND_OK,
      tbl_data[ARAD_PMF_CE_TABLE_LENGTH_IN_UINT32S];
    soc_mem_t
        ce_table;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PMF_CE_NOP_ENTRY_SET_UNSAFE);

  LOG_DEBUG(BSL_LS_SOC_FP,
            (BSL_META_U(unit,
                        "     "
                        "CE: NOP set, "
                        "CE-ID: %d, "
                        "Stage: %s, "
                        "PMF-Program: %d, "
                        "Key: %d, "
                        "2nd-lookup: %d, "
                        "Is-MSB: %d\n\r"),
             ce_ndx, SOC_PPC_FP_DATABASE_STAGE_to_string(stage), pmf_pgm_ndx, pmf_key, is_second_lookup, is_ce_in_msb));

  res = arad_pmf_ce_entry_verify( unit,
                                  stage,
                                  pmf_pgm_ndx,
                                  pmf_key,
                                  ce_ndx,
                                  is_ce_in_msb,
                                  is_second_lookup );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  sal_memset(tbl_data, 0x0, ARAD_PMF_CE_TABLE_LENGTH_IN_UINT32S * sizeof(uint32));


  
  ce_table =  arad_pmf_ce_table_get(unit, stage, is_ce_in_msb, is_second_lookup, FALSE , ce_ndx);
  line_ndx = arad_pmf_ce_line_get(unit, stage, is_ce_in_msb, pmf_pgm_ndx, ce_ndx);
  res = soc_mem_read(
          unit,
          ce_table,
          MEM_BLOCK_ANY,
          line_ndx,
          tbl_data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  
  
  fld_val = soc_mem_field32_get(unit, ce_table, tbl_data, arad_pmf_ce_valid_fld_get(unit, stage, ce_ndx, pmf_key));
  SOC_SAND_SET_BIT(fld_val, SOC_SAND_BOOL2NUM_INVERSE(is_ce_not_valid), arad_pmf_ce_valid_fld_bit_location_get(unit, stage, ce_group_ndx, ce_ndx));
  soc_mem_field32_set(unit, ce_table, tbl_data, arad_pmf_ce_valid_fld_get(unit, stage, ce_ndx, pmf_key), fld_val);

  res = soc_mem_write(
          unit,
          ce_table,
          MEM_BLOCK_ANY,
          line_ndx,
          tbl_data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_ce_nop_entry_set_unsafe()", pmf_pgm_ndx, pmf_key);
}



uint32
  arad_pmf_ce_nop_entry_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  uint32                            pmf_pgm_ndx,
    SOC_SAND_IN  uint32                            pmf_key,
    SOC_SAND_IN  uint32                            ce_ndx,
    SOC_SAND_IN  uint8                            is_ce_in_msb,
    SOC_SAND_IN  uint8                            is_second_lookup,
    SOC_SAND_OUT uint8                            *is_ce_not_valid,
    SOC_SAND_OUT uint32                             *loc 
  )
{
    uint32
        ce_group_ndx = arad_pmf_low_level_ce_is_second_group(unit, stage, ce_ndx),
        line_ndx,
      fld_val,
        ce_instr_encoding = 0,
      res = SOC_SAND_OK,
      tbl_data[ARAD_PMF_CE_TABLE_LENGTH_IN_UINT32S];
    soc_mem_t
        ce_table;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PMF_CE_NOP_ENTRY_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(is_ce_not_valid);
  SOC_SAND_CHECK_NULL_INPUT(loc);

  res = arad_pmf_ce_entry_verify( unit,
                                  stage,
                                  pmf_pgm_ndx,
                                  pmf_key,
                                  ce_ndx,
                                  is_ce_in_msb,
                                  is_second_lookup );

  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  sal_memset(tbl_data, 0x0, ARAD_PMF_CE_TABLE_LENGTH_IN_UINT32S * sizeof(uint32));


  
  ce_table = arad_pmf_ce_table_get(unit, stage, is_ce_in_msb, is_second_lookup, FALSE , ce_ndx);
  line_ndx = arad_pmf_ce_line_get(unit, stage, is_ce_in_msb, pmf_pgm_ndx, ce_ndx);
  res = soc_mem_read(
          unit,
          ce_table,
          MEM_BLOCK_ANY,
          line_ndx,
          tbl_data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  
  
  fld_val = soc_mem_field32_get(unit, ce_table, tbl_data, arad_pmf_ce_valid_fld_get(unit, stage, ce_ndx, pmf_key));
  fld_val = SOC_SAND_GET_BIT(fld_val, arad_pmf_ce_valid_fld_bit_location_get(unit, stage, ce_group_ndx, ce_ndx));

  if (fld_val == FALSE)
  {
    *is_ce_not_valid = TRUE;
  }
  else
  {
    *is_ce_not_valid = FALSE;
    
    ce_instr_encoding = soc_mem_field32_get(unit, ce_table, tbl_data, arad_pmf_ce_instruction_fld_get(unit, stage, ce_ndx));

    
    fld_val = SOC_SAND_GET_BITS_RANGE(ce_instr_encoding, ARAD_PMF_CE_INSTR_ENCODING_IS_HEADER_MSB, ARAD_PMF_CE_INSTR_ENCODING_IS_HEADER_LSB);
    *loc = (fld_val == ARAD_PMF_CE_INSTR_ENCODING_VALUE_IS_INTERNAL_FIELD) ? 0 : 1;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pmf_ce_nop_entry_get_unsafe()", pmf_pgm_ndx, pmf_key);
}

uint32
  ARAD_PMF_CE_PACKET_HEADER_INFO_verify(
     SOC_SAND_IN  int                            unit,
      SOC_SAND_IN  SOC_PPC_FP_DATABASE_STAGE            stage,
    SOC_SAND_IN  ARAD_PMF_CE_PACKET_HEADER_INFO *info,
    SOC_SAND_IN  uint32                        ce_ndx
  )
{
    uint8
        is_32b_ce;
    uint32
        resolution_ce_in_bits,
        ce_nof_bits_max = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->sub_header, ARAD_PMF_LOW_LEVEL_SUB_HEADER_MAX, ARAD_PMF_LOW_LEVEL_SUB_HEADER_OUT_OF_RANGE_ERR, 10, exit);

  
  
  if ((info->offset > 1023))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_PMF_LOW_LEVEL_OFFSET_OUT_OF_RANGE_ERR, 11, exit);
  }


  is_32b_ce = arad_pmf_low_level_ce_is_32b_ce(unit, stage, ce_ndx);
  ce_nof_bits_max = (is_32b_ce)? ARAD_PMF_LOW_LEVEL_NOF_BITS_CE_32B_MAX : ARAD_PMF_LOW_LEVEL_NOF_BITS_CE_16B_MAX;
  LOG_DEBUG(BSL_LS_SOC_FP,
            (BSL_META_U(unit,
                        "     "
                        "CE: NOP set, "
                        "CE-ID: %d, "
                        "Stage: %s, "
                        "ce_nof_bits_max: %d, "
                        "info->nof_bits: %d\n\r"),
             ce_ndx, SOC_PPC_FP_DATABASE_STAGE_to_string(stage), ce_nof_bits_max, info->nof_bits));
  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->nof_bits, ARAD_PMF_LOW_LEVEL_NOF_BITS_MIN, ce_nof_bits_max, ARAD_PMF_LOW_LEVEL_NOF_BITS_OUT_OF_RANGE_ERR, 12, exit);

  
  resolution_ce_in_bits = (is_32b_ce)? ARAD_PMF_CE_INSTR_RESOLUTION_32B_BYTES:ARAD_PMF_CE_INSTR_RESOLUTION_16B_NIBBLES;
  if (((info->offset + info->nof_bits) % resolution_ce_in_bits) != 0) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PMF_LOW_LEVEL_OFFSET_OUT_OF_RANGE_ERR, 15, exit);
  }
  

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PMF_CE_PACKET_HEADER_INFO_verify()",0,0);
}

void
  ARAD_PMF_CE_PACKET_HEADER_INFO_clear(
    SOC_SAND_OUT ARAD_PMF_CE_PACKET_HEADER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_CE_PACKET_HEADER_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if ARAD_DEBUG_IS_LVL1

const char*
  ARAD_PMF_CE_SUB_HEADER_to_string(
    SOC_SAND_IN  ARAD_PMF_CE_SUB_HEADER enum_val
  )
{
  return SOC_TMC_PMF_CE_SUB_HEADER_to_string(enum_val);
}


void
  ARAD_PMF_CE_PACKET_HEADER_INFO_print(
    SOC_SAND_IN  ARAD_PMF_CE_PACKET_HEADER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PMF_CE_PACKET_HEADER_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

#undef _ERR_MSG_MODULE_NAME

#endif 




