/*! \file bcm56996_a0_fp_vlan_qual_db.c
 *
 *  VFP qualifier configuration DB initialization function for Tomahawk-4GG(56996_A0).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/chip/bcm56996_a0_fp.h>
#include <bcmltd/chip/bcmltd_id.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcmfp_bcm56996_a0_vlan_qual_cfg_db_init(int unit,
                                        bcmfp_stage_t *stage)
{
    uint32_t f1_offset = 164, f2_offset = 36, f3_offset = 0;
    uint32_t fid = 0;
    bcmfp_qual_flags_bmp_t qual_flags;

    BCMFP_QUAL_CFG_DECL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    sal_memset(&qual_flags, 0, sizeof(bcmfp_qual_flags_bmp_t));

    /* VFP_F1*/
    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INPORTf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            9, f1_offset + 0, 0,
            BCMFP_EXT_CTRL_SEL_SRC, BCMFP_FWD_ENTITY_MODPORTGPORT);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_SRC_TRUNKf;
    BCMFP_QUAL_CFG_TWO_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
                            qual_flags,
                            BCMFP_EXT_SECTION_FPF1, 0,
                            8, f1_offset, 0,/* trunk id field of SGLP */
                            0, 0,
                            1, f1_offset + 17 , 0,/* trunk bit of SGLP */
                            BCMFP_EXT_CTRL_SEL_SRC, BCMFP_FWD_ENTITY_GLP);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_PORT_GRP_OR_PORT_SYSTEM_CLASSf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            8, f1_offset + 9, 0,/* ingress port group field */
            BCMFP_EXT_CTRL_SEL_SRC, BCMFP_FWD_ENTITY_PORTGROUPNUM);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_SRC_PORTf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            9, f1_offset + 0, 0,/* Port field of unresolved SGLP */
            BCMFP_EXT_CTRL_SEL_SRC, BCMFP_FWD_ENTITY_MODPORTGPORT);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_IP_CHECKSUM_VALIDf;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            1, f1_offset + 18, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_L2_FORMATf;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            2, f1_offset + 20, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_HIGIG_PKTf;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            1, f1_offset + 19, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_IP_TYPEf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            4, f1_offset + 22, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_IP_TYPEf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            5,  f1_offset + 22, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_IP_PROTO_COMMONf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            3, f1_offset + 26, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_IP_PROTO_COMMONf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            3, f1_offset + 26, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_L4_PKTf;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            1, f1_offset + 29, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_VLAN_OUTER_PRESENTf;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            1, f1_offset + 30, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_OPAQUE_TAG_PRESENTf;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            1, f1_offset + 31, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_OPAQUE_TAG_TYPEf;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            2, f1_offset + 32, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_OUTER_TPIDf;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            2, f1_offset + 34, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_OUTER_VLAN_IDf;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            12, f1_offset + 36, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_OUTER_VLAN_CFIf;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            1, f1_offset + 48, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_OUTER_VLAN_PRIf;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            3, f1_offset + 49, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_TNL_TERMINATEDf;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            1, f1_offset + 56, 0);

    /* VFP_F2_0*/
    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_DST_IP4f;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            32, f2_offset + 0, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_DST_IP4f;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            32, f2_offset + 0, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_SRC_IP4f;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            32, f2_offset + 32, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_SRC_IP4f;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            32, f2_offset + 32, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_IP_FRAGf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            2, f2_offset + 64, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_IP_FRAGf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            2, f2_offset + 64, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_IP_PROTOCOLf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            8, f2_offset + 66, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_IP_PROTOCOLf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            8, f2_offset + 66, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_L4DST_PORTf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            16, f2_offset + 74, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_L4DST_PORTf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            16, f2_offset + 74, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_L4SRC_PORTf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            16, f2_offset + 90, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_L4SRC_PORTf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            16, f2_offset + 90, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_ICMP_TYPE_CODEf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            16, f2_offset + 90, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_TCP_FLAGSf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            6, f2_offset + 106, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_TOSf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            8, f2_offset + 112, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_TOSf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            8, f2_offset + 112, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_TTLf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            8, f2_offset + 120, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_TTLf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            8, f2_offset + 120, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    /* VFP_F2_1 */
    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_SRC_IP6f;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 1,
            128, f2_offset, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_SRC_IP6f;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 1,
            128, f2_offset, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    /* VFP_F2_2 */
    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_DST_IP6f;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 2,
            128, f2_offset, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_DST_IP6f;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 2,
            128, f2_offset, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    /* VFP_F2_3 */
    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_ETHERTYPEf;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 3,
            16, f2_offset, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_DST_MACf;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 3,
            48, f2_offset + 16, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_SRC_MACf;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 3,
            48, f2_offset + 64, 0);

    /* VFP_F2_4 */
    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_DST_IP4f;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 4,
            32, f2_offset, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_DST_IP4f;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 4,
            32, f2_offset, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_SRC_IP4f;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 4,
            32, f2_offset + 32, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_SRC_IP4f;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 4,
            32, f2_offset + 32, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_IP_PROTOCOLf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 4,
            8, f2_offset + 64, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_IP_PROTOCOLf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 4,
            8, f2_offset + 64, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_SRC_MACf;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 4,
            48, f2_offset + 72, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_TOSf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 4,
            8, f2_offset + 120, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_TOSf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 4,
            8, f2_offset + 120, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    /* VFP_F2_5 */
    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_DST_IP4f;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 5,
            32, f2_offset, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_DST_IP4f;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 5,
            32, f2_offset, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_SRC_IP4f;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 5,
            32, f2_offset + 32, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_SRC_IP4f;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 5,
            32, f2_offset + 32, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_IP_PROTOCOLf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 5,
            8, f2_offset + 64, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_IP_PROTOCOLf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 5,
            8, f2_offset + 64, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_DST_MACf;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 5,
            48, f2_offset + 72, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_TOSf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 5,
            8, f2_offset + 120, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_TOSf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 5,
            8, f2_offset + 120, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    /* VFP_F2_6 */
    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_DST_IP6_HIGHf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 6,
            64, f2_offset, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_DST_IP6_HIGHf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 6,
            64, f2_offset, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_SRC_IP6_HIGHf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 6,
            64, f2_offset + 64, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_SRC_IP6_HIGHf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 6,
            64, f2_offset + 64, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    /* VFP_F2_7 */
    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_LLC_HEADERf;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 7,
            24, f2_offset, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_SNAP_HEADERf;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 7,
            40, f2_offset + 24, 0);

    /* VFP_F2_8  */
    /* UDF1 Chunk 7 */
    fid = FP_VLAN_GRP_TEMPLATEt_FIELD_COUNT + 7;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 8,
            16, f2_offset, 0);

    /* UDF1 Chunk 6 */
    fid = FP_VLAN_GRP_TEMPLATEt_FIELD_COUNT + 6;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 8,
            16, f2_offset + 16, 0);

    /* UDF1 Chunk 5 */
    fid = FP_VLAN_GRP_TEMPLATEt_FIELD_COUNT + 5;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 8,
            16, f2_offset + 32, 0);

    /* UDF1 Chunk 4 */
    fid = FP_VLAN_GRP_TEMPLATEt_FIELD_COUNT + 4;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 8,
            16, f2_offset + 48, 0);

    /* UDF1 Chunk 3 */
    fid = FP_VLAN_GRP_TEMPLATEt_FIELD_COUNT + 3;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 8,
            16, f2_offset + 64, 0);

    /* UDF1 Chunk 2 */
    fid = FP_VLAN_GRP_TEMPLATEt_FIELD_COUNT + 2;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 8,
            16, f2_offset + 80, 0);

    /* UDF1 Chunk 1 */
    fid = FP_VLAN_GRP_TEMPLATEt_FIELD_COUNT + 1;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 8,
            16, f2_offset + 96, 0);

    /* UDF1 Chunk 0 */
    fid = FP_VLAN_GRP_TEMPLATEt_FIELD_COUNT;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 8,
            16, f2_offset + 112, 0);

    /* VFP_F2_9 */
    /* UDF2 Chunk 7 */
    fid = FP_VLAN_GRP_TEMPLATEt_FIELD_COUNT + 15;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 9,
            16, f2_offset, 0);

    /* UDF2 Chunk 6 */
    fid = FP_VLAN_GRP_TEMPLATEt_FIELD_COUNT + 14;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 9,
            16, f2_offset + 16, 0);

    /* UDF2 Chunk 5 */
    fid = FP_VLAN_GRP_TEMPLATEt_FIELD_COUNT + 13;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 9,
            16, f2_offset + 32, 0);

    /* UDF2 Chunk 4 */
    fid = FP_VLAN_GRP_TEMPLATEt_FIELD_COUNT + 12;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 9,
            16, f2_offset + 48, 0);

    /* UDF2 Chunk 3 */
    fid = FP_VLAN_GRP_TEMPLATEt_FIELD_COUNT + 11;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 9,
            16, f2_offset + 64, 0);

    /* UDF2 Chunk 2 */
    fid = FP_VLAN_GRP_TEMPLATEt_FIELD_COUNT + 10;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 9,
            16, f2_offset + 80, 0);

    /* UDF2 Chunk 1 */
    fid = FP_VLAN_GRP_TEMPLATEt_FIELD_COUNT + 9;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 9,
            16, f2_offset + 96, 0);

    /* UDF2 Chunk 0 */
    fid = FP_VLAN_GRP_TEMPLATEt_FIELD_COUNT + 8;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 9,
            16, f2_offset + 112, 0);

    /* VFP_F3_0 */
    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_ETHERTYPEf;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 0,
            16, f3_offset, 0);

    /* VFP_F3_1 */
    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_L4DST_PORTf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 1,
            16, f3_offset + 0, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_L4DST_PORTf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 1,
            16, f3_offset + 0, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_L4SRC_PORTf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 1,
            16, f3_offset + 16, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_L4SRC_PORTf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 1,
            16, f3_offset + 16, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_ICMP_TYPE_CODEf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 1,
            16, f3_offset + 16, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

/* VFP_F3_2 */
    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_IP_FIRST_EH_PROTOf;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 2,
            8, f3_offset, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_IP_FIRST_EH_SUBCODEf;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 2,
            8, f3_offset + 8, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_IP_PROTOCOLf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 2,
            8, f3_offset + 16, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_IP_PROTOCOLf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 2,
            8, f3_offset + 16, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_TOSf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 2,
            8, f3_offset + 24, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_TOSf;
    BCMFP_QUAL_CFG_ONE_OFFSET_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 2,
            8, f3_offset + 24, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    /* VFP_F3_3 */
    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_TNL_CLASS_IDf;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 3,
            12, f3_offset, 0);

    /* VFP_DOUBLE_WIDE_F2_0 */
    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_DST_IP4f;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            32, f2_offset , 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_DST_IP4f;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            32, f2_offset, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_SRC_IP4f;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            32, f2_offset + 32, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_SRC_IP4f;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            32, f2_offset + 32, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_IP_FRAGf;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            2, f2_offset + 64, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_IP_FRAGf;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            2, f2_offset + 64, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_IP_PROTOCOLf;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            8, f2_offset + 66, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_IP_PROTOCOLf;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            8, f2_offset + 66, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_L4DST_PORTf;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            16, f2_offset + 74, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_L4DST_PORTf;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            16, f2_offset + 74, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_L4SRC_PORTf;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            16, f2_offset + 90, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_L4SRC_PORTf;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            16, f2_offset + 90, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_ICMP_TYPE_CODEf;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            16, f2_offset + 90, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_TCP_FLAGSf;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            6, f2_offset + 106, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_TOSf;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            8, f2_offset + 112, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_TOSf;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            8, f2_offset + 112, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_TTLf;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            8, f2_offset + 120, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_TTLf;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 0,
            8, f2_offset + 120, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    /* VFP_DOUBLE_WIDE_F2_1 */
    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_SRC_IP6f;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 1,
            128, f2_offset, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_SRC_IP6f;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 1,
            128, f2_offset, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    /* VFP_DOUBLE_WIDE_F2_2 */
    /* UDF2 Chunk 7 */
    fid = FP_VLAN_GRP_TEMPLATEt_FIELD_COUNT + 15;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 2,
            16, f2_offset, 0);

    /* UDF2 Chunk 6 */
    fid = FP_VLAN_GRP_TEMPLATEt_FIELD_COUNT + 14;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 2,
            16, f2_offset + 16, 0);

    /* UDF2 Chunk 5 */
    fid = FP_VLAN_GRP_TEMPLATEt_FIELD_COUNT + 13;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 2,
            16, f2_offset + 32, 0);

    /* UDF2 Chunk 4 */
    fid = FP_VLAN_GRP_TEMPLATEt_FIELD_COUNT + 12;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 2,
            16, f2_offset + 48, 0);

    /* UDF2 Chunk 3 */
    fid = FP_VLAN_GRP_TEMPLATEt_FIELD_COUNT + 11;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 2,
            16, f2_offset + 64, 0);

    /* UDF2 Chunk 2 */
    fid = FP_VLAN_GRP_TEMPLATEt_FIELD_COUNT + 10;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 2,
            16, f2_offset + 80, 0);

    /* UDF2 Chunk 1 */
    fid = FP_VLAN_GRP_TEMPLATEt_FIELD_COUNT + 9;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 2,
            16, f2_offset + 96, 0);

    /* UDF2 Chunk 0 */
    fid = FP_VLAN_GRP_TEMPLATEt_FIELD_COUNT + 8;
    BCMFP_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 2,
            16, f2_offset + 112, 0);

    /* VFP_DOUBLE_WIDE_F2_3 */
    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_DST_IP6_HIGHf;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD
        (unit, stage, fid,
         qual_flags,
         BCMFP_EXT_SECTION_FPF2, 3,
         64, f2_offset, 0,
         BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_DST_IP6_HIGHf;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD
        (unit, stage, fid,
         qual_flags,
         BCMFP_EXT_SECTION_FPF2, 3,
         64, f2_offset, 0,
         BCMFP_EXT_CTRL_SEL_IP_HEADER  , 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_DST_MACf;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF2, 3,
            48, f2_offset + 64, 0,
            BCMFP_EXT_SECTION_DISABLE, 0);

    /* VFP_DOUBLE_WIDE_F3 */
    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_IP_FRAGf;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 0,
            2, f3_offset + 0, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_IP_FRAGf;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 0,
            2, f3_offset + 0, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_IP_PROTOCOLf;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 0,
            8, f3_offset + 2, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_IP_PROTOCOLf;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 0,
            8, f3_offset + 2, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_TCP_FLAGSf;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 0,
            6, f3_offset + 10, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_TOSf;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 0,
            8, f3_offset + 16, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_TOSf;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 0,
            8, f3_offset + 16, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_INNER_TTLf;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 0,
            8, f3_offset + 24, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 1);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_TTLf;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF3, 0,
            8, f3_offset + 24, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    /* VFP_DOUBLE_WIDE_F1 */
    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_ETHERTYPEf;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            16, f1_offset, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_IP_FIRST_EH_PROTOf;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            8, f1_offset + 16, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_IP_FIRST_EH_SUBCODEf;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            8, f1_offset + 24, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);

    fid = FP_VLAN_GRP_TEMPLATEt_QUAL_TNL_CLASS_IDf;
    BCMFP_QUAL_CFG_INTRA_SLICE_CTRL_SEL_ADD(unit, stage, fid,
            qual_flags,
            BCMFP_EXT_SECTION_FPF1, 0,
            12, f1_offset + 32, 0,
            BCMFP_EXT_CTRL_SEL_IP_HEADER, 0);
exit:
    SHR_FUNC_EXIT();
}
