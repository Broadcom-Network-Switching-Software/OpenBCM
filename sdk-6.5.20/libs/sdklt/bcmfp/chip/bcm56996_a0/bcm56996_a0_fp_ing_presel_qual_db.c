/*! \file bcm56996_a0_fp_ing_presel_qual_db.c
 *
 *  IFP preselection qualifier configuration DB
 *  initialization function for Tomahawk-4G(56996_A0).
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
bcmfp_bcm56996_a0_ingress_presel_qual_cfg_db_init(int unit,
                                                  bcmfp_stage_t *stage)
{
    uint32_t fid = 0;
    /*
     * All the qualifiers offset mentioned are the actual individual HW field
     * offset in IFP_LOGICAL_TABLE_SELECT Memory which are starting from bit
     * position 1. "KEY" overlap field in the memory is used during write,
     * hence to accomidate in the field, the offsets of the qualifiers has to
     * be substracted by 1.
     */
    uint8_t mixed_src_class_offset = 0;
    uint8_t em_lt_id_offset = 0;
    
    uint8_t drop_offset = 0;
    uint8_t pkt_res_offset = 0;
    uint8_t fwd_type_offset = 0;
    uint8_t lookup_status_offset = 0;
    uint8_t mystation_hit_offset = 0;
    uint8_t mystation_2_hit_offset = 0;
    uint8_t tunnel_type_offset = 0;
    uint8_t loopback_type_offset = 0;
    uint8_t l4pkt_offset = 0;
    uint8_t ip_type_offset = 0;
    bcmfp_qual_flags_bmp_t qual_flags;
    BCMFP_PRESEL_QUAL_CFG_DECL;

    SHR_FUNC_ENTER(unit);

    mixed_src_class_offset = 0;
    em_lt_id_offset = 32;
    
    drop_offset = 47;
    pkt_res_offset = 48;
    fwd_type_offset = 54;
    lookup_status_offset = 58;
    mystation_hit_offset = 76;
    mystation_2_hit_offset = 77;
    tunnel_type_offset = 78;
    loopback_type_offset = 81;
    l4pkt_offset = 84;
    ip_type_offset = 85;

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    sal_memset(&qual_flags, 0, sizeof(bcmfp_qual_flags_bmp_t));

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_MIXED_SRC_CLASSf;
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
        qual_flags, BCMFP_EXT_SECTION_DISABLE, 0,
        32, mixed_src_class_offset, 0);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_EM_FIRST_LOOKUP_LTIDf;
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
        qual_flags, BCMFP_EXT_SECTION_DISABLE, 0,
        4, em_lt_id_offset, 0);




    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_DROP_PKTf;
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
        qual_flags, BCMFP_EXT_SECTION_DISABLE, 0,
        1, drop_offset, 0);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_PKT_RESOLUTIONf;
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
        qual_flags, BCMFP_EXT_SECTION_DISABLE, 0,
        6, pkt_res_offset, 0);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_FWD_TYPEf;
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
        qual_flags, BCMFP_EXT_SECTION_DISABLE, 0,
        4, fwd_type_offset, 0);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_MYSTATIONTCAM_HITf;
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
        qual_flags, BCMFP_EXT_SECTION_DISABLE, 0,
        1, mystation_hit_offset, 0);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_MYSTATIONTCAM_2_HITf;
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
        qual_flags, BCMFP_EXT_SECTION_DISABLE, 0,
        1, mystation_2_hit_offset, 0);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_TNL_TYPEf;
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
        qual_flags, BCMFP_EXT_SECTION_DISABLE, 0,
        3, tunnel_type_offset, 0);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_LOOPBACK_TYPEf;
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
        qual_flags, BCMFP_EXT_SECTION_DISABLE, 0,
        1, loopback_type_offset, 0);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_L4_PKTf;
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
        qual_flags, BCMFP_EXT_SECTION_DISABLE, 0,
        1, l4pkt_offset, 0);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_IP_TYPEf;
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
        qual_flags, BCMFP_EXT_SECTION_DISABLE, 0,
        5, ip_type_offset, 0);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_ING_STP_STATEf;
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
        qual_flags, BCMFP_EXT_SECTION_DISABLE, 0,
        2, lookup_status_offset, 0);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_DOSATTACK_PKTf;
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
        qual_flags, BCMFP_EXT_SECTION_DISABLE, 0,
        1, lookup_status_offset + 2, 0);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_FWD_VLAN_VALIDf;
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
        qual_flags, BCMFP_EXT_SECTION_DISABLE, 0,
        1, lookup_status_offset + 3, 0);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_MPLSENTRY_FIRSTLOOKUP_HITf;
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
        qual_flags, BCMFP_EXT_SECTION_DISABLE, 0,
        1, lookup_status_offset + 4, 0);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_MPLSENTRY_SECONDLOOKUP_HITf;
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
        qual_flags, BCMFP_EXT_SECTION_DISABLE, 0,
        1, lookup_status_offset + 5, 0);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_MPLS_BOS_TERMINATEDf;
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
        qual_flags, BCMFP_EXT_SECTION_DISABLE, 0,
        1, lookup_status_offset + 6, 0);

    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_L3_TNL_HITf;
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
        qual_flags, BCMFP_EXT_SECTION_DISABLE, 0,
        1, lookup_status_offset + 7, 0);

exit:
    SHR_FUNC_EXIT();
}


int
bcmfp_bcm56996_a0_em_presel_qual_cfg_db_init(int unit,
                                             bcmfp_stage_t *stage)
{
    uint32_t fid = 0;
    uint8_t mixed_src_class_offset = 0;
    
    uint8_t drop_offset = 0;
    uint8_t fwd_type_offset = 0;
    uint8_t lookup_status_offset = 0;
    uint8_t mystation_hit_offset = 0;
    uint8_t mystation_2_hit_offset = 0;
    uint8_t tunnel_type_offset = 0;
    uint8_t loopback_type_offset = 0;
    uint8_t l4pkt_offset = 0;
    uint8_t ip_type_offset = 0;
    bcmfp_qual_flags_bmp_t qual_flags;
    BCMFP_PRESEL_QUAL_CFG_DECL;

    SHR_FUNC_ENTER(unit);

    mixed_src_class_offset = 1;
    
    drop_offset = 34;
    fwd_type_offset = 43;
    lookup_status_offset = 47;
    mystation_hit_offset = 65;
    mystation_2_hit_offset = 66;
    tunnel_type_offset = 67;
    loopback_type_offset = 70;
    l4pkt_offset = 73;
    ip_type_offset = 74;

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    sal_memset(&qual_flags, 0, sizeof(bcmfp_qual_flags_bmp_t));

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_MIXED_SRC_CLASSf;
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
        qual_flags, BCMFP_EXT_SECTION_DISABLE, 1,
        32, mixed_src_class_offset, 0);



    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_DROP_PKTf;
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
        qual_flags, BCMFP_EXT_SECTION_DISABLE, 0,
        1, drop_offset, 0);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_FWD_TYPEf;
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
        qual_flags, BCMFP_EXT_SECTION_DISABLE, 0,
        4, fwd_type_offset, 0);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_MYSTATIONTCAM_HITf;
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
        qual_flags, BCMFP_EXT_SECTION_DISABLE, 0,
        1, mystation_hit_offset, 0);
    /* Inner L2 MY STATION hit status in case of VXLAN */
    fid = FP_ING_PRESEL_ENTRY_TEMPLATEt_QUAL_MYSTATIONTCAM_2_HITf;
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
        qual_flags, BCMFP_EXT_SECTION_DISABLE, 0,
        1, mystation_2_hit_offset, 0);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_TNL_TYPEf;
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
        qual_flags, BCMFP_EXT_SECTION_DISABLE, 0,
        3, tunnel_type_offset, 0);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_TNL_TYPEf;
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
        qual_flags, BCMFP_EXT_SECTION_DISABLE, 0,
        1, loopback_type_offset, 0);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_L4_PKTf;
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
        qual_flags, BCMFP_EXT_SECTION_DISABLE, 0,
        1, l4pkt_offset, 0);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_IP_TYPEf;
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
        qual_flags, BCMFP_EXT_SECTION_DISABLE, 0,
        5, ip_type_offset, 0);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_ING_STP_STATEf;
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
        qual_flags, BCMFP_EXT_SECTION_DISABLE, 0,
        2, lookup_status_offset, 0);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_DOSATTACK_PKTf;
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
        qual_flags, BCMFP_EXT_SECTION_DISABLE, 0,
        1, lookup_status_offset + 2, 0);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_FWD_VLAN_VALIDf;
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
        qual_flags, BCMFP_EXT_SECTION_DISABLE, 0,
        1, lookup_status_offset + 3, 0);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_MPLSENTRY_FIRSTLOOKUP_HITf;
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
        qual_flags, BCMFP_EXT_SECTION_DISABLE, 0,
        1, lookup_status_offset + 4, 0);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_MPLSENTRY_SECONDLOOKUP_HITf;
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
        qual_flags, BCMFP_EXT_SECTION_DISABLE, 0,
        1, lookup_status_offset + 5, 0);

    fid = FP_EM_PRESEL_ENTRY_TEMPLATEt_QUAL_L3_TNL_HITf;
    BCMFP_PRESEL_QUAL_CFG_ONE_OFFSET_ADD(unit, stage, fid,
        qual_flags, BCMFP_EXT_SECTION_DISABLE, 0,
        1, lookup_status_offset + 7, 0);
exit:
    SHR_FUNC_EXIT();
}
