/** \file field_map_qual.c
 * 
 *
 * Field database procedures for DNX.
 *
 * This file implements mapping from BCM to DNX qualifiers and access for per DNX qualifier/action information
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX
/*
 * Include files.
 * {
 */

#include <soc/sand/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_rhlist.h>

#include <soc/dnx/dbal/dbal_dynamic.h>
#include <soc/sand/sand_signals.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_pp.h>

#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/field/field_port.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field_map.h>

#include <bcm_int/dnx/algo/swstate/auto_generated/types/dnx_algo_field_types.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_qualifier_access.h>
#include <bcm/field.h>

#include "field_map_local.h"
#include <include/soc/dnx/pemladrv/pemladrv_virtual_wires.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>

extern const char *bcm_qual_description[bcmFieldQualifyCount];
extern const dnx_field_qual_input_type_info_t dnx_input_type_info[DNX_FIELD_INPUT_TYPE_NOF];
extern const dnx_field_sw_qual_info_t dnx_sw_qual_info[DNX_FIELD_SW_QUAL_NOF];
extern const dnx_field_qual_map_t dnx_global_qual_map[bcmFieldQualifyCount];

/*
 * }Include files
 */

/* *INDENT-OFF* */
static const char *dnx_field_qual_class_names[DNX_FIELD_QUAL_CLASS_NOF] = {
    [DNX_FIELD_QUAL_CLASS_INVALID] =        "Invalid",
    [DNX_FIELD_QUAL_CLASS_META] =           "Metadata",
    [DNX_FIELD_QUAL_CLASS_META2] =          "Metadata2",
    [DNX_FIELD_QUAL_CLASS_LAYER_RECORD] =   "Layer Record",
    [DNX_FIELD_QUAL_CLASS_USER] =           "User Defined",
    [DNX_FIELD_QUAL_CLASS_HEADER] =         "Header",
    [DNX_FIELD_QUAL_CLASS_SW] =             "SW",
    [DNX_FIELD_QUAL_CLASS_VW] =             "VW",
};

static const char *dnx_field_input_types_names[DNX_FIELD_INPUT_TYPE_NOF] = {
    [DNX_FIELD_INPUT_TYPE_LAYER_FWD] =              "Layer Forwarding",
    [DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE] =         "Layer Absolute",
    [DNX_FIELD_INPUT_TYPE_META_DATA] =              "Metadata",
    [DNX_FIELD_INPUT_TYPE_META_DATA2] =             "Metadata2",
    [DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_FWD] =      "Layer Records Forwarding",
    [DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_ABSOLUTE] = "Layer Records Absolute",
    [DNX_FIELD_INPUT_TYPE_EXTERNAL] =               "External",
    [DNX_FIELD_INPUT_TYPE_CASCADED] =               "Cascaded"
};

typedef struct
{
    const char *name;
} dnx_field_qual_name_t;

static const dnx_field_qual_name_t dnx_lr_qual_name[DNX_FIELD_LR_QUAL_NOF] = {
    [DNX_FIELD_LR_QUAL_OFFSET]                              = {"Q_LR_LAYER_OFFSET"},
    [DNX_FIELD_LR_QUAL_QUALIFIER]                           = {"Q_LR_LAYER_QUALIFIER"},
    [DNX_FIELD_LR_QUAL_PROTOCOL]                            = {"Q_LR_LAYER_TYPES"},
    [DNX_FIELD_LR_QUAL_ETH_IS_DA_MC]                        = {"Q_LR_ETH_IS_DA_MC"},
    [DNX_FIELD_LR_QUAL_ETH_IS_DA_BC]                        = {"Q_LR_ETH_IS_DA_BC"},
    [DNX_FIELD_LR_QUAL_ETH_1ST_TPID_EXIST]                  = {"Q_LR_ETH_1ST_TPID_EXIST"},
    [DNX_FIELD_LR_QUAL_ETH_1ST_TPID_INDEX]                  = {"Q_LR_ETH_1ST_TPID_INDEX"},
    [DNX_FIELD_LR_QUAL_ETH_2ND_TPID_EXIST]                  = {"Q_LR_ETH_2ND_TPID_EXIST"},
    [DNX_FIELD_LR_QUAL_ETH_2ND_TPID_INDEX]                  = {"Q_LR_ETH_2ND_TPID_INDEX"},
    [DNX_FIELD_LR_QUAL_ETH_3RD_TPID_EXIST]                  = {"Q_LR_ETH_3RD_TPID_EXIST"},
    [DNX_FIELD_LR_QUAL_ETH_3RD_TPID_INDEX]                  = {"Q_LR_ETH_3RD_TPID_INDEX"},
    [DNX_FIELD_LR_QUAL_ETH_FRAME_TYPE]                      = {"Q_LR_ETH_FRAME_TYPE"},
    [DNX_FIELD_LR_QUAL_IPV4_IS_MC]                          = {"Q_LR_IPV4_IS_MC"},
    [DNX_FIELD_LR_QUAL_IPV4_HAS_OPTION]                     = {"Q_LR_IPV4_HAS_OPTION"},
    [DNX_FIELD_LR_QUAL_IPV4_IS_FRAGMENTED]                  = {"Q_LR_IPV4_IS_FRAGMENTED"},
    [DNX_FIELD_LR_QUAL_IPV4_IS_1ST_FRAGMENT]                = {"Q_LR_IPV4_IS_1ST_FRAGMENT"},
    [DNX_FIELD_LR_QUAL_IPV4_1ST_ADD_HEADER_EXIST]           = {"Q_LR_IPV4_1ST_ADD_HEADER_EXIST"},
    [DNX_FIELD_LR_QUAL_IPV4_1ST_ADD_HEADER]                 = {"Q_LR_IPV4_1ST_ADD_HEADER"},
    [DNX_FIELD_LR_QUAL_IPV4_2ND_ADD_HEADER_EXISTS]          = {"Q_LR_IPV4_2ND_ADD_HEADER_EXISTS"},
    [DNX_FIELD_LR_QUAL_IPV4_TUNNEL_TYPE]                    = {"Q_LR_IPV4_TUNNEL_TYPE"},
    [DNX_FIELD_LR_QUAL_IPV4_TUNNEL_TYPE_RAW]                = {"Q_LR_IPV4_TUNNEL_TYPE_RAW"},
    [DNX_FIELD_LR_QUAL_IPV4_FRAGMENTED_NON_FIRST]           = {"Q_LR_IPV4_FRAGMENTED_NON_FIRST"},
    [DNX_FIELD_LR_QUAL_IPV4_FIRST_FRAG_OR_NON_FRAG]         = {"Q_LR_IPV4_FIRST_FRAG_OR_NON_FRAG"},
    [DNX_FIELD_LR_QUAL_IPV6_IS_MC]                          = {"Q_LR_IPV6_IS_MC"},
    [DNX_FIELD_LR_QUAL_IPV6_1ST_ADD_HEADER_EXIST]           = {"Q_LR_IPV6_1ST_ADD_HEADER_EXIST"},
    [DNX_FIELD_LR_QUAL_IPV6_1ST_ADD_HEADER]                 = {"Q_LR_IPV6_QUALIFIER_ADDITIONAL_HEADER_1ST"},
    [DNX_FIELD_LR_QUAL_IPV6_2ND_ADD_HEADER_EXISTS]          = {"Q_LR_IPV6_2ND_ADD_HEADER_EXISTS"},
    [DNX_FIELD_LR_QUAL_IPV6_2ND_ADD_HEADER]                 = {"Q_LR_IPV6_QUALIFIER_ADDITIONAL_HEADER_2ND"},
    [DNX_FIELD_LR_QUAL_IPV6_3RD_ADD_HEADER_EXISTS]          = {"Q_LR_IPV6_3RD_ADD_HEADER_EXISTS"},
    [DNX_FIELD_LR_QUAL_IPV6_FRAGMENTED_NON_FIRST]           = {"Q_LR_IPV6_FRAGMENTED_NON_FIRST"},
    [DNX_FIELD_LR_QUAL_SRV6_EP_SEGMENT_LEFT_MINUS_ONE]      = {"Q_LR_SRV6_EP_SEGMENT_LEFT_MINUS_ONE"},
    [DNX_FIELD_LR_QUAL_MPLS_NOF_LABELS]                     = {"Q_LR_MPLS_NOF_LABELS"},
    [DNX_FIELD_LR_QUAL_BIER_BITSTRINGLENGTH]                = {"Q_LR_BIER_BITSTRINGLENGTH"},
    [DNX_FIELD_LR_QUAL_FCOE_IS_FIP]                         = {"Q_LR_FCOE_IS_FIP"},
    [DNX_FIELD_LR_QUAL_FCOE_IS_ENCAP]                       = {"Q_LR_FCOE_IS_ENCAP"},
    [DNX_FIELD_LR_QUAL_FCOE_VFT_EXIST]                      = {"Q_LR_FCOE_VFT_EXIST"},
    [DNX_FIELD_LR_QUAL_ITMH_PPH_TYPE]                       = {"Q_LR_ITMH_PPH_TYPE"}
};

static const dnx_field_qual_name_t dnx_qual_name[DNX_FIELD_QUAL_NOF] = {
        [DNX_FIELD_QUAL_INVALID]                                        = {"Q_INVALID"},
        [DNX_FIELD_QUAL_ACL_CONTEXT]                                    = {"Q_ACL_CONTEXT"},
        [DNX_FIELD_QUAL_ACTION_PROFILE_DA_NOT_FOUND_INDEX]              = {"Q_ACTION_PROFILE_DA_NOT_FOUND_INDEX"},
        [DNX_FIELD_QUAL_ACTION_PROFILE_SA_DROP_INDEX]                   = {"Q_ACTION_PROFILE_SA_DROP_INDEX"},
        [DNX_FIELD_QUAL_ACTION_PROFILE_SA_NOT_FOUND_INDEX]              = {"Q_ACTION_PROFILE_SA_NOT_FOUND_INDEX"},
        [DNX_FIELD_QUAL_AC_IN_LIF_WIDE_DATA]                            = {"Q_AC_IN_LIF_WIDE_DATA"},
        [DNX_FIELD_QUAL_AC_IN_LIF_WIDE_DATA_EXTENDED]                   = {"Q_AC_IN_LIF_WIDE_DATA_EXTENDED"},
        [DNX_FIELD_QUAL_ADMT_PROFILE]                                   = {"Q_ADMT_PROFILE"},
        [DNX_FIELD_QUAL_ALGO_LPM_RESULT_1]                              = {"Q_ALGO_LPM_RESULT_1"},
        [DNX_FIELD_QUAL_ALGO_LPM_RESULT_2]                              = {"Q_ALGO_LPM_RESULT_2"},
        [DNX_FIELD_QUAL_ALL_ONES]                                       = {"Q_ALL_ONES"},
        [DNX_FIELD_QUAL_ALT_CONTEXT]                                    = {"Q_ALT_CONTEXT"},
        [DNX_FIELD_QUAL_APPLICATION_SPECIFIC_EXT]                       = {"Q_APPLICATION_SPECIFIC_EXT"},
        [DNX_FIELD_QUAL_BIER_BFR_EXT]                                   = {"Q_BIER_BFR_EXT"},
        [DNX_FIELD_QUAL_BIER_STR_OFFSET]                                = {"Q_BIER_STR_OFFSET"},
        [DNX_FIELD_QUAL_BIER_STR_SIZE]                                  = {"Q_BIER_STR_SIZE"},
        [DNX_FIELD_QUAL_BTC_MINUS_BTK]                                  = {"Q_BTC_MINUS_BTK"},
        [DNX_FIELD_QUAL_BTC_OFFSET_SOP]                                 = {"Q_BTC_OFFSET_SOP"},
        [DNX_FIELD_QUAL_BTK_OFFSET_SOP]                                 = {"Q_BTK_OFFSET_SOP"},
        [DNX_FIELD_QUAL_BYTES_TO_REMOVE]                                = {"Q_BYTES_TO_REMOVE"},
        [DNX_FIELD_QUAL_CFG_CORE_ID]                                    = {"Q_CFG_CORE_ID"},
        [DNX_FIELD_QUAL_CMP_KEY_0_DECODED]                              = {"Q_CMP_KEY_0_DECODED"},
        [DNX_FIELD_QUAL_CMP_KEY_1_DECODED]                              = {"Q_CMP_KEY_1_DECODED"},
        [DNX_FIELD_QUAL_CMP_KEY_TCAM_0_DECODED]                         = {"Q_CMP_KEY_TCAM_0_DECODED"},
        [DNX_FIELD_QUAL_CMP_KEY_TCAM_1_DECODED]                         = {"Q_CMP_KEY_TCAM_1_DECODED"},
        [DNX_FIELD_QUAL_CONGESTION_INFO]                                = {"Q_CONGESTION_INFO"},
        [DNX_FIELD_QUAL_CONTEXT]                                        = {"Q_CONTEXT"},
        [DNX_FIELD_QUAL_CONTEXT_KEY_GEN_VAR]                            = {"Q_CONTEXT_KEY_GEN_VAR"},
        [DNX_FIELD_QUAL_CORE_ID]                                        = {"Q_CORE_ID"},
        [DNX_FIELD_QUAL_CPU_TRAP_CODE_PROFILE]                          = {"Q_CPU_TRAP_CODE_PROFILE"},
        [DNX_FIELD_QUAL_CUD_OUTLIF_OR_MCDB_PTR]                         = {"Q_CUD_OUTLIF_OR_MCDB_PTR"},
        [DNX_FIELD_QUAL_DISCARD]                                        = {"Q_DISCARD"},
        [DNX_FIELD_QUAL_DP]                                             = {"Q_DP"},
        [DNX_FIELD_QUAL_DST_SYS_PORT]                                   = {"Q_DST_SYS_PORT"},
        [DNX_FIELD_QUAL_DUAL_QUEUE]                                     = {"Q_DUAL_QUEUE"},
        [DNX_FIELD_QUAL_ECL_VALID]                                      = {"Q_ECL_VALID"},
        [DNX_FIELD_QUAL_ECMP_LB_KEY_0]                                  = {"Q_ECMP_LB_KEY_0"},
        [DNX_FIELD_QUAL_ECMP_LB_KEY_1]                                  = {"Q_ECMP_LB_KEY_1"},
        [DNX_FIELD_QUAL_ECMP_LB_KEY_2]                                  = {"Q_ECMP_LB_KEY_2"},
        [DNX_FIELD_QUAL_ECN]                                            = {"Q_ECN"},
        [DNX_FIELD_QUAL_EEI]                                            = {"Q_EEI"},
        [DNX_FIELD_QUAL_EGRESS_LEARN_ENABLE]                            = {"Q_EGRESS_LEARN_ENABLE"},
        [DNX_FIELD_QUAL_EGRESS_PARSING_INDEX]                           = {"Q_EGRESS_PARSING_INDEX"},
        [DNX_FIELD_QUAL_ELEPHANT_LEARN_NEEDED]                          = {"Q_ELEPHANT_LEARN_NEEDED"},
        [DNX_FIELD_QUAL_ELEPHANT_TRAP_INFO]                             = {"Q_ELEPHANT_TRAP_INFO"},
        [DNX_FIELD_QUAL_ELK_LKP_HIT_0]                                  = {"Q_ELK_LKP_HIT_0"},
        [DNX_FIELD_QUAL_ELK_LKP_HIT_1]                                  = {"Q_ELK_LKP_HIT_1"},
        [DNX_FIELD_QUAL_ELK_LKP_HIT_2]                                  = {"Q_ELK_LKP_HIT_2"},
        [DNX_FIELD_QUAL_ELK_LKP_HIT_3]                                  = {"Q_ELK_LKP_HIT_3"},
        [DNX_FIELD_QUAL_ELK_LKP_HIT_4]                                  = {"Q_ELK_LKP_HIT_4"},
        [DNX_FIELD_QUAL_ELK_LKP_HIT_5]                                  = {"Q_ELK_LKP_HIT_5"},
        [DNX_FIELD_QUAL_ELK_LKP_HIT_6]                                  = {"Q_ELK_LKP_HIT_6"},
        [DNX_FIELD_QUAL_ELK_LKP_HIT_7]                                  = {"Q_ELK_LKP_HIT_7"},
        [DNX_FIELD_QUAL_EXTERNAL_RESULT_BUFFER_PART_0]                  = {"Q_EXTERNAL_RESULT_BUFFER_PART_0"},
        [DNX_FIELD_QUAL_EXTERNAL_RESULT_BUFFER_PART_1]                  = {"Q_EXTERNAL_RESULT_BUFFER_PART_1"},
        [DNX_FIELD_QUAL_ENABLE_PP_INJECT]                               = {"Q_ENABLE_PP_INJECT"},
        [DNX_FIELD_QUAL_ENCODER0]                                       = {"Q_ENCODER0"},
        [DNX_FIELD_QUAL_ENCODER1]                                       = {"Q_ENCODER1"},
        [DNX_FIELD_QUAL_ENCODER2]                                       = {"Q_ENCODER2"},
        [DNX_FIELD_QUAL_ENCODER3]                                       = {"Q_ENCODER3"},
        [DNX_FIELD_QUAL_END_OF_PACKET_EDITING]                          = {"Q_END_OF_PACKET_EDITING"},
        [DNX_FIELD_QUAL_EPMF_CONTEXT_LFSR_1]                            = {"Q_EPMF_CONTEXT_LFSR_1"},
        [DNX_FIELD_QUAL_EPMF_CONTEXT_LFSR_2]                            = {"Q_EPMF_CONTEXT_LFSR_2"},
        [DNX_FIELD_QUAL_EPMF_CONTEXT_VAR]                               = {"Q_EPMF_CONTEXT_VAR"},
        [DNX_FIELD_QUAL_ESTIMATED_BTR]                                  = {"Q_ESTIMATED_BTR"},
        [DNX_FIELD_QUAL_ETHER_TYPE_CODE]                                = {"Q_ETHER_TYPE_CODE"},
        [DNX_FIELD_QUAL_ETH_TAG_FORMAT]                                 = {"Q_ETH_TAG_FORMAT"},
        [DNX_FIELD_QUAL_EXT_STATISTICS_ATR_0]                           = {"Q_EXT_STATISTICS_ATR_0"},
        [DNX_FIELD_QUAL_EXT_STATISTICS_ATR_1]                           = {"Q_EXT_STATISTICS_ATR_1"},
        [DNX_FIELD_QUAL_EXT_STATISTICS_ATR_2]                           = {"Q_EXT_STATISTICS_ATR_2"},
        [DNX_FIELD_QUAL_EXT_STATISTICS_ATR_3]                           = {"Q_EXT_STATISTICS_ATR_3"},
        [DNX_FIELD_QUAL_EXT_STATISTICS_ID_0]                            = {"Q_EXT_STATISTICS_ID_0"},
        [DNX_FIELD_QUAL_EXT_STATISTICS_ID_1]                            = {"Q_EXT_STATISTICS_ID_1"},
        [DNX_FIELD_QUAL_EXT_STATISTICS_ID_2]                            = {"Q_EXT_STATISTICS_ID_2"},
        [DNX_FIELD_QUAL_EXT_STATISTICS_ID_3]                            = {"Q_EXT_STATISTICS_ID_3"},
        [DNX_FIELD_QUAL_EXT_STATISTICS_VALID]                           = {"Q_EXT_STATISTICS_VALID"},
        [DNX_FIELD_QUAL_FABRIC_OR_EGRESS_MC]                            = {"Q_FABRIC_OR_EGRESS_MC"},
        [DNX_FIELD_QUAL_FAI_DO_NOT_DEC_TTL]                             = {"Q_FAI_DO_NOT_DEC_TTL"},
        [DNX_FIELD_QUAL_FER_STAGE_0_IN_DST]                             = {"Q_FER_STAGE_0_IN_DST"},
        [DNX_FIELD_QUAL_FER_STAGE_1_IN_DST]                             = {"Q_FER_STAGE_1_IN_DST"},
        [DNX_FIELD_QUAL_FER_STAGE_2_IN_DST]                             = {"Q_FER_STAGE_2_IN_DST"},
        [DNX_FIELD_QUAL_FER_STAGE_0_IN_DST_IS_MINE]                     = {"Q_FER_STAGE_0_IN_DST_IS_MINE"},
        [DNX_FIELD_QUAL_FER_STAGE_1_IN_DST_IS_MINE]                     = {"Q_FER_STAGE_1_IN_DST_IS_MINE"},
        [DNX_FIELD_QUAL_FER_STAGE_2_IN_DST_IS_MINE]                     = {"Q_FER_STAGE_2_IN_DST_IS_MINE"},
        [DNX_FIELD_QUAL_FER_STAGE_0_RESOLVED_FEC_PTR]                   = {"Q_FER_STAGE_0_RESOLVED_FEC_PTR"},
        [DNX_FIELD_QUAL_FER_STAGE_1_RESOLVED_FEC_PTR]                   = {"Q_FER_STAGE_1_RESOLVED_FEC_PTR"},
        [DNX_FIELD_QUAL_FER_STAGE_2_RESOLVED_FEC_PTR]                   = {"Q_FER_STAGE_2_RESOLVED_FEC_PTR"},
        [DNX_FIELD_QUAL_FER_STAGE_0_RESOLVED_FEC_VALID]                 = {"Q_FER_STAGE_0_RESOLVED_FEC_VALID"},
        [DNX_FIELD_QUAL_FER_STAGE_1_RESOLVED_FEC_VALID]                 = {"Q_FER_STAGE_1_RESOLVED_FEC_VALID"},
        [DNX_FIELD_QUAL_FER_STAGE_2_RESOLVED_FEC_VALID]                 = {"Q_FER_STAGE_2_RESOLVED_FEC_VALID"},
        [DNX_FIELD_QUAL_USER_HEADER_1_EXT]                              = {"Q_USER_HEADER_1_EXT"},
        [DNX_FIELD_QUAL_USER_HEADER_2_EXT]                              = {"Q_USER_HEADER_2_EXT"},
        [DNX_FIELD_QUAL_FER_STATISTICS_OBJ]                             = {"Q_FER_STATISTICS_OBJ"},
        [DNX_FIELD_QUAL_FHEI_EXT]                                       = {"Q_FHEI_EXT"},
        [DNX_FIELD_QUAL_FLOW_ID_EXT]                                    = {"Q_FLOW_ID_EXT"},
        [DNX_FIELD_QUAL_FLP_STATISTICS_ATR_0]                           = {"Q_FLP_STATISTICS_ATR_0"},
        [DNX_FIELD_QUAL_FLP_STATISTICS_ATR_1]                           = {"Q_FLP_STATISTICS_ATR_1"},
        [DNX_FIELD_QUAL_FLP_STATISTICS_ATR_2]                           = {"Q_FLP_STATISTICS_ATR_2"},
        [DNX_FIELD_QUAL_FLP_STATISTICS_ATR_3]                           = {"Q_FLP_STATISTICS_ATR_3"},
        [DNX_FIELD_QUAL_FLP_STATISTICS_ATR_4]                           = {"Q_FLP_STATISTICS_ATR_4"},
        [DNX_FIELD_QUAL_FLP_STATISTICS_ATR_5]                           = {"Q_FLP_STATISTICS_ATR_5"},
        [DNX_FIELD_QUAL_FLP_STATISTICS_ID_0]                            = {"Q_FLP_STATISTICS_ID_0"},
        [DNX_FIELD_QUAL_FLP_STATISTICS_ID_1]                            = {"Q_FLP_STATISTICS_ID_1"},
        [DNX_FIELD_QUAL_FLP_STATISTICS_ID_2]                            = {"Q_FLP_STATISTICS_ID_2"},
        [DNX_FIELD_QUAL_FLP_STATISTICS_ID_3]                            = {"Q_FLP_STATISTICS_ID_3"},
        [DNX_FIELD_QUAL_FLP_STATISTICS_ID_4]                            = {"Q_FLP_STATISTICS_ID_4"},
        [DNX_FIELD_QUAL_FLP_STATISTICS_ID_5]                            = {"Q_FLP_STATISTICS_ID_5"},
        [DNX_FIELD_QUAL_FTMH_APPLICATION_SPECIFIC_EXT_PRESENT]          = {"Q_FTMH_APPLICATION_SPECIFIC_EXT_PRESENT"},
        [DNX_FIELD_QUAL_FTMH_AQM_PROFILE]                               = {"Q_FTMH_AQM_PROFILE"},
        [DNX_FIELD_QUAL_FTMH_BIER_BFR_EXT_PRESENT]                      = {"Q_FTMH_BIER_BFR_EXT_PRESENT"},
        [DNX_FIELD_QUAL_FTMH_CNI]                                       = {"Q_FTMH_CNI"},
        [DNX_FIELD_QUAL_FTMH_DP]                                        = {"Q_FTMH_DP"},
        [DNX_FIELD_QUAL_FTMH_ECN_ENABLE]                                = {"Q_FTMH_ECN_ENABLE"},
        [DNX_FIELD_QUAL_FTMH_FLOW_ID_EXT_PRESENT]                       = {"Q_FTMH_FLOW_ID_EXT_PRESENT"},
        [DNX_FIELD_QUAL_FTMH_MCID_OR_OUTLIF_0_OR_MCDB_PTR]              = {"Q_FTMH_MCID_OR_OUTLIF_0_OR_MCDB_PTR"},
        [DNX_FIELD_QUAL_FTMH_PACKET_SIZE]                               = {"Q_FTMH_PACKET_SIZE"},
        [DNX_FIELD_QUAL_FTMH_PPH_PRESENT]                               = {"Q_FTMH_PPH_PRESENT"},
        [DNX_FIELD_QUAL_FTMH_RESERVED]                                  = {"Q_FTMH_RESERVED"},
        [DNX_FIELD_QUAL_FTMH_SRC_SYS_PORT]                              = {"Q_FTMH_SRC_SYS_PORT"},
        [DNX_FIELD_QUAL_FTMH_TC]                                        = {"Q_FTMH_TC"},
        [DNX_FIELD_QUAL_FTMH_TM_ACTION_IS_MC]                           = {"Q_FTMH_TM_ACTION_IS_MC"},
        [DNX_FIELD_QUAL_FTMH_TM_ACTION_TYPE]                            = {"Q_FTMH_TM_ACTION_TYPE"},
        [DNX_FIELD_QUAL_FTMH_TM_DESTINATION_EXT_PRESENT]                = {"Q_FTMH_TM_DESTINATION_EXT_PRESENT"},
        [DNX_FIELD_QUAL_FTMH_TM_PROFILE]                                = {"Q_FTMH_TM_PROFILE"},
        [DNX_FIELD_QUAL_FTMH_TSH_EXT_PRESENT]                           = {"Q_FTMH_TSH_EXT_PRESENT"},
        [DNX_FIELD_QUAL_FTMH_VISIBILITY]                                = {"Q_FTMH_VISIBILITY"},
        [DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_FWD_FOUND]                = {"Q_FWD1_PRIORITY_DECODER_FWD_FOUND"},
        [DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_FWD_PROFILE]              = {"Q_FWD1_PRIORITY_DECODER_FWD_PROFILE"},
        [DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_FWD_RESULT]               = {"Q_FWD1_PRIORITY_DECODER_FWD_RESULT"},
        [DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_FWD_STRENGTH]             = {"Q_FWD1_PRIORITY_DECODER_FWD_STRENGTH"},
        [DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_RPF_FOUND]                = {"Q_FWD1_PRIORITY_DECODER_RPF_FOUND"},
        [DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_RPF_PROFILE]              = {"Q_FWD1_PRIORITY_DECODER_RPF_PROFILE"},
        [DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_RPF_RESULT]               = {"Q_FWD1_PRIORITY_DECODER_RPF_RESULT"},
        [DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_RPF_STRENGTH]             = {"Q_FWD1_PRIORITY_DECODER_RPF_STRENGTH"},
        [DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_CODE]                       = {"Q_FWD_ACTION_CPU_TRAP_CODE"},
        [DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_QUAL]                       = {"Q_FWD_ACTION_CPU_TRAP_QUAL"},
        [DNX_FIELD_QUAL_FWD_ACTION_DESTINATION]                         = {"Q_FWD_ACTION_DESTINATION"},
        [DNX_FIELD_QUAL_FWD_ACTION_DP]                                  = {"Q_FWD_ACTION_DP"},
        [DNX_FIELD_QUAL_FWD_ACTION_DP_METER_COMMAND]                    = {"Q_FWD_ACTION_DP_METER_COMMAND"},
        [DNX_FIELD_QUAL_FWD_ACTION_PROFILE_INDEX]                       = {"Q_FWD_ACTION_PROFILE_INDEX"},
        [DNX_FIELD_QUAL_FWD_ACTION_STRENGTH]                            = {"Q_FWD_ACTION_STRENGTH"},
        [DNX_FIELD_QUAL_FWD_ACTION_TC]                                  = {"Q_FWD_ACTION_TC"},
        [DNX_FIELD_QUAL_FWD_CONTEXT]                                    = {"Q_FWD_CONTEXT"},
        [DNX_FIELD_QUAL_FWD_DOMAIN]                                     = {"Q_FWD_DOMAIN"},
        [DNX_FIELD_QUAL_FWD_DOMAIN_PROFILE]                             = {"Q_FWD_DOMAIN_PROFILE"},
        [DNX_FIELD_QUAL_FWD_KAPSB_IF0_ERR]                              = {"Q_FWD_KAPSB_IF0_ERR"},
        [DNX_FIELD_QUAL_FWD_KAPSB_IF0_FOUND]                            = {"Q_FWD_KAPSB_IF0_FOUND"},
        [DNX_FIELD_QUAL_FWD_KAPSB_IF0_PAYLOAD]                          = {"Q_FWD_KAPSB_IF0_PAYLOAD"},
        [DNX_FIELD_QUAL_FWD_KAPSB_IF1_ERR]                              = {"Q_FWD_KAPSB_IF1_ERR"},
        [DNX_FIELD_QUAL_FWD_KAPSB_IF1_FOUND]                            = {"Q_FWD_KAPSB_IF1_FOUND"},
        [DNX_FIELD_QUAL_FWD_KAPSB_IF1_PAYLOAD]                          = {"Q_FWD_KAPSB_IF1_PAYLOAD"},
        [DNX_FIELD_QUAL_FWD_KAPSC_IF0_ERR]                              = {"Q_FWD_KAPSC_IF0_ERR"},
        [DNX_FIELD_QUAL_FWD_KAPSC_IF0_FOUND]                            = {"Q_FWD_KAPSC_IF0_FOUND"},
        [DNX_FIELD_QUAL_FWD_KAPSC_IF0_PAYLOAD]                          = {"Q_FWD_KAPSC_IF0_PAYLOAD"},
        [DNX_FIELD_QUAL_FWD_KAPSC_IF1_ERR]                              = {"Q_FWD_KAPSC_IF1_ERR"},
        [DNX_FIELD_QUAL_FWD_KAPSC_IF1_FOUND]                            = {"Q_FWD_KAPSC_IF1_FOUND"},
        [DNX_FIELD_QUAL_FWD_KAPSC_IF1_PAYLOAD]                          = {"Q_FWD_KAPSC_IF1_PAYLOAD"},
        [DNX_FIELD_QUAL_FWD_LAYER_ADDITIONAL_INFO]                      = {"Q_FWD_LAYER_ADDITIONAL_INFO"},
        [DNX_FIELD_QUAL_FWD_LAYER_INDEX]                                = {"Q_FWD_LAYER_INDEX"},
        [DNX_FIELD_QUAL_FWD_OPAQUE_FIELD]                               = {"Q_FWD_OPAQUE_FIELD"},
        [DNX_FIELD_QUAL_FWD_PAYLOAD]                                    = {"Q_FWD_PAYLOAD"},
        [DNX_FIELD_QUAL_FWD_PLUS1_REMARK_PROFILE]                       = {"Q_FWD_PLUS1_REMARK_PROFILE"},
        [DNX_FIELD_QUAL_TYPE_FWD_REMARK_OR_KEEP]                        = {"Q_TYPE_FWD_REMARK_OR_KEEP"},
        [DNX_FIELD_QUAL_FWD_STAT_ATTRIBUTE_OBJ]                         = {"Q_FWD_STAT_ATTRIBUTE_OBJ"},
        [DNX_FIELD_QUAL_FWD_STAT_ATTRIBUTE_OBJ_CMD]                     = {"Q_FWD_STAT_ATTRIBUTE_OBJ_CMD"},
        [DNX_FIELD_QUAL_GENERAL_DATA]                                   = {"Q_GENERAL_DATA"},
        [DNX_FIELD_QUAL_GENERAL_DATA_0]                                 = {"Q_GENERAL_DATA_0"},
        [DNX_FIELD_QUAL_GENERAL_DATA_1]                                 = {"Q_GENERAL_DATA_1"},
        [DNX_FIELD_QUAL_GENERAL_DATA_2]                                 = {"Q_GENERAL_DATA_2"},
        [DNX_FIELD_QUAL_GENERAL_DATA_3]                                 = {"Q_GENERAL_DATA_3"},
        [DNX_FIELD_QUAL_GENERAL_DATA_PMF_CONTAINER]                     = {"Q_GENERAL_DATA_PMF_CONTAINER"},
        [DNX_FIELD_QUAL_GLOB_IN_LIF_0]                                  = {"Q_GLOB_IN_LIF_0"},
        [DNX_FIELD_QUAL_GLOB_IN_LIF_1]                                  = {"Q_GLOB_IN_LIF_1"},
        [DNX_FIELD_QUAL_GLOB_OUT_LIF_0]                                 = {"Q_GLOB_OUT_LIF_0"},
        [DNX_FIELD_QUAL_GLOB_OUT_LIF_1]                                 = {"Q_GLOB_OUT_LIF_1"},
        [DNX_FIELD_QUAL_GLOB_OUT_LIF_2]                                 = {"Q_GLOB_OUT_LIF_2"},
        [DNX_FIELD_QUAL_GLOB_OUT_LIF_3]                                 = {"Q_GLOB_OUT_LIF_3"},
        [DNX_FIELD_QUAL_GLOB_VOQ_AGG_ADDR]                              = {"Q_GLOB_VOQ_AGG_ADDR"},
        [DNX_FIELD_QUAL_IEEE1588_COMMAND]                               = {"Q_IEEE1588_COMMAND"},
        [DNX_FIELD_QUAL_IEEE1588_COMPENSATE_TIME_STAMP]                 = {"Q_IEEE1588_COMPENSATE_TIME_STAMP"},
        [DNX_FIELD_QUAL_IEEE1588_ENCAPSULATION]                         = {"Q_IEEE1588_ENCAPSULATION"},
        [DNX_FIELD_QUAL_IEEE1588_HEADER_OFFSET]                         = {"Q_IEEE1588_HEADER_OFFSET"},
        [DNX_FIELD_QUAL_INCOMING_TAG_STRUCTURE]                         = {"Q_INCOMING_TAG_STRUCTURE"},
        [DNX_FIELD_QUAL_INGRESS_LEARN_ENABLE]                           = {"Q_INGRESS_LEARN_ENABLE"},
        [DNX_FIELD_QUAL_INGRESS_PROTOCOL_TRAP_IN_LIF_PROFILE]           = {"Q_INGRESS_PROTOCOL_TRAP_IN_LIF_PROFILE"},
        [DNX_FIELD_QUAL_IN_LIF_PROFILE_0]                               = {"Q_IN_LIF_PROFILE_0"},
        [DNX_FIELD_QUAL_PPH_IN_LIF_PROFILE_0]                           = {"Q_PPH_IN_LIF_PROFILE_0"},
        [DNX_FIELD_QUAL_IN_LIF_PROFILE_1]                               = {"Q_IN_LIF_PROFILE_1"},
        [DNX_FIELD_QUAL_IN_LIF_UNKNOWN_DA_PROFILE]                      = {"Q_IN_LIF_UNKNOWN_DA_PROFILE"},
        [DNX_FIELD_QUAL_IN_PORT]                                        = {"Q_IN_PORT"},
        [DNX_FIELD_QUAL_IN_PORT_KEY_GEN_VAR]                            = {"Q_IN_PORT_KEY_GEN_VAR"},
        [DNX_FIELD_QUAL_IN_PORT_MAPPED_PP_PORT]                         = {"Q_IN_PORT_MAPPED_PP_PORT"},
        [DNX_FIELD_QUAL_IN_TTL]                                         = {"Q_IN_TTL"},
        [DNX_FIELD_QUAL_IPV4_CHECKSUM_ERROR]                            = {"Q_IPV4_CHECKSUM_ERROR"},
        [DNX_FIELD_QUAL_IPV4_DIP_ZERO_ERROR]                            = {"Q_IPV4_DIP_ZERO_ERROR"},
        [DNX_FIELD_QUAL_IPV4_HEADER_LENGTH_ERROR]                       = {"Q_IPV4_HEADER_LENGTH_ERROR"},
        [DNX_FIELD_QUAL_IPV4_OPTIONS_ERROR]                             = {"Q_IPV4_OPTIONS_ERROR"},
        [DNX_FIELD_QUAL_IPV4_SIP_EQUAL_DIP_ERROR]                       = {"Q_IPV4_SIP_EQUAL_DIP_ERROR"},
        [DNX_FIELD_QUAL_IPV4_SIP_IS_MC_ERROR]                           = {"Q_IPV4_SIP_IS_MC_ERROR"},
        [DNX_FIELD_QUAL_IPV4_TOTAL_LENGTH_ERROR]                        = {"Q_IPV4_TOTAL_LENGTH_ERROR"},
        [DNX_FIELD_QUAL_IPV4_VERSION_ERROR]                             = {"Q_IPV4_VERSION_ERROR"},
        [DNX_FIELD_QUAL_IP_MC_ELIGIBLE]                                 = {"Q_IP_MC_ELIGIBLE"},
        [DNX_FIELD_QUAL_IP_MC_SHOULD_BE_BRIDGED]                        = {"Q_IP_MC_SHOULD_BE_BRIDGED"},
        [DNX_FIELD_QUAL_IP_TUNNEL_IN_LIF_WIDE_DATA]                     = {"Q_IP_TUNNEL_IN_LIF_WIDE_DATA"},
        [DNX_FIELD_QUAL_IS_IN_LIF_VALID_FOR_LM]                         = {"Q_IS_IN_LIF_VALID_FOR_LM"},
        [DNX_FIELD_QUAL_IS_TDM]                                         = {"Q_IS_TDM"},
        [DNX_FIELD_QUAL_ITPP_DELTA]                                     = {"Q_ITPP_DELTA"},
        [DNX_FIELD_QUAL_IVE_BYTES_TO_ADD]                               = {"Q_IVE_BYTES_TO_ADD"},
        [DNX_FIELD_QUAL_IVE_BYTES_TO_REMOVE]                            = {"Q_IVE_BYTES_TO_REMOVE"},
        [DNX_FIELD_QUAL_KEY_F_INITIAL]                                  = {"Q_KEY_F_INITIAL"},
        [DNX_FIELD_QUAL_KEY_G_INITIAL]                                  = {"Q_KEY_G_INITIAL"},
        [DNX_FIELD_QUAL_KEY_H_INITIAL]                                  = {"Q_KEY_H_INITIAL"},
        [DNX_FIELD_QUAL_KEY_I_INITIAL]                                  = {"Q_KEY_I_INITIAL"},
        [DNX_FIELD_QUAL_KEY_J_INITIAL]                                  = {"Q_KEY_J_INITIAL"},
        [DNX_FIELD_QUAL_L4OPS]                                          = {"Q_L4OPS"},
        [DNX_FIELD_QUAL_L4_PORT_IN_RANGE]                               = {"Q_L4_PORT_IN_RANGE"},
        [DNX_FIELD_QUAL_LAG_LB_KEY]                                     = {"Q_LAG_LB_KEY"},
        [DNX_FIELD_QUAL_LATENCY_FLOW_ID]                                = {"Q_LATENCY_FLOW_ID"},
        [DNX_FIELD_QUAL_LATENCY_FLOW_ID_VALID]                          = {"Q_LATENCY_FLOW_ID_VALID"},
        [DNX_FIELD_QUAL_LB_KEY]                                         = {"Q_LB_KEY"},
        [DNX_FIELD_QUAL_LEARN_DATA]                                     = {"Q_LEARN_DATA"},
        [DNX_FIELD_QUAL_LEARN_EXPECTED_WON]                             = {"Q_LEARN_EXPECTED_WON"},
        [DNX_FIELD_QUAL_LEARN_EXT]                                      = {"Q_LEARN_EXT"},
        [DNX_FIELD_QUAL_LEARN_EXT_DATA]                                 = {"Q_LEARN_EXT_DATA"},
        [DNX_FIELD_QUAL_LEARN_EXT_SRC_MAC]                              = {"Q_LEARN_EXT_SRC_MAC"},
        [DNX_FIELD_QUAL_LEARN_EXT_VLAN]                                 = {"Q_LEARN_EXT_VLAN"},
        [DNX_FIELD_QUAL_LEARN_EXT_VSI]                                  = {"Q_LEARN_EXT_VSI"},
        [DNX_FIELD_QUAL_LEARN_FOUND]                                    = {"Q_LEARN_FOUND"},
        [DNX_FIELD_QUAL_LEARN_INFO]                                     = {"Q_LEARN_INFO"},
        [DNX_FIELD_QUAL_LEARN_MATCH]                                    = {"Q_LEARN_MATCH"},
        [DNX_FIELD_QUAL_LEARN_SRC_MAC]                                  = {"Q_LEARN_SRC_MAC"},
        [DNX_FIELD_QUAL_LEARN_STATION_MOVE]                             = {"Q_LEARN_STATION_MOVE"},
        [DNX_FIELD_QUAL_LEARN_VALID]                                    = {"Q_LEARN_VALID"},
        [DNX_FIELD_QUAL_LEARN_VLAN]                                     = {"Q_LEARN_VLAN"},
        [DNX_FIELD_QUAL_LEARN_VSI]                                      = {"Q_LEARN_VSI"},
        [DNX_FIELD_QUAL_LFSR_1]                                         = {"Q_LFSR_1"},
        [DNX_FIELD_QUAL_LFSR_2]                                         = {"Q_LFSR_2"},
        [DNX_FIELD_QUAL_LIF_EXT]                                        = {"Q_LIF_EXT"},
        [DNX_FIELD_QUAL_LIF_OAM_TRAP_PROFILE]                           = {"Q_LIF_OAM_TRAP_PROFILE"},
        [DNX_FIELD_QUAL_LPM_A1_FOUND]                                   = {"Q_LPM_A1_FOUND"},
        [DNX_FIELD_QUAL_LPM_A1_LENGTH]                                  = {"Q_LPM_A1_LENGTH"},
        [DNX_FIELD_QUAL_LPM_A2_FOUND]                                   = {"Q_LPM_A2_FOUND"},
        [DNX_FIELD_QUAL_LPM_A2_LENGTH]                                  = {"Q_LPM_A2_LENGTH"},
        [DNX_FIELD_QUAL_LOCAL_OUT_LIF]                                  = {"Q_LOCAL_OUT_LIF"},
        [DNX_FIELD_QUAL_MACT_ENTRY_GROUPING]                            = {"Q_MACT_ENTRY_GROUPING"},
        [DNX_FIELD_QUAL_MDL]                                            = {"Q_MDL"},
        [DNX_FIELD_QUAL_MEM_SOFT_ERR]                                   = {"Q_MEM_SOFT_ERR"},
        [DNX_FIELD_QUAL_MIRROR_CODE]                                    = {"Q_MIRROR_CODE"},
        [DNX_FIELD_QUAL_MIRROR_QUALIFIER]                               = {"Q_MIRROR_QUALIFIER"},
        [DNX_FIELD_QUAL_MKX_OPCODE]                                     = {"Q_MKX_OPCODE"},
        [DNX_FIELD_QUAL_MP_PROFILE_SEL]                                 = {"Q_MP_PROFILE_SEL"},
        [DNX_FIELD_QUAL_MY_CFM_MAC]                                     = {"Q_MY_CFM_MAC"},
        [DNX_FIELD_QUAL_NASID]                                          = {"Q_NASID"},
        [DNX_FIELD_QUAL_NATIVE_AC_IN_LIF_WIDE_DATA]                     = {"Q_NATIVE_AC_IN_LIF_WIDE_DATA"},
        [DNX_FIELD_QUAL_NAUID]                                          = {"Q_NAUID"},
        [DNX_FIELD_QUAL_NOF_VALID_LM_LIFS]                              = {"Q_NOF_VALID_LM_LIFS"},
        [DNX_FIELD_QUAL_NOF_VALID_PACKET_BYTES]                         = {"Q_NOF_VALID_PACKET_BYTES"},
        [DNX_FIELD_QUAL_NWK_KEY]                                        = {"Q_NWK_KEY"},
        [DNX_FIELD_QUAL_NWK_LB_KEY]                                     = {"Q_NWK_LB_KEY"},
        [DNX_FIELD_QUAL_NWK_QOS]                                        = {"Q_NWK_QOS"},
        [DNX_FIELD_QUAL_OAM_ID]                                         = {"Q_OAM_ID"},
        [DNX_FIELD_QUAL_OAM_METER_DISABLE]                              = {"Q_OAM_METER_DISABLE"},
        [DNX_FIELD_QUAL_OAM_OFFSET]                                     = {"Q_OAM_OFFSET"},
        [DNX_FIELD_QUAL_OAM_OPCODE]                                     = {"Q_OAM_OPCODE"},
        [DNX_FIELD_QUAL_OAM_STAMP_OFFSET]                               = {"Q_OAM_STAMP_OFFSET"},
        [DNX_FIELD_QUAL_OAM_STATISTICS_OBJ_0]                           = {"Q_OAM_STATISTICS_OBJ_0"},
        [DNX_FIELD_QUAL_OAM_STATISTICS_OBJ_1]                           = {"Q_OAM_STATISTICS_OBJ_1"},
        [DNX_FIELD_QUAL_OAM_STATISTICS_OBJ_2]                           = {"Q_OAM_STATISTICS_OBJ_2"},
        [DNX_FIELD_QUAL_OAM_SUB_TYPE]                                   = {"Q_OAM_SUB_TYPE"},
        [DNX_FIELD_QUAL_OAM_UP_MEP]                                     = {"Q_OAM_UP_MEP"},
        [DNX_FIELD_QUAL_OAM_YOUR_DISCR]                                 = {"Q_OAM_YOUR_DISCR"},
        [DNX_FIELD_QUAL_OUT_LIF_PROFILE]                                = {"Q_OUT_LIF_PROFILE"},
        [DNX_FIELD_QUAL_OUT_LIF_PROFILE_0]                              = {"Q_OUT_LIF_PROFILE_0"},
        [DNX_FIELD_QUAL_OUT_LIF_PROFILE_1]                              = {"Q_OUT_LIF_PROFILE_1"},
        [DNX_FIELD_QUAL_OUT_LIF_PROFILE_2]                              = {"Q_OUT_LIF_PROFILE_2"},
        [DNX_FIELD_QUAL_OUT_LIF_PROFILE_3]                              = {"Q_OUT_LIF_PROFILE_3"},
        [DNX_FIELD_QUAL_OUT_LIF_PROFILE_LFSR_1_IN_RANGE]                = {"Q_OUT_LIF_PROFILE_LFSR_1_IN_RANGE"},
        [DNX_FIELD_QUAL_OUT_LIF_PROFILE_LFSR_2_IN_RANGE]                = {"Q_OUT_LIF_PROFILE_LFSR_2_IN_RANGE"},
        [DNX_FIELD_QUAL_OUT_LIF_PROFILE_PMF_QUAL_0]                     = {"Q_OUT_LIF_PROFILE_PMF_QUAL_0"},
        [DNX_FIELD_QUAL_OUT_LIF_PROFILE_PMF_QUAL_1]                     = {"Q_OUT_LIF_PROFILE_PMF_QUAL_1"},
        [DNX_FIELD_QUAL_OUT_LIF_PROFILE_PMF_QUAL_2]                     = {"Q_OUT_LIF_PROFILE_PMF_QUAL_2"},
        [DNX_FIELD_QUAL_OUT_LIF_PROFILE_PMF_QUAL_3]                     = {"Q_OUT_LIF_PROFILE_PMF_QUAL_3"},
        [DNX_FIELD_QUAL_OUT_PP_PORT]                                    = {"Q_OUT_PP_PORT"},
        [DNX_FIELD_QUAL_OUT_PP_PORT_LFSR_1_IN_RANGE]                    = {"Q_OUT_PP_PORT_LFSR_1_IN_RANGE"},
        [DNX_FIELD_QUAL_OUT_PP_PORT_LFSR_2_IN_RANGE]                    = {"Q_OUT_PP_PORT_LFSR_2_IN_RANGE"},
        [DNX_FIELD_QUAL_OUT_TM_PORT]                                    = {"Q_OUT_TM_PORT"},
        [DNX_FIELD_QUAL_PACKET_HEADER_SIZE]                             = {"Q_PACKET_HEADER_SIZE"},
        [DNX_FIELD_QUAL_PACKET_HEADER_SIZE_RANGE]                       = {"Q_PACKET_HEADER_SIZE_RANGE"},
        [DNX_FIELD_QUAL_PACKET_IS_APPLET]                               = {"Q_PACKET_IS_APPLET"},
        [DNX_FIELD_QUAL_PACKET_IS_BFD]                                  = {"Q_PACKET_IS_BFD"},
        [DNX_FIELD_QUAL_PACKET_IS_BIER]                                 = {"Q_PACKET_IS_BIER"},
        [DNX_FIELD_QUAL_PACKET_IS_COMPATIBLE_MC]                        = {"Q_PACKET_IS_COMPATIBLE_MC"},
        [DNX_FIELD_QUAL_PACKET_IS_IEEE1588]                             = {"Q_PACKET_IS_IEEE1588"},
        [DNX_FIELD_QUAL_PACKET_IS_OAM]                                  = {"Q_PACKET_IS_OAM"},
        [DNX_FIELD_QUAL_PER_PORT_TABLE_DATA]                            = {"Q_PER_PORT_TABLE_DATA"},
        [DNX_FIELD_QUAL_PMF1_DIRECT_ACTION]                             = {"Q_PMF1_DIRECT_ACTION"},
        [DNX_FIELD_QUAL_PMF1_EXEM_ACTION]                               = {"Q_PMF1_EXEM_ACTION"},
        [DNX_FIELD_QUAL_PMF1_TCAM_ACTION_0]                             = {"Q_PMF1_TCAM_ACTION_0"},
        [DNX_FIELD_QUAL_PMF1_TCAM_ACTION_1]                             = {"Q_PMF1_TCAM_ACTION_1"},
        [DNX_FIELD_QUAL_PMF1_TCAM_ACTION_2]                             = {"Q_PMF1_TCAM_ACTION_2"},
        [DNX_FIELD_QUAL_PMF1_TCAM_ACTION_3]                             = {"Q_PMF1_TCAM_ACTION_3"},
        [DNX_FIELD_QUAL_PMF_ETH_RIF_PROFILE]                            = {"Q_PMF_ETH_RIF_PROFILE"},
        [DNX_FIELD_QUAL_PPH_END_OF_PACKET_EDITING]                      = {"Q_PPH_END_OF_PACKET_EDITING"},
        [DNX_FIELD_QUAL_PPH_FHEI_SIZE]                                  = {"Q_PPH_FHEI_SIZE"},
        [DNX_FIELD_QUAL_PPH_FWD_DOMAIN]                                 = {"Q_PPH_FWD_DOMAIN"},
        [DNX_FIELD_QUAL_PPH_FWD_LAYER_ADDITIONAL_INFO]                  = {"Q_PPH_FWD_LAYER_ADDITIONAL_INFO"},
        [DNX_FIELD_QUAL_PPH_FWD_LAYER_INDEX]                            = {"Q_PPH_FWD_LAYER_INDEX"},
        [DNX_FIELD_QUAL_PPH_FWD_STRENGTH]                               = {"Q_PPH_FWD_STRENGTH"},
        [DNX_FIELD_QUAL_PPH_IN_LIF]                                     = {"Q_PPH_IN_LIF"},
        [DNX_FIELD_QUAL_PPH_IN_LIF_PROFILE]                             = {"Q_PPH_IN_LIF_PROFILE"},
        [DNX_FIELD_QUAL_PPH_LEARN_EXT_PRESENT]                          = {"Q_PPH_LEARN_EXT_PRESENT"},
        [DNX_FIELD_QUAL_PPH_LIF_EXT_TYPE]                               = {"Q_PPH_LIF_EXT_TYPE"},
        [DNX_FIELD_QUAL_PPH_NWK_QOS]                                    = {"Q_PPH_NWK_QOS"},
        [DNX_FIELD_QUAL_PPH_PARSING_START_OFFSET]                       = {"Q_PPH_PARSING_START_OFFSET"},
        [DNX_FIELD_QUAL_PPH_PARSING_START_TYPE]                         = {"Q_PPH_PARSING_START_TYPE"},
        [DNX_FIELD_QUAL_PPH_RESERVED]                                   = {"Q_PPH_RESERVED"},
        [DNX_FIELD_QUAL_PPH_TTL]                                        = {"Q_PPH_TTL"},
        [DNX_FIELD_QUAL_PPH_TYPE]                                       = {"Q_PPH_TYPE"},
        [DNX_FIELD_QUAL_PPH_VALUE1]                                     = {"Q_PPH_VALUE1"},
        [DNX_FIELD_QUAL_PPH_VALUE2]                                     = {"Q_PPH_VALUE2"},
        [DNX_FIELD_QUAL_PPH_VRF_VALUE]                                  = {"Q_PPH_VRF_VALUE"},
        [DNX_FIELD_QUAL_PP_DSP]                                         = {"Q_PP_DSP"},
        [DNX_FIELD_QUAL_PP_PORT]                                        = {"Q_PP_PORT"},
        [DNX_FIELD_QUAL_PP_PORT_PMF_GENERAL_DATA]                       = {"Q_PP_PORT_PMF_GENERAL_DATA"},
        [DNX_FIELD_QUAL_PP_PORT_PMF_PROFILE]                            = {"Q_PP_PORT_PMF_PROFILE"},
        [DNX_FIELD_QUAL_PROGRAM_INDEX]                                  = {"Q_PROGRAM_INDEX"},
        [DNX_FIELD_QUAL_PRT_QUALIFIER]                                  = {"Q_PRT_QUALIFIER"},
        [DNX_FIELD_QUAL_PTC]                                            = {"Q_PTC"},
        [DNX_FIELD_QUAL_PTC_KEY_GEN_VAR]                                = {"Q_PTC_KEY_GEN_VAR"},
        [DNX_FIELD_QUAL_PWE_IN_LIF_WIDE_DATA]                           = {"Q_PWE_IN_LIF_WIDE_DATA"},
        [DNX_FIELD_QUAL_RPF_DEFAULT_ROUTE_FOUND]                        = {"Q_RPF_DEFAULT_ROUTE_FOUND"},
        [DNX_FIELD_QUAL_RPF_DST]                                        = {"Q_RPF_DST"},
        [DNX_FIELD_QUAL_RPF_DST_VALID]                                  = {"Q_RPF_DST_VALID"},
        [DNX_FIELD_QUAL_RPF_ECMP_GROUP]                                 = {"Q_RPF_ECMP_GROUP"},
        [DNX_FIELD_QUAL_RPF_ECMP_MODE]                                  = {"Q_RPF_ECMP_MODE"},
        [DNX_FIELD_QUAL_RPF_OPAQUE_FIELD]                               = {"Q_RPF_OPAQUE_FIELD"},
        [DNX_FIELD_QUAL_RPF_OUT_LIF]                                    = {"Q_RPF_OUT_LIF"},
        [DNX_FIELD_QUAL_RPF_ROUTE_VALID]                                = {"Q_RPF_ROUTE_VALID"},
        [DNX_FIELD_QUAL_SAVED_CONTEXT_PROFILE]                          = {"Q_SAVED_CONTEXT_PROFILE"},
        [DNX_FIELD_QUAL_SLB_KEY]                                        = {"Q_SLB_KEY"},
        [DNX_FIELD_QUAL_SLB_LEARN_NEEDED]                               = {"Q_SLB_LEARN_NEEDED"},
        [DNX_FIELD_QUAL_SLB_PAYLOAD]                                    = {"Q_SLB_PAYLOAD"},
        [DNX_FIELD_QUAL_SLLB_MEMBER_REFERENCE]                          = {"Q_SLLB_MEMBER_REFERENCE"},
        [DNX_FIELD_QUAL_SNOOP_ACTION_PROFILE_INDEX]                     = {"Q_SNOOP_ACTION_PROFILE_INDEX"},
        [DNX_FIELD_QUAL_SNOOP_CODE]                                     = {"Q_SNOOP_CODE"},
        [DNX_FIELD_QUAL_SNOOP_QUALIFIER]                                = {"Q_SNOOP_QUALIFIER"},
        [DNX_FIELD_QUAL_SNOOP_STRENGTH]                                 = {"Q_SNOOP_STRENGTH"},
        [DNX_FIELD_QUAL_SOCST]                                          = {"Q_SOCST"},
        [DNX_FIELD_QUAL_SONST]                                          = {"Q_SONST"},
        [DNX_FIELD_QUAL_SRC_SYS_PORT]                                   = {"Q_SRC_SYS_PORT"},
        [DNX_FIELD_QUAL_STACKING_EXT]                                   = {"Q_STACKING_EXT"},
        [DNX_FIELD_QUAL_STACKING_ROUTE_HISTORY_BITMAP]                  = {"Q_STACKING_ROUTE_HISTORY_BITMAP"},
        [DNX_FIELD_QUAL_STATE_TABLE_ADDRESS]                            = {"Q_STATE_TABLE_ADDRESS"},
        [DNX_FIELD_QUAL_STATE_TABLE_DATA]                               = {"Q_STATE_TABLE_DATA"},
        [DNX_FIELD_QUAL_STATE_TABLE_DATA_WRITE]                         = {"Q_STATE_TABLE_DATA_WRITE"},
        [DNX_FIELD_QUAL_STATISTICAL_SAMPLING_CODE]                      = {"Q_STATISTICAL_SAMPLING_CODE"},
        [DNX_FIELD_QUAL_STATISTICAL_SAMPLING_QUALIFIER]                 = {"Q_STATISTICAL_SAMPLING_QUALIFIER"},
        [DNX_FIELD_QUAL_STATISTICS_ATR_0]                               = {"Q_STATISTICS_ATR_0"},
        [DNX_FIELD_QUAL_STATISTICS_ATR_1]                               = {"Q_STATISTICS_ATR_1"},
        [DNX_FIELD_QUAL_STATISTICS_ATR_2]                               = {"Q_STATISTICS_ATR_2"},
        [DNX_FIELD_QUAL_STATISTICS_ATR_3]                               = {"Q_STATISTICS_ATR_3"},
        [DNX_FIELD_QUAL_STATISTICS_ATR_4]                               = {"Q_STATISTICS_ATR_4"},
        [DNX_FIELD_QUAL_STATISTICS_ATR_5]                               = {"Q_STATISTICS_ATR_5"},
        [DNX_FIELD_QUAL_STATISTICS_ATR_6]                               = {"Q_STATISTICS_ATR_6"},
        [DNX_FIELD_QUAL_STATISTICS_ATR_7]                               = {"Q_STATISTICS_ATR_7"},
        [DNX_FIELD_QUAL_STATISTICS_ATR_8]                               = {"Q_STATISTICS_ATR_8"},
        [DNX_FIELD_QUAL_STATISTICS_ATR_9]                               = {"Q_STATISTICS_ATR_9"},
        [DNX_FIELD_QUAL_STATISTICS_ATR_10]                              = {"Q_STATISTICS_ATR_10"},
        [DNX_FIELD_QUAL_STATISTICS_ATR_11]                              = {"Q_STATISTICS_ATR_11"},
        [DNX_FIELD_QUAL_STATISTICS_ID_0]                                = {"Q_STATISTICS_ID_0"},
        [DNX_FIELD_QUAL_STATISTICS_ID_1]                                = {"Q_STATISTICS_ID_1"},
        [DNX_FIELD_QUAL_STATISTICS_ID_2]                                = {"Q_STATISTICS_ID_2"},
        [DNX_FIELD_QUAL_STATISTICS_ID_3]                                = {"Q_STATISTICS_ID_3"},
        [DNX_FIELD_QUAL_STATISTICS_ID_4]                                = {"Q_STATISTICS_ID_4"},
        [DNX_FIELD_QUAL_STATISTICS_ID_5]                                = {"Q_STATISTICS_ID_5"},
        [DNX_FIELD_QUAL_STATISTICS_ID_6]                                = {"Q_STATISTICS_ID_6"},
        [DNX_FIELD_QUAL_STATISTICS_ID_7]                                = {"Q_STATISTICS_ID_7"},
        [DNX_FIELD_QUAL_STATISTICS_ID_8]                                = {"Q_STATISTICS_ID_8"},
        [DNX_FIELD_QUAL_STATISTICS_ID_9]                                = {"Q_STATISTICS_ID_9"},
        [DNX_FIELD_QUAL_STATISTICS_ID_10]                               = {"Q_STATISTICS_ID_10"},
        [DNX_FIELD_QUAL_STATISTICS_ID_11]                               = {"Q_STATISTICS_ID_11"},
        [DNX_FIELD_QUAL_STATISTICS_META_DATA]                           = {"Q_STATISTICS_META_DATA"},
        [DNX_FIELD_QUAL_STATISTICS_OBJECT10]                            = {"Q_STATISTICS_OBJECT10"},
        [DNX_FIELD_QUAL_STATISTICS_OBJ_0]                               = {"Q_STATISTICS_OBJ_0"},
        [DNX_FIELD_QUAL_STATISTICS_OBJ_1]                               = {"Q_STATISTICS_OBJ_1"},
        [DNX_FIELD_QUAL_STATISTICS_OBJ_2]                               = {"Q_STATISTICS_OBJ_2"},
        [DNX_FIELD_QUAL_STATISTICS_OBJ_3]                               = {"Q_STATISTICS_OBJ_3"},
        [DNX_FIELD_QUAL_STATISTICS_OBJ_4]                               = {"Q_STATISTICS_OBJ_4"},
        [DNX_FIELD_QUAL_STATISTICS_OBJ_5]                               = {"Q_STATISTICS_OBJ_5"},
        [DNX_FIELD_QUAL_STATISTICS_OBJ_6]                               = {"Q_STATISTICS_OBJ_6"},
        [DNX_FIELD_QUAL_STATISTICS_OBJ_7]                               = {"Q_STATISTICS_OBJ_7"},
        [DNX_FIELD_QUAL_STATISTICS_OBJ_8]                               = {"Q_STATISTICS_OBJ_8"},
        [DNX_FIELD_QUAL_STATISTICS_OBJ_9]                               = {"Q_STATISTICS_OBJ_9"},
        [DNX_FIELD_QUAL_STATISTICS_OBJ_10]                              = {"Q_STATISTICS_OBJ_10"},
        [DNX_FIELD_QUAL_STATISTICS_OBJ_11]                              = {"Q_STATISTICS_OBJ_11"},
        [DNX_FIELD_QUAL_STATISTICS_OBJ_12]                              = {"Q_STATISTICS_OBJ_12"},
        [DNX_FIELD_QUAL_STAT_OBJ_LM_READ_INDEX]                         = {"Q_STAT_OBJ_LM_READ_INDEX"},
        [DNX_FIELD_QUAL_ST_VSQ_PTR]                                     = {"Q_ST_VSQ_PTR"},
        [DNX_FIELD_QUAL_SYSTEM_HEADERS_SIZE]                            = {"Q_SYSTEM_HEADERS_SIZE"},
        [DNX_FIELD_QUAL_SYSTEM_HEADER_PROFILE_INDEX]                    = {"Q_SYSTEM_HEADER_PROFILE_INDEX"},
        [DNX_FIELD_QUAL_TC]                                             = {"Q_TC"},
        [DNX_FIELD_QUAL_TC_MAP_PROFILE]                                 = {"Q_TC_MAP_PROFILE"},
        [DNX_FIELD_QUAL_TM_DESTINATION_EXT]                             = {"Q_TM_DESTINATION_EXT"},
        [DNX_FIELD_QUAL_TM_FLOW_ATR]                                    = {"Q_TM_FLOW_ATR"},
        [DNX_FIELD_QUAL_TM_FLOW_ID]                                     = {"Q_TM_FLOW_ID"},
        [DNX_FIELD_QUAL_TM_PORT_PMF_PROFILE]                            = {"Q_TM_PORT_PMF_PROFILE"},
        [DNX_FIELD_QUAL_TM_PROFILE]                                     = {"Q_TM_PROFILE"},
        [DNX_FIELD_QUAL_TRACE_PACKET]                                   = {"Q_TRACE_PACKET"},
        [DNX_FIELD_QUAL_TRJ_HASH]                                       = {"Q_TRJ_HASH"},
        [DNX_FIELD_QUAL_TSH_EXT]                                        = {"Q_TSH_EXT"},
        [DNX_FIELD_QUAL_UDH_BASE_ENTRY_PUSH_PROFILE]                    = {"Q_UDH_BASE_ENTRY_PUSH_PROFILE"},
        [DNX_FIELD_QUAL_UDH_BASE_PRP_CS_VAR]                            = {"Q_UDH_BASE_PRP_CS_VAR"},
        [DNX_FIELD_QUAL_UNKNOWN_ADDRESS]                                = {"Q_UNKNOWN_ADDRESS"},
        [DNX_FIELD_QUAL_UPDATED_TPIDS_PACKET_DATA]                      = {"Q_UPDATED_TPIDS_PACKET_DATA"},
        [DNX_FIELD_QUAL_USER_HEADERS_TYPE]                              = {"Q_USER_HEADERS_TYPE"},
        [DNX_FIELD_QUAL_USER_HEADER_1]                                  = {"Q_USER_HEADER_1"},
        [DNX_FIELD_QUAL_USER_HEADER_1_TYPE]                             = {"Q_USER_HEADER_1_TYPE"},
        [DNX_FIELD_QUAL_USER_HEADER_2]                                  = {"Q_USER_HEADER_2"},
        [DNX_FIELD_QUAL_USER_HEADER_2_TYPE]                             = {"Q_USER_HEADER_2_TYPE"},
        [DNX_FIELD_QUAL_USER_HEADER_3]                                  = {"Q_USER_HEADER_3"},
        [DNX_FIELD_QUAL_USER_HEADER_3_TYPE]                             = {"Q_USER_HEADER_3_TYPE"},
        [DNX_FIELD_QUAL_USER_HEADER_4]                                  = {"Q_USER_HEADER_4"},
        [DNX_FIELD_QUAL_USER_HEADER_4_TYPE]                             = {"Q_USER_HEADER_4_TYPE"},
        [DNX_FIELD_QUAL_VID_VALID]                                      = {"Q_VID_VALID"},
        [DNX_FIELD_QUAL_VISIBILITY]                                     = {"Q_VISIBILITY"},
        [DNX_FIELD_QUAL_VISIBILITY_CLEAR]                               = {"Q_VISIBILITY_CLEAR"},
        [DNX_FIELD_QUAL_VLAN_EDIT_CMD]                                  = {"Q_VLAN_EDIT_CMD"},
        [DNX_FIELD_QUAL_VLAN_EDIT_CMD_INDEX]                            = {"Q_VLAN_EDIT_CMD_INDEX"},
        [DNX_FIELD_QUAL_VLAN_EDIT_DEI_1]                                = {"Q_VLAN_EDIT_DEI_1"},
        [DNX_FIELD_QUAL_VLAN_EDIT_DEI_2]                                = {"Q_VLAN_EDIT_DEI_2"},
        [DNX_FIELD_QUAL_VLAN_EDIT_PCP_1]                                = {"Q_VLAN_EDIT_PCP_1"},
        [DNX_FIELD_QUAL_VLAN_EDIT_PCP_2]                                = {"Q_VLAN_EDIT_PCP_2"},
        [DNX_FIELD_QUAL_VLAN_EDIT_PCP_DEI_1]                            = {"Q_VLAN_EDIT_PCP_DEI_1"},
        [DNX_FIELD_QUAL_VLAN_EDIT_PCP_DEI_2]                            = {"Q_VLAN_EDIT_PCP_DEI_2"},
        [DNX_FIELD_QUAL_VLAN_EDIT_VID_1]                                = {"Q_VLAN_EDIT_VID_1"},
        [DNX_FIELD_QUAL_VLAN_EDIT_VID_2]                                = {"Q_VLAN_EDIT_VID_2"},
        [DNX_FIELD_QUAL_VRF_VALUE]                                      = {"Q_VRF_VALUE"},
        [DNX_FIELD_QUAL_VSI_UNKNOWN_DA_DESTINATION]                     = {"Q_VSI_UNKNOWN_DA_DESTINATION"},
        [DNX_FIELD_QUAL_VTT_OAM_LIF_0]                                  = {"Q_VTT_OAM_LIF_0"},
        [DNX_FIELD_QUAL_VTT_OAM_LIF_0_VALID]                            = {"Q_VTT_OAM_LIF_0_VALID"},
        [DNX_FIELD_QUAL_VTT_OAM_LIF_1]                                  = {"Q_VTT_OAM_LIF_1"},
        [DNX_FIELD_QUAL_VTT_OAM_LIF_1_VALID]                            = {"Q_VTT_OAM_LIF_1_VALID"},
        [DNX_FIELD_QUAL_VTT_OAM_LIF_2]                                  = {"Q_VTT_OAM_LIF_2"},
        [DNX_FIELD_QUAL_VTT_OAM_LIF_2_VALID]                            = {"Q_VTT_OAM_LIF_2_VALID"},
        [DNX_FIELD_QUAL_VTT_OAM_PCP_0]                                  = {"Q_VTT_OAM_PCP_0"},
        [DNX_FIELD_QUAL_VTT_OAM_PCP_1]                                  = {"Q_VTT_OAM_PCP_1"},
        [DNX_FIELD_QUAL_VTT_OAM_PCP_2]                                  = {"Q_VTT_OAM_PCP_2"},
        [DNX_FIELD_QUAL_VW_MEMBER_REFERENCE]                            = {"Q_VW_MEMBER_REFERENCE"},
        [DNX_FIELD_QUAL_VW_PCC_HIT]                                     = {"Q_VW_PCC_HIT"},
        [DNX_FIELD_QUAL_VW_VIP_ID]                                      = {"Q_VW_VIP_ID"},
        [DNX_FIELD_QUAL_VW_VIP_VALID]                                   = {"Q_VW_VIP_VALID"},
        [DNX_FIELD_QUAL_WEAK_TM_PROFILE]                                = {"Q_WEAK_TM_PROFILE"},
        [DNX_FIELD_QUAL_WEAK_TM_VALID]                                  = {"Q_WEAK_TM_VALID"},
        [DNX_FIELD_QUAL_WEXEM_KEY_0]                                    = {"Q_WEXEM_KEY_0"},
        [DNX_FIELD_QUAL_WEXEM_KEY_1]                                    = {"Q_WEXEM_KEY_1"},
        [DNX_FIELD_QUAL_WEXEM_KEY_2]                                    = {"Q_WEXEM_KEY_2"},
        [DNX_FIELD_QUAL_WEXEM_KEY_3]                                    = {"Q_WEXEM_KEY_3"},
        [DNX_FIELD_QUAL_WEXEM_KEY_4]                                    = {"Q_WEXEM_KEY_4"},
        [DNX_FIELD_QUAL_WEXEM_KEY_J]                                    = {"Q_WEXEM_KEY_J"},
        [DNX_FIELD_QUAL_WEXEM_LEARN_APPDB]                              = {"Q_WEXEM_LEARN_APPDB"},
        [DNX_FIELD_QUAL_WEXEM_LEARN_NEEDED]                             = {"Q_WEXEM_LEARN_NEEDED"},
        [DNX_FIELD_QUAL_WEXEM_PAYLOAD_1]                                = {"Q_WEXEM_PAYLOAD_1"},
        [DNX_FIELD_QUAL_WEXEM_PAYLOAD_2]                                = {"Q_WEXEM_PAYLOAD_2"},
        [DNX_FIELD_QUAL_WEXEM_RESULT]                                   = {"Q_WEXEM_RESULT"},
        [DNX_FIELD_QUAL_FER_STATISTICS_OBJ_ID_0]                        = {"Q_FER_STATISTICS_OBJ_ID_0"},
        [DNX_FIELD_QUAL_FER_STATISTICS_OBJ_ID_1]                        = {"Q_FER_STATISTICS_OBJ_ID_1"},
        [DNX_FIELD_QUAL_FER_STATISTICS_OBJ_ID_2]                        = {"Q_FER_STATISTICS_OBJ_ID_2"},
        [DNX_FIELD_QUAL_L3SRCBIND_HIT]                                  = {"Q_L3SrcBind_HIT"},
        [DNX_FIELD_QUAL_SRV6_GSID_NEXT_SI]                              = {"Q_SRV6_GSID_NEXT_SI"},
        [DNX_FIELD_QUAL_SRV6_GSID_NEXT_GSID]                            = {"Q_SRV6_GSID_NEXT_GSID"},
};
/* *INDENT-ON* */

#define DNX_FIELD_QUAL_SIZE_VERIFY(qual_class,qual_id,size)                                                      \
{                                                                                                                \
    if((size < 1) || (size > dnx_data_field.qual.max_bits_in_qual_get(unit)))                                    \
    {                                                                                                            \
        SHR_ERR_EXIT(_SHR_E_PARAM, "Qualifier class:%s id: qual:%s size (%d) must be between %d and %d\n",    \
                    dnx_field_qual_class_text(qual_class),dnx_qual_name[qual_id].name,                             \
                    size, 1, dnx_data_field.qual.max_bits_in_qual_get(unit));                                   \
    }                                                                                                            \
}

/**
 * \brief Checks if a qualifier's mapping include value conversion.
 * \param [in] qual_map_entry_p - Pointer to qualifier mapping of type dnx_field_qual_map_t.
 * \return
 *      TRUE  - If the mapping includes value conversion.
 *      FALSE - Otherwise.
 */
#define DNX_FIELD_MAP_QUAL_MAPPING_HAS_VALUE_CONVERSION(qual_map_entry_p) \
    (qual_map_entry_p->conversion_cb != NULL)

shr_error_e
dnx_field_map_qual_layer_record_info_get(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_qual_id_t dnx_qual_id,
    dnx_field_map_qual_layer_record_pbus_info_t * lr_qual_info_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(lr_qual_info_p, _SHR_E_PARAM, "lr_qual_info_p");

    DNX_FIELD_LAYER_QUAL_VERIFY(stage, dnx_qual_id);

    if (DNX_FIELD_IS_LAYER_RECORDS_QUALS_SUPPORTED(stage) == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Layer Qualifiers not supported for stage %s\n", dnx_field_stage_text(unit, stage));
    }

    switch (stage)
    {
        case DNX_FIELD_STAGE_EXTERNAL:
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
        case DNX_FIELD_STAGE_IPMF3:
        case DNX_FIELD_STAGE_L4_OPS:
        {
            lr_qual_info_p->valid = dnx_data_field_map.qual.layer_record_info_ingress_get(unit, dnx_qual_id)->valid;
            lr_qual_info_p->size = dnx_data_field_map.qual.layer_record_info_ingress_get(unit, dnx_qual_id)->size;
            lr_qual_info_p->offset = dnx_data_field_map.qual.layer_record_info_ingress_get(unit, dnx_qual_id)->offset;
            break;
        }
        case DNX_FIELD_STAGE_EPMF:
        {
            lr_qual_info_p->valid = dnx_data_field_map.qual.layer_record_info_egress_get(unit, dnx_qual_id)->valid;
            lr_qual_info_p->size = dnx_data_field_map.qual.layer_record_info_egress_get(unit, dnx_qual_id)->size;
            lr_qual_info_p->offset = dnx_data_field_map.qual.layer_record_info_egress_get(unit, dnx_qual_id)->offset;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Stage \"%s\" not mapped in this function.\n",
                         dnx_field_stage_text(unit, stage));
        }
    }

    if ((stage == DNX_FIELD_STAGE_EXTERNAL) && (lr_qual_info_p->valid) && (dnx_qual_id != DNX_FIELD_LR_QUAL_OFFSET))
    {
        lr_qual_info_p->offset += dnx_data_field.qual.kbp_extra_offset_after_layer_record_offset_get(unit);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Gets the info for the qualifier which is relevant to the end user.
*  It fills it in qual_in_info_p.
* \param [in] unit              - Device ID.
* \param [in] bcm_qual          - BCM Qualifier ID.
* \param [out] qual_info_p      - holds all relevant info for the action, size, stage and name.
* \param [out] dbal_field_id_p  - The DBAL field ID of the user defined qualifier.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_qual_get_int(
    int unit,
    bcm_field_qualify_t bcm_qual,
    dnx_field_qualifier_in_info_t * qual_info_p,
    dbal_fields_e * dbal_field_id_p)
{
    dnx_field_qual_id_t qual_id;
    dnx_field_user_qual_info_t user_qual_info;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(qual_info_p, _SHR_E_PARAM, "qual_in_info_p");

    if (bcm_qual < dnx_data_field.qual.user_1st_get(unit) ||
        bcm_qual >= dnx_data_field.qual.user_1st_get(unit) + dnx_data_field.qual.user_nof_get(unit))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
        SHR_EXIT();
    }

    qual_id = bcm_qual - dnx_data_field.qual.user_1st_get(unit);

    SHR_IF_ERR_EXIT(dnx_field_qual_sw_db.info.get(unit, qual_id, &user_qual_info));
    if (user_qual_info.valid == FALSE)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    sal_strncpy_s(qual_info_p->name, dbal_field_to_string(unit, user_qual_info.field_id), DBAL_MAX_STRING_LENGTH);

    qual_info_p->size = user_qual_info.size;

    *dbal_field_id_p = user_qual_info.field_id;

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map_local.h
 */
shr_error_e
dnx_field_qual_vw_signal_offset(
    int unit,
    dnx_field_stage_e stage,
    char *signal_name,
    int offset_within_signal,
    int *is_qual_available_p,
    int *offset_p)
{
    int signal_index;
    int signals_nof = dnx_data_field.virtual_wire.signals_nof_get(unit);
    int found;
    int signal_offset;
    int offset_including_within_signal;
    const dnx_data_field_virtual_wire_signal_mapping_t *dnx_data_signals_struct_p;
    int mapped_qual;
    char general_data_signal_name[] = "general_data";
    const dnx_data_field_virtual_wire_signal_mapping_t *dnx_data_signals_struct_ipmf1_p;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(signal_name, _SHR_E_PARAM, "signal_name");
    SHR_NULL_CHECK(is_qual_available_p, _SHR_E_PARAM, "is_qual_available_p");
    SHR_NULL_CHECK(offset_p, _SHR_E_PARAM, "offset_p");

    /** Initialize offset to an illegal value.*/
    (*offset_p) = -1;

    found = FALSE;
    for (signal_index = 0; signal_index < signals_nof; signal_index++)
    {
        dnx_data_signals_struct_p = dnx_data_field.virtual_wire.signal_mapping_get(unit, stage, signal_index);
        mapped_qual = dnx_data_signals_struct_p->mapped_qual;
        if ((mapped_qual != DNX_FIELD_QUAL_ID_INVALID)
            && (0 == sal_strncmp(signal_name, dnx_data_signals_struct_p->signal_name, MAX_VW_NAME_LENGTH)))
        {
            found = TRUE;
            signal_offset = dnx_data_field_map.qual.params_get(unit, stage, mapped_qual)->offset;
            if (signal_offset < 0)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "Signal \"%.*s\" is mapped to qualifier %d but has no valid offset (%d) "
                             "as a regular qualifier.\r\n",
                             MAX_VW_NAME_LENGTH, signal_name, mapped_qual, signal_offset);
            }
            break;
        }
    }

    if (found)
    {
        /*
         * Special case: general data in iPMF3, only part of the signal is available.
         */
        if ((stage == DNX_FIELD_STAGE_IPMF3) &&
            (0 == sal_strncmp(general_data_signal_name,
                              (dnx_data_signals_struct_ipmf1_p =
                               dnx_data_field.virtual_wire.signal_mapping_get
                               (unit, DNX_FIELD_STAGE_IPMF1,
                                dnx_data_field.virtual_wire.ipmf1_general_data_index_get(unit)))->signal_name,
                              sizeof(general_data_signal_name))))
        {
            int max_nof_actions = dnx_data_field.virtual_wire.actions_per_signal_nof_get(unit);
            int action_id;
            dnx_field_action_t ipmf1_dnx_action;
            int reached_the_end = FALSE;
            unsigned int action_size;
            int action_lsb_on_signal;
            int sum_action_bits = 0;
            int action_index;

            for (action_index = 0; action_index < max_nof_actions; action_index++)
            {
                action_id = dnx_data_signals_struct_ipmf1_p->mapped_action[action_index];
                if (action_id <= 0)
                {
                    if (action_index == 0)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "No actions on general data.\n");
                    }
                    break;
                }

                ipmf1_dnx_action = DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF1, action_id);
                SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size
                                (unit, DNX_FIELD_STAGE_IPMF1, ipmf1_dnx_action, &action_size));
                if (action_size <= 0)
                {
                    /** Note we can continue if (action_size == 0), but such a scenario shouldn't occur.*/
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Non positive action size %d for action %d of general data.\r\n",
                                 action_size, action_index);
                }
                action_lsb_on_signal = dnx_data_signals_struct_ipmf1_p->action_offset[action_index];
                if (action_lsb_on_signal < 0)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Negative offset on signal %d for action %d of general data.\r\n",
                                 action_lsb_on_signal, action_index);
                }

                if ((offset_within_signal >= action_lsb_on_signal) &&
                    (offset_within_signal < action_lsb_on_signal + action_size))
                {
                    reached_the_end = TRUE;
                    offset_including_within_signal = signal_offset + sum_action_bits +
                        (offset_within_signal - action_lsb_on_signal);
                    break;
                }

                sum_action_bits += action_size;
            }
            if (reached_the_end == FALSE)
            {
                LOG_DEBUG_EX(BSL_LOG_MODULE, "Offset on signal %d not accessible for signal \"%s\" on iPMF3.%s%s\n",
                             offset_within_signal, general_data_signal_name, EMPTY, EMPTY);
                found = FALSE;
            }
        }
        else
        {
            /*
             * Add the offset on signal to the offset of the signal.
             */
            offset_including_within_signal = signal_offset + offset_within_signal;
        }
    }

    (*is_qual_available_p) = found;
    if (found)
    {
        (*offset_p) = offset_including_within_signal;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief  Provides all the information per DNX qualifier requested for HW configuration
 * \param [in] unit - Identifier of HW platform.
 * \param [in] stage - Stage identifier
 * \param [in] dnx_qual - DNX qualifier identifier, encoded with Qual Class
 * \param [in,out] dnx_qual_info_p - pointer to DNX qualifier info structure, data will be copied into it
 * \return
 *   \retval _SHR_E_NONE      - On success
 *   \retval _SHR_E_PARAM     - One of input parameters is out of range
 *   \retval Other - Other errors as per shr_error_e
 * \remark
 *  The assumption is that the amount of data is modest, so we can copy info data
 *  Pay attention that dnx_qual_info_t structure memory is in user responsibility
 */
static shr_error_e
dnx_field_map_qual_info(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_qual_t dnx_qual,
    dnx_field_qual_info_t * dnx_qual_info_p)
{
    dnx_field_qual_class_e dnx_qual_class = DNX_QUAL_CLASS(dnx_qual);
    dnx_field_stage_e dnx_stage = DNX_QUAL_STAGE(dnx_qual);
    int dnx_qual_id = DNX_QUAL_ID(dnx_qual);

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_qual_info_p, _SHR_E_PARAM, "dnx_qual_info_p");
    DNX_FIELD_STAGE_VERIFY(stage);
    LOG_DEBUG_EX(BSL_LOG_MODULE, "stage %d, dnx_qual 0x%08X, dnx_qual_id %d dnx_qual_class %d\n", stage,
                 (uint32) dnx_qual, dnx_qual_id, dnx_qual_class);
    /*
     * Initialize info struct, don't trust the client
     */
    sal_memset(dnx_qual_info_p, 0, sizeof(*dnx_qual_info_p));

    if (stage != dnx_stage)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Inconsistency between API:\"%s\" and encoded:\"%s\" stages\n",
                     dnx_field_stage_text(unit, stage), dnx_field_stage_text(unit, dnx_stage));
    }

    if ((stage == DNX_FIELD_STAGE_IPMF2) && (dnx_qual_class != DNX_FIELD_QUAL_CLASS_META2))
    {
        /*
         * Switch to info on IPMF1 qualifiers
         */
        stage = DNX_FIELD_STAGE_IPMF1;
    }

    dnx_qual_info_p->class = dnx_qual_class;
    dnx_qual_info_p->dnx_stage = stage;

    dnx_qual_info_p->nof_mappings = 1;

    switch (dnx_qual_class)
    {
        case DNX_FIELD_QUAL_CLASS_USER:
        {
            dnx_field_user_qual_info_t user_qual_info;
            SHR_IF_ERR_EXIT(dnx_field_qual_sw_db.info.get(unit, dnx_qual_id, &user_qual_info));
            if (user_qual_info.valid == FALSE)
            {
                /*
                 * Cannot retrieve info on a user qualifier which has not been created. Qualifier is marked as 'invalid'.
                 */
                SHR_ERR_EXIT(_SHR_E_PARAM, "DNX qualifier 0x%08X has not been created: Cannot retrieve info\r\n",
                             dnx_qual);
            }

            dnx_qual_info_p->field_id = user_qual_info.field_id;
            dnx_qual_info_p->sizes[0] = user_qual_info.size;
            /*
             * For now user defined actions aren't mapped to any qualifiers and have no mapped offset.
             * We still set it to zero explicitly, even though the sal_memset should take care of it.
             */
            dnx_qual_info_p->offsets[0] = 0;
            break;
        }
        case DNX_FIELD_QUAL_CLASS_META:
        case DNX_FIELD_QUAL_CLASS_META2:
        {
            int regular_qual_dnx_data_offset;
            /*
             * Verify that dnx_qual_id is valid for this stage
             */
            DNX_FIELD_META_QUAL_VERIFY(dnx_qual_id);

            SHR_IF_ERR_EXIT(dnx_field_qual_sw_db.predefined.
                            field_id.get(unit, dnx_qual_id, &(dnx_qual_info_p->field_id)));
            if (dnx_qual_info_p->field_id == DBAL_FIELD_EMPTY)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Meta Qualifier:%d is not supported\n", dnx_qual_id);
            }

            /*
             * Checks if a regualr metadata qualifier or a special one.
             */
            regular_qual_dnx_data_offset = dnx_data_field_map.qual.params_get(unit, stage, dnx_qual_id)->offset;
            if (regular_qual_dnx_data_offset >= 0)
            {
                /*
                 * Regular qualifier
                 * Acquire offset from DNX DATA and size from DBAL.
                 */
                dnx_qual_info_p->offsets[0] = regular_qual_dnx_data_offset;
                dnx_qual_info_p->sizes[0] = dnx_data_field_map.qual.params_get(unit, stage, dnx_qual_id)->size;
                DNX_FIELD_QUAL_SIZE_VERIFY(DNX_FIELD_QUAL_CLASS_META, dnx_qual_id, dnx_qual_info_p->sizes[0]);
            }
            else
            {
                /*
                 * Possibly special qualifier
                 * Acquire both offset and size from DNX DATA.
                 */
                int qual_part_ndx;
                int non_vm_nof_parts =
                    dnx_data_field_map.qual.special_metadata_qual_get(unit, stage, dnx_qual_id)->nof_parts;
                int total_size = 0;
                if (non_vm_nof_parts > 0)
                {
                    /*
                     * Non VW special qualifier.
                     */
                    /** Sanity check.*/
                    if (non_vm_nof_parts > DNX_FIELD_QAUL_MAX_NOF_MAPPINGS)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "DNX metadata qualifier:%s on stage %s has "
                                     "nof_parts %d, maximum is %d.\n",
                                     dbal_field_to_string(unit, dnx_qual_info_p->field_id),
                                     dnx_field_stage_text(unit, stage), non_vm_nof_parts,
                                     DNX_FIELD_QAUL_MAX_NOF_MAPPINGS);
                    }
                    for (qual_part_ndx = 0; qual_part_ndx < non_vm_nof_parts; qual_part_ndx++)
                    {
                        dnx_qual_info_p->offsets[qual_part_ndx] =
                            dnx_data_field_map.qual.special_metadata_qual_get(unit, stage,
                                                                              dnx_qual_id)->offsets[qual_part_ndx];
                        dnx_qual_info_p->sizes[qual_part_ndx] =
                            dnx_data_field_map.qual.special_metadata_qual_get(unit, stage,
                                                                              dnx_qual_id)->sizes[qual_part_ndx];
                        total_size += dnx_qual_info_p->sizes[qual_part_ndx];
                        DNX_FIELD_QUAL_SIZE_VERIFY(DNX_FIELD_QUAL_CLASS_META, dnx_qual_id,
                                                   dnx_qual_info_p->sizes[qual_part_ndx]);

                    }

                    DNX_FIELD_QUAL_SIZE_VERIFY(DNX_FIELD_QUAL_CLASS_META, dnx_qual_id, total_size);
                    dnx_qual_info_p->nof_mappings = non_vm_nof_parts;
                }
                else
                {
                    /*
                     * Possibly VW special qualifier
                     */
                    VirtualWireInfo *vw_info;
                    int is_qual_available;
                    char *vw_name =
                        dnx_data_field_map.qual.special_metadata_qual_get(unit, stage, dnx_qual_id)->vw_name;
                    if (vw_name == NULL)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "DNX metadata qualifier:%s is not supported for stage %s\n",
                                     dbal_field_to_string(unit, dnx_qual_info_p->field_id),
                                     dnx_field_stage_text(unit, stage));
                    }
                    
                    SHR_IF_ERR_EXIT(pemladrv_vw_wire_get
                                    (unit, vw_name, dnx_field_map_stage_info[stage].pp_stage, &vw_info));
                    
                    /** Sanity check.*/
                    if (vw_info->nof_mappings < 1)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "Qualifier %s on stage %s is mapped to virtual wire \"%.*s\", "
                                     "but it has %d nof_mappings.\n",
                                     dbal_field_to_string(unit, dnx_qual_info_p->field_id),
                                     dnx_field_stage_text(unit, stage), MAX_VW_NAME_LENGTH, vw_name,
                                     vw_info->nof_mappings);
                    }
                    for (qual_part_ndx = 0; qual_part_ndx < vw_info->nof_mappings; qual_part_ndx++)
                    {
                        SHR_IF_ERR_EXIT(dnx_field_qual_vw_signal_offset(unit, stage,
                                                                        vw_info->vw_mappings_arr
                                                                        [qual_part_ndx].physical_wire_name,
                                                                        vw_info->vw_mappings_arr
                                                                        [qual_part_ndx].physical_wire_lsb,
                                                                        &is_qual_available,
                                                                        &(dnx_qual_info_p->offsets[qual_part_ndx])));
                        if (is_qual_available == FALSE)
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                         "Qualifier %s on stage %s is mapped to virtual wire \"%.*s\", "
                                         "but mapping %d isn't legal.\n",
                                         dbal_field_to_string(unit, dnx_qual_info_p->field_id),
                                         dnx_field_stage_text(unit, stage), MAX_VW_NAME_LENGTH, vw_name, qual_part_ndx);
                        }
                        dnx_qual_info_p->sizes[qual_part_ndx] =
                            vw_info->vw_mappings_arr[qual_part_ndx].virtual_wire_msb -
                            vw_info->vw_mappings_arr[qual_part_ndx].virtual_wire_lsb + 1;
                        total_size += dnx_qual_info_p->sizes[qual_part_ndx];
                        DNX_FIELD_QUAL_SIZE_VERIFY(DNX_FIELD_QUAL_CLASS_META, dnx_qual_id,
                                                   dnx_qual_info_p->sizes[qual_part_ndx]);

                    }
                    /** Sanity check.*/
                    if (total_size != vw_info->width_in_bits)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "Qualifier %s on stage %s is mapped to virtual wire \"%.*s\", "
                                     "with %d parts, sizes are varianble %d and %d.\n",
                                     dbal_field_to_string(unit, dnx_qual_info_p->field_id),
                                     dnx_field_stage_text(unit, stage), MAX_VW_NAME_LENGTH, vw_name,
                                     qual_part_ndx, total_size, vw_info->width_in_bits);
                    }

                    DNX_FIELD_QUAL_SIZE_VERIFY(DNX_FIELD_QUAL_CLASS_META, dnx_qual_id, total_size);
                    dnx_qual_info_p->nof_mappings = vw_info->nof_mappings;
                }

            }
            break;
        }
        case DNX_FIELD_QUAL_CLASS_HEADER:
        {
            if (dnx_data_field_map.qual.header_get(unit, dnx_qual_id)->dbal_field == DBAL_FIELD_EMPTY)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Header Qualifier(%d) Stage:%s is not supported\n",
                             dnx_qual_id, dnx_field_stage_text(unit, stage));
            }
            if (dnx_data_field.stage.stage_info_get(unit, dnx_stage)->pbus_header_length <= 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Stage:%s does not support header qualifiers.\n",
                             dnx_field_stage_text(unit, stage));
            }
            dnx_qual_info_p->field_id = dnx_data_field_map.qual.header_get(unit, dnx_qual_id)->dbal_field;
            dnx_qual_info_p->sizes[0] = dnx_data_field_map.qual.header_get(unit, dnx_qual_id)->size;
            dnx_qual_info_p->offsets[0] = dnx_data_field_map.qual.header_get(unit, dnx_qual_id)->offset;

            break;
        }
        case DNX_FIELD_QUAL_CLASS_SW:
        {
            const dnx_field_sw_qual_info_t *sw_qual_info_p;
            sw_qual_info_p = &dnx_sw_qual_info[dnx_qual_id];
            if (sw_qual_info_p->field_id == 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "SW Qualifier:%d is not supported\n", dnx_qual_id);
            }
            dnx_qual_info_p->field_id = sw_qual_info_p->field_id;
            dnx_qual_info_p->sizes[0] = sw_qual_info_p->size;
            dnx_qual_info_p->offsets[0] = 0;
            break;
        }
        case DNX_FIELD_QUAL_CLASS_LAYER_RECORD:
        {
            dnx_field_layer_record_qual_info_t layer_qual_map_info;
            dnx_field_map_qual_layer_record_pbus_info_t lr_qual_info_dnx_data;

            SHR_IF_ERR_EXIT(dnx_field_qual_sw_db.layer_record.
                            field_id.get(unit, dnx_qual_id, &layer_qual_map_info.dbal_field_id));
            if (layer_qual_map_info.dbal_field_id == DBAL_FIELD_EMPTY)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Layer Qualifier:%d is not supported\n", dnx_qual_id);
            }
            /*
             * Obtain the information from the DNX data, with the stage determining if we use the ingress or egress
             * layer record data.
             */
            SHR_IF_ERR_EXIT(dnx_field_map_qual_layer_record_info_get(unit, stage, dnx_qual_id, &lr_qual_info_dnx_data));
            if (lr_qual_info_dnx_data.valid == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Layer Qualifier:%s(%d) at stage:%s is not supported.\n",
                             dbal_field_to_string(unit, layer_qual_map_info.dbal_field_id),
                             dnx_qual_id, dnx_field_stage_text(unit, stage));
            }
            dnx_qual_info_p->field_id = layer_qual_map_info.dbal_field_id;
            dnx_qual_info_p->sizes[0] = lr_qual_info_dnx_data.size;
            dnx_qual_info_p->offsets[0] = lr_qual_info_dnx_data.offset;
            break;
        }
        case DNX_FIELD_QUAL_CLASS_VW:
        {
            VirtualWireInfo *vw_info;
            dnx_field_vw_qual_info_t vw_qual_info;
            char vw_name[MAX_VW_NAME_LENGTH];
            int is_qual_avail;

            SHR_IF_ERR_EXIT(dnx_field_map_vw_qual_info_get(unit, dnx_qual_id, &vw_qual_info));
            if (vw_qual_info.field_id == DBAL_FIELD_EMPTY)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Virtual wire qualifier %d is not supported\n", dnx_qual_id);
            }
            if ((vw_qual_info.stage_bmp & (SAL_BIT(stage))) == 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Virtual wire qualifier %d not supported for stage:%s\n",
                             dnx_qual_id, dnx_field_stage_text(unit, stage));
            }
            sal_strncpy_s(vw_name, dbal_field_to_string(unit, vw_qual_info.field_id), MAX_VW_NAME_LENGTH);

            SHR_IF_ERR_EXIT(pemladrv_vw_wire_get(unit, vw_name, dnx_field_map_stage_info[stage].pp_stage, &vw_info));

            if (vw_info->nof_mappings != 1)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Only supports nof_mapping 1. nof_mapping is %d. Virtual wire is \"%.*s\".\n",
                             vw_info->nof_mappings, DBAL_MAX_STRING_LENGTH, vw_name);
            }

            dnx_qual_info_p->field_id = vw_qual_info.field_id;
            dnx_qual_info_p->sizes[0] = vw_info->width_in_bits;
            DNX_FIELD_QUAL_SIZE_VERIFY(DNX_FIELD_QUAL_CLASS_VW, dnx_qual_id, dnx_qual_info_p->sizes[0]);
            SHR_IF_ERR_EXIT(dnx_field_qual_vw_signal_offset(unit, stage,
                                                            vw_info->vw_mappings_arr[0].physical_wire_name,
                                                            vw_info->vw_mappings_arr[0].physical_wire_lsb,
                                                            &is_qual_avail, &(dnx_qual_info_p->offsets[0])));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported dnx qualifier class:%s(%d). Qualifier 0x%08X\n",
                         dnx_field_qual_class_text(dnx_qual_class), dnx_qual_class, (uint32) dnx_qual);
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See field_map.h
 */
CONST char *
dnx_field_bcm_qual_text(
    int unit,
    bcm_field_qualify_t bcm_qual)
{
    dnx_field_qualifier_in_info_t qual_in_info;
    dbal_fields_e dbal_field_id;
    static char *bcm_qual_text[bcmFieldQualifyCount] = BCM_FIELD_QUALIFY_STRINGS;
    int user_defined_first = dnx_data_field.qual.user_1st_get(unit);
    int user_defined_last = dnx_data_field.qual.user_1st_get(unit) + dnx_data_field.qual.user_nof_get(unit) - 1;
    int vw_first = dnx_data_field.qual.vw_1st_get(unit);
    int vw_last = dnx_data_field.qual.vw_1st_get(unit) + dnx_data_field.qual.vw_nof_get(unit) - 1;

    if ((int) bcm_qual >= 0 && bcm_qual < bcmFieldQualifyCount)
    {
        const char *bcm_qual_name = NULL;
        /** Switch case to handle the unambiguous BCM qualifiers. */
        switch (bcm_qual)
        {
            case bcmFieldQualifyL4PortRangeCheck:
            {
                bcm_qual_name = "L4PortRangeCheck";
                break;
            }
            case bcmFieldQualifyIp6NextHeader:
            {
                bcm_qual_name = "Ip6NextHeader";
                break;
            }
            case bcmFieldQualifyIp6TrafficClass:
            {
                bcm_qual_name = "Ip6TrafficClass";
                break;
            }
            case bcmFieldQualifyIp6HopLimit:
            {
                bcm_qual_name = "Ip6HopLimit";
                break;
            }
            case bcmFieldQualifyInnerIp6HopLimit:
            {
                bcm_qual_name = "InnerIp6HopLimit";
                break;
            }
            case bcmFieldQualifyInnerIp6TrafficClass:
            {
                bcm_qual_name = "InnerIp6TrafficClass";
                break;
            }
            case bcmFieldQualifyInnerIp6NextHeader:
            {
                bcm_qual_name = "InnerIp6NextHeader";
                break;
            }
            default:
            {
                bcm_qual_name = bcm_qual_text[bcm_qual];
                break;
            }
        }
        /** Predefined qualifier.*/
        if (ISEMPTY(bcm_qual_name))
        {
            return "Invalid BCM qualifier";
        }
        return bcm_qual_name;
    }
    else if ((bcm_qual >= user_defined_first) && (bcm_qual <= user_defined_last))
    {
        /** User defined qualifier.*/
        if (dnx_field_qual_get_int(unit, bcm_qual, &qual_in_info, &dbal_field_id) == _SHR_E_NONE)
        {
            return dbal_field_to_string(unit, dbal_field_id);
        }
        else
        {
            return "Invalid BCM qualifier";
        }
    }
    else if ((bcm_qual >= vw_first) && (bcm_qual <= vw_last))
    {
        /** Virtual wire qualifier.*/
        dnx_field_vw_qual_info_t vw_qual_info;
        if (dnx_field_map_vw_qual_info_get(unit, bcm_qual - vw_first, &vw_qual_info) != _SHR_E_NONE)
        {
            return "Invalid BCM qualifier";
        }
        dbal_field_id = vw_qual_info.field_id;
        if (dbal_field_id != DBAL_FIELD_EMPTY)
        {
            return dbal_field_to_string(unit, dbal_field_id);
        }
        else
        {
            return "Invalid BCM qualifier";
        }
    }
    else
    {
        return "Invalid BCM qualifier";
    }
}

/*
 * See field_map.h
 */
CONST char *
dnx_field_dnx_qual_text(
    int unit,
    dnx_field_qual_t dnx_qual)
{
    dnx_field_qual_info_t dnx_qual_info;

    if (dnx_field_map_qual_info(unit, DNX_QUAL_STAGE(dnx_qual), dnx_qual, &dnx_qual_info) != _SHR_E_NONE)
        return "Invalid DNX Qualifier";

    return dbal_field_to_string(unit, dnx_qual_info.field_id);
}

/*
 * See field_map.h
 */
char *
dnx_field_qual_class_text(
    dnx_field_qual_class_e qual_class)
{
    char *class_name;
    if ((qual_class < DNX_FIELD_QUAL_CLASS_FIRST) || (qual_class >= DNX_FIELD_QUAL_CLASS_NOF))
    {
        class_name = "Invalid Class";
    }
    else if (dnx_field_qual_class_names[qual_class] == NULL)
    {
        class_name = "Unnamed Class";
    }
    else
    {
        class_name = (char *) dnx_field_qual_class_names[qual_class];
    }

    return class_name;
}

/*
 * See field_map.h
 */
char *
dnx_field_input_type_text(
    dnx_field_input_type_e input_type)
{
    char *input_type_name;
    if ((input_type < DNX_FIELD_INPUT_TYPE_LAYER_FWD) || (input_type >= DNX_FIELD_INPUT_TYPE_NOF))
    {
        input_type_name = "Invalid Input Type";
    }
    else if (dnx_field_input_types_names[input_type] == NULL)
    {
        input_type_name = "Unnamed Input Type";
    }
    else
    {
        input_type_name = (char *) dnx_field_input_types_names[input_type];
    }

    return input_type_name;
}

/*
 * See header in field_action.h
 */
shr_error_e
dnx_field_qual_in_info_init(
    int unit,
    dnx_field_qualifier_in_info_t * qual_info_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(qual_info_p, _SHR_E_PARAM, "qual_info_p");

    sal_memset(qual_info_p, 0, sizeof(*qual_info_p));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_qual_ref_dec(
    int unit,
    dnx_field_qual_t qual)
{
    uint32 fg_refs = 0;
    int id = DNX_QUAL_ID(qual);
    uint32 valid;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_qual_sw_db.info.valid.get(unit, id, &valid));
    if (valid == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Cannot decrement ref count. Qualifier 0x%08X has not been created.\r\n", qual);
    }
    SHR_IF_ERR_EXIT(dnx_field_qual_sw_db.info.ref_count.get(unit, id, &fg_refs));
    if (fg_refs <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Qualifier id %d didnt have any Field grops ref's (unused)\n", qual);
    }
    fg_refs = fg_refs - 1;
    SHR_IF_ERR_EXIT(dnx_field_qual_sw_db.info.ref_count.set(unit, id, fg_refs));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_qual_ref_inc(
    int unit,
    dnx_field_qual_t qual)
{
    uint32 fg_refs = 0;
    int id = DNX_QUAL_ID(qual);
    uint32 valid;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_qual_sw_db.info.valid.get(unit, id, &valid));
    if (valid == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Cannot increment ref count. Qualifier 0x%08X has not been created.\r\n", qual);
    }
    SHR_IF_ERR_EXIT(dnx_field_qual_sw_db.info.ref_count.get(unit, id, &fg_refs));
    fg_refs = fg_refs + 1;
    SHR_IF_ERR_EXIT(dnx_field_qual_sw_db.info.ref_count.set(unit, id, fg_refs));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Clear the dnx_field_user_qual_info_t, set it to preferred init values
* \param [in] unit              - Device ID
* \param [in] user_qual_info    - Pointer to input structure of user_qual_info that needs to be init
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_user_qual_info_t_init(
    int unit,
    dnx_field_user_qual_info_t * user_qual_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(user_qual_info, _SHR_E_PARAM, "user_qual_info");

    sal_memset(user_qual_info, 0x0, sizeof(*user_qual_info));

    user_qual_info->field_id = DBAL_FIELD_EMPTY;
    user_qual_info->bcm_id = bcmFieldQualifyCount;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_qual_create(
    int unit,
    dnx_field_qual_flags_e flags,
    dnx_field_qualifier_in_info_t * qual_info_p,
    bcm_field_qualify_t * bcm_qual_p)
{
    dnx_field_qual_id_t qual_id;
    int alloc_flag = 0;
    bcm_field_qualify_t bcm_qual = bcmFieldQualifyCount;
    dnx_field_user_qual_info_t user_qual_info;
    char field_name[DBAL_MAX_STRING_LENGTH];

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(qual_info_p, _SHR_E_PARAM, "qual_info_p");
    SHR_NULL_CHECK(bcm_qual_p, _SHR_E_PARAM, "bcm_qual_p");

    SHR_IF_ERR_EXIT(dnx_field_user_qual_info_t_init(unit, &user_qual_info));

    if ((qual_info_p->size < 1) || (qual_info_p->size > dnx_data_field.qual.max_bits_in_qual_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Qualifier class:User, input_size (%d), must be between %d and %d\n",
                     qual_info_p->size, 1, dnx_data_field.qual.max_bits_in_qual_get(unit));
    }

    if (flags & DNX_FIELD_QUALIFIER_FLAG_WITH_ID)
    {
        uint32 valid;
        /*
         * BCM Qualifier is leading number, we'll extract DNX one from BCM
         */
        bcm_qual = *bcm_qual_p;
        if ((bcm_qual < dnx_data_field.qual.user_1st_get(unit)) ||
            (bcm_qual >= dnx_data_field.qual.user_1st_get(unit) + dnx_data_field.qual.user_nof_get(unit)))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "BCM Qual ID:%d is out of range:%d-%d\n", bcm_qual,
                         dnx_data_field.qual.user_1st_get(unit),
                         dnx_data_field.qual.user_1st_get(unit) + dnx_data_field.qual.user_nof_get(unit));
        }
        qual_id = bcm_qual - dnx_data_field.qual.user_1st_get(unit);

        SHR_IF_ERR_EXIT(dnx_field_qual_sw_db.info.valid.get(unit, qual_id, &valid));
        if (valid == TRUE)
        {
            /*
             * Cannot create a qualifier which is already marked as 'valid' (i.e., Has already
             * been created).
             */
            SHR_ERR_EXIT(_SHR_E_PARAM, "BCM Qual ID:%d was already created\n", bcm_qual);
        }
        alloc_flag = DNX_ALGO_RES_ALLOCATE_WITH_ID;
    }

    SHR_IF_ERR_EXIT(dnx_algo_field_user_qual_id_allocate(unit, alloc_flag, &qual_id));
    /**Sets the user qualifier name and generate DBAL field ID */
    sal_strncpy_s(field_name, qual_info_p->name, DBAL_MAX_STRING_LENGTH);
    SHR_IF_ERR_EXIT(dbal_fields_field_create(unit, DBAL_FIELD_TYPE_DEF_UINT, field_name, &user_qual_info.field_id));

    if (!(flags & DNX_FIELD_ACTION_FLAG_WITH_ID))
    {
        bcm_qual = dnx_data_field.qual.user_1st_get(unit) + qual_id;
        *bcm_qual_p = bcm_qual;
    }

    user_qual_info.flags = flags;
    user_qual_info.ref_count = 0;
    user_qual_info.size = qual_info_p->size;
    user_qual_info.valid = TRUE;
    user_qual_info.bcm_id = bcm_qual;

    /**If allocation did not fail we can set the values and SW state*/
    SHR_IF_ERR_EXIT(dnx_field_qual_sw_db.info.set(unit, qual_id, &user_qual_info));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_qual_destroy(
    int unit,
    bcm_field_qualify_t bcm_qual)
{
    dnx_field_qual_id_t qual_id;
    dnx_field_user_qual_info_t user_qual_info_get;
    dnx_field_user_qual_info_t user_qual_info_set;

    SHR_FUNC_INIT_VARS(unit);

    if ((bcm_qual < dnx_data_field.qual.user_1st_get(unit)) ||
        (bcm_qual >= dnx_data_field.qual.user_1st_get(unit) + dnx_data_field.qual.user_nof_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "BCM Qual ID:%d is out of range:%d-%d\n", bcm_qual,
                     dnx_data_field.qual.user_1st_get(unit),
                     dnx_data_field.qual.user_1st_get(unit) + dnx_data_field.qual.user_nof_get(unit));
    }

    qual_id = bcm_qual - dnx_data_field.qual.user_1st_get(unit);

    SHR_IF_ERR_EXIT(dnx_field_qual_sw_db.info.get(unit, qual_id, &user_qual_info_get));

    if (user_qual_info_get.valid == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Cannot Destroy BCM qual 0x%08X. Qualifier is invalid. Qual id %d.\n", bcm_qual, qual_id);
    }
    if (user_qual_info_get.ref_count > 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Can't Destroy, Num of Field groups references = %d\n",
                     user_qual_info_get.ref_count);
    }

    SHR_IF_ERR_EXIT(dnx_algo_field_user_qual_id_deallocate(unit, qual_id));
    SHR_IF_ERR_EXIT(dbal_fields_field_destroy(unit, user_qual_info_get.field_id));

    SHR_IF_ERR_EXIT(dnx_field_user_qual_info_t_init(unit, &user_qual_info_set));

    SHR_IF_ERR_EXIT(dnx_field_qual_sw_db.info.set(unit, qual_id, &user_qual_info_set));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_qual_get(
    int unit,
    bcm_field_qualify_t bcm_qual,
    dnx_field_qualifier_in_info_t * qual_info_p)
{
    dbal_fields_e dbal_field_id;
    _shr_error_t rv;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(qual_info_p, _SHR_E_PARAM, "qual_info_p");

    rv = dnx_field_qual_get_int(unit, bcm_qual, qual_info_p, &dbal_field_id);
    if (rv == _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The user defined qualifier (%d) is not within range (%d-%d)!",
                     bcm_qual, dnx_data_field.qual.user_1st_get(unit),
                     dnx_data_field.qual.user_1st_get(unit) + dnx_data_field.qual.user_nof_get(unit) - 1);
    }
    else if (rv == _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "The user defined qualifier (%d) does not exist!", bcm_qual);
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_map_qual_bcm_to_dnx_int(
    int unit,
    dnx_field_stage_e dnx_stage,
    uint32 print_errors,
    bcm_field_qualify_t bcm_qual,
    dnx_field_qual_t * dnx_qual_p,
    const dnx_field_qual_map_t ** qual_map_entry_pp)
{
    dnx_field_qual_t dnx_qual;
    uint8 is_std_1;
    field_data_conversion_cb conversion_cb;
    int bare_metal_support;
    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_STAGE_VERIFY(dnx_stage);
    SHR_NULL_CHECK(dnx_qual_p, _SHR_E_PARAM, "dnx_qual_p");
    SHR_NULL_CHECK(qual_map_entry_pp, _SHR_E_PARAM, "qual_map_entry_pp");

    (*qual_map_entry_pp) = NULL;

    /*
     * Internal map routine - no need for verifications - all done level above
     */
    /*
     * First check if the qualifier is user defined one
     */
    if ((bcm_qual >= dnx_data_field.qual.user_1st_get(unit)) &&
        (bcm_qual < dnx_data_field.qual.user_1st_get(unit) + dnx_data_field.qual.user_nof_get(unit)))
    {
        uint32 valid;
        dnx_field_qual_id_t qual_id = bcm_qual - dnx_data_field.qual.user_1st_get(unit);
        /*
         * Verify that qualifier was already allocated
         */
        SHR_IF_ERR_EXIT(dnx_field_qual_sw_db.info.valid.get(unit, qual_id, &valid));
        if (valid == FALSE)
        {
            if (print_errors)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                             "User defined BCM Qualifier %s(%d) doesn't exist!%s",
                             dnx_field_bcm_qual_text(unit, bcm_qual), bcm_qual, EMPTY);
            }
            else
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
        }
        /*
         * To obtain DNX from BCM just subtract 1st
         */
        *dnx_qual_p = DNX_QUAL(DNX_FIELD_QUAL_CLASS_USER, dnx_stage, qual_id);
    }
    else if (((int) bcm_qual >= 0) && (bcm_qual < bcmFieldQualifyCount))
    {
        (*qual_map_entry_pp) = &dnx_global_qual_map[bcm_qual];
        /*
         * Predefined qualifier.
         */
        /*
         * Now check check global list - if not found go to per stage one
         */
        dnx_qual = (*qual_map_entry_pp)->dnx_qual;
        conversion_cb = (*qual_map_entry_pp)->conversion_cb;
        if (dnx_qual == DNX_FIELD_QUAL_INVALID)
        {
            (*qual_map_entry_pp) = &dnx_field_map_stage_info[dnx_stage].meta_qual_map[bcm_qual];
            /*
             * First check native metadata
             */
            dnx_qual = (*qual_map_entry_pp)->dnx_qual;
            if (dnx_qual == DNX_FIELD_QUAL_ID_INVALID)
            {
                /*
                 * Native not found, if it is IPMF2 try IPMF1 metadata
                 */
                if (dnx_stage == DNX_FIELD_STAGE_IPMF2)
                {
                    (*qual_map_entry_pp) = &dnx_field_map_stage_info[DNX_FIELD_STAGE_IPMF1].meta_qual_map[bcm_qual];
                    dnx_qual = (*qual_map_entry_pp)->dnx_qual;
                    if (dnx_qual == DNX_FIELD_QUAL_ID_INVALID)
                    {
                        if (print_errors)
                        {
                            SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                                         "BCM Qualifier %s is not supported for stages: %s!\n "
                                         "To see the supported qualifiers, run: field qualifier predefined BCM stage=%s",
                                         dnx_field_bcm_qual_text(unit, bcm_qual), dnx_field_stage_text(unit,
                                                                                                       dnx_stage),
                                         dnx_field_stage_text(unit, dnx_stage));
                        }
                        else
                        {
                            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                            SHR_EXIT();
                        }
                    }
                    /*
                     * Qualifier found in IPMF1 - assign META for class
                     */
                    dnx_qual |= (DNX_FIELD_QUAL_CLASS_META << DNX_QUAL_CLASS_SHIFT);
                    conversion_cb = (*qual_map_entry_pp)->conversion_cb;
                }
                else
                {
                    if (print_errors)
                    {
                        SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                                     "BCM Qualifier %s is not supported for stage: %s!\n "
                                     "To see the supported qualifiers, run: field qualifier predefined BCM stage=%s",
                                     dnx_field_bcm_qual_text(unit, bcm_qual), dnx_field_stage_text(unit, dnx_stage),
                                     dnx_field_stage_text(unit, dnx_stage));
                    }
                    else
                    {
                        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                        SHR_EXIT();
                    }
                }
            }
            else
            {
                /*
                 * Found - add class - it will be always metadata, or metadata2 for iPMF2.
                 */
                if (dnx_stage == DNX_FIELD_STAGE_IPMF2)
                {
                    dnx_qual |= (DNX_FIELD_QUAL_CLASS_META2 << DNX_QUAL_CLASS_SHIFT);
                }
                else
                {
                    dnx_qual |= (DNX_FIELD_QUAL_CLASS_META << DNX_QUAL_CLASS_SHIFT);
                }
                conversion_cb = (*qual_map_entry_pp)->conversion_cb;
            }
        }
        else
        {
            dnx_field_map_qual_layer_record_pbus_info_t lr_qual_info_dnx_data;
            /*
             * For header qualifiers, check if the stage has a part of the header on pbus.
             */
            if ((DNX_QUAL_CLASS(dnx_qual) == DNX_FIELD_QUAL_CLASS_HEADER) &&
                (dnx_data_field.stage.stage_info_get(unit, dnx_stage)->pbus_header_length <= 0))
            {
                if (print_errors)
                {
                    SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                                 "BCM Qualifier %s(%d) is a header qualifier, header qualifiers are not supported on stage: %s!",
                                 dnx_field_bcm_qual_text(unit, bcm_qual), bcm_qual, dnx_field_stage_text(unit,
                                                                                                         dnx_stage));
                }
                else
                {
                    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                    SHR_EXIT();
                }
            }

            /*
             * For layer records, check if valid.
             */
            if (DNX_QUAL_CLASS(dnx_qual) == DNX_FIELD_QUAL_CLASS_LAYER_RECORD)
            {
                if (DNX_FIELD_IS_LAYER_RECORDS_QUALS_SUPPORTED(dnx_stage) == FALSE)
                {
                    if (print_errors)
                    {
                        SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                                     "BCM Qualifier %s(%d) is a layer record qualifier. "
                                     "Layer record qualifiers are not supported on stage: %s.",
                                     dnx_field_bcm_qual_text(unit, bcm_qual), bcm_qual,
                                     dnx_field_stage_text(unit, dnx_stage));
                    }
                    else
                    {
                        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                        SHR_EXIT();
                    }
                }
                SHR_IF_ERR_EXIT(dnx_field_map_qual_layer_record_info_get
                                (unit, dnx_stage, DNX_QUAL_ID(dnx_qual), &lr_qual_info_dnx_data));
                if (lr_qual_info_dnx_data.valid == FALSE)
                {
                    if (print_errors)
                    {
                        SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                                     "Layer record BCM Qualifier %s(%d) is not supported on stage: %s!",
                                     dnx_field_bcm_qual_text(unit, bcm_qual), bcm_qual, dnx_field_stage_text(unit,
                                                                                                             dnx_stage));
                    }
                    else
                    {
                        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                        SHR_EXIT();
                    }
                }
            }
        }

        SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_std_1));
        if (!is_std_1 && (conversion_cb != NULL))
        {
            SHR_IF_ERR_EXIT(dnx_field_bare_metal_support_check(unit, conversion_cb, &bare_metal_support));
            if (!bare_metal_support)
            {
                if (print_errors)
                {
                    SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                                 "BCM Qualifier %s(%d) is not supported in BareMetal mode!%s",
                                 dnx_field_bcm_qual_text(unit, bcm_qual), bcm_qual, EMPTY);
                }
                else
                {
                    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                    SHR_EXIT();
                }
            }
        }

        *dnx_qual_p = dnx_qual | (dnx_stage << DNX_QUAL_STAGE_SHIFT);
    }
    else if ((bcm_qual >= dnx_data_field.qual.vw_1st_get(unit)) &&
             (bcm_qual < dnx_data_field.qual.vw_1st_get(unit) + dnx_data_field.qual.vw_nof_get(unit)))
    {
        /*
         * Virtual wire qualifer.
         */
        dnx_field_vw_qual_info_t vw_qual_info;
        dnx_field_qual_id_t qual_id = bcm_qual - dnx_data_field.qual.vw_1st_get(unit);
        SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_std_1));
        if (is_std_1)
        {
            if (print_errors)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                             "BCM Qualifier %s(%d) is a virtual wire qualifier. All virtual wires are not supported on standard 1!",
                             dnx_field_bcm_qual_text(unit, bcm_qual), bcm_qual);
            }
            else
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
        }
        SHR_IF_ERR_EXIT(dnx_field_map_vw_qual_info_get(unit, qual_id, &vw_qual_info));
        if (vw_qual_info.field_id == DBAL_FIELD_EMPTY)
        {
            if (print_errors)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                             "Virtual wire BCM Qualifier %s(%d) does not exist!%s",
                             dnx_field_bcm_qual_text(unit, bcm_qual), bcm_qual, EMPTY);
            }
            else
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
        }
        if ((vw_qual_info.stage_bmp & (SAL_BIT(dnx_stage))) == 0)
        {
            if (print_errors)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                             "Virtual wire BCM Qualifier %s(%d) is not supported for stage!%s",
                             dnx_field_bcm_qual_text(unit, bcm_qual), bcm_qual, dnx_field_stage_text(unit, dnx_stage));
            }
            else
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
        }

        (*dnx_qual_p) = DNX_QUAL(DNX_FIELD_QUAL_CLASS_VW, dnx_stage, qual_id);

    }
    else
    {
        /*
         * Not in any legal range.
         */
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "BCM Qualifier %s(%d) is not a legal BCM id!%s",
                     dnx_field_bcm_qual_text(unit, bcm_qual), bcm_qual, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_map_qual_bcm_to_dnx(
    int unit,
    dnx_field_stage_e dnx_stage,
    bcm_field_qualify_t bcm_qual,
    dnx_field_qual_t * dnx_qual_p)
{
    const dnx_field_qual_map_t *qual_map_entry_p = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_map_qual_bcm_to_dnx_int(unit, dnx_stage, TRUE, bcm_qual, dnx_qual_p, &qual_map_entry_p));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_map_qual_info_bcm_to_dnx(
    int unit,
    bcm_core_t core,
    dnx_field_stage_e dnx_stage,
    bcm_field_entry_qual_t * bcm_qual_info,
    dnx_field_entry_qual_t * dnx_qual_info)
{
    const dnx_field_qual_map_t *qual_map_entry_p = NULL;
    int i_data;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_qual_info, _SHR_E_PARAM, "bcm_qual_info");
    SHR_NULL_CHECK(dnx_qual_info, _SHR_E_PARAM, "dnx_qual_info");
    /*
     * First get dnx qualifiers
     */
    SHR_IF_ERR_EXIT(dnx_field_map_qual_bcm_to_dnx_int
                    (unit, dnx_stage, TRUE, bcm_qual_info->type, &dnx_qual_info->dnx_qual, &qual_map_entry_p));
    /*
     * Now fill values, if it was user defined or no conversion callback on static on, just copy from bcm to dnx
     * meanwhile conversion does not deal with mask - just copy it - but may be in the future.
     */
    if (qual_map_entry_p != NULL && DNX_FIELD_MAP_QUAL_MAPPING_HAS_VALUE_CONVERSION(qual_map_entry_p))
    {
        SHR_IF_ERR_EXIT_WITH_LOG(qual_map_entry_p->conversion_cb
                                 (unit, 0, core, bcm_qual_info->value, dnx_qual_info->qual_value),
                                 "BCM Qualifier %s(%d) failed to convert value.%s\n",
                                 dnx_field_bcm_qual_text(unit, bcm_qual_info->type), bcm_qual_info->type, EMPTY);
    }
    else
    {
        for (i_data = 0; i_data < BCM_FIELD_QUAL_WIDTH_IN_WORDS; i_data++)
        {
            dnx_qual_info->qual_value[i_data] = bcm_qual_info->value[i_data];
            dnx_qual_info->qual_max_value[i_data] = bcm_qual_info->max_value[i_data];
        }
    }
    for (i_data = 0; i_data < BCM_FIELD_QUAL_WIDTH_IN_WORDS; i_data++)
    {
        dnx_qual_info->qual_mask[i_data] = bcm_qual_info->mask[i_data];
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_map_qual_has_conversion(
    int unit,
    bcm_field_stage_t bcm_stage,
    bcm_field_qualify_t bcm_qual,
    int *has_conversion_p)
{
    const dnx_field_qual_map_t *qual_map_entry_p = NULL;
    dnx_field_stage_e dnx_stage;
    dnx_field_qual_t dnx_qual;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(has_conversion_p, _SHR_E_PARAM, "has_conversion_p");

    SHR_IF_ERR_EXIT(dnx_field_map_stage_bcm_to_dnx(unit, bcm_stage, &dnx_stage));

    SHR_IF_ERR_EXIT(dnx_field_map_qual_bcm_to_dnx_int(unit, dnx_stage, TRUE, bcm_qual, &dnx_qual, &qual_map_entry_p));

    if (qual_map_entry_p != NULL && DNX_FIELD_MAP_QUAL_MAPPING_HAS_VALUE_CONVERSION(qual_map_entry_p))
    {
        (*has_conversion_p) = TRUE;
    }
    else
    {
        (*has_conversion_p) = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_map_qual_dnx_to_bcm_global_get(
    int unit,
    dnx_field_qual_id_t qual_id,
    dnx_field_qual_class_e qual_class,
    bcm_field_qualify_t * bcm_qual_p,
    uint8 *found_p)
{
    bcm_field_qualify_t i_bcm_qual;
    const dnx_field_qual_map_t *global_qual_map_p;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(bcm_qual_p, _SHR_E_INTERNAL, "bcm_qual_p");
    SHR_NULL_CHECK(found_p, _SHR_E_INTERNAL, "found_p");

    (*found_p) = FALSE;

    /*
     * First look for basic object later look for no CB
     */
    for (i_bcm_qual = 0; i_bcm_qual < bcmFieldQualifyCount; i_bcm_qual++)
    {
        global_qual_map_p = &dnx_global_qual_map[i_bcm_qual];
        if (global_qual_map_p->flags & BCM_TO_DNX_ADDITIONAL_OBJ)
        {
            continue;
        }
        if ((qual_class == DNX_QUAL_CLASS(global_qual_map_p->dnx_qual)) &&
            (qual_id == DNX_QUAL_ID(global_qual_map_p->dnx_qual)))
        {
            *bcm_qual_p = i_bcm_qual;
            *found_p = TRUE;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_qual_dnx_to_bcm_meta_get(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_qual_id_t qual_id,
    bcm_field_qualify_t * bcm_qual_p,
    uint8 *found_p)
{
    const dnx_field_qual_map_t *meta_qual_map_p;
    bcm_field_qualify_t i_bcm_qual;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(bcm_qual_p, _SHR_E_INTERNAL, "bcm_qual_p");
    SHR_NULL_CHECK(found_p, _SHR_E_INTERNAL, "found_p");

    (*found_p) = FALSE;

    /*
     * First look for basic object later look for no CB
     */
    for (i_bcm_qual = 0; i_bcm_qual < bcmFieldQualifyCount; i_bcm_qual++)
    {
        meta_qual_map_p = &dnx_field_map_stage_info[stage].meta_qual_map[i_bcm_qual];
        /*
         * If Additional means not main object
         */
        if (meta_qual_map_p->flags & BCM_TO_DNX_ADDITIONAL_OBJ)
        {
            continue;
        }
        if (qual_id == meta_qual_map_p->dnx_qual)
        {
            *bcm_qual_p = i_bcm_qual;
            *found_p = TRUE;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_map_qual_dnx_to_bcm_int(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_qual_t dnx_qual,
    bcm_field_qualify_t * bcm_qual_p,
    uint8 *found_p)
{
    dnx_field_qual_class_e dnx_qual_class = DNX_QUAL_CLASS(dnx_qual);
    dnx_field_qual_id_t dnx_qual_id = DNX_QUAL_ID(dnx_qual);
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_qual_p, _SHR_E_PARAM, "bcm_qual_p");
    SHR_NULL_CHECK(found_p, _SHR_E_PARAM, "found_p");
    DNX_FIELD_STAGE_VERIFY(stage);

    if (DNX_QUAL_STAGE(dnx_qual) != stage)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "DNX qualifer 0x%x has encoded stage %d (\"%s\"), "
                     "but the function is called with stage %d (\"%s\").\n",
                     dnx_qual, DNX_QUAL_STAGE(dnx_qual), dnx_field_stage_text(unit, DNX_QUAL_STAGE(dnx_qual)),
                     stage, dnx_field_stage_text(unit, stage));
    }

    switch (dnx_qual_class)
    {
        case DNX_FIELD_QUAL_CLASS_USER:
        {
            dnx_field_user_qual_info_t user_qual_info;
            SHR_IF_ERR_EXIT(dnx_field_qual_sw_db.info.get(unit, dnx_qual_id, &user_qual_info));
            if (user_qual_info.valid == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "DNX qualifier 0x%08X has not been created: Cannot retrieve info\r\n",
                             dnx_qual);
            }
            *bcm_qual_p = user_qual_info.bcm_id;
            *found_p = TRUE;
            break;
        }
        case DNX_FIELD_QUAL_CLASS_META:
        {
            dnx_field_stage_e meta_stage;
            /*
             * iPMF2 Meta is actualy what iPMF1 can read throught initial keys
             */
            if (stage == DNX_FIELD_STAGE_IPMF2)
            {
                meta_stage = DNX_FIELD_STAGE_IPMF1;
            }
            else
            {
                meta_stage = stage;
            }
            SHR_IF_ERR_EXIT(dnx_field_qual_dnx_to_bcm_meta_get(unit, meta_stage, dnx_qual_id, bcm_qual_p, found_p));
            break;
        }
        case DNX_FIELD_QUAL_CLASS_META2:
        {
            /*
             * First need to look in the iPMF1 qua
             */
            if (stage != DNX_FIELD_STAGE_IPMF2)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Class:%s , not suppoeted for stage:%s. tried to access with  Qualifier 0x%08X\n",
                             dnx_field_qual_class_text(dnx_qual_class), dnx_field_stage_e_get_name(stage),
                             (uint32) dnx_qual);
            }
            SHR_IF_ERR_EXIT(dnx_field_qual_dnx_to_bcm_meta_get(unit, stage, dnx_qual_id, bcm_qual_p, found_p));
            break;
        }
        case DNX_FIELD_QUAL_CLASS_HEADER:
        case DNX_FIELD_QUAL_CLASS_LAYER_RECORD:
        {
            SHR_IF_ERR_EXIT(dnx_field_map_qual_dnx_to_bcm_global_get
                            (unit, dnx_qual_id, dnx_qual_class, bcm_qual_p, found_p));
            break;
        }
        case DNX_FIELD_QUAL_CLASS_SW:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Dont have BCM qualifiers for class:%s(%d). Qualifier 0x%08X\n",
                         dnx_field_qual_class_text(dnx_qual_class), dnx_qual_class, (uint32) dnx_qual);

            break;
        }
        case DNX_FIELD_QUAL_CLASS_VW:
        {

            *bcm_qual_p = dnx_data_field.qual.vw_1st_get(unit) + dnx_qual_id;
            *found_p = TRUE;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported dnx qualifier class:%s(%d). Qualifier 0x%08X\n",
                         dnx_field_qual_class_text(dnx_qual_class), dnx_qual_class, (uint32) dnx_qual);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_map_qual_dnx_to_bcm(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_qual_t dnx_qual,
    bcm_field_qualify_t * bcm_qual_p)
{
    uint8 found;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_qual_p, _SHR_E_PARAM, "bcm_qual_p");
    DNX_FIELD_STAGE_VERIFY(stage);

    SHR_IF_ERR_EXIT(dnx_field_map_qual_dnx_to_bcm_int(unit, stage, dnx_qual, bcm_qual_p, &found));

    if (found == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "DNX qualifer 0x%x qual_id %d class \"%s\", stage \"%s\", no BCM mapping found.\n",
                     dnx_qual, DNX_QUAL_ID(dnx_qual), dnx_field_qual_class_text(DNX_QUAL_CLASS(dnx_qual)),
                     dnx_field_stage_text(unit, stage));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_map_qual_input_type_to_ranges(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_input_type_e input_type,
    int offset,
    uint8 *ranges_p)
{
    const dnx_field_qual_input_type_info_t *input_type_info_p;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ranges_p, _SHR_E_PARAM, "ranges_p");

    /*
     * Note that input type const can lead to "all zeros" and/or "all ones". since "all zeros" doesn't use FFC and we
     * don't care about the ranges for it, we treat input type const as if it is necessarily referring to all ones,
     * thus just like Metadata.
     */
    input_type_info_p = &dnx_input_type_info[input_type];
    if (input_type_info_p->valid == TRUE)
    {
        (*ranges_p) = input_type_info_p->ranges;
        if ((stage == DNX_FIELD_STAGE_IPMF2) && (input_type != DNX_FIELD_INPUT_TYPE_META_DATA2))
        {
            /*
             * All but native metadata for iPMF2 is taken from the initial keys, and we can't use the 4th range for them.
             */
            (*ranges_p) &= SAL_RBIT(3);
        }
        /*
         * The second range (Metadata) cannot access the header part of the PBUS.
         */
        if ((input_type == DNX_FIELD_INPUT_TYPE_META_DATA) &&
            (offset < dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->pbus_header_length))
        {
            (*ranges_p) &= SAL_RBIT(1);
        }
    }
    else
    {
        /*
         * All stages but iPMF1 and iPMF2 have one range, and don't use the 'ranges' bitmap.
         * Input type zero doesn't use FFCs at all.
         */
        (*ranges_p) = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_map_qual_key_to_ranges(
    int unit,
    dnx_field_stage_e stage,
    dbal_enum_value_field_field_key_e key_id,
    uint8 *ranges_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ranges_p, _SHR_E_PARAM, "ranges_p");

    (*ranges_p) = 0;

    if (stage == DNX_FIELD_STAGE_IPMF1 && key_id >= dnx_data_field.stage.stage_info_get(unit, stage)->nof_keys_alloc)
    {
        /*
         * The fourth range is forbidden
         */
        (*ranges_p) |= SAL_RBIT(3);
    }
    else
    {
        /*
         * All ranges are allowed (and there are no ranges).
         */
        (*ranges_p) |= (~0);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_map_layer_index_modulo(
    int unit,
    dnx_field_stage_e stage,
    int layer_index_in,
    int *layer_index_out)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(layer_index_out, _SHR_E_PARAM, "layer_index_out");
    DNX_FIELD_STAGE_VERIFY(stage);

    if (dnx_data_field.stage.stage_info_get(unit, stage)->nof_layer_records == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Stage %s does not have any layer records.\n", dnx_field_stage_text(unit, stage));
    }
    /*
     * Note that we can perform now:
     * (*layer_index_out)=layer_index_in%(dnx_data_field.stage.stage_info_get(unit, stage)->nof_layer_records);
     * Instead we only support negative layer indices, not real modulo for out of range ones,
     * and perform the next line:
     */
    (*layer_index_out) =
        (layer_index_in >= 0) ?
        layer_index_in : (layer_index_in + dnx_data_field.stage.stage_info_get(unit, stage)->nof_layer_records);

    /*
     * Test if we are out of range (as we don't actually perform modulo).
     */
    DNX_FIELD_LAYER_INDEX_VERIFY(stage, *layer_index_out);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_map_layer_record_offset(
    int unit,
    dnx_field_stage_e stage,
    int layer_index,
    int *offset_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(offset_p, _SHR_E_PARAM, "offset_p");
    DNX_FIELD_STAGE_VERIFY(stage);

    DNX_FIELD_LAYER_INDEX_VERIFY(stage, layer_index);

    if (dnx_data_field.stage.stage_info_get(unit, stage)->nof_layer_records == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Stage %s does not have any layer records.\n", dnx_field_stage_text(unit, stage));
    }

    *offset_p = dnx_data_field.stage.stage_info_get(unit, stage)->pbus_header_length +
        layer_index * dnx_data_field.stage.stage_info_get(unit, stage)->layer_record_size;
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_qual_list(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_qual_t ** dnx_qual_list_p,
    int *dnx_qual_num_p)
{
    int i_info, i_qual = 0;
    dnx_field_qual_class_e qual_class;
    /*
     * dnx_qual_list will be a list of DNX qualifiers usually used for diagnostics
     */
    dnx_field_qual_t *dnx_qual_list = NULL;
    /*
     * this parameter will represent absolute max number of qualifier per stage
     */
    int dnx_qual_max = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_qual_list_p, _SHR_E_PARAM, "dnx_qual_list_p");
    SHR_NULL_CHECK(dnx_qual_num_p, _SHR_E_PARAM, "dnx_qual_num_p");

    /*
     * First Step - figure out how many qualifiers do we have per stage
     */
    dnx_qual_max = DNX_FIELD_QUAL_NOF - 1;

    if (dnx_data_field.stage.stage_info_get(unit, stage)->pbus_header_length > 0)
    {
        dnx_qual_max += (DNX_FIELD_HEADER_QUAL_NOF - 1);
    }

    if (DNX_FIELD_IS_LAYER_RECORDS_QUALS_SUPPORTED(stage))
    {
        dnx_qual_max += (DNX_FIELD_LR_QUAL_NOF - 1);
    }
    /*
     * Allocate space for list of DNX qualifiers
     */
    SHR_ALLOC_ERR_EXIT(dnx_qual_list, dnx_qual_max * sizeof(dnx_field_qual_t), "dnx_qual_list", "%s%s%s",
                       "Error allocating memory of dnx qual list for stage:\"", dnx_field_stage_text(unit, stage),
                       "\"");

    /*
     * Now using the same conditional logic as above go through all qualifiers and assign DNX quals
     */

    /*
     * Add the META qualifiers for the stages ipmf1, ipmf2, ipmf3. For ipmf2 the META2 quals will be added
     * */
    for (i_info = DNX_FIELD_QUAL_FIRST; i_info < DNX_FIELD_QUAL_NOF; i_info++)
    {

        if (dnx_data_field_map.qual.params_get(unit, stage, i_info)->offset < 0)
        {
            /** Check if special qualifier instead*/
            if (dnx_data_field_map.qual.special_metadata_qual_get(unit, stage, i_info)->nof_parts <= 0)
            {
                continue;
            }
        }
        if (i_qual >= dnx_qual_max)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Too much qualifiers for:\"%s\"\n", dnx_field_stage_text(unit, stage));
        }
        if (stage == DNX_FIELD_STAGE_IPMF2)
        {
            qual_class = DNX_FIELD_QUAL_CLASS_META2;
        }
        else
        {
            qual_class = DNX_FIELD_QUAL_CLASS_META;
        }
        dnx_qual_list[i_qual++] = DNX_QUAL(qual_class, stage, i_info);

    }
    /*
     * Add the META qualifiers of the ipmf1 to the list of the qualifiers for the ipmf2 (used to generate the initial quals)
     * */
    if (stage == DNX_FIELD_STAGE_IPMF2)
    {
        for (i_info = DNX_FIELD_QUAL_FIRST; i_info < DNX_FIELD_QUAL_NOF; i_info++)
        {

            if (dnx_data_field_map.qual.params_get(unit, DNX_FIELD_STAGE_IPMF1, i_info)->offset < 0)
            {
                /** Check if special qualifier instead*/
                if (dnx_data_field_map.qual.special_metadata_qual_get(unit, DNX_FIELD_STAGE_IPMF1, i_info)->nof_parts <=
                    0)
                {
                    continue;
                }
            }
            if (i_qual >= dnx_qual_max)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Too much qualifiers for:\"%s\"\n", dnx_field_stage_text(unit, stage));
            }
            dnx_qual_list[i_qual++] = DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, stage, i_info);

        }
    }

    /*
     * For all stages supporting header qualifiers
     */
    if (dnx_data_field.stage.stage_info_get(unit, stage)->pbus_header_length > 0)
    {
        for (i_info = DNX_FIELD_HEADER_QUAL_FIRST; i_info < DNX_FIELD_HEADER_QUAL_NOF; i_info++)
        {
            if (dnx_data_field_map.qual.header_get(unit, i_info)->dbal_field == DBAL_FIELD_EMPTY)
                continue;

            if (i_qual >= dnx_qual_max)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Too much qualifiers for:\"%s\"\n", dnx_field_stage_text(unit, stage));
            }
            dnx_qual_list[i_qual++] = DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, stage, i_info);
        }
    }

    /*
     * If LR exists for current stage - add the LR quals
     * */
    if (DNX_FIELD_IS_LAYER_RECORDS_QUALS_SUPPORTED(stage))
    {
        for (i_info = DNX_FIELD_LR_QUAL_FIRST; i_info < DNX_FIELD_LR_QUAL_NOF; i_info++)
        {
            dnx_field_map_qual_layer_record_pbus_info_t lr_qual_info_dnx_data;
            SHR_IF_ERR_EXIT(dnx_field_map_qual_layer_record_info_get(unit, stage, i_info, &lr_qual_info_dnx_data));
            if (lr_qual_info_dnx_data.valid == FALSE)
            {
                continue;
            }
            if (i_qual >= dnx_qual_max)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Too much qualifiers for:\"%s\"\n", dnx_field_stage_text(unit, stage));
            }
            dnx_qual_list[i_qual++] = DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, stage, i_info);
        }
    }

    *dnx_qual_list_p = dnx_qual_list;
    *dnx_qual_num_p = i_qual;
exit:
    if (SHR_FUNC_ERR())
    {
        SHR_FREE(dnx_qual_list);
    }
    SHR_FUNC_EXIT;
}

/**
* See map.h
*/
shr_error_e
dnx_field_map_dnx_to_dbal_qual(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_qual_t dnx_qual,
    dbal_fields_e * dbal_field_p)
{
    dnx_field_qual_info_t dnx_qual_info;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dbal_field_p, _SHR_E_PARAM, "dbal_field_p");

    SHR_IF_ERR_EXIT(dnx_field_map_qual_info(unit, stage, dnx_qual, &dnx_qual_info));

    *dbal_field_p = dnx_qual_info.field_id;
exit:
    SHR_FUNC_EXIT;
}

/**
* See map.h
*/
shr_error_e
dnx_field_map_dnx_qual_size(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_qual_t dnx_qual,
    int *nof_mappings_p,
    uint32 qual_sizes[DNX_FIELD_QAUL_MAX_NOF_MAPPINGS],
    uint32 *qual_size_total_p)
{
    dnx_field_qual_info_t dnx_qual_info;
    int qual_part_index;
    uint32 size_total = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(qual_size_total_p, _SHR_E_PARAM, "qual_size_total_p");

    SHR_IF_ERR_EXIT(dnx_field_map_qual_info(unit, stage, dnx_qual, &dnx_qual_info));

    /** Sanity check.*/
    if (dnx_qual_info.nof_mappings <= 0 || dnx_qual_info.nof_mappings > DNX_FIELD_QAUL_MAX_NOF_MAPPINGS)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Qualifier %s (0x%x) has %d mappings, must be between (1-%d).\n",
                     dnx_field_dnx_qual_text(unit, dnx_qual), dnx_qual, dnx_qual_info.nof_mappings,
                     DNX_FIELD_QAUL_MAX_NOF_MAPPINGS);
    }

    if (qual_sizes != NULL)
    {
        sal_memset(qual_sizes, 0x0, sizeof(qual_sizes[0]) * DNX_FIELD_QAUL_MAX_NOF_MAPPINGS);
    }

    for (qual_part_index = 0; qual_part_index < dnx_qual_info.nof_mappings; qual_part_index++)
    {
        if (qual_sizes != NULL)
        {
            qual_sizes[qual_part_index] = dnx_qual_info.sizes[qual_part_index];
        }
        size_total += dnx_qual_info.sizes[qual_part_index];
    }

    if (nof_mappings_p != NULL)
    {
        *nof_mappings_p = dnx_qual_info.nof_mappings;
    }
    *qual_size_total_p = size_total;

exit:
    SHR_FUNC_EXIT;
}

/**
* See map.h
*/
shr_error_e
dnx_field_map_dnx_qual_offset(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_qual_t dnx_qual,
    int *nof_mappings_p,
    int offsets[DNX_FIELD_QAUL_MAX_NOF_MAPPINGS])
{
    dnx_field_qual_info_t dnx_qual_info;
    int qual_part_index;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(nof_mappings_p, _SHR_E_PARAM, "nof_mappings_p");
    SHR_NULL_CHECK(offsets, _SHR_E_PARAM, "offsets");

    SHR_IF_ERR_EXIT(dnx_field_map_qual_info(unit, stage, dnx_qual, &dnx_qual_info));

    sal_memset(offsets, 0x0, sizeof(offsets[0]) * DNX_FIELD_QAUL_MAX_NOF_MAPPINGS);

    /** Sanity check.*/
    if (dnx_qual_info.nof_mappings <= 0 || dnx_qual_info.nof_mappings > DNX_FIELD_QAUL_MAX_NOF_MAPPINGS)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Qualifier %s (0x%x) has %d mappings, must be between (1-%d).\n",
                     dnx_field_dnx_qual_text(unit, dnx_qual), dnx_qual, dnx_qual_info.nof_mappings,
                     DNX_FIELD_QAUL_MAX_NOF_MAPPINGS);
    }

    for (qual_part_index = 0; qual_part_index < dnx_qual_info.nof_mappings; qual_part_index++)
    {
        offsets[qual_part_index] = dnx_qual_info.offsets[qual_part_index];
        /** Sanity check*/
        if ((offsets[qual_part_index] & QUAL_OFFSET_SHIFT_MASK) != offsets[qual_part_index])
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Qualifier %s (0x%x) has %d mappings, mapping %d has offset %d, bigger then mask 0x%x.\n",
                         dnx_field_dnx_qual_text(unit, dnx_qual), dnx_qual, dnx_qual_info.nof_mappings,
                         qual_part_index, offsets[qual_part_index], QUAL_OFFSET_SHIFT_MASK);
        }
    }

    *nof_mappings_p = dnx_qual_info.nof_mappings;

exit:
    SHR_FUNC_EXIT;
}

/**
* See map.h
*/
shr_error_e
dnx_field_map_dnx_lr_qual_offset_in_lr_qual(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_qual_t dnx_qual,
    int *offset_p)
{

    const dnx_field_qual_map_t *lr_qual_qual_map_p;
    dnx_field_qual_t dnx_lr_qual_qual;
    int lr_qual_nof_mappings, lr_qual_qual_nof_mappings;
    int lr_qual_offsets[DNX_FIELD_QAUL_MAX_NOF_MAPPINGS];
    int lr_qual_qual_offsets[DNX_FIELD_QAUL_MAX_NOF_MAPPINGS];

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(offset_p, _SHR_E_PARAM, "offset_p");

    if (DNX_QUAL_CLASS(dnx_qual) != DNX_FIELD_QUAL_CLASS_LAYER_RECORD)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Qualifier:\"%s\" is not a LR qualifier\n", dnx_field_dnx_qual_text(unit, dnx_qual));
    }

    if (DNX_QUAL_ID(dnx_qual) == DNX_FIELD_LR_QUAL_PROTOCOL || DNX_QUAL_ID(dnx_qual) == DNX_FIELD_LR_QUAL_OFFSET
        || DNX_QUAL_ID(dnx_qual) == DNX_FIELD_LR_QUAL_QUALIFIER)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "LR qualifier:\"%s\" is a general LR qualifier\n",
                     dnx_field_dnx_qual_text(unit, dnx_qual));
    }
    /**
     * The offset is relative to LayerRecordQualifier offset
     */
    lr_qual_qual_map_p = &dnx_global_qual_map[bcmFieldQualifyLayerRecordQualifier];
    dnx_lr_qual_qual = DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, stage, DNX_QUAL_ID(lr_qual_qual_map_p->dnx_qual));
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_offset(unit, stage, dnx_qual, &lr_qual_nof_mappings, lr_qual_offsets));
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_offset
                    (unit, stage, dnx_lr_qual_qual, &lr_qual_qual_nof_mappings, lr_qual_qual_offsets));
    /** Sanity check*/
    if (lr_qual_nof_mappings != 1 || lr_qual_qual_nof_mappings != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "LR qualifier is split. Unexpected.\n");
    }
    /** Sanity check*/
    if (lr_qual_offsets[0] < lr_qual_qual_offsets[0])
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "LR qualifier \"%s\" has offest %d, not within \"%s\" (offset %d).\n",
                     dnx_field_dnx_qual_text(unit, dnx_qual), lr_qual_offsets[0],
                     dnx_field_dnx_qual_text(unit, dnx_lr_qual_qual), lr_qual_qual_offsets[0]);
    }

    *offset_p = lr_qual_offsets[0] - lr_qual_qual_offsets[0];
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Maps BCM qual input type to
 *  DNX qual input type
 * \param [in] unit -
 *  Device Id
 * \param [in] bcm_qual_input_type -
 *  BCM qual input type
 * \param [out] dnx_qual_input_type_p -
 *  The mapped DNX qual input type
 * \return
 *   shr_error_e - Error Type
 * \see
 *   * None
 */
static shr_error_e
dnx_field_map_qual_input_type_bcm_to_dnx(
    int unit,
    bcm_field_input_types_t bcm_qual_input_type,
    dnx_field_input_type_e * dnx_qual_input_type_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_qual_input_type_p, _SHR_E_PARAM, "dnx_qual_input_type_p");

    switch (bcm_qual_input_type)
    {
        case bcmFieldInputTypeLayerFwd:
        {
            *dnx_qual_input_type_p = DNX_FIELD_INPUT_TYPE_LAYER_FWD;
            break;
        }
        case bcmFieldInputTypeLayerAbsolute:
        {
            *dnx_qual_input_type_p = DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE;
            break;
        }
        case bcmFieldInputTypeMetaData:
        {
            *dnx_qual_input_type_p = DNX_FIELD_INPUT_TYPE_META_DATA;
            break;
        }
        case bcmFieldInputTypeLayerRecordsFwd:
        {
            *dnx_qual_input_type_p = DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_FWD;
            break;
        }
        case bcmFieldInputTypeLayerRecordsAbsolute:
        {
            *dnx_qual_input_type_p = DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_ABSOLUTE;
            break;
        }
        case bcmFieldInputTypeExternal:
        {
            *dnx_qual_input_type_p = DNX_FIELD_INPUT_TYPE_EXTERNAL;
            break;
        }
        case bcmFieldInputTypeCascaded:
        {
            *dnx_qual_input_type_p = DNX_FIELD_INPUT_TYPE_CASCADED;
            break;
        }
        case bcmFieldInputTypeConst:
        {
            *dnx_qual_input_type_p = DNX_FIELD_INPUT_TYPE_CONST;
            break;
        }
        default:
        {
            /*
             * None of the supported types
             */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid BCM Qualifier Input Type:%d \n", bcm_qual_input_type);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_qual_input_type_dnx_to_bcm(
    int unit,
    dnx_field_input_type_e dnx_qual_input_type,
    bcm_field_input_types_t * bcm_qual_input_type_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_qual_input_type_p, _SHR_E_PARAM, "bcm_qual_input_type_p");

    switch (dnx_qual_input_type)
    {
        case DNX_FIELD_INPUT_TYPE_LAYER_FWD:
        {
            *bcm_qual_input_type_p = bcmFieldInputTypeLayerFwd;
            break;
        }
        case DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE:
        {
            *bcm_qual_input_type_p = bcmFieldInputTypeLayerAbsolute;
            break;
        }
        case DNX_FIELD_INPUT_TYPE_META_DATA:
        {
            *bcm_qual_input_type_p = bcmFieldInputTypeMetaData;
            break;
        }
        case DNX_FIELD_INPUT_TYPE_META_DATA2:
        {
            /*
             * DNX_FIELD_INPUT_TYPE_META_DATA2 is translated to bcmFieldInputTypeMetaData, but it's offset is expected
             * to be encoded with QUAL_OFFSET_META2_BIT bit if it is a user defined qualifier.
             */
            *bcm_qual_input_type_p = bcmFieldInputTypeMetaData;
            break;
        }
        case DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_FWD:
        {
            *bcm_qual_input_type_p = bcmFieldInputTypeLayerRecordsFwd;
            break;
        }
        case DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_ABSOLUTE:
        {
            *bcm_qual_input_type_p = bcmFieldInputTypeLayerRecordsAbsolute;
            break;
        }
        case DNX_FIELD_INPUT_TYPE_EXTERNAL:
        {
            *bcm_qual_input_type_p = bcmFieldInputTypeExternal;
            break;
        }
        case DNX_FIELD_INPUT_TYPE_CASCADED:
        {
            *bcm_qual_input_type_p = bcmFieldInputTypeCascaded;
            break;
        }
        case DNX_FIELD_INPUT_TYPE_CONST:
        {
            *bcm_qual_input_type_p = bcmFieldInputTypeConst;
            break;
        }
        default:
        {
            /*
             * None of the supported types
             */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid DNX Qualifier Input Type:%d \n", dnx_qual_input_type);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_qual_with_attach_info_bcm_to_dnx(
    int unit,
    dnx_field_stage_e dnx_stage,
    bcm_field_qualify_t bcm_qual,
    bcm_field_qualify_attach_info_t * bcm_attach_info_p,
    dnx_field_qual_t * dnx_qual_p,
    dnx_field_qual_attach_info_t * dnx_attach_info_p)
{
    uint32 offset;
    bcm_field_qualify_t bcm_base_qual;
    dnx_field_qual_t dnx_base_qual;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(bcm_attach_info_p, _SHR_E_PARAM, "bcm_attach_info_p");
    SHR_NULL_CHECK(dnx_qual_p, _SHR_E_PARAM, "dnx_qual_p");
    SHR_NULL_CHECK(dnx_attach_info_p, _SHR_E_PARAM, "dnx_attach_info_p");

    SHR_IF_ERR_EXIT(dnx_field_qual_attach_info_t_init(unit, dnx_attach_info_p));

    SHR_IF_ERR_EXIT(dnx_field_map_qual_bcm_to_dnx(unit, dnx_stage, bcm_qual, dnx_qual_p));

    SHR_IF_ERR_EXIT(dnx_field_map_qual_input_type_bcm_to_dnx(unit,
                                                             bcm_attach_info_p->input_type,
                                                             &(dnx_attach_info_p->input_type)));

    offset = bcm_attach_info_p->offset;
    dnx_attach_info_p->base_qual = DNX_FIELD_QUAL_TYPE_INVALID;

    /** If we have input type METADATA, use the offset encoding to determine if it is METADATA or METADADA 2.*/
    if (bcm_attach_info_p->input_type == bcmFieldInputTypeMetaData)
    {
        offset = ((uint32) bcm_attach_info_p->offset) & QUAL_OFFSET_SHIFT_MASK;
        /** Handle non native Meta Data qualifier for user defined qualifier.*/
        if (bcm_attach_info_p->offset & SAL_BIT(QUAL_OFFSET_META2_BIT))

        {
            if (dnx_stage == DNX_FIELD_STAGE_IPMF2)
            {
                if (dnx_attach_info_p->input_type == DNX_FIELD_INPUT_TYPE_META_DATA)
                {
                    if ((DNX_QUAL_CLASS(*dnx_qual_p)) == DNX_FIELD_QUAL_CLASS_USER)
                    {
                        dnx_attach_info_p->input_type = DNX_FIELD_INPUT_TYPE_META_DATA2;
                    }
                    else
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "Offset of qual %d (\"%s\") is encoded as metadata 2 in stage \"%s\", but the "
                                     "qualifier is not user defined.\n",
                                     bcm_qual, dnx_field_bcm_qual_text(unit, bcm_qual),
                                     dnx_field_stage_text(unit, dnx_stage));
                    }
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Offset of qual %d (\"%s\") is encoded as metadata 2 in stage \"%s\", but the "
                                 "input type is %d, and not metadata (%d).\n",
                                 bcm_qual, dnx_field_bcm_qual_text(unit, bcm_qual),
                                 dnx_field_stage_text(unit, dnx_stage),
                                 dnx_attach_info_p->input_type, DNX_FIELD_INPUT_TYPE_META_DATA);
                }
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Offset of qual %d (\"%s\") is encoded as metadata 2, but the stage is \"%s\", "
                             "and only iPMF2 supports metadata 2.\n",
                             bcm_qual, dnx_field_bcm_qual_text(unit, bcm_qual), dnx_field_stage_text(unit, dnx_stage));
            }
        }
        /** Handle base qualifier.*/
        if (bcm_attach_info_p->offset & SAL_BIT(QUAL_OFFSET_BASE_QUAL_EXITS_BIT))
        {
            if ((DNX_QUAL_CLASS(*dnx_qual_p)) != DNX_FIELD_QUAL_CLASS_USER)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Offset of qual %d (\"%s\") is encoded as having base qualifier, "
                             "but is is not a user defined qualifier.\n",
                             bcm_qual, dnx_field_bcm_qual_text(unit, bcm_qual));
            }
            bcm_base_qual =
                (((uint32) bcm_attach_info_p->offset) >> QUAL_OFFSET_BASE_QUAL_LSB) & QUAL_OFFSET_BASE_QUAL_MASK;
            SHR_IF_ERR_EXIT_WITH_LOG(dnx_field_map_qual_bcm_to_dnx
                                     (unit, dnx_stage, bcm_base_qual, &dnx_base_qual),
                                     "Error for base BCM Qualifier %d (\"%s\"). Offset is 0x%x.\n",
                                     bcm_base_qual, dnx_field_bcm_qual_text(unit, bcm_qual), bcm_attach_info_p->offset);
            if ((DNX_QUAL_CLASS(dnx_base_qual)) != DNX_FIELD_QUAL_CLASS_META &&
                (dnx_attach_info_p->input_type == DNX_FIELD_INPUT_TYPE_META_DATA))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Offset of qual %d (\"%s\") is encoded as having base qualifier %d (\"%s\"), "
                             "but is is not encoded as non native metadata.\n",
                             bcm_qual, dnx_field_bcm_qual_text(unit, bcm_qual),
                             bcm_base_qual, dnx_field_bcm_qual_text(unit, bcm_base_qual));
            }
            if ((DNX_QUAL_CLASS(dnx_base_qual)) != DNX_FIELD_QUAL_CLASS_META2 &&
                (dnx_attach_info_p->input_type == DNX_FIELD_INPUT_TYPE_META_DATA2))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Offset of qual %d (\"%s\") is encoded as having base qualifier %d (\"%s\"), "
                             "but is is not encoded as native metadata.\n",
                             bcm_qual, dnx_field_bcm_qual_text(unit, bcm_qual),
                             bcm_base_qual, dnx_field_bcm_qual_text(unit, bcm_base_qual));
            }
            dnx_attach_info_p->base_qual = dnx_base_qual;
        }
    }
    /** Handle non native Meta Data qualifier for predefined qualifier.*/
    if ((DNX_QUAL_CLASS(*dnx_qual_p)) == DNX_FIELD_QUAL_CLASS_META2)
    {
        if (dnx_attach_info_p->input_type == DNX_FIELD_INPUT_TYPE_META_DATA)
        {
            dnx_attach_info_p->input_type = DNX_FIELD_INPUT_TYPE_META_DATA2;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Qual %d (\"%s\") is a predefined native metadata qualifier in stage \"%s\", but the "
                         "input type is %d, and not metadata (%d).\n",
                         bcm_qual, dnx_field_bcm_qual_text(unit, bcm_qual), dnx_field_stage_text(unit, dnx_stage),
                         dnx_attach_info_p->input_type, DNX_FIELD_INPUT_TYPE_META_DATA);
        }
    }
    dnx_attach_info_p->input_arg = bcm_attach_info_p->input_arg;
    dnx_attach_info_p->offset = offset;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_qual_with_attach_info_dnx_to_bcm(
    int unit,
    dnx_field_stage_e dnx_stage,
    dnx_field_qual_t dnx_qual,
    dnx_field_qual_attach_info_t * dnx_attach_info_p,
    bcm_field_qualify_t * bcm_qual_p,
    bcm_field_qualify_attach_info_t * bcm_attach_info_p)
{
    uint32 offset;
    dnx_field_qual_class_e dnx_qual_class;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dnx_attach_info_p, _SHR_E_PARAM, "dnx_attach_info_p");
    SHR_NULL_CHECK(bcm_qual_p, _SHR_E_PARAM, "bcm_qual_p");
    SHR_NULL_CHECK(bcm_attach_info_p, _SHR_E_PARAM, "bcm_attach_info_p");

    bcm_field_qual_attach_info_t_init(bcm_attach_info_p);

    SHR_IF_ERR_EXIT(dnx_field_map_qual_dnx_to_bcm(unit, dnx_stage, dnx_qual, bcm_qual_p));

    SHR_IF_ERR_EXIT(dnx_field_map_qual_input_type_dnx_to_bcm(unit,
                                                             dnx_attach_info_p->input_type,
                                                             &(bcm_attach_info_p->input_type)));

    dnx_qual_class = DNX_QUAL_CLASS(dnx_qual);

    offset = dnx_attach_info_p->offset;

    /*
     * Handle encoding base qualifer.
     */
    if (dnx_qual_class != DNX_FIELD_QUAL_CLASS_USER)
    {
        if (dnx_attach_info_p->base_qual != DNX_FIELD_QUAL_TYPE_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Qualifier 0x%x (\"%s\") is not user defined but has base qualifier 0x%x.\n",
                         dnx_qual, dnx_field_dnx_qual_text(unit, dnx_qual), dnx_attach_info_p->base_qual);
        }
    }
    else if (dnx_attach_info_p->base_qual != DNX_FIELD_QUAL_TYPE_INVALID)
    {
        /*
         * Add the base qualifer.
         */
        bcm_field_qualify_t bcm_base_qual;
        dnx_field_qual_class_e dnx_base_qual_class = DNX_QUAL_CLASS(dnx_attach_info_p->base_qual);
        if ((dnx_base_qual_class != DNX_FIELD_QUAL_CLASS_META) && (dnx_base_qual_class != DNX_FIELD_QUAL_CLASS_META2))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "base qualifer 0x%x (\"%s\") is not a metadata qualifier.\n",
                         dnx_attach_info_p->base_qual, dnx_field_dnx_qual_text(unit, dnx_attach_info_p->base_qual));
        }
        if ((offset & QUAL_OFFSET_SHIFT_MASK) != offset)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "offset 0x%x does not fit in 0x%x.\n", offset, QUAL_OFFSET_SHIFT_MASK);
        }
        SHR_IF_ERR_EXIT(dnx_field_map_qual_dnx_to_bcm(unit, dnx_stage, dnx_attach_info_p->base_qual, &bcm_base_qual));
        offset = (offset & QUAL_OFFSET_SHIFT_MASK) |
            (bcm_base_qual << QUAL_OFFSET_BASE_QUAL_LSB) | (SAL_BIT(QUAL_OFFSET_BASE_QUAL_EXITS_BIT));
    }

    /*
     * Encode the offset for Metadata2
     */
    if (((DNX_QUAL_CLASS(dnx_qual)) == DNX_FIELD_QUAL_CLASS_USER) &&
        (dnx_attach_info_p->input_type == DNX_FIELD_INPUT_TYPE_META_DATA2))
    {
        /** Sanity check.*/
        if ((offset & SAL_BIT(QUAL_OFFSET_META2_BIT)) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "offset 0x%x already contains bit %d.\n", offset, QUAL_OFFSET_META2_BIT);
        }
        offset |= SAL_BIT(QUAL_OFFSET_META2_BIT);
    }

    bcm_attach_info_p->input_arg = dnx_attach_info_p->input_arg;
    bcm_attach_info_p->offset = offset;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See field_map.h
 */
shr_error_e
dnx_field_map_name_to_qual(
    int unit,
    const char name[DBAL_MAX_STRING_LENGTH],
    bcm_field_qualify_t * bcm_qual_p)
{
    bcm_field_qualify_t bcm_qual_ndx;
    int found = FALSE;
    int user_defined_first = dnx_data_field.qual.user_1st_get(unit);
    int user_defined_last = dnx_data_field.qual.user_1st_get(unit) + dnx_data_field.qual.user_nof_get(unit) - 1;
    int vw_first = dnx_data_field.qual.vw_1st_get(unit);
    int vw_last = dnx_data_field.qual.vw_1st_get(unit) + dnx_data_field.qual.vw_nof_get(unit) - 1;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(name, _SHR_E_PARAM, "name");
    SHR_NULL_CHECK(bcm_qual_p, _SHR_E_PARAM, "bcm_qual_p");

    if (name[0] == '\0')
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Name is an empty string.\n");
    }
    if (0 == sal_strncmp(name, "Invalid BCM qualifier", DBAL_MAX_STRING_LENGTH))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Name is \"%.*s\". \"%s\" is an illegal string.\n",
                     DBAL_MAX_STRING_LENGTH, name, "Invalid BCM qualifier");
    }

    /*
     * Go over virtual wires qualifiers.
     */
    for (bcm_qual_ndx = vw_first; bcm_qual_ndx <= vw_last; bcm_qual_ndx++)
    {
        if (0 == sal_strncmp(name, dnx_field_bcm_qual_text(unit, bcm_qual_ndx), DBAL_MAX_STRING_LENGTH))
        {
            found = TRUE;
            break;
        }
    }
    /*
     * Go over user defined qualifiers.
     */
    if (found == FALSE)
    {
        for (bcm_qual_ndx = user_defined_first; bcm_qual_ndx <= user_defined_last; bcm_qual_ndx++)
        {
            if (0 == sal_strncmp(name, dnx_field_bcm_qual_text(unit, bcm_qual_ndx), DBAL_MAX_STRING_LENGTH))
            {
                found = TRUE;
                break;
            }
        }
    }

    /*
     * Go over predefined qualifiers.
     */
    if (found == FALSE)
    {
        for (bcm_qual_ndx = 0; bcm_qual_ndx < bcmFieldQualifyCount; bcm_qual_ndx++)
        {
            if (0 == sal_strncmp(name, dnx_field_bcm_qual_text(unit, bcm_qual_ndx), DBAL_MAX_STRING_LENGTH))
            {
                found = TRUE;
                break;
            }
        }
    }

    if (found)
    {
        (*bcm_qual_p) = bcm_qual_ndx;
    }
    else
    {
        (*bcm_qual_p) = bcmFieldQualifyCount;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief  Validates that every qualifier with a conversion function has a raw qualifier
 * \param [in] unit      - Identifier of HW platform.
 * \param [in] dnx_class - Dnx qualifier class.
 * \param [in] dnx_stage - Dnx stage.
 * \return
 *   \retval _SHR_E_NONE      - On success
 *   \retval _SHR_E_NOT_FOUND - When there is a bcm qualifier with conversion function BUT without the raw qualifier
 * \remark
 *
 */
static shr_error_e
dnx_field_map_qual_has_raw_validate(
    int unit,
    dnx_field_qual_class_e dnx_class,
    dnx_field_stage_e dnx_stage)
{
    bcm_field_qualify_t i_bcm_qual;
    dnx_field_qual_t dnx_qual;
    int i_dnx_qual;
    const dnx_field_qual_map_t *qual_map_entry_p;
    const dnx_field_qual_map_t *raw_qual_map_entry_p;
    SHR_FUNC_INIT_VARS(unit);

    for (i_bcm_qual = 0; i_bcm_qual < bcmFieldQualifyCount; i_bcm_qual++)
    {
        if (dnx_class == DNX_FIELD_QUAL_CLASS_LAYER_RECORD || dnx_class == DNX_FIELD_QUAL_CLASS_HEADER)
        {
            qual_map_entry_p = &dnx_global_qual_map[i_bcm_qual];
        }
        else
        {
            qual_map_entry_p = &dnx_field_map_stage_info[dnx_stage].meta_qual_map[i_bcm_qual];

        }
        dnx_qual = qual_map_entry_p->dnx_qual;

        if ((DNX_QUAL_CLASS(dnx_qual) == DNX_FIELD_QUAL_CLASS_LAYER_RECORD
             && DNX_QUAL_ID(dnx_qual) != DNX_FIELD_LR_QUAL_PROTOCOL)
            || (DNX_QUAL_ID(dnx_qual) == DNX_FIELD_QUAL_FTMH_TM_ACTION_TYPE))
        {
            continue;
        }
        if (dnx_qual != DNX_FIELD_QUAL_ID_INVALID)
        {
            if (qual_map_entry_p->conversion_cb != NULL)
            {
                /*
                 * run over all bcm_quals that have the same dnx qual
                 */
                for (i_dnx_qual = 0; i_dnx_qual < bcmFieldQualifyCount; i_dnx_qual++)
                {
                    if (dnx_class == DNX_FIELD_QUAL_CLASS_LAYER_RECORD || dnx_class == DNX_FIELD_QUAL_CLASS_HEADER)
                    {
                        raw_qual_map_entry_p = &dnx_global_qual_map[i_dnx_qual];
                    }
                    else
                    {
                        raw_qual_map_entry_p = &dnx_field_map_stage_info[dnx_stage].meta_qual_map[i_dnx_qual];
                    }
                    /*
                     * get it's dnx qual'
                     */
                    if (dnx_qual == raw_qual_map_entry_p->dnx_qual)
                    {
                        /*
                         * if raw_qual_map_entry.dnx_qual has no convert_func - we have found the raw
                         * qualifier
                         */
                        if ((raw_qual_map_entry_p->conversion_cb == NULL)
                            && BCM_TO_DNX_IS_BAISC_OBJ(raw_qual_map_entry_p->flags))
                        {
                            LOG_INFO(BSL_LOG_MODULE, (BSL_META("FOUND Raw qualifier for :\"%s\" BCM Qualifier \n"),
                                                      dnx_field_bcm_qual_text(unit, i_bcm_qual)));
                            break;
                        }
                    }
                }
                if (i_dnx_qual == bcmFieldQualifyCount)
                {
                    /*
                     * If we got here - there is no raw qualifier for the given bcm qualifier
                     */
                    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "BCM Qualifier:\"%s\" mapped to:\"%s\" has no valid raw qualifier\n",
                                 dnx_field_bcm_qual_text(unit, i_bcm_qual), dnx_field_dnx_qual_text(unit, dnx_qual));
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief  Validates that every qualifier with a conversion function in the system has a raw qualifier
 * \param [in] unit - Identifier of HW platform.
 * \return
 *   \retval _SHR_E_NONE      - On success
 *   \retval _SHR_E_NOT_FOUND - When there is a bcm qualifier with conversion function BUT without the raw qualifier
 * \remark
 *
 */
static shr_error_e
dnx_field_map_qual_raw_qualifier_map_validate(
    int unit)
{
    dnx_field_stage_e dnx_stage;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_map_qual_has_raw_validate(unit, DNX_FIELD_QUAL_CLASS_HEADER, DNX_FIELD_STAGE_FIRST));
    SHR_IF_ERR_EXIT(dnx_field_map_qual_has_raw_validate
                    (unit, DNX_FIELD_QUAL_CLASS_LAYER_RECORD, DNX_FIELD_STAGE_FIRST));

    DNX_FIELD_STAGE_QUAL_ITERATOR(dnx_stage)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_qual_has_raw_validate(unit, DNX_FIELD_QUAL_CLASS_META, dnx_stage));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_qual_init(
    int unit)
{
    int i_id;
    bcm_field_qualify_t i_bcm_qual;
    dnx_field_stage_e dnx_stage;
    const dnx_field_qual_map_t *qual_map_info_p;
    dbal_fields_e field_id = DBAL_FIELD_EMPTY;
    dnx_field_metadata_qual_e meta_qual = DNX_FIELD_QUAL_INVALID;
    char field_name[DBAL_MAX_STRING_LENGTH];
    int size;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialize Header Qualifiers
     */
    for (i_id = DNX_FIELD_HEADER_QUAL_FIRST; i_id < DNX_FIELD_HEADER_QUAL_NOF; i_id++)
    {
        DNX_FIELD_QUAL_SIZE_VERIFY(DNX_FIELD_QUAL_CLASS_HEADER, i_id,
                                   dnx_data_field_map.qual.header_get(unit, i_id)->size);
    }
    /*
     * Initialize Layer Record Qualifiers
     */

    if (DNX_DATA_MAX_FIELD_QUAL_LAYER_RECORD_NOF < DNX_FIELD_LR_QUAL_NOF)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Size of Sw state for Layer Records qualifiers is smaller (%d) that the amoune of qualifiers (%d)\n",
                     DNX_DATA_MAX_FIELD_QUAL_LAYER_RECORD_NOF, DNX_FIELD_LR_QUAL_NOF);
    }

    for (i_id = DNX_FIELD_LR_QUAL_FIRST; i_id < DNX_FIELD_LR_QUAL_NOF; i_id++)
    {
        /*
         * Verify that the sizes of the layer records in ingess and egress are within range.
         */
        if (dnx_data_field_map.qual.layer_record_info_ingress_get(unit, i_id)->valid)
        {
            DNX_FIELD_QUAL_SIZE_VERIFY(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, i_id,
                                       dnx_data_field_map.qual.layer_record_info_ingress_get(unit, i_id)->size);
        }
        if (dnx_data_field_map.qual.layer_record_info_egress_get(unit, i_id)->valid)
        {
            DNX_FIELD_QUAL_SIZE_VERIFY(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, i_id,
                                       dnx_data_field_map.qual.layer_record_info_egress_get(unit, i_id)->size);
        }
        if (dnx_lr_qual_name[i_id].name == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "DNX LR Qualifier:%d has no valid name\n", i_id);
        }
        SHR_IF_ERR_EXIT(dnx_field_qual_sw_db.layer_record.field_id.get(unit, i_id, &field_id));
        if (field_id == DBAL_FIELD_EMPTY)
        {
            sal_strncpy_s(field_name, dnx_lr_qual_name[i_id].name, DBAL_MAX_STRING_LENGTH);
            SHR_IF_ERR_EXIT(dbal_fields_field_create(unit, DBAL_FIELD_TYPE_DEF_UINT, field_name, &field_id));
            SHR_IF_ERR_EXIT(dnx_field_qual_sw_db.layer_record.field_id.set(unit, i_id, field_id));
        }
    }

    /*
     * Before start, initialize map
     */
    if (DNX_DATA_MAX_FIELD_QUAL_PREDEFINED_NOF < DNX_FIELD_QUAL_NOF)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Size of Sw state for predefined qualifier is smaller (%d) that the amoune of qualifiers (%d)\n",
                     DNX_DATA_MAX_FIELD_QUAL_PREDEFINED_NOF, DNX_FIELD_QUAL_NOF);
    }
    for (i_id = DNX_FIELD_QUAL_FIRST; i_id < DNX_FIELD_QUAL_NOF; i_id++)
    {
        if (dnx_qual_name[i_id].name == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "DNX Qualifier:%d has no valid name\n", i_id);
        }
        SHR_IF_ERR_EXIT(dnx_field_qual_sw_db.predefined.field_id.get(unit, i_id, &field_id));
        if (field_id == DBAL_FIELD_EMPTY)
        {
            sal_strncpy_s(field_name, dnx_qual_name[i_id].name, DBAL_MAX_STRING_LENGTH);
            SHR_IF_ERR_EXIT(dbal_fields_field_create(unit, DBAL_FIELD_TYPE_DEF_UINT, field_name, &field_id));
            SHR_IF_ERR_EXIT(dnx_field_qual_sw_db.predefined.field_id.set(unit, i_id, field_id));
        }
    }

    for (i_bcm_qual = 0; i_bcm_qual < bcmFieldQualifyCount; i_bcm_qual++)
    {
        dnx_field_qual_t dnx_qual;
        qual_map_info_p = &dnx_global_qual_map[i_bcm_qual];
        if ((dnx_qual = qual_map_info_p->dnx_qual) != DNX_FIELD_QUAL_ID_INVALID)
        {
            if (ISEMPTY(bcm_qual_description[i_bcm_qual]))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "BCM Qualifier:\"%s\" mapped to:\"%s\" has no valid description\n",
                             dnx_field_bcm_qual_text(unit, i_bcm_qual), dnx_field_dnx_qual_text(unit, dnx_qual));
            }
        }
        else
        {
            for (dnx_stage = DNX_FIELD_STAGE_FIRST; dnx_stage < DNX_FIELD_STAGE_NOF; dnx_stage++)
            {
                if (dnx_field_map_stage_info[dnx_stage].meta_qual_map != NULL)
                {
                    meta_qual = dnx_field_map_stage_info[dnx_stage].meta_qual_map[i_bcm_qual].dnx_qual;
                    /*
                     * Bcm qualifier has mapping to Metadata
                     */
                    if (meta_qual != DNX_FIELD_QUAL_INVALID)
                    {
                        if (dnx_data_field_map.qual.params_get(unit, dnx_stage, meta_qual)->offset > -1)
                        {
                            size = dnx_data_field_map.qual.params_get(unit, dnx_stage, meta_qual)->size;
                            if (size <= 0)
                            {
                                /*
                                 * In LIF/RIF profile qualifiers are based on Soc Property and can have size 0
                                 */
                                if (meta_qual != DNX_FIELD_QUAL_IN_LIF_PROFILE_0 &&
                                    meta_qual != DNX_FIELD_QUAL_IN_LIF_PROFILE_1 &&
                                    meta_qual != DNX_FIELD_QUAL_PMF_ETH_RIF_PROFILE && size != 0)
                                {
                                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "BCM Qualifier %d (\"%s\") has invalid size %d.\n",
                                                 i_bcm_qual, dnx_field_bcm_qual_text(unit, i_bcm_qual), size);
                                }
                            }
                            if (ISEMPTY(bcm_qual_description[i_bcm_qual]))
                            {
                                SHR_ERR_EXIT(_SHR_E_INTERNAL, "BCM Qualifier:\"%s\" has no valid description\n",
                                             dnx_field_bcm_qual_text(unit, i_bcm_qual));
                            }
                        }
                    }

                }
            }
        }
    }

    if (dnx_data_field.qual.user_1st_get(unit) != BCM_FIELD_FIRST_USER_QUALIFIER_ID)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "In DNX data the first BCM user defined qualifier is %d, "
                     "while BCM_FIELD_FIRST_USER_QUALIFIER_ID is %d.\n",
                     dnx_data_field.qual.user_1st_get(unit), BCM_FIELD_FIRST_USER_QUALIFIER_ID);
    }

    if (dnx_data_field.qual.user_1st_get(unit) < bcmFieldQualifyCount)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "First BCM User Qualifier ID is %d, should be least bcmFieldQualifyCount:%d\n",
                     dnx_data_field.qual.user_1st_get(unit), bcmFieldQualifyCount);
    }

    if ((dnx_data_field.qual.user_1st_get(unit) + dnx_data_field.qual.user_nof_get(unit)) >
        dnx_data_field.qual.vw_1st_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "First VW Qualifier ID is %d, and last User defined qualifier ID is %d.\n",
                     dnx_data_field.qual.vw_1st_get(unit),
                     dnx_data_field.qual.user_1st_get(unit) + dnx_data_field.qual.user_nof_get(unit) - 1);
    }

    /*
     * Validate that every qualifier with a conversion function in the system has a raw qualifier
     * */
    SHR_IF_ERR_EXIT(dnx_field_map_qual_raw_qualifier_map_validate(unit));

    /*
     * Validate that that all conversion CBs, have BareMetal support macro check
     * */
    for (i_bcm_qual = 0; i_bcm_qual < bcmFieldQualifyCount; i_bcm_qual++)
    {
        int bare_metal_support;
        DNX_FIELD_STAGE_QUAL_ITERATOR(dnx_stage)
        {
            qual_map_info_p = &dnx_field_map_stage_info[dnx_stage].meta_qual_map[i_bcm_qual];
            if (qual_map_info_p->conversion_cb != NULL)
            {
                SHR_IF_ERR_EXIT(dnx_field_bare_metal_support_check
                                (unit, qual_map_info_p->conversion_cb, &bare_metal_support));
            }
        }
    }

    /*
     * Validate that predefined qualifiers fit into offset encoding.
     */
    if ((bcmFieldQualifyCount & QUAL_OFFSET_BASE_QUAL_MASK) != bcmFieldQualifyCount)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Number of predefined qualifers %d doesn't fit into mask 0x%x.\n",
                     bcmFieldQualifyCount, QUAL_OFFSET_BASE_QUAL_MASK);
    }
    /*
     * Validate that there is no overlap in offset encoding.
     */
    if ((QUAL_OFFSET_SHIFT_MASK & (QUAL_OFFSET_BASE_QUAL_MASK << QUAL_OFFSET_BASE_QUAL_LSB) &
         (1U << QUAL_OFFSET_BASE_QUAL_EXITS_BIT) & (1U << QUAL_OFFSET_META2_BIT)) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "There is an overlap in offset encoding.\n");
    }

exit:
    SHR_FUNC_EXIT;
}
