/*! \file bcm56780_a0_dna_2_4_13_gene_lib.c
 *
 * Broadcom BCM56780_a0 DNA_2_4_13 Generic Extensible NPL Encapsulation.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/flexdigest.h>

#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/chip/bcmgene_db.h>
#include <bcm_int/ltsw/chip/bcmgene_feature.h>

#include <bcm_int/ltsw/hash_output_selection.h>

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */

/******************************************************************************
 * Private functions
 */

static bcmgene_field_desc_t bcmgene_flexdigest_norm_profile_seed_key[] = {
    {
        .flags = 0,
        .blk = 0,
        .offset = 0,
        .width = 0,
        .depth = 0,
        .field = FLEX_DIGEST_NORM_PROFILE_SEED_IDs,
        .converter = NULL,
    },
};

static bcmgene_field_desc_t bcmgene_flexdigest_norm_profile_seed_fld[] = {
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestNormSeedControlBinASeed,
        .width = 0,
        .depth = 0,
        .field = BIN_A_SEEDs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestNormSeedControlBinBSeed,
        .width = 0,
        .depth = 0,
        .field = BIN_B_SEEDs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestNormSeedControlBinCSeed,
        .width = 0,
        .depth = 0,
        .field = BIN_C_SEEDs,
        .converter = NULL,
    },
};

static bcmgene_field_desc_t bcmgene_flexdigest_norm_profile_key[] = {
    {
        .flags = 0,
        .blk = 0,
        .offset = 0,
        .width = 0,
        .depth = 0,
        .field = FLEX_DIGEST_NORM_PROFILE_IDs,
        .converter = NULL,
    },
};

static bcmgene_field_desc_t bcmgene_flexdigest_norm_profile_fld[] = {
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestNormProfileControlNorm,
        .width = 0,
        .depth = 0,
        .field = NORMs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestNormProfileControlSeedId,
        .width = 0,
        .depth = 0,
        .field = FLEX_DIGEST_NORM_PROFILE_SEED_IDs,
        .converter = NULL,
    },
};
static bcmgene_table_desc_t bcmgene_flexdigest_norm[] = {
    {
        .sub_f = BCMGENE_FLEXDIGEST_SUB_F_NORM_SEED_PROFILE,
        .table = FLEX_DIGEST_NORM_PROFILE_SEEDs,
        .idx_min = 0,
        .idx_max = 1,
        .keys = COUNTOF(bcmgene_flexdigest_norm_profile_seed_key),
        .key = bcmgene_flexdigest_norm_profile_seed_key,
        .flds = COUNTOF(bcmgene_flexdigest_norm_profile_seed_fld),
        .fld = bcmgene_flexdigest_norm_profile_seed_fld,
    },
    {
        .sub_f = BCMGENE_FLEXDIGEST_SUB_F_NORM_PROFILE,
        .table = FLEX_DIGEST_NORM_PROFILEs,
        .idx_min = 0,
        .idx_max = 0,
        .keys = COUNTOF(bcmgene_flexdigest_norm_profile_key),
        .key = bcmgene_flexdigest_norm_profile_key,
        .flds = COUNTOF(bcmgene_flexdigest_norm_profile_fld),
        .fld = bcmgene_flexdigest_norm_profile_fld,
    },
};

static bcmgene_field_desc_t bcmgene_flexdigest_lkup_presel_fld[] = {
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestQualifyL4Valid,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_L4_VALIDs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestQualifyMyStation1Hit,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_MY_STATION_1_HITs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestQualifyTunnelProcessingResults1,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_TUNNEL_PROCESSING_RESULTS_1s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .offset = bcmFlexDigestQualifyTunnelProcessingResultsRaw,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_TUNNEL_PROCESSING_RESULTS_RAWs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestQualifyFlexHve2ResultSet1,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_FLEX_HVE2_RESULT_SET_1s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestQualifyFixedHve2ResultSet0,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_FIXED_HVE2_RESULT_SET_0s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestQualifyFlexHve1ResultSet1,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_FLEX_HVE1_RESULT_SET_1s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestQualifyFixedHve1ResultSet0,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_FIXED_HVE1_RESULT_SET_0s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestQualifyL2IifFlexDigestCtrlIdB,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_L2_IIF_FLEX_DIGEST_CTRL_ID_Bs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestQualifyL2IifFlexDigestCtrlIdA,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_L2_IIF_FLEX_DIGEST_CTRL_ID_As,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestQualifyProtocolPktOpaqueCtrlId,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_PROTOCOL_PKT_OPAQUE_CTRL_IDs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestQualifyVfiOpaqueCtrlId,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_VFI_OPAQUE_CTRL_IDs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestQualifySvpOpaqueCtrlId,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_SVP_OPAQUE_CTRL_IDs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestQualifyL3IifOpaqueCtrlId,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_L3_IIF_OPAQUE_CTRL_IDs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestQualifyL2IifOpaqueCtrlId,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_L2_IIF_OPAQUE_CTRL_IDs,
        .converter = NULL,
    },
};

static bcmgene_field_desc_t bcmgene_flexdigest_lkup_key[] = {
    {
        .flags = 0,
        .blk = 0,
        .offset = 0,
        .width = 0,
        .depth = 0,
        .field = FLEX_DIGEST_LKUP_IDs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = 1,
        .width = 0,
        .depth = 0,
        .field = GROUP_PRIORITYs,
        .converter = NULL,
    },
};

static bcmgene_field_value_map_t bcmgene_flexdigest_lkup_fld_val_map[] = {
    {
        .in = 0,
        .out.s = INVALIDs,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_MACSA_0_15,
        .out.s = PARSER1_FIELD_BUS_MACSA_0_15s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_MACSA_16_31,
        .out.s = PARSER1_FIELD_BUS_MACSA_16_31s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_MACSA_32_47,
        .out.s = PARSER1_FIELD_BUS_MACSA_32_47s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_MACDA_0_15,
        .out.s = PARSER1_FIELD_BUS_MACDA_0_15s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_MACDA_16_31,
        .out.s = PARSER1_FIELD_BUS_MACDA_16_31s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_MACDA_32_47,
        .out.s = PARSER1_FIELD_BUS_MACDA_32_47s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_0_15,
        .out.s = PARSER1_FIELD_BUS_IP_HDR_SIP_0_15s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_16_31,
        .out.s = PARSER1_FIELD_BUS_IP_HDR_SIP_16_31s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_32_47,
        .out.s = PARSER1_FIELD_BUS_IP_HDR_SIP_32_47s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_48_63,
        .out.s = PARSER1_FIELD_BUS_IP_HDR_SIP_48_63s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_64_79,
        .out.s = PARSER1_FIELD_BUS_IP_HDR_SIP_64_79s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_80_95,
        .out.s = PARSER1_FIELD_BUS_IP_HDR_SIP_80_95s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_96_111,
        .out.s = PARSER1_FIELD_BUS_IP_HDR_SIP_96_111s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_SIP_112_127,
        .out.s = PARSER1_FIELD_BUS_IP_HDR_SIP_112_127s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_0_15,
        .out.s = PARSER1_FIELD_BUS_IP_HDR_DIP_0_15s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_16_31,
        .out.s = PARSER1_FIELD_BUS_IP_HDR_DIP_16_31s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_32_47,
        .out.s = PARSER1_FIELD_BUS_IP_HDR_DIP_32_47s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_48_63,
        .out.s = PARSER1_FIELD_BUS_IP_HDR_DIP_48_63s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_64_79,
        .out.s = PARSER1_FIELD_BUS_IP_HDR_DIP_64_79s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_80_95,
        .out.s = PARSER1_FIELD_BUS_IP_HDR_DIP_80_95s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_96_111,
        .out.s = PARSER1_FIELD_BUS_IP_HDR_DIP_96_111s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_DIP_112_127,
        .out.s = PARSER1_FIELD_BUS_IP_HDR_DIP_112_127s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_ITAG_0_15,
        .out.s = PARSER1_FIELD_BUS_ITAG_0_15s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_ITAG_16_31,
        .out.s = PARSER1_FIELD_BUS_ITAG_16_31s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_VXLAN_VN_ID_RESERVED_2_0_15,
        .out.s = PARSER1_FIELD_BUS_VXLAN_VN_ID_RESERVED_2_0_15s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_VXLAN_VN_ID_RESERVED_2_16_31,
        .out.s = PARSER1_FIELD_BUS_VXLAN_VN_ID_RESERVED_2_16_31s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_OTAG_0_15,
        .out.s = PARSER1_FIELD_BUS_OTAG_0_15s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_OTAG_16_31,
        .out.s = PARSER1_FIELD_BUS_OTAG_16_31s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_L4_DST_PORT,
        .out.s = PARSER1_FIELD_BUS_L4_DST_PORTs,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_PROTOCOL,
        .out.s = PARSER1_FIELD_BUS_IP_HDR_PROTOCOLs,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_ETHERTYPE,
        .out.s = PARSER1_FIELD_BUS_ETHERTYPEs,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_L4_SRC_PORT,
        .out.s = PARSER1_FIELD_BUS_L4_SRC_PORTs,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_FIRST_TWO_BYTES,
        .out.s = PARSER1_FIELD_BUS_IP_HDR_FIRST_TWO_BYTESs,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_FLOW_LABEL,
        .out.s = PARSER1_FIELD_BUS_IP_HDR_FLOW_LABELs,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_IP_EXT_HDR1_PROTOCOL,
        .out.s = PARSER1_FIELD_BUS_IP_EXT_HDR1_PROTOCOLs,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_IP_HDR_TTL,
        .out.s = PARSER1_FIELD_BUS_IP_HDR_TTLs,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_IP_EXT_HDR2_PROTOCOL,
        .out.s = PARSER1_FIELD_BUS_IP_EXT_HDR2_PROTOCOLs,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_L5_BYTES_4_7_0_15,
        .out.s = PARSER1_FIELD_BUS_L5_BYTES_4_7_0_15s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_L5_BYTES_4_7_16_31,
        .out.s = PARSER1_FIELD_BUS_L5_BYTES_4_7_16_31s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_IP_LAST_PROTOCOL,
        .out.s = ING_OBJ_BUS_OUTER_IP_LAST_PROTOCOLs,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_MACSA_0_15,
        .out.s = PARSER2_FIELD_BUS_MACSA_0_15s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_MACSA_16_31,
        .out.s = PARSER2_FIELD_BUS_MACSA_16_31s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_MACSA_32_47,
        .out.s = PARSER2_FIELD_BUS_MACSA_32_47s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_MACDA_0_15,
        .out.s = PARSER2_FIELD_BUS_MACDA_0_15s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_MACDA_16_31,
        .out.s = PARSER2_FIELD_BUS_MACDA_16_31s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_MACDA_32_47,
        .out.s = PARSER2_FIELD_BUS_MACDA_32_47s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_SIP_0_15,
        .out.s = PARSER2_FIELD_BUS_IP_HDR_SIP_0_15s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_SIP_16_31,
        .out.s = PARSER2_FIELD_BUS_IP_HDR_SIP_16_31s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_SIP_32_47,
        .out.s = PARSER2_FIELD_BUS_IP_HDR_SIP_32_47s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_SIP_48_63,
        .out.s = PARSER2_FIELD_BUS_IP_HDR_SIP_48_63s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_SIP_64_79,
        .out.s = PARSER2_FIELD_BUS_IP_HDR_SIP_64_79s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_SIP_80_95,
        .out.s = PARSER2_FIELD_BUS_IP_HDR_SIP_80_95s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_SIP_96_111,
        .out.s = PARSER2_FIELD_BUS_IP_HDR_SIP_96_111s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_SIP_112_127,
        .out.s = PARSER2_FIELD_BUS_IP_HDR_SIP_112_127s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_DIP_0_15,
        .out.s = PARSER2_FIELD_BUS_IP_HDR_DIP_0_15s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_DIP_16_31,
        .out.s = PARSER2_FIELD_BUS_IP_HDR_DIP_16_31s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_DIP_32_47,
        .out.s = PARSER2_FIELD_BUS_IP_HDR_DIP_32_47s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_DIP_48_63,
        .out.s = PARSER2_FIELD_BUS_IP_HDR_DIP_48_63s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_DIP_64_79,
        .out.s = PARSER2_FIELD_BUS_IP_HDR_DIP_64_79s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_DIP_80_95,
        .out.s = PARSER2_FIELD_BUS_IP_HDR_DIP_80_95s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_DIP_96_111,
        .out.s = PARSER2_FIELD_BUS_IP_HDR_DIP_96_111s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_DIP_112_127,
        .out.s = PARSER2_FIELD_BUS_IP_HDR_DIP_112_127s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_ITAG_0_15,
        .out.s = PARSER2_FIELD_BUS_ITAG_0_15s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_ITAG_16_31,
        .out.s = PARSER2_FIELD_BUS_ITAG_16_31s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_OTAG_0_15,
        .out.s = PARSER2_FIELD_BUS_OTAG_0_15s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_OTAG_16_31,
        .out.s = PARSER2_FIELD_BUS_OTAG_16_31s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_L4_DST_PORT,
        .out.s = PARSER2_FIELD_BUS_L4_DST_PORTs,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_PROTOCOL,
        .out.s = PARSER2_FIELD_BUS_IP_HDR_PROTOCOLs,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_ETHERTYPE,
        .out.s = PARSER2_FIELD_BUS_ETHERTYPEs,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_L4_SRC_PORT,
        .out.s = PARSER2_FIELD_BUS_L4_SRC_PORTs,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_FIRST_TWO_BYTES,
        .out.s = PARSER2_FIELD_BUS_IP_HDR_FIRST_TWO_BYTESs,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_FLOW_LABEL,
        .out.s = PARSER2_FIELD_BUS_IP_HDR_FLOW_LABELs,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_IP_EXT_HDR1_PROTOCOL,
        .out.s = PARSER2_FIELD_BUS_IP_EXT_HDR1_PROTOCOLs,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_IP_HDR_TTL,
        .out.s = PARSER2_FIELD_BUS_IP_HDR_TTLs,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_IP_EXT_HDR2_PROTOCOL,
        .out.s = PARSER2_FIELD_BUS_IP_EXT_HDR2_PROTOCOLs,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_L5_BYTES_4_7_0_15,
        .out.s = PARSER2_FIELD_BUS_L5_BYTES_4_7_0_15s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_L5_BYTES_4_7_16_31,
        .out.s = PARSER2_FIELD_BUS_L5_BYTES_4_7_16_31s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_IP_LAST_PROTOCOL,
        .out.s = ING_OBJ_BUS_INNER_IP_LAST_PROTOCOLs,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_SYSTEM_SOURCE,
        .out.s = ING_OBJ_BUS_SYSTEM_SOURCEs,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INGRESS_PP_PORT,
        .out.s = ING_OBJ_BUS_INGRESS_PP_PORTs,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_L2_IIF,
        .out.s = ING_OBJ_BUS_L2_IIFs,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_L3_IIF,
        .out.s = ING_OBJ_BUS_L3_IIFs,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_SVP,
        .out.s = ING_OBJ_BUS_SVPs,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_VFI,
        .out.s = ING_OBJ_BUS_VFIs,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_OUTER_XOR_SIP_0_15,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_OUTER_XOR_SIP_0_15s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_OUTER_XOR_SIP_16_31,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_OUTER_XOR_SIP_16_31s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_OUTER_XOR_DIP_0_15,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_OUTER_XOR_DIP_0_15s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_OUTER_XOR_DIP_16_31,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_OUTER_XOR_DIP_16_31s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_INNER_XOR_SIP_0_15,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_INNER_XOR_SIP_0_15s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_INNER_XOR_SIP_16_31,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_INNER_XOR_SIP_16_31s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_INNER_XOR_DIP_0_15,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_INNER_XOR_DIP_0_15s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_INNER_XOR_DIP_16_31,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_INNER_XOR_DIP_16_31s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_OUTER_IP_ADDR_XOR_HI_0_15,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_OUTER_IP_ADDR_XOR_HI_0_15s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_OUTER_IP_ADDR_XOR_HI_16_31,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_OUTER_IP_ADDR_XOR_HI_16_31s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_OUTER_IP_ADDR_XOR_LO_0_15,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_OUTER_IP_ADDR_XOR_LO_0_15s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_OUTER_IP_ADDR_XOR_LO_16_31,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_OUTER_IP_ADDR_XOR_LO_16_31s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_INNER_IP_ADDR_XOR_HI_0_15,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_INNER_IP_ADDR_XOR_HI_0_15s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_INNER_IP_ADDR_XOR_HI_16_31,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_INNER_IP_ADDR_XOR_HI_16_31s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_INNER_IP_ADDR_XOR_LO_0_15,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_INNER_IP_ADDR_XOR_LO_0_15s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_INNER_IP_ADDR_XOR_LO_16_31,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_INNER_IP_ADDR_XOR_LO_16_31s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_OUTER_IP_ADDR_LSB32_HI_0_15,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_OUTER_IP_ADDR_LSB32_HI_0_15s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_OUTER_IP_ADDR_LSB32_HI_16_31,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_OUTER_IP_ADDR_LSB32_HI_16_31s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_OUTER_IP_ADDR_LSB32_LO_0_15,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_OUTER_IP_ADDR_LSB32_LO_0_15s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_OUTER_IP_ADDR_LSB32_LO_16_31,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_OUTER_IP_ADDR_LSB32_LO_16_31s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_INNER_IP_ADDR_LSB32_HI_0_15,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_INNER_IP_ADDR_LSB32_HI_0_15s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_INNER_IP_ADDR_LSB32_HI_16_31,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_INNER_IP_ADDR_LSB32_HI_16_31s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_INNER_IP_ADDR_LSB32_LO_0_15,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_INNER_IP_ADDR_LSB32_LO_0_15s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_INNER_IP_ADDR_LSB32_LO_16_31,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_INNER_IP_ADDR_LSB32_LO_16_31s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_OUTER_L4_PORT_XOR_HI,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_OUTER_L4_PORT_XOR_HIs,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_OUTER_L4_PORT_XOR_LO,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_OUTER_L4_PORT_XOR_LOs,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_INNER_L4_PORT_XOR_HI,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_INNER_L4_PORT_XOR_HIs,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_INNER_L4_PORT_XOR_LO,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_INNER_L4_PORT_XOR_LOs,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_OUTER_L4_PORT_LSB32_HI,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_OUTER_L4_PORT_LSB32_HIs,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_OUTER_L4_PORT_LSB32_LO,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_OUTER_L4_PORT_LSB32_LOs,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_INNER_L4_PORT_LSB32_HI,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_INNER_L4_PORT_LSB32_HIs,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_INNER_L4_PORT_LSB32_LO,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_INNER_L4_PORT_LSB32_LOs,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_SEED_A_0_15,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_SEED_A_0_15s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_SEED_A_16_31,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_SEED_A_16_31s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_SEED_B_0_15,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_SEED_B_0_15s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_SEED_B_16_31,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_SEED_B_16_31s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_SEED_C_0_15,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_SEED_C_0_15s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_NORMALIZED_SEED_C_16_31,
        .out.s = FLEX_DIGEST_NORMALIZED_FIELD_BUS_SEED_C_16_31s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_UDF_TWO_BYTE_0,
        .out.s = IPARSER1_CONTAINER_2_BYTE_0s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_UDF_TWO_BYTE_1,
        .out.s = IPARSER1_CONTAINER_2_BYTE_1s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_UDF_TWO_BYTE_2,
        .out.s = IPARSER1_CONTAINER_2_BYTE_2s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_UDF_TWO_BYTE_3,
        .out.s = IPARSER1_CONTAINER_2_BYTE_3s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_UDF_TWO_BYTE_4,
        .out.s = IPARSER1_CONTAINER_2_BYTE_4s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_UDF_TWO_BYTE_5,
        .out.s = IPARSER1_CONTAINER_2_BYTE_5s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_UDF_ONE_BYTE_0,
        .out.s = IPARSER1_CONTAINER_1_BYTE_0s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_UDF_ONE_BYTE_1,
        .out.s = IPARSER1_CONTAINER_1_BYTE_1s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_UDF_ONE_BYTE_2,
        .out.s = IPARSER1_CONTAINER_1_BYTE_2s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_UDF_ONE_BYTE_3,
        .out.s = IPARSER1_CONTAINER_1_BYTE_3s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_OUTER_UDF_ONE_BYTE_4,
        .out.s = IPARSER1_CONTAINER_1_BYTE_4s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_UDF_TWO_BYTE_0,
        .out.s = IPARSER2_CONTAINER_2_BYTE_0s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_UDF_TWO_BYTE_1,
        .out.s = IPARSER2_CONTAINER_2_BYTE_1s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_UDF_TWO_BYTE_2,
        .out.s = IPARSER2_CONTAINER_2_BYTE_2s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_UDF_TWO_BYTE_3,
        .out.s = IPARSER2_CONTAINER_2_BYTE_3s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_UDF_TWO_BYTE_4,
        .out.s = IPARSER2_CONTAINER_2_BYTE_4s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_UDF_TWO_BYTE_5,
        .out.s = IPARSER2_CONTAINER_2_BYTE_5s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_UDF_ONE_BYTE_0,
        .out.s = IPARSER2_CONTAINER_1_BYTE_0s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_UDF_ONE_BYTE_1,
        .out.s = IPARSER2_CONTAINER_1_BYTE_1s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_UDF_ONE_BYTE_2,
        .out.s = IPARSER2_CONTAINER_1_BYTE_2s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_UDF_ONE_BYTE_3,
        .out.s = IPARSER2_CONTAINER_1_BYTE_3s,
    },
    {
        .in = BCM_FLEXDIGEST_FIELD_INNER_UDF_ONE_BYTE_4,
        .out.s = IPARSER2_CONTAINER_1_BYTE_4s,
    },
};

static bcmgene_field_value_converter_t bcmgene_flexdigest_lkup_fld_val = {
    .flags = 0,
    .maps = COUNTOF(bcmgene_flexdigest_lkup_fld_val_map),
    .map = bcmgene_flexdigest_lkup_fld_val_map,
};


static bcmgene_field_desc_t bcmgene_flexdigest_lkup_fld[] = {
    {
        .flags = 0,
        .blk = 0,
        .offset = 0,
        .width = 0,
        .depth = 0,
        .field = ENTRY_PRIORITYs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchSysHdrEpNih,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_SYS_HDR_EP_NIHs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchSysHdrLoopBack,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_SYS_HDR_LOOPBACKs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL2HdrVntag,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L2_HDR_VNTAGs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL2HdrSnapOrLlc,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L2_HDR_SNAP_OR_LLCs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL2HdrSvtag,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L2_HDR_SVTAGs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL2HdrOtag,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L2_HDR_OTAGs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL2HdrL2,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L2_HDR_L2s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL2HdrItag,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L2_HDR_ITAGs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL2HdrHg3Ext0,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L2_HDR_HG3_EXT_0s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL2HdrHg3Base,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L2_HDR_HG3_BASEs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL2HdrEtag,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L2_HDR_ETAGs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrVxlan,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_VXLANs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrUnknownL5,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_UNKNOWN_L5s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrUnknownL4,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_UNKNOWN_L4s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrUnknownL3,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_UNKNOWN_L3s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrUdp,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_UDPs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrTcpLast16Bytes,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_TCP_LAST_16BYTESs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrTcpFirst4Bytes,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_TCP_FIRST_4BYTESs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrRarp,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_RARPs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrP1588,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_P_1588s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrMplsCw,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_MPLS_CWs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrMplsAch,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_MPLS_ACHs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrMpls6,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_MPLS6s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrMpls5,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_MPLS5s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrMpls4,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_MPLS4s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrMpls3,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_MPLS3s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrMpls2,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_MPLS2s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrMpls1,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_MPLS1s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrMpls0,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_MPLS0s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrIpv6,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_IPV6s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrIpv4,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_IPV4s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrIgmp,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_IGMPs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrIcmp,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_ICMPs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrGreSeq,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_GRE_SEQs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrGreRout,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_GRE_ROUTs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrGreKey,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_GRE_KEYs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrGreChksum,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_GRE_CHKSUMs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrGre,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_GREs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrGpe,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_GPEs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrFragExt2,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_FRAG_EXT_2s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrFragExt1,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_FRAG_EXT_1s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrEtherType,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_ETHERTYPEs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrEspExt,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_ESP_EXTs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrBfd,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_BFDs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrAuthExt2,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_AUTH_EXT_2s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrAuthExt1,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_AUTH_EXT_1s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchOuterL3L4HdrArp,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_OUTER_L3_L4_HDR_ARPs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchInnerL2HdrVntag,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_INNER_L2_HDR_VNTAGs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchInnerL2HdrSnapOrLlc,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_INNER_L2_HDR_SNAP_OR_LLCs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchInnerL2HdrOtag,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_INNER_L2_HDR_OTAGs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchInnerL2HdrL2,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_INNER_L2_HDR_L2s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchInnerL2HdrItag,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_INNER_L2_HDR_ITAGs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchInnerL2HdrEtag,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_INNER_L2_HDR_ETAGs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchInnerL3L4HdrUnknownL5,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_INNER_L3_L4_HDR_UNKNOWN_L5s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchInnerL3L4HdrUnknownL4,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_INNER_L3_L4_HDR_UNKNOWN_L4s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchInnerL3L4HdrUnknownL3,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_INNER_L3_L4_HDR_UNKNOWN_L3s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchInnerL3L4HdrUdp,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_INNER_L3_L4_HDR_UDPs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchInnerL3L4HdrTcpLast16Bytes,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_INNER_L3_L4_HDR_TCP_LAST_16BYTESs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchInnerL3L4HdrTcpFirst4Bytes,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_INNER_L3_L4_HDR_TCP_FIRST_4BYTESs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchInnerL3L4HdrRarp,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_INNER_L3_L4_HDR_RARPs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchInnerL3L4HdrP1588,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_INNER_L3_L4_HDR_P_1588s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchInnerL3L4HdrIpv6,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_INNER_L3_L4_HDR_IPV6s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchInnerL3L4HdrIpv4,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_INNER_L3_L4_HDR_IPV4s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchInnerL3L4HdrIgmp,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_INNER_L3_L4_HDR_IGMPs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchInnerL3L4HdrIcmp,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_INNER_L3_L4_HDR_ICMPs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchInnerL3L4HdrFragExt2,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_INNER_L3_L4_HDR_FRAG_EXT_2s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchInnerL3L4HdrFragExt1,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_INNER_L3_L4_HDR_FRAG_EXT_1s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchInnerL3L4HdrEtherType,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_INNER_L3_L4_HDR_ETHERTYPEs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchInnerL3L4HdrEspExt,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_INNER_L3_L4_HDR_ESP_EXTs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchInnerL3L4HdrBfd,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_INNER_L3_L4_HDR_BFDs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchInnerL3L4HdrAuthExt2,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_INNER_L3_L4_HDR_AUTH_EXT_2s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchInnerL3L4HdrAuthExt1,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_INNER_L3_L4_HDR_AUTH_EXT_1s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 1,
        .offset = bcmFlexDigestMatchInnerL3L4HdrArp,
        .width = 0,
        .depth = 0,
        .field = INGRESS_PKT_INNER_L3_L4_HDR_ARPs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 2,
        .offset = bcmFlexDigestQualifyL4Valid,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_L4_VALIDs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 2,
        .offset = bcmFlexDigestQualifyMyStation1Hit,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_MY_STATION_1_HITs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 2,
        .offset = bcmFlexDigestQualifyTunnelProcessingResults1,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_TUNNEL_PROCESSING_RESULTS_1s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 2,
        .offset = bcmFlexDigestQualifyTunnelProcessingResultsRaw,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_TUNNEL_PROCESSING_RESULTS_RAWs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 2,
        .offset = bcmFlexDigestQualifyFlexHve2ResultSet1,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_FLEX_HVE2_RESULT_SET_1s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 2,
        .offset = bcmFlexDigestQualifyFixedHve2ResultSet0,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_FIXED_HVE2_RESULT_SET_0s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 2,
        .offset = bcmFlexDigestQualifyFlexHve1ResultSet1,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_FLEX_HVE1_RESULT_SET_1s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 2,
        .offset = bcmFlexDigestQualifyFixedHve1ResultSet0,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_FIXED_HVE1_RESULT_SET_0s,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 2,
        .offset = bcmFlexDigestQualifyL2IifFlexDigestCtrlIdB,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_L2_IIF_FLEX_DIGEST_CTRL_ID_Bs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 2,
        .offset = bcmFlexDigestQualifyL2IifFlexDigestCtrlIdA,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_L2_IIF_FLEX_DIGEST_CTRL_ID_As,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 2,
        .offset = bcmFlexDigestQualifyProtocolPktOpaqueCtrlId,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_PROTOCOL_PKT_OPAQUE_CTRL_IDs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 2,
        .offset = bcmFlexDigestQualifyVfiOpaqueCtrlId,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_VFI_OPAQUE_CTRL_IDs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 2,
        .offset = bcmFlexDigestQualifySvpOpaqueCtrlId,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_SVP_OPAQUE_CTRL_IDs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 2,
        .offset = bcmFlexDigestQualifyL3IifOpaqueCtrlId,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_L3_IIF_OPAQUE_CTRL_IDs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 2,
        .offset = bcmFlexDigestQualifyL2IifOpaqueCtrlId,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_L2_IIF_OPAQUE_CTRL_IDs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 3,
        .offset = bcmFlexDigestQualifyL4Valid,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_L4_VALID_MASKs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 3,
        .offset = bcmFlexDigestQualifyMyStation1Hit,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_MY_STATION_1_HIT_MASKs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 3,
        .offset = bcmFlexDigestQualifyTunnelProcessingResults1,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_TUNNEL_PROCESSING_RESULTS_1_MASKs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 3,
        .offset = bcmFlexDigestQualifyTunnelProcessingResultsRaw,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_TUNNEL_PROCESSING_RESULTS_RAW_MASKs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 3,
        .offset = bcmFlexDigestQualifyFlexHve2ResultSet1,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_FLEX_HVE2_RESULT_SET_1_MASKs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 3,
        .offset = bcmFlexDigestQualifyFixedHve2ResultSet0,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_FIXED_HVE2_RESULT_SET_0_MASKs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 3,
        .offset = bcmFlexDigestQualifyFlexHve1ResultSet1,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_FLEX_HVE1_RESULT_SET_1_MASKs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 3,
        .offset = bcmFlexDigestQualifyFixedHve1ResultSet0,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_FIXED_HVE1_RESULT_SET_0_MASKs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 3,
        .offset = bcmFlexDigestQualifyL2IifFlexDigestCtrlIdB,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_L2_IIF_FLEX_DIGEST_CTRL_ID_B_MASKs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 3,
        .offset = bcmFlexDigestQualifyL2IifFlexDigestCtrlIdA,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_L2_IIF_FLEX_DIGEST_CTRL_ID_A_MASKs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 3,
        .offset = bcmFlexDigestQualifyProtocolPktOpaqueCtrlId,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_PROTOCOL_PKT_OPAQUE_CTRL_ID_MASKs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 3,
        .offset = bcmFlexDigestQualifyVfiOpaqueCtrlId,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_VFI_OPAQUE_CTRL_ID_MASKs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 3,
        .offset = bcmFlexDigestQualifySvpOpaqueCtrlId,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_SVP_OPAQUE_CTRL_ID_MASKs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 3,
        .offset = bcmFlexDigestQualifyL3IifOpaqueCtrlId,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_L3_IIF_OPAQUE_CTRL_ID_MASKs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 3,
        .offset = bcmFlexDigestQualifyL2IifOpaqueCtrlId,
        .width = 0,
        .depth = 0,
        .field = ING_CMD_BUS_L2_IIF_OPAQUE_CTRL_ID_MASKs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 4,
        .offset = bcmFlexDigestActionExtractBinSetACmds0,
        .width = 0,
        .depth = 16,
        .field = BIN_As,
        .converter = &bcmgene_flexdigest_lkup_fld_val,
    },
    {
        .flags = 0,
        .blk = 5,
        .offset = bcmFlexDigestActionExtractBinSetACmds0,
        .width = 0,
        .depth = 16,
        .field = BIN_A_FLEX_DIGEST_LKUP_MASK_PROFILE_IDs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 6,
        .offset = bcmFlexDigestActionExtractBinSetBCmds0,
        .width = 0,
        .depth = 16,
        .field = BIN_Bs,
        .converter = &bcmgene_flexdigest_lkup_fld_val,
    },
    {
        .flags = 0,
        .blk = 7,
        .offset = bcmFlexDigestActionExtractBinSetBCmds0,
        .width = 0,
        .depth = 16,
        .field = BIN_B_FLEX_DIGEST_LKUP_MASK_PROFILE_IDs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 8,
        .offset = bcmFlexDigestActionExtractBinSetCCmds0,
        .width = 0,
        .depth = 0,
        .field = BIN_C_MASKs,
        .converter = NULL,
    },
};

static bcmgene_field_desc_t bcmgene_flexdigest_lkup_mask_profile_key[] = {
    {
        .flags = 0,
        .blk = 0,
        .offset = 0,
        .width = 0,
        .depth = 0,
        .field = FLEX_DIGEST_LKUP_MASK_PROFILE_IDs,
        .converter = NULL,
    },
};

static bcmgene_field_desc_t bcmgene_flexdigest_lkup_mask_profile_fld[] = {
    {
        .flags = 0,
        .blk = 0,
        .offset = 0,
        .width = 0,
        .depth = 0,
        .field = MASKs,
        .converter = NULL,
    },
};

static bcmgene_table_desc_t bcmgene_flexdigest_lkup[] = {
    {
        .sub_f = BCMGENE_FLEXDIGEST_SUB_F_LKUP_GROUP | BCMGENE_FLEXDIGEST_SUB_F_LKUP_PRESEL,
        .flags = 0,
        .table = FLEX_DIGEST_LKUP_PRESELs,
        .idx_min = 0,
        .idx_max = 0,
        .keys = 0,
        .key = NULL,
        .flds = COUNTOF(bcmgene_flexdigest_lkup_presel_fld),
        .fld = bcmgene_flexdigest_lkup_presel_fld,
    },
    {
        .sub_f = BCMGENE_FLEXDIGEST_SUB_F_LKUP_GROUP | BCMGENE_FLEXDIGEST_SUB_F_LKUP_ENTRY | BCMGENE_FLEXDIGEST_SUB_F_LKUP_OPAQUE_KEY | BCMGENE_FLEXDIGEST_SUB_F_LKUP_MATCH_ID | BCMGENE_FLEXDIGEST_SUB_F_LKUP_POLICY,
        .flags = 0,
        .table = FLEX_DIGEST_LKUPs,
        .idx_min = 0,
        .idx_max = 0,
        .keys = COUNTOF(bcmgene_flexdigest_lkup_key),
        .key = bcmgene_flexdigest_lkup_key,
        .flds = COUNTOF(bcmgene_flexdigest_lkup_fld),
        .fld = bcmgene_flexdigest_lkup_fld,
    },
    {
        .sub_f = BCMGENE_FLEXDIGEST_SUB_F_LKUP_GROUP | BCMGENE_FLEXDIGEST_SUB_F_LKUP_MASK_PROFILE,
        .flags = 0,
        .table = FLEX_DIGEST_LKUP_MASK_PROFILEs,
        .idx_min = 0,
        .idx_max = 0,
        .keys = COUNTOF(bcmgene_flexdigest_lkup_mask_profile_key),
        .key = bcmgene_flexdigest_lkup_mask_profile_key,
        .flds = COUNTOF(bcmgene_flexdigest_lkup_mask_profile_fld),
        .fld = bcmgene_flexdigest_lkup_mask_profile_fld,
    },
};

static bcmgene_field_desc_t bcmgene_flexdigest_hash_profile_key[] = {
    {
        .flags = 0,
        .blk = 0,
        .offset = 0,
        .width = 0,
        .depth = 0,
        .field = FLEX_DIGEST_HASH_PROFILE_IDs,
        .converter = NULL,
    },
};

static bcmgene_field_value_map_t bcmgene_flexdigest_hash_profile_fld_val_map[] = {
    {
        .in = 0,
        .out.s = RESERVEDs,
    },
    {
        .in = BCM_FLEXDIGEST_HASH_FUNCTION_CRC16,
        .out.s = CRC16_BISYNCs,
    },
    {
        .in = BCM_FLEXDIGEST_HASH_FUNCTION_CRC16XOR1,
        .out.s = CRC16_BISYNC_AND_XOR1s,
    },
    {
        .in = BCM_FLEXDIGEST_HASH_FUNCTION_CRC16XOR2,
        .out.s = CRC16_BISYNC_AND_XOR2s,
    },
    {
        .in = BCM_FLEXDIGEST_HASH_FUNCTION_CRC16XOR4,
        .out.s = CRC16_BISYNC_AND_XOR4s,
    },
    {
        .in = BCM_FLEXDIGEST_HASH_FUNCTION_CRC16XOR8,
        .out.s = CRC16_BISYNC_AND_XOR8s,
    },
    {
        .in = BCM_FLEXDIGEST_HASH_FUNCTION_XOR16,
        .out.s = XOR16s,
    },
    {
        .in = BCM_FLEXDIGEST_HASH_FUNCTION_CRC16CCITT,
        .out.s = CRC16_CCITTs,
    },
    {
        .in = BCM_FLEXDIGEST_HASH_FUNCTION_CRC32LO,
        .out.s = CRC32_LOs,
    },
    {
        .in = BCM_FLEXDIGEST_HASH_FUNCTION_CRC32HI,
        .out.s = CRC32_HIs,
    },
    {
        .in = BCM_FLEXDIGEST_HASH_FUNCTION_CRC32_ETH_LO,
        .out.s = CRC32_ETH_LOs,
    },
    {
        .in = BCM_FLEXDIGEST_HASH_FUNCTION_CRC32_ETH_HI,
        .out.s = CRC32_ETH_HIs,
    },
    {
        .in = BCM_FLEXDIGEST_HASH_FUNCTION_KOOPMAN_LO,
        .out.s = CRC32_KOOPMAN_LOs,
    },
    {
        .in = BCM_FLEXDIGEST_HASH_FUNCTION_KOOPMAN_HI,
        .out.s = CRC32_KOOPMAN_HIs,
    },
    {
        .in = BCM_FLEXDIGEST_HASH_FUNCTION_VERSATILE_HASH_0,
        .out.s = VERSATILE_HASH_0s,
    },
    {
        .in = BCM_FLEXDIGEST_HASH_FUNCTION_VERSATILE_HASH_1,
        .out.s = VERSATILE_HASH_1s,
    },
};

static bcmgene_field_value_converter_t bcmgene_flexdigest_hash_profile_fld_val = {
    .flags = 0,
    .maps = COUNTOF(bcmgene_flexdigest_hash_profile_fld_val_map),
    .map = bcmgene_flexdigest_hash_profile_fld_val_map,
};

static bcmgene_field_desc_t bcmgene_flexdigest_hash_profile_fld[] = {
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestHashProfileControlPreProcessBinA,
        .width = 0,
        .depth = 0,
        .field = PRE_PROCESSING_BIN_As,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestHashProfileControlPreProcessBinB,
        .width = 0,
        .depth = 0,
        .field = PRE_PROCESSING_BIN_Bs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestHashProfileControlPreProcessBinC,
        .width = 0,
        .depth = 0,
        .field = PRE_PROCESSING_BIN_Cs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestHashProfileControlXorSaltBinA,
        .depth = 0,
        .field = XOR_SALT_BIN_As,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestHashProfileControlXorSaltBinB,
        .width = 0,
        .depth = 0,
        .field = XOR_SALT_BIN_Bs,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestHashProfileControlXorSaltBinC,
        .width = 0,
        .depth = 0,
        .field = XOR_SALT_BIN_Cs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestHashProfileControlBinA0FunctionSelection,
        .width = 0,
        .depth = 0,
        .field = BIN_A_0_FUNCTION_SELECTIONs,
        .converter = &bcmgene_flexdigest_hash_profile_fld_val,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestHashProfileControlBinA1FunctionSelection,
        .width = 0,
        .depth = 0,
        .field = BIN_A_1_FUNCTION_SELECTIONs,
        .converter = &bcmgene_flexdigest_hash_profile_fld_val,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestHashProfileControlBinB0FunctionSelection,
        .width = 0,
        .depth = 0,
        .field = BIN_B_0_FUNCTION_SELECTIONs,
        .converter = &bcmgene_flexdigest_hash_profile_fld_val,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestHashProfileControlBinB1FunctionSelection,
        .width = 0,
        .depth = 0,
        .field = BIN_B_1_FUNCTION_SELECTIONs,
        .converter = &bcmgene_flexdigest_hash_profile_fld_val,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestHashProfileControlBinC0FunctionSelection,
        .width = 0,
        .depth = 0,
        .field = BIN_C_0_FUNCTION_SELECTIONs,
        .converter = &bcmgene_flexdigest_hash_profile_fld_val,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestHashProfileControlBinC1FunctionSelection,
        .width = 0,
        .depth = 0,
        .field = BIN_C_1_FUNCTION_SELECTIONs,
        .converter = &bcmgene_flexdigest_hash_profile_fld_val,
    },
};

static bcmgene_field_desc_t bcmgene_flexdigest_hash_salt_fld[] = {
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestHashBinA,
        .width = 0,
        .depth = 16,
        .field = BIN_As,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestHashBinB,
        .width = 0,
        .depth = 16,
        .field = BIN_Bs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestHashBinC,
        .width = 0,
        .depth = 16,
        .field = BIN_Cs,
        .converter = NULL,
    },
};

static bcmgene_table_desc_t bcmgene_flexdigest_hash[] = {
    {
        .sub_f = BCMGENE_FLEXDIGEST_SUB_F_HASH_PROFILE | BCMGENE_FLEXDIGEST_SUB_F_HASH_PRE_PROCESSING | BCMGENE_FLEXDIGEST_SUB_F_HASH_FUNCTION,
        .flags = 0,
        .table = FLEX_DIGEST_HASH_PROFILEs,
        .idx_min = 0,
        .idx_max = 0,
        .keys = COUNTOF(bcmgene_flexdigest_hash_profile_key),
        .key = bcmgene_flexdigest_hash_profile_key,
        .flds = COUNTOF(bcmgene_flexdigest_hash_profile_fld),
        .fld = bcmgene_flexdigest_hash_profile_fld,
    },
    {
        .sub_f = BCMGENE_FLEXDIGEST_SUB_F_HASH_SALT,
        .flags = 0,
        .table = FLEX_DIGEST_HASH_SALTs,
        .idx_min = 0,
        .idx_max = 0,
        .keys = 0,
        .key = NULL,
        .flds = COUNTOF(bcmgene_flexdigest_hash_salt_fld),
        .fld = bcmgene_flexdigest_hash_salt_fld,
    },
};

static bcmgene_field_desc_t bcmgene_flexdigest_global_fld[] = {
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestControlHashVersatileRandomSeedBinA,
        .width = 0,
        .depth = 0,
        .field = VERSATILE_HASH_RANDOM_SEED_As,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestControlHashVersatileRandomSeedBinB,
        .width = 0,
        .depth = 0,
        .field = VERSATILE_HASH_RANDOM_SEED_Bs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmFlexDigestControlHashVersatileRandomSeedBinC,
        .width = 0,
        .depth = 0,
        .field = VERSATILE_HASH_RANDOM_SEED_Cs,
        .converter = NULL,
    },
};

static bcmgene_table_desc_t bcmgene_flexdigest_global[] = {
    {
        .sub_f = BCMGENE_FLEXDIGEST_SUB_F_GLOBAL_VERSATILE_HASH_SEED,
        .flags = 0,
        .table = FLEX_DIGEST_CONTROLs,
        .idx_min = 0,
        .idx_max = 0,
        .keys = 0,
        .key = NULL,
        .flds = COUNTOF(bcmgene_flexdigest_global_fld),
        .fld = bcmgene_flexdigest_global_fld,
    },
};

static bcmgene_func_desc_t bcmgene_func_flexdigest[] = {
    {
        .func = BCMGENE_FLEXDIGEST_F_NORM,
        .tbls = COUNTOF(bcmgene_flexdigest_norm),
        .tbl = bcmgene_flexdigest_norm,
    },
    {
        .func = BCMGENE_FLEXDIGEST_F_LKUP,
        .tbls = COUNTOF(bcmgene_flexdigest_lkup),
        .tbl = bcmgene_flexdigest_lkup,
    },
    {
        .func = BCMGENE_FLEXDIGEST_F_HASH,
        .tbls = COUNTOF(bcmgene_flexdigest_hash),
        .tbl = bcmgene_flexdigest_hash,
    },
    {
        .func = BCMGENE_FLEXDIGEST_F_GLOBAL,
        .tbls = COUNTOF(bcmgene_flexdigest_global),
        .tbl = bcmgene_flexdigest_global,
    },
};

static bcmgene_field_value_map_t bcmgene_hash_output_selection_profile_key_val_map[] = {
    {
        .in = 0,
        .out.s = BASE_VIEWs,
    },
};

static bcmgene_field_value_converter_t bcmgene_hash_output_selection_profile_key_val = {
    .flags = 0,
    .maps = COUNTOF(bcmgene_hash_output_selection_profile_key_val_map),
    .map = bcmgene_hash_output_selection_profile_key_val_map,
};

static bcmgene_field_value_map_t bcmgene_hash_output_selection_profile_ecmp_key_val_map[] = {
    {
        .in = 0,
        .out.s = PROFILE_0s,
    },
    {
        .in = 1,
        .out.s = PROFILE_1s,
    },
    {
        .in = 2,
        .out.s = PROFILE_2s,
    },
    {
        .in = 3,
        .out.s = PROFILE_3s,
    },
};

static bcmgene_field_value_converter_t bcmgene_hash_output_selection_profile_ecmp_key_val = {
    .flags = 0,
    .maps = COUNTOF(bcmgene_hash_output_selection_profile_ecmp_key_val_map),
    .map = bcmgene_hash_output_selection_profile_ecmp_key_val_map,
};

static bcmgene_field_desc_t bcmgene_hash_output_selection_profile_trunk_key[] = {
    {
        .flags = 0,
        .blk = 0,
        .offset = 0,
        .width = 2,
        .depth = 0,
        .field = TRUNK_HASH_OUTPUT_SELECTION_PROFILE_IDs,
        .converter = &bcmgene_hash_output_selection_profile_key_val,
    },
};

static bcmgene_field_desc_t bcmgene_hash_output_selection_profile_ecmp_key[] = {
    {
        .flags = 0,
        .blk = 0,
        .offset = 0,
        .width = 2,
        .depth = 0,
        .field = PROFILE_IDs,
        .converter = &bcmgene_hash_output_selection_profile_ecmp_key_val,
    },
};

static bcmgene_field_desc_t bcmgene_hash_output_selection_profile_dlb_trunk_key[] = {
    {
        .flags = 0,
        .blk = 0,
        .offset = 0,
        .width = 2,
        .depth = 0,
        .field = DLB_TRUNK_HASH_OUTPUT_SELECTION_PROFILE_IDs,
        .converter = &bcmgene_hash_output_selection_profile_key_val,
    },
};

static bcmgene_field_desc_t bcmgene_hash_output_selection_profile_entropy_key[] = {
    {
        .flags = 0,
        .blk = 0,
        .offset = 0,
        .width = 2,
        .depth = 0,
        .field = PROFILE_IDs,
        .converter = &bcmgene_hash_output_selection_profile_key_val,
    },
};

static bcmgene_field_value_map_t bcmgene_hash_output_selection_profile_fld_flow_id_src_val_map[] = {
    {
        .in = bcmiHosFlowIdSrcHashA0Lo,
        .out.s = HASH_A0_LOs,
    },
    {
        .in = bcmiHosFlowIdSrcHashA0Hi,
        .out.s = HASH_A0_HIs,
    },
    {
        .in = bcmiHosFlowIdSrcHashA1Lo,
        .out.s = HASH_A1_LOs,
    },
    {
        .in = bcmiHosFlowIdSrcHashA1Hi,
        .out.s = HASH_A1_HIs,
    },
    {
        .in = bcmiHosFlowIdSrcHashB0Lo,
        .out.s = HASH_B0_LOs,
    },
    {
        .in = bcmiHosFlowIdSrcHashB0Hi,
        .out.s = HASH_B0_HIs,
    },
    {
        .in = bcmiHosFlowIdSrcHashB1Lo,
        .out.s = HASH_B1_LOs,
    },
    {
        .in = bcmiHosFlowIdSrcHashB1Hi,
        .out.s = HASH_B1_HIs,
    },
    {
        .in = bcmiHosFlowIdSrcHashC0Lo,
        .out.s = HASH_C0_LOs,
    },
    {
        .in = bcmiHosFlowIdSrcHashC0Hi,
        .out.s = HASH_C0_HIs,
    },
    {
        .in = bcmiHosFlowIdSrcHashC1Lo,
        .out.s = HASH_C1_LOs,
    },
    {
        .in = bcmiHosFlowIdSrcHashC1Hi,
        .out.s = HASH_C1_HIs,
    },
    {
        .in = bcmiHosFlowIdSrcBusHashIndex,
        .out.s = BUS_HASH_INDEXs,
    },
};

static bcmgene_field_value_converter_t bcmgene_hash_output_selection_profile_fld_flow_id_src_val = {
    .flags = 0,
    .maps = COUNTOF(bcmgene_hash_output_selection_profile_fld_flow_id_src_val_map),
    .map = bcmgene_hash_output_selection_profile_fld_flow_id_src_val_map,
};

static bcmgene_field_value_map_t bcmgene_hash_output_selection_profile_fld_ecmp_member_idx_val_map[] = {
    {
        .in = bcmiHosEcmpMemberIndexLocallyGenerated,
        .out.s = LOCALLY_GENERATEDs,
    },
    {
        .in = bcmiHosEcmpMemberIndexInputBus,
        .out.s = INPUT_BUSs,
    },
};

static bcmgene_field_value_converter_t bcmgene_hash_output_selection_profile_fld_ecmp_member_idx_val = {
    .flags = 0,
    .maps = COUNTOF(bcmgene_hash_output_selection_profile_fld_ecmp_member_idx_val_map),
    .map = bcmgene_hash_output_selection_profile_fld_ecmp_member_idx_val_map,
};

static bcmgene_field_desc_t bcmgene_hash_output_selection_profile_trunk_fld[] = {
    {
        .flags = 0,
        .blk = bcmiHosTypeTrunkUc,
        .offset = bcmiHosProfileControlInstance,
        .width = 0,
        .depth = 0,
        .field = HASH_INSTANCE_UCs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = bcmiHosTypeTrunkUc,
        .offset = bcmiHosProfileControlFlowIdSrc,
        .width = 0,
        .depth = 0,
        .field = HASH_FLOW_ID_SRC_UCs,
        .converter = &bcmgene_hash_output_selection_profile_fld_flow_id_src_val,
    },
    {
        .flags = 0,
        .blk = bcmiHosTypeTrunkNonUc,
        .offset = bcmiHosProfileControlInstance,
        .width = 0,
        .depth = 0,
        .field = HASH_INSTANCE_NONUCs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = bcmiHosTypeTrunkNonUc,
        .offset = bcmiHosProfileControlFlowIdSrc,
        .width = 0,
        .depth = 0,
        .field = HASH_FLOW_ID_SRC_NONUCs,
        .converter = &bcmgene_hash_output_selection_profile_fld_flow_id_src_val,
    },
    {
        .flags = 0,
        .blk = bcmiHosTypeTrunkNonUc,
        .offset = bcmiHosProfileControlUseHigig3Entropy,
        .width = 0,
        .depth = 0,
        .field = HASH_USE_HIGIG3_ENTROPY_NONUCs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = bcmiHosTypeTrunkFailover,
        .offset = bcmiHosProfileControlInstance,
        .width = 0,
        .depth = 0,
        .field = HASH_INSTANCE_FAILOVERs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = bcmiHosTypeTrunkFailover,
        .offset = bcmiHosProfileControlFlowIdSrc,
        .width = 0,
        .depth = 0,
        .field = HASH_FLOW_ID_SRC_FAILOVERs,
        .converter = &bcmgene_hash_output_selection_profile_fld_flow_id_src_val,
    },
    {
        .flags = 0,
        .blk = bcmiHosTypeTrunkSystem,
        .offset = bcmiHosProfileControlInstance,
        .width = 0,
        .depth = 0,
        .field = HASH_INSTANCE_SYSTEMs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = bcmiHosTypeTrunkSystem,
        .offset = bcmiHosProfileControlFlowIdSrc,
        .width = 0,
        .depth = 0,
        .field = HASH_FLOW_ID_SRC_SYSTEMs,
        .converter = &bcmgene_hash_output_selection_profile_fld_flow_id_src_val,
    },
    {
        .flags = 0,
        .blk = bcmiHosTypeTrunkSystemFailover,
        .offset = bcmiHosProfileControlInstance,
        .width = 0,
        .depth = 0,
        .field = HASH_INSTANCE_SYSTEM_FAILOVERs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = bcmiHosTypeTrunkSystemFailover,
        .offset = bcmiHosProfileControlFlowIdSrc,
        .width = 0,
        .depth = 0,
        .field = HASH_FLOW_ID_SRC_SYSTEM_FAILOVERs,
        .converter = &bcmgene_hash_output_selection_profile_fld_flow_id_src_val,
    },
};

static bcmgene_field_desc_t bcmgene_hash_output_selection_profile_ecmp_level0_fld[] = {
    {
        .flags = 0,
        .blk = bcmiHosTypeTrunkVp,
        .offset = bcmiHosProfileControlInstance,
        .width = 0,
        .depth = 0,
        .field = HASH_TABLE_INSTANCEs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = bcmiHosTypeTrunkVp,
        .offset = bcmiHosProfileControlFlowIdSrc,
        .width = 0,
        .depth = 0,
        .field = HASH_FLOW_ID_SRCs,
        .converter = &bcmgene_hash_output_selection_profile_fld_flow_id_src_val,
    },
    {
        .flags = 0,
        .blk = bcmiHosTypeEcmpOverlay,
        .offset = bcmiHosProfileControlInstance,
        .width = 0,
        .depth = 0,
        .field = HASH_TABLE_INSTANCEs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = bcmiHosTypeEcmpOverlay,
        .offset = bcmiHosProfileControlFlowIdSrc,
        .width = 0,
        .depth = 0,
        .field = HASH_FLOW_ID_SRCs,
        .converter = &bcmgene_hash_output_selection_profile_fld_flow_id_src_val,
    },
    {
        .flags = 0,
        .blk = bcmiHosTypeEcmpOverlay,
        .offset = bcmiHosProfileControlMemberIndex,
        .width = 0,
        .depth = 0,
        .field = MEMBER_INDEXs,
        .converter = &bcmgene_hash_output_selection_profile_fld_ecmp_member_idx_val,
    },
    {
        .flags = 0,
        .blk = bcmiHosTypeDlbEcmp,
        .offset = bcmiHosProfileControlInstance,
        .width = 0,
        .depth = 0,
        .field = HASH_TABLE_INSTANCEs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = bcmiHosTypeDlbEcmp,
        .offset = bcmiHosProfileControlFlowIdSrc,
        .width = 0,
        .depth = 0,
        .field = HASH_FLOW_ID_SRCs,
        .converter = &bcmgene_hash_output_selection_profile_fld_flow_id_src_val,
    },
};

static bcmgene_field_desc_t bcmgene_hash_output_selection_profile_ecmp_level1_fld[] = {
    {
        .flags = 0,
        .blk = bcmiHosTypeEcmpUnderlay,
        .offset = bcmiHosProfileControlInstance,
        .width = 0,
        .depth = 0,
        .field = HASH_TABLE_INSTANCEs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = bcmiHosTypeEcmpUnderlay,
        .offset = bcmiHosProfileControlFlowIdSrc,
        .width = 0,
        .depth = 0,
        .field = HASH_FLOW_ID_SRCs,
        .converter = &bcmgene_hash_output_selection_profile_fld_flow_id_src_val,
    },
    {
        .flags = 0,
        .blk = bcmiHosTypeEcmpUnderlay,
        .offset = bcmiHosProfileControlWEcmp,
        .width = 0,
        .depth = 0,
        .field = WECMPs,
        .converter = NULL,
    },
};

static bcmgene_field_desc_t bcmgene_hash_output_selection_profile_dlb_trunk_fld[] = {
    {
        .flags = 0,
        .blk = bcmiHosTypeDlbTrunk,
        .offset = bcmiHosProfileControlInstance,
        .width = 0,
        .depth = 0,
        .field = HASH_INSTANCEs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = bcmiHosTypeDlbTrunk,
        .offset = bcmiHosProfileControlFlowIdSrc,
        .width = 0,
        .depth = 0,
        .field = HASH_FLOW_ID_SRCs,
        .converter = &bcmgene_hash_output_selection_profile_fld_flow_id_src_val,
    },
};

static bcmgene_field_desc_t bcmgene_hash_output_selection_profile_entropy_fld[] = {
    {
        .flags = 0,
        .blk = bcmiHosTypeEntropy,
        .offset = bcmiHosProfileControlInstance,
        .width = 0,
        .depth = 0,
        .field = HASH_INSTANCEs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = bcmiHosTypeEntropy,
        .offset = bcmiHosProfileControlFlowIdSrc,
        .width = 0,
        .depth = 0,
        .field = HASH_FLOW_ID_SRCs,
        .converter = &bcmgene_hash_output_selection_profile_fld_flow_id_src_val,
    },
};

static bcmgene_table_desc_t bcmgene_hash_output_selection_profile[] = {
    {
        .sub_f = BCMGENE_HASH_OUTPUT_SELECTION_SUB_F_TRUNK_UC | \
                 BCMGENE_HASH_OUTPUT_SELECTION_SUB_F_TRUNK_NONUC | \
                 BCMGENE_HASH_OUTPUT_SELECTION_SUB_F_TRUNK_FAILOVER | \
                 BCMGENE_HASH_OUTPUT_SELECTION_SUB_F_TRUNK_SYSTEM | \
                 BCMGENE_HASH_OUTPUT_SELECTION_SUB_F_TRUNK_SYSTEM_FAILOVER,
        .flags = 0,
        .table = TRUNK_HASH_OUTPUT_SELECTION_PROFILEs,
        .idx_min = 0,
        .idx_max = 0,
        .keys = COUNTOF(bcmgene_hash_output_selection_profile_trunk_key),
        .key = bcmgene_hash_output_selection_profile_trunk_key,
        .flds = COUNTOF(bcmgene_hash_output_selection_profile_trunk_fld),
        .fld = bcmgene_hash_output_selection_profile_trunk_fld,
    },
    {
        .sub_f = BCMGENE_HASH_OUTPUT_SELECTION_SUB_F_TRUNK_VP| \
                 BCMGENE_HASH_OUTPUT_SELECTION_SUB_F_ECMP_OVERLAY| \
                 BCMGENE_HASH_OUTPUT_SELECTION_SUB_F_DLB_ECMP,
        .flags = 0,
        .table = ECMP_LEVEL0_HASH_OUTPUT_CONTROL_PROFILEs,
        .idx_min = 0,
        .idx_max = 3,
        .keys = COUNTOF(bcmgene_hash_output_selection_profile_ecmp_key),
        .key = bcmgene_hash_output_selection_profile_ecmp_key,
        .flds = COUNTOF(bcmgene_hash_output_selection_profile_ecmp_level0_fld),
        .fld = bcmgene_hash_output_selection_profile_ecmp_level0_fld,
    },
    {
        .sub_f = BCMGENE_HASH_OUTPUT_SELECTION_SUB_F_ECMP_UNDERLAY,
        .flags = 0,
        .table = ECMP_LEVEL1_HASH_OUTPUT_CONTROL_PROFILEs,
        .idx_min = 0,
        .idx_max = 3,
        .keys = COUNTOF(bcmgene_hash_output_selection_profile_ecmp_key),
        .key = bcmgene_hash_output_selection_profile_ecmp_key,
        .flds = COUNTOF(bcmgene_hash_output_selection_profile_ecmp_level1_fld),
        .fld = bcmgene_hash_output_selection_profile_ecmp_level1_fld,
    },
    {
        .sub_f = BCMGENE_HASH_OUTPUT_SELECTION_SUB_F_DLB_TRUNK,
        .flags = 0,
        .table = DLB_TRUNK_HASH_OUTPUT_SELECTION_PROFILEs,
        .idx_min = 0,
        .idx_max = 0,
        .keys = COUNTOF(bcmgene_hash_output_selection_profile_dlb_trunk_key),
        .key = bcmgene_hash_output_selection_profile_dlb_trunk_key,
        .flds = COUNTOF(bcmgene_hash_output_selection_profile_dlb_trunk_fld),
        .fld = bcmgene_hash_output_selection_profile_dlb_trunk_fld,
    },
    {
        .sub_f = BCMGENE_HASH_OUTPUT_SELECTION_SUB_F_ENTROPY,
        .flags = 0,
        .table = LB_HASH_ENTROPY_HASH_SELECTION_CONTROL_PROFILEs,
        .idx_min = 0,
        .idx_max = 0,
        .keys = COUNTOF(bcmgene_hash_output_selection_profile_entropy_key),
        .key = bcmgene_hash_output_selection_profile_entropy_key,
        .flds = COUNTOF(bcmgene_hash_output_selection_profile_entropy_fld),
        .fld = bcmgene_hash_output_selection_profile_entropy_fld,
    },
};

static bcmgene_field_desc_t bcmgene_hash_output_selection_trunk_uc_key[] = {
    {
        .flags = 0,
        .blk = 0,
        .offset = 0,
        .width = 1,
        .depth = 0,
        .field = LB_HASH_INSTANCEs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = 0,
        .width = 8,
        .depth = 0,
        .field = LB_HASH_TRUNK_UC_OUTPUT_SELECTION_IDs,
        .converter = NULL,
    },
};

static bcmgene_field_desc_t bcmgene_hash_output_selection_trunk_nonuc_key[] = {
    {
        .flags = 0,
        .blk = 0,
        .offset = 0,
        .width = 1,
        .depth = 0,
        .field = LB_HASH_INSTANCEs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = 0,
        .width = 8,
        .depth = 0,
        .field = LB_HASH_TRUNK_NONUC_OUTPUT_SELECTION_IDs,
        .converter = NULL,
    },
};

static bcmgene_field_desc_t bcmgene_hash_output_selection_trunk_failover_key[] = {
    {
        .flags = 0,
        .blk = 0,
        .offset = 0,
        .width = 1,
        .depth = 0,
        .field = LB_HASH_INSTANCEs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = 0,
        .width = 8,
        .depth = 0,
        .field = LB_HASH_TRUNK_FAILOVER_OUTPUT_SELECTION_IDs,
        .converter = NULL,
    },
};

static bcmgene_field_desc_t bcmgene_hash_output_selection_trunk_system_key[] = {
    {
        .flags = 0,
        .blk = 0,
        .offset = 0,
        .width = 1,
        .depth = 0,
        .field = LB_HASH_INSTANCEs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = 0,
        .width = 8,
        .depth = 0,
        .field = LB_HASH_TRUNK_SYSTEM_OUTPUT_SELECTION_IDs,
        .converter = NULL,
    },
};

static bcmgene_field_desc_t bcmgene_hash_output_selection_trunk_system_failover_key[] = {
    {
        .flags = 0,
        .blk = 0,
        .offset = 0,
        .width = 1,
        .depth = 0,
        .field = LB_HASH_INSTANCEs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = 0,
        .width = 8,
        .depth = 0,
        .field = LB_HASH_TRUNK_SYSTEM_FAILOVER_OUTPUT_SELECTION_IDs,
        .converter = NULL,
    },
};

static bcmgene_field_desc_t bcmgene_hash_output_selection_ecmp_level0_key[] = {
    {
        .flags = 0,
        .blk = 0,
        .offset = 0,
        .width = 1,
        .depth = 0,
        .field = LB_HASH_TABLE_INSTANCEs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = 0,
        .width = 8,
        .depth = 0,
        .field = LB_HASH_ECMP_LEVEL0_OUTPUT_SELECTION_IDs,
        .converter = NULL,
    },
};

static bcmgene_field_desc_t bcmgene_hash_output_selection_ecmp_level1_key[] = {
    {
        .flags = 0,
        .blk = 0,
        .offset = 0,
        .width = 1,
        .depth = 0,
        .field = LB_HASH_TABLE_INSTANCEs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = 0,
        .width = 8,
        .depth = 0,
        .field = LB_HASH_ECMP_LEVEL1_OUTPUT_SELECTION_IDs,
        .converter = NULL,
    },
};

static bcmgene_field_desc_t bcmgene_hash_output_selection_dlb_trunk_key[] = {
    {
        .flags = 0,
        .blk = 0,
        .offset = 0,
        .width = 1,
        .depth = 0,
        .field = LB_HASH_INSTANCEs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = 0,
        .width = 8,
        .depth = 0,
        .field = LB_HASH_DLB_TRUNK_OUTPUT_SELECTION_IDs,
        .converter = NULL,
    },
};

static bcmgene_field_desc_t bcmgene_hash_output_selection_dlb_ecmp_key[] = {
    {
        .flags = 0,
        .blk = 0,
        .offset = 0,
        .width = 1,
        .depth = 0,
        .field = LB_HASH_INSTANCEs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = 0,
        .width = 8,
        .depth = 0,
        .field = LB_HASH_DLB_ECMP_LEVEL0_OUTPUT_SELECTION_IDs,
        .converter = NULL,
    },
};

static bcmgene_field_desc_t bcmgene_hash_output_selection_entropy_key[] = {
    {
        .flags = 0,
        .blk = 0,
        .offset = 0,
        .width = 1,
        .depth = 0,
        .field = LB_HASH_INSTANCEs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = 0,
        .width = 8,
        .depth = 0,
        .field = LB_HASH_ENTROPY_LABEL_OUTPUT_SELECTION_IDs,
        .converter = NULL,
    },
};

static bcmgene_field_value_map_t bcmgene_hash_output_selection_fld_result_size_val_map[] = {
    {
        .in = 1,
        .out.s = RESULT_SIZE_1BITs,
    },
    {
        .in = 2,
        .out.s = RESULT_SIZE_2BITSs,
    },
    {
        .in = 3,
        .out.s = RESULT_SIZE_3BITSs,
    },
    {
        .in = 4,
        .out.s = RESULT_SIZE_4BITSs,
    },
    {
        .in = 5,
        .out.s = RESULT_SIZE_5BITSs,
    },
    {
        .in = 6,
        .out.s = RESULT_SIZE_6BITSs,
    },
    {
        .in = 7,
        .out.s = RESULT_SIZE_7BITSs,
    },
    {
        .in = 8,
        .out.s = RESULT_SIZE_8BITSs,
    },
    {
        .in = 9,
        .out.s = RESULT_SIZE_9BITSs,
    },
    {
        .in = 10,
        .out.s = RESULT_SIZE_10BITSs,
    },
    {
        .in = 11,
        .out.s = RESULT_SIZE_11BITSs,
    },
    {
        .in = 12,
        .out.s = RESULT_SIZE_12BITSs,
    },
    {
        .in = 13,
        .out.s = RESULT_SIZE_13BITSs,
    },
    {
        .in = 14,
        .out.s = RESULT_SIZE_14BITSs,
    },
    {
        .in = 15,
        .out.s = RESULT_SIZE_15BITSs,
    },
    {
        .in = 16,
        .out.s = RESULT_SIZE_16BITSs,
    },
};

static bcmgene_field_value_converter_t bcmgene_hash_output_selection_fld_result_size_val = {
    .flags = 0,
    .maps = COUNTOF(bcmgene_hash_output_selection_fld_result_size_val_map),
    .map = bcmgene_hash_output_selection_fld_result_size_val_map,
};

static bcmgene_field_value_map_t bcmgene_hash_output_selection_fld_subset_select_val_map[] = {
    {
        .in = 0,
        .out.s = USE_0_0s,
    },
    {
        .in = 1,
        .out.s = USE_1_0s,
    },
    {
        .in = 2,
        .out.s = USE_0_1s,
    },
    {
        .in = 3,
        .out.s = USE_1_1s,
    },
    {
        .in = 4,
        .out.s = USE_0_2s,
    },
    {
        .in = 5,
        .out.s = USE_1_2s,
    },
    {
        .in = 6,
        .out.s = USE_0_3s,
    },
    {
        .in = 7,
        .out.s = USE_1_3s,
    },
};

static bcmgene_field_value_converter_t bcmgene_hash_output_selection_fld_subset_select_val = {
    .flags = 0,
    .maps = COUNTOF(bcmgene_hash_output_selection_fld_subset_select_val_map),
    .map = bcmgene_hash_output_selection_fld_subset_select_val_map,
};

static bcmgene_field_desc_t bcmgene_hash_output_selection_fld[] = {
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmiHosInstaceControlResultSize,
        .width = 0,
        .depth = 0,
        .field = RESULT_SIZEs,
        .converter = &bcmgene_hash_output_selection_fld_result_size_val,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmiHosInstaceControlOffset,
        .width = 0,
        .depth = 0,
        .field = OFFSETs,
        .converter = NULL,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmiHosInstaceControlSubSelect,
        .width = 0,
        .depth = 0,
        .field = SUBSET_SELECTs,
        .converter = &bcmgene_hash_output_selection_fld_subset_select_val,
    },
    {
        .flags = 0,
        .blk = 0,
        .offset = bcmiHosInstaceControlConcat,
        .width = 0,
        .depth = 0,
        .field = CONCATs,
        .converter = NULL,
    },
};

static bcmgene_table_desc_t bcmgene_hash_output_selection[] = {
    {
        .sub_f = BCMGENE_HASH_OUTPUT_SELECTION_SUB_F_TRUNK_UC,
        .flags = 0,
        .table = LB_HASH_TRUNK_UC_OUTPUT_SELECTIONs,
        .idx_min = 0,
        .idx_max = 0,
        .keys = COUNTOF(bcmgene_hash_output_selection_trunk_uc_key),
        .key = bcmgene_hash_output_selection_trunk_uc_key,
        .flds = COUNTOF(bcmgene_hash_output_selection_fld),
        .fld = bcmgene_hash_output_selection_fld,
    },
    {
        .sub_f = BCMGENE_HASH_OUTPUT_SELECTION_SUB_F_TRUNK_NONUC,
        .flags = 0,
        .table = LB_HASH_TRUNK_NONUC_OUTPUT_SELECTIONs,
        .idx_min = 0,
        .idx_max = 0,
        .keys = COUNTOF(bcmgene_hash_output_selection_trunk_nonuc_key),
        .key = bcmgene_hash_output_selection_trunk_nonuc_key,
        .flds = COUNTOF(bcmgene_hash_output_selection_fld),
        .fld = bcmgene_hash_output_selection_fld,
    },
    {
        .sub_f = BCMGENE_HASH_OUTPUT_SELECTION_SUB_F_TRUNK_FAILOVER,
        .flags = 0,
        .table = LB_HASH_TRUNK_FAILOVER_OUTPUT_SELECTIONs,
        .idx_min = 0,
        .idx_max = 0,
        .keys = COUNTOF(bcmgene_hash_output_selection_trunk_failover_key),
        .key = bcmgene_hash_output_selection_trunk_failover_key,
        .flds = COUNTOF(bcmgene_hash_output_selection_fld),
        .fld = bcmgene_hash_output_selection_fld,
    },
    {
        .sub_f = BCMGENE_HASH_OUTPUT_SELECTION_SUB_F_TRUNK_SYSTEM,
        .flags = 0,
        .table = LB_HASH_TRUNK_SYSTEM_OUTPUT_SELECTIONs,
        .idx_min = 0,
        .idx_max = 0,
        .keys = COUNTOF(bcmgene_hash_output_selection_trunk_system_key),
        .key = bcmgene_hash_output_selection_trunk_system_key,
        .flds = COUNTOF(bcmgene_hash_output_selection_fld),
        .fld = bcmgene_hash_output_selection_fld,
    },
    {
        .sub_f = BCMGENE_HASH_OUTPUT_SELECTION_SUB_F_TRUNK_SYSTEM_FAILOVER,
        .flags = 0,
        .table = LB_HASH_TRUNK_SYSTEM_FAILOVER_OUTPUT_SELECTIONs,
        .idx_min = 0,
        .idx_max = 0,
        .keys = COUNTOF(bcmgene_hash_output_selection_trunk_system_failover_key),
        .key = bcmgene_hash_output_selection_trunk_system_failover_key,
        .flds = COUNTOF(bcmgene_hash_output_selection_fld),
        .fld = bcmgene_hash_output_selection_fld,
    },
    {
        .sub_f = BCMGENE_HASH_OUTPUT_SELECTION_SUB_F_ECMP_OVERLAY,
        .flags = 0,
        .table = LB_HASH_ECMP_LEVEL0_OUTPUT_SELECTIONs,
        .idx_min = 0,
        .idx_max = 0,
        .keys = COUNTOF(bcmgene_hash_output_selection_ecmp_level0_key),
        .key = bcmgene_hash_output_selection_ecmp_level0_key,
        .flds = COUNTOF(bcmgene_hash_output_selection_fld),
        .fld = bcmgene_hash_output_selection_fld,
    },
    {
        .sub_f = BCMGENE_HASH_OUTPUT_SELECTION_SUB_F_ECMP_UNDERLAY,
        .flags = 0,
        .table = LB_HASH_ECMP_LEVEL1_OUTPUT_SELECTIONs,
        .idx_min = 0,
        .idx_max = 0,
        .keys = COUNTOF(bcmgene_hash_output_selection_ecmp_level1_key),
        .key = bcmgene_hash_output_selection_ecmp_level1_key,
        .flds = COUNTOF(bcmgene_hash_output_selection_fld),
        .fld = bcmgene_hash_output_selection_fld,
    },
    {
        .sub_f = BCMGENE_HASH_OUTPUT_SELECTION_SUB_F_DLB_TRUNK,
        .flags = 0,
        .table = LB_HASH_DLB_TRUNK_OUTPUT_SELECTIONs,
        .idx_min = 0,
        .idx_max = 0,
        .keys = COUNTOF(bcmgene_hash_output_selection_dlb_trunk_key),
        .key = bcmgene_hash_output_selection_dlb_trunk_key,
        .flds = COUNTOF(bcmgene_hash_output_selection_fld),
        .fld = bcmgene_hash_output_selection_fld,
    },
    {
        .sub_f = BCMGENE_HASH_OUTPUT_SELECTION_SUB_F_DLB_ECMP,
        .flags = 0,
        .table = LB_HASH_DLB_ECMP_LEVEL0_OUTPUT_SELECTIONs,
        .idx_min = 0,
        .idx_max = 0,
        .keys = COUNTOF(bcmgene_hash_output_selection_dlb_ecmp_key),
        .key = bcmgene_hash_output_selection_dlb_ecmp_key,
        .flds = COUNTOF(bcmgene_hash_output_selection_fld),
        .fld = bcmgene_hash_output_selection_fld,
    },
    {
        .sub_f = BCMGENE_HASH_OUTPUT_SELECTION_SUB_F_ENTROPY,
        .flags = 0,
        .table = LB_HASH_ENTROPY_LABEL_OUTPUT_SELECTIONs,
        .idx_min = 0,
        .idx_max = 0,
        .keys = COUNTOF(bcmgene_hash_output_selection_entropy_key),
        .key = bcmgene_hash_output_selection_entropy_key,
        .flds = COUNTOF(bcmgene_hash_output_selection_fld),
        .fld = bcmgene_hash_output_selection_fld,
    },
};

static bcmgene_func_desc_t bcmgene_func_hash_output_selection[] = {
    {
        .func = BCMGENE_HASH_OUTPUT_SELECTION_F_PROFILE,
        .tbls = COUNTOF(bcmgene_hash_output_selection_profile),
        .tbl = bcmgene_hash_output_selection_profile,
    },
    {
        .func = BCMGENE_HASH_OUTPUT_SELECTION_F_INSTANCE,
        .tbls = COUNTOF(bcmgene_hash_output_selection),
        .tbl = bcmgene_hash_output_selection,
    },
};

static bcmgene_desc_t bcmgene[] = {
    {
        .name = "FlexDigest",
        .feature = LTSW_FT_FLEXDIGEST,
        .funcs = COUNTOF(bcmgene_func_flexdigest),
        .func = bcmgene_func_flexdigest,
    },
    {
        .name = "HashOutputSelection",
        .feature = LTSW_FT_HASH_OUTPUT_SELECTION_PROFILE,
        .funcs = COUNTOF(bcmgene_func_hash_output_selection),
        .func = bcmgene_func_hash_output_selection,
    },
};

static bcmgene_db_t bcmgene_db = {
    .ver = "BCM56780_A0_DNA_2_4_13",
    .genes = COUNTOF(bcmgene),
    .gene = bcmgene,
};

/******************************************************************************
 * Public functions
 */

bcmgene_db_t *
bcm56780_a0_dna_2_4_13_gene_db_get(void)
{
    return &bcmgene_db;
}

