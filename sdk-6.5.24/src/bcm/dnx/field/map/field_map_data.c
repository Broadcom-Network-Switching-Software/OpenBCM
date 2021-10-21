/** \file field_map_data.c
 * 
 *
 * Data for bcm to dnx and dnx to HW mapping
 *
 * This file implements mapping from BCM to DNX qualifiers and access for per DNX qualifier/action information
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX
/*
 * Include files.
 * {
 */

#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_rhlist.h>
#include <bcm_int/dnx/field/field_map.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include <soc/dnx/dbal/dbal_string_mgmt.h>

#include "field_map_local.h"
/*
 * }Include files
 */

/* *INDENT-OFF* */
static const dnx_field_context_param_t context_param_set[bcmFieldContextParamCount][bcmFieldStageCount] = {
    [bcmFieldContextParamTypeSystemHeaderProfile] = {
        [bcmFieldStageIngressPMF1] = {  DBAL_TABLE_FIELD_IPMF1_CONTEXT_GENERAL,
                                        {DBAL_FIELD_FIELD_PMF_CTX_ID},
                                        {DBAL_FIELD_SYSTEM_HEADER_PROFILE},
                                        dnx_field_convert_context_sys_header_profile},
    },
    [bcmFieldContextParamTypeKeyVal] = {
        [bcmFieldStageIngressPMF1] = {  DBAL_TABLE_FIELD_PMF_CONTEXT_GENERAL,
                                        {DBAL_FIELD_FIELD_PMF_CTX_ID, DBAL_FIELD_FIELD_PMF_STAGE},
                                        {DBAL_FIELD_PROGRAM_KEY_GEN_VAR},
                                        dnx_field_convert_context_param_key_val},
        [bcmFieldStageIngressPMF3] = {  DBAL_TABLE_FIELD_PMF_CONTEXT_GENERAL,
                                        {DBAL_FIELD_FIELD_PMF_CTX_ID, DBAL_FIELD_FIELD_PMF_STAGE},
                                        {DBAL_FIELD_PROGRAM_KEY_GEN_VAR},
                                        dnx_field_convert_context_param_key_val},
    },
    [bcmFieldContextParamTypeSystemHeaderStrip] = {
        [bcmFieldStageIngressPMF2] = {  DBAL_TABLE_FIELD_IPMF2_CONTEXT_GENERAL,
                                        {DBAL_FIELD_FIELD_PMF_CTX_ID},
                                        {DBAL_FIELD_NUM_LAYERS_TO_REMOVE, DBAL_FIELD_NUM_BYTES_TO_REMOVE},
                                        dnx_field_convert_context_param_header_strip}
    },
    [bcmFieldContextParamTypeFlushLearnEnable] = {
        [bcmFieldStageIngressPMF2] = {  DBAL_TABLE_FIELD_PMF_ELEPHANT_LEARN_CFG,
                                        {DBAL_FIELD_FIELD_PMF_CTX_ID},
                                        {DBAL_FIELD_ELEPHANT_LEARN_ENABLE},
                                        dnx_field_convert_context_param_key_val}
    }
};

static const dnx_field_range_map_t range_map_legacy[DNX_FIELD_RANGE_TYPE_NOF][DNX_FIELD_STAGE_NOF] = {
    [DNX_FIELD_RANGE_TYPE_L4_SRC_PORT] = {
        [DNX_FIELD_STAGE_IPMF1] =    {"L4SRC",
                                       DBAL_TABLE_FIELD_IPMF1_L4_OPS_RANGES,
                                       DBAL_FIELD_FIELD_PMF_RANGE_ID,
                                       DBAL_FIELD_MIN_SRC_L4_PORT ,DBAL_FIELD_MAX_SRC_L4_PORT,
                                       FALSE,
                                     },
        [DNX_FIELD_STAGE_EPMF] =     {"L4SRC",
                                       DBAL_TABLE_FIELD_EPMF_L4_OPS_RANGES,
                                       DBAL_FIELD_FIELD_PMF_RANGE_ID,
                                       DBAL_FIELD_MIN_SRC_L4_PORT ,DBAL_FIELD_MAX_SRC_L4_PORT,
                                       FALSE,
                                      },
    },
    [DNX_FIELD_RANGE_TYPE_L4_DST_PORT] = {
        [DNX_FIELD_STAGE_IPMF1] =    {"L4DST",
                                       DBAL_TABLE_FIELD_IPMF1_L4_OPS_RANGES,
                                       DBAL_FIELD_FIELD_PMF_RANGE_ID,
                                       DBAL_FIELD_MIN_DEST_L4_PORT ,DBAL_FIELD_MAX_DEST_L4_PORT,
                                       FALSE,
                                     },
        [DNX_FIELD_STAGE_EPMF] =     {"L4DST",
                                       DBAL_TABLE_FIELD_EPMF_L4_OPS_RANGES,
                                       DBAL_FIELD_FIELD_PMF_RANGE_ID,
                                       DBAL_FIELD_MIN_DEST_L4_PORT ,DBAL_FIELD_MAX_DEST_L4_PORT,
                                       FALSE,
                                      },
    },
    [DNX_FIELD_RANGE_TYPE_OUT_LIF] = {
        [DNX_FIELD_STAGE_IPMF1] =    {"OutLif",
                                       DBAL_TABLE_FIELD_IPMF1_OUT_LIF_RANGES,
                                       DBAL_FIELD_FIELD_PMF_RANGE_ID,
                                       DBAL_FIELD_OUT_LIF_RANGE_MIN_N, DBAL_FIELD_OUT_LIF_RANGE_MAX_N,
                                       TRUE,
                                     },
        [DNX_FIELD_STAGE_IPMF3] =    {"OutLif",
                                      DBAL_TABLE_FIELD_IPMF3_OUT_LIF_RANGES,
                                      DBAL_FIELD_FIELD_PMF_RANGE_ID,
                                      DBAL_FIELD_OUT_LIF_RANGE_MIN_N, DBAL_FIELD_OUT_LIF_RANGE_MAX_N,
                                      TRUE,
                                      }
    },
    [DNX_FIELD_RANGE_TYPE_PKT_HDR_SIZE] = {
        [DNX_FIELD_STAGE_IPMF1] =    {"PktHeaderSize",
                                       DBAL_TABLE_FIELD_IPMF1_PKT_HDR_SIZE_RANGE,
                                       DBAL_FIELD_FIELD_PMF_RANGE_ID,
                                       DBAL_FIELD_PKT_HDR_SIZE_RANGE_MIN, DBAL_FIELD_PKT_HDR_SIZE_RANGE_MAX,
                                       TRUE,
                                      },
    },
};

/*
 * Context Selection related data below
 * {
 */
/*
 * Inside maps all CS qualifiers are listed, ones that has no bcm qualifier located yet are in comments *
 */
static const dbal_fields_e dnx_ipmf1_cs_qual_layer_type[] = {
    /** We allow The FW layer to be -7 to 7.*/
    DBAL_FIELD_FWD_LAYER_TYPE_1,
    DBAL_FIELD_FWD_LAYER_TYPE_2,
    DBAL_FIELD_FWD_LAYER_TYPE_3,
    DBAL_FIELD_FWD_LAYER_TYPE_4,
    DBAL_FIELD_FWD_LAYER_TYPE_5,
    DBAL_FIELD_FWD_LAYER_TYPE_6,
    DBAL_FIELD_FWD_LAYER_TYPE_MINUS_1,
    DBAL_FIELD_FWD_LAYER_TYPE_0,
    DBAL_FIELD_FWD_LAYER_TYPE_1,
    DBAL_FIELD_FWD_LAYER_TYPE_2,
    DBAL_FIELD_FWD_LAYER_TYPE_3,
    DBAL_FIELD_FWD_LAYER_TYPE_4,
    DBAL_FIELD_FWD_LAYER_TYPE_5,
    DBAL_FIELD_FWD_LAYER_TYPE_6,
    DBAL_FIELD_FWD_LAYER_TYPE_MINUS_1,
};
static const dbal_fields_e dnx_ipmf1_cs_qual_inlif_profile_map[] = {
    DBAL_FIELD_IN_LIF_PROFILE_MAP_0,
    DBAL_FIELD_IN_LIF_PROFILE_MAP_1,
};
static const dbal_fields_e dnx_ipmf1_cs_qual_outlif_range[] = {
    DBAL_FIELD_OUT_LIF_RANGE_0,
    DBAL_FIELD_OUT_LIF_RANGE_1,
};
static const dbal_fields_e dnx_ipmf1_cs_qual_fwd_layer_qual[] = {
    DBAL_FIELD_FWD_LAYER_QUAL_MINUS_1,
    DBAL_FIELD_FWD_LAYER_QUAL_0,
    DBAL_FIELD_FWD_LAYER_QUAL_1,
    DBAL_FIELD_FWD_LAYER_QUAL_2,
};

static const dnx_field_cs_qual_map_t dnx_ipmf1_cs_qual_map[bcmFieldQualifyCount] = {
        [bcmFieldQualifyPortClassPacketProcessing]  = {1, DBAL_FIELD_IN_PORT_CS_PROFILE, },
        [bcmFieldQualifyPortClassTrafficManagement] = {1, DBAL_FIELD_PTC_CS_PROFILE, },

        [bcmFieldQualifyPtch]                    = {1, DBAL_FIELD_PRT_QUALIFIER_RAW, },
        [bcmFieldQualifyAppTypeRaw]              = {1, DBAL_FIELD_ACL_CONTEXT_PROFILE, },
        [bcmFieldQualifyAppType]                 = {1, DBAL_FIELD_ACL_CONTEXT_PROFILE, BCM_TO_DNX_ADDITIONAL_OBJ , NULL, 0, NULL, NULL, dnx_field_convert_app_type_context_profile},
        [bcmFieldQualifyAppTypePredefinedRaw]    = {1, DBAL_FIELD_FWD_CONTEXT_PROFILE, },
        [bcmFieldQualifyAppTypePredefined]       = {1, DBAL_FIELD_FWD_CONTEXT_PROFILE, BCM_TO_DNX_ADDITIONAL_OBJ , NULL, 0, NULL, NULL, dnx_field_convert_app_type_predef_only},
        [bcmFieldQualifyForwardingLayerIndex]    = {1, DBAL_FIELD_FWD_LAYER_INDEX, },
        [bcmFieldQualifyRxTrapCode]              = {1, DBAL_FIELD_CPU_TRAP_CODE, },
        [bcmFieldQualifyInVportClass]            = {sizeof(dnx_ipmf1_cs_qual_inlif_profile_map) / sizeof(dbal_fields_e),
                                                    DBAL_FIELD_EMPTY, 0, (dbal_fields_e *) dnx_ipmf1_cs_qual_inlif_profile_map},
        [bcmFieldQualifyVPortRangeCheck]         = {sizeof(dnx_ipmf1_cs_qual_outlif_range) / sizeof(dbal_fields_e),
                                                    DBAL_FIELD_EMPTY,0 , (dbal_fields_e *) dnx_ipmf1_cs_qual_outlif_range},
        [bcmFieldQualifyVlanFormatRaw]           = {1, DBAL_FIELD_INCOMING_TAG_STRUCTURE, },
        [bcmFieldQualifyVlanFormat]              = {1, DBAL_FIELD_INCOMING_TAG_STRUCTURE, BCM_TO_DNX_ADDITIONAL_OBJ , NULL, 0, NULL, NULL,
                                                    dnx_field_convert_vlan_format},

        [bcmFieldQualifyExternalHit0]            = {1, DBAL_FIELD_ELK_FOUND_RESULT_0, },
        [bcmFieldQualifyExternalHit1]            = {1, DBAL_FIELD_ELK_FOUND_RESULT_1, },
        [bcmFieldQualifyExternalHit2]            = {1, DBAL_FIELD_ELK_FOUND_RESULT_2, },
        [bcmFieldQualifyExternalHit3]            = {1, DBAL_FIELD_ELK_FOUND_RESULT_3, },
        [bcmFieldQualifyExternalHit4]            = {1, DBAL_FIELD_ELK_FOUND_RESULT_4, },
        [bcmFieldQualifyExternalHit5]            = {1, DBAL_FIELD_ELK_FOUND_RESULT_5, },
        [bcmFieldQualifyExternalHit6]            = {1, DBAL_FIELD_ELK_FOUND_RESULT_6, },
        [bcmFieldQualifyExternalHit7]            = {1, DBAL_FIELD_ELK_FOUND_RESULT_7, },

        [bcmFieldQualifyForwardingTypeRaw]       = {sizeof(dnx_ipmf1_cs_qual_layer_type) / sizeof(dbal_fields_e),
                                                    DBAL_FIELD_EMPTY,0, (dbal_fields_e *) dnx_ipmf1_cs_qual_layer_type, 7},
        [bcmFieldQualifyForwardingType]          = {sizeof(dnx_ipmf1_cs_qual_layer_type) / sizeof(dbal_fields_e),
                                                    DBAL_FIELD_EMPTY, BCM_TO_DNX_ADDITIONAL_OBJ, (dbal_fields_e *) dnx_ipmf1_cs_qual_layer_type, 7,
                                                    NULL, NULL, dnx_field_convert_forwarding_type},

        [bcmFieldQualifyForwardingLayerQualifier]= {sizeof(dnx_ipmf1_cs_qual_fwd_layer_qual) / sizeof(dbal_fields_e),
                                                    DBAL_FIELD_EMPTY, 0, (dbal_fields_e *) dnx_ipmf1_cs_qual_fwd_layer_qual, 1},
        [bcmFieldQualifyTracePacket]             = {1, DBAL_FIELD_TRACE_PACKET, },


        [bcmFieldQualifyAcInLifWideData]         = {1, DBAL_FIELD_CONTAINER_24_0_RANGE_16_23, },

};

static const dbal_fields_e dnx_ipmf2_cs_qual_cascaded_key_qual[] = {
    DBAL_FIELD_PMF1_TCAM_ACTION_0_MSB,
    DBAL_FIELD_PMF1_TCAM_ACTION_1_MSB,
    DBAL_FIELD_PMF1_TCAM_ACTION_2_MSB,
    DBAL_FIELD_PMF1_TCAM_ACTION_3_MSB,
};

static const dnx_field_cs_qual_map_t dnx_ipmf2_cs_qual_map[bcmFieldQualifyCount] = {

     /**
      * bcmFieldQualifyContextId should not be exposed, this is managed by SDK
      * 1) Cascaded from is mentioned in Context Create API of iPMF2 stage - saved in SW state and profile allocated
      * 2) There is no reason to switch context to iPMF2 unless additional Qualifier in CS is used
      *    (otherwise user can use iPMF1 context configuration)
      *    When the presel set API will be called for context in iPMF2 (with additional qualifier) then SDK also set
      *    the qualifier of context profile which was saved previously
 */
    /*[bcmFieldQualifyContextId]               = {1, DBAL_FIELD_PMF1_PROGRAM_SELECTION_PROFILE},*/
    /*
     * In Jericho there were tcam_action lsb instead
     */
    [bcmFieldQualifyCascadedKeyValue]        = {sizeof(dnx_ipmf2_cs_qual_cascaded_key_qual) / sizeof(dbal_fields_e),
                                                DBAL_FIELD_EMPTY,0 , (dbal_fields_e *) dnx_ipmf2_cs_qual_cascaded_key_qual,
                                                0, dnx_field_convert_cs_qual_type_cascaded_group,
                                                dnx_field_convert_cs_qual_type_cascaded_group_back, NULL},
/*  DBAL_FIELD_PMF1_DIRECT_MSB, */
/*  DBAL_FIELD_PMF1_EXEM_MSB, */
    [bcmFieldQualifyStateTableData]          = {1, DBAL_FIELD_STATE_TABLE_DATA_READ, 0,
                                                NULL, 0, NULL, NULL, NULL},
    [bcmFieldQualifyStateTableDataWrite]     = {1, DBAL_FIELD_STATE_TABLE_DATA_WRITE, 0,
                                                NULL, 0, NULL, NULL, NULL},
/*  DBAL_FIELD_STATISTICS_MAPPING_REQUIRED, */
    /**
     * using bcmFieldQualifyCompareKeysResult0, bcmFieldQualifyCompareKeysResult1 as CS qualifiers, each has 3 bits:
     * bit 2 = greater than
     * bit 1 = equal
     * bit 0 = smaller than
     */
    [bcmFieldQualifyCompareKeysResult0]      = {1, DBAL_FIELD_CMP_OUTPUT_PAIR_1, },
    [bcmFieldQualifyCompareKeysResult1]      = {1, DBAL_FIELD_CMP_OUTPUT_PAIR_2, },
/*  DBAL_FIELD_CMP_OUTPUT, */
/*  DBAL_FIELD_RESERVED_0, */
/*  DBAL_FIELD_RESERVED_1, */
/*  DBAL_FIELD_RESERVED_2, */
};

static const dbal_fields_e dnx_ipmf3_cs_qual_outlif_range[] = {
    DBAL_FIELD_OUT_LIF_RANGE_0,
    DBAL_FIELD_OUT_LIF_RANGE_1,
    DBAL_FIELD_OUT_LIF_RANGE_2,
    DBAL_FIELD_OUT_LIF_RANGE_3,
};

static const dnx_field_cs_qual_map_t dnx_ipmf3_cs_qual_map[bcmFieldQualifyCount] = {
    [bcmFieldQualifyPortClassPacketProcessing]  = {1, DBAL_FIELD_IN_PORT_CS_PROFILE, },
    [bcmFieldQualifyPortClassTrafficManagement] = {1, DBAL_FIELD_PTC_CS_PROFILE, },
    [bcmFieldQualifyForwardingTypeRaw]          = {1, DBAL_FIELD_FWD_LAYER_TYPE_0, },
    [bcmFieldQualifyForwardingType]             = {1, DBAL_FIELD_FWD_LAYER_TYPE_0, BCM_TO_DNX_ADDITIONAL_OBJ, NULL, 0, NULL, NULL,
                                                   dnx_field_convert_forwarding_type},
    [bcmFieldQualifyForwardingLayerQualifier]   = {1, DBAL_FIELD_FWD_LAYER_QUALIFIER, },
    [bcmFieldQualifyContextId]                  = {1, DBAL_FIELD_ACL_CONTEXT_PROFILE, },
    [bcmFieldQualifyRpfEcmpMode]                = {1, DBAL_FIELD_RPF_ECMP_MODE, },
/*  DBAL_FIELD_PEM_SCRATCH_PAD_MSB, */
/*  DBAL_FIELD_PEM_SCRATCH_PAD_LSB, */
/*  DBAL_FIELD_COUNTER_PROFILE_MAP_0, */
/*  DBAL_FIELD_COUNTER_PROFILE_MAP_1, */
/*  DBAL_FIELD_COUNTER_PROFILE_MAP_2, */
    [bcmFieldQualifyDstType]                    = {1, DBAL_FIELD_CS_DESTINATION, },
/*  DBAL_FIELD_WEAK_TM_VALID, */
    [bcmFieldQualifyVPortRangeCheck]            = {sizeof(dnx_ipmf3_cs_qual_outlif_range) / sizeof(dbal_fields_e),
                                                   DBAL_FIELD_EMPTY, 0, (dbal_fields_e *) dnx_ipmf3_cs_qual_outlif_range},

    [bcmFieldQualifyRxTrapProfile]              = {1, DBAL_FIELD_TRAP_CODE_PROFILE, },
/*  DBAL_FIELD_RPF_MC_MODE, */
/*  DBAL_FIELD_RPF_DST_IS_ECMP, */
};

static const dbal_fields_e dnx_epmf_cs_qual_fwd_layer_type[] = {
    DBAL_FIELD_FWD_LAYER_TYPE_MINUS_1,
    DBAL_FIELD_FWD_LAYER_TYPE_0,
    DBAL_FIELD_FWD_LAYER_TYPE_1,
    DBAL_FIELD_FWD_LAYER_TYPE_2,
};
static const dbal_fields_e dnx_epmf_cs_qual_fwd_layer_qual[] = {
    DBAL_FIELD_FWD_LAYER_QUAL_MINUS_1,
    DBAL_FIELD_FWD_LAYER_QUAL_0,
    DBAL_FIELD_FWD_LAYER_QUAL_1,
};

static const dnx_field_cs_qual_map_t dnx_epmf_cs_qual_map[bcmFieldQualifyCount] = {
        [bcmFieldQualifyPortClassPacketProcessing]  = {1, DBAL_FIELD_PER_PORT_TABLE_DATA_PMF_PROFILE, },
        [bcmFieldQualifyContextIdRaw]               = {1, DBAL_FIELD_FWD_CONTEXT_PROFILE, },
        [bcmFieldQualifyContextId]                  = {1, DBAL_FIELD_FWD_CONTEXT_PROFILE, BCM_TO_DNX_ADDITIONAL_OBJ , NULL, 0, NULL, NULL, dnx_field_convert_forwarding_context},
        [bcmFieldQualifyVlanFormatRaw]              = {1, DBAL_FIELD_ETHERNET_TAG_FORMAT, },
        [bcmFieldQualifyVlanFormat]                 = {1, DBAL_FIELD_ETHERNET_TAG_FORMAT, BCM_TO_DNX_ADDITIONAL_OBJ , NULL, 0, NULL, NULL,
                                                       dnx_field_convert_vlan_format},
        [bcmFieldQualifyPphPresent]                 = {1, DBAL_FIELD_PPH_PRESENT, },
    /*  DBAL_FIELD_TSH_EXTENISON_PRESENT, */
    /*  DBAL_FIELD_BIER_BFR_EXTENSION_PRESENT, */
    /*  DBAL_FIELD_TM_DESTINATION_EXTENSION_PRESENT, */
        [bcmFieldQualifyLearnExtensionPresent]      = {1, DBAL_FIELD_LEARN_EXTENSION_PRESENT, },
        [bcmFieldQualifyFheiSize]                   = {1, DBAL_FIELD_FHEI_SIZE, },
    /*  DBAL_FIELD_LIF_EXTENSION_TYPE, */
    /*  DBAL_FIELD_OUT_LIF_PROFILE, */
        [bcmFieldQualifyOutVportClass]               = {1, DBAL_FIELD_PMF_OUT_LIF_PROFILE, },
        [bcmFieldQualifyInterfaceClassL2]            = {1, DBAL_FIELD_PMF_OUT_RIF_PROFILE, },
    /*  DBAL_FIELD_RESERVED_0, */
        [bcmFieldQualifyDstClassField]              = {1,DBAL_FIELD_PPH_VALUE_1, },
        [bcmFieldQualifySrcClassField]              = {1,DBAL_FIELD_PPH_VALUE_2_LSB, },
        [bcmFieldQualifyUDHBase0]                    = {1, DBAL_FIELD_UDH_BASE_0, },
        [bcmFieldQualifyUDHBase1]                    = {1, DBAL_FIELD_UDH_BASE_1, },
        [bcmFieldQualifyUDHBase2]                    = {1, DBAL_FIELD_UDH_BASE_2, },
        [bcmFieldQualifyUDHBase3]                    = {1, DBAL_FIELD_UDH_BASE_3, },
        [bcmFieldQualifyForwardingTypeRaw]           = {sizeof(dbal_fields_e),
                                                        DBAL_FIELD_EMPTY, 0, (dbal_fields_e *) dnx_epmf_cs_qual_fwd_layer_type, 1},
        [bcmFieldQualifyForwardingType]             = {sizeof(dbal_fields_e),
                                                       DBAL_FIELD_EMPTY, BCM_TO_DNX_ADDITIONAL_OBJ, (dbal_fields_e *) dnx_epmf_cs_qual_fwd_layer_type,
                                                       1, NULL, NULL, dnx_field_convert_forwarding_type},
        [bcmFieldQualifyForwardingLayerQualifier]   = {sizeof(dnx_epmf_cs_qual_fwd_layer_qual) / sizeof(dbal_fields_e),
                                                       DBAL_FIELD_EMPTY, 0,
                                                       (dbal_fields_e *) dnx_epmf_cs_qual_fwd_layer_qual, 1},
        [bcmFieldQualifyInVportClass0]               = {1, DBAL_FIELD_IN_LIF_PROFILE_0, },



        [bcmFieldQualifyFtmhAsePresent]            = {1, DBAL_FIELD_APP_SPECIFIC_EXTENSION_PRESENT, },
        [bcmFieldQualifyFtmhAseType]               = {1, DBAL_FIELD_APP_SPECIFIC_EXTENSION_LSB, },
    /*  DBAL_FIELD_FHEI_EXTENSION_LSB, */
    /*  DBAL_FIELD_FWD_STRENGTH, */
    /*  DBAL_FIELD_FABRIC_OR_EGRESS_MC, */
};

static const dbal_fields_e dnx_external_cs_qual_fwd_layer_type[] = {
    DBAL_FIELD_LAYER_TYPE_PREVIOUS_LAYER,
    DBAL_FIELD_LAYER_TYPE_CURRENT_LAYER,
    DBAL_FIELD_LAYER_TYPE_NEXT_LAYER,
};
static const dbal_fields_e dnx_external_cs_qual_fwd_layer_qual[] = {
    DBAL_FIELD_LAYER_QUALIFIER_PREVIOUS_LAYER,
    DBAL_FIELD_LAYER_QUALIFIER_CURRENT_LAYER,
    DBAL_FIELD_LAYER_QUALIFIER_NEXT_LAYER,
};

static const dnx_field_cs_qual_map_t dnx_external_cs_qual_map[bcmFieldQualifyCount] = {
/**
  * bcmFieldQualifyContextId should not be exposed, this is managed by SDK
  * All presels to a dymanic Apptype (the only kind) include the context of the Apptype they cascaded from.
  * Uses DBAL_FIELD_CURRENT_CONTEXT_PROFILE.
 */
/*  DBAL_FIELD_CURRENT_CONTEXT_PROFILE,  */
/*  DBAL_FIELD_SAVED_CONTEXT_PROFILE,  */
    [bcmFieldQualifyForwardingLayerQualifier]= {sizeof(dnx_external_cs_qual_fwd_layer_qual) / sizeof(dbal_fields_e),
                                                DBAL_FIELD_EMPTY, 0, (dbal_fields_e *) dnx_external_cs_qual_fwd_layer_qual, 1},
    [bcmFieldQualifyForwardingTypeRaw]          = {sizeof(dnx_external_cs_qual_fwd_layer_type) / sizeof(dbal_fields_e),
                                                       DBAL_FIELD_EMPTY, 0, (dbal_fields_e *) dnx_external_cs_qual_fwd_layer_type, 1},
    [bcmFieldQualifyForwardingType]          = {sizeof(dnx_external_cs_qual_fwd_layer_type) / sizeof(dbal_fields_e),
                                                DBAL_FIELD_EMPTY, BCM_TO_DNX_ADDITIONAL_OBJ, (dbal_fields_e *) dnx_external_cs_qual_fwd_layer_type,
                                                1, NULL, NULL, dnx_field_convert_forwarding_type},
    [bcmFieldQualifyForwardingLayerIndex]    = {1, DBAL_FIELD_FWD_LAYER_INDEX, },
    [bcmFieldQualifyForwardingProfile]       = {1, DBAL_FIELD_FWD_DOMAIN_PROFILE, },
    [bcmFieldQualifyIpMulticastCompatible]   = {1, DBAL_FIELD_IS_COMPATIBLE_MULTICAST, },

    /* DBAL_FIELD_FLPA_PD_FWD_HIT*/
    /* DBAL_FIELD_FLPA_PD_RPF_HIT */
/*  DBAL_FIELD_DUMMY_EGW_STATUS,  */
    [bcmFieldQualifyRxTrapCode]              = {1, DBAL_FIELD_INGRESS_TRAP_CODE, },
/*  DBAL_FIELD_LAST_LIF_CS_PROFILE,  */
/*  DBAL_FIELD_PREVIOUS_LIF_CS_PROFILE,  */
    [bcmFieldQualifyVlanFormatRaw]           = {1, DBAL_FIELD_INCOMING_TAG_STRUCTURE, },
    [bcmFieldQualifyVlanFormat]              = {1, DBAL_FIELD_INCOMING_TAG_STRUCTURE, BCM_TO_DNX_ADDITIONAL_OBJ , NULL, 0, NULL, NULL,
                                                dnx_field_convert_vlan_format},
/*  DBAL_FIELD_LIF_SERVICE_TYPE,  */
/*  DBAL_FIELD_OAM_CS_PROFILE,   */
    [bcmFieldQualifyPortClassPacketProcessing]  = {1, DBAL_FIELD_PP_PORT_PROFILE, },
    [bcmFieldQualifyPortClassTrafficManagement] = {1, DBAL_FIELD_PORT_TERMINATION_PTC_PROFILE, },


/*  DBAL_FIELD_FWD1_CONTEXT_ID,  */
/*  DBAL_FIELD_VT1_PEM_CS_VAR  */
};
/* *INDENT-ON* */

/*
 * End of Context Selection related data
 * }
 */

/* *INDENT-OFF* */
/*
 * Qualifier related data below
 * {
 */


/*
 * Qualifier related data below
 * {
 */


static const char *bcm_qual_description[bcmFieldQualifyCount] = {

    [bcmFieldQualifyDstMac]                = "Destination MAC address",
    [bcmFieldQualifySrcMac]                = "Source MAC address",
    [bcmFieldQualifyEtherTypeUntagged]     = "802.1 Ethernet Type, only for untagged frames",

    [bcmFieldQualifyTpid]                  = "VLAN TPID, offset of the value is inside the VLAN tag. Need to add the offset where the tag start in the Layer (inner/outer..)",
    [bcmFieldQualifyVlanId]                = "VLAN ID, offset of the value is inside the VLAN tag. Need to add the offset where the tag start in the Layer (inner/outer..)",
    [bcmFieldQualifyVlanPri]               = "VLAN P-bits , offset of the value is inside the VLAN tag. Need to add the offset where the tag start in the Layer (inner/outer..)",
    [bcmFieldQualifyVlanCfi]               = "VLAN CFI , offset of the value is inside the VLAN tag. Need to add the offset where the tag start in the Layer (inner/outer..)",
    [bcmFieldQualifyVlanPriCfi]            = "VLAN P-bits and CFI , offset of the value is inside the VLAN tag. Need to add the offset where the tag start in the Layer (inner/outer..)",

    [bcmFieldQualifyArpSenderIp4]          = "Sender IPv4 field of ARP header",
    [bcmFieldQualifyArpTargetIp4]          = "Target IPv4 field of ARP header",
    [bcmFieldQualifyArpOpcode]             = "Opcode field of ARP header.",

    [bcmFieldQualifyMplsLabel]             = "MPLS Label  ",
    [bcmFieldQualifyMplsLabelId]           = "ID field of MPLS Label",
    [bcmFieldQualifyMplsLabelTtl]          = "TTL field of MPLS Label",
    [bcmFieldQualifyMplsLabelBos]          = "BOS field of MPLS Label",
    [bcmFieldQualifyMplsLabelExp]          = "TTL field of MPLS Label",

    [bcmFieldQualifySrcIp]                 = "Packet Source IP address",
    [bcmFieldQualifyDstIp]                 = "Packet destination IP address",
    [bcmFieldQualifyIpFlags]               = "IP Flags Field",

    [bcmFieldQualifySrcIp6]                = "IPv6 Source address",
    [bcmFieldQualifyDstIp6]                = "IPv6 destination address",

    [bcmFieldQualifySrcIp6High]            = "Source IPv6 Address (High/Upper 64 bits, 64 MSB)",
    [bcmFieldQualifyDstIp6High]            = "Destination IPv6 Address (High/Upper 64 bits, 64 MSB)",
    [bcmFieldQualifySrcIp6Low]             = "Source IPv6 Address (Low/Lower 64 bits, 64 LSB)",
    [bcmFieldQualifyDstIp6Low]             = "Destination IPv6 Address (Low/Lower 64 bits, 64 LSB)",
    [bcmFieldQualifyIp6FlowLabel]          = "IPv6 Flow Label.",

    [bcmFieldQualifyL4DstPort]             = "L4 destination port",
    [bcmFieldQualifyL4SrcPort]             = "L4 source port",
    [bcmFieldQualifyTcpControl]            = "TCP control Flags",

    [bcmFieldQualifyIpFrag]                  = "IP fragments",
    [bcmFieldQualifyLayerRecordType]         = "Qualifies on the Layer Record Type. For possible values look at bcm_field_layer_type_t",
    [bcmFieldQualifyLayerRecordOffset]       = "Qualifies on the Layer Record Offset",
    [bcmFieldQualifyLayerRecordQualifier]    = "Qualifies on the Layer Record qualifier",
    [bcmFieldQualifyEthernetMulticast]       = "Qualify on Ethernet packets which destination MAC address is multicast (MAC-DA[40] is set)",
    [bcmFieldQualifyEthernetBroadcast]       = "Qualify on Ethernet packets which destination MAC address is broadcast (MAC-DA[47:0] is all 1).",
    [bcmFieldQualifyEthernetFirstTpidExist]  = "Qualify on Ethernet packets which their first Vlan TPID exists.",
    [bcmFieldQualifyEthernetFirstTpidIndex]  = "Qualify on Ethernet packets  first Vlan TPID Index",
    [bcmFieldQualifyEthernetSecondTpidExist] = "Qualify on Ethernet packets which their second Vlan TPID exists.",
    [bcmFieldQualifyEthernetSecondTpidIndex] = "Qualify on Ethernet packets  second Vlan TPID Index",
    [bcmFieldQualifyEthernetThirdTpidExist]  = "Qualify on Ethernet packets which their third Vlan TPID exists.",
    [bcmFieldQualifyEthernetThirdTpidIndex]  = "Qualify on Ethernet packets  third Vlan TPID Index",
    [bcmFieldQualifyIpMulticastCompatible]   = "Packet is compatible for multicast. ",
    [bcmFieldQualifyIpHasOptions]            = "Qualify on IPv4 packets where the IP Header include options.",
    [bcmFieldQualifyIpFirstFrag]             = "Qualify on IPv4 packets where the IP Header is fragmented and this is the first fragment.",
    [bcmFieldQualifyIpTunnelType]            = "Qualify on IPv4 TunnelType (see bcmFieldTunnelXXX).",
    [bcmFieldQualifyIpTunnelTypeRaw]         = "Raw Qualify on IPv4 TunnelType.",
    [bcmFieldQualifyIp6MulticastCompatible]  = "Qualify on IPv6 packets which their DIP is Multicast (8msb are 0xFF).",
    [bcmFieldQualifyIp6FirstAdditionalHeaderExist]  = "Qualify on IPv6 packets first additional header exists",
    [bcmFieldQualifyIp6FirstAdditionalHeader]    = "Qualify on IPv6 packets first additional header",
    [bcmFieldQualifyIp6SecondAdditionalHeaderExist]  = "Qualify on IPv6 packets second additional header exists",
    [bcmFieldQualifyIp6SecondAdditionalHeader]   = "Qualify on IPv6 packets second additional header",
    [bcmFieldQualifyItmhPphType]                 = "Qualify on Ingress TM Header PPH type",
    [bcmFieldQualifyL4PortRangeCheck]            = "Qualify on L4 Ops (L4 source and dest ports) range. The qualifier is a bitmap and not specific range id, "
                                                   "it means if range 2 was configured and the qualifier is hit, the entry value is expected to be 0x100",
    [bcmFieldQualifyRangeFirstHit0]              = "Qualify on L4 Ops Extended Encoder0. Encoder0 can be configured to consider a certain subset of range types, "
                                                   "so that for the chosen subset, the encoder will output 8b at max that consists of 1 valid bit (indicates if "
                                                   "at least one hit occurred inside the subset of chosen ranges) and 7b of the first hit index, the valid 1b "
                                                   "offset is dynamically allocated according to the number of chosen range types in the subset (i.e offset is "
                                                   "6 when 1 type is selected, 7 when 2-3 types are selected, and 8 when 4 types are selected).",
    [bcmFieldQualifyRangeFirstHit1]              = "See bcmFieldQualifyRangeFirstHit0",
    [bcmFieldQualifyRangeFirstHit2]              = "See bcmFieldQualifyRangeFirstHit0",
    [bcmFieldQualifyRangeFirstHit3]              = "See bcmFieldQualifyRangeFirstHit0",
    [bcmFieldQualifyGeneratedTtl]                = "Qualify on Packet's generate Time-To-Live",
    [bcmFieldQualifyColor]                       = "Qualify based on packet color, value should be one of BCM_FIELD_COLOR_XXX",
    [bcmFieldQualifyPacketIsIEEE1588]            = "Indicating whether the packet is 1588",
    [bcmFieldQualifyIEEE1588Encapsulation]       = "IEEE-1588 Encapsulation according to bcm_field_IEEE1588Encap_t",
    [bcmFieldQualifyIEEE1588CompensateTimeStamp] = "IEEE-1588 update time stamp",
    [bcmFieldQualifyIEEE1588Command]             = "Command used by egress pipeline, indicating if CF(correction field) needs to be update",
    [bcmFieldQualifyIEEE1588HeaderOffset]        = "This field indicates the offset in bytes  of the IEEE-1588 header",
    [bcmFieldQualifyRxTrapCodeForSnoop]          = "Qualify on Snoop Code",
    [bcmFieldQualifyRxTrapCode]                  = "Qualify on RX Trap Code",
    [bcmFieldQualifyRxTrapData]                  = "Qualify on RX Trap qualifier",
    [bcmFieldQualifyRxTrapStrength]              = "Qualify on RX Trap Strength",
    [bcmFieldQualifyDstRpfGport]                 = "RPF destination (gport) for the RPF check",
    [bcmFieldQualifyKeyGenVar]                   = "Match on configured key Gen Variable (values that was configured for context)",
    [bcmFieldQualifySrcPort]                     = "Qualify on source port, value should be encoded as SYS_PORT GPORT Type",
    [bcmFieldQualifySrcModPortGport]             = "Qualify on Source module/port pair, value should be encoded as SYS_PORT GPORT Type",
    [bcmFieldQualifyDstPort]                     = "Qualify on destination port, all ingress stages expect raw value, egress expects GPORT encoded as SYS_PORT",
    [bcmFieldQualifyDstMulticastGroup]           = "Multicast Group id, encoded as GPORT",
    [bcmFieldQualifyFlowId]                      = "Qualify on destination Flow-Id",
    [bcmFieldQualifyDstL3Egress]                 = "L3 Egress Interface, Raw value of SYS_PORT",
    [bcmFieldQualifyOamUpMep]                    = "It indicates if the OAM packet is vUP-MEP (sent to a destination in the network, as opposed to a specific port)",
    [bcmFieldQualifyOamSubtype]                  = "In OAM the packet type is specified in the OAM header and mapped to a subtype in the hardware",
    [bcmFieldQualifyOamStampOffset]              = "indicates the offset to the position, in the OAM header, where the ToD or counter value should be stamped relative to the start of packet",
    [bcmFieldQualifyOamHeaderOffset]             = "indicates the offset of the OAM header relative to the start of packet (as opposed to start of header-offset)",
    [bcmFieldQualifyOamMepId]                    = "If MEP is handled in OAMP, then the OAM-ID is the MEP-ID (equivalent to the index used to access the MEP DB). ",
    [bcmFieldQualifyOamMeterDisable]             = "attribute that is passed to the PMF and can also be configured by the user per MEP.",
    [bcmFieldQualifyPacketSize]                  = " Packet size in Bytes.",
    [bcmFieldQualifyInVPort0]                    = "In LIF 0, value should be GPORT with sub-type LIF",
    [bcmFieldQualifyInVPort1]                    = "In LIF 1, value should be GPORT with sub-type LIF",
    [bcmFieldQualifyOutVPort0]                   = "Out LIF 0, value should be GPORT with sub-type LIF",
    [bcmFieldQualifyOutVPort1]                   = "Out LIF 1, value should be GPORT with sub-type LIF",
    [bcmFieldQualifyOutVPort2]                   = "Out LIF 2, value should be GPORT with sub-type LIF",
    [bcmFieldQualifyOutVPort3]                   = "Out LIF 3, value should be GPORT with sub-type LIF",
    [bcmFieldQualifyTrunkHashResult]             = "Trunk Hash Result (i.e., the Load-balancing Key)",
    [bcmFieldQualifyIntPriority]                 = "Internal priority (TC)",
    [bcmFieldQualifyExternalHit0]                = "External Lookup Hit 0",
    [bcmFieldQualifyExternalHit1]                = "External Lookup Hit 1",
    [bcmFieldQualifyExternalHit2]                = "External Lookup Hit 2",
    [bcmFieldQualifyExternalHit3]                = "External Lookup Hit 3",
    [bcmFieldQualifyExternalHit4]                = "External Lookup Hit 4",
    [bcmFieldQualifyExternalHit5]                = "External Lookup Hit 5",
    [bcmFieldQualifyExternalHit6]                = "External Lookup Hit 6",
    [bcmFieldQualifyExternalHit7]                = "External Lookup Hit 7",
    [bcmFieldQualifyInVportClass0]               = "In-LIF Profile 0",
    [bcmFieldQualifyInVportClass1]               = "In-LIF Profile 1",
    [bcmFieldQualifyOutVportClass]               = "Out-LIF Profile",
    [bcmFieldQualifyTranslatedOuterVlanId ]      = "Translated Outer VLAN ID  ",
    [bcmFieldQualifyTranslatedOuterVlanPri]      = "Translated Outer VLAN priority" ,
    [bcmFieldQualifyTranslatedOuterVlanCfi]      = "Translated Outer VLAN CFI" ,
    [bcmFieldQualifyTranslatedInnerVlanId ]      = "Translated inner VLAN Id" ,
    [bcmFieldQualifyTranslatedInnerVlanPri]      = "Translated inner VLAN priority" ,
    [bcmFieldQualifyTranslatedInnerVlanCfi]      = "Translated inner VLAN CFI" ,
    [bcmFieldQualifyVlanAction]                  = "Vlan translation action  ID" ,
    [bcmFieldQualifyInPort]                      = "Ingress Port, value should be encoded as LOCAL GPORT Type. Includes core ID, and can be used by entries for all cores.",
    [bcmFieldQualifyInPortWithoutCore]           = "Ingress Port, value should be encoded as LOCAL GPORT Type. Does not include the core ID, caution should be taken when not adding entry by core.",
    [bcmFieldQualifyPacketLengthRangeCheck]      = "Packet length range, range id which packet matched upon",
    [bcmFieldQualifyVlanFormat]                  = "Incoming vlan structure format (untagged/single/double/priority-tagged), value should be set of BCM_FIELD_VLAN_FORMAT_ flags",
    [bcmFieldQualifyL2Learn]                     = "L2 learn enable",
    [bcmFieldQualifyTrillEgressRbridge]          = "Egress RBridge Nickname",
    [bcmFieldQualifyISid]                        = "I-SID (MAC-in-MAC lookup-id)",
    [bcmFieldQualifyMplsForwardingLabelAction]   = "MPLS forwarding label action.",
    [bcmFieldQualifyVrf]                         = "VRF Id",
    [bcmFieldQualifyVpn]                         = "VSI Id",
    [bcmFieldQualifyForwardingLayerIndex]        = "Qualify on the Forwarding Layer Index",
    [bcmFieldQualifyInterfaceClassL2]            = "VSI Profile",
    [bcmFieldQualifyIncomingIpIfClass]           = "RIF profile ",
    [bcmFieldQualifyAcInLifWideData]             = "Qualifies on the Ac in-lif wide(generic) data. In case of Context Selection it is only 8 bits.",
    [bcmFieldQualifyNativeAcInLifWideData]       = "Qualifies on the native Ac in-lif wide(generic) data",
    [bcmFieldQualifyPweInLifWideData]            = "Qualifies on the Pwe in-lif wide(generic) data.",
    [bcmFieldQualifyIpTunnelInLifWideData]       = "Qualifies on the Ip Tunnel in-lif wide(generic) data.",
    [bcmFieldQualifyHashValue]                   = "Hash value",
    [bcmFieldQualifyNetworkLoadBalanceKey]       = "Network Load Balance Key",
    [bcmFieldQualifyEcmpLoadBalanceKey0]         = "ECMP Load Balance Key 0",
    [bcmFieldQualifyEcmpLoadBalanceKey1]         = "ECMP Load Balance Key 1",
    [bcmFieldQualifyEcmpLoadBalanceKey2]         = "ECMP Load Balance Key 2",
    [bcmFieldQualifyStateTableData]              = "state Table payload, as read from state table",
    [bcmFieldQualifyStateTableDataWrite]         = "state Table payload, as written to state table",
    [bcmFieldQualifyCompareKeysResult0]          = "compare keys result 0",
    [bcmFieldQualifyCompareKeysResult1]          = "compare keys result 1",
    [bcmFieldQualifyCompareKeysTcam0]            = "compare keys and TCAM result 0",
    [bcmFieldQualifyCompareKeysTcam1]            = "compare keys and TCAM result 1",
    [bcmFieldQualifyUDHData0]                    = "User Defined Header 0 payload MS bits of UDH",
    [bcmFieldQualifyUDHData1]                    = "User Defined Header 1 payload",
    [bcmFieldQualifyUDHData2]                    = "User Defined Header 2 payload",
    [bcmFieldQualifyUDHData3]                    = "User Defined Header 3 payload LS bits of UDH",
    [bcmFieldQualifyUDHBase0]                    = "User Defined Header 0 encoded type and size - value 1: type PMF size 32b",
    [bcmFieldQualifyUDHBase1]                    = "User Defined Header 1 encoded type and size - value 1: type PMF size 32b",
    [bcmFieldQualifyUDHBase2]                    = "User Defined Header 2 encoded type and size - value 1: type PMF size 32b",
    [bcmFieldQualifyUDHBase3]                    = "User Defined Header 3 encoded type and size - value 1: type PMF size 32b",
    [bcmFieldQualifyPortClassPacketProcessing]   = "PP port profile",
    [bcmFieldQualifyPortClassTrafficManagement]  = "TM port profile",
    [bcmFieldQualifyPortClassPacketProcessingGeneralData] = "PP port general data",
    [bcmFieldQualifyOutPortTrafficManagement]    = "Out TM-port",
    [bcmFieldQualifyDstClassField]               = "PPH Value 1, supported for JR1 mode only",
    [bcmFieldQualifySrcClassField]               = "PPH Value 2, supported for JR1 mode only",
    [bcmFieldQualifyInInterface]                 = "Ingress Logical Interface (LIF/RIF), value should be encoded as L3 interface",
    [bcmFieldQualifyOutInterface]                = "Egress Logical Interface (LIF/RIF), should be encoded as L3 interface",
    [bcmFieldQualifyFhei]                        = "DNX FHEI header field",
    [bcmFieldQualifyFheiSize]                    = "DNX FHEI header size in bytes",
    [bcmFieldQualifyRepCopy]                     = "qualify on Copy type. For possible values look at bcm_field_rep_copy_type_t",
    [bcmFieldQualifyEcnValue]                    = "qualify on metadata ECN / congestion info (including CNI)",
    [bcmFieldQualifyOamTsSystemHeader]           = "FTMH Application specific extension",
    [bcmFieldQualifyIPTProfile]                  = "Qualifies upon IPT Profile (End Of Packet Editing).",
    [bcmFieldQualifyRxSnoopCode]                 = "qualify upon Snoop profile",
    [bcmFieldQualifyOutPort]                     = "Egress port, value should be encoded as LOCAL GPORT Type. Does not include the core ID, caution should be taken when not adding entry by core.",
    [bcmFieldQualifyStackingRoute]               = "Stacking Route History bitmap",
    [bcmFieldQualifyContainer]                   = "This qualifier will be used as container in IPMF3, to receive the action buffer, when performing cascading between IPMF1/2 and IPMF3.",
    [bcmFieldQualifyRxSnoopStrength]             = "Snoop Strength",
    [bcmFieldQualifyStatSamplingCode]            = "Statistical Sampling Code value",
    [bcmFieldQualifyStatSamplingQualifier]       = "Statistical Sampling qualifier value",
    [bcmFieldQualifyRpfEcmpMode]                 = "RPF ECMP mode",
    [bcmFieldQualifyStatOamLM]                   = "OAM LM counter value, the index of the values are bcmFieldStatOamLmIndexXXX",
    [bcmFieldQualifyStatOamLMRaw]                = "OAM LM counter raw value",
    [bcmFieldQualifyBierStringOffset]            = "Qualifies upon Bier string offset.",
    [bcmFieldQualifyBierStringSize]              = "Qualifies upon Bier string size.",
    [bcmFieldQualifyPacketIsBier]                = "Qualifies upon Bier packets.",
    [bcmFieldQualifyTracePacket]                 = "Qualifies upon Packets that Trace was set for them",
    [bcmFieldQualifyForwardingAdditionalInfo]    = "Qualifies on the Forwarding additional info.",
    [bcmFieldQualifyStatId0]                     = "Qualifies on statistics ID 0.",
    [bcmFieldQualifyStatId1]                     = "Qualifies on statistics ID 1.",
    [bcmFieldQualifyStatId2]                     = "Qualifies on statistics ID 2.",
    [bcmFieldQualifyStatId3]                     = "Qualifies on statistics ID 3.",
    [bcmFieldQualifyStatId4]                     = "Qualifies on statistics ID 4.",
    [bcmFieldQualifyStatId5]                     = "Qualifies on statistics ID 5.",
    [bcmFieldQualifyStatId6]                     = "Qualifies on statistics ID 6.",
    [bcmFieldQualifyStatId7]                     = "Qualifies on statistics ID 7.",
    [bcmFieldQualifyStatId8]                     = "Qualifies on statistics ID 8.",
    [bcmFieldQualifyStatId9]                     = "Qualifies on statistics ID 9.",
    [bcmFieldQualifyStatProfile0]                = "Qualifies on statistics profile 0.",
    [bcmFieldQualifyStatProfile1]                = "Qualifies on statistics profile 1.",
    [bcmFieldQualifyStatProfile2]                = "Qualifies on statistics profile 2.",
    [bcmFieldQualifyStatProfile3]                = "Qualifies on statistics profile 3.",
    [bcmFieldQualifyStatProfile4]                = "Qualifies on statistics profile 4.",
    [bcmFieldQualifyStatProfile5]                = "Qualifies on statistics profile 5.",
    [bcmFieldQualifyStatProfile6]                = "Qualifies on statistics profile 6.",
    [bcmFieldQualifyStatProfile7]                = "Qualifies on statistics profile 7.",
    [bcmFieldQualifyStatProfile8]                = "Qualifies on statistics profile 8.",
    [bcmFieldQualifyStatProfile9]                = "Qualifies on statistics profile 9.",
    [bcmFieldQualifyExtStat0]                    = "Qualifies on External Statistics ID 0.",
    [bcmFieldQualifyExtStat1]                    = "Qualifies on External Statistics ID 1.",
    [bcmFieldQualifyExtStat2]                    = "Qualifies on External Statistics ID 2.",
    [bcmFieldQualifyExtStat3]                    = "Qualifies on External Statistics ID 3.",
    [bcmFieldQualifyExtStatProfile0]             = "Qualifies on External Statistics profile 0.",
    [bcmFieldQualifyExtStatProfile1]             = "Qualifies on External Statistics profile 1.",
    [bcmFieldQualifyExtStatProfile2]             = "Qualifies on External Statistics profile 2.",
    [bcmFieldQualifyExtStatProfile3]             = "Qualifies on External Statistics profile 3.",
    [bcmFieldQualifyStatMetaData]                = "Qualifies on Statistics meta data.",
    [bcmFieldQualifyPphPresent]                  = "Qualifies on PPH present field in ITMH.",
    [bcmFieldQualifyPacketProcessingInVportClass]= "Qualifies on PPH In-LIF profile.",
    [bcmFieldQualifyIp6NextHeader]               = "Next protocol field in IPv6 Header",
    [bcmFieldQualifyIp6HopLimit]                 = "Hop Count field in IPv6 Header",
    [bcmFieldQualifyIp6TrafficClass]             = "Traffic class field in IPv6 Header",
    [bcmFieldQualifyIp4Protocol]                 = "Qualify Upon IPv4 Protocol.",
    [bcmFieldQualifyIp4Tos]                      = "Qualify Upon IPv4 TOS.",
    [bcmFieldQualifyIp4Ttl]                      = "Qualify Upon IPv4 TTL.",
    [bcmFieldQualifyAceEntryId]                  = "Qualify Upon Ace Entry ID.",
    [bcmFieldQualifyNetworkQos]                  = "Qualify Upon Network Qos.",
    [bcmFieldQualifyPtch]                        = "Opaque attribute field of the Injected packets. Part of the PTCH_2 header",
    [bcmFieldQualifyAppType]                     = "Packet Application type used by the ACL lookup. Can be either predefined (bcm_field_AppType_t) or user defined.",
    [bcmFieldQualifyInVportClass]                = "Qualify on the profile of an in LIF encoded as Gport",
    [bcmFieldQualifyVPortRangeCheck]             = "Virtual Port Range Check Results",
    [bcmFieldQualifyForwardingType]              = "This qualifier should be used together with input_arg, to indicate which layer number relative to forwarding layer to qualify upon,possible layer types are: bcmFieldLayerTypeXXX",
    [bcmFieldQualifyForwardingLayerQualifier]    = "This qualifier should be used together with input_arg, to indicate which layer number relative to forwarding layer to qualify upon, qualifies Layer Qualifiers (look for layer record qualifiers in UM)",
    [bcmFieldQualifyCascadedKeyValue]            = "Value of key cascaded from prior group in cascade",
    [bcmFieldQualifyContextId]                   = "Qualifies on the Context Id.",
    [bcmFieldQualifyForwardingProfile]           = "Qualifies on the Forwarding Profile.",
    [bcmFieldQualifyDstGport]                    = "Qualifies packet according to destination encoded as trap code.",
    [bcmFieldQualifyExtensionHeaderType]         = "Qualifies on Next Header Field in First Extension Header.",
    [bcmFieldQualifyInVPort0Raw]                    = "Qualifies on In LIF 0 input expects raw value.",
    [bcmFieldQualifyInVPort1Raw]                    = "Qualifies on In LIF 1 input expects raw value.",
    [bcmFieldQualifyOutVPort0Raw]                   = "Qualifies on Out LIF 0 input expects raw value.",
    [bcmFieldQualifyOutVPort1Raw]                   = "Qualifies on Out LIF 1 input expects raw value.",
    [bcmFieldQualifyOutVPort2Raw]                   = "Qualifies on Out LIF 2 input expects raw value.",
    [bcmFieldQualifyOutVPort3Raw]                   = "Qualifies on Out LIF 3 input expects raw value.",
    [bcmFieldQualifyAppTypePredefined]              = "The predefined AppType (bcm_field_AppType_t) used by the forwarding lookup.",
    [bcmFieldQualifyDstSysPortExt]              = "Qualifes upon TM Destination extension header",
    [bcmFieldQualifyDstSysPortExtPresent]       = "Qualifes upon if TM Destination extension header is present",
    [bcmFieldQualifyMirrorCode]              = "Matches on Mirror Code.",
    [bcmFieldQualifyMirrorData]              = "Matches on Mirror Data (qualifier).",
    [bcmFieldQualifyLearnData]                   = "Qualifies on data field in the Learn Info",
    [bcmFieldQualifyLearnSrcMac]                 = "Qualifies on learn source MAC",
    [bcmFieldQualifyLearnVsi]                    = "Qualifies on learn VSI",
    [bcmFieldQualifyLearnVlan]                   = "Qualifies on learn VLAN",
    [bcmFieldQualifyLearnStationMove]            = "Qualifies on learn Station move",
    [bcmFieldQualifyLearnMatch]                  = "Qualifies on learn Match",
    [bcmFieldQualifyLearnFound]                  = "Qualifies on learn Found",
    [bcmFieldQualifyLearnExpectedWon]            = "Qualifies on learn Expected Won",
    [bcmFieldQualifyVrfValue]                    = "Qualifies on the VRF part of forwarding data, without checking that the type is VRF.",
    [bcmFieldQualifySaLookupAcceptedStrength]    = "Qualifies on Accepted strength LSB returned from SA Lookup result. Qualifier is only relevant for bridged packets.",
    [bcmFieldQualifyLayerRecordTypeRaw]         = "Qualifies on the Layer Record Type. For possible values look at bcm_field_layer_type_t, input expects raw value.",
    [bcmFieldQualifySrcPortRaw]                 = "Qualifies on source port, input expects raw value.",
    [bcmFieldQualifyInPortWithoutCoreRaw]       = "Qualifies on Ingress Port. Does not include the core ID, caution should be taken when not adding entry by core, input expects raw value.",
    [bcmFieldQualifyVlanFormatRaw]              = "Qualifies on Incoming vlan structure format (untagged/single/double/priority-tagged), input expects raw value.",
    [bcmFieldQualifyInPortRaw]                  = "Qualifies on Ingress Port. Includes core ID, and can be used by entries for all cores, input expects raw value.",
    [bcmFieldQualifyColorRaw]                   = "Qualifies on packet color, input expects raw value.",
    [bcmFieldQualifyDstPortRaw]                 = "Qualifies on destination port, input expects raw value.",
    [bcmFieldQualifyOutPortRaw]                 = "Qualifies on Egress port. Does not include the core ID, caution should be taken when not adding entry by core, input expects raw value.",
    [bcmFieldQualifyPacketRes]                  = "When TRUE packet lookup result was Unknown Destination",
    [bcmFieldQualifyDstRpfValid]                = "Checks if the RPF Destination is valid.",
    [bcmFieldQualifyRpfOutVPort]                = "Qualifies on the RPF OUT LIF. Value should be GPORT with sub-type LIF.",
    [bcmFieldQualifyRpfOutInterface]            = "Qualifies on the RPF OUT LIF. Value should be L3 interface.",
    [bcmFieldQualifyRpfOutVPortRaw]               = "Qualifies on the RPF OUT LIF.",
    [bcmFieldQualifyRpfRouteValid]              = "Qualifies on the RPF Route valid.",
    [bcmFieldQualifyEcmpGroup]                  = "Qualifies on the ECMP group.",
    [bcmFieldQualifyIp4DstMulticast]            = "Qualify on IPv4 packets which their DIP is Multicast.",
    [bcmFieldQualifyOamInLifIdValid]            = "Qualifies if the LIF that has been found is an OAM LIF.",
    [bcmFieldQualifyOamInLifId]                 = "Qualifies on the last OAM LIF that has been found.",
    [bcmFieldQualifyLearnExtensionPresent]      = "Qualifies on whether PPH learn extension is present.",
    [bcmFieldQualifyMcastPkt]                   = "All ingress replicas will have this values set to 1.",
    [bcmFieldQualifyAppTypeRaw]                 = "Qualifies on packet Application type used by the ACL lookup. Can be either predefined or user defined. Input expects raw value.",
    [bcmFieldQualifyAppTypePredefinedRaw]       = "Qualifies on the predefined AppType used by the forwarding lookup. Input expects raw value.",
    [bcmFieldQualifyForwardingTypeRaw]          = "Same qualifier as bcmFieldQualifyForwardingType just doesnt have mapping function, i.e.input expects raw value (HW value).",
    [bcmFieldQualifyContextIdRaw]               = "Qualifies on the Forwarding Context value. Input expects raw value. Relevant for egress stage only.",
    [bcmFieldQualifyFtmhAsePresent]             = "Qualify packet on FTMH ASE present field.",
    [bcmFieldQualifyFtmhAseType]                = "Qualify packet on FTMH ASE type value.",
    [bcmFieldQualifyVipValid]                   = "Qualifies on SLLB Virtual Wire VW_VIP_VALID.",
    [bcmFieldQualifyVipId]                      = "Qualifies on SLLB Virtual Wire VW_VIP_ID.",
    [bcmFieldQualifyVIPMemberReference]         = "Qualifies on SLLB Virtual Wire VW_MEMBER_REFERENCE.",
    [bcmFieldQualifyPccHit]                     = "Qualifies on SLLB Virtual Wire VW_PCC_HIT.",
    [bcmFieldQualifyL2Format]                   = "Qualify on the type of the frame (see bcmFieldL2FormatXXX)",
    [bcmFieldQualifyRxTrapProfile]              = "Qualifies on Trap profile.",
    [bcmFieldQualifyAcInLifWideDataExtended]    = "Qualify on the Ac in-lif wide(generic) extended data: value[0]=32 LSB of the data value[1]=26b MSB of the data value[2]=hit_bit",
    [bcmFieldQualifyAcInLifWideDataExtendedRaw] = "Qualify on the Ac in-lif wide(generic) extended data. Input expects raw value.",
    [bcmFieldQualifyDstClassL2]                 = "Qualify on the MACT entry_group.",
    [bcmFieldQualifyIPv6FragmentedNonFirst]     = "Qualify on IPv6 packets where the IP Header is both fragmented and also not the first fragment.",
    [bcmFieldQualifyIPv4FragmentedNonFirst]     = "Qualify on IPv4 packets where the IP Header is both fragmented and also not the first fragment.",
    [bcmFieldQualifyIpMcFallbacktoBridge]       = "Qualify on IP MC packets that fallback2bridge.",
    [bcmFieldQualifyRchTimeStamp]               = "Qualify on RCH timestamp.",
    [bcmFieldQualifyRchForwardingAdditioalInfo] = "Qualify on the RCH forwarding additional info.",
    [bcmFieldQualifyRchType]                    = "Qualify on the type of RCH flow.",
    [bcmFieldQualifyRchVisibility]              = "Qualify on whether visibility for RCH is set.",
    [bcmFieldQualifyDstType]                    = "Qualify on 8MSB of destination, which indicate destination type. The endocding can be found in Arch guide (Destination Port Encoding).",

};

static const dnx_field_qual_map_t dnx_global_qual_map[bcmFieldQualifyCount] = {
        /*
         * SW qualifiers, same for all stages. None currently in use.
         */
        /*
         * {
         * Header qualifiers, stage will be inserted by mapping routine, class is always HEADER
         */
        [bcmFieldQualifyDstMac]                = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_MAC_DST),             },
        [bcmFieldQualifySrcMac]                = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_MAC_SRC),             },
        [bcmFieldQualifyEtherTypeUntagged]     = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_ETHERTYPE),           },

        [bcmFieldQualifyTpid]                  = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_VLAN_TPID),           },
        [bcmFieldQualifyVlanId]                = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_VLAN_ID),             },
        [bcmFieldQualifyVlanPri]               = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_VLAN_PRI),            },
        [bcmFieldQualifyVlanCfi]               = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_VLAN_CFI),            },
        [bcmFieldQualifyVlanPriCfi]            = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_VLAN_PRI_CFI),        },

        [bcmFieldQualifyArpSenderIp4]          = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_ARP_SENDER_IP),       },
        [bcmFieldQualifyArpTargetIp4]          = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_ARP_TARGET_IP),       },
        [bcmFieldQualifyArpOpcode]             = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_ARP_OPCODE),          },

        [bcmFieldQualifyMplsLabel]             = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_MPLS_LABEL),          },
        [bcmFieldQualifyMplsLabelId]           = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_MPLS_LABEL_ID),       },
        [bcmFieldQualifyMplsLabelTtl]          = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_MPLS_TTL),            },
        [bcmFieldQualifyMplsLabelBos]          = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_MPLS_BOS),            },
        [bcmFieldQualifyMplsLabelExp]          = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_MPLS_TC),             },

        [bcmFieldQualifySrcIp]                 = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_IPV4_SRC),            },
        [bcmFieldQualifyDstIp]                 = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_IPV4_DST),            },
        [bcmFieldQualifyIp4Tos]                = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_IPV4_TOS),            },
        [bcmFieldQualifyIp4Ttl]                = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_IPV4_TTL),            },
        [bcmFieldQualifyIp4Protocol]           = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_IPV4_PROTOCOL),       },
        [bcmFieldQualifyIpFlags]               = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_IPV4_FLAGS),          },

        [bcmFieldQualifySrcIp6]                = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_IPV6_SIP),            },
        [bcmFieldQualifyDstIp6]                = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_IPV6_DIP),            },
        [bcmFieldQualifySrcIp6High]            = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_IPV6_SIP_HIGH),       },
        [bcmFieldQualifyDstIp6High]            = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_IPV6_DIP_HIGH),       },
        [bcmFieldQualifySrcIp6Low]             = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_IPV6_SIP_LOW),        },
        [bcmFieldQualifyDstIp6Low]             = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_IPV6_DIP_LOW),        },
        [bcmFieldQualifyIp6FlowLabel]          = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_IPV6_FLOW_LABEL),     },
        [bcmFieldQualifyIp6NextHeader]       = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_IPV6_NEXT_HEADER),    },
        [bcmFieldQualifyIp6HopLimit]           = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_IPV6_HOP_LIMIT),      },
        [bcmFieldQualifyIp6TrafficClass]       = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_IPV6_TC),             },
        [bcmFieldQualifyExtensionHeaderType]   = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_IPV6_1ST_EXTENSION_NEXT_HEADER),  },

        [bcmFieldQualifyL4DstPort]             = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_L4_DST_PORT),         },
        [bcmFieldQualifyL4SrcPort]             = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_L4_SRC_PORT),         },
        [bcmFieldQualifyTcpControl]            = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_TCP_CTL),             },

        [bcmFieldQualifyRchTimeStamp]          = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_RCH_TIME_STAMP),      },
        [bcmFieldQualifyRchForwardingAdditioalInfo]  = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_RCH_FORWARDING_ADDITIONAL_INFO),       },
        [bcmFieldQualifyRchType]               = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_RCH_TYPE)             },
        [bcmFieldQualifyRchVisibility]         = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, 0, DNX_FIELD_HEADER_QUAL_RCH_VISIBILITY)       },
        /*
         * }
         */
        /*
         * Layer Record Based BCM Qualifiers
         * Stage not relevan since its global and relevant for all stages
         * {
         */
        [bcmFieldQualifyIpFrag]                  = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, 0, DNX_FIELD_LR_QUAL_IPV4_IS_FRAGMENTED),},
        [bcmFieldQualifyLayerRecordTypeRaw]      = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, 0, DNX_FIELD_LR_QUAL_PROTOCOL), },
        [bcmFieldQualifyLayerRecordType]         = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, 0, DNX_FIELD_LR_QUAL_PROTOCOL), BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_forwarding_type},
        [bcmFieldQualifyLayerRecordOffset]       = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, 0, DNX_FIELD_LR_QUAL_OFFSET),},
        [bcmFieldQualifyLayerRecordQualifier]    = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, 0, DNX_FIELD_LR_QUAL_QUALIFIER),},
        [bcmFieldQualifyEthernetMulticast]       = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, 0, DNX_FIELD_LR_QUAL_ETH_IS_DA_MC),},
        [bcmFieldQualifyEthernetBroadcast]       = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, 0, DNX_FIELD_LR_QUAL_ETH_IS_DA_BC),},
        [bcmFieldQualifyEthernetFirstTpidExist]  = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, 0, DNX_FIELD_LR_QUAL_ETH_1ST_TPID_EXIST),},
        [bcmFieldQualifyEthernetFirstTpidIndex]  = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, 0, DNX_FIELD_LR_QUAL_ETH_1ST_TPID_INDEX),},
        [bcmFieldQualifyEthernetSecondTpidExist] = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, 0, DNX_FIELD_LR_QUAL_ETH_2ND_TPID_EXIST),},
        [bcmFieldQualifyEthernetSecondTpidIndex] = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, 0, DNX_FIELD_LR_QUAL_ETH_2ND_TPID_INDEX),},
        [bcmFieldQualifyEthernetThirdTpidExist]  = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, 0, DNX_FIELD_LR_QUAL_ETH_3RD_TPID_EXIST),},
        [bcmFieldQualifyEthernetThirdTpidIndex]  = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, 0, DNX_FIELD_LR_QUAL_ETH_3RD_TPID_INDEX),},
        [bcmFieldQualifyL2Format]                = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, 0, DNX_FIELD_LR_QUAL_ETH_FRAME_TYPE), 0, dnx_field_convert_l2_format},
        [bcmFieldQualifyIp4DstMulticast]         = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, 0, DNX_FIELD_LR_QUAL_IPV4_IS_MC),},
        [bcmFieldQualifyIpHasOptions]            = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, 0, DNX_FIELD_LR_QUAL_IPV4_HAS_OPTION),},
        [bcmFieldQualifyIpFirstFrag]             = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, 0, DNX_FIELD_LR_QUAL_IPV4_IS_1ST_FRAGMENT),},
        [bcmFieldQualifyIpTunnelTypeRaw]         = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, 0, DNX_FIELD_LR_QUAL_IPV4_TUNNEL_TYPE), },
        [bcmFieldQualifyIpTunnelType]            = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, 0, DNX_FIELD_LR_QUAL_IPV4_TUNNEL_TYPE),BCM_TO_DNX_ADDITIONAL_OBJ , dnx_field_convert_tunnel_type},
        [bcmFieldQualifyIPv4FragmentedNonFirst]  = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, 0, DNX_FIELD_LR_QUAL_IPV4_FRAGMENTED_NON_FIRST),},
        [bcmFieldQualifyIp6MulticastCompatible]  = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, 0, DNX_FIELD_LR_QUAL_IPV6_IS_MC),},
        [bcmFieldQualifyIp6FirstAdditionalHeaderExist]  = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, 0, DNX_FIELD_LR_QUAL_IPV6_1ST_ADD_HEADER_EXIST),},
        [bcmFieldQualifyIp6FirstAdditionalHeader]  = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, 0, DNX_FIELD_LR_QUAL_IPV6_1ST_ADD_HEADER), 0, dnx_field_convert_ip6_additional_header},
        [bcmFieldQualifyIp6SecondAdditionalHeaderExist] = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, 0, DNX_FIELD_LR_QUAL_IPV6_2ND_ADD_HEADER_EXISTS),},
        [bcmFieldQualifyIp6SecondAdditionalHeader] = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, 0, DNX_FIELD_LR_QUAL_IPV6_2ND_ADD_HEADER),0 , dnx_field_convert_ip6_additional_header},
        [bcmFieldQualifyIPv6FragmentedNonFirst]  = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, 0, DNX_FIELD_LR_QUAL_IPV6_FRAGMENTED_NON_FIRST),},
        [bcmFieldQualifyItmhPphType]             = {DNX_QUAL(DNX_FIELD_QUAL_CLASS_LAYER_RECORD, 0, DNX_FIELD_LR_QUAL_ITMH_PPH_TYPE),},

        /*
         * }
         */
};

/*
 * BCM to DNX set maps per stage
 */
/*
 * End of BCM to DNX set maps per stage
 * {
 */
static const dnx_field_qual_map_t dnx_external_meta_qual_map[bcmFieldQualifyCount] = {

        [bcmFieldQualifyGeneratedTtl]                = {DNX_FIELD_QUAL_IN_TTL,                         },
        [bcmFieldQualifyIpMulticastCompatible]       = {DNX_FIELD_QUAL_PACKET_IS_COMPATIBLE_MC,        },
        [bcmFieldQualifyColorRaw]                    = {DNX_FIELD_QUAL_DP,                             },
        [bcmFieldQualifyColor]                       = {DNX_FIELD_QUAL_DP, BCM_TO_DNX_ADDITIONAL_OBJ , dnx_field_convert_color },
        [bcmFieldQualifyPacketIsIEEE1588]            = {DNX_FIELD_QUAL_PACKET_IS_IEEE1588,             },
        [bcmFieldQualifyIEEE1588Encapsulation]       = {DNX_FIELD_QUAL_IEEE1588_ENCAPSULATION,         },
        [bcmFieldQualifyIEEE1588CompensateTimeStamp] = {DNX_FIELD_QUAL_IEEE1588_COMPENSATE_TIME_STAMP, },
        [bcmFieldQualifyIEEE1588Command]             = {DNX_FIELD_QUAL_IEEE1588_COMMAND,               },
        [bcmFieldQualifyIEEE1588HeaderOffset]        = {DNX_FIELD_QUAL_IEEE1588_HEADER_OFFSET,         },
        [bcmFieldQualifyRxTrapCodeForSnoop]          = {DNX_FIELD_QUAL_SNOOP_CODE,                     },
        [bcmFieldQualifyRxTrapCode]                  = {DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_CODE,       },
        [bcmFieldQualifyRxTrapData]                  = {DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_QUAL,       },
        [bcmFieldQualifyRxTrapStrength]              = {DNX_FIELD_QUAL_FWD_ACTION_STRENGTH,            },
        [bcmFieldQualifyDstRpfGport]                 = {DNX_FIELD_QUAL_RPF_DST,                        },
        [bcmFieldQualifyDstRpfValid]                 = {DNX_FIELD_QUAL_RPF_DST_VALID,},
        [bcmFieldQualifyRpfRouteValid]               = {DNX_FIELD_QUAL_RPF_ROUTE_VALID,},
        [bcmFieldQualifySrcPortRaw]                  = {DNX_FIELD_QUAL_SRC_SYS_PORT, },
        [bcmFieldQualifySrcPort]                     = {DNX_FIELD_QUAL_SRC_SYS_PORT, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_sys_port_gport_to_port },
        [bcmFieldQualifySrcModPortGport]             = {DNX_FIELD_QUAL_SRC_SYS_PORT,BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_sys_port_gport_to_port},
        [bcmFieldQualifyDstPort]                     = {DNX_FIELD_QUAL_FWD_ACTION_DESTINATION, },
        [bcmFieldQualifyDstMulticastGroup]           = {DNX_FIELD_QUAL_FWD_ACTION_DESTINATION,BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_dst},
        [bcmFieldQualifyOamStampOffset]              = {DNX_FIELD_QUAL_OAM_STAMP_OFFSET,               },
        [bcmFieldQualifyOamHeaderOffset]             = {DNX_FIELD_QUAL_OAM_OFFSET,                     },
        [bcmFieldQualifyPacketSize]                  = {DNX_FIELD_QUAL_PACKET_HEADER_SIZE,             },
        [bcmFieldQualifyInVPort0Raw]                 = {DNX_FIELD_QUAL_GLOB_IN_LIF_0, },
        [bcmFieldQualifyInVPort1Raw]                 = {DNX_FIELD_QUAL_GLOB_IN_LIF_1, },
        [bcmFieldQualifyOutVPort0Raw]                = {DNX_FIELD_QUAL_GLOB_OUT_LIF_0, },
        [bcmFieldQualifyOutVPort1Raw]                = {DNX_FIELD_QUAL_GLOB_OUT_LIF_1, },
        [bcmFieldQualifyInVPort0]                    = {DNX_FIELD_QUAL_GLOB_IN_LIF_0, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_in_lif},
        [bcmFieldQualifyInVPort1]                    = {DNX_FIELD_QUAL_GLOB_IN_LIF_1,BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_in_lif},
        [bcmFieldQualifyOutVPort0]                   = {DNX_FIELD_QUAL_GLOB_OUT_LIF_0,BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_out_lif},
        [bcmFieldQualifyOutVPort1]                   = {DNX_FIELD_QUAL_GLOB_OUT_LIF_1,BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_out_lif},
        [bcmFieldQualifyInInterface]                 = {DNX_FIELD_QUAL_GLOB_IN_LIF_0,BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_rif_intf_to_lif},
        [bcmFieldQualifyOutInterface]                = {DNX_FIELD_QUAL_GLOB_OUT_LIF_0,BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_rif_intf_to_lif},
        [bcmFieldQualifyTrunkHashResult]             = {DNX_FIELD_QUAL_LAG_LB_KEY,                     },
        [bcmFieldQualifyIntPriority]                 = {DNX_FIELD_QUAL_TC,                             },
        [bcmFieldQualifyInVportClass0]               = {DNX_FIELD_QUAL_IN_LIF_PROFILE_0,               },
        [bcmFieldQualifyInVportClass1]               = {DNX_FIELD_QUAL_IN_LIF_PROFILE_1,               },
        [bcmFieldQualifyInPortWithoutCoreRaw]           = {DNX_FIELD_QUAL_PP_PORT, },
        [bcmFieldQualifyInPortWithoutCore]           = {DNX_FIELD_QUAL_PP_PORT, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_port_without_core},
        [bcmFieldQualifyVlanFormatRaw]                  = {DNX_FIELD_QUAL_INCOMING_TAG_STRUCTURE, },
        [bcmFieldQualifyVlanFormat]                  = {DNX_FIELD_QUAL_INCOMING_TAG_STRUCTURE,BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_vlan_format},
        [bcmFieldQualifyL2Learn]                     = {DNX_FIELD_QUAL_INGRESS_LEARN_ENABLE,           },
        [bcmFieldQualifyMplsForwardingLabelAction]   = {DNX_FIELD_QUAL_EEI,     },
        [bcmFieldQualifyTrillEgressRbridge]          = {DNX_FIELD_QUAL_EEI,BCM_TO_DNX_ADDITIONAL_OBJ},
        [bcmFieldQualifyISid]                        = {DNX_FIELD_QUAL_EEI,BCM_TO_DNX_ADDITIONAL_OBJ},
        [bcmFieldQualifyVrf]                         = {DNX_FIELD_QUAL_FWD_DOMAIN,                     },
        [bcmFieldQualifyInterfaceClassL2]            = {DNX_FIELD_QUAL_PMF_ETH_RIF_PROFILE,  },
        [bcmFieldQualifyIncomingIpIfClass]           = {DNX_FIELD_QUAL_PMF_ETH_RIF_PROFILE, BCM_TO_DNX_ADDITIONAL_OBJ },
        [bcmFieldQualifyAcInLifWideData]             = {DNX_FIELD_QUAL_AC_IN_LIF_WIDE_DATA,            },
        [bcmFieldQualifyTracePacket]                 = {DNX_FIELD_QUAL_TRACE_PACKET,                   },
        [bcmFieldQualifyForwardingAdditionalInfo]    = {DNX_FIELD_QUAL_FWD_LAYER_ADDITIONAL_INFO,                   },
        [bcmFieldQualifyStatId0]                 = {DNX_FIELD_QUAL_FLP_STATISTICS_ID_0,    },
        [bcmFieldQualifyStatId1]                 = {DNX_FIELD_QUAL_FLP_STATISTICS_ID_1,    },
        [bcmFieldQualifyStatId2]                 = {DNX_FIELD_QUAL_FLP_STATISTICS_ID_2,    },
        [bcmFieldQualifyStatId3]                 = {DNX_FIELD_QUAL_FLP_STATISTICS_ID_3,    },
        [bcmFieldQualifyStatId4]                 = {DNX_FIELD_QUAL_FLP_STATISTICS_ID_4,    },
        [bcmFieldQualifyStatId5]                 = {DNX_FIELD_QUAL_FLP_STATISTICS_ID_5,    },
        [bcmFieldQualifyStatProfile0]            = {DNX_FIELD_QUAL_FLP_STATISTICS_ATR_0,   },
        [bcmFieldQualifyStatProfile1]            = {DNX_FIELD_QUAL_FLP_STATISTICS_ATR_1,   },
        [bcmFieldQualifyStatProfile2]            = {DNX_FIELD_QUAL_FLP_STATISTICS_ATR_2,   },
        [bcmFieldQualifyStatProfile3]            = {DNX_FIELD_QUAL_FLP_STATISTICS_ATR_3,   },
        [bcmFieldQualifyStatProfile4]            = {DNX_FIELD_QUAL_FLP_STATISTICS_ATR_4,   },
        [bcmFieldQualifyStatProfile5]            = {DNX_FIELD_QUAL_FLP_STATISTICS_ATR_5,   },
        [bcmFieldQualifyStatMetaData]            = {DNX_FIELD_QUAL_STATISTICS_META_DATA,   },
        [bcmFieldQualifyNetworkQos]              = {DNX_FIELD_QUAL_NWK_QOS, },
        [bcmFieldQualifyMirrorCode]                   = {DNX_FIELD_QUAL_MIRROR_CODE,},
        [bcmFieldQualifyVrfValue]                     = {DNX_FIELD_QUAL_VRF_VALUE,},
        [bcmFieldQualifySaLookupAcceptedStrength]     = {DNX_FIELD_QUAL_RPF_DEFAULT_ROUTE_FOUND,},
        [bcmFieldQualifyRpfOutVPortRaw]              = {DNX_FIELD_QUAL_RPF_OUT_LIF, },
        [bcmFieldQualifyRpfOutVPort]                 = {DNX_FIELD_QUAL_RPF_OUT_LIF,BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_out_lif},
        [bcmFieldQualifyRpfOutInterface]             = {DNX_FIELD_QUAL_RPF_OUT_LIF,BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_rif_intf_to_lif},
        [bcmFieldQualifyPacketRes]                   = {DNX_FIELD_QUAL_UNKNOWN_ADDRESS, },
        [bcmFieldQualifyOamInLifIdValid]             = {DNX_FIELD_QUAL_VTT_OAM_LIF_0_VALID,},
        [bcmFieldQualifyOamInLifId]                  = {DNX_FIELD_QUAL_VTT_OAM_LIF_0,},
        [bcmFieldQualifyEcnValue]                    = {DNX_FIELD_QUAL_ECN,},
        [bcmFieldQualifyVipValid]                    = {DNX_FIELD_QUAL_VW_VIP_VALID,},
        [bcmFieldQualifyVipId]                       = {DNX_FIELD_QUAL_VW_VIP_ID,},
        [bcmFieldQualifyVIPMemberReference]          = {DNX_FIELD_QUAL_VW_MEMBER_REFERENCE,},
        [bcmFieldQualifyPccHit]                      = {DNX_FIELD_QUAL_VW_PCC_HIT,},
        [bcmFieldQualifyAcInLifWideDataExtendedRaw]  = {DNX_FIELD_QUAL_AC_IN_LIF_WIDE_DATA_EXTENDED, },
        [bcmFieldQualifyAcInLifWideDataExtended]     = {DNX_FIELD_QUAL_AC_IN_LIF_WIDE_DATA_EXTENDED,BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_ac_in_lif_wide_data_extended},
        [bcmFieldQualifyDstClassL2]                  = {DNX_FIELD_QUAL_MACT_ENTRY_GROUPING, },
};

static const dnx_field_qual_map_t dnx_ipmf1_meta_qual_map[bcmFieldQualifyCount] = {

        [bcmFieldQualifyL4PortRangeCheck]            = {DNX_FIELD_QUAL_L4OPS,                          },
        [bcmFieldQualifyRangeFirstHit0]              = {DNX_FIELD_QUAL_ENCODER0,                 },
        [bcmFieldQualifyRangeFirstHit1]              = {DNX_FIELD_QUAL_ENCODER1,                 },
        [bcmFieldQualifyRangeFirstHit2]              = {DNX_FIELD_QUAL_ENCODER2,                 },
        [bcmFieldQualifyRangeFirstHit3]              = {DNX_FIELD_QUAL_ENCODER3,                 },
        [bcmFieldQualifyGeneratedTtl]                = {DNX_FIELD_QUAL_IN_TTL,                         },
        [bcmFieldQualifyIpMulticastCompatible]       = {DNX_FIELD_QUAL_PACKET_IS_COMPATIBLE_MC,        },
        [bcmFieldQualifyColorRaw]                    = {DNX_FIELD_QUAL_DP, },
        [bcmFieldQualifyColor]                       = {DNX_FIELD_QUAL_DP, BCM_TO_DNX_ADDITIONAL_OBJ , dnx_field_convert_color },
        [bcmFieldQualifyPacketIsIEEE1588]            = {DNX_FIELD_QUAL_PACKET_IS_IEEE1588,             },
        [bcmFieldQualifyIEEE1588Encapsulation]       = {DNX_FIELD_QUAL_IEEE1588_ENCAPSULATION,         },
        [bcmFieldQualifyIEEE1588CompensateTimeStamp] = {DNX_FIELD_QUAL_IEEE1588_COMPENSATE_TIME_STAMP, },
        [bcmFieldQualifyIEEE1588Command]             = {DNX_FIELD_QUAL_IEEE1588_COMMAND,               },
        [bcmFieldQualifyIEEE1588HeaderOffset]        = {DNX_FIELD_QUAL_IEEE1588_HEADER_OFFSET,         },
        [bcmFieldQualifyRxTrapCodeForSnoop]          = {DNX_FIELD_QUAL_SNOOP_CODE,                     },
        [bcmFieldQualifyRxTrapCode]                  = {DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_CODE,       },
        [bcmFieldQualifyRxTrapData]                  = {DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_QUAL,       },
        [bcmFieldQualifyRxTrapStrength]              = {DNX_FIELD_QUAL_FWD_ACTION_STRENGTH,            },
        [bcmFieldQualifyDstRpfGport]                 = {DNX_FIELD_QUAL_RPF_DST,                        },
        [bcmFieldQualifyDstRpfValid]                 = {DNX_FIELD_QUAL_RPF_DST_VALID,},
        [bcmFieldQualifyRpfRouteValid]               = {DNX_FIELD_QUAL_RPF_ROUTE_VALID,},
        [bcmFieldQualifyKeyGenVar]                   = {DNX_FIELD_QUAL_CONTEXT_KEY_GEN_VAR,            },
        [bcmFieldQualifySrcPortRaw]                  = {DNX_FIELD_QUAL_SRC_SYS_PORT, },
        [bcmFieldQualifySrcPort]                     = {DNX_FIELD_QUAL_SRC_SYS_PORT,BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_sys_port_gport_to_port},
        [bcmFieldQualifySrcModPortGport]             = {DNX_FIELD_QUAL_SRC_SYS_PORT,BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_sys_port_gport_to_port},
        [bcmFieldQualifyDstPort]                     = {DNX_FIELD_QUAL_FWD_ACTION_DESTINATION, },
        [bcmFieldQualifyDstMulticastGroup]           = {DNX_FIELD_QUAL_FWD_ACTION_DESTINATION,BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_dst},
        [bcmFieldQualifyOamUpMep]                    = {DNX_FIELD_QUAL_OAM_UP_MEP,                     },
        [bcmFieldQualifyOamSubtype]                  = {DNX_FIELD_QUAL_OAM_SUB_TYPE,                   },
        [bcmFieldQualifyOamStampOffset]              = {DNX_FIELD_QUAL_OAM_STAMP_OFFSET,               },
        [bcmFieldQualifyOamHeaderOffset]             = {DNX_FIELD_QUAL_OAM_OFFSET,                     },
        [bcmFieldQualifyOamMepId]                    = {DNX_FIELD_QUAL_OAM_ID,                         },
        [bcmFieldQualifyOamMeterDisable]             = {DNX_FIELD_QUAL_OAM_METER_DISABLE,              },
        [bcmFieldQualifyPacketSize]                  = {DNX_FIELD_QUAL_PACKET_HEADER_SIZE,             },
        [bcmFieldQualifyInVPort0Raw]                 = {DNX_FIELD_QUAL_GLOB_IN_LIF_0, },
        [bcmFieldQualifyInVPort1Raw]                 = {DNX_FIELD_QUAL_GLOB_IN_LIF_1, },
        [bcmFieldQualifyOutVPort0Raw]                = {DNX_FIELD_QUAL_GLOB_OUT_LIF_0, },
        [bcmFieldQualifyOutVPort1Raw]                = {DNX_FIELD_QUAL_GLOB_OUT_LIF_1, },
        [bcmFieldQualifyInVPort0]                    = {DNX_FIELD_QUAL_GLOB_IN_LIF_0, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_in_lif},
        [bcmFieldQualifyInVPort1]                    = {DNX_FIELD_QUAL_GLOB_IN_LIF_1, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_in_lif},
        [bcmFieldQualifyOutVPort0]                   = {DNX_FIELD_QUAL_GLOB_OUT_LIF_0, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_out_lif},
        [bcmFieldQualifyOutVPort1]                   = {DNX_FIELD_QUAL_GLOB_OUT_LIF_1, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_out_lif},
        [bcmFieldQualifyInInterface]                 = {DNX_FIELD_QUAL_GLOB_IN_LIF_0, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_rif_intf_to_lif},
        [bcmFieldQualifyOutInterface]                = {DNX_FIELD_QUAL_GLOB_OUT_LIF_0, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_rif_intf_to_lif},
        [bcmFieldQualifyTrunkHashResult]             = {DNX_FIELD_QUAL_LAG_LB_KEY,                     },
        [bcmFieldQualifyIntPriority]                 = {DNX_FIELD_QUAL_TC,                             },
        [bcmFieldQualifyExternalHit0]                = {DNX_FIELD_QUAL_ELK_LKP_HIT_0,                  },
        [bcmFieldQualifyExternalHit1]                = {DNX_FIELD_QUAL_ELK_LKP_HIT_1,                  },
        [bcmFieldQualifyExternalHit2]                = {DNX_FIELD_QUAL_ELK_LKP_HIT_2,                  },
        [bcmFieldQualifyExternalHit3]                = {DNX_FIELD_QUAL_ELK_LKP_HIT_3,                  },
        [bcmFieldQualifyExternalHit4]                = {DNX_FIELD_QUAL_ELK_LKP_HIT_4,                  },
        [bcmFieldQualifyExternalHit5]                = {DNX_FIELD_QUAL_ELK_LKP_HIT_5,                  },
        [bcmFieldQualifyExternalHit6]                = {DNX_FIELD_QUAL_ELK_LKP_HIT_6,                  },
        [bcmFieldQualifyExternalHit7]                = {DNX_FIELD_QUAL_ELK_LKP_HIT_7,                  },
        [bcmFieldQualifyInVportClass0]               = {DNX_FIELD_QUAL_IN_LIF_PROFILE_0,               },
        [bcmFieldQualifyInVportClass1]               = {DNX_FIELD_QUAL_IN_LIF_PROFILE_1,               },
        [bcmFieldQualifyTranslatedOuterVlanId ]      = {DNX_FIELD_QUAL_VLAN_EDIT_VID_1,                },
        [bcmFieldQualifyTranslatedOuterVlanPri]      = {DNX_FIELD_QUAL_VLAN_EDIT_PCP_1,                },
        [bcmFieldQualifyTranslatedOuterVlanCfi]      = {DNX_FIELD_QUAL_VLAN_EDIT_DEI_1,                },
        [bcmFieldQualifyTranslatedInnerVlanId ]      = {DNX_FIELD_QUAL_VLAN_EDIT_VID_2,                },
        [bcmFieldQualifyTranslatedInnerVlanPri]      = {DNX_FIELD_QUAL_VLAN_EDIT_PCP_2,                },
        [bcmFieldQualifyTranslatedInnerVlanCfi]      = {DNX_FIELD_QUAL_VLAN_EDIT_DEI_2,                },
        [bcmFieldQualifyVlanAction]                  = {DNX_FIELD_QUAL_VLAN_EDIT_CMD_INDEX,            },
        [bcmFieldQualifyInPortRaw]                   = {DNX_FIELD_QUAL_IN_PORT, },
        [bcmFieldQualifyInPortWithoutCoreRaw]        = {DNX_FIELD_QUAL_PP_PORT, },
        [bcmFieldQualifyInPort]                      = {DNX_FIELD_QUAL_IN_PORT, BCM_TO_DNX_ADDITIONAL_OBJ , dnx_field_convert_gport_to_port_with_core},
        [bcmFieldQualifyInPortWithoutCore]           = {DNX_FIELD_QUAL_PP_PORT, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_port_without_core},
        [bcmFieldQualifyPacketLengthRangeCheck]      = {DNX_FIELD_QUAL_PACKET_HEADER_SIZE_RANGE,       },
        [bcmFieldQualifyVlanFormatRaw]               = {DNX_FIELD_QUAL_INCOMING_TAG_STRUCTURE, },
        [bcmFieldQualifyVlanFormat]                  = {DNX_FIELD_QUAL_INCOMING_TAG_STRUCTURE,  BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_vlan_format},
        [bcmFieldQualifyL2Learn]                     = {DNX_FIELD_QUAL_INGRESS_LEARN_ENABLE,           },
        [bcmFieldQualifyMplsForwardingLabelAction]   = {DNX_FIELD_QUAL_EEI,     },
        [bcmFieldQualifyTrillEgressRbridge]          = {DNX_FIELD_QUAL_EEI, BCM_TO_DNX_ADDITIONAL_OBJ},
        [bcmFieldQualifyISid]                        = {DNX_FIELD_QUAL_EEI, BCM_TO_DNX_ADDITIONAL_OBJ},
        [bcmFieldQualifyVrf]                         = {DNX_FIELD_QUAL_FWD_DOMAIN,},
        [bcmFieldQualifyVpn]                         = {DNX_FIELD_QUAL_FWD_DOMAIN,BCM_TO_DNX_ADDITIONAL_OBJ},
        [bcmFieldQualifyForwardingLayerIndex]        = {DNX_FIELD_QUAL_FWD_LAYER_INDEX,                },
        [bcmFieldQualifyInterfaceClassL2]            = {DNX_FIELD_QUAL_PMF_ETH_RIF_PROFILE,            },
        [bcmFieldQualifyIncomingIpIfClass]           = {DNX_FIELD_QUAL_PMF_ETH_RIF_PROFILE,BCM_TO_DNX_ADDITIONAL_OBJ},
        [bcmFieldQualifyAcInLifWideData]             = {DNX_FIELD_QUAL_AC_IN_LIF_WIDE_DATA,            },
        [bcmFieldQualifyNativeAcInLifWideData]       = {DNX_FIELD_QUAL_NATIVE_AC_IN_LIF_WIDE_DATA,     },
        [bcmFieldQualifyPweInLifWideData]            = {DNX_FIELD_QUAL_PWE_IN_LIF_WIDE_DATA,           },
        [bcmFieldQualifyIpTunnelInLifWideData]       = {DNX_FIELD_QUAL_IP_TUNNEL_IN_LIF_WIDE_DATA,     },
        [bcmFieldQualifyPortClassPacketProcessing]   = {DNX_FIELD_QUAL_PP_PORT_PMF_PROFILE,            },
        [bcmFieldQualifyPortClassTrafficManagement]  = {DNX_FIELD_QUAL_TM_PORT_PMF_PROFILE,            },
        [bcmFieldQualifyPortClassPacketProcessingGeneralData] = {DNX_FIELD_QUAL_PP_PORT_PMF_GENERAL_DATA, },
        [bcmFieldQualifyBierStringOffset]            = {DNX_FIELD_QUAL_BIER_STR_OFFSET,                },
        [bcmFieldQualifyBierStringSize]              = {DNX_FIELD_QUAL_BIER_STR_SIZE,                  },
        [bcmFieldQualifyPacketIsBier]                = {DNX_FIELD_QUAL_PACKET_IS_BIER,                 },
        [bcmFieldQualifyTracePacket]                 = {DNX_FIELD_QUAL_TRACE_PACKET,                   },
        [bcmFieldQualifyForwardingAdditionalInfo]    = {DNX_FIELD_QUAL_FWD_LAYER_ADDITIONAL_INFO,                   },
        [bcmFieldQualifyStatId0]                 = {DNX_FIELD_QUAL_STATISTICS_ID_0,        },
        [bcmFieldQualifyStatId1]                 = {DNX_FIELD_QUAL_STATISTICS_ID_1,        },
        [bcmFieldQualifyStatId2]                 = {DNX_FIELD_QUAL_STATISTICS_ID_2,        },
        [bcmFieldQualifyStatId3]                 = {DNX_FIELD_QUAL_STATISTICS_ID_3,        },
        [bcmFieldQualifyStatId4]                 = {DNX_FIELD_QUAL_STATISTICS_ID_4,        },
        [bcmFieldQualifyStatId5]                 = {DNX_FIELD_QUAL_STATISTICS_ID_5,        },
        [bcmFieldQualifyStatId6]                 = {DNX_FIELD_QUAL_STATISTICS_ID_6,        },
        [bcmFieldQualifyStatId7]                 = {DNX_FIELD_QUAL_STATISTICS_ID_7,        },
        [bcmFieldQualifyStatId8]                 = {DNX_FIELD_QUAL_STATISTICS_ID_8,        },
        [bcmFieldQualifyStatId9]                 = {DNX_FIELD_QUAL_STATISTICS_ID_9,        },
        [bcmFieldQualifyStatProfile0]            = {DNX_FIELD_QUAL_STATISTICS_ATR_0,       },
        [bcmFieldQualifyStatProfile1]            = {DNX_FIELD_QUAL_STATISTICS_ATR_1,       },
        [bcmFieldQualifyStatProfile2]            = {DNX_FIELD_QUAL_STATISTICS_ATR_2,       },
        [bcmFieldQualifyStatProfile3]            = {DNX_FIELD_QUAL_STATISTICS_ATR_3,       },
        [bcmFieldQualifyStatProfile4]            = {DNX_FIELD_QUAL_STATISTICS_ATR_4,       },
        [bcmFieldQualifyStatProfile5]            = {DNX_FIELD_QUAL_STATISTICS_ATR_5,       },
        [bcmFieldQualifyStatProfile6]            = {DNX_FIELD_QUAL_STATISTICS_ATR_6,       },
        [bcmFieldQualifyStatProfile7]            = {DNX_FIELD_QUAL_STATISTICS_ATR_7,       },
        [bcmFieldQualifyStatProfile8]            = {DNX_FIELD_QUAL_STATISTICS_ATR_8,       },
        [bcmFieldQualifyStatProfile9]            = {DNX_FIELD_QUAL_STATISTICS_ATR_9,       },
        [bcmFieldQualifyExtStat0]                = {DNX_FIELD_QUAL_EXT_STATISTICS_ID_0,    },
        [bcmFieldQualifyExtStat1]                = {DNX_FIELD_QUAL_EXT_STATISTICS_ID_1,    },
        [bcmFieldQualifyExtStat2]                = {DNX_FIELD_QUAL_EXT_STATISTICS_ID_2,    },
        [bcmFieldQualifyExtStat3]                = {DNX_FIELD_QUAL_EXT_STATISTICS_ID_3,    },
        [bcmFieldQualifyExtStatProfile0]         = {DNX_FIELD_QUAL_EXT_STATISTICS_ATR_0,   },
        [bcmFieldQualifyExtStatProfile1]         = {DNX_FIELD_QUAL_EXT_STATISTICS_ATR_1,   },
        [bcmFieldQualifyExtStatProfile2]         = {DNX_FIELD_QUAL_EXT_STATISTICS_ATR_2,   },
        [bcmFieldQualifyExtStatProfile3]         = {DNX_FIELD_QUAL_EXT_STATISTICS_ATR_3,   },
        [bcmFieldQualifyStatMetaData]            = {DNX_FIELD_QUAL_STATISTICS_META_DATA,   },
        [bcmFieldQualifyEcmpLoadBalanceKey0]     = {DNX_FIELD_QUAL_ECMP_LB_KEY_0,          },
        [bcmFieldQualifyEcmpLoadBalanceKey1]     = {DNX_FIELD_QUAL_ECMP_LB_KEY_1,          },
        [bcmFieldQualifyEcmpLoadBalanceKey2]     = {DNX_FIELD_QUAL_ECMP_LB_KEY_2,          },
        [bcmFieldQualifyNetworkQos]              = {DNX_FIELD_QUAL_NWK_QOS, },
        [bcmFieldQualifyMirrorCode]                  = {DNX_FIELD_QUAL_MIRROR_CODE,},
        [bcmFieldQualifyPtch]                        = {DNX_FIELD_QUAL_PRT_QUALIFIER,},
        [bcmFieldQualifyLearnData]                   = {DNX_FIELD_QUAL_LEARN_DATA,},
        [bcmFieldQualifyLearnSrcMac]                 = {DNX_FIELD_QUAL_LEARN_SRC_MAC,},
        [bcmFieldQualifyLearnVsi]                    = {DNX_FIELD_QUAL_LEARN_VSI,},
        [bcmFieldQualifyLearnVlan]                   = {DNX_FIELD_QUAL_LEARN_VLAN,},
        [bcmFieldQualifyLearnStationMove]            = {DNX_FIELD_QUAL_LEARN_STATION_MOVE,},
        [bcmFieldQualifyLearnMatch]                  = {DNX_FIELD_QUAL_LEARN_MATCH,},
        [bcmFieldQualifyLearnFound]                  = {DNX_FIELD_QUAL_LEARN_FOUND,},
        [bcmFieldQualifyLearnExpectedWon]            = {DNX_FIELD_QUAL_LEARN_EXPECTED_WON,},
        [bcmFieldQualifyVrfValue]                    = {DNX_FIELD_QUAL_VRF_VALUE,},
        [bcmFieldQualifySaLookupAcceptedStrength]    = {DNX_FIELD_QUAL_RPF_DEFAULT_ROUTE_FOUND,},
        [bcmFieldQualifyRpfOutVPortRaw]              = {DNX_FIELD_QUAL_RPF_OUT_LIF, },
        [bcmFieldQualifyRpfOutVPort]                 = {DNX_FIELD_QUAL_RPF_OUT_LIF, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_out_lif},
        [bcmFieldQualifyRpfOutInterface]             = {DNX_FIELD_QUAL_RPF_OUT_LIF, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_rif_intf_to_lif},
        [bcmFieldQualifyPacketRes]                   = {DNX_FIELD_QUAL_UNKNOWN_ADDRESS, },
        [bcmFieldQualifyEcnValue]                    = {DNX_FIELD_QUAL_ECN,},
        [bcmFieldQualifyVipValid]                    = {DNX_FIELD_QUAL_VW_VIP_VALID,},
        [bcmFieldQualifyVipId]                       = {DNX_FIELD_QUAL_VW_VIP_ID,},
        [bcmFieldQualifyVIPMemberReference]          = {DNX_FIELD_QUAL_VW_MEMBER_REFERENCE,},
        [bcmFieldQualifyPccHit]                      = {DNX_FIELD_QUAL_VW_PCC_HIT,},
        [bcmFieldQualifyAcInLifWideDataExtendedRaw]  = {DNX_FIELD_QUAL_AC_IN_LIF_WIDE_DATA_EXTENDED, },
        [bcmFieldQualifyAcInLifWideDataExtended]     = {DNX_FIELD_QUAL_AC_IN_LIF_WIDE_DATA_EXTENDED, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_ac_in_lif_wide_data_extended},
        [bcmFieldQualifyDstClassL2]                  = {DNX_FIELD_QUAL_MACT_ENTRY_GROUPING, },

};

static const dnx_field_qual_map_t dnx_ipmf2_meta_qual_map[bcmFieldQualifyCount] = {
    [bcmFieldQualifyHashValue]                   = {DNX_FIELD_QUAL_TRJ_HASH,                       },
    [bcmFieldQualifyNetworkLoadBalanceKey]       = {DNX_FIELD_QUAL_NWK_LB_KEY,                     },
    [bcmFieldQualifyEcmpLoadBalanceKey0]         = {DNX_FIELD_QUAL_ECMP_LB_KEY_0,                  },
    [bcmFieldQualifyEcmpLoadBalanceKey1]         = {DNX_FIELD_QUAL_ECMP_LB_KEY_1,                  },
    [bcmFieldQualifyEcmpLoadBalanceKey2]         = {DNX_FIELD_QUAL_ECMP_LB_KEY_2,                  },
    [bcmFieldQualifyStateTableData]              = {DNX_FIELD_QUAL_STATE_DATA,                     },
    [bcmFieldQualifyCompareKeysResult0]          = {DNX_FIELD_QUAL_CMP_KEY_0_DECODED,              },
    [bcmFieldQualifyCompareKeysResult1]          = {DNX_FIELD_QUAL_CMP_KEY_1_DECODED,              },
    [bcmFieldQualifyCompareKeysTcam0]            = {DNX_FIELD_QUAL_CMP_KEY_TCAM_0_DECODED,         },
    [bcmFieldQualifyCompareKeysTcam1]            = {DNX_FIELD_QUAL_CMP_KEY_TCAM_1_DECODED,         },

};

static const dnx_field_qual_map_t dnx_ipmf3_meta_qual_map[bcmFieldQualifyCount] = {
        [bcmFieldQualifyGeneratedTtl]                = {DNX_FIELD_QUAL_IN_TTL,                         },
        [bcmFieldQualifyPacketIsIEEE1588]            = {DNX_FIELD_QUAL_PACKET_IS_IEEE1588,             },
        [bcmFieldQualifyIEEE1588Encapsulation]       = {DNX_FIELD_QUAL_IEEE1588_ENCAPSULATION,         },
        [bcmFieldQualifyIEEE1588CompensateTimeStamp] = {DNX_FIELD_QUAL_IEEE1588_COMPENSATE_TIME_STAMP, },
        [bcmFieldQualifyIEEE1588Command]             = {DNX_FIELD_QUAL_IEEE1588_COMMAND,               },
        [bcmFieldQualifyIEEE1588HeaderOffset]        = {DNX_FIELD_QUAL_IEEE1588_HEADER_OFFSET,         },
        [bcmFieldQualifyStackingRoute]               = {DNX_FIELD_QUAL_STACKING_ROUTE_HISTORY_BITMAP,  },
        [bcmFieldQualifyDstRpfGport]                 = {DNX_FIELD_QUAL_RPF_DST,                        },
        [bcmFieldQualifyDstRpfValid]                 = {DNX_FIELD_QUAL_RPF_DST_VALID,                  },
        [bcmFieldQualifyKeyGenVar]                   = {DNX_FIELD_QUAL_CONTEXT_KEY_GEN_VAR,            },
        [bcmFieldQualifySrcPortRaw]                  = {DNX_FIELD_QUAL_SRC_SYS_PORT,                   },
        [bcmFieldQualifySrcPort]                     = {DNX_FIELD_QUAL_SRC_SYS_PORT,BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_sys_port_gport_to_port},
        [bcmFieldQualifySrcModPortGport]             = {DNX_FIELD_QUAL_SRC_SYS_PORT,BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_sys_port_gport_to_port},
        [bcmFieldQualifyInPortRaw]                   = {DNX_FIELD_QUAL_IN_PORT,                        },
        [bcmFieldQualifyInPort]                      = {DNX_FIELD_QUAL_IN_PORT,BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_port_with_core},
        [bcmFieldQualifyInPortWithoutCoreRaw]        = {DNX_FIELD_QUAL_PP_PORT, },
        [bcmFieldQualifyInPortWithoutCore]           = {DNX_FIELD_QUAL_PP_PORT,BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_port_without_core},
        [bcmFieldQualifyRxTrapCodeForSnoop]          = {DNX_FIELD_QUAL_SNOOP_CODE,                     },
        [bcmFieldQualifyRxSnoopStrength]             = {DNX_FIELD_QUAL_SNOOP_STRENGTH,                 },
        [bcmFieldQualifyOamUpMep]                    = {DNX_FIELD_QUAL_OAM_UP_MEP,                     },
        [bcmFieldQualifyOamSubtype]                  = {DNX_FIELD_QUAL_OAM_SUB_TYPE,                   },
        [bcmFieldQualifyOamStampOffset]              = {DNX_FIELD_QUAL_OAM_STAMP_OFFSET,               },
        [bcmFieldQualifyOamHeaderOffset]             = {DNX_FIELD_QUAL_OAM_OFFSET,                     },
        [bcmFieldQualifyOamMepId]                    = {DNX_FIELD_QUAL_OAM_ID,                         },
        [bcmFieldQualifyInVPort0Raw]                 = {DNX_FIELD_QUAL_GLOB_IN_LIF_0},
        [bcmFieldQualifyInVPort1Raw]                 = {DNX_FIELD_QUAL_GLOB_IN_LIF_1 },
        [bcmFieldQualifyOutVPort0Raw]                = {DNX_FIELD_QUAL_GLOB_OUT_LIF_0 },
        [bcmFieldQualifyOutVPort1Raw]                = {DNX_FIELD_QUAL_GLOB_OUT_LIF_1 },
        [bcmFieldQualifyOutVPort2Raw]                = {DNX_FIELD_QUAL_GLOB_OUT_LIF_2 },
        [bcmFieldQualifyOutVPort3Raw]                = {DNX_FIELD_QUAL_GLOB_OUT_LIF_3},
        [bcmFieldQualifyInVPort0]                    = {DNX_FIELD_QUAL_GLOB_IN_LIF_0, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_in_lif},
        [bcmFieldQualifyInVPort1]                    = {DNX_FIELD_QUAL_GLOB_IN_LIF_1, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_in_lif},
        [bcmFieldQualifyOutVPort0]                   = {DNX_FIELD_QUAL_GLOB_OUT_LIF_0, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_out_lif},
        [bcmFieldQualifyOutVPort1]                   = {DNX_FIELD_QUAL_GLOB_OUT_LIF_1, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_out_lif},
        [bcmFieldQualifyOutVPort2]                   = {DNX_FIELD_QUAL_GLOB_OUT_LIF_2, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_out_lif},
        [bcmFieldQualifyOutVPort3]                   = {DNX_FIELD_QUAL_GLOB_OUT_LIF_3, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_out_lif},
        [bcmFieldQualifyInInterface]                 = {DNX_FIELD_QUAL_GLOB_IN_LIF_0, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_rif_intf_to_lif},
        [bcmFieldQualifyOutInterface]                = {DNX_FIELD_QUAL_GLOB_OUT_LIF_0, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_rif_intf_to_lif},
        [bcmFieldQualifyDstPort]                     = {DNX_FIELD_QUAL_FWD_ACTION_DESTINATION},
        [bcmFieldQualifyDstGport]                    = {DNX_FIELD_QUAL_FWD_ACTION_DESTINATION, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_trap_gport_to_hw_dest},
        [bcmFieldQualifyTrunkHashResult]             = {DNX_FIELD_QUAL_LAG_LB_KEY,                     },
        [bcmFieldQualifyInVportClass0]               = {DNX_FIELD_QUAL_IN_LIF_PROFILE_0,               },
        [bcmFieldQualifyInVportClass1]               = {DNX_FIELD_QUAL_IN_LIF_PROFILE_1,               },
        [bcmFieldQualifyTranslatedOuterVlanId ]      = {DNX_FIELD_QUAL_VLAN_EDIT_VID_1,                },
        [bcmFieldQualifyTranslatedOuterVlanPri]      = {DNX_FIELD_QUAL_VLAN_EDIT_PCP_1,                },
        [bcmFieldQualifyTranslatedOuterVlanCfi]      = {DNX_FIELD_QUAL_VLAN_EDIT_DEI_1,                },
        [bcmFieldQualifyTranslatedInnerVlanId ]      = {DNX_FIELD_QUAL_VLAN_EDIT_VID_2,                },
        [bcmFieldQualifyTranslatedInnerVlanPri]      = {DNX_FIELD_QUAL_VLAN_EDIT_PCP_2,                },
        [bcmFieldQualifyTranslatedInnerVlanCfi]      = {DNX_FIELD_QUAL_VLAN_EDIT_DEI_2,                },
        [bcmFieldQualifyVlanAction]                  = {DNX_FIELD_QUAL_VLAN_EDIT_CMD_INDEX,            },
        [bcmFieldQualifyL2Learn]                     = {DNX_FIELD_QUAL_INGRESS_LEARN_ENABLE,           },
        [bcmFieldQualifyMplsForwardingLabelAction]   = {DNX_FIELD_QUAL_EEI,                            },
        [bcmFieldQualifyTrillEgressRbridge]          = {DNX_FIELD_QUAL_EEI, BCM_TO_DNX_ADDITIONAL_OBJ  },
        [bcmFieldQualifyISid]                        = {DNX_FIELD_QUAL_EEI, BCM_TO_DNX_ADDITIONAL_OBJ  },
        [bcmFieldQualifyVrf]                         = {DNX_FIELD_QUAL_FWD_DOMAIN,                      },
        [bcmFieldQualifyVpn]                         = {DNX_FIELD_QUAL_FWD_DOMAIN,BCM_TO_DNX_ADDITIONAL_OBJ  },
        [bcmFieldQualifyContainer]                   = {DNX_FIELD_QUAL_GENERAL_DATA_PMF_CONTAINER      },
        [bcmFieldQualifyIPTProfile]                  = {DNX_FIELD_QUAL_END_OF_PACKET_EDITING,          },
        [bcmFieldQualifyRxTrapCode]                  = {DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_CODE,       },
        [bcmFieldQualifyRxTrapData]                  = {DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_QUAL,       },
        [bcmFieldQualifyUDHData0]                    = {DNX_FIELD_QUAL_USER_HEADER_1,                  },
        [bcmFieldQualifyUDHData1]                    = {DNX_FIELD_QUAL_USER_HEADER_2,                  },
        [bcmFieldQualifyUDHData2]                    = {DNX_FIELD_QUAL_USER_HEADER_3,                  },
        [bcmFieldQualifyUDHData3]                    = {DNX_FIELD_QUAL_USER_HEADER_4,                  },
        [bcmFieldQualifyUDHBase0]                    = {DNX_FIELD_QUAL_USER_HEADER_1_TYPE,             },
        [bcmFieldQualifyUDHBase1]                    = {DNX_FIELD_QUAL_USER_HEADER_2_TYPE,             },
        [bcmFieldQualifyUDHBase2]                    = {DNX_FIELD_QUAL_USER_HEADER_3_TYPE,             },
        [bcmFieldQualifyUDHBase3]                    = {DNX_FIELD_QUAL_USER_HEADER_4_TYPE,             },
        [bcmFieldQualifyRxTrapStrength]              = {DNX_FIELD_QUAL_FWD_ACTION_STRENGTH             },
        [bcmFieldQualifyColorRaw]                    = {DNX_FIELD_QUAL_FWD_ACTION_DP,               },
        [bcmFieldQualifyColor]                       = {DNX_FIELD_QUAL_FWD_ACTION_DP, BCM_TO_DNX_ADDITIONAL_OBJ , dnx_field_convert_color },
        [bcmFieldQualifyIntPriority]                 = {DNX_FIELD_QUAL_FWD_ACTION_TC,                  },
        [bcmFieldQualifyStatSamplingCode]            = {DNX_FIELD_QUAL_STATISTICAL_SAMPLING_CODE,      },
        [bcmFieldQualifyStatSamplingQualifier]       = {DNX_FIELD_QUAL_STATISTICAL_SAMPLING_QUALIFIER, },
        [bcmFieldQualifyRpfEcmpMode]                 = {DNX_FIELD_QUAL_RPF_ECMP_MODE, },
        [bcmFieldQualifyInterfaceClassL2]            = {DNX_FIELD_QUAL_PMF_ETH_RIF_PROFILE, },
        [bcmFieldQualifyIncomingIpIfClass]           = {DNX_FIELD_QUAL_PMF_ETH_RIF_PROFILE,BCM_TO_DNX_ADDITIONAL_OBJ },
        [bcmFieldQualifyStatOamLMRaw]                = {DNX_FIELD_QUAL_STAT_OBJ_LM_READ_INDEX, },
        [bcmFieldQualifyStatOamLM]                   = {DNX_FIELD_QUAL_STAT_OBJ_LM_READ_INDEX ,BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_stat_lm_index},
        [bcmFieldQualifyForwardingLayerIndex]        = {DNX_FIELD_QUAL_FWD_LAYER_INDEX,                },
        [bcmFieldQualifyPortClassPacketProcessing]   = {DNX_FIELD_QUAL_PP_PORT_PMF_PROFILE,            },
        [bcmFieldQualifyPortClassTrafficManagement]  = {DNX_FIELD_QUAL_TM_PORT_PMF_PROFILE,            },
        [bcmFieldQualifyPortClassPacketProcessingGeneralData] = {DNX_FIELD_QUAL_PP_PORT_PMF_GENERAL_DATA, },
        [bcmFieldQualifyFlowId]                      = {DNX_FIELD_QUAL_LATENCY_FLOW_ID,                },
        [bcmFieldQualifyBierStringOffset]            = {DNX_FIELD_QUAL_BIER_STR_OFFSET,                },
        [bcmFieldQualifyBierStringSize]              = {DNX_FIELD_QUAL_BIER_STR_SIZE,                  },
        [bcmFieldQualifyPacketIsBier]                = {DNX_FIELD_QUAL_PACKET_IS_BIER,                 },
        [bcmFieldQualifyForwardingAdditionalInfo]    = {DNX_FIELD_QUAL_FWD_LAYER_ADDITIONAL_INFO,      },
        [bcmFieldQualifyStatMetaData]                = {DNX_FIELD_QUAL_STATISTICS_META_DATA,   },
        [bcmFieldQualifyNetworkQos]                  = {DNX_FIELD_QUAL_NWK_QOS, },
        [bcmFieldQualifyMirrorCode]                   = {DNX_FIELD_QUAL_MIRROR_CODE,},
        [bcmFieldQualifyMirrorData]                   = {DNX_FIELD_QUAL_MIRROR_QUALIFIER,},
        [bcmFieldQualifyLearnData]                   = {DNX_FIELD_QUAL_LEARN_DATA,},
        [bcmFieldQualifyLearnSrcMac]                 = {DNX_FIELD_QUAL_LEARN_SRC_MAC,},
        [bcmFieldQualifyLearnVsi]                    = {DNX_FIELD_QUAL_LEARN_VSI,},
        [bcmFieldQualifyLearnVlan]                   = {DNX_FIELD_QUAL_LEARN_VLAN,},
        [bcmFieldQualifyLearnStationMove]            = {DNX_FIELD_QUAL_LEARN_STATION_MOVE,},
        [bcmFieldQualifyLearnMatch]                  = {DNX_FIELD_QUAL_LEARN_MATCH,},
        [bcmFieldQualifyLearnFound]                  = {DNX_FIELD_QUAL_LEARN_FOUND,},
        [bcmFieldQualifyLearnExpectedWon]            = {DNX_FIELD_QUAL_LEARN_EXPECTED_WON,},
        [bcmFieldQualifyVrfValue]                    = {DNX_FIELD_QUAL_VRF_VALUE,},
        [bcmFieldQualifyEcmpGroup]                   = {DNX_FIELD_QUAL_RPF_ECMP_GROUP,},
        [bcmFieldQualifyEcnValue]                    = {DNX_FIELD_QUAL_CONGESTION_INFO,},

};

static const dnx_field_qual_map_t dnx_epmf_meta_qual_map[bcmFieldQualifyCount] = {
        [bcmFieldQualifyFhei]                        = {DNX_FIELD_QUAL_FHEI_EXT,                 },
        [bcmFieldQualifyFheiSize]                    = {DNX_FIELD_QUAL_PPH_FHEI_SIZE,            },
        [bcmFieldQualifyRepCopy]                     = {DNX_FIELD_QUAL_FTMH_TM_ACTION_TYPE, 0, dnx_field_convert_rep_copy_type},
        [bcmFieldQualifySrcPortRaw]                  = {DNX_FIELD_QUAL_FTMH_SRC_SYS_PORT, },
        [bcmFieldQualifySrcPort]                     = {DNX_FIELD_QUAL_FTMH_SRC_SYS_PORT, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_sys_port_gport_to_port},
        [bcmFieldQualifySrcModPortGport]             = {DNX_FIELD_QUAL_FTMH_SRC_SYS_PORT,   BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_sys_port_gport_to_port},
        [bcmFieldQualifyDstPortRaw]                  = {DNX_FIELD_QUAL_DST_SYS_PORT, },
        [bcmFieldQualifyDstL3Egress]                 = {DNX_FIELD_QUAL_DST_SYS_PORT, BCM_TO_DNX_ADDITIONAL_OBJ, },
        [bcmFieldQualifyDstPort]                     = {DNX_FIELD_QUAL_DST_SYS_PORT,BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_sys_port_gport_to_port},
        [bcmFieldQualifyDstMulticastGroup]           = {DNX_FIELD_QUAL_DST_SYS_PORT,BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_sys_port_gport_to_port },
        [bcmFieldQualifyDstClassField]               = {DNX_FIELD_QUAL_PPH_VALUE1,               },
        [bcmFieldQualifySrcClassField]               = {DNX_FIELD_QUAL_PPH_VALUE2,               },
        [bcmFieldQualifyEcnValue]                    = {DNX_FIELD_QUAL_FTMH_CNI,                 },
        [bcmFieldQualifyStackingRoute]               = {DNX_FIELD_QUAL_STACKING_EXT,  },
        /* In order for Application Specific Extension to be OAMTS first bit should be 0 */
        [bcmFieldQualifyOamTsSystemHeader]           = {DNX_FIELD_QUAL_APPLICATION_SPECIFIC_EXT, },
        [bcmFieldQualifyInVPort0Raw]                 = {DNX_FIELD_QUAL_GLOB_IN_LIF_0, },
        [bcmFieldQualifyOutVPort0Raw]                = {DNX_FIELD_QUAL_GLOB_OUT_LIF_0, },
        [bcmFieldQualifyOutVPort1Raw]                = {DNX_FIELD_QUAL_GLOB_OUT_LIF_1, },
        [bcmFieldQualifyOutVPort2Raw]                = {DNX_FIELD_QUAL_GLOB_OUT_LIF_2, },
        [bcmFieldQualifyOutVPort3Raw]                = {DNX_FIELD_QUAL_GLOB_OUT_LIF_3, },
        [bcmFieldQualifyInVPort0]                    = {DNX_FIELD_QUAL_GLOB_IN_LIF_0, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_in_lif},
        [bcmFieldQualifyOutVPort0]                   = {DNX_FIELD_QUAL_GLOB_OUT_LIF_0, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_out_lif},
        [bcmFieldQualifyOutVPort1]                   = {DNX_FIELD_QUAL_GLOB_OUT_LIF_1, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_out_lif},
        [bcmFieldQualifyOutVPort2]                   = {DNX_FIELD_QUAL_GLOB_OUT_LIF_2, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_out_lif},
        [bcmFieldQualifyOutVPort3]                   = {DNX_FIELD_QUAL_GLOB_OUT_LIF_3, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_out_lif},
        [bcmFieldQualifyInInterface]                 = {DNX_FIELD_QUAL_GLOB_IN_LIF_0, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_rif_intf_to_lif},
        [bcmFieldQualifyOutInterface]                = {DNX_FIELD_QUAL_GLOB_OUT_LIF_0, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_rif_intf_to_lif},
        [bcmFieldQualifyOutPortRaw]                  = {DNX_FIELD_QUAL_OUT_PP_PORT, },
        [bcmFieldQualifyOutPort]                     = {DNX_FIELD_QUAL_OUT_PP_PORT, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_port_without_core},
        [bcmFieldQualifyTrunkHashResult]             = {DNX_FIELD_QUAL_LB_KEY,                   },
        [bcmFieldQualifyNetworkLoadBalanceKey]       = {DNX_FIELD_QUAL_NWK_KEY,                  },
        [bcmFieldQualifyInVportClass0]               = {DNX_FIELD_QUAL_IN_LIF_PROFILE_0,         },
        [bcmFieldQualifyVrf]                         = {DNX_FIELD_QUAL_PPH_FWD_DOMAIN, },
        [bcmFieldQualifyVpn]                         = {DNX_FIELD_QUAL_PPH_FWD_DOMAIN,BCM_TO_DNX_ADDITIONAL_OBJ },
        [bcmFieldQualifyIPTProfile]                  = {DNX_FIELD_QUAL_PPH_END_OF_PACKET_EDITING,},
        [bcmFieldQualifyL4PortRangeCheck]            = {DNX_FIELD_QUAL_L4_PORT_IN_RANGE          },
        [bcmFieldQualifyGeneratedTtl]                = {DNX_FIELD_QUAL_PPH_TTL,                  },
        [bcmFieldQualifyColorRaw]                    = {DNX_FIELD_QUAL_FTMH_DP, },
        [bcmFieldQualifyColor]                       = {DNX_FIELD_QUAL_FTMH_DP, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_color },
        [bcmFieldQualifyIntPriority]                 = {DNX_FIELD_QUAL_FTMH_TC,                  },
        [bcmFieldQualifyRxSnoopStrength]             = {DNX_FIELD_QUAL_SNOOP_STRENGTH,           },
        [bcmFieldQualifyUDHData0]                    = {DNX_FIELD_QUAL_USER_HEADER_1,            },
        [bcmFieldQualifyUDHData1]                    = {DNX_FIELD_QUAL_USER_HEADER_2,            },
        [bcmFieldQualifyUDHData2]                    = {DNX_FIELD_QUAL_USER_HEADER_3,            },
        [bcmFieldQualifyUDHData3]                    = {DNX_FIELD_QUAL_USER_HEADER_4,            },
        [bcmFieldQualifyUDHBase0]                    = {DNX_FIELD_QUAL_USER_HEADER_1_TYPE,       },
        [bcmFieldQualifyUDHBase1]                    = {DNX_FIELD_QUAL_USER_HEADER_2_TYPE,       },
        [bcmFieldQualifyUDHBase2]                    = {DNX_FIELD_QUAL_USER_HEADER_3_TYPE,       },
        [bcmFieldQualifyUDHBase3]                    = {DNX_FIELD_QUAL_USER_HEADER_4_TYPE,       },
        [bcmFieldQualifyRxTrapStrength]              = {DNX_FIELD_QUAL_FWD_STRENGTH,             },
        [bcmFieldQualifyRxSnoopCode]                 = {DNX_FIELD_QUAL_SNOOP_ACTION_PROFILE_INDEX,},
        [bcmFieldQualifyOutVportClass]               = {DNX_FIELD_QUAL_OUT_LIF_PROFILE,},
        [bcmFieldQualifyInterfaceClassL2]            = {DNX_FIELD_QUAL_PMF_ETH_RIF_PROFILE       },
        [bcmFieldQualifyPortClassPacketProcessing]   = {DNX_FIELD_QUAL_PP_PORT_PMF_PROFILE,      },
        [bcmFieldQualifyPortClassTrafficManagement]  = {DNX_FIELD_QUAL_TM_PORT_PMF_PROFILE,      },
        [bcmFieldQualifyOutPortTrafficManagement]    = {DNX_FIELD_QUAL_PP_DSP,                   },
        [bcmFieldQualifyForwardingAdditionalInfo]    = {DNX_FIELD_QUAL_PPH_FWD_LAYER_ADDITIONAL_INFO, },
        [bcmFieldQualifyPphPresent]                  = {DNX_FIELD_QUAL_FTMH_PPH_PRESENT,         },
        [bcmFieldQualifyPacketProcessingInVportClass] = {DNX_FIELD_QUAL_PPH_IN_LIF_PROFILE,         },
        [bcmFieldQualifyRxTrapCode]                  = {DNX_FIELD_QUAL_FWD_ACTION_PROFILE_INDEX, },
        [bcmFieldQualifyAceEntryId]                  = {DNX_FIELD_QUAL_CUD_OUTLIF_OR_MCDB_PTR, },
        [bcmFieldQualifyNetworkQos]                  = {DNX_FIELD_QUAL_PPH_NWK_QOS, },
        [bcmFieldQualifyDstSysPortExt]               = {DNX_FIELD_QUAL_TM_DESTINATION_EXT, },
        [bcmFieldQualifyVrfValue]                    = {DNX_FIELD_QUAL_PPH_VRF_VALUE},
        [bcmFieldQualifyDstSysPortExtPresent]        = {DNX_FIELD_QUAL_FTMH_TM_DESTINATION_EXT_PRESENT, },
        [bcmFieldQualifyLearnData]                   = {DNX_FIELD_QUAL_LEARN_EXT_DATA,},
        [bcmFieldQualifyLearnSrcMac]                 = {DNX_FIELD_QUAL_LEARN_EXT_SRC_MAC,},
        [bcmFieldQualifyLearnVsi]                    = {DNX_FIELD_QUAL_LEARN_EXT_VSI,},
        [bcmFieldQualifyLearnVlan]                   = {DNX_FIELD_QUAL_LEARN_EXT_VLAN,},
        [bcmFieldQualifyLearnExtensionPresent]       = {DNX_FIELD_QUAL_PPH_LEARN_EXT_PRESENT,},
        [bcmFieldQualifyMcastPkt]                    = {DNX_FIELD_QUAL_FTMH_TM_ACTION_IS_MC,},
        [bcmFieldQualifyFtmhAsePresent]              = {DNX_FIELD_QUAL_FTMH_APPLICATION_SPECIFIC_EXT_PRESENT,},
        [bcmFieldQualifyIpMcFallbacktoBridge]        = {DNX_FIELD_QUAL_IP_MC_SHOULD_BE_BRIDGED,},
        [bcmFieldQualifyForwardingLayerIndex]        = {DNX_FIELD_QUAL_PPH_FWD_LAYER_INDEX,},
};

static const dnx_field_qual_map_t dnx_l4ops_meta_qual_map[bcmFieldQualifyCount] = {
    [bcmFieldQualifyVrf]                      = {DNX_FIELD_QUAL_FWD_DOMAIN, },
    [bcmFieldQualifyInVPort0]                 = {DNX_FIELD_QUAL_GLOB_IN_LIF_0, },
    [bcmFieldQualifyInVPort1]                 = {DNX_FIELD_QUAL_GLOB_IN_LIF_1, },
    [bcmFieldQualifyOutVPort0]                = {DNX_FIELD_QUAL_GLOB_OUT_LIF_0, },
    [bcmFieldQualifyOutVPort1]                = {DNX_FIELD_QUAL_GLOB_OUT_LIF_1, },
};

/*
 * End of BCM to DNX set maps per stage
 * }
 */
/*
 * DNX QUAL data structures array
 * {
 */
/**
 * Mapping to DBAL of layer record qualifiers.
 * More information regarding the layer records can be found in LayerRecordsTypesStructures.xml
 */
const static dnx_field_layer_record_qual_info_t dnx_layer_record_qual_info[DNX_FIELD_LR_QUAL_NOF] = {
 /*  DNX Qualifier ID                                     Name    DBAL Field ID,         */
    [DNX_FIELD_LR_QUAL_OFFSET]                         = {DBAL_FIELD_LAYER_OFFSET},
    [DNX_FIELD_LR_QUAL_QUALIFIER]                      = {DBAL_FIELD_LAYER_QUALIFIER},
    [DNX_FIELD_LR_QUAL_PROTOCOL]                       = {DBAL_FIELD_LAYER_TYPES},
    [DNX_FIELD_LR_QUAL_ETH_IS_DA_MC]                   = {DBAL_FIELD_ETH_IS_DA_MC},
    [DNX_FIELD_LR_QUAL_ETH_IS_DA_BC]                   = {DBAL_FIELD_ETH_IS_DA_BC},
    [DNX_FIELD_LR_QUAL_ETH_1ST_TPID_EXIST]             = {DBAL_FIELD_ETH_1ST_TPID_EXIST},
    [DNX_FIELD_LR_QUAL_ETH_1ST_TPID_INDEX]             = {DBAL_FIELD_ETH_1ST_TPID_INDEX},
    [DNX_FIELD_LR_QUAL_ETH_2ND_TPID_EXIST]             = {DBAL_FIELD_ETH_2ND_TPID_EXIST},
    [DNX_FIELD_LR_QUAL_ETH_2ND_TPID_INDEX]             = {DBAL_FIELD_ETH_2ND_TPID_INDEX},
    [DNX_FIELD_LR_QUAL_ETH_3RD_TPID_EXIST]             = {DBAL_FIELD_ETH_3RD_TPID_EXIST},
    [DNX_FIELD_LR_QUAL_ETH_3RD_TPID_INDEX]             = {DBAL_FIELD_ETH_3RD_TPID_INDEX},
    [DNX_FIELD_LR_QUAL_ETH_FRAME_TYPE]                 = {DBAL_FIELD_ETH_FRAME_TYPE},
    [DNX_FIELD_LR_QUAL_IPV4_IS_MC]                     = {DBAL_FIELD_IPV4_IS_MC},
    [DNX_FIELD_LR_QUAL_IPV4_HAS_OPTION]                = {DBAL_FIELD_IPV4_HAS_OPTION},
    [DNX_FIELD_LR_QUAL_IPV4_IS_FRAGMENTED]             = {DBAL_FIELD_IPV4_IS_FRAGMENTED},
    [DNX_FIELD_LR_QUAL_IPV4_IS_1ST_FRAGMENT]           = {DBAL_FIELD_IPV4_IS_1ST_FRAGMENT},
    [DNX_FIELD_LR_QUAL_IPV4_1ST_ADD_HEADER_EXIST]      = {DBAL_FIELD_IPV4_1ST_ADD_HEADER_EXIST},
    [DNX_FIELD_LR_QUAL_IPV4_1ST_ADD_HEADER]            = {DBAL_FIELD_IPV4_1ST_ADD_HEADER},
    [DNX_FIELD_LR_QUAL_IPV4_2ND_ADD_HEADER_EXISTS]     = {DBAL_FIELD_IPV4_2ND_ADD_HEADER_EXISTS},
    [DNX_FIELD_LR_QUAL_IPV4_TUNNEL_TYPE]               = {DBAL_FIELD_IPV4_TUNNEL_TYPE},
    [DNX_FIELD_LR_QUAL_IPV4_TUNNEL_TYPE_RAW]           = {DBAL_FIELD_IPV4_TUNNEL_TYPE_RAW},
    [DNX_FIELD_LR_QUAL_IPV4_FRAGMENTED_NON_FIRST]      = {DBAL_FIELD_IPV4_FRAGMENTED_NON_FIRST},
    [DNX_FIELD_LR_QUAL_IPV6_IS_MC]                     = {DBAL_FIELD_IPV6_IS_MC},
    [DNX_FIELD_LR_QUAL_IPV6_1ST_ADD_HEADER_EXIST]      = {DBAL_FIELD_IPV6_1ST_ADD_HEADER_EXIST},
    [DNX_FIELD_LR_QUAL_IPV6_1ST_ADD_HEADER]            = {DBAL_FIELD_IPV6_QUALIFIER_ADDITIONAL_HEADER_1ST},
    [DNX_FIELD_LR_QUAL_IPV6_2ND_ADD_HEADER_EXISTS]     = {DBAL_FIELD_IPV6_2ND_ADD_HEADER_EXISTS},
    [DNX_FIELD_LR_QUAL_IPV6_2ND_ADD_HEADER]            = {DBAL_FIELD_IPV6_QUALIFIER_ADDITIONAL_HEADER_2ND},
    [DNX_FIELD_LR_QUAL_IPV6_3RD_ADD_HEADER_EXISTS]     = {DBAL_FIELD_IPV6_3RD_ADD_HEADER_EXISTS},
    [DNX_FIELD_LR_QUAL_IPV6_FRAGMENTED_NON_FIRST]      = {DBAL_FIELD_IPV6_FRAGMENTED_NON_FIRST},
    [DNX_FIELD_LR_QUAL_SRV6_EP_SEGMENT_LEFT_MINUS_ONE] = {DBAL_FIELD_SRV6_EP_SEGMENT_LEFT_MINUS_ONE},
    [DNX_FIELD_LR_QUAL_MPLS_NOF_LABELS]                = {DBAL_FIELD_MPLS_NOF_LABELS},
    [DNX_FIELD_LR_QUAL_BIER_BITSTRINGLENGTH]           = {DBAL_FIELD_BIER_BITSTRINGLENGTH},
    [DNX_FIELD_LR_QUAL_FCOE_IS_FIP]                    = {DBAL_FIELD_FCOE_IS_FIP},
    [DNX_FIELD_LR_QUAL_FCOE_IS_ENCAP]                  = {DBAL_FIELD_FCOE_IS_ENCAP},
    [DNX_FIELD_LR_QUAL_FCOE_VFT_EXIST]                 = {DBAL_FIELD_FCOE_VFT_EXIST},
    [DNX_FIELD_LR_QUAL_ITMH_PPH_TYPE]                  = {DBAL_FIELD_ITMH_PPH_TYPE},
};

static const dnx_field_header_qual_info_t dnx_header_qual_info[DNX_FIELD_HEADER_QUAL_NOF] = {
 /*  DNX Qualifier ID                           Name                  DBAL Field ID,   Size, Offset, Signal info, Valid, BCM Qual */
    [DNX_FIELD_HEADER_QUAL_MAC_DST]          = {"ETH0.DA",            DBAL_FIELD_MAC_DST, 48, 0, {{"NIF", "PRT", "DA"}}},
    [DNX_FIELD_HEADER_QUAL_MAC_SRC]          = {"ETH0.SA",            DBAL_FIELD_MAC_SRC, 48, 48, {{"NIF", "PRT", "SA"}}},
    [DNX_FIELD_HEADER_QUAL_ETHERTYPE]        = {"ETH0.Type",          DBAL_FIELD_ETHERTYPE, 16, 96, {{"NIF", "PRT", "Type"}}},
    /*
     * VLAN Header Qualifiers
     */
    [DNX_FIELD_HEADER_QUAL_VLAN_TPID]        = {"VLAN.TPID",          DBAL_FIELD_VLAN_TPID, 16, 0,{{"NIF", "PRT", "Header.VLAN.TPID"}}},
    [DNX_FIELD_HEADER_QUAL_VLAN_ID]          = {"VLAN.VID",           DBAL_FIELD_VLAN_ID, 12, 20, {{"NIF", "PRT", "Header.VLAN.VID"}}},
    [DNX_FIELD_HEADER_QUAL_VLAN_PRI]         = {"VLAN.PCP",           DBAL_FIELD_VLAN_PRI, 3, 16, {{"NIF", "PRT", "Header.VLAN.PCP"}}},
    [DNX_FIELD_HEADER_QUAL_VLAN_CFI]         = {"VLAN.DEI",           DBAL_FIELD_VLAN_CFI, 1, 19, {{"NIF", "PRT", "Header.VLAN.DEI"}}},
    [DNX_FIELD_HEADER_QUAL_VLAN_TCI]         = {"VLAN.TCI",           DBAL_FIELD_VLAN_TCI, 16, 16},
    [DNX_FIELD_HEADER_QUAL_VLAN_PRI_CFI]     = {"VLAN.PRI_CFI",       DBAL_FIELD_VLAN_PRI_CFI, 4, 20, {{"NIF", "PRT", "Header.VLAN.PRI_CFI"}}},
    /*
     * IPv4 Header Qualifiers
     */
    [DNX_FIELD_HEADER_QUAL_IPV4_TOS]         = {"IPv4.TOS",           DBAL_FIELD_IPV4_TOS, 8, 8, {{"NIF", "PRT", "Header.IPv4.TOS"}}},
    [DNX_FIELD_HEADER_QUAL_IPV4_DSCP]        = {"IPv4.TOS.DSCP",      DBAL_FIELD_IPV4_DSCP, 6, 8, {{"NIF", "PRT", "Header.IPv4.TOS.DSCP"}}},
    [DNX_FIELD_HEADER_QUAL_IPV4_ECN]         = {"IPv4.TOS.ECN",       DBAL_FIELD_IPV4_ECN, 2, 14, {{"NIF", "PRT", "Header.IPv4.TOS.ECN"}}},
    [DNX_FIELD_HEADER_QUAL_IPV4_FLAGS]       = {"IPv4.Flags",         DBAL_FIELD_IPV4_FLAGS, 3, 48, {{"NIF", "PRT", "Header.IPv4.Flags"}}},
    [DNX_FIELD_HEADER_QUAL_IPV4_TTL]         = {"IPv4.TTL",           DBAL_FIELD_IPV4_TTL, 8, 64, {{"NIF", "PRT", "Header.IPv4.TTL"}}},
    [DNX_FIELD_HEADER_QUAL_IPV4_PROTOCOL]    = {"IPv4.Protocol",      DBAL_FIELD_IPV4_PROTOCOL, 8, 72, {{"NIF", "PRT", "Header.IPv4.Protocol"}}},
    [DNX_FIELD_HEADER_QUAL_IPV4_SRC]         = {"IPv4.SIP",           DBAL_FIELD_IPV4_SIP, 32, 96, {{"NIF", "PRT", "Header.IPv4.SIP"}}},
    [DNX_FIELD_HEADER_QUAL_IPV4_DST]         = {"IPv4.DIP",           DBAL_FIELD_IPV4_DIP, 32, 128, {{"NIF", "PRT", "Header.IPv4.DIP"}}},
    /*
     * IPv6 Header Qualifiers
     */
    [DNX_FIELD_HEADER_QUAL_IPV6_SIP]         = {"IPv6.SIP",           DBAL_FIELD_IPV6_SIP, 128, 64, {{"NIF", "PRT", "Header.IPv6.SIP"}}},
    [DNX_FIELD_HEADER_QUAL_IPV6_DIP]         = {"IPv6.DIP",           DBAL_FIELD_IPV6_DIP, 128, 192, {{"NIF", "PRT", "Header.IPv6.DI"}}},
    [DNX_FIELD_HEADER_QUAL_IPV6_SIP_LOW]     = {"IPv6.SIP.Low",       DBAL_FIELD_IPV6_SIP_LOW, 64, 128, {{"NIF", "PRT", "Header.IPv6.SIP.Low"}}},
    [DNX_FIELD_HEADER_QUAL_IPV6_SIP_HIGH]    = {"IPv6.SIP.High",      DBAL_FIELD_IPV6_SIP_HIGH, 64, 64, {{"NIF", "PRT", "Header.IPv6.SIP.High"}}},
    [DNX_FIELD_HEADER_QUAL_IPV6_DIP_LOW]     = {"IPv6.DIP.Low",       DBAL_FIELD_IPV6_DIP_LOW, 64, 256, {{"NIF", "PRT", "Header.IPv6.DIP.Low"}}},
    [DNX_FIELD_HEADER_QUAL_IPV6_DIP_HIGH]    = {"IPv6.DIP.High",      DBAL_FIELD_IPV6_DIP_HIGH, 64, 192, {{"NIF", "PRT", "Header.IPv6.DIP.High"}}},
    [DNX_FIELD_HEADER_QUAL_IPV6_TC]          = {"IPv6.Traffic_Class", DBAL_FIELD_IPV6_TC, 8, 4, {{"NIF", "PRT", "Header.IPv6.Traffic_Class"}}},
    [DNX_FIELD_HEADER_QUAL_IPV6_FLOW_LABEL]  = {"IPv6.Flow_Label",    DBAL_FIELD_IPV6_FLOW_LABEL, 20, 12, {{"NIF", "PRT", "Header.IPv6.Flow_Label"}}},
    [DNX_FIELD_HEADER_QUAL_IPV6_NEXT_HEADER] = {"IPv6.Next_Header",   DBAL_FIELD_IPV6_NEXT_HEADER, 8, 48, {{"NIF", "PRT", "Header.IPv6.Next_Header"}}},
    [DNX_FIELD_HEADER_QUAL_IPV6_HOP_LIMIT]   = {"IPv6.Hop_Limit",     DBAL_FIELD_IPV6_HOP_LIMIT, 8, 56, {{"NIF", "PRT", "Header.IPv6.Hop_Limit"}}},
    [DNX_FIELD_HEADER_QUAL_IPV6_1ST_EXTENSION_NEXT_HEADER]   = {"IPv6_With_Ext.Ext.Next_Header", DBAL_FIELD_IPV6_1ST_EXTENSION_NEXT_HEADER, 8, 320, {{"NIF", "PRT", "Header.SRv6.Next_Header"}}},
    /*
     * TCP/UDP Common Fields
     */
    [DNX_FIELD_HEADER_QUAL_L4_SRC_PORT]      = {"UDP.Src_Port",       DBAL_FIELD_L4_SRC_PORT, 16, 0, {{"NIF", "PRT", "Header.UDP.Src_Port"}}},
    [DNX_FIELD_HEADER_QUAL_L4_DST_PORT]      = {"UDP.Dst_Port",       DBAL_FIELD_L4_DST_PORT, 16, 16, {{"NIF", "PRT", "Header.UDP.Dst_Port"}}},
    /*
     * MPLS Header Qualifiers
     */
    [DNX_FIELD_HEADER_QUAL_MPLS_LABEL]       = {"MPLS",               DBAL_FIELD_MPLS_HEADER, 32, 0, {{"NIF", "PRT", "Header.MPLS"}}},
    [DNX_FIELD_HEADER_QUAL_MPLS_LABEL_ID]    = {"MPLS.Label",         DBAL_FIELD_MPLS_LABEL, 20, 0, {{"NIF", "PRT", "Header.MPLS.Label"}}},
    [DNX_FIELD_HEADER_QUAL_MPLS_TC]          = {"MPLS.EXP",           DBAL_FIELD_MPLS_TC, 3, 20, {{"NIF", "PRT", "Header.MPLS.EXP"}}},
    [DNX_FIELD_HEADER_QUAL_MPLS_BOS]         = {"MPLS.BOS",           DBAL_FIELD_MPLS_BOS, 1, 23, {{"NIF", "PRT", "Header.MPLS.BOS"}}},
    [DNX_FIELD_HEADER_QUAL_MPLS_TTL]         = {"MPLS.TTL",           DBAL_FIELD_MPLS_TTL, 8, 24, {{"NIF", "PRT", "Header.MPLS.TTL"}}},
    /*
     * ARP Header Qualifiers
     */
    [DNX_FIELD_HEADER_QUAL_ARP_OPCODE]       = {"ARP.Opcode",         DBAL_FIELD_ARP_OPCODE, 16, 48, {{"NIF", "PRT", "Header.ARP.Opcode"}}},
    [DNX_FIELD_HEADER_QUAL_ARP_SENDER_IP]    = {"ARP.Sender_IP",      DBAL_FIELD_ARP_SENDER_IP, 32, 112, {{"NIF", "PRT", "Header.ARP.Sender_IP"}}},
    [DNX_FIELD_HEADER_QUAL_ARP_TARGET_IP]    = {"ARP.Target_IP",      DBAL_FIELD_ARP_TARGET_IP, 32, 192, {{"NIF", "PRT", "Header.ARP.Target_IP"}}},
    /*
     * TCP Requested Fields
     */
    [DNX_FIELD_HEADER_QUAL_TCP_CTL]          = {"TCP.Control.Base",         DBAL_FIELD_TCP_CTL, 6, 106, {{"NIF", "PRT", "Header.TCP.Control.Base"}}},
    /*
     * RCH Header Qualifiers
     */
    [DNX_FIELD_HEADER_QUAL_RCH_TIME_STAMP]    = {"RCH.TS",            DBAL_FIELD_RCH_TIME_STAMP , 1, 4, {{"NIF", "PRT", "Header.RCH.TS"}}},
    [DNX_FIELD_HEADER_QUAL_RCH_FORWARDING_ADDITIONAL_INFO]    = {"RCH.FAI",           DBAL_FIELD_RCH_FORWARDING_ADDITIONAL_INFO , 4, 132, {{"NIF", "PRT", "Header.RCH.FAI"}}},
    [DNX_FIELD_HEADER_QUAL_RCH_TYPE]          = {"RCH.RCH_Type",      DBAL_FIELD_RCH_TYPE , 4, 0, {{"NIF", "PRT", "Header.RCH.RCH_Type"}}},
    [DNX_FIELD_HEADER_QUAL_RCH_VISIBILITY]    = {"RCH.Vis",           DBAL_FIELD_RCH_VISIBILITY , 1, 32, {{"NIF", "PRT", "Header.RCH.Vis"}}},

};

static const dnx_field_sw_qual_info_t dnx_sw_qual_info[DNX_FIELD_SW_QUAL_NOF] = {
 /* DNX Qualifier ID                           Size, Field ID           Offset, Index, FFC Type*/
 /* Pure SW qualifier - currently only one */
    [DNX_FIELD_SW_QUAL_ALL_ZERO]    = {DBAL_FIELD_ALL_ZERO, 32}
};

static const dnx_field_meta_qual_info_t dnx_external_meta_qual_info[] = {
        [DNX_FIELD_QUAL_FWD_LAYER_INDEX]                    = {DBAL_FIELD_FWD_LAYER_INDEX},
        [DNX_FIELD_QUAL_RPF_ROUTE_VALID]                    = {DBAL_FIELD_RPF_ROUTE_VALID},
        [DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_FWD_PROFILE]  = {DBAL_FIELD_FWD1_PRIORITY_DECODER_FWD_PROFILE},
        [DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_FWD_RESULT]   = {DBAL_FIELD_FWD1_PRIORITY_DECODER_FWD_RESULT},
        [DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_RPF_PROFILE]  = {DBAL_FIELD_FWD1_PRIORITY_DECODER_RPF_PROFILE},
        [DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_RPF_RESULT]   = {DBAL_FIELD_FWD1_PRIORITY_DECODER_RPF_RESULT},
        [DNX_FIELD_QUAL_MP_PROFILE_SEL]                     = {DBAL_FIELD_MP_PROFILE_SEL},
        [DNX_FIELD_QUAL_RPF_DEFAULT_ROUTE_FOUND]            = {DBAL_FIELD_RPF_DEFAULT_ROUTE_FOUND},
        [DNX_FIELD_QUAL_ACL_CONTEXT]                        = {DBAL_FIELD_ACL_CONTEXT},
        [DNX_FIELD_QUAL_CONTEXT]                            = {DBAL_FIELD_CONTEXT},
        [DNX_FIELD_QUAL_NASID]                              = {DBAL_FIELD_NASID},
        [DNX_FIELD_QUAL_ENABLE_PP_INJECT]                   = {DBAL_FIELD_ENABLE_PP_INJECT},
        [DNX_FIELD_QUAL_EEI]                                = {DBAL_FIELD_EEI},
        [DNX_FIELD_QUAL_EGRESS_LEARN_ENABLE]                = {DBAL_FIELD_EGRESS_LEARN_ENABLE},
        [DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_CODE]           = {DBAL_FIELD_FWD_ACTION_CPU_TRAP_CODE},
        [DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_QUAL]           = {DBAL_FIELD_FWD_ACTION_CPU_TRAP_QUAL},
        [DNX_FIELD_QUAL_FWD_ACTION_DESTINATION]             = {DBAL_FIELD_FWD_ACTION_DESTINATION},
        [DNX_FIELD_QUAL_FWD_ACTION_STRENGTH]                = {DBAL_FIELD_FWD_ACTION_STRENGTH},
        [DNX_FIELD_QUAL_FWD_DOMAIN]                         = {DBAL_FIELD_FWD_DOMAIN},
        /* Domain profile should not be exposed, exposed only pmf bits in pmf */
        [DNX_FIELD_QUAL_FWD_DOMAIN_PROFILE]                 = {DBAL_FIELD_FWD_DOMAIN_PROFILE},
        [DNX_FIELD_QUAL_PMF_ETH_RIF_PROFILE]                = {DBAL_FIELD_PMF_ETH_RIF_PROFILE},
        [DNX_FIELD_QUAL_FWD_LAYER_ADDITIONAL_INFO]          = {DBAL_FIELD_FWD_LAYER_ADDITIONAL_INFO},
        [DNX_FIELD_QUAL_GENERAL_DATA_0]                     = {DBAL_FIELD_GENERAL_DATA_0},
        [DNX_FIELD_QUAL_GENERAL_DATA_1]                     = {DBAL_FIELD_GENERAL_DATA_1},
        [DNX_FIELD_QUAL_GENERAL_DATA_2]                     = {DBAL_FIELD_GENERAL_DATA_2},
        [DNX_FIELD_QUAL_GENERAL_DATA_3]                     = {DBAL_FIELD_GENERAL_DATA_3},
        [DNX_FIELD_QUAL_AC_IN_LIF_WIDE_DATA]                = {DBAL_FIELD_AC_IN_LIF_WIDE_DATA},
        [DNX_FIELD_QUAL_IEEE1588_COMMAND]                   = {DBAL_FIELD_IEEE1588_COMMAND},
        [DNX_FIELD_QUAL_IEEE1588_COMPENSATE_TIME_STAMP]     = {DBAL_FIELD_IEEE1588_COMPENSATE_TIME_STAMP},
        [DNX_FIELD_QUAL_IEEE1588_ENCAPSULATION]             = {DBAL_FIELD_IEEE1588_ENCAPSULATION},
        [DNX_FIELD_QUAL_IEEE1588_HEADER_OFFSET]             = {DBAL_FIELD_IEEE1588_HEADER_OFFSET},
        [DNX_FIELD_QUAL_IN_LIF_PROFILE_0]                   = {DBAL_FIELD_IN_LIF_PROFILE_0},
        [DNX_FIELD_QUAL_IN_LIF_PROFILE_1]                   = {DBAL_FIELD_IN_LIF_PROFILE_1},
        [DNX_FIELD_QUAL_GLOB_IN_LIF_0]                      = {DBAL_FIELD_GLOB_IN_LIF_0},
        [DNX_FIELD_QUAL_GLOB_IN_LIF_1]                      = {DBAL_FIELD_GLOB_IN_LIF_1},
        [DNX_FIELD_QUAL_PP_PORT]                            = {DBAL_FIELD_PP_PORT},
        [DNX_FIELD_QUAL_INCOMING_TAG_STRUCTURE]             = {DBAL_FIELD_INCOMING_TAG_STRUCTURE},
        [DNX_FIELD_QUAL_INGRESS_LEARN_ENABLE]               = {DBAL_FIELD_INGRESS_LEARN_ENABLE},
        [DNX_FIELD_QUAL_LEARN_INFO]                         = {DBAL_FIELD_LEARN_INFO},
        [DNX_FIELD_QUAL_MEM_SOFT_ERR]                       = {DBAL_FIELD_MEM_SOFT_ERR},
        [DNX_FIELD_QUAL_NWK_QOS]                            = {DBAL_FIELD_NWK_QOS},
        [DNX_FIELD_QUAL_OAM_OFFSET]                         = {DBAL_FIELD_OAM_OFFSET},
        [DNX_FIELD_QUAL_OAM_STAMP_OFFSET]                   = {DBAL_FIELD_OAM_STAMP_OFFSET},
        [DNX_FIELD_QUAL_OAM_OPCODE]                         = {DBAL_FIELD_OAM_OPCODE},
        [DNX_FIELD_QUAL_OAM_YOUR_DISCR]                     = {DBAL_FIELD_OAM_YOUR_DISCR},
        [DNX_FIELD_QUAL_MY_CFM_MAC]                         = {DBAL_FIELD_MY_CFM_MAC},
        [DNX_FIELD_QUAL_PACKET_IS_OAM]                      = {DBAL_FIELD_PACKET_IS_OAM},
        [DNX_FIELD_QUAL_PACKET_IS_BFD]                      = {DBAL_FIELD_PACKET_IS_BFD},
        [DNX_FIELD_QUAL_MDL]                                = {DBAL_FIELD_MDL},
        [DNX_FIELD_QUAL_VTT_OAM_LIF_0]                      = {DBAL_FIELD_VTT_OAM_LIF_0},
        [DNX_FIELD_QUAL_VTT_OAM_LIF_0_VALID]                = {DBAL_FIELD_VTT_OAM_LIF_0_VALID},
        [DNX_FIELD_QUAL_VTT_OAM_LIF_1]                      = {DBAL_FIELD_VTT_OAM_LIF_1},
        [DNX_FIELD_QUAL_VTT_OAM_LIF_1_VALID]                = {DBAL_FIELD_VTT_OAM_LIF_1_VALID},
        [DNX_FIELD_QUAL_VTT_OAM_LIF_2]                      = {DBAL_FIELD_VTT_OAM_LIF_2},
        [DNX_FIELD_QUAL_VTT_OAM_LIF_2_VALID]                = {DBAL_FIELD_VTT_OAM_LIF_2_VALID},
        [DNX_FIELD_QUAL_VTT_OAM_PCP_0]                      = {DBAL_FIELD_VTT_OAM_PCP_0},
        [DNX_FIELD_QUAL_VTT_OAM_PCP_1]                      = {DBAL_FIELD_VTT_OAM_PCP_1},
        [DNX_FIELD_QUAL_VTT_OAM_PCP_2]                      = {DBAL_FIELD_VTT_OAM_PCP_2},
        [DNX_FIELD_QUAL_GLOB_OUT_LIF_0]                     = {DBAL_FIELD_GLOB_OUT_LIF_0},
        [DNX_FIELD_QUAL_GLOB_OUT_LIF_1]                     = {DBAL_FIELD_GLOB_OUT_LIF_1},
        [DNX_FIELD_QUAL_PRT_QUALIFIER]                      = {DBAL_FIELD_PRT_QUALIFIER_RAW},
        [DNX_FIELD_QUAL_PACKET_HEADER_SIZE]                 = {DBAL_FIELD_PACKET_HEADER_SIZE},
        [DNX_FIELD_QUAL_PACKET_IS_APPLET]                   = {DBAL_FIELD_IS_APPLET},
        [DNX_FIELD_QUAL_PACKET_IS_COMPATIBLE_MC]            = {DBAL_FIELD_PACKET_IS_COMPATIBLE_MC},
        [DNX_FIELD_QUAL_PACKET_IS_IEEE1588]                 = {DBAL_FIELD_PACKET_IS_IEEE1588},
        [DNX_FIELD_QUAL_DP]                                 = {DBAL_FIELD_DP},
        [DNX_FIELD_QUAL_ECN]                                = {DBAL_FIELD_METADATA_ECN},
        [DNX_FIELD_QUAL_TC]                                 = {DBAL_FIELD_TC},
        [DNX_FIELD_QUAL_PTC]                                = {DBAL_FIELD_PTC},
        [DNX_FIELD_QUAL_RPF_DST]                            = {DBAL_FIELD_RPF_DST},
        [DNX_FIELD_QUAL_RPF_DST_VALID]                      = {DBAL_FIELD_RPF_DST_VALID},
        [DNX_FIELD_QUAL_SNOOP_CODE]                         = {DBAL_FIELD_SNOOP_CODE},
        [DNX_FIELD_QUAL_SNOOP_STRENGTH]                     = {DBAL_FIELD_SNOOP_STRENGTH},
        [DNX_FIELD_QUAL_MIRROR_CODE]                        = {DBAL_FIELD_MIRROR_CODE},
        [DNX_FIELD_QUAL_STATISTICAL_SAMPLING_CODE]          = {DBAL_FIELD_STATISTICAL_SAMPLING_CODE},
        [DNX_FIELD_QUAL_SRC_SYS_PORT]                       = {DBAL_FIELD_SRC_SYS_PORT},
        [DNX_FIELD_QUAL_FLP_STATISTICS_ID_0]                = {DBAL_FIELD_STATISTICS_ID_0},
        [DNX_FIELD_QUAL_FLP_STATISTICS_ID_1]                = {DBAL_FIELD_STATISTICS_ID_1},
        [DNX_FIELD_QUAL_FLP_STATISTICS_ID_2]                = {DBAL_FIELD_STATISTICS_ID_2},
        [DNX_FIELD_QUAL_FLP_STATISTICS_ID_3]                = {DBAL_FIELD_STATISTICS_ID_3},
        [DNX_FIELD_QUAL_FLP_STATISTICS_ID_4]                = {DBAL_FIELD_STATISTICS_ID_4},
        [DNX_FIELD_QUAL_FLP_STATISTICS_ID_5]                = {DBAL_FIELD_STATISTICS_ID_5},
        [DNX_FIELD_QUAL_FLP_STATISTICS_ATR_0]               = {DBAL_FIELD_STATISTICS_ATR_0},
        [DNX_FIELD_QUAL_FLP_STATISTICS_ATR_1]               = {DBAL_FIELD_STATISTICS_ATR_1},
        [DNX_FIELD_QUAL_FLP_STATISTICS_ATR_2]               = {DBAL_FIELD_STATISTICS_ATR_2},
        [DNX_FIELD_QUAL_FLP_STATISTICS_ATR_3]               = {DBAL_FIELD_STATISTICS_ATR_3},
        [DNX_FIELD_QUAL_FLP_STATISTICS_ATR_4]               = {DBAL_FIELD_STATISTICS_ATR_4},
        [DNX_FIELD_QUAL_FLP_STATISTICS_ATR_5]               = {DBAL_FIELD_STATISTICS_ATR_5},
        [DNX_FIELD_QUAL_VLAN_EDIT_CMD]                      = {DBAL_FIELD_VLAN_EDIT_CMD},
        [DNX_FIELD_QUAL_TRACE_PACKET]                       = {DBAL_FIELD_TRACE_PACKET},
        [DNX_FIELD_QUAL_IN_TTL]                             = {DBAL_FIELD_IN_TTL},
        [DNX_FIELD_QUAL_LIF_OAM_TRAP_PROFILE]               = {DBAL_FIELD_LIF_OAM_TRAP_PROFILE},
        [DNX_FIELD_QUAL_VISIBILITY_CLEAR]                   = {DBAL_FIELD_VISIBILITY_CLEAR},
        [DNX_FIELD_QUAL_VISIBILITY]                         = {DBAL_FIELD_VISIBILITY},
        [DNX_FIELD_QUAL_RPF_OUT_LIF]                        = {DBAL_FIELD_RPF_OUT_LIF},
        [DNX_FIELD_QUAL_ACTION_PROFILE_SA_DROP_INDEX]       = {DBAL_FIELD_ACTION_PROFILE_SA_DROP_INDEX},
        [DNX_FIELD_QUAL_ACTION_PROFILE_SA_NOT_FOUND_INDEX]  = {DBAL_FIELD_ACTION_PROFILE_SA_NOT_FOUND_INDEX},
        [DNX_FIELD_QUAL_ACTION_PROFILE_DA_NOT_FOUND_INDEX]  = {DBAL_FIELD_ACTION_PROFILE_DA_NOT_FOUND_INDEX},
        [DNX_FIELD_QUAL_SAVED_CONTEXT_PROFILE]              = {DBAL_FIELD_SAVED_CONTEXT_PROFILE},
        [DNX_FIELD_QUAL_IPV4_VERSION_ERROR]                 = {DBAL_FIELD_IPV4_VERSION_ERROR},
        [DNX_FIELD_QUAL_IPV4_CHECKSUM_ERROR]                = {DBAL_FIELD_IPV4_CHECKSUM_ERROR},
        [DNX_FIELD_QUAL_IPV4_TOTAL_LENGTH_ERROR]            = {DBAL_FIELD_IPV4_TOTAL_LENGTH_ERROR},
        [DNX_FIELD_QUAL_IPV4_OPTIONS_ERROR]                 = {DBAL_FIELD_IPV4_OPTIONS_ERROR},
        [DNX_FIELD_QUAL_IPV4_SIP_EQUAL_DIP_ERROR]           = {DBAL_FIELD_IPV4_SIP_EQUAL_DIP_ERROR},
        [DNX_FIELD_QUAL_IPV4_DIP_ZERO_ERROR]                = {DBAL_FIELD_IPV4_DIP_ZERO_ERROR},
        [DNX_FIELD_QUAL_IPV4_SIP_IS_MC_ERROR]               = {DBAL_FIELD_IPV4_SIP_IS_MC_ERROR},
        [DNX_FIELD_QUAL_IPV4_HEADER_LENGTH_ERROR]           = {DBAL_FIELD_IPV4_HEADER_LENGTH_ERROR},
        [DNX_FIELD_QUAL_IN_LIF_UNKNOWN_DA_PROFILE]          = {DBAL_FIELD_IN_LIF_UNKNOWN_DA_PROFILE},
        [DNX_FIELD_QUAL_NOF_VALID_LM_LIFS]                  = {DBAL_FIELD_NOF_VALID_LM_LIFS},
        [DNX_FIELD_QUAL_IS_IN_LIF_VALID_FOR_LM]             = {DBAL_FIELD_IS_IN_LIF_VALID_FOR_LM},
        [DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_FWD_STRENGTH] = {DBAL_FIELD_FWD1_PRIORITY_DECODER_FWD_STRENGTH},
        [DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_RPF_STRENGTH] = {DBAL_FIELD_FWD1_PRIORITY_DECODER_RPF_STRENGTH},
        [DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_FWD_FOUND]    = {DBAL_FIELD_FWD1_PRIORITY_DECODER_FWD_FOUND},
        [DNX_FIELD_QUAL_FWD1_PRIORITY_DECODER_RPF_FOUND]    = {DBAL_FIELD_FWD1_PRIORITY_DECODER_RPF_FOUND},
        [DNX_FIELD_QUAL_STATISTICS_META_DATA]               = {DBAL_FIELD_STATISTICS_META_DATA},
        [DNX_FIELD_QUAL_FWD_STAT_ATTRIBUTE_OBJ_CMD]         = {DBAL_FIELD_FWD_STAT_ATTRIBUTE_OBJ_CMD},
        [DNX_FIELD_QUAL_FWD_STAT_ATTRIBUTE_OBJ]             = {DBAL_FIELD_FWD_STAT_ATTRIBUTE_OBJ},
        [DNX_FIELD_QUAL_VSI_UNKNOWN_DA_DESTINATION]         = {DBAL_FIELD_VSI_UNKNOWN_DA_DESTINATION},
        [DNX_FIELD_QUAL_ECMP_LB_KEY_0]                      = {DBAL_FIELD_ECMP_LB_KEY_0},
        [DNX_FIELD_QUAL_ECMP_LB_KEY_1]                      = {DBAL_FIELD_ECMP_LB_KEY_1},
        [DNX_FIELD_QUAL_ECMP_LB_KEY_2]                      = {DBAL_FIELD_ECMP_LB_KEY_2},
        [DNX_FIELD_QUAL_LAG_LB_KEY]                         = {DBAL_FIELD_LAG_LB_KEY},
        [DNX_FIELD_QUAL_NWK_LB_KEY]                         = {DBAL_FIELD_NWK_LB_KEY},
        [DNX_FIELD_QUAL_VRF_VALUE]                          = {DBAL_FIELD_VRF_VALUE},
        [DNX_FIELD_QUAL_UNKNOWN_ADDRESS]                    = {DBAL_FIELD_UNKNOWN_ADDRESS},
        [DNX_FIELD_QUAL_INGRESS_PROTOCOL_TRAP_IN_LIF_PROFILE] = {DBAL_FIELD_INGRESS_PROTOCOL_TRAP_IN_LIF_PROFILE},
        [DNX_FIELD_QUAL_VW_VIP_VALID]                       = {DBAL_FIELD_VW_VIP_VALID},
        [DNX_FIELD_QUAL_VW_VIP_ID]                          = {DBAL_FIELD_VW_VIP_ID},
        [DNX_FIELD_QUAL_VW_MEMBER_REFERENCE]                = {DBAL_FIELD_VW_MEMBER_REFERENCE},
        [DNX_FIELD_QUAL_VW_PCC_HIT]                         = {DBAL_FIELD_VW_PCC_HIT},
        [DNX_FIELD_QUAL_AC_IN_LIF_WIDE_DATA_EXTENDED]       = {DBAL_FIELD_AC_IN_LIF_WIDE_DATA_EXTENDED},
        [DNX_FIELD_QUAL_MACT_ENTRY_GROUPING]                = {DBAL_FIELD_MACT_ENTRY_GROUPING},

};

static const dnx_field_meta_qual_info_t dnx_ipmf1_meta_qual_info[] = {
        [DNX_FIELD_QUAL_ALL_ONES]                         = {DBAL_FIELD_ALL_ONES},
        [DNX_FIELD_QUAL_MEM_SOFT_ERR]                     = {DBAL_FIELD_MEM_SOFT_ERR},
        [DNX_FIELD_QUAL_ACL_CONTEXT]                      = {DBAL_FIELD_ACL_CONTEXT},
        [DNX_FIELD_QUAL_NASID]                            = {DBAL_FIELD_NASID},
        [DNX_FIELD_QUAL_FWD_CONTEXT]                      = {DBAL_FIELD_FWD_CONTEXT},
        [DNX_FIELD_QUAL_RPF_OUT_LIF]                      = {DBAL_FIELD_RPF_OUT_LIF},
        [DNX_FIELD_QUAL_RPF_DEFAULT_ROUTE_FOUND]          = {DBAL_FIELD_RPF_DEFAULT_ROUTE_FOUND},
        [DNX_FIELD_QUAL_RPF_ROUTE_VALID]                  = {DBAL_FIELD_RPF_ROUTE_VALID},
        [DNX_FIELD_QUAL_PACKET_IS_APPLET]                 = {DBAL_FIELD_IS_APPLET},
        [DNX_FIELD_QUAL_GENERAL_DATA_2]                   = {DBAL_FIELD_GENERAL_DATA_2},
        [DNX_FIELD_QUAL_GENERAL_DATA_1]                   = {DBAL_FIELD_GENERAL_DATA_1},
        [DNX_FIELD_QUAL_GENERAL_DATA_0]                   = {DBAL_FIELD_GENERAL_DATA_0},
        [DNX_FIELD_QUAL_FWD_DOMAIN]                       = {DBAL_FIELD_FWD_DOMAIN},
        /* Domain profile should not be exposed, exposed only pmf bits in pmf */
        [DNX_FIELD_QUAL_FWD_DOMAIN_PROFILE]               = {DBAL_FIELD_FWD_DOMAIN_PROFILE},
        [DNX_FIELD_QUAL_PMF_ETH_RIF_PROFILE]              = {DBAL_FIELD_PMF_ETH_RIF_PROFILE},
        /* Separate fields inside VLAN Edit Data */
        [DNX_FIELD_QUAL_VLAN_EDIT_PCP_2]                  = {DBAL_FIELD_VLAN_EDIT_PCP_2},
        [DNX_FIELD_QUAL_VLAN_EDIT_DEI_2]                  = {DBAL_FIELD_VLAN_EDIT_DEI_2},
        [DNX_FIELD_QUAL_VLAN_EDIT_PCP_DEI_2]              = {DBAL_FIELD_VLAN_EDIT_PCP_DEI_2},
        [DNX_FIELD_QUAL_VLAN_EDIT_VID_2]                  = {DBAL_FIELD_VLAN_EDIT_VID_2},
        [DNX_FIELD_QUAL_VLAN_EDIT_PCP_1]                  = {DBAL_FIELD_VLAN_EDIT_PCP_1},
        [DNX_FIELD_QUAL_VLAN_EDIT_DEI_1]                  = {DBAL_FIELD_VLAN_EDIT_DEI_1},
        [DNX_FIELD_QUAL_VLAN_EDIT_PCP_DEI_1]              = {DBAL_FIELD_VLAN_EDIT_PCP_DEI_1},
        [DNX_FIELD_QUAL_VLAN_EDIT_VID_1]                  = {DBAL_FIELD_VLAN_EDIT_VID_1},
        [DNX_FIELD_QUAL_VLAN_EDIT_CMD_INDEX]              = {DBAL_FIELD_VLAN_EDIT_CMD_INDEX},
        /* Data(39) contains PCP2(3) DEI2(1), VID2(12), PCP1(3) DEI1(1), VID1(12), CMD(7) <- LSB */
        [DNX_FIELD_QUAL_VLAN_EDIT_CMD]                    = {DBAL_FIELD_VLAN_EDIT_CMD},
        [DNX_FIELD_QUAL_SRC_SYS_PORT]                     = {DBAL_FIELD_SRC_SYS_PORT},
        [DNX_FIELD_QUAL_SNOOP_CODE]                       = {DBAL_FIELD_SNOOP_CODE},
        [DNX_FIELD_QUAL_SNOOP_STRENGTH]                   = {DBAL_FIELD_SNOOP_STRENGTH},
        [DNX_FIELD_QUAL_FWD_LAYER_ADDITIONAL_INFO]        = {DBAL_FIELD_FWD_LAYER_ADDITIONAL_INFO},
        [DNX_FIELD_QUAL_RPF_DST]                          = {DBAL_FIELD_RPF_DST},
        [DNX_FIELD_QUAL_RPF_DST_VALID]                    = {DBAL_FIELD_RPF_DST_VALID},
        [DNX_FIELD_QUAL_PACKET_IS_IEEE1588]               = {DBAL_FIELD_PACKET_IS_IEEE1588},
        [DNX_FIELD_QUAL_IEEE1588_COMPENSATE_TIME_STAMP]   = {DBAL_FIELD_IEEE1588_COMPENSATE_TIME_STAMP},
        [DNX_FIELD_QUAL_IEEE1588_COMMAND]                 = {DBAL_FIELD_IEEE1588_COMMAND},
        [DNX_FIELD_QUAL_IEEE1588_ENCAPSULATION]           = {DBAL_FIELD_IEEE1588_ENCAPSULATION},
        [DNX_FIELD_QUAL_IEEE1588_HEADER_OFFSET]           = {DBAL_FIELD_IEEE1588_HEADER_OFFSET},
        [DNX_FIELD_QUAL_PACKET_IS_COMPATIBLE_MC]          = {DBAL_FIELD_PACKET_IS_COMPATIBLE_MC},
        [DNX_FIELD_QUAL_PRT_QUALIFIER]                    = {DBAL_FIELD_PRT_QUALIFIER_RAW},
        [DNX_FIELD_QUAL_PACKET_HEADER_SIZE]               = {DBAL_FIELD_PACKET_HEADER_SIZE},
        [DNX_FIELD_QUAL_MIRROR_CODE]                      = {DBAL_FIELD_MIRROR_CODE},
        [DNX_FIELD_QUAL_VISIBILITY]                       = {DBAL_FIELD_VISIBILITY},
        [DNX_FIELD_QUAL_OAM_ID]                           = {DBAL_FIELD_PMF_OAM_ID},
        [DNX_FIELD_QUAL_OAM_UP_MEP]                       = {DBAL_FIELD_OAM_UP_MEP},
        [DNX_FIELD_QUAL_OAM_SUB_TYPE]                     = {DBAL_FIELD_OAM_SUB_TYPE},
        [DNX_FIELD_QUAL_OAM_OFFSET]                       = {DBAL_FIELD_OAM_OFFSET},
        [DNX_FIELD_QUAL_OAM_STAMP_OFFSET]                 = {DBAL_FIELD_OAM_STAMP_OFFSET},
        [DNX_FIELD_QUAL_LEARN_INFO]                       = {DBAL_FIELD_LEARN_INFO},
        [DNX_FIELD_QUAL_INGRESS_LEARN_ENABLE]             = {DBAL_FIELD_INGRESS_LEARN_ENABLE},
        [DNX_FIELD_QUAL_EGRESS_LEARN_ENABLE]              = {DBAL_FIELD_EGRESS_LEARN_ENABLE},
        [DNX_FIELD_QUAL_GLOB_IN_LIF_1]                    = {DBAL_FIELD_GLOB_IN_LIF_1},
        [DNX_FIELD_QUAL_GLOB_IN_LIF_0]                    = {DBAL_FIELD_GLOB_IN_LIF_0},
        [DNX_FIELD_QUAL_IN_LIF_PROFILE_1]                 = {DBAL_FIELD_IN_LIF_PROFILE_1},
        [DNX_FIELD_QUAL_IN_LIF_PROFILE_0]                 = {DBAL_FIELD_IN_LIF_PROFILE_0},
        [DNX_FIELD_QUAL_FWD_ACTION_STRENGTH]              = {DBAL_FIELD_FWD_ACTION_STRENGTH},
        [DNX_FIELD_QUAL_FWD_ACTION_DESTINATION]           = {DBAL_FIELD_FWD_ACTION_DESTINATION},
        [DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_CODE]         = {DBAL_FIELD_FWD_ACTION_CPU_TRAP_CODE},
        [DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_QUAL]         = {DBAL_FIELD_FWD_ACTION_CPU_TRAP_QUAL},
        [DNX_FIELD_QUAL_EEI]                              = {DBAL_FIELD_EEI},
        [DNX_FIELD_QUAL_ELK_LKP_HIT_0]                    = {DBAL_FIELD_ELK_LKP_HIT_0},
        [DNX_FIELD_QUAL_ELK_LKP_HIT_1]                    = {DBAL_FIELD_ELK_LKP_HIT_1},
        [DNX_FIELD_QUAL_ELK_LKP_HIT_2]                    = {DBAL_FIELD_ELK_LKP_HIT_2},
        [DNX_FIELD_QUAL_ELK_LKP_HIT_3]                    = {DBAL_FIELD_ELK_LKP_HIT_3},
        [DNX_FIELD_QUAL_ELK_LKP_HIT_4]                    = {DBAL_FIELD_ELK_LKP_HIT_4},
        [DNX_FIELD_QUAL_ELK_LKP_HIT_5]                    = {DBAL_FIELD_ELK_LKP_HIT_5},
        [DNX_FIELD_QUAL_ELK_LKP_HIT_6]                    = {DBAL_FIELD_ELK_LKP_HIT_6},
        [DNX_FIELD_QUAL_ELK_LKP_HIT_7]                    = {DBAL_FIELD_ELK_LKP_HIT_7},
        [DNX_FIELD_QUAL_EXTERNAL_RESULT_BUFFER_PART_1]    = {DBAL_FIELD_ELK_LKP_PAYLOAD_ALL_PART_1},
        [DNX_FIELD_QUAL_EXTERNAL_RESULT_BUFFER_PART_0]    = {DBAL_FIELD_ELK_LKP_PAYLOAD_ALL_PART_0},
        /* KBP results were taken out to an input type to be used by user defined qualifiers */
        [DNX_FIELD_QUAL_IN_PORT]                          = {DBAL_FIELD_IN_PORT},
        [DNX_FIELD_QUAL_PP_PORT]                          = {DBAL_FIELD_PP_PORT},
        [DNX_FIELD_QUAL_PTC]                              = {DBAL_FIELD_PTC},
        [DNX_FIELD_QUAL_LAG_LB_KEY]                       = {DBAL_FIELD_LAG_LB_KEY},
        [DNX_FIELD_QUAL_NWK_LB_KEY]                       = {DBAL_FIELD_NWK_LB_KEY},
        [DNX_FIELD_QUAL_ECMP_LB_KEY_2]                    = {DBAL_FIELD_ECMP_LB_KEY_2},
        [DNX_FIELD_QUAL_ECMP_LB_KEY_1]                    = {DBAL_FIELD_ECMP_LB_KEY_1},
        [DNX_FIELD_QUAL_ECMP_LB_KEY_0]                    = {DBAL_FIELD_ECMP_LB_KEY_0},
        [DNX_FIELD_QUAL_GLOB_OUT_LIF_1]                   = {DBAL_FIELD_GLOB_OUT_LIF_1},
        [DNX_FIELD_QUAL_GLOB_OUT_LIF_0]                   = {DBAL_FIELD_GLOB_OUT_LIF_0},
        [DNX_FIELD_QUAL_IN_TTL]                           = {DBAL_FIELD_IN_TTL},
        [DNX_FIELD_QUAL_BIER_STR_OFFSET]                  = {DBAL_FIELD_BIER_STR_OFFSET},
        [DNX_FIELD_QUAL_BIER_STR_SIZE]                    = {DBAL_FIELD_BIER_STR_SIZE},
        [DNX_FIELD_QUAL_PACKET_IS_BIER]                   = {DBAL_FIELD_PACKET_IS_BIER},
        [DNX_FIELD_QUAL_SAVED_CONTEXT_PROFILE]            = {DBAL_FIELD_SAVED_CONTEXT_PROFILE},
        [DNX_FIELD_QUAL_OAM_METER_DISABLE]                = {DBAL_FIELD_OAM_METER_DISABLE},
        [DNX_FIELD_QUAL_INCOMING_TAG_STRUCTURE]           = {DBAL_FIELD_INCOMING_TAG_STRUCTURE},
        [DNX_FIELD_QUAL_TC]                               = {DBAL_FIELD_TC},
        [DNX_FIELD_QUAL_DP]                               = {DBAL_FIELD_DP},
        [DNX_FIELD_QUAL_ECN]                              = {DBAL_FIELD_METADATA_ECN},
        [DNX_FIELD_QUAL_NWK_QOS]                          = {DBAL_FIELD_NWK_QOS},
        [DNX_FIELD_QUAL_TRACE_PACKET]                     = {DBAL_FIELD_TRACE_PACKET},
        [DNX_FIELD_QUAL_PACKET_HEADER_SIZE_RANGE]         = {DBAL_FIELD_PACKET_HEADER_SIZE_RANGE},
        [DNX_FIELD_QUAL_IN_PORT_KEY_GEN_VAR]              = {DBAL_FIELD_IN_PORT_PMF_DATA},
        [DNX_FIELD_QUAL_IN_PORT_MAPPED_PP_PORT]           = {DBAL_FIELD_IN_PORT_MAPPED_PP_PORT},
        [DNX_FIELD_QUAL_PTC_KEY_GEN_VAR]                  = {DBAL_FIELD_PTC_PMF_DATA},
        [DNX_FIELD_QUAL_VID_VALID]                        = {DBAL_FIELD_VID_VALID},
        [DNX_FIELD_QUAL_L4OPS]                            = {DBAL_FIELD_L4OPS},
        [DNX_FIELD_QUAL_ENCODER0]                   = {DBAL_FIELD_L4OPS_ENCODER0},
        [DNX_FIELD_QUAL_ENCODER1]                   = {DBAL_FIELD_L4OPS_ENCODER1},
        [DNX_FIELD_QUAL_ENCODER2]                   = {DBAL_FIELD_L4OPS_ENCODER2},
        [DNX_FIELD_QUAL_ENCODER3]                   = {DBAL_FIELD_L4OPS_ENCODER3},
        [DNX_FIELD_QUAL_STATISTICS_OBJECT10]              = {DBAL_FIELD_STATISTICS_OBJECT_10},
        [DNX_FIELD_QUAL_CPU_TRAP_CODE_PROFILE]            = {DBAL_FIELD_CPU_TRAP_CODE_PROFILE},
        /* All IDs are together represented by IDc Vector DNX_FIELD_QUAL_STATISTIC_ID_VEC */
        [DNX_FIELD_QUAL_TM_FLOW_ID]                       = {DBAL_FIELD_TM_FLOW_ID},
        [DNX_FIELD_QUAL_EXT_STATISTICS_ID_3]              = {DBAL_FIELD_EXT_STATISTICS_ID_3},
        [DNX_FIELD_QUAL_EXT_STATISTICS_ID_2]              = {DBAL_FIELD_EXT_STATISTICS_ID_2},
        [DNX_FIELD_QUAL_EXT_STATISTICS_ID_1]              = {DBAL_FIELD_EXT_STATISTICS_ID_1},
        [DNX_FIELD_QUAL_EXT_STATISTICS_ID_0]              = {DBAL_FIELD_EXT_STATISTICS_ID_0},
        [DNX_FIELD_QUAL_STATISTICS_ID_9]                  = {DBAL_FIELD_STATISTICS_ID_9},
        [DNX_FIELD_QUAL_STATISTICS_ID_8]                  = {DBAL_FIELD_STATISTICS_ID_8},
        [DNX_FIELD_QUAL_STATISTICS_ID_7]                  = {DBAL_FIELD_STATISTICS_ID_7},
        [DNX_FIELD_QUAL_STATISTICS_ID_6]                  = {DBAL_FIELD_STATISTICS_ID_6},
        [DNX_FIELD_QUAL_STATISTICS_ID_5]                  = {DBAL_FIELD_STATISTICS_ID_5},
        [DNX_FIELD_QUAL_STATISTICS_ID_4]                  = {DBAL_FIELD_STATISTICS_ID_4},
        [DNX_FIELD_QUAL_STATISTICS_ID_3]                  = {DBAL_FIELD_STATISTICS_ID_3},
        [DNX_FIELD_QUAL_STATISTICS_ID_2]                  = {DBAL_FIELD_STATISTICS_ID_2},
        [DNX_FIELD_QUAL_STATISTICS_ID_1]                  = {DBAL_FIELD_STATISTICS_ID_1},
        [DNX_FIELD_QUAL_STATISTICS_ID_0]                  = {DBAL_FIELD_STATISTICS_ID_0},
        /* All attributes are constructed as following {valid,Type(2),Is_LM(1),Is_Meter(1)}  <-- lsb */
        /* All Atrs are together represented by IDc Vector DNX_FIELD_QUAL_STATISTIC_ATR_VEC */
        [DNX_FIELD_QUAL_TM_FLOW_ATR]                      = {DBAL_FIELD_TM_FLOW_ATR},
        [DNX_FIELD_QUAL_EXT_STATISTICS_ATR_3]             = {DBAL_FIELD_EXT_STATISTICS_ATR_3},
        [DNX_FIELD_QUAL_EXT_STATISTICS_ATR_2]             = {DBAL_FIELD_EXT_STATISTICS_ATR_2},
        [DNX_FIELD_QUAL_EXT_STATISTICS_ATR_1]             = {DBAL_FIELD_EXT_STATISTICS_ATR_1},
        [DNX_FIELD_QUAL_EXT_STATISTICS_ATR_0]             = {DBAL_FIELD_EXT_STATISTICS_ATR_0},
        [DNX_FIELD_QUAL_STATISTICS_ATR_9]                 = {DBAL_FIELD_STATISTICS_ATR_9},
        [DNX_FIELD_QUAL_STATISTICS_ATR_8]                 = {DBAL_FIELD_STATISTICS_ATR_8},
        [DNX_FIELD_QUAL_STATISTICS_ATR_7]                 = {DBAL_FIELD_STATISTICS_ATR_7},
        [DNX_FIELD_QUAL_STATISTICS_ATR_6]                 = {DBAL_FIELD_STATISTICS_ATR_6},
        [DNX_FIELD_QUAL_STATISTICS_ATR_5]                 = {DBAL_FIELD_STATISTICS_ATR_5},
        [DNX_FIELD_QUAL_STATISTICS_ATR_4]                 = {DBAL_FIELD_STATISTICS_ATR_4},
        [DNX_FIELD_QUAL_STATISTICS_ATR_3]                 = {DBAL_FIELD_STATISTICS_ATR_3},
        [DNX_FIELD_QUAL_STATISTICS_ATR_2]                 = {DBAL_FIELD_STATISTICS_ATR_2},
        [DNX_FIELD_QUAL_STATISTICS_ATR_1]                 = {DBAL_FIELD_STATISTICS_ATR_1},
        [DNX_FIELD_QUAL_STATISTICS_ATR_0]                 = {DBAL_FIELD_STATISTICS_ATR_0},
        [DNX_FIELD_QUAL_STATISTICS_META_DATA]             = {DBAL_FIELD_STATISTICS_META_DATA},
        [DNX_FIELD_QUAL_CONTEXT_KEY_GEN_VAR]              = {DBAL_FIELD_CONTEXT_KEY_GEN_VAR},
        [DNX_FIELD_QUAL_EGRESS_PARSING_INDEX]             = {DBAL_FIELD_EGRESS_PARSING_INDEX},
        [DNX_FIELD_QUAL_FWD_LAYER_INDEX]                  = {DBAL_FIELD_FWD_LAYER_INDEX},
        [DNX_FIELD_QUAL_AC_IN_LIF_WIDE_DATA]              = {DBAL_FIELD_AC_IN_LIF_WIDE_DATA},
        [DNX_FIELD_QUAL_NATIVE_AC_IN_LIF_WIDE_DATA]       = {DBAL_FIELD_NATIVE_AC_IN_LIF_WIDE_DATA},
        [DNX_FIELD_QUAL_PWE_IN_LIF_WIDE_DATA]             = {DBAL_FIELD_PWE_IN_LIF_WIDE_DATA},
        [DNX_FIELD_QUAL_IP_TUNNEL_IN_LIF_WIDE_DATA]       = {DBAL_FIELD_IP_TUNNEL_IN_LIF_WIDE_DATA},
        [DNX_FIELD_QUAL_PP_PORT_PMF_PROFILE]              = {DBAL_FIELD_PP_PORT_PMF_PROFILE},
        [DNX_FIELD_QUAL_TM_PORT_PMF_PROFILE]              = {DBAL_FIELD_TM_PORT_PMF_PROFILE},
        [DNX_FIELD_QUAL_PP_PORT_PMF_GENERAL_DATA]         = {DBAL_FIELD_PP_PORT_PMF_GENERAL_DATA},
        [DNX_FIELD_QUAL_UNKNOWN_ADDRESS]                  = {DBAL_FIELD_UNKNOWN_ADDRESS},
        [DNX_FIELD_QUAL_LEARN_SRC_MAC]                    = {DBAL_FIELD_LEARN_SRC_MAC},
        [DNX_FIELD_QUAL_LEARN_VLAN]                       = {DBAL_FIELD_LEARN_VLAN},
        [DNX_FIELD_QUAL_LEARN_VSI]                        = {DBAL_FIELD_LEARN_VSI},
        [DNX_FIELD_QUAL_LEARN_DATA]                       = {DBAL_FIELD_LEARN_DATA},
        [DNX_FIELD_QUAL_LEARN_STATION_MOVE]               = {DBAL_FIELD_LEARN_STATION_MOVE},
        [DNX_FIELD_QUAL_LEARN_MATCH]                      = {DBAL_FIELD_LEARN_MATCH},
        [DNX_FIELD_QUAL_LEARN_FOUND]                      = {DBAL_FIELD_LEARN_FOUND},
        [DNX_FIELD_QUAL_LEARN_EXPECTED_WON]               = {DBAL_FIELD_LEARN_EXPECTED_WON},
        [DNX_FIELD_QUAL_VRF_VALUE]                        = {DBAL_FIELD_VRF_VALUE},
        [DNX_FIELD_QUAL_CORE_ID]                          = {DBAL_FIELD_CORE},
        [DNX_FIELD_QUAL_VW_VIP_VALID]                = {DBAL_FIELD_VW_VIP_VALID},
        [DNX_FIELD_QUAL_VW_VIP_ID]                 = {DBAL_FIELD_VW_VIP_ID},
        [DNX_FIELD_QUAL_VW_MEMBER_REFERENCE]              = {DBAL_FIELD_VW_MEMBER_REFERENCE},
        [DNX_FIELD_QUAL_VW_PCC_HIT]                 = {DBAL_FIELD_VW_PCC_HIT},
        [DNX_FIELD_QUAL_AC_IN_LIF_WIDE_DATA_EXTENDED]     = {DBAL_FIELD_AC_IN_LIF_WIDE_DATA_EXTENDED},
        [DNX_FIELD_QUAL_MACT_ENTRY_GROUPING]              = {DBAL_FIELD_MACT_ENTRY_GROUPING},
};

static const dnx_field_meta_qual_info_t dnx_ipmf2_meta_qual_info[] = {
    [DNX_FIELD_QUAL_PMF1_TCAM_ACTION_0]               = {DBAL_FIELD_PMF1_TCAM_ACTION_0},
    [DNX_FIELD_QUAL_PMF1_TCAM_ACTION_1]               = {DBAL_FIELD_PMF1_TCAM_ACTION_1},
    [DNX_FIELD_QUAL_PMF1_TCAM_ACTION_2]               = {DBAL_FIELD_PMF1_TCAM_ACTION_2},
    [DNX_FIELD_QUAL_PMF1_TCAM_ACTION_3]               = {DBAL_FIELD_PMF1_TCAM_ACTION_3},
    [DNX_FIELD_QUAL_PMF1_EXEM_ACTION]                 = {DBAL_FIELD_PMF1_EXEM_ACTION},
    [DNX_FIELD_QUAL_PMF1_DIRECT_ACTION]               = {DBAL_FIELD_PMF1_DIRECT_ACTION},
    /*
     * For non atomic read modify write state table, STATE_TABLE_DATA means data read, unless only write took place, in which case data written.
     * For atomic read modify write state table, STATE_TABLE_DATA means data read only, and we use the address to write the data written.
     */
    [DNX_FIELD_QUAL_STATE_DATA]                       = {DBAL_FIELD_STATE_TABLE_DATA_SW},
    [DNX_FIELD_QUAL_STATE_DATA_WRITE]                 = {DBAL_FIELD_STATE_TABLE_DATA_WRITE},
    [DNX_FIELD_QUAL_STATE_ADDRESS]                    = {DBAL_FIELD_STATE_TABLE_ADDRESS_HW},
    [DNX_FIELD_QUAL_KEY_F_INITIAL]                    = {DBAL_FIELD_KEY_F_INITIAL},
    [DNX_FIELD_QUAL_KEY_G_INITIAL]                    = {DBAL_FIELD_KEY_G_INITIAL},
    [DNX_FIELD_QUAL_KEY_H_INITIAL]                    = {DBAL_FIELD_KEY_H_INITIAL},
    [DNX_FIELD_QUAL_KEY_I_INITIAL]                    = {DBAL_FIELD_KEY_I_INITIAL},
    [DNX_FIELD_QUAL_KEY_J_INITIAL]                    = {DBAL_FIELD_KEY_J_INITIAL},
    [DNX_FIELD_QUAL_LAG_LB_KEY]                       = {DBAL_FIELD_LAG_LB_KEY},
    [DNX_FIELD_QUAL_NWK_LB_KEY]                       = {DBAL_FIELD_NWK_LB_KEY},
    [DNX_FIELD_QUAL_ECMP_LB_KEY_2]                    = {DBAL_FIELD_ECMP_LB_KEY_2},
    [DNX_FIELD_QUAL_ECMP_LB_KEY_1]                    = {DBAL_FIELD_ECMP_LB_KEY_1},
    [DNX_FIELD_QUAL_ECMP_LB_KEY_0]                    = {DBAL_FIELD_ECMP_LB_KEY_0},
    [DNX_FIELD_QUAL_TRJ_HASH]                         = {DBAL_FIELD_TRJ_HASH},
    [DNX_FIELD_QUAL_CMP_KEY_0_DECODED]                = {DBAL_FIELD_CMP_KEY_0_DECODED},
    [DNX_FIELD_QUAL_CMP_KEY_1_DECODED]                = {DBAL_FIELD_CMP_KEY_1_DECODED},
    [DNX_FIELD_QUAL_CMP_KEY_TCAM_0_DECODED]           = {DBAL_FIELD_CMP_KEY_TCAM_0_DECODED},
    [DNX_FIELD_QUAL_CMP_KEY_TCAM_1_DECODED]           = {DBAL_FIELD_CMP_KEY_TCAM_1_DECODED},
    [DNX_FIELD_QUAL_NASID]                            = {DBAL_FIELD_NASID},
};

static const dnx_field_meta_qual_info_t dnx_ipmf3_meta_qual_info[] = {
        [DNX_FIELD_QUAL_ALL_ONES]                         = {DBAL_FIELD_ALL_ONES},
        [DNX_FIELD_QUAL_LEARN_INFO]                       = {DBAL_FIELD_LEARN_INFO},
        [DNX_FIELD_QUAL_RPF_ECMP_MODE]                    = {DBAL_FIELD_RPF_ECMP_MODE},
        [DNX_FIELD_QUAL_RPF_ECMP_GROUP]                   = {DBAL_FIELD_RPF_ECMP_GROUP},
        [DNX_FIELD_QUAL_ADMT_PROFILE]                     = {DBAL_FIELD_ADMT_PROFILE},
        [DNX_FIELD_QUAL_BIER_STR_OFFSET]                  = {DBAL_FIELD_BIER_STR_OFFSET},
        [DNX_FIELD_QUAL_BIER_STR_SIZE]                    = {DBAL_FIELD_BIER_STR_SIZE},
        [DNX_FIELD_QUAL_BYTES_TO_REMOVE]                  = {DBAL_FIELD_BYTES_TO_REMOVE},
        [DNX_FIELD_QUAL_WEAK_TM_VALID]                    = {DBAL_FIELD_WEAK_TM_VALID},
        [DNX_FIELD_QUAL_WEAK_TM_PROFILE]                  = {DBAL_FIELD_WEAK_TM_PROFILE},
        [DNX_FIELD_QUAL_STAT_OBJ_LM_READ_INDEX]           = {DBAL_FIELD_STAT_OBJ_LM_READ_INDEX},
        [DNX_FIELD_QUAL_CONGESTION_INFO]                  = {DBAL_FIELD_CONGESTION_INFO},
        [DNX_FIELD_QUAL_END_OF_PACKET_EDITING]            = {DBAL_FIELD_END_OF_PACKET_EDITING},
        [DNX_FIELD_QUAL_EEI]                              = {DBAL_FIELD_EEI},
        [DNX_FIELD_QUAL_EGRESS_LEARN_ENABLE]              = {DBAL_FIELD_EGRESS_LEARN_ENABLE},
        [DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_CODE]         = {DBAL_FIELD_FWD_ACTION_CPU_TRAP_CODE},
        [DNX_FIELD_QUAL_FWD_ACTION_CPU_TRAP_QUAL]         = {DBAL_FIELD_FWD_ACTION_CPU_TRAP_QUAL},
        [DNX_FIELD_QUAL_FWD_ACTION_DP_METER_COMMAND]      = {DBAL_FIELD_DP_METER_COMMAND},
        [DNX_FIELD_QUAL_FWD_ACTION_DP]                    = {DBAL_FIELD_DP},
        [DNX_FIELD_QUAL_FWD_ACTION_TC]                    = {DBAL_FIELD_TC},
        [DNX_FIELD_QUAL_FWD_ACTION_DESTINATION]           = {DBAL_FIELD_FWD_ACTION_DESTINATION},
        [DNX_FIELD_QUAL_FWD_ACTION_STRENGTH]              = {DBAL_FIELD_FWD_ACTION_STRENGTH},
        /* Above field are subfields of FWD_PAYLOAD */
        [DNX_FIELD_QUAL_FWD_PAYLOAD]                      = {DBAL_FIELD_FWD_PAYLOAD},
        [DNX_FIELD_QUAL_FWD_DOMAIN]                       = {DBAL_FIELD_FWD_DOMAIN},
        /* Domain profile should not be exposed, exposed only pmf bits in pmf */
        [DNX_FIELD_QUAL_FWD_DOMAIN_PROFILE]               = {DBAL_FIELD_FWD_DOMAIN_PROFILE},
        [DNX_FIELD_QUAL_PMF_ETH_RIF_PROFILE]              = {DBAL_FIELD_PMF_ETH_RIF_PROFILE},
        [DNX_FIELD_QUAL_GENERAL_DATA_PMF_CONTAINER]       = {DBAL_FIELD_PEM_GENERAL_DATA3},
        [DNX_FIELD_QUAL_GENERAL_DATA]                     = {DBAL_FIELD_GENERAL_DATA_IPMF3},
        [DNX_FIELD_QUAL_IEEE1588_COMMAND]                 = {DBAL_FIELD_IEEE1588_COMMAND},
        [DNX_FIELD_QUAL_IEEE1588_COMPENSATE_TIME_STAMP]   = {DBAL_FIELD_IEEE1588_COMPENSATE_TIME_STAMP},
        [DNX_FIELD_QUAL_IEEE1588_ENCAPSULATION]           = {DBAL_FIELD_IEEE1588_ENCAPSULATION},
        [DNX_FIELD_QUAL_IEEE1588_HEADER_OFFSET]           = {DBAL_FIELD_IEEE1588_HEADER_OFFSET},
        [DNX_FIELD_QUAL_IN_LIF_PROFILE_1]                 = {DBAL_FIELD_IN_LIF_PROFILE_1},
        [DNX_FIELD_QUAL_IN_LIF_PROFILE_0]                 = {DBAL_FIELD_IN_LIF_PROFILE_0},
        [DNX_FIELD_QUAL_GLOB_IN_LIF_1]                    = {DBAL_FIELD_GLOB_IN_LIF_1},
        [DNX_FIELD_QUAL_GLOB_IN_LIF_0]                    = {DBAL_FIELD_GLOB_IN_LIF_0},
         /**LSB {core_id,port} MSB*/
        [DNX_FIELD_QUAL_IN_PORT]                          = {DBAL_FIELD_IN_PORT},
        [DNX_FIELD_QUAL_PP_PORT]                          = {DBAL_FIELD_PP_PORT},
        [DNX_FIELD_QUAL_CORE_ID]                          = {DBAL_FIELD_CORE},
        [DNX_FIELD_QUAL_IN_TTL]                           = {DBAL_FIELD_IN_TTL},
        [DNX_FIELD_QUAL_INGRESS_LEARN_ENABLE]             = {DBAL_FIELD_INGRESS_LEARN_ENABLE},
        [DNX_FIELD_QUAL_ITPP_DELTA]                       = {DBAL_FIELD_ITPP_DELTA},
        [DNX_FIELD_QUAL_LAG_LB_KEY]                       = {DBAL_FIELD_LAG_LB_KEY},
        [DNX_FIELD_QUAL_LATENCY_FLOW_ID_VALID]            = {DBAL_FIELD_LATENCY_FLOW_ID_VALID},
        [DNX_FIELD_QUAL_LATENCY_FLOW_ID]                  = {DBAL_FIELD_LATENCY_FLOW_ID},
        [DNX_FIELD_QUAL_MIRROR_CODE]                      = {DBAL_FIELD_MIRROR_CODE},
        [DNX_FIELD_QUAL_MIRROR_QUALIFIER]                 = {DBAL_FIELD_MIRROR_QUALIFIER},
        [DNX_FIELD_QUAL_NWK_LB_KEY]                       = {DBAL_FIELD_NWK_LB_KEY},
        [DNX_FIELD_QUAL_NWK_QOS]                          = {DBAL_FIELD_NWK_QOS},
        [DNX_FIELD_QUAL_OAM_ID]                           = {DBAL_FIELD_PMF_OAM_ID},
        [DNX_FIELD_QUAL_OAM_OFFSET]                       = {DBAL_FIELD_OAM_OFFSET},
        [DNX_FIELD_QUAL_OAM_STAMP_OFFSET]                 = {DBAL_FIELD_OAM_STAMP_OFFSET},
        [DNX_FIELD_QUAL_OAM_SUB_TYPE]                     = {DBAL_FIELD_OAM_SUB_TYPE},
        [DNX_FIELD_QUAL_OAM_UP_MEP]                       = {DBAL_FIELD_OAM_UP_MEP},
        [DNX_FIELD_QUAL_GLOB_OUT_LIF_0]                   = {DBAL_FIELD_GLOB_OUT_LIF_0},
        [DNX_FIELD_QUAL_GLOB_OUT_LIF_1]                   = {DBAL_FIELD_GLOB_OUT_LIF_1},
        [DNX_FIELD_QUAL_GLOB_OUT_LIF_2]                   = {DBAL_FIELD_GLOB_OUT_LIF_2},
        [DNX_FIELD_QUAL_GLOB_OUT_LIF_3]                   = {DBAL_FIELD_GLOB_OUT_LIF_3},
        [DNX_FIELD_QUAL_PACKET_IS_APPLET]                 = {DBAL_FIELD_IS_APPLET},
        [DNX_FIELD_QUAL_NASID]                            = {DBAL_FIELD_NASID},
        [DNX_FIELD_QUAL_PACKET_IS_BIER]                   = {DBAL_FIELD_PACKET_IS_BIER},
        [DNX_FIELD_QUAL_PACKET_IS_IEEE1588]               = {DBAL_FIELD_PACKET_IS_IEEE1588},
        [DNX_FIELD_QUAL_EGRESS_PARSING_INDEX]             = {DBAL_FIELD_EGRESS_PARSING_INDEX},
        [DNX_FIELD_QUAL_PTC]                              = {DBAL_FIELD_PTC},
        [DNX_FIELD_QUAL_EXT_STATISTICS_VALID]             = {DBAL_FIELD_EXT_STATISTICS_VALID},
        [DNX_FIELD_QUAL_PPH_RESERVED]                     = {DBAL_FIELD_PPH_RESERVED},
        [DNX_FIELD_QUAL_PPH_TYPE]                         = {DBAL_FIELD_PPH_TYPE},
        [DNX_FIELD_QUAL_RPF_DST]                          = {DBAL_FIELD_RPF_DST},
        [DNX_FIELD_QUAL_RPF_DST_VALID]                    = {DBAL_FIELD_RPF_DST_VALID},
        [DNX_FIELD_QUAL_SNOOP_CODE]                       = {DBAL_FIELD_SNOOP_CODE},
        [DNX_FIELD_QUAL_SNOOP_STRENGTH]                   = {DBAL_FIELD_SNOOP_STRENGTH},
        [DNX_FIELD_QUAL_SNOOP_QUALIFIER]                  = {DBAL_FIELD_SNOOP_QUALIFIER},
        [DNX_FIELD_QUAL_SRC_SYS_PORT]                     = {DBAL_FIELD_SRC_SYS_PORT},
        [DNX_FIELD_QUAL_ST_VSQ_PTR]                       = {DBAL_FIELD_ST_VSQ_PTR},
        [DNX_FIELD_QUAL_STATISTICAL_SAMPLING_CODE]        = {DBAL_FIELD_STATISTICAL_SAMPLING_CODE},
        [DNX_FIELD_QUAL_STATISTICAL_SAMPLING_QUALIFIER]   = {DBAL_FIELD_STATISTICAL_SAMPLING_QUALIFIER},
        [DNX_FIELD_QUAL_STACKING_ROUTE_HISTORY_BITMAP]    = {DBAL_FIELD_STACKING_ROUTE_HISTORY_BITMAP},
        [DNX_FIELD_QUAL_STATISTICS_META_DATA]             = {DBAL_FIELD_STATISTICS_META_DATA},
        [DNX_FIELD_QUAL_STATISTICS_OBJECT10]              = {DBAL_FIELD_STATISTICS_OBJECT_10},
        [DNX_FIELD_QUAL_SYSTEM_HEADER_PROFILE_INDEX]      = {DBAL_FIELD_SYSTEM_HEADER_PROFILE_INDEX},
        [DNX_FIELD_QUAL_TM_PROFILE]                       = {DBAL_FIELD_TM_PROFILE},
        [DNX_FIELD_QUAL_USER_HEADER_1]                    = {DBAL_FIELD_USER_HEADER_1},
        [DNX_FIELD_QUAL_USER_HEADER_2]                    = {DBAL_FIELD_USER_HEADER_2},
        [DNX_FIELD_QUAL_USER_HEADER_3]                    = {DBAL_FIELD_USER_HEADER_3},
        [DNX_FIELD_QUAL_USER_HEADER_4]                    = {DBAL_FIELD_USER_HEADER_4},
        [DNX_FIELD_QUAL_USER_HEADER_1_TYPE]               = {DBAL_FIELD_USER_HEADER_1_TYPE},
        [DNX_FIELD_QUAL_USER_HEADER_2_TYPE]               = {DBAL_FIELD_USER_HEADER_2_TYPE},
        [DNX_FIELD_QUAL_USER_HEADER_3_TYPE]               = {DBAL_FIELD_USER_HEADER_3_TYPE},
        [DNX_FIELD_QUAL_USER_HEADER_4_TYPE]               = {DBAL_FIELD_USER_HEADER_4_TYPE},
        /* This one include all four */
        [DNX_FIELD_QUAL_USER_HEADERS_TYPE]                = {DBAL_FIELD_USER_HEADERS_TYPE},
        [DNX_FIELD_QUAL_VISIBILITY]                       = {DBAL_FIELD_VISIBILITY},
        /* Separate fields inside VLAN Edit Data */
        [DNX_FIELD_QUAL_VLAN_EDIT_VID_2]                  = {DBAL_FIELD_VLAN_EDIT_VID_2},
        [DNX_FIELD_QUAL_VLAN_EDIT_VID_1]                  = {DBAL_FIELD_VLAN_EDIT_VID_1},
        [DNX_FIELD_QUAL_VLAN_EDIT_DEI_2]                  = {DBAL_FIELD_VLAN_EDIT_DEI_2},
        [DNX_FIELD_QUAL_VLAN_EDIT_PCP_2]                  = {DBAL_FIELD_VLAN_EDIT_PCP_2},
        [DNX_FIELD_QUAL_VLAN_EDIT_PCP_DEI_2]              = {DBAL_FIELD_VLAN_EDIT_PCP_DEI_2},
        [DNX_FIELD_QUAL_VLAN_EDIT_DEI_1]                  = {DBAL_FIELD_VLAN_EDIT_DEI_1},
        [DNX_FIELD_QUAL_VLAN_EDIT_PCP_1]                  = {DBAL_FIELD_VLAN_EDIT_PCP_1},
        [DNX_FIELD_QUAL_VLAN_EDIT_PCP_DEI_1]              = {DBAL_FIELD_VLAN_EDIT_PCP_DEI_1},
        [DNX_FIELD_QUAL_VLAN_EDIT_CMD_INDEX]              = {DBAL_FIELD_VLAN_EDIT_CMD_INDEX},
        /* Data(39) contains VID1(12), VID2(12), PCP_DEI1(4),  PCP_DEI2(4), CMD(7) */
        [DNX_FIELD_QUAL_VLAN_EDIT_CMD]                    = {DBAL_FIELD_VLAN_EDIT_CMD},
        [DNX_FIELD_QUAL_FWD_LAYER_ADDITIONAL_INFO]        = {DBAL_FIELD_FWD_LAYER_ADDITIONAL_INFO},
        [DNX_FIELD_QUAL_SLB_LEARN_NEEDED]                 = {DBAL_FIELD_SLB_LEARN_NEEDED},
        [DNX_FIELD_QUAL_SLB_KEY]                          = {DBAL_FIELD_SLB_KEY},
        [DNX_FIELD_QUAL_SLB_PAYLOAD]                      = {DBAL_FIELD_SLB_PAYLOAD},
        [DNX_FIELD_QUAL_PTC_KEY_GEN_VAR]                  = {DBAL_FIELD_PTC_PMF_DATA},
        [DNX_FIELD_QUAL_IN_PORT_KEY_GEN_VAR]              = {DBAL_FIELD_IN_PORT_PMF_DATA},
        [DNX_FIELD_QUAL_IN_PORT_MAPPED_PP_PORT]           = {DBAL_FIELD_IN_PORT_MAPPED_PP_PORT},
        [DNX_FIELD_QUAL_CONTEXT_KEY_GEN_VAR]              = {DBAL_FIELD_CONTEXT_KEY_GEN_VAR},
        /* {fer stat 2,fer stat 1,fer stat 0} <-- lsb  each one is 24 bits and arrives from one fer stage. */
        [DNX_FIELD_QUAL_FER_STATISTICS_OBJ]               = {DBAL_FIELD_FER_STATISTICS_OBJ},
        [DNX_FIELD_QUAL_FWD_LAYER_INDEX]                  = {DBAL_FIELD_FWD_LAYER_INDEX},
        [DNX_FIELD_QUAL_PP_PORT_PMF_PROFILE]              = {DBAL_FIELD_PP_PORT_PMF_PROFILE},
        [DNX_FIELD_QUAL_TM_PORT_PMF_PROFILE]              = {DBAL_FIELD_TM_PORT_PMF_PROFILE},
        [DNX_FIELD_QUAL_PP_PORT_PMF_GENERAL_DATA]         = {DBAL_FIELD_PP_PORT_PMF_GENERAL_DATA},
        [DNX_FIELD_QUAL_DUAL_QUEUE]                       = {DBAL_FIELD_DUAL_QUEUE},
        [DNX_FIELD_QUAL_ELEPHANT_TRAP_INFO]               = {DBAL_FIELD_ELEPHANT_TRAP_INFO},
        [DNX_FIELD_QUAL_ELEPHANT_LEARN_NEEDED]            = {DBAL_FIELD_ELEPHANT_LEARN_NEEDED},
        [DNX_FIELD_QUAL_PRT_QUALIFIER]                    = {DBAL_FIELD_PRT_QUALIFIER_RAW},
        [DNX_FIELD_QUAL_ECL_VALID]                        = {DBAL_FIELD_ECL_VALID},
        [DNX_FIELD_QUAL_BTK_OFFSET_SOP]                   = {DBAL_FIELD_BTK_OFFSET_SOP},
        [DNX_FIELD_QUAL_BTC_OFFSET_SOP]                   = {DBAL_FIELD_BTC_OFFSET_SOP},
        [DNX_FIELD_QUAL_BTC_MINUS_BTK]                    = {DBAL_FIELD_BTC_MINUS_BTK},
        [DNX_FIELD_QUAL_LEARN_SRC_MAC]                    = {DBAL_FIELD_LEARN_SRC_MAC},
        [DNX_FIELD_QUAL_LEARN_VLAN]                       = {DBAL_FIELD_LEARN_VLAN},
        [DNX_FIELD_QUAL_LEARN_VSI]                        = {DBAL_FIELD_LEARN_VSI},
        [DNX_FIELD_QUAL_LEARN_DATA]                       = {DBAL_FIELD_LEARN_DATA},
        [DNX_FIELD_QUAL_LEARN_STATION_MOVE]               = {DBAL_FIELD_LEARN_STATION_MOVE},
        [DNX_FIELD_QUAL_LEARN_MATCH]                      = {DBAL_FIELD_LEARN_MATCH},
        [DNX_FIELD_QUAL_LEARN_FOUND]                      = {DBAL_FIELD_LEARN_FOUND},
        [DNX_FIELD_QUAL_LEARN_EXPECTED_WON]               = {DBAL_FIELD_LEARN_EXPECTED_WON},
        [DNX_FIELD_QUAL_VRF_VALUE]                        = {DBAL_FIELD_VRF_VALUE},

};

static const dnx_field_meta_qual_info_t dnx_epmf_meta_qual_info[] = {
        [DNX_FIELD_QUAL_FTMH_RESERVED]                         = {DBAL_FIELD_FTMH_RESERVED},
        [DNX_FIELD_QUAL_FTMH_VISIBILITY]                       = {DBAL_FIELD_FTMH_VISIBILITY},
        [DNX_FIELD_QUAL_FTMH_TM_PROFILE]                       = {DBAL_FIELD_FTMH_TM_PROFILE},
        [DNX_FIELD_QUAL_FTMH_ECN_ENABLE]                       = {DBAL_FIELD_FTMH_ECN_ENABLE},
        [DNX_FIELD_QUAL_FTMH_CNI]                              = {DBAL_FIELD_FTMH_CNI},
        [DNX_FIELD_QUAL_FTMH_BIER_BFR_EXT_PRESENT]             = {DBAL_FIELD_FTMH_BIER_BFR_EXT_PRESENT},
        [DNX_FIELD_QUAL_FTMH_FLOW_ID_EXT_PRESENT]              = {DBAL_FIELD_FTMH_FLOW_ID_EXT_PRESENT},
        [DNX_FIELD_QUAL_FTMH_APPLICATION_SPECIFIC_EXT_PRESENT] = {DBAL_FIELD_FTMH_APPLICATION_SPECIFIC_EXT_PRESENT},
        [DNX_FIELD_QUAL_FTMH_TM_DESTINATION_EXT_PRESENT]       = {DBAL_FIELD_FTMH_TM_DESTINATION_EXT_PRESENT},
        [DNX_FIELD_QUAL_FTMH_MCID_OR_OUTLIF_0_OR_MCDB_PTR]     = {DBAL_FIELD_FTMH_MCID_OR_OUTLIF_0_OR_MCDB_PTR},
        [DNX_FIELD_QUAL_FTMH_TM_ACTION_IS_MC]                  = {DBAL_FIELD_FTMH_TM_ACTION_IS_MC},
        [DNX_FIELD_QUAL_FTMH_PPH_PRESENT]                      = {DBAL_FIELD_FTMH_PPH_PRESENT},
        [DNX_FIELD_QUAL_FTMH_TSH_EXT_PRESENT]                  = {DBAL_FIELD_FTMH_TSH_EXT_PRESENT},
        [DNX_FIELD_QUAL_FTMH_TM_ACTION_TYPE]                   = {DBAL_FIELD_FTMH_TM_ACTION_TYPE},
        [DNX_FIELD_QUAL_FTMH_DP]                               = {DBAL_FIELD_FTMH_DP},
        [DNX_FIELD_QUAL_PP_DSP]                                = {DBAL_FIELD_PP_DSP},
        [DNX_FIELD_QUAL_FTMH_SRC_SYS_PORT]                     = {DBAL_FIELD_SRC_SYS_PORT},
        [DNX_FIELD_QUAL_FTMH_TC]                               = {DBAL_FIELD_FTMH_TC},
        [DNX_FIELD_QUAL_FTMH_PACKET_SIZE]                      = {DBAL_FIELD_FTMH_PACKET_SIZE},
        [DNX_FIELD_QUAL_LB_KEY]                                = {DBAL_FIELD_LB_KEY},
        [DNX_FIELD_QUAL_NWK_KEY]                               = {DBAL_FIELD_NWK_LB_KEY},
        [DNX_FIELD_QUAL_STACKING_EXT]                          = {DBAL_FIELD_STACKING_EXT},
        [DNX_FIELD_QUAL_BIER_BFR_EXT]                          = {DBAL_FIELD_BIER_BFR_EXT},
        [DNX_FIELD_QUAL_TM_DESTINATION_EXT]                    = {DBAL_FIELD_TM_DESTINATION_EXT},
        [DNX_FIELD_QUAL_APPLICATION_SPECIFIC_EXT]              = {DBAL_FIELD_APPLICATION_SPECIFIC_EXT},
        [DNX_FIELD_QUAL_TSH_EXT]                               = {DBAL_FIELD_TSH_EXT},
        [DNX_FIELD_QUAL_FWD_STRENGTH]                          = {DBAL_FIELD_FWD_STRENGTH},
        [DNX_FIELD_QUAL_PPH_PARSING_START_TYPE]                = {DBAL_FIELD_PPH_PARSING_START_TYPE},
        [DNX_FIELD_QUAL_PPH_PARSING_START_OFFSET]              = {DBAL_FIELD_PPH_PARSING_START_OFFSET},
        [DNX_FIELD_QUAL_PPH_LIF_EXT_TYPE]                      = {DBAL_FIELD_PPH_LIF_EXT_TYPE},
        [DNX_FIELD_QUAL_PPH_FHEI_SIZE]                         = {DBAL_FIELD_FHEI_SIZE},
        [DNX_FIELD_QUAL_PPH_LEARN_EXT_PRESENT]                 = {DBAL_FIELD_PPH_LEARN_EXT_PRESENT},
        [DNX_FIELD_QUAL_PPH_TTL]                               = {DBAL_FIELD_PPH_TTL},
        [DNX_FIELD_QUAL_PPH_NWK_QOS]                           = {DBAL_FIELD_NWK_QOS},
        [DNX_FIELD_QUAL_PPH_IN_LIF]                            = {DBAL_FIELD_PPH_IN_LIF},
        [DNX_FIELD_QUAL_PPH_FWD_DOMAIN]                        = {DBAL_FIELD_PPH_FWD_DOMAIN},
        [DNX_FIELD_QUAL_PPH_IN_LIF_PROFILE]                    = {DBAL_FIELD_PPH_IN_LIF_PROFILE},
        [DNX_FIELD_QUAL_PPH_END_OF_PACKET_EDITING]             = {DBAL_FIELD_PPH_END_OF_PACKET_EDITING},
        [DNX_FIELD_QUAL_PPH_FWD_LAYER_ADDITIONAL_INFO]         = {DBAL_FIELD_PPH_FORWARDING_LAYER_ADDITIONAL_INFO},
        [DNX_FIELD_QUAL_PPH_VALUE1]                            = {DBAL_FIELD_PPH_VALUE1},
        [DNX_FIELD_QUAL_PPH_VALUE2]                            = {DBAL_FIELD_PPH_VALUE2},
        [DNX_FIELD_QUAL_FHEI_EXT]                              = {DBAL_FIELD_FHEI_EXT},
        [DNX_FIELD_QUAL_LEARN_EXT]                             = {DBAL_FIELD_LEARN_EXT},
        [DNX_FIELD_QUAL_LIF_EXT]                               = {DBAL_FIELD_LIF_EXT},
        [DNX_FIELD_QUAL_USER_HEADER_1]                         = {DBAL_FIELD_USER_HEADER_1},
        [DNX_FIELD_QUAL_USER_HEADER_2]                         = {DBAL_FIELD_USER_HEADER_2},
        [DNX_FIELD_QUAL_USER_HEADER_3]                         = {DBAL_FIELD_USER_HEADER_3},
        [DNX_FIELD_QUAL_USER_HEADER_4]                         = {DBAL_FIELD_USER_HEADER_4},
        [DNX_FIELD_QUAL_USER_HEADER_1_TYPE]                    = {DBAL_FIELD_USER_HEADER_1_TYPE},
        [DNX_FIELD_QUAL_USER_HEADER_2_TYPE]                    = {DBAL_FIELD_USER_HEADER_2_TYPE},
        [DNX_FIELD_QUAL_USER_HEADER_3_TYPE]                    = {DBAL_FIELD_USER_HEADER_3_TYPE},
        [DNX_FIELD_QUAL_USER_HEADER_4_TYPE]                    = {DBAL_FIELD_USER_HEADER_4_TYPE},
        [DNX_FIELD_QUAL_IS_TDM]                                = {DBAL_FIELD_IS_TDM},
        [DNX_FIELD_QUAL_CUD_OUTLIF_OR_MCDB_PTR]                = {DBAL_FIELD_CUD_OUTLIF_OR_MCDB_PTR},
        [DNX_FIELD_QUAL_DISCARD]                               = {DBAL_FIELD_DISCARD},
        [DNX_FIELD_QUAL_OUT_LIF_PROFILE]                       = {DBAL_FIELD_PMF_OUT_LIF_PROFILE},
        [DNX_FIELD_QUAL_PMF_ETH_RIF_PROFILE]                   = {DBAL_FIELD_PMF_OUT_RIF_PROFILE},
        [DNX_FIELD_QUAL_ETH_TAG_FORMAT]                        = {DBAL_FIELD_ETH_TAG_FORMAT},
        [DNX_FIELD_QUAL_UPDATED_TPIDS_PACKET_DATA]             = {DBAL_FIELD_UPDATED_TPIDS_PACKET_DATA},
        [DNX_FIELD_QUAL_IVE_BYTES_TO_ADD]                      = {DBAL_FIELD_IVE_BYTES_TO_ADD},
        [DNX_FIELD_QUAL_IVE_BYTES_TO_REMOVE]                   = {DBAL_FIELD_IVE_BYTES_TO_REMOVE},
        [DNX_FIELD_QUAL_ETHER_TYPE_CODE]                       = {DBAL_FIELD_ETHER_TYPE_CODE},
        [DNX_FIELD_QUAL_FABRIC_OR_EGRESS_MC]                   = {DBAL_FIELD_FABRIC_OR_EGRESS_MC},
        [DNX_FIELD_QUAL_ESTIMATED_BTR]                         = {DBAL_FIELD_ESTIMATED_BTR},
        [DNX_FIELD_QUAL_FWD_CONTEXT]                           = {DBAL_FIELD_FWD_CONTEXT},
        [DNX_FIELD_QUAL_GLOB_OUT_LIF_0]                        = {DBAL_FIELD_GLOB_OUT_LIF_0},
        [DNX_FIELD_QUAL_GLOB_OUT_LIF_1]                        = {DBAL_FIELD_GLOB_OUT_LIF_1},
        [DNX_FIELD_QUAL_GLOB_OUT_LIF_2]                        = {DBAL_FIELD_GLOB_OUT_LIF_2},
        [DNX_FIELD_QUAL_GLOB_OUT_LIF_3]                        = {DBAL_FIELD_GLOB_OUT_LIF_3},
        [DNX_FIELD_QUAL_GLOB_IN_LIF_0]                         = {DBAL_FIELD_GLOB_IN_LIF_0},
        [DNX_FIELD_QUAL_IN_LIF_PROFILE_0]                      = {DBAL_FIELD_IN_LIF_PROFILE_0},
        [DNX_FIELD_QUAL_IP_MC_ELIGIBLE]                        = {DBAL_FIELD_IP_MC_ELIGIBLE},
        [DNX_FIELD_QUAL_IP_MC_SHOULD_BE_BRIDGED]               = {DBAL_FIELD_IP_MC_SHOULD_BE_BRIDGED},
        [DNX_FIELD_QUAL_LEARN_VALID]                           = {DBAL_FIELD_LEARN_VALID},
        [DNX_FIELD_QUAL_LOCAL_OUT_LIF]                         = {DBAL_FIELD_LOCAL_OUT_LIF},
        [DNX_FIELD_QUAL_FWD_ACTION_PROFILE_INDEX]              = {DBAL_FIELD_FWD_ACTION_PROFILE_INDEX},
        [DNX_FIELD_QUAL_SNOOP_ACTION_PROFILE_INDEX]            = {DBAL_FIELD_SNOOP_ACTION_PROFILE_INDEX},
        [DNX_FIELD_QUAL_SNOOP_STRENGTH]                        = {DBAL_FIELD_SNOOP_STRENGTH},
        [DNX_FIELD_QUAL_SYSTEM_HEADERS_SIZE]                   = {DBAL_FIELD_SYSTEM_HEADERS_SIZE},
        [DNX_FIELD_QUAL_DST_SYS_PORT]                          = {DBAL_FIELD_DST_SYS_PORT},
        [DNX_FIELD_QUAL_OUT_TM_PORT]                           = {DBAL_FIELD_OUT_TM_PORT},
        [DNX_FIELD_QUAL_OUT_PP_PORT]                           = {DBAL_FIELD_OUT_PP_PORT},
        [DNX_FIELD_QUAL_PACKET_IS_APPLET]                      = {DBAL_FIELD_IS_APPLET},
        [DNX_FIELD_QUAL_MEM_SOFT_ERR]                          = {DBAL_FIELD_MEM_SOFT_ERR},
        [DNX_FIELD_QUAL_TC_MAP_PROFILE]                        = {DBAL_FIELD_TC_MAP_PROFILE},
        [DNX_FIELD_QUAL_L4_PORT_IN_RANGE]                      = {DBAL_FIELD_L4_PORT_IN_RANGE},
        [DNX_FIELD_QUAL_PROGRAM_INDEX]                         = {DBAL_FIELD_PROGRAM_INDEX},
        [DNX_FIELD_QUAL_PER_PORT_TABLE_DATA]                   = {DBAL_FIELD_PER_PORT_TABLE_DATA},
        [DNX_FIELD_QUAL_PP_PORT_PMF_PROFILE]                   = {DBAL_FIELD_PP_PORT_PMF_PROFILE},
        [DNX_FIELD_QUAL_TM_PORT_PMF_PROFILE]                   = {DBAL_FIELD_TM_PORT_PMF_PROFILE},
        [DNX_FIELD_QUAL_PPH_VRF_VALUE]                         = {DBAL_FIELD_PPH_VRF_VALUE},
        [DNX_FIELD_QUAL_LEARN_EXT_SRC_MAC]                     = {DBAL_FIELD_LEARN_SRC_MAC},
        [DNX_FIELD_QUAL_LEARN_EXT_VLAN]                        = {DBAL_FIELD_LEARN_VLAN},
        [DNX_FIELD_QUAL_LEARN_EXT_VSI]                         = {DBAL_FIELD_LEARN_VSI},
        [DNX_FIELD_QUAL_LEARN_EXT_DATA]                        = {DBAL_FIELD_LEARN_DATA},
        [DNX_FIELD_QUAL_PPH_FWD_LAYER_INDEX]                   = {DBAL_FIELD_PPH_FWD_LAYER_INDEX},

};

static const dnx_field_meta_qual_info_t dnx_l4ops_meta_qual_info[] = {
        [DNX_FIELD_QUAL_DP]                               = {DBAL_FIELD_DP},
        [DNX_FIELD_QUAL_ECN]                              = {DBAL_FIELD_METADATA_ECN},
        [DNX_FIELD_QUAL_EEI]                              = {DBAL_FIELD_EEI},
        [DNX_FIELD_QUAL_ELK_LKP_HIT_0]                    = {DBAL_FIELD_ELK_LKP_HIT_0},
        [DNX_FIELD_QUAL_ELK_LKP_HIT_1]                    = {DBAL_FIELD_ELK_LKP_HIT_1},
        [DNX_FIELD_QUAL_ELK_LKP_HIT_2]                    = {DBAL_FIELD_ELK_LKP_HIT_2},
        [DNX_FIELD_QUAL_ELK_LKP_HIT_3]                    = {DBAL_FIELD_ELK_LKP_HIT_3},
        [DNX_FIELD_QUAL_ELK_LKP_HIT_4]                    = {DBAL_FIELD_ELK_LKP_HIT_4},
        [DNX_FIELD_QUAL_ELK_LKP_HIT_5]                    = {DBAL_FIELD_ELK_LKP_HIT_5},
        [DNX_FIELD_QUAL_ELK_LKP_HIT_6]                    = {DBAL_FIELD_ELK_LKP_HIT_6},
        [DNX_FIELD_QUAL_ELK_LKP_HIT_7]                    = {DBAL_FIELD_ELK_LKP_HIT_7},
        [DNX_FIELD_QUAL_EXTERNAL_RESULT_BUFFER_PART_1]    = {DBAL_FIELD_ELK_LKP_PAYLOAD_ALL_PART_1},
        [DNX_FIELD_QUAL_EXTERNAL_RESULT_BUFFER_PART_0]    = {DBAL_FIELD_ELK_LKP_PAYLOAD_ALL_PART_0},
        [DNX_FIELD_QUAL_FWD_DOMAIN]                       = {DBAL_FIELD_FWD_DOMAIN},
        [DNX_FIELD_QUAL_FWD_LAYER_ADDITIONAL_INFO]        = {DBAL_FIELD_FWD_LAYER_ADDITIONAL_INFO},
        [DNX_FIELD_QUAL_GENERAL_DATA_2]                   = {DBAL_FIELD_GENERAL_DATA_2},
        [DNX_FIELD_QUAL_GENERAL_DATA_1]                   = {DBAL_FIELD_GENERAL_DATA_1},
        [DNX_FIELD_QUAL_GENERAL_DATA_0]                   = {DBAL_FIELD_GENERAL_DATA_0},
        [DNX_FIELD_QUAL_GLOB_IN_LIF_1]                    = {DBAL_FIELD_GLOB_IN_LIF_1},
        [DNX_FIELD_QUAL_GLOB_IN_LIF_0]                    = {DBAL_FIELD_GLOB_IN_LIF_0},
        [DNX_FIELD_QUAL_NWK_QOS]                          = {DBAL_FIELD_NWK_QOS},
        [DNX_FIELD_QUAL_GLOB_OUT_LIF_1]                   = {DBAL_FIELD_GLOB_OUT_LIF_1},
        [DNX_FIELD_QUAL_GLOB_OUT_LIF_0]                   = {DBAL_FIELD_GLOB_OUT_LIF_0},
        [DNX_FIELD_QUAL_PRT_QUALIFIER]                    = {DBAL_FIELD_PRT_QUALIFIER_RAW},
        [DNX_FIELD_QUAL_RPF_DST]                          = {DBAL_FIELD_RPF_DST},
        [DNX_FIELD_QUAL_RPF_DST_VALID]                    = {DBAL_FIELD_RPF_DST_VALID},
        [DNX_FIELD_QUAL_RPF_OUT_LIF]                      = {DBAL_FIELD_RPF_OUT_LIF},
        [DNX_FIELD_QUAL_STATISTICS_OBJ_12]                = {DBAL_FIELD_STATISTICS_OBJ_12},
        [DNX_FIELD_QUAL_STATISTICS_OBJ_11]                = {DBAL_FIELD_STATISTICS_OBJ_11},
        [DNX_FIELD_QUAL_STATISTICS_OBJ_10]                = {DBAL_FIELD_STATISTICS_OBJ_10},
        [DNX_FIELD_QUAL_STATISTICS_OBJ_9]                 = {DBAL_FIELD_STATISTICS_OBJ_9},
        [DNX_FIELD_QUAL_STATISTICS_OBJ_8]                 = {DBAL_FIELD_STATISTICS_OBJ_8},
        [DNX_FIELD_QUAL_STATISTICS_OBJ_7]                 = {DBAL_FIELD_STATISTICS_OBJ_7},
        [DNX_FIELD_QUAL_STATISTICS_OBJ_6]                 = {DBAL_FIELD_STATISTICS_OBJ_6},
        [DNX_FIELD_QUAL_STATISTICS_OBJ_5]                 = {DBAL_FIELD_STATISTICS_OBJ_5},
        [DNX_FIELD_QUAL_STATISTICS_OBJ_4]                 = {DBAL_FIELD_STATISTICS_OBJ_4},
        [DNX_FIELD_QUAL_STATISTICS_OBJ_3]                 = {DBAL_FIELD_STATISTICS_OBJ_3},
        [DNX_FIELD_QUAL_STATISTICS_OBJ_2]                 = {DBAL_FIELD_STATISTICS_OBJ_2},
        [DNX_FIELD_QUAL_STATISTICS_OBJ_1]                 = {DBAL_FIELD_STATISTICS_OBJ_1},
        [DNX_FIELD_QUAL_STATISTICS_OBJ_0]                 = {DBAL_FIELD_STATISTICS_OBJ_0},
        [DNX_FIELD_QUAL_OAM_STATISTICS_OBJ_2]             = {DBAL_FIELD_OAM_STATISTICS_OBJ_2},
        [DNX_FIELD_QUAL_OAM_STATISTICS_OBJ_1]             = {DBAL_FIELD_OAM_STATISTICS_OBJ_1},
        [DNX_FIELD_QUAL_OAM_STATISTICS_OBJ_0]             = {DBAL_FIELD_OAM_STATISTICS_OBJ_0},
        [DNX_FIELD_QUAL_SRC_SYS_PORT]                     = {DBAL_FIELD_SRC_SYS_PORT},
        [DNX_FIELD_QUAL_TC]                               = {DBAL_FIELD_TC},
        [DNX_FIELD_QUAL_CORE_ID]                          = {DBAL_FIELD_CORE},
        [DNX_FIELD_QUAL_IN_PORT]                          = {DBAL_FIELD_IN_PORT},
        [DNX_FIELD_QUAL_FWD_LAYER_INDEX]                  = {DBAL_FIELD_FWD_LAYER_INDEX},
};

/*
 * End of DNX Qualifier data
 * }
 */

const dnx_field_qual_input_type_info_t dnx_input_type_info[DNX_FIELD_INPUT_TYPE_NOF] = {
    [DNX_FIELD_INPUT_TYPE_LAYER_FWD]               = {TRUE, 0xd /* 1101 */},
    [DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE]          = {TRUE, 0xd /* 1101 */},
    [DNX_FIELD_INPUT_TYPE_META_DATA]               = {TRUE, 0xb /* 1011 */},
    [DNX_FIELD_INPUT_TYPE_CONST]                   = {TRUE, 0xb /* 1011 */},
    [DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_FWD]       = {TRUE, 0xf /* 1111 */},
    [DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_ABSOLUTE]  = {TRUE, 0xf /* 1111 */},
    [DNX_FIELD_INPUT_TYPE_EXTERNAL]                = {TRUE, 0xb /* 1011 */},
};
/*
 * End of qualifier related data
 * }
 */

/*
 * Action related data below
 * {
 */


static const char *bcm_action_description[bcmFieldActionCount] = {
    [bcmFieldActionSnoop]                  = "Snoop matched packets, input can be either a snoop command: value[0] = Mirror Gport, value[1]=snoop qualifier, value[2]=snoop strength OR: snoop code: value[0] = Trap Gport, value[1]=snoop qualifier, at egress stage refers only to the snoop ID (strength is not relevant, and can be changed using bcmFieldActionSnoopStrength action)",
    [bcmFieldActionSnoopStrength]          = "updates the snoop strength",
    [bcmFieldActionPphPresentSet]          = "Create Packet Processing and Time Stamp headers if injected",
    [bcmFieldActionPrioIntNew]             = "TC update",
    [bcmFieldActionDropPrecedence]         = "Set Packet's Drop Precedence, the values are BCM_FIELD_COLOR_XXX",
    [bcmFieldActionForward]                = "Forward Destination Raw value, setting to 1's will drop the packet",
    [bcmFieldActionRedirectMcast]          = "Redirect to Multicast Group Id",
    [bcmFieldActionRedirect     ]          = "Redirect packet to to destination",
    [bcmFieldActionFabricHeaderSet]        = "Change System Header Profile, bcm_field_system_header_profile_t",
    [bcmFieldActionUDHData0]               = "Set the Payload for User Defined Header 0",
    [bcmFieldActionUDHData1]               = "Set the Payload for User Defined Header 1",
    [bcmFieldActionUDHData2]               = "Set the Payload for User Defined Header 2",
    [bcmFieldActionUDHData3]               = "Set the Payload for User Defined Header 3",
    [bcmFieldActionUDHBase]                = "Set the Base for All  User Defined Header 0-3, UDHBase0 is at LSB",
    [bcmFieldActionUDHBase0]               = "Set the Base for User Defined Header 0",
    [bcmFieldActionUDHBase1]               = "Set the Base for User Defined Header 1",
    [bcmFieldActionUDHBase2]               = "Set the Base for User Defined Header 2",
    [bcmFieldActionUDHBase3]               = "Set the Base for User Defined Header 3",
    [bcmFieldActionVlanActionSetNew]       = "Modify the VLAN Action Set Id ",
    [bcmFieldActionOuterVlanPrioNew]       = "Replace outer VLAN tag priority",
    [bcmFieldActionOuterVlanNew]           = "Replace outer VLAN Id",
    [bcmFieldActionInnerVlanPrioNew]       = "Replace inner VLAN tag priority",
    [bcmFieldActionInnerVlanNew]           = "Replace inner VLAN Id",
    [bcmFieldActionUsePolicerResult]       = "Specify/override where policer result will be used for matched packets",
    [bcmFieldActionSystemHeaderSet]        = "Modify EEI",
    [bcmFieldActionVSQ]                    = "Assign matching packets to specified VSQ",
    [bcmFieldActionAdmitProfile]           = "Admit Profile",
    [bcmFieldActionIEEE1588]               = "Setting various parameters for 1588 frames. Bits(0:7): header_offset. Bit(8): encapsulation. Bits(9:10) command. Bit(11): compensate_time_stamp. Bit(12): packet_is_ieee1588.",
    [bcmFieldActionSrcGportNew]            = "Set the Source-Port ",
    [bcmFieldActionTtlSet]                 = "new TTL",
    [bcmFieldActionLatencyFlowId]          = "change the latency flow-Id. value[0] valid. value[1]: latency_flow_id. value[2]: latency_flow_profile.",
    [bcmFieldActionIngressGportSet]        = "Set a new ingress Global Lif encoded, Values should be Encoded as GPORT",
    [bcmFieldActionInterfaceClassVPort]    = "set Inlif profile",
    [bcmFieldActionMirrorIngress]          = "set mirror, value should be encoded as  GPORT Type Mirror for mirror command",
    [bcmFieldActionStackingRouteNew]       = "Replace the value of the stacking route",
    [bcmFieldActionOam]                    = "Changes 4 OAM signals. value[0]: OAM-Stamp-Offset. value[1]: OAM-offset. value[2]: OAM-Sub-Type. value[3]: OAM-Up-Mep.",
    [bcmFieldActionPphSnoopCode]           = "Set the PPH Snoop code value (Reserved Bits)",
    [bcmFieldActionTrap]                   = "Set Trap code, use TRAP GPORT type value[0]=TRAP_GPORT value[1]=Trap Qualifier. at egress stage refers only to the TRAP ID (strength is not relevant, and can be changed using bcmFieldActionTrapStrength action)",
    [bcmFieldActionTrapStrength]           = "Updates the Trap Strength. Use TRAP Gport ",
    [bcmFieldActionOutVport0]              = "change outlif 0, value should be encoded as a GPORT with LIF subtype",
    [bcmFieldActionOutVport1]              = "change outlif 1, value should be encoded as a GPORT with LIF subtype",
    [bcmFieldActionOutVport2]              = "change outlif 2, value should be encoded as a GPORT with LIF subtype",
    [bcmFieldActionOutVport3]              = "change outlif 3, value should be encoded as a GPORT with LIF subtype",
    [bcmFieldActionOutInterface0]          = "change outlif 0, value should be encoded as type interface.",
    [bcmFieldActionOutInterface1]          = "change outlif 1, value should be encoded as type interface.",
    [bcmFieldActionOutInterface2]          = "change outlif 2, value should be encoded as type interface.",
    [bcmFieldActionOutInterface3]          = "change outlif 3, value should be encoded as type interface.",
    [bcmFieldActionVrfSet]                 = "Set VRF. value[0]: fwd_domain_id_type. value[1]: fwd_domain_id_value. value[1]: fwd_domain_profile. In iPMF3, fwd_domain_profile isn't updated.",
    [bcmFieldActionVSwitchNew]             = "new VSI. value[0]: fwd_domain_id_type. value[1]: fwd_domain_id_value. value[1]: fwd_domain_profile. In iPMF3, fwd_domain_profile isn't updated.",
    [bcmFieldActionContainer]              = "This action will be used as container in IPMF2, to parse the action buffer, when performing cascading between IPMF2 and IPMF3.",
    [bcmFieldActionCbts]                   = "Changing tunnel priority for CBTS. For each hierarchy takes a mask of current 3LSB and performs bitwise OR with the value. [0:2] Hierarchy 1 TP mask [3:5] Hierarchy 1 TP value [6:8] Hierarchy 2 TP mask [9:1] Hierarchy 2 TP value [12:14] Hierarchy 3 TP mask [15:17] Hierarchy 3 TP value [18] valid bit.",
    [bcmFieldActionIPTProfile]            = "IPT Profile action (used for instrumentation)",
    [bcmFieldActionStatId0]                = "Updates statistics ID 0",
    [bcmFieldActionStatId1]                = "Updates statistics ID 1",
    [bcmFieldActionStatId2]                = "Updates statistics ID 2",
    [bcmFieldActionStatId3]                = "Updates statistics ID 3",
    [bcmFieldActionStatId4]                = "Updates statistics ID 4",
    [bcmFieldActionStatId5]                = "Updates statistics ID 5",
    [bcmFieldActionStatId6]                = "Updates statistics ID 6",
    [bcmFieldActionStatId7]                = "Updates statistics ID 7",
    [bcmFieldActionStatId8]                = "Updates statistics ID 8",
    [bcmFieldActionStatId9]                = "Updates statistics ID 9",
    [bcmFieldActionStatProfile0]           = "Updates statistics profile 0. In ePMF indicates which value to read. In iPMF1/2/3: value[0]: is_meter. value[1]: is_lm. value[2]: type. value[3]: valid.",
    [bcmFieldActionStatProfile1]           = "Updates statistics profile 1. In ePMF indicates which value to read. In iPMF1/2/3: value[0]: is_meter. value[1]: is_lm. value[2]: type. value[3]: valid.",
    [bcmFieldActionStatProfile2]           = "Updates statistics profile 2. value[0]: is_meter. value[1]: is_lm. value[2]: type. value[3]: valid.",
    [bcmFieldActionStatProfile3]           = "Updates statistics profile 3. value[0]: is_meter. value[1]: is_lm. value[2]: type. value[3]: valid.",
    [bcmFieldActionStatProfile4]           = "Updates statistics profile 4. value[0]: is_meter. value[1]: is_lm. value[2]: type. value[3]: valid.",
    [bcmFieldActionStatProfile5]           = "Updates statistics profile 5. value[0]: is_meter. value[1]: is_lm. value[2]: type. value[3]: valid.",
    [bcmFieldActionStatProfile6]           = "Updates statistics profile 6. value[0]: is_meter. value[1]: is_lm. value[2]: type. value[3]: valid.",
    [bcmFieldActionStatProfile7]           = "Updates statistics profile 7. value[0]: is_meter. value[1]: is_lm. value[2]: type. value[3]: valid.",
    [bcmFieldActionStatProfile8]           = "Updates statistics profile 8. value[0]: is_meter. value[1]: is_lm. value[2]: type. value[3]: valid.",
    [bcmFieldActionStatProfile9]           = "Updates statistics profile 9. value[0]: is_meter. value[1]: is_lm. value[2]: type. value[3]: valid.",
    [bcmFieldActionIngressTimeStampInsert] ="Insert IPIPE time stamp",
    [bcmFieldActionStatMetaData]           = "Set statistics metaData",
    [bcmFieldActionInvalidNext]            = "Invalid next Action macro Id",
    [bcmFieldActionExtStat0]               = "Update External Statistics ID 0. value[0]: object_stat_id. value[1]: opcode. value[2]: valid. ",
    [bcmFieldActionExtStat1]               = "Update External Statistics ID 1. value[0]: object_stat_id. value[1]: opcode.",
    [bcmFieldActionExtStat2]               = "Update External Statistics ID 2. value[0]: object_stat_id. value[1]: opcode.",
    [bcmFieldActionExtStat3]               = "Update External Statistics ID 3. value[0]: object_stat_id. value[1]: opcode.",
    [bcmFieldActionForwardingTypeMeterMap] = "Maps L2 forwarding type to one of the general meters",
    [bcmFieldActionStatSampling]           = "Update statistical sampling value, value encoded as Mirror Gport",
    [bcmFieldActionStatOamLM]              = "Update the counter of OAM LM, the index of the values are bcmFieldStatOamLmIndexXXX",
    [bcmFieldActionStatOamLMRaw]           = "Update the counter of OAM LM with a raw value",
    [bcmFieldActionForwardingLayerIndex]   = "Update forwarding layer index",
    [bcmFieldActionVisibilityEnable]       = "Enables visibility for current packet",
    [bcmFieldActionVisibilityClear]        = "Disables visibility for current packet",
    [bcmFieldActionInVport0]               = "Change the In Lif0, value should be encoded as a GPORT with LIF subtype",
    [bcmFieldActionInVport1]               = "Change the In Lif1, value should be encoded as a GPORT with LIF subtype",
    [bcmFieldActionInInterface0]           = "Change the In Lif0, value should be encoded as an interface",
    [bcmFieldActionInInterface1]           = "Change the In Lif1, value should be encoded as an interface",
    [bcmFieldActionSmallExemLearn]         = "Updates the Small Exact Match learn info (IPMF2 only).",
    [bcmFieldActionPacketTraceEnable]      = "Enable Trace packet",
    [bcmFieldActionEcnNew]                 = "Change metadata ECN / congestion info (including CNI)",
    [bcmFieldActionDrop]                   = "Discard packet",
    [bcmFieldActionStat0]                  = "Stat meter object. value[0]: stat_meter_obj_cmd. value[1]: stat_meter_obj_id",
    [bcmFieldActionStat1]                  = "Stat counter object. value[0]: stat_counter_obj_cmd. value[1]: stat_counter_obj_id. value[2]: valid.",
    [bcmFieldActionMirrorEgress]           = "Update the mirror profile, mirror_cmd should be encoded as MIRROR Gport",
    [bcmFieldActionVoid]                   = "Void action, adds bits to the payload buffer without automatically associating an action for them. Used for cascading, FEM, added EFES or padding. All actions on an external lookup are void.",
    [bcmFieldActionTrunkHashKeySet]        = "Set the Trunk Hash Key",
    [bcmFieldActionNetworkLoadBalanceKey]  = "Set the network load balancing Key",
    [bcmFieldActionParsingStartType]      = "Indicate the layer type that egress parser should consider",
    [bcmFieldActionParsingStartOffset]    = "Indicate from which offset egress parser should start parsing (all info before that will not be parsed).",
    [bcmFieldActionEgressForwardingIndex] = "Indicate for egress parser which layer is the Forwarding layer.",
    [bcmFieldActionIPTCommand]            = "IPT Profile command. value[0]: int_commnad. value[1]: int_profile.",
    [bcmFieldActionStartPacketStrip]      = "Strip the start of packet. value[0]: bcm_field_start_packet_strip_t value[1]: extra bytes to remove.",
    [bcmFieldActionEstimatedStartPacketStrip] = "Estimated bytes to remove at the start of packet.",
    [bcmFieldActionLearnSrcPortNew]       = "Replace the learnt packet source Port",
    [bcmFieldActionAceContextValue]       = "Updates programmable value per ACE context, bcm_field_ace_context_t. Up to four different values can be given in value[0-3], although some may be mutually exclusive",
    [bcmFieldActionQosMapIdNew]           = "Set the QoS profile ",
    [bcmFieldActionAceEntryId]            = "Pointer to the ACE entry ID",
    [bcmFieldActionOutPortTrafficManagement] = "Updates the Out-TM port",
    [bcmFieldActionBierStringOffset]         = "Updates Bier string offset.",
    [bcmFieldActionBierStringSize]           = "Updates Bier string size.",
    [bcmFieldActionPacketIsBier]          = "Updates is Bier packet.",
    [bcmFieldActionEventor]               = "Enable push data to eventor.",
    [bcmFieldActionForwardingAdditionalInfo]  = "Updates forwarding additional information.",
    [bcmFieldActionSystemHeaderSizeAdjust]  = "System Header Size adjust.",
    [bcmFieldActionInVportClass0]           = "Updates the profile of an in LIF encoded as Gport.",
    [bcmFieldActionInVportClass1]           = "Updates the profile of an in LIF encoded as Gport.",
    [bcmFieldActionNetworkQos]           = "Updates Network Qos value",
    [bcmFieldActionIngressDoNotLearn]             = "Disable Ingress learning",
    [bcmFieldActionEgressDoNotLearn]             = "Disable Egress learning",
    [bcmFieldActionInVport0Raw]               = "Change the In Lif0, with raw value.",
    [bcmFieldActionInVport1Raw]               = "Change the In Lif1, with raw value.",
    [bcmFieldActionOutVport0Raw]              = "Change the Out Lif 0, with raw value.",
    [bcmFieldActionOutVport1Raw]              = "Change the Out Lif 1, with raw value.",
    [bcmFieldActionOutVport2Raw]              = "Change the Out Lif 2, with raw value.",
    [bcmFieldActionOutVport3Raw]              = "Change the Out Lif 3, with raw value.",
    [bcmFieldActionAppendPointerCompensation]  = "Configures pointer to header append compensation value.",
    [bcmFieldActionIngressTimeStampInsertValid]  = "Insert ingress timestamp. Overrides the time stamp in TSH header.",
    [bcmFieldActionSnoopRaw]  = "Snoop matched packets, input should be raw value. At egress stage refers only to the snoop ID (strength is not relevant, and can be changed using bcmFieldActionSnoopStrength action)",
    [bcmFieldActionDropPrecedenceRaw]  = "Set Packet's Drop Precedence, input expects raw value.",
    [bcmFieldActionFabricHeaderSetRaw]  = "Change System Header Profile, input expects raw value.",
    [bcmFieldActionSrcGportNewRaw]  = "Set the Source-Port, input expects raw value.",
    [bcmFieldActionLatencyFlowIdRaw]  = "Change the latency flow-Id, input expects raw value.",
    [bcmFieldActionMirrorIngressRaw]  = "Set mirror in ingress, input expects raw value.",
    [bcmFieldActionOamRaw]  = "Changes 4 OAM signals (OAM-Stamp-Offset, OAM-offset, OAM-Sub-Type, OAM-Up-Mep), input expects raw value.",
    [bcmFieldActionTrapRaw]  = "Set Trap code, input should be raw value. At egress stage refers only to the TRAP ID (strength is not relevant, and can be changed using bcmFieldActionTrapStrength action).",
    [bcmFieldActionVrfSetRaw]  = "Set VRF, input expects raw value.",
    [bcmFieldActionStatProfile0Raw]  = "Updates statistics profile 0. Input expects raw value.",
    [bcmFieldActionStatProfile1Raw]  = "Updates statistics profile 1. Input expects raw value.",
    [bcmFieldActionStatProfile2Raw]  = "Updates statistics profile 2. Input expects raw value.",
    [bcmFieldActionStatProfile3Raw]  = "Updates statistics profile 3. Input expects raw value.",
    [bcmFieldActionStatProfile4Raw]  = "Updates statistics profile 4. Input expects raw value.",
    [bcmFieldActionStatProfile5Raw]  = "Updates statistics profile 5. Input expects raw value.",
    [bcmFieldActionStatProfile6Raw]  = "Updates statistics profile 6. Input expects raw value.",
    [bcmFieldActionStatProfile7Raw]  = "Updates statistics profile 7. Input expects raw value.",
    [bcmFieldActionStatProfile8Raw]  = "Updates statistics profile 8. Input expects raw value.",
    [bcmFieldActionStatProfile9Raw]  = "Updates statistics profile 9. Input expects raw value.",
    [bcmFieldActionExtStat0Raw]  = "Update External Statistics ID 0. Input expects raw value.",
    [bcmFieldActionExtStat1Raw]  = "Update External Statistics ID 1. Input expects raw value.",
    [bcmFieldActionExtStat2Raw]  = "Update External Statistics ID 2. Input expects raw value.",
    [bcmFieldActionExtStat3Raw]  = "Update External Statistics ID 3. Input expects raw value.",
    [bcmFieldActionStatSamplingRaw]  = "Update statistical sampling value, input expects raw value.",
    [bcmFieldActionStartPacketStripRaw]  = "Strip the start of packet. Refers to bcm_field_start_packet_strip_t. Input expects raw value.",
    [bcmFieldActionParsingStartTypeRaw]  = "Indicate the layer type that egress parser should consider, input expects raw value.",
    [bcmFieldActionParsingStartOffsetRaw]  = "Indicate from which offset egress parser should start parsing (all info before that will not be parsed). Input expects raw value.",
    [bcmFieldActionIPTCommandRaw]  = "IPT Profile command. Input expects raw value.",
    [bcmFieldActionMirrorEgressRaw]  = "Update the egress mirror profile, input expects raw value.",
    [bcmFieldActionTrapStrengthRaw]  = "Updates the Trap Strength, input expects raw value.",
    [bcmFieldActionSnoopStrengthRaw]  = "Updates the Snoop Strength, input expects raw value.",
    [bcmFieldActionAceContextValueRaw]  = "Updates programmable value per ACE context, input expects raw value.",
    [bcmFieldActionStat0Raw]  = "Stat meter object. Input expects raw value.",
    [bcmFieldActionStat1Raw]  = "Stat counter object. Input expects raw value.",
    [bcmFieldActionIpMulticastCompatible]  = "Designates when a Compatible MC procedure found a match.",
    [bcmFieldActionRpfOutVport]  = "Updates the RPF OUT LIF. value[0]: route_valid, value[1]: default_route_found, value[2]: rpf_out_lif (encoded as GPORT with LIF subtype)",
    [bcmFieldActionRpfOutInterface]  = "Updates the RPF OUT LIF. value[0]: route_valid, value[1]: default_route_found, value[2]: rpf_out_lif (encoded as L3 interface)",
    [bcmFieldActionRpfOutVportRaw]  = "Updates the RPF OUT LIF. Input expects raw value.",
    [bcmFieldActionL3Switch]  = "Sets packet destination, input expects L3 interface.",
    [bcmFieldActionLearnRaw0]  = "Updates learn info 0.",
    [bcmFieldActionLearnRaw1]  = "Updates learn info 1.",
    [bcmFieldActionLearnRaw2]  = "Updates learn info 2.",
    [bcmFieldActionLearnRaw3]  = "Updates learn info 3.",
    [bcmFieldActionLearnRaw4]  = "Updates learn info 4.",
    [bcmFieldActionLearnOrTransplant]  = "Updates learn or transplant bit.",
    [bcmFieldActionLearnKey0]  = "Updates bits 31:0 of learn info key.",
    [bcmFieldActionLearnKey1]  = "Updates bits 63:32 of learn info key.",
    [bcmFieldActionLearnKey2]  = "Updates learn info key and app_db_index. value[0]: bits 79:64 of learn info ley, value[1] app_db_index.",
    [bcmFieldActionLearnKey2Raw]  = "Updates learn info key and app_db_index. Key is at the LSB.",
    [bcmFieldActionLearnPayload0]  = "Updates bits 31:0 of learn info data (payload).",
    [bcmFieldActionLearnPayload1]  = "Updates bits 63:32 of learn info data (payload).",
    [bcmFieldActionLearnEntryFormatIndex]  = "Updates entry format index in learn info.",
    [bcmFieldActionDstRpfGportNew]  = "Set the RPF Destination.",
    [bcmFieldActionDstRpfGportNewValid]  = "Set the RPF Destination Valid.",
    [bcmFieldActionDstRpfGportNewRaw]  = "Set the RPF Destination. Input expects raw value.",
    [bcmFieldActionPrtQualifier] = "Update prt qualifier",
    [bcmFieldActionDstClassField]  = "Set the pph value1. Input expects raw value.",
    [bcmFieldActionSrcClassField]  = "Set the pph value2. Input expects raw value.",
    [bcmFieldActionClassField]  = "Set the fhei_ext low 32 bits. Input expects raw value.",
    [bcmFieldActionRepCopy]  = "Set the copy type. For possible values look at bcm_field_rep_copy_type_t.",
};

static const dnx_field_action_map_t dnx_global_action_map[bcmFieldActionCount] = {
    [bcmFieldActionVoid]              = {DNX_ACTION(DNX_FIELD_ACTION_CLASS_SW, 0, DNX_FIELD_SW_ACTION_VOID)},
};

static const dnx_field_action_map_t dnx_ipmf1_action_map[bcmFieldActionCount] = {
        [bcmFieldActionSnoopRaw]               = {DBAL_ENUM_FVAL_IPMF1_ACTION_SNOOP_DATA, },
        [bcmFieldActionSnoop]                  = {DBAL_ENUM_FVAL_IPMF1_ACTION_SNOOP_DATA, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_ingress_snoop_gport_to_code},
        [bcmFieldActionPphPresentSet]          = {DBAL_ENUM_FVAL_IPMF1_ACTION_PPH_TYPE},
        [bcmFieldActionPrioIntNew]             = {DBAL_ENUM_FVAL_IPMF1_ACTION_TC, },
        [bcmFieldActionDropPrecedenceRaw]      = {DBAL_ENUM_FVAL_IPMF1_ACTION_DP, },
        [bcmFieldActionDropPrecedence]         = {DBAL_ENUM_FVAL_IPMF1_ACTION_DP, BCM_TO_DNX_ADDITIONAL_OBJ ,dnx_field_convert_color},
        [bcmFieldActionForward]                = {DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA, },
        [bcmFieldActionRedirectMcast]          = {DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_dst},
        [bcmFieldActionRedirect]               = {DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_dst},
        [bcmFieldActionL3Switch]               = {DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_rif_intf_to_dst},
        [bcmFieldActionFabricHeaderSetRaw]     = {DBAL_ENUM_FVAL_IPMF1_ACTION_SYSTEM_HEADER_PROFILE_INDEX, },
        [bcmFieldActionFabricHeaderSet]        = {DBAL_ENUM_FVAL_IPMF1_ACTION_SYSTEM_HEADER_PROFILE_INDEX, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_system_header_profile},
        [bcmFieldActionUDHData0]               = {DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_1},
        [bcmFieldActionUDHData1]               = {DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_2},
        [bcmFieldActionUDHData2]               = {DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_3},
        [bcmFieldActionUDHData3]               = {DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_4},
        [bcmFieldActionUDHBase0]               = {DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_1_TYPE},
        [bcmFieldActionUDHBase1]               = {DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_2_TYPE},
        [bcmFieldActionUDHBase2]               = {DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_3_TYPE},
        [bcmFieldActionUDHBase3]               = {DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_4_TYPE},
        [bcmFieldActionVlanActionSetNew]       = {DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_CMD_INDEX},
        [bcmFieldActionOuterVlanPrioNew]       = {DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_PCP_DEI_1},
        [bcmFieldActionOuterVlanNew]           = {DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_VID_1},
        [bcmFieldActionInnerVlanPrioNew]       = {DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_PCP_DEI_2},
        [bcmFieldActionInnerVlanNew]           = {DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_VID_2},
        [bcmFieldActionUsePolicerResult]       = {DBAL_ENUM_FVAL_IPMF1_ACTION_DP_METER_COMMAND},
        [bcmFieldActionSystemHeaderSet]        = {DBAL_ENUM_FVAL_IPMF1_ACTION_EEI},
        [bcmFieldActionVSQ]                    = {DBAL_ENUM_FVAL_IPMF1_ACTION_ST_VSQ_PTR},
        [bcmFieldActionAdmitProfile]           = {DBAL_ENUM_FVAL_IPMF1_ACTION_ADMT_PROFILE},
        [bcmFieldActionIEEE1588]               = {DBAL_ENUM_FVAL_IPMF1_ACTION_IEEE1588_DATA},
        [bcmFieldActionSrcGportNewRaw]         = {DBAL_ENUM_FVAL_IPMF1_ACTION_SRC_SYS_PORT, },
        [bcmFieldActionSrcGportNew]            = {DBAL_ENUM_FVAL_IPMF1_ACTION_SRC_SYS_PORT, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_sys_port_gport_to_port},
        [bcmFieldActionTtlSet]                 = {DBAL_ENUM_FVAL_IPMF1_ACTION_IN_TTL},
        [bcmFieldActionLatencyFlowIdRaw]       = {DBAL_ENUM_FVAL_IPMF1_ACTION_LATENCY_FLOW, },
        [bcmFieldActionLatencyFlowId]          = {DBAL_ENUM_FVAL_IPMF1_ACTION_LATENCY_FLOW, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_latency_flow_id},
        [bcmFieldActionInVport0Raw]            = {DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_IN_LIF_0, },
        [bcmFieldActionInVport1Raw]            = {DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_IN_LIF_1, },
        [bcmFieldActionIngressGportSet]        = {DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_IN_LIF_0, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_in_lif},
        [bcmFieldActionInVport0]               = {DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_IN_LIF_0, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_in_lif},
        [bcmFieldActionInVport1]               = {DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_IN_LIF_1, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_in_lif},
        [bcmFieldActionInInterface0]           = {DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_IN_LIF_0, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_rif_intf_to_lif},
        [bcmFieldActionInInterface1]           = {DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_IN_LIF_1, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_rif_intf_to_lif},
        [bcmFieldActionInVportClass0]          = {DBAL_ENUM_FVAL_IPMF1_ACTION_IN_LIF_PROFILE_0},
        [bcmFieldActionInVportClass1]          = {DBAL_ENUM_FVAL_IPMF1_ACTION_IN_LIF_PROFILE_1},
        [bcmFieldActionMirrorIngressRaw]       = {DBAL_ENUM_FVAL_IPMF1_ACTION_MIRROR_DATA, },
        [bcmFieldActionMirrorIngress]          = {DBAL_ENUM_FVAL_IPMF1_ACTION_MIRROR_DATA, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_ingress_sniff_gport_to_code},
        [bcmFieldActionStackingRouteNew]       = {DBAL_ENUM_FVAL_IPMF1_ACTION_STACKING_ROUTE_HISTORY_BITMAP},
        [bcmFieldActionOamRaw]                 = {DBAL_ENUM_FVAL_IPMF1_ACTION_OAM_DATA, },
        [bcmFieldActionOam]                    = {DBAL_ENUM_FVAL_IPMF1_ACTION_OAM_DATA, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_oam},
        [bcmFieldActionPphSnoopCode]           = {DBAL_ENUM_FVAL_IPMF1_ACTION_PPH_RESERVED},
        [bcmFieldActionTrapRaw]                = {DBAL_ENUM_FVAL_IPMF1_ACTION_FWD, },
        [bcmFieldActionTrap]                   = {DBAL_ENUM_FVAL_IPMF1_ACTION_FWD, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_trap_gport},
        [bcmFieldActionOutVport0Raw]           = {DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_0, },
        [bcmFieldActionOutVport1Raw]           = {DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_1, },
        [bcmFieldActionOutVport2Raw]           = {DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_2, },
        [bcmFieldActionOutVport3Raw]           = {DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_3, },
        [bcmFieldActionOutVport0]              = {DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_0, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_out_lif},
        [bcmFieldActionOutVport1]              = {DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_1, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_out_lif},
        [bcmFieldActionOutVport2]              = {DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_2, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_out_lif},
        [bcmFieldActionOutVport3]              = {DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_3, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_out_lif},
        [bcmFieldActionOutInterface0]          = {DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_0, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_rif_intf_to_lif},
        [bcmFieldActionOutInterface1]          = {DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_1, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_rif_intf_to_lif},
        [bcmFieldActionOutInterface2]          = {DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_2, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_rif_intf_to_lif},
        [bcmFieldActionOutInterface3]          = {DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_3, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_rif_intf_to_lif},
        [bcmFieldActionVrfSetRaw]              = {DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_DOMAIN, },
        [bcmFieldActionVrfSet]                 = {DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_DOMAIN, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_fwd_domain},
        [bcmFieldActionVSwitchNew]             = {DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_DOMAIN, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_fwd_domain},
        [bcmFieldActionContainer]              = {DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA3},
        /*
         * Mapping changed according to field.features.ecmp_tunnel_priority
         * [bcmFieldActionCbts]                   = {DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA0},
         */
        [bcmFieldActionEgressForwardingIndex]  = {DBAL_ENUM_FVAL_IPMF1_ACTION_EGRESS_PARSING_INDEX},
        [bcmFieldActionIPTProfile]             = {DBAL_ENUM_FVAL_IPMF1_ACTION_END_OF_PACKET_EDITING},
        [bcmFieldActionStatId0]                = {DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_0},
        [bcmFieldActionStatId1]                = {DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_1},
        [bcmFieldActionStatId2]                = {DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_2},
        [bcmFieldActionStatId3]                = {DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_3},
        [bcmFieldActionStatId4]                = {DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_4},
        [bcmFieldActionStatId5]                = {DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_5},
        [bcmFieldActionStatId6]                = {DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_6},
        [bcmFieldActionStatId7]                = {DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_7},
        [bcmFieldActionStatId8]                = {DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_8},
        [bcmFieldActionStatId9]                = {DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_9},
        [bcmFieldActionStatProfile0Raw]        = {DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_0, },
        [bcmFieldActionStatProfile1Raw]        = {DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_1, },
        [bcmFieldActionStatProfile2Raw]        = {DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_2, },
        [bcmFieldActionStatProfile3Raw]        = {DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_3, },
        [bcmFieldActionStatProfile4Raw]        = {DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_4, },
        [bcmFieldActionStatProfile5Raw]        = {DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_5, },
        [bcmFieldActionStatProfile6Raw]        = {DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_6, },
        [bcmFieldActionStatProfile7Raw]        = {DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_7, },
        [bcmFieldActionStatProfile8Raw]        = {DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_8, },
        [bcmFieldActionStatProfile9Raw]        = {DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_9, },
        [bcmFieldActionExtStat0Raw]            = {DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_0, },
        [bcmFieldActionExtStat1Raw]            = {DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_1, },
        [bcmFieldActionExtStat2Raw]            = {DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_2, },
        [bcmFieldActionExtStat3Raw]            = {DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_3, },
        [bcmFieldActionStatProfile0]           = {DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_0, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_stat_profile},
        [bcmFieldActionStatProfile1]           = {DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_1, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_stat_profile},
        [bcmFieldActionStatProfile2]           = {DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_2, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_stat_profile},
        [bcmFieldActionStatProfile3]           = {DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_3, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_stat_profile},
        [bcmFieldActionStatProfile4]           = {DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_4, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_stat_profile},
        [bcmFieldActionStatProfile5]           = {DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_5, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_stat_profile},
        [bcmFieldActionStatProfile6]           = {DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_6, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_stat_profile},
        [bcmFieldActionStatProfile7]           = {DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_7, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_stat_profile},
        [bcmFieldActionStatProfile8]           = {DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_8, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_stat_profile},
        [bcmFieldActionStatProfile9]           = {DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_9, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_stat_profile},
        [bcmFieldActionExtStat0]               = {DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_0, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_ext_stat_with_valid},
        [bcmFieldActionExtStat1]               = {DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_1, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_ext_stat},
        [bcmFieldActionExtStat2]               = {DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_2, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_ext_stat},
        [bcmFieldActionExtStat3]               = {DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_3, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_ext_stat},
        [bcmFieldActionIngressTimeStampInsert] = {DBAL_ENUM_FVAL_IPMF1_ACTION_INGRESS_TIME_STAMP},
        [bcmFieldActionStatMetaData]           = {DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_META_DATA},
        [bcmFieldActionInvalidNext]            = {DBAL_ENUM_FVAL_IPMF1_ACTION_INVALIDATE_NEXT},
        [bcmFieldActionForwardingTypeMeterMap] = {DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_OBJECT_10},
        [bcmFieldActionStatSamplingRaw]        = {DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICAL_SAMPLING, },
        [bcmFieldActionStatSampling]           = {DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICAL_SAMPLING, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_ingress_sniff_gport_to_code},
        [bcmFieldActionStatOamLMRaw]           = {DBAL_ENUM_FVAL_IPMF1_ACTION_STAT_OBJ_LM_READ_INDEX, },
        [bcmFieldActionStatOamLM]              = {DBAL_ENUM_FVAL_IPMF1_ACTION_STAT_OBJ_LM_READ_INDEX,BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_stat_lm_index},
        [bcmFieldActionForwardingLayerIndex]   = {DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_LAYER_INDEX},
        [bcmFieldActionVisibilityEnable]       = {DBAL_ENUM_FVAL_IPMF1_ACTION_VISIBILITY},
        [bcmFieldActionVisibilityClear]        = {DBAL_ENUM_FVAL_IPMF1_ACTION_VISIBILITY_CLEAR},
        [bcmFieldActionSmallExemLearn]         = {DBAL_ENUM_FVAL_IPMF1_ACTION_ELEPHANT_PAYLOAD},
        [bcmFieldActionPacketTraceEnable]      = {DBAL_ENUM_FVAL_IPMF1_ACTION_TRACE_PACKET_ACT},
        [bcmFieldActionBierStringOffset]       = {DBAL_ENUM_FVAL_IPMF1_ACTION_BIER_STR_OFFSET},
        [bcmFieldActionBierStringSize]         = {DBAL_ENUM_FVAL_IPMF1_ACTION_BIER_STR_SIZE},
        [bcmFieldActionPacketIsBier]           = {DBAL_ENUM_FVAL_IPMF1_ACTION_PACKET_IS_BIER},
        [bcmFieldActionEventor]                = {DBAL_ENUM_FVAL_IPMF1_ACTION_EVENTOR},
        [bcmFieldActionForwardingAdditionalInfo]  = {DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_LAYER_ADDITIONAL_INFO},
        [bcmFieldActionNetworkQos]             = {DBAL_ENUM_FVAL_IPMF1_ACTION_NWK_QOS},
        [bcmFieldActionIngressDoNotLearn]             = {DBAL_ENUM_FVAL_IPMF1_ACTION_INGRESS_LEARN_ENABLE},
        [bcmFieldActionEgressDoNotLearn]             = {DBAL_ENUM_FVAL_IPMF1_ACTION_EGRESS_LEARN_ENABLE},
        [bcmFieldActionIngressTimeStampInsertValid]    = {DBAL_ENUM_FVAL_IPMF1_ACTION_INGRESS_TIME_STAMP_OVERRIDE},
        [bcmFieldActionIpMulticastCompatible]     = {DBAL_ENUM_FVAL_IPMF1_ACTION_PACKET_IS_COMPATIBLE_MC, },
        [bcmFieldActionDstRpfGportNewRaw]         = {DBAL_ENUM_FVAL_IPMF1_ACTION_RPF_DST, },
        [bcmFieldActionDstRpfGportNew]            = {DBAL_ENUM_FVAL_IPMF1_ACTION_RPF_DST, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_dst},
        [bcmFieldActionDstRpfGportNewValid]       = {DBAL_ENUM_FVAL_IPMF1_ACTION_RPF_DST_VALID, },
        [bcmFieldActionRpfOutVportRaw]            = {DBAL_ENUM_FVAL_IPMF1_ACTION_RPF_OUT_LIF, },
        [bcmFieldActionRpfOutVport]               = {DBAL_ENUM_FVAL_IPMF1_ACTION_RPF_OUT_LIF, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_rpf_out_lif_encoded},
        [bcmFieldActionRpfOutInterface]           = {DBAL_ENUM_FVAL_IPMF1_ACTION_RPF_OUT_LIF, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_rif_intf_to_rpf_out_lif_encoded},
        [bcmFieldActionLearnRaw0]                 = {DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_0, },
        [bcmFieldActionLearnRaw1]                 = {DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_1, },
        [bcmFieldActionLearnRaw2]                 = {DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_2, },
        [bcmFieldActionLearnRaw3]                 = {DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_3, },
        [bcmFieldActionLearnRaw4]                 = {DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_4, },
        [bcmFieldActionLearnOrTransplant]         = {DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_OR_TRANSPLANT, },
        [bcmFieldActionLearnKey0]                 = {DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_KEY_0, },
        [bcmFieldActionLearnKey1]                 = {DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_KEY_1, },
        [bcmFieldActionLearnKey2Raw]              = {DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_KEY_2_APP_DB_INDEX, },
        [bcmFieldActionLearnKey2]                 = {DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_KEY_2_APP_DB_INDEX, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_learn_info_key_2},
        [bcmFieldActionLearnPayload0]             = {DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_PAYLOAD_0, },
        [bcmFieldActionLearnPayload1]             = {DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_PAYLOAD_1, },
        [bcmFieldActionLearnEntryFormatIndex]     = {DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_FORMAT_INDEX, },
        [bcmFieldActionEcnNew]                    = {DBAL_ENUM_FVAL_IPMF1_ACTION_CONGESTION_INFO},
        [bcmFieldActionPrtQualifier]              = {DBAL_ENUM_FVAL_IPMF1_ACTION_PRT_QUALIFIER},
};

static const dnx_field_action_map_t dnx_ipmf3_action_map[bcmFieldActionCount] = {
        [bcmFieldActionSnoopRaw]              = {DBAL_ENUM_FVAL_IPMF3_ACTION_SNOOP_DATA, },
        [bcmFieldActionSnoop]                 = {DBAL_ENUM_FVAL_IPMF3_ACTION_SNOOP_DATA, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_ingress_snoop_gport_to_code},
        [bcmFieldActionPphPresentSet]         = {DBAL_ENUM_FVAL_IPMF3_ACTION_PPH_TYPE},
        [bcmFieldActionPrioIntNew]            = {DBAL_ENUM_FVAL_IPMF3_ACTION_TC,},
        [bcmFieldActionDropPrecedenceRaw]     = {DBAL_ENUM_FVAL_IPMF3_ACTION_DP, },
        [bcmFieldActionDropPrecedence]        = {DBAL_ENUM_FVAL_IPMF3_ACTION_DP, BCM_TO_DNX_ADDITIONAL_OBJ ,dnx_field_convert_color},
        [bcmFieldActionForward]               = {DBAL_ENUM_FVAL_IPMF3_ACTION_DST_DATA, },
        [bcmFieldActionRedirectMcast]         = {DBAL_ENUM_FVAL_IPMF3_ACTION_DST_DATA,BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_dst},
        [bcmFieldActionRedirect     ]         = {DBAL_ENUM_FVAL_IPMF3_ACTION_DST_DATA,BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_dst},
        [bcmFieldActionL3Switch]              = {DBAL_ENUM_FVAL_IPMF3_ACTION_DST_DATA,BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_rif_intf_to_dst},
        [bcmFieldActionStartPacketStripRaw]   = {DBAL_ENUM_FVAL_IPMF3_ACTION_BYTES_TO_REMOVE, },
        [bcmFieldActionStartPacketStrip]      = {DBAL_ENUM_FVAL_IPMF3_ACTION_BYTES_TO_REMOVE, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_packet_strip},
        [bcmFieldActionVlanActionSetNew]      = {DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_CMD_INDEX},
        [bcmFieldActionOuterVlanPrioNew]      = {DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_PCP_DEI_1},
        [bcmFieldActionOuterVlanNew]          = {DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_VID_1},
        [bcmFieldActionInnerVlanPrioNew]      = {DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_PCP_DEI_2},
        [bcmFieldActionInnerVlanNew]          = {DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_VID_2},
        [bcmFieldActionUsePolicerResult]      = {DBAL_ENUM_FVAL_IPMF3_ACTION_DP_METER_COMMAND},
        [bcmFieldActionSystemHeaderSet]       = {DBAL_ENUM_FVAL_IPMF3_ACTION_EEI},
        [bcmFieldActionVSQ]                   = {DBAL_ENUM_FVAL_IPMF3_ACTION_ST_VSQ_PTR},
        [bcmFieldActionAdmitProfile]          = {DBAL_ENUM_FVAL_IPMF3_ACTION_ADMT_PROFILE},
        [bcmFieldActionIEEE1588]              = {DBAL_ENUM_FVAL_IPMF3_ACTION_IEEE1588_DATA},
        [bcmFieldActionSrcGportNewRaw]        = {DBAL_ENUM_FVAL_IPMF3_ACTION_SRC_SYS_PORT, },
        [bcmFieldActionSrcGportNew]           = {DBAL_ENUM_FVAL_IPMF3_ACTION_SRC_SYS_PORT, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_sys_port_gport_to_port},
        [bcmFieldActionTtlSet]                = {DBAL_ENUM_FVAL_IPMF3_ACTION_IN_TTL},
        [bcmFieldActionLatencyFlowIdRaw]      = {DBAL_ENUM_FVAL_IPMF3_ACTION_LATENCY_FLOW, },
        [bcmFieldActionLatencyFlowId]         = {DBAL_ENUM_FVAL_IPMF3_ACTION_LATENCY_FLOW, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_latency_flow_id},
        [bcmFieldActionInVport0Raw]           = {DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_IN_LIF_0, },
        [bcmFieldActionInVport1Raw]           = {DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_IN_LIF_1, },
        [bcmFieldActionIngressGportSet]       = {DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_IN_LIF_0, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_in_lif},
        [bcmFieldActionInVport0]              = {DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_IN_LIF_0, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_in_lif},
        [bcmFieldActionInVport1]              = {DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_IN_LIF_1, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_in_lif},
        [bcmFieldActionInInterface0]          = {DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_IN_LIF_0, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_rif_intf_to_lif},
        [bcmFieldActionInInterface1]          = {DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_IN_LIF_1, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_rif_intf_to_lif},
        [bcmFieldActionInVportClass0]         = {DBAL_ENUM_FVAL_IPMF3_ACTION_IN_LIF_PROFILE_0},
        [bcmFieldActionInVportClass1]         = {DBAL_ENUM_FVAL_IPMF3_ACTION_IN_LIF_PROFILE_1},
        [bcmFieldActionMirrorIngressRaw]      = {DBAL_ENUM_FVAL_IPMF3_ACTION_MIRROR_DATA, },
        [bcmFieldActionMirrorIngress]         = {DBAL_ENUM_FVAL_IPMF3_ACTION_MIRROR_DATA, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_ingress_sniff_gport_to_code},
        [bcmFieldActionStackingRouteNew]      = {DBAL_ENUM_FVAL_IPMF3_ACTION_STACKING_ROUTE_HISTORY_BITMAP},
        [bcmFieldActionOamRaw]                = {DBAL_ENUM_FVAL_IPMF3_ACTION_OAM_DATA, },
        [bcmFieldActionOam]                   = {DBAL_ENUM_FVAL_IPMF3_ACTION_OAM_DATA, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_oam},
        [bcmFieldActionPphSnoopCode]          = {DBAL_ENUM_FVAL_IPMF3_ACTION_PPH_RESERVED},
        [bcmFieldActionTrapRaw]               = {DBAL_ENUM_FVAL_IPMF3_ACTION_FWD, },
        [bcmFieldActionTrap]                  = {DBAL_ENUM_FVAL_IPMF3_ACTION_FWD, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_trap_gport},
        [bcmFieldActionOutVport0Raw]          = {DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_0, },
        [bcmFieldActionOutVport1Raw]          = {DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_1, },
        [bcmFieldActionOutVport2Raw]          = {DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_2, },
        [bcmFieldActionOutVport3Raw]          = {DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_3, },
        [bcmFieldActionOutVport0]             = {DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_0, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_out_lif},
        [bcmFieldActionOutVport1]             = {DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_1, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_out_lif},
        [bcmFieldActionOutVport2]             = {DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_2, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_out_lif},
        [bcmFieldActionOutVport3]             = {DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_3, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_out_lif},
        [bcmFieldActionOutInterface0]         = {DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_0, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_rif_intf_to_lif},
        [bcmFieldActionOutInterface1]         = {DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_1, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_rif_intf_to_lif},
        [bcmFieldActionOutInterface2]         = {DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_2, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_rif_intf_to_lif},
        [bcmFieldActionOutInterface3]         = {DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_3, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_rif_intf_to_lif},
        [bcmFieldActionVSwitchNew]            = {DBAL_ENUM_FVAL_IPMF3_ACTION_FWD_DOMAIN,},
        [bcmFieldActionVrfSet]                = {DBAL_ENUM_FVAL_IPMF3_ACTION_FWD_DOMAIN, BCM_TO_DNX_ADDITIONAL_OBJ},
        [bcmFieldActionTrunkHashKeySet]       = {DBAL_ENUM_FVAL_IPMF3_ACTION_LAG_LB_KEY},
        [bcmFieldActionNetworkLoadBalanceKey] = {DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_LB_KEY},
        [bcmFieldActionParsingStartTypeRaw]   = {DBAL_ENUM_FVAL_IPMF3_ACTION_PARSING_START_TYPE, },
        [bcmFieldActionParsingStartType]      = {DBAL_ENUM_FVAL_IPMF3_ACTION_PARSING_START_TYPE, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_parsing_start_type},
        [bcmFieldActionParsingStartOffsetRaw] = {DBAL_ENUM_FVAL_IPMF3_ACTION_PARSING_START_OFFSET, },
        [bcmFieldActionParsingStartOffset]    = {DBAL_ENUM_FVAL_IPMF3_ACTION_PARSING_START_OFFSET, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_parsing_start_offset},
        [bcmFieldActionEgressForwardingIndex] = {DBAL_ENUM_FVAL_IPMF3_ACTION_EGRESS_PARSING_INDEX},
        [bcmFieldActionUDHData0]              = {DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_1},
        [bcmFieldActionUDHData1]              = {DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_2},
        [bcmFieldActionUDHData2]              = {DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_3},
        [bcmFieldActionUDHData3]              = {DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_4},
        [bcmFieldActionUDHBase]               = {DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADERS_TYPE},
        [bcmFieldActionIPTCommandRaw]         = {DBAL_ENUM_FVAL_IPMF3_ACTION_INT_DATA,},
        [bcmFieldActionIPTCommand]            = {DBAL_ENUM_FVAL_IPMF3_ACTION_INT_DATA, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_ipt_command},
        [bcmFieldActionStatId0]               = {DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_0},
        [bcmFieldActionStatId1]               = {DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_1},
        [bcmFieldActionStatId2]               = {DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_2},
        [bcmFieldActionStatId3]               = {DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_3},
        [bcmFieldActionStatId4]               = {DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_4},
        [bcmFieldActionStatId5]               = {DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_5},
        [bcmFieldActionStatId6]               = {DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_6},
        [bcmFieldActionStatId7]               = {DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_7},
        [bcmFieldActionStatId8]               = {DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_8},
        [bcmFieldActionStatId9]               = {DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_9},
        [bcmFieldActionStatProfile0Raw]       = {DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_0, },
        [bcmFieldActionStatProfile1Raw]       = {DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_1, },
        [bcmFieldActionStatProfile2Raw]       = {DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_2, },
        [bcmFieldActionStatProfile3Raw]       = {DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_3, },
        [bcmFieldActionStatProfile4Raw]       = {DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_4, },
        [bcmFieldActionStatProfile5Raw]       = {DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_5, },
        [bcmFieldActionStatProfile6Raw]       = {DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_6, },
        [bcmFieldActionStatProfile7Raw]       = {DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_7, },
        [bcmFieldActionStatProfile8Raw]       = {DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_8, },
        [bcmFieldActionStatProfile9Raw]       = {DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_9, },
        [bcmFieldActionStatProfile0]          = {DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_0, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_stat_profile},
        [bcmFieldActionStatProfile1]          = {DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_1, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_stat_profile},
        [bcmFieldActionStatProfile2]          = {DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_2, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_stat_profile},
        [bcmFieldActionStatProfile3]          = {DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_3, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_stat_profile},
        [bcmFieldActionStatProfile4]          = {DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_4, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_stat_profile},
        [bcmFieldActionStatProfile5]          = {DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_5, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_stat_profile},
        [bcmFieldActionStatProfile6]          = {DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_6, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_stat_profile},
        [bcmFieldActionStatProfile7]          = {DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_7, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_stat_profile},
        [bcmFieldActionStatProfile8]          = {DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_8, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_stat_profile},
        [bcmFieldActionStatProfile9]          = {DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_9, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_stat_profile},
        [bcmFieldActionStatMetaData]          = {DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_META_DATA},
        [bcmFieldActionInvalidNext]           = {DBAL_ENUM_FVAL_IPMF3_ACTION_INVALIDATE_NEXT},
        [bcmFieldActionExtStat0Raw]           = {DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_0, },
        [bcmFieldActionExtStat1Raw]           = {DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_1, },
        [bcmFieldActionExtStat2Raw]           = {DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_2, },
        [bcmFieldActionExtStat3Raw]           = {DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_3, },
        [bcmFieldActionExtStat0]              = {DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_0, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_ext_stat_with_valid},
        [bcmFieldActionExtStat1]              = {DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_1, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_ext_stat},
        [bcmFieldActionExtStat2]              = {DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_2, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_ext_stat},
        [bcmFieldActionExtStat3]              = {DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_3, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_ext_stat},
        [bcmFieldActionForwardingTypeMeterMap]= {DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_OBJECT_10},
        [bcmFieldActionStatSamplingRaw]       = {DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICAL, },
        [bcmFieldActionStatSampling]          = {DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICAL, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_ingress_sniff_gport_to_code},
        [bcmFieldActionIPTProfile]            = {DBAL_ENUM_FVAL_IPMF3_ACTION_END_OF_PACKET_EDITING},
        [bcmFieldActionStatOamLMRaw]          = {DBAL_ENUM_FVAL_IPMF3_ACTION_STAT_OBJ_LM_READ_INDEX, },
        [bcmFieldActionStatOamLM]             = {DBAL_ENUM_FVAL_IPMF3_ACTION_STAT_OBJ_LM_READ_INDEX,BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_stat_lm_index},
        [bcmFieldActionForwardingLayerIndex]  = {DBAL_ENUM_FVAL_IPMF3_ACTION_FWD_LAYER_INDEX},
        [bcmFieldActionVisibilityEnable]      = {DBAL_ENUM_FVAL_IPMF3_ACTION_VISIBILITY},
        [bcmFieldActionBierStringOffset]      = {DBAL_ENUM_FVAL_IPMF3_ACTION_BIER_STR_OFFSET},
        [bcmFieldActionBierStringSize]        = {DBAL_ENUM_FVAL_IPMF3_ACTION_BIER_STR_SIZE},
        [bcmFieldActionPacketIsBier]          = {DBAL_ENUM_FVAL_IPMF3_ACTION_PACKET_IS_BIER},
        [bcmFieldActionForwardingAdditionalInfo]  = {DBAL_ENUM_FVAL_IPMF3_ACTION_FWD_LAYER_ADDITIONAL_INFO},
        [bcmFieldActionSystemHeaderSizeAdjust]      = {DBAL_ENUM_FVAL_IPMF3_ACTION_SYSTEM_HEADER_SIZE_ADJUST},
        [bcmFieldActionNetworkQos]             = {DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_QOS},
        [bcmFieldActionAppendPointerCompensation]  = {DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_HEADER_APPEND_SIZE},
        [bcmFieldActionIngressTimeStampInsertValid]    = {DBAL_ENUM_FVAL_IPMF3_ACTION_INGRESS_TIME_STAMP_OVERRIDE},
        [bcmFieldActionLearnRaw0]                = {DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_0, },
        [bcmFieldActionLearnRaw1]                = {DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_1, },
        [bcmFieldActionLearnRaw2]                = {DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_2, },
        [bcmFieldActionLearnRaw3]                = {DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_3, },
        [bcmFieldActionLearnRaw4]                = {DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_4, },
        [bcmFieldActionLearnOrTransplant]        = {DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_OR_TRANSPLANT, },
        [bcmFieldActionLearnKey0]                = {DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_KEY_0, },
        [bcmFieldActionLearnKey1]                = {DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_KEY_1, },
        [bcmFieldActionLearnKey2Raw]             = {DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_KEY_2_APP_DB_INDEX, },
        [bcmFieldActionLearnKey2]                = {DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_KEY_2_APP_DB_INDEX, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_learn_info_key_2},
        [bcmFieldActionLearnPayload0]            = {DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_PAYLOAD_0, },
        [bcmFieldActionLearnPayload1]            = {DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_PAYLOAD_1, },
        [bcmFieldActionLearnEntryFormatIndex]    = {DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_FORMAT_INDEX, },
        [bcmFieldActionEcnNew]                   = {DBAL_ENUM_FVAL_IPMF3_ACTION_CONGESTION_INFO},
        [bcmFieldActionFabricHeaderSetRaw]       = {DBAL_ENUM_FVAL_IPMF3_ACTION_SYSTEM_HEADER_PROFILE_INDEX, },
        [bcmFieldActionFabricHeaderSet]          = {DBAL_ENUM_FVAL_IPMF3_ACTION_SYSTEM_HEADER_PROFILE_INDEX, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_system_header_profile},
        [bcmFieldActionRepCopy]                  = {DBAL_ENUM_FVAL_IPMF3_ACTION_TM_ACTION_TYPE, 0, dnx_field_convert_rep_copy_type},
};

static const dnx_field_action_map_t dnx_epmf_action_map[bcmFieldActionCount] = {
        [bcmFieldActionPrioIntNew]                  = {DBAL_ENUM_FVAL_EPMF_ACTION_TC, },
        [bcmFieldActionDropPrecedenceRaw]           = {DBAL_ENUM_FVAL_EPMF_ACTION_DP, },
        [bcmFieldActionDropPrecedence]              = {DBAL_ENUM_FVAL_EPMF_ACTION_DP, BCM_TO_DNX_ADDITIONAL_OBJ ,dnx_field_convert_color},
        [bcmFieldActionQosMapIdNew]                 = {DBAL_ENUM_FVAL_EPMF_ACTION_COS_PROFILE},
        [bcmFieldActionMirrorEgressRaw]             = {DBAL_ENUM_FVAL_EPMF_ACTION_MIRROR_PROFILE, },
        [bcmFieldActionMirrorEgress]                = {DBAL_ENUM_FVAL_EPMF_ACTION_MIRROR_PROFILE, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_egress_sniff_gport_to_profile},
        [bcmFieldActionAceEntryId]                  = {DBAL_ENUM_FVAL_EPMF_ACTION_CUD_OUTLIF_OR_MCDB_PTR},
        [bcmFieldActionStatId0]                     = {DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_0_PTR},
        [bcmFieldActionStatProfile0]                = {DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_0_PROFILE},
        [bcmFieldActionStatId1]                     = {DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_1_PTR},
        [bcmFieldActionStatProfile1]                = {DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_1_PROFILE},
        [bcmFieldActionOutPortTrafficManagement]    = {DBAL_ENUM_FVAL_EPMF_ACTION_PP_DSP},
        [bcmFieldActionInvalidNext]                 = {DBAL_ENUM_FVAL_EPMF_ACTION_INVALIDATE_NEXT},
        [bcmFieldActionTrap]                        = {DBAL_ENUM_FVAL_EPMF_ACTION_FWD_ACTION_PROFILE_INDEX, 0, dnx_field_convert_egress_trap_id},
        [bcmFieldActionTrapStrengthRaw]             = {DBAL_ENUM_FVAL_EPMF_ACTION_PMF_FWD_STRENGTH, },
        [bcmFieldActionTrapStrength]                = {DBAL_ENUM_FVAL_EPMF_ACTION_PMF_FWD_STRENGTH, BCM_TO_DNX_ADDITIONAL_OBJ , dnx_field_convert_egress_trap_gport_to_strength},
        [bcmFieldActionSnoopRaw]                    = {DBAL_ENUM_FVAL_EPMF_ACTION_SNOOP_PROFILE , },
        [bcmFieldActionSnoopStrengthRaw]            = {DBAL_ENUM_FVAL_EPMF_ACTION_SNOOP_STRENGTH, },
        [bcmFieldActionSnoop]                       = {DBAL_ENUM_FVAL_EPMF_ACTION_SNOOP_PROFILE , BCM_TO_DNX_ADDITIONAL_OBJ , dnx_field_convert_egress_sniff_gport_to_profile},
        [bcmFieldActionSnoopStrength]               = {DBAL_ENUM_FVAL_EPMF_ACTION_SNOOP_STRENGTH, BCM_TO_DNX_ADDITIONAL_OBJ , dnx_field_convert_egress_snoop_gport_to_strength},
        [bcmFieldActionDrop]                        = {DBAL_ENUM_FVAL_EPMF_ACTION_DISCARD},
        [bcmFieldActionEstimatedStartPacketStrip]   = {DBAL_ENUM_FVAL_EPMF_ACTION_EBTR_SOP, },};

static const dnx_field_action_map_t dnx_ace_action_map[bcmFieldActionCount] = {
    [bcmFieldActionPrioIntNew]         = {DBAL_ENUM_FVAL_ACE_ACTION_FTMH_TC,              },
    [bcmFieldActionDropPrecedenceRaw]  = {DBAL_ENUM_FVAL_ACE_ACTION_FTMH_DP, },
    [bcmFieldActionDropPrecedence]     = {DBAL_ENUM_FVAL_ACE_ACTION_FTMH_DP, BCM_TO_DNX_ADDITIONAL_OBJ ,dnx_field_convert_color},
    [bcmFieldActionIPTProfile]         = {DBAL_ENUM_FVAL_ACE_ACTION_PPH_END_OF_PACKET_EDITING},
    [bcmFieldActionInterfaceClassVPort]= {DBAL_ENUM_FVAL_ACE_ACTION_PPH_IN_LIF_PROFILE },
    [bcmFieldActionTtlSet]             = {DBAL_ENUM_FVAL_ACE_ACTION_PPH_TTL},
    [bcmFieldActionAceContextValueRaw] = {DBAL_ENUM_FVAL_ACE_ACTION_ACE_CONTEXT_VALUE, },
    [bcmFieldActionAceContextValue]    = {DBAL_ENUM_FVAL_ACE_ACTION_ACE_CONTEXT_VALUE, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_ace_context},
    [bcmFieldActionEcnNew]             = {DBAL_ENUM_FVAL_ACE_ACTION_FTMH_CNI},
    [bcmFieldActionStat0Raw]           = {DBAL_ENUM_FVAL_ACE_ACTION_ACE_STAT_METER_OBJ_CMD_ID, },
    [bcmFieldActionStat1Raw]           = {DBAL_ENUM_FVAL_ACE_ACTION_ACE_STAT_OBJ_CMD_ID_VALID, },
    [bcmFieldActionStat0]              = {DBAL_ENUM_FVAL_ACE_ACTION_ACE_STAT_METER_OBJ_CMD_ID, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_ace_stat_meter_object},
    [bcmFieldActionStat1]              = {DBAL_ENUM_FVAL_ACE_ACTION_ACE_STAT_OBJ_CMD_ID_VALID, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_ace_stat_counter_object},
    [bcmFieldActionLearnSrcPortNew]    = {DBAL_ENUM_FVAL_ACE_ACTION_FTMH_SOURCE_SYSTEM_PORT},
    [bcmFieldActionInvalidNext]        = {DBAL_ENUM_FVAL_ACE_ACTION_INVALIDATE_NEXT},
    [bcmFieldActionOutVport0Raw]       = {DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF0_CUD_OUT_LIF0VALID,},
    [bcmFieldActionOutVport1Raw]       = {DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF1_CUD_OUT_LIF1VALID, },
    [bcmFieldActionOutVport2Raw]       = {DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF2_CUD_OUT_LIF2VALID, },
    [bcmFieldActionOutVport3Raw]       = {DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF3_CUD_OUT_LIF3VALID, },
    [bcmFieldActionOutVport0]          = {DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF0_CUD_OUT_LIF0VALID, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_out_lif_add_valid},
    [bcmFieldActionOutVport1]          = {DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF1_CUD_OUT_LIF1VALID, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_out_lif_add_valid},
    [bcmFieldActionOutVport2]          = {DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF2_CUD_OUT_LIF2VALID, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_out_lif_add_valid},
    [bcmFieldActionOutVport3]          = {DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF3_CUD_OUT_LIF3VALID, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_gport_to_global_out_lif_add_valid},
    [bcmFieldActionOutInterface0]      = {DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF0_CUD_OUT_LIF0VALID, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_rif_intf_to_lif_add_valid},
    [bcmFieldActionOutInterface1]      = {DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF1_CUD_OUT_LIF1VALID, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_rif_intf_to_lif_add_valid},
    [bcmFieldActionOutInterface2]      = {DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF2_CUD_OUT_LIF2VALID, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_rif_intf_to_lif_add_valid},
    [bcmFieldActionOutInterface3]      = {DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF3_CUD_OUT_LIF3VALID, BCM_TO_DNX_ADDITIONAL_OBJ, dnx_field_convert_rif_intf_to_lif_add_valid},
    [bcmFieldActionForwardingAdditionalInfo]    = {DBAL_ENUM_FVAL_ACE_ACTION_PPH_FORWARDING_LAYER_ADDITIONAL_INFO},
    [bcmFieldActionForwardingLayerIndex]  = {DBAL_ENUM_FVAL_ACE_ACTION_PPH_FWD_LAYER_INDEX, },
    [bcmFieldActionNetworkQos]            = {DBAL_ENUM_FVAL_ACE_ACTION_PPH_NWK_QOS, },
    [bcmFieldActionDstClassField]         = {DBAL_ENUM_FVAL_ACE_ACTION_PPH_VALUE1, },
    [bcmFieldActionSrcClassField]         = {DBAL_ENUM_FVAL_ACE_ACTION_PPH_VALUE2, },
    [bcmFieldActionClassField]            = {DBAL_ENUM_FVAL_ACE_ACTION_FHEI_EXT31TO00, },
};

dnx_field_base_action_info_t dnx_sw_action_info[DNX_FIELD_SW_ACTION_NOF] = {
    [DNX_FIELD_SW_ACTION_VOID]                                  = {DBAL_FIELD_VOID, {""}, 32}
};

static const dnx_field_base_action_info_t dnx_ipmf1_action_info[] = {
    [DBAL_ENUM_FVAL_IPMF1_ACTION_ACL_CONTEXT]                   = {DBAL_FIELD_ACL_CONTEXT},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_ADMT_PROFILE]                  = {DBAL_FIELD_ADMT_PROFILE, {"Admt_Profile"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_BIER_STR_OFFSET]               = {DBAL_FIELD_BIER_STR_OFFSET, {"BIER_String_Offset"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_BIER_STR_SIZE]                 = {DBAL_FIELD_BIER_STR_SIZE, {"BIER_String_Size"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_CONGESTION_INFO]               = {DBAL_FIELD_CONGESTION_INFO, {"Congestion_Info"}},
    /* Value (21) + Qualifier (11) */
    [DBAL_ENUM_FVAL_IPMF1_ACTION_DST_DATA]                      = {DBAL_FIELD_DST_DATA, {"Fwd_Action_Dst.type", "Fwd_Action_Dst.value"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_DP]                            = {DBAL_FIELD_DP, {"DP"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_DP_METER_COMMAND]              = {DBAL_FIELD_DP_METER_COMMAND, {"DP_Meter_Cmd"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_EEI]                           = {DBAL_FIELD_EEI, {"Sys_Header_Profile_Index"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_EGRESS_LEARN_ENABLE]           = {DBAL_FIELD_EGRESS_LEARN_ENABLE, {"Egress_Learn_Enable"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_ELEPHANT_PAYLOAD]              = {DBAL_FIELD_ELEPHANT_PAYLOAD},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_TRACE_PACKET_ACT]              = {DBAL_FIELD_TRACE_PACKET_ACT},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_END_OF_PACKET_EDITING]         = {DBAL_FIELD_END_OF_PACKET_EDITING, {"End_of_Pkt_Editing"}},
    /* Assuming that each statistics is valid(1) + opcode(2) + data(20) */
    [DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_0]              = {DBAL_FIELD_EXT_STATISTICS_0, {"Ext_Stats_Valid", "Ext_Stats_Opcode", "Ext_Stats"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_1]              = {DBAL_FIELD_EXT_STATISTICS_1, {"Ext_Stats_Valid", "Ext_Stats_Opcode", "Ext_Stats"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_2]              = {DBAL_FIELD_EXT_STATISTICS_2, {"Ext_Stats_Valid", "Ext_Stats_Opcode", "Ext_Stats"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_EXT_STATISTICS_3]              = {DBAL_FIELD_EXT_STATISTICS_3, {"Ext_Stats_Valid", "Ext_Stats_Opcode", "Ext_Stats"}},
    /*LSB CPU-Trap-Code(9) , Forward-Strength(4) , CPU-Trap-Qualifier(19) MSB*/
    [DBAL_ENUM_FVAL_IPMF1_ACTION_FWD]                           = {DBAL_FIELD_FWD_TRAP, {"Fwd_Action_CPU_Trap_Code", "Fwd_Action_Strength", "Fwd_Action_CPU_Trap_Qualifier"}},
    /* HW action writes to both Fwd_Domain_ID and Fwd_Domain_Profile, but by DBAL field size we only write to Fwd_Domain_ID, and Fwd_Domain_Profile is filled with zeros.*/
    [DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_DOMAIN]                    = {DBAL_FIELD_FWD_DOMAIN, {"Fwd_Domain_ID.type", "Fwd_Domain_ID.value"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_LAYER_ADDITIONAL_INFO]     = {DBAL_FIELD_FWD_LAYER_ADDITIONAL_INFO, {"Fwd_Layer_Additional_Info"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_CONTEXT]                   = {DBAL_FIELD_FWD_CONTEXT, {"Fwd_Context"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_FWD_LAYER_INDEX]               = {DBAL_FIELD_FWD_LAYER_INDEX, {"Fwd_Layer_Index"}},
    /* {Packet-is(1), Update-Time-Stamp(1), Command(2), Encaps(1), Offset(8)} */
    [DBAL_ENUM_FVAL_IPMF1_ACTION_IEEE1588_DATA]                 = {DBAL_FIELD_IEEE1588_DATA,
        {"IEEE1588_Cmd", "IEEE1588_Compensate_Time_Stamp", "IEEE1588_Enc", "IEEE1588_Header_Offset", "Pkt_is_IEEE1588"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_INGRESS_LEARN_ENABLE]          = {DBAL_FIELD_INGRESS_LEARN_ENABLE, {"Ingress_Learn_Enable"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_IN_LIF_0]                 = {DBAL_FIELD_GLOB_IN_LIF_0, {"In_LIFs.0"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_IN_LIF_1]                 = {DBAL_FIELD_GLOB_IN_LIF_1, {"In_LIFs.1"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_IN_LIF_PROFILE_0]              = {DBAL_FIELD_IN_LIF_PROFILE_0, {"In_LIF_Profiles.0"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_IN_LIF_PROFILE_1]              = {DBAL_FIELD_IN_LIF_PROFILE_1, {"In_LIF_Profiles.1"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_IN_PP_PORT]                    = {DBAL_FIELD_PP_PORT},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_IN_TTL]                        = {DBAL_FIELD_IN_TTL, {"In_TTL"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_ITPP_DELTA]                    = {DBAL_FIELD_ITPP_DELTA_AND_VALID, {"ITPP_Delta"}},
    /* { latency_flow_profile(4), latency_flow_id(19), latency_flow_id_valid(1) } */
    [DBAL_ENUM_FVAL_IPMF1_ACTION_LATENCY_FLOW]                  = {DBAL_FIELD_LATENCY_FLOW, {"Latency_Flow_Profile", "Latency_Flow_ID", "Latency_Flow_ID_Valid"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_0]                  = {DBAL_FIELD_LEARN_INFO_0},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_1]                  = {DBAL_FIELD_LEARN_INFO_1},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_2]                  = {DBAL_FIELD_LEARN_INFO_2},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_3]                  = {DBAL_FIELD_LEARN_INFO_3},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_4]                  = {DBAL_FIELD_LEARN_INFO_4},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_OR_TRANSPLANT]           = {DBAL_FIELD_LEARN_OR_TRANSPLANT, {"Learn_Info.Learn_Or_Transplant"}},
    /* Mirror-Code (9) + Qualifier(8) */
    [DBAL_ENUM_FVAL_IPMF1_ACTION_MIRROR_DATA]                   = {DBAL_FIELD_MIRROR_DATA, {"Mirror_Code", "Mirror_Qualifier"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_NWK_QOS]                       = {DBAL_FIELD_NWK_QOS, {"Nwk_QOS"}},
    /* Oam-Up-Mep(1), Oam-Sub-Type(4), Oam-Offset(8), * Oam-Stamp-Offset(8) */
    [DBAL_ENUM_FVAL_IPMF1_ACTION_OAM_DATA]                      = {DBAL_FIELD_OAM_DATA, {"OAM_Up_MEP", "OAM_Sub_Type", "OAM_Offset", "OAM_Stamp_Offset"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_OAM_ID]                        = {DBAL_FIELD_PMF_OAM_ID, {"OAM_ID"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_0]                = {DBAL_FIELD_GLOB_OUT_LIF_0, {"All_Out_LIFs.0"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_1]                = {DBAL_FIELD_GLOB_OUT_LIF_1, {"All_Out_LIFs.1"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_2]                = {DBAL_FIELD_GLOB_OUT_LIF_2, {"All_Out_LIFs.2"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_GLOB_OUT_LIF_3]                = {DBAL_FIELD_GLOB_OUT_LIF_3, {"All_Out_LIFs.3"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_OUT_LIF_PUSH]                  = {DBAL_FIELD_OUT_LIF_PUSH},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_PACKET_IS_COMPATIBLE_MC]       = {DBAL_FIELD_PACKET_IS_COMPATIBLE_MC, {"Pkt_is_Compatible_MC"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_PACKET_IS_BIER]                = {DBAL_FIELD_PACKET_IS_BIER, {"Pkt_is_BIER"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_EGRESS_PARSING_INDEX]          = {DBAL_FIELD_EGRESS_PARSING_INDEX, {"Egress_Parsing_Index"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA0]             = {DBAL_FIELD_PEM_GENERAL_DATA0, {"General_Data"}},
    /*
     * Mapping changed according to field.features.ecmp_tunnel_priority
     * [DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA0]             = {DBAL_FIELD_PEM_GENERAL_DATA_CBTS, {"General_Data"}},
     */
    [DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA1]             = {DBAL_FIELD_PEM_GENERAL_DATA1, {"General_Data"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA2]             = {DBAL_FIELD_PEM_GENERAL_DATA2, {"General_Data"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA3]             = {DBAL_FIELD_PEM_GENERAL_DATA3, {"General_Data.PMF_Container"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_PPH_RESERVED]                  = {DBAL_FIELD_PPH_RESERVED, {"PPH_Reserved"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_PPH_TYPE]                      = {DBAL_FIELD_PPH_TYPE, {"PPH_Type"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_RPF_DST]                       = {DBAL_FIELD_RPF_DST, {"RPF_Dst"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_RPF_DST_VALID]                 = {DBAL_FIELD_RPF_DST_VALID, {"RPF_Dst_Valid"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_RPF_OUT_LIF]                   = {DBAL_FIELD_RPF_OUT_LIF_ENCODED, {"RPF_Route_Valid", "RPF_Default_Route_Found", "RPF_Out_LIF"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_PTC]                           = {DBAL_FIELD_PTC, {"PTC"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_PRT_QUALIFIER]                 = {DBAL_FIELD_PRT_QUALIFIER_RAW, {"Prt_Qualifier"}},
    /* sampling_qualifier(16), sampling_code(9) */
    [DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICAL_SAMPLING]          = {DBAL_FIELD_STATISTICAL_SAMPLING, {"Statistical_Sampling_Qualifier", "Statistical_Sampling_Code"}},
    /* Snoop-Code(9) + Snoop-Strength(3) + Qualifier (8) */
    [DBAL_ENUM_FVAL_IPMF1_ACTION_SNOOP_DATA]                    = {DBAL_FIELD_SNOOP_DATA, {"Snoop_Code", "Snoop_Qualifier", "Snoop_Strength"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_SRC_SYS_PORT]                  = {DBAL_FIELD_SRC_SYS_PORT, {"Src_Sys_Port_ID"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_STACKING_ROUTE_HISTORY_BITMAP] = {DBAL_FIELD_STACKING_ROUTE_HISTORY_BITMAP, {"Stacking_Route_History_Bitmap"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_PAYLOAD0]                  = {DBAL_FIELD_SLB_PAYLOAD0},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_PAYLOAD1]                  = {DBAL_FIELD_SLB_PAYLOAD1},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_KEY0]                      = {DBAL_FIELD_SLB_KEY0},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_KEY1]                      = {DBAL_FIELD_SLB_KEY1},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_KEY2]                      = {DBAL_FIELD_SLB_KEY2},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_SLB_FOUND]                     = {DBAL_FIELD_SLB_FOUND, {"SLB_Found"}},
    /* Address(10), Data(8), Write(1) */
    [DBAL_ENUM_FVAL_IPMF1_ACTION_STATE_ADDRESS_DATA]            = {DBAL_FIELD_STATE_ADDRESS_DATA},
     /* Data(8), Write(1) */
    [DBAL_ENUM_FVAL_IPMF1_ACTION_STATE_DATA]                    = {DBAL_FIELD_STATE_DATA},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_0]               = {DBAL_FIELD_STATISTICS_ID_0, {"Stats_Objects_Resolved.1"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_1]               = {DBAL_FIELD_STATISTICS_ID_1, {"Stats_Objects_Resolved.2"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_2]               = {DBAL_FIELD_STATISTICS_ID_2, {"Stats_Objects_Resolved.3"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_3]               = {DBAL_FIELD_STATISTICS_ID_3, {"Stats_Objects_Resolved.4"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_4]               = {DBAL_FIELD_STATISTICS_ID_4, {"Stats_Objects_Resolved.5"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_5]               = {DBAL_FIELD_STATISTICS_ID_5, {"Stats_Objects_Resolved.6"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_6]               = {DBAL_FIELD_STATISTICS_ID_6, {"Stats_Objects_Resolved.7"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_7]               = {DBAL_FIELD_STATISTICS_ID_7, {"Stats_Objects_Resolved.8"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_8]               = {DBAL_FIELD_STATISTICS_ID_8, {"Stats_Objects_Resolved.9"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ID_9]               = {DBAL_FIELD_STATISTICS_ID_9, {"Stats_Objects_Resolved.10"}},
    /* valid, Type(2), Is_LM(1), Is_Meter(1)}  <-- lsb */
    [DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_0]              = {DBAL_FIELD_STATISTICS_ATR_0,
        {"Stats_Objects_Atr.1.Is_Meter", "Stats_Objects_Atr.1.Is_LM", "Stats_Objects_Atr.1.Type", "Stats_Objects_Atr.1.Valid"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_1]              = {DBAL_FIELD_STATISTICS_ATR_1,
        {"Stats_Objects_Atr.2.Is_Meter", "Stats_Objects_Atr.2.Is_LM", "Stats_Objects_Atr.2.Type", "Stats_Objects_Atr.2.Valid"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_2]              = {DBAL_FIELD_STATISTICS_ATR_2,
        {"Stats_Objects_Atr.3.Is_Meter", "Stats_Objects_Atr.3.Is_LM", "Stats_Objects_Atr.3.Type", "Stats_Objects_Atr.3.Valid"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_3]              = {DBAL_FIELD_STATISTICS_ATR_3,
        {"Stats_Objects_Atr.4.Is_Meter", "Stats_Objects_Atr.4.Is_LM", "Stats_Objects_Atr.4.Type", "Stats_Objects_Atr.4.Valid"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_4]              = {DBAL_FIELD_STATISTICS_ATR_4,
        {"Stats_Objects_Atr.5.Is_Meter", "Stats_Objects_Atr.5.Is_LM", "Stats_Objects_Atr.5.Type", "Stats_Objects_Atr.5.Valid"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_5]              = {DBAL_FIELD_STATISTICS_ATR_5,
        {"Stats_Objects_Atr.6.Is_Meter", "Stats_Objects_Atr.6.Is_LM", "Stats_Objects_Atr.6.Type", "Stats_Objects_Atr.6.Valid"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_6]              = {DBAL_FIELD_STATISTICS_ATR_6,
        {"Stats_Objects_Atr.7.Is_Meter", "Stats_Objects_Atr.7.Is_LM", "Stats_Objects_Atr.7.Type", "Stats_Objects_Atr.7.Valid"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_7]              = {DBAL_FIELD_STATISTICS_ATR_7,
        {"Stats_Objects_Atr.8.Is_Meter", "Stats_Objects_Atr.8.Is_LM", "Stats_Objects_Atr.8.Type", "Stats_Objects_Atr.8.Valid"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_8]              = {DBAL_FIELD_STATISTICS_ATR_8,
        {"Stats_Objects_Atr.9.Is_Meter", "Stats_Objects_Atr.9.Is_LM", "Stats_Objects_Atr.9.Type", "Stats_Objects_Atr.9.Valid"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_ATR_9]              = {DBAL_FIELD_STATISTICS_ATR_9,
        {"Stats_Objects_Atr.10.Is_Meter", "Stats_Objects_Atr.10.Is_LM", "Stats_Objects_Atr.10.Type", "Stats_Objects_Atr.10.Valid"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_META_DATA]          = {DBAL_FIELD_STATISTICS_META_DATA, {"Stats_Meta_Data"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_STATISTICS_OBJECT_10]          = {DBAL_FIELD_STATISTICS_OBJECT_10, {"Stats_Obj_10"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_ST_VSQ_PTR]                    = {DBAL_FIELD_ST_VSQ_PTR, {"St_VSQ_Ptr"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_SYSTEM_HEADER_PROFILE_INDEX]   = {DBAL_FIELD_SYSTEM_HEADER_PROFILE_INDEX, {"Sys_Header_Profile_Index"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_TC]                            = {DBAL_FIELD_TC, {"TC"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_TM_PROFILE]                    = {DBAL_FIELD_TM_PROFILE, {"TM_Profile"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_1]                 = {DBAL_FIELD_USER_HEADER_1, {"User_Header_1"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_1_TYPE]            = {DBAL_FIELD_USER_HEADER_1_TYPE, {"User_Headers_Type"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_2]                 = {DBAL_FIELD_USER_HEADER_2, {"User_Header_2"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_2_TYPE]            = {DBAL_FIELD_USER_HEADER_2_TYPE, {"User_Headers_Type"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_3]                 = {DBAL_FIELD_USER_HEADER_3, {"User_Header_3"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_3_TYPE]            = {DBAL_FIELD_USER_HEADER_3_TYPE, {"User_Headers_Type"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_4]                 = {DBAL_FIELD_USER_HEADER_4, {"User_Header_4"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_USER_HEADER_4_TYPE]            = {DBAL_FIELD_USER_HEADER_4_TYPE, {"User_Headers_Type"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_CMD_INDEX]           = {DBAL_FIELD_VLAN_EDIT_CMD_INDEX, {"VLAN_Edit_Cmd.Cmd"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_PCP_DEI_1]           = {DBAL_FIELD_VLAN_EDIT_PCP_DEI_1, {"VLAN_Edit_Cmd.DEI_PCP1"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_PCP_DEI_2]           = {DBAL_FIELD_VLAN_EDIT_PCP_DEI_2, {"VLAN_Edit_Cmd.DEI_PCP2"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_VID_1]               = {DBAL_FIELD_VLAN_EDIT_VID_1, {"VLAN_Edit_Cmd.VID_1"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_VLAN_EDIT_VID_2]               = {DBAL_FIELD_VLAN_EDIT_VID_2, {"VLAN_Edit_Cmd.VID_2"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_INGRESS_TIME_STAMP]            = {DBAL_FIELD_INGRESS_TIME_STAMP, {"Ingress_Time_Stamp"}},
    /* int_profile(8), int_command(2). INT_DATA action does not work in iPMF1/2.*/
    [DBAL_ENUM_FVAL_IPMF1_ACTION_INT_DATA]                      = {DBAL_FIELD_INT_DATA},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_INGRESS_TIME_STAMP_OVERRIDE]   = {DBAL_FIELD_INGRESS_TIME_STAMP_OVERRIDE, {"Ingress_Time_Stamp_Override"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_EVENTOR]                       = {DBAL_FIELD_EVENTOR},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_STAT_OBJ_LM_READ_INDEX]        = {DBAL_FIELD_STAT_OBJ_LM_READ_INDEX, {"Stats_Obj_Lm_Rd_Index"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_KEY_0]              = {DBAL_FIELD_LEARN_INFO_KEY_0, {"Learn_Info.Key(Full_Learn_Key)"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_KEY_1]              = {DBAL_FIELD_LEARN_INFO_KEY_1, {"Learn_Info.Key(Full_Learn_Key)"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_KEY_2_APP_DB_INDEX] = {DBAL_FIELD_LEARN_INFO_KEY_2_APP_DB_INDEX, {"Learn_Info.App_DB_Index", "Learn_Info.Key(Full_Learn_Key)"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_PAYLOAD_0]          = {DBAL_FIELD_LEARN_INFO_PAYLOAD_0, {"Learn_Info.Learn_Data"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_PAYLOAD_1]          = {DBAL_FIELD_LEARN_INFO_PAYLOAD_1, {"Learn_Info.Learn_Data"}},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_LEARN_INFO_FORMAT_INDEX]       = {DBAL_FIELD_LEARN_INFO_FORMAT_INDEX, {"Learn_Info.Entry_Format_Index"}},
    /* not in use so no need in DBAL field */
    [DBAL_ENUM_FVAL_IPMF1_ACTION_VISIBILITY_CLEAR]              = {DBAL_FIELD_VISIBILITY_CLEAR},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_VISIBILITY]                    = {DBAL_FIELD_VISIBILITY},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_INVALIDATE_NEXT]               = {DBAL_FIELD_INVALIDATE_NEXT},
    [DBAL_ENUM_FVAL_IPMF1_ACTION_INVALID]                       = {DBAL_FIELD_INVALID}
};

static const dnx_field_base_action_info_t dnx_ipmf3_action_info[] = {
    [DBAL_ENUM_FVAL_IPMF3_ACTION_ACL_CONTEXT]                   = {DBAL_FIELD_ACL_CONTEXT},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_ADMT_PROFILE]                  = {DBAL_FIELD_ADMT_PROFILE, {"Admt_Profile"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_BIER_STR_OFFSET]               = {DBAL_FIELD_BIER_STR_OFFSET, {"BIER_String_Offset"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_BIER_STR_SIZE]                 = {DBAL_FIELD_BIER_STR_SIZE, {"BIER_String_Size"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_BYTES_TO_REMOVE]               = {DBAL_FIELD_BYTES_TO_REMOVE, {"Bytes_to_Remove_Header"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_CONGESTION_INFO]               = {DBAL_FIELD_CONGESTION_INFO, {"Congestion_Info"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_DP]                            = {DBAL_FIELD_DP, {"DP"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_DP_METER_COMMAND]              = {DBAL_FIELD_DP_METER_COMMAND, {"DP_Meter_Cmd"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_EEI]                           = {DBAL_FIELD_EEI, {"EEI.type", "EEI.value"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_EGRESS_LEARN_ENABLE]           = {DBAL_FIELD_EGRESS_LEARN_ENABLE, {"Egress_Learn_Enable"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_END_OF_PACKET_EDITING]         = {DBAL_FIELD_END_OF_PACKET_EDITING, {"End_of_Pkt_Editing"}},
    /* Assuming that each statistics is valid(1) + opcode(2) + data(20) */
    [DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_0]              = {DBAL_FIELD_EXT_STATISTICS_0, {"Ext_Stats_Valid", "Ext_Stats_Opcode", "Ext_Stats"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_1]              = {DBAL_FIELD_EXT_STATISTICS_1, {"Ext_Stats_Valid", "Ext_Stats_Opcode", "Ext_Stats"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_2]              = {DBAL_FIELD_EXT_STATISTICS_2, {"Ext_Stats_Valid", "Ext_Stats_Opcode", "Ext_Stats"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_EXT_STATISTICS_3]              = {DBAL_FIELD_EXT_STATISTICS_3, {"Ext_Stats_Valid", "Ext_Stats_Opcode", "Ext_Stats"}},
    /* CPU-Trap-Code(9) + Forward-Strength(4) + * * CPU-Trap-Qualifier(19) */
    [DBAL_ENUM_FVAL_IPMF3_ACTION_FWD]                           = {DBAL_FIELD_FWD_TRAP, {"Fwd_Action_CPU_Trap_Code", "Fwd_Action_Strength", "Fwd_Action_CPU_Trap_Qualifier"}},
    /* Value (21) + Qualifier (11) */
    [DBAL_ENUM_FVAL_IPMF3_ACTION_DST_DATA]                      = {DBAL_FIELD_DST_DATA, {"Fwd_Action_Dst.type", "Fwd_Action_Dst.value"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_FWD_DOMAIN]                    = {DBAL_FIELD_FWD_DOMAIN, {"Fwd_Domain_ID.type", "Fwd_Domain_ID.value"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_FWD_LAYER_ADDITIONAL_INFO]     = {DBAL_FIELD_FWD_LAYER_ADDITIONAL_INFO, {"Fwd_Layer_Additional_Info"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_FWD_LAYER_INDEX]               = {DBAL_FIELD_FWD_LAYER_INDEX, {"Fwd_Layer_Index"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_0]                = {DBAL_FIELD_ACTION_GENERAL_DATA_0},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_1]                = {DBAL_FIELD_ACTION_GENERAL_DATA_1},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_2]                = {DBAL_FIELD_ACTION_GENERAL_DATA_2},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_GENERAL_DATA_3]                = {DBAL_FIELD_ACTION_GENERAL_DATA_3},
    /* {Packet-is(1), Update-Time-Stamp(1), Command(2), Encaps(1), Offset(8)} */
    [DBAL_ENUM_FVAL_IPMF3_ACTION_IEEE1588_DATA]                 = {DBAL_FIELD_IEEE1588_DATA,
        {"IEEE1588_Cmd", "IEEE1588_Compensate_Time_Stamp", "IEEE1588_Enc", "IEEE1588_Header_Offset", "Pkt_is_IEEE1588"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_INGRESS_LEARN_ENABLE]          = {DBAL_FIELD_INGRESS_LEARN_ENABLE, {"Ingress_Learn_Enable"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_INGRESS_TIME_STAMP]            = {DBAL_FIELD_INGRESS_TIME_STAMP, {"Ingress_Time_Stamp"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_INGRESS_TIME_STAMP_OVERRIDE]   = {DBAL_FIELD_INGRESS_TIME_STAMP_OVERRIDE, {"Ingress_Time_Stamp_Override"}},
    /* int_profile(8), int_command(2) */
    [DBAL_ENUM_FVAL_IPMF3_ACTION_INT_DATA]                      = {DBAL_FIELD_INT_DATA, {"Int_Profile", "Int_Cmd"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_IN_LIF_0]                 = {DBAL_FIELD_GLOB_IN_LIF_0, {"In_LIFs.0"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_IN_LIF_PROFILE_0]              = {DBAL_FIELD_IN_LIF_PROFILE_0, {"In_LIF_Profiles.0"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_IN_LIF_1]                 = {DBAL_FIELD_GLOB_IN_LIF_1, {"In_LIFs.1"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_IN_LIF_PROFILE_1]              = {DBAL_FIELD_IN_LIF_PROFILE_0, {"In_LIF_Profiles.1"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_IN_PP_PORT]                    = {DBAL_FIELD_PP_PORT, {"In_Port"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_IN_TTL]                        = {DBAL_FIELD_IN_TTL, {"In_TTL"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_ITPP_DELTA]                    = {DBAL_FIELD_ITPP_DELTA_AND_VALID, {"ITPP_Delta", "ITPP_Delta_Valid"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_LAG_LB_KEY]                    = {DBAL_FIELD_LAG_LB_KEY, {"LAG_LB_Key"}},
    /* { latency_flow_profile(4), latency_flow_id(19), latency_flow_id_valid(1) } */
    [DBAL_ENUM_FVAL_IPMF3_ACTION_LATENCY_FLOW]                  = {DBAL_FIELD_LATENCY_FLOW, {"Latency_Flow_Profile", "Latency_Flow_ID", "Latency_Flow_ID_Valid"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_0]                  = {DBAL_FIELD_LEARN_INFO_0},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_1]                  = {DBAL_FIELD_LEARN_INFO_1},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_2]                  = {DBAL_FIELD_LEARN_INFO_2},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_3]                  = {DBAL_FIELD_LEARN_INFO_3},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_4]                  = {DBAL_FIELD_LEARN_INFO_4},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_OR_TRANSPLANT]           = {DBAL_FIELD_LEARN_OR_TRANSPLANT, {"Learn_Info.Learn_Or_Transplant"}},
    /* Mirror-Code (9) + Qualifier(8) */
    [DBAL_ENUM_FVAL_IPMF3_ACTION_MIRROR_DATA]                   = {DBAL_FIELD_MIRROR_DATA, {"Mirror_Code", "Mirror_Qualifier"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_LB_KEY]                    = {DBAL_FIELD_NWK_LB_KEY, {"Nwk_LB_Key"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_QOS]                       = {DBAL_FIELD_NWK_QOS, {"Nwk_QOS"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_NWK_HEADER_APPEND_SIZE]        = {DBAL_FIELD_NWK_HEADER_APPEND_SIZE, {"Nwk_Header_Append_Size_Ptr"}},
    /* Oam-Up-Mep(1), Oam-Sub-Type(4), Oam-Offset(8), * Oam-Stamp-Offset(8) */
    [DBAL_ENUM_FVAL_IPMF3_ACTION_OAM_DATA]                      = {DBAL_FIELD_OAM_DATA, {"OAM_Up_MEP", "OAM_Sub_Type", "OAM_Offset", "OAM_Stamp_Offset"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_OAM_ID]                        = {DBAL_FIELD_PMF_OAM_ID, {"OAM_ID"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_0]                = {DBAL_FIELD_GLOB_OUT_LIF_0, {"All_Out_LIFs.0"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_1]                = {DBAL_FIELD_GLOB_OUT_LIF_1, {"All_Out_LIFs.1"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_2]                = {DBAL_FIELD_GLOB_OUT_LIF_2, {"All_Out_LIFs.2"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_GLOB_OUT_LIF_3]                = {DBAL_FIELD_GLOB_OUT_LIF_3, {"All_Out_LIFs.3"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_PACKET_IS_APPLET]              = {DBAL_FIELD_IS_APPLET, {"Pkt_is_Applet"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_PACKET_IS_BIER]                = {DBAL_FIELD_PACKET_IS_BIER, {"Pkt_is_BIER"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_EGRESS_PARSING_INDEX]          = {DBAL_FIELD_EGRESS_PARSING_INDEX, {"Egress_Parsing_Index"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_PPH_RESERVED]                  = {DBAL_FIELD_PPH_RESERVED, {"PPH_Reserved"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_PPH_TYPE]                      = {DBAL_FIELD_PPH_TYPE, {"PPH_Type"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_PUSH_OUT_LIF]                  = {DBAL_FIELD_PUSH_OUT_LIF},
    /* Snoop-Code(9) + Snoop-Strength(3) + Qualifier (8) */
    [DBAL_ENUM_FVAL_IPMF3_ACTION_SNOOP_DATA]                    = {DBAL_FIELD_SNOOP_DATA, {"Snoop_Code", "Snoop_Qualifier", "Snoop_Strength"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_SRC_SYS_PORT]                  = {DBAL_FIELD_SRC_SYS_PORT, {"Src_Sys_Port_ID"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_STACKING_ROUTE_HISTORY_BITMAP] = {DBAL_FIELD_STACKING_ROUTE_HISTORY_BITMAP, {"Stacking_Route_History_Bitmap"}},
    /* statistical_sampling_qualifier(16),statistical_sampling_code(9) */
    [DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICAL]                   = {DBAL_FIELD_STATISTICAL_SAMPLING, {"Statistical_Sampling_Qualifier", "Statistical_Sampling_Code"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_META_DATA]          = {DBAL_FIELD_STATISTICS_META_DATA, {"Stats_Meta_Data"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_0]               = {DBAL_FIELD_STATISTICS_ID_0, {"Stats_Objects_Resolved.1"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_1]               = {DBAL_FIELD_STATISTICS_ID_1, {"Stats_Objects_Resolved.2"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_2]               = {DBAL_FIELD_STATISTICS_ID_2, {"Stats_Objects_Resolved.3"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_3]               = {DBAL_FIELD_STATISTICS_ID_3, {"Stats_Objects_Resolved.4"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_4]               = {DBAL_FIELD_STATISTICS_ID_4, {"Stats_Objects_Resolved.5"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_5]               = {DBAL_FIELD_STATISTICS_ID_5, {"Stats_Objects_Resolved.6"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_6]               = {DBAL_FIELD_STATISTICS_ID_6, {"Stats_Objects_Resolved.7"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_7]               = {DBAL_FIELD_STATISTICS_ID_7, {"Stats_Objects_Resolved.8"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_8]               = {DBAL_FIELD_STATISTICS_ID_8, {"Stats_Objects_Resolved.9"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ID_9]               = {DBAL_FIELD_STATISTICS_ID_9, {"Stats_Objects_Resolved.10"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_0]              = {DBAL_FIELD_STATISTICS_ATR_0,
        {"Stats_Objects_Atr.1.Valid", "Stats_Objects_Atr.1.Type", "Stats_Objects_Atr.1.Is_LM", "Stats_Objects_Atr.1.Is_Meter"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_1]              = {DBAL_FIELD_STATISTICS_ATR_1,
        {"Stats_Objects_Atr.2.Valid", "Stats_Objects_Atr.2.Type", "Stats_Objects_Atr.2.Is_LM", "Stats_Objects_Atr.2.Is_Meter"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_2]              = {DBAL_FIELD_STATISTICS_ATR_2,
        {"Stats_Objects_Atr.3.Valid", "Stats_Objects_Atr.3.Type", "Stats_Objects_Atr.3.Is_LM", "Stats_Objects_Atr.3.Is_Meter"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_3]              = {DBAL_FIELD_STATISTICS_ATR_3,
        {"Stats_Objects_Atr.4.Valid", "Stats_Objects_Atr.4.Type", "Stats_Objects_Atr.4.Is_LM", "Stats_Objects_Atr.4.Is_Meter"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_4]              = {DBAL_FIELD_STATISTICS_ATR_4,
        {"Stats_Objects_Atr.5.Valid", "Stats_Objects_Atr.5.Type", "Stats_Objects_Atr.5.Is_LM", "Stats_Objects_Atr.5.Is_Meter"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_5]              = {DBAL_FIELD_STATISTICS_ATR_5,
        {"Stats_Objects_Atr.6.Valid", "Stats_Objects_Atr.6.Type", "Stats_Objects_Atr.6.Is_LM", "Stats_Objects_Atr.6.Is_Meter"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_6]              = {DBAL_FIELD_STATISTICS_ATR_6,
        {"Stats_Objects_Atr.7.Valid", "Stats_Objects_Atr.7.Type", "Stats_Objects_Atr.7.Is_LM", "Stats_Objects_Atr.7.Is_Meter"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_7]              = {DBAL_FIELD_STATISTICS_ATR_7,
        {"Stats_Objects_Atr.8.Valid", "Stats_Objects_Atr.8.Type", "Stats_Objects_Atr.8.Is_LM", "Stats_Objects_Atr.8.Is_Meter"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_8]              = {DBAL_FIELD_STATISTICS_ATR_8,
        {"Stats_Objects_Atr.9.Valid", "Stats_Objects_Atr.9.Type", "Stats_Objects_Atr.9.Is_LM", "Stats_Objects_Atr.9.Is_Meter"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_ATR_9]              = {DBAL_FIELD_STATISTICS_ATR_9,
        {"Stats_Objects_Atr.10.Valid", "Stats_Objects_Atr.10.Type", "Stats_Objects_Atr.10.Is_LM", "Stats_Objects_Atr.10.Is_Meter"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_STATISTICS_OBJECT_10]          = {DBAL_FIELD_STATISTICS_OBJECT_10, {"Stats_Obj_10"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_ST_VSQ_PTR]                    = {DBAL_FIELD_ST_VSQ_PTR, {"St_VSQ_Ptr"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_SYSTEM_HEADER_PROFILE_INDEX]   = {DBAL_FIELD_SYSTEM_HEADER_PROFILE_INDEX, {"Sys_Header_Profile_Index"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_SYSTEM_HEADER_SIZE_ADJUST]     = {DBAL_FIELD_SYSTEM_HEADER_SIZE_ADJUST, {"Sys_Header_Size_Adjust"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_TC]                            = {DBAL_FIELD_TC, {"TC"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_TM_PROFILE]                    = {DBAL_FIELD_TM_PROFILE, {"TM_Profile"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_1]                 = {DBAL_FIELD_USER_HEADER_1, {"User_Header_1"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_2]                 = {DBAL_FIELD_USER_HEADER_2, {"User_Header_2"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_3]                 = {DBAL_FIELD_USER_HEADER_3, {"User_Header_3"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADER_4]                 = {DBAL_FIELD_USER_HEADER_4, {"User_Header_4"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_USER_HEADERS_TYPE]             = {DBAL_FIELD_USER_HEADERS_TYPE, {"User_Headers_Type"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_VISIBILITY]                    = {DBAL_FIELD_VISIBILITY},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_CMD_INDEX]           = {DBAL_FIELD_VLAN_EDIT_CMD_INDEX, {"VLAN_Edit_Cmd.Cmd"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_PCP_DEI_1]           = {DBAL_FIELD_VLAN_EDIT_PCP_DEI_1, {"VLAN_Edit_Cmd.DEI_PCP1"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_PCP_DEI_2]           = {DBAL_FIELD_VLAN_EDIT_PCP_DEI_2, {"VLAN_Edit_Cmd.DEI_PCP2"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_VID_1]               = {DBAL_FIELD_VLAN_EDIT_VID_1, {"VLAN_Edit_Cmd.VID_1"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_VLAN_EDIT_VID_2]               = {DBAL_FIELD_VLAN_EDIT_VID_2, {"VLAN_Edit_Cmd.VID_2"}},
    /**  { parsing_start_offset, parsing_start_offset_valid} Default 0 */
    [DBAL_ENUM_FVAL_IPMF3_ACTION_PARSING_START_OFFSET]          = {DBAL_FIELD_PARSING_START_OFFSET},
    /** {parsing_start_type, parsing_start_type_valid Default 0}*/
    [DBAL_ENUM_FVAL_IPMF3_ACTION_PARSING_START_TYPE]            = {DBAL_FIELD_PARSING_START_TYPE_ACTION},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_STAT_OBJ_LM_READ_INDEX]        = {DBAL_FIELD_STAT_OBJ_LM_READ_INDEX, {"Stats_Obj_Lm_Rd_Index"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_KEY_0]              = {DBAL_FIELD_LEARN_INFO_KEY_0, {"Learn_Info.Key(Full_Learn_Key)"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_KEY_1]              = {DBAL_FIELD_LEARN_INFO_KEY_1, {"Learn_Info.Key(Full_Learn_Key)"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_KEY_2_APP_DB_INDEX] = {DBAL_FIELD_LEARN_INFO_KEY_2_APP_DB_INDEX, {"Learn_Info.App_DB_Index", "Learn_Info.Key(Full_Learn_Key)"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_PAYLOAD_0]          = {DBAL_FIELD_LEARN_INFO_PAYLOAD_0, {"Learn_Info.Learn_Data"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_PAYLOAD_1]          = {DBAL_FIELD_LEARN_INFO_PAYLOAD_1, {"Learn_Info.Learn_Data"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_LEARN_INFO_FORMAT_INDEX]       = {DBAL_FIELD_LEARN_INFO_FORMAT_INDEX, {"Learn_Info.Entry_Format_Index"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_TM_ACTION_TYPE]                = {DBAL_FIELD_TM_ACTION_TYPE, {"TM_Action_Type"}},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_INVALIDATE_NEXT]               = {DBAL_FIELD_INVALIDATE_NEXT},
    [DBAL_ENUM_FVAL_IPMF3_ACTION_INVALID]                       = {DBAL_FIELD_INVALID}
};

static const dnx_field_base_action_info_t dnx_epmf_action_info[] = {
    [DBAL_ENUM_FVAL_EPMF_ACTION_DISCARD]                        = {DBAL_FIELD_DISCARD, {"Discard"}},
    [DBAL_ENUM_FVAL_EPMF_ACTION_TC_MAP_PROFILE]                 = {DBAL_FIELD_TC_MAP_PROFILE, {"TC_Map_Profile"}},
    [DBAL_ENUM_FVAL_EPMF_ACTION_TC]                             = {DBAL_FIELD_TC, {"FTMH_TC"}},
    [DBAL_ENUM_FVAL_EPMF_ACTION_DP]                             = {DBAL_FIELD_DP, {"FTMH_DP"}},
    [DBAL_ENUM_FVAL_EPMF_ACTION_CUD_OUTLIF_OR_MCDB_PTR]         = {DBAL_FIELD_CUD_OUTLIF_OR_MCDB_PTR, {"CUD_Out_LIF_or_MCDB_Ptr"}},
    [DBAL_ENUM_FVAL_EPMF_ACTION_PP_DSP]                         = {DBAL_FIELD_PP_DSP, {"PP_DSP"}},
    [DBAL_ENUM_FVAL_EPMF_ACTION_COS_PROFILE]                    = {DBAL_FIELD_COS_PROFILE, {"CoS_Profile"}},
    [DBAL_ENUM_FVAL_EPMF_ACTION_MIRROR_PROFILE]                 = {DBAL_FIELD_MIRROR_PROFILE, {"Mirror_Profile"}},
    [DBAL_ENUM_FVAL_EPMF_ACTION_SNOOP_PROFILE]                  = {DBAL_FIELD_SNOOP_PROFILE, {"Snoop_Action_Profile_Index"}},
    [DBAL_ENUM_FVAL_EPMF_ACTION_SNOOP_STRENGTH]                 = {DBAL_FIELD_SNOOP_STRENGTH, {"Snoop_Strength"}},
    [DBAL_ENUM_FVAL_EPMF_ACTION_FWD_ACTION_PROFILE_INDEX]       = {DBAL_FIELD_FWD_ACTION_PROFILE_INDEX, {"Fwd_Action_Profile_Index"}},
    [DBAL_ENUM_FVAL_EPMF_ACTION_FWD_STRENGTH]                   = {DBAL_FIELD_FWD_STRENGTH, {"Fwd_Strength"}},
    [DBAL_ENUM_FVAL_EPMF_ACTION_PMF_FWD_STRENGTH]               = {DBAL_FIELD_PMF_FWD_STRENGTH, {"PMF_Fwd_Strength"}},
    [DBAL_ENUM_FVAL_EPMF_ACTION_PMF_SNOOP_STRENGTH]             = {DBAL_FIELD_PMF_SNOOP_STRENGTH, {"PMF_Snoop_Strength"}},
    [DBAL_ENUM_FVAL_EPMF_ACTION_PMF_PROFILE]                    = {DBAL_FIELD_PMF_PROFILE, {"PMF_Profile"}},
    [DBAL_ENUM_FVAL_EPMF_ACTION_PMF_SNOOP_OVERRIDE_ENABLE]      = {DBAL_FIELD_PMF_SNOOP_OVERRIDE_ENABLE, {"Snoop_Override_Enable"}},
    [DBAL_ENUM_FVAL_EPMF_ACTION_PMF_FWD_OVERRIDE_ENABLE]        = {DBAL_FIELD_PMF_FWD_OVERRIDE_ENABLE, {"PMF_Fwd_Override_Enable"}},
    [DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_0_PROFILE]          = {DBAL_FIELD_PMF_COUNTER_0_PROFILE, {"PMF_Counter_0_Profile"}},
    [DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_0_PTR]              = {DBAL_FIELD_PMF_COUNTER_0_PTR, {"PMF_Counter_0_Ptr"}},
    [DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_1_PROFILE]          = {DBAL_FIELD_PMF_COUNTER_1_PROFILE, {"PMF_Counter_1_Profile"}},
    [DBAL_ENUM_FVAL_EPMF_ACTION_PMF_COUNTER_1_PTR]              = {DBAL_FIELD_PMF_COUNTER_1_PTR, {"PMF_Counter_1_Ptr"}},
    [DBAL_ENUM_FVAL_EPMF_ACTION_EBTR_SOP]                       = {DBAL_FIELD_EBTR_SOP_DELTA, {"Ebtr_SOP_EPMF_Delta"}},
    [DBAL_ENUM_FVAL_EPMF_ACTION_EBTR_EOP]                       = {DBAL_FIELD_EBTR_EOP_DELTA, {"Ebtr_EOP_EPMF_Delta"}},
    [DBAL_ENUM_FVAL_EPMF_ACTION_EBTA_SOP]                       = {DBAL_FIELD_EBTA_SOP_DELTA, {"Ebta_SOP_EPMF_Delta"}},
    [DBAL_ENUM_FVAL_EPMF_ACTION_EBTA_EOP]                       = {DBAL_FIELD_EBTA_EOP_DELTA, {"Ebta_EOP_EPMF_Delta"}},
    [DBAL_ENUM_FVAL_EPMF_ACTION_INVALIDATE_NEXT]                = {DBAL_FIELD_INVALIDATE_NEXT},
    [DBAL_ENUM_FVAL_EPMF_ACTION_INVALID]                        = {DBAL_FIELD_INVALID}
};

static const dnx_field_base_action_info_t dnx_ace_action_info[] = {
    [DBAL_ENUM_FVAL_ACE_ACTION_PP_DSP]                                = {DBAL_FIELD_PP_DSP, {"FTMH_PP_DSP"}},
    [DBAL_ENUM_FVAL_ACE_ACTION_FTMH_TC]                               = {DBAL_FIELD_FTMH_TC, {"FTMH_TC"}},
    [DBAL_ENUM_FVAL_ACE_ACTION_FTMH_DP]                               = {DBAL_FIELD_FTMH_DP, {"FTMH_DP"}},
    [DBAL_ENUM_FVAL_ACE_ACTION_FTMH_TM_ACTION_TYPE]                   = {DBAL_FIELD_FTMH_TM_ACTION_TYPE, {"FTMH_TM_Action_Type"}},
    [DBAL_ENUM_FVAL_ACE_ACTION_FTMH_CNI]                              = {DBAL_FIELD_FTMH_CNI, {"FTMH_CNI"}},
    [DBAL_ENUM_FVAL_ACE_ACTION_FTMH_ECN_ENABLE]                       = {DBAL_FIELD_FTMH_ECN_ENABLE, {"FTMH_ECN_Enable"}},
    [DBAL_ENUM_FVAL_ACE_ACTION_FTMH_VISIBILITY]                       = {DBAL_FIELD_FTMH_VISIBILITY, {"FTMH_Base.FTMH_Visibility"}},
    [DBAL_ENUM_FVAL_ACE_ACTION_PPH_FORWARD_DOMAIN]                    = {DBAL_FIELD_PPH_FORWARD_DOMAIN, {"PPH_Fwd_Domain.type", "PPH_Fwd_Domain.value"}},
    [DBAL_ENUM_FVAL_ACE_ACTION_PPH_NWK_QOS]                           = {DBAL_FIELD_NWK_QOS, {"PPH_Nwk_QOS"}},
    [DBAL_ENUM_FVAL_ACE_ACTION_PPH_VALUE1]                            = {DBAL_FIELD_PPH_VALUE1, {"PPH_Value_1"}},
    [DBAL_ENUM_FVAL_ACE_ACTION_PPH_VALUE2]                            = {DBAL_FIELD_PPH_VALUE2, {"PPH_Value_2"}},
    [DBAL_ENUM_FVAL_ACE_ACTION_FHEI_EXT31TO00]                        = {DBAL_FIELD_FHEI_EXT31TO00, {"FHEI_Ext"}},
    [DBAL_ENUM_FVAL_ACE_ACTION_FHEI_EXT63TO32]                        = {DBAL_FIELD_FHEI_EXT63TO32, {"FHEI_Ext"}},
    [DBAL_ENUM_FVAL_ACE_ACTION_PPH_TTL]                               = {DBAL_FIELD_PPH_TTL, {"PPH_TTL"}},
    [DBAL_ENUM_FVAL_ACE_ACTION_PPH_IN_LIF]                            = {DBAL_FIELD_PPH_IN_LIF, {"PPH_In_LIF"}},
    [DBAL_ENUM_FVAL_ACE_ACTION_PPH_IN_LIF_PROFILE]                    = {DBAL_FIELD_PPH_IN_LIF_PROFILE, {"PPH_In_LIF_Profile"}},
    [DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF0_CUD_OUT_LIF0VALID]        = {DBAL_FIELD_OUT_LIF0_CUD_OUT_LIF0VALID},
    [DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF1_CUD_OUT_LIF1VALID]        = {DBAL_FIELD_OUT_LIF1_CUD_OUT_LIF1VALID},
    [DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF2_CUD_OUT_LIF2VALID]        = {DBAL_FIELD_OUT_LIF2_CUD_OUT_LIF2VALID},
    [DBAL_ENUM_FVAL_ACE_ACTION_CUD_OUT_LIF3_CUD_OUT_LIF3VALID]        = {DBAL_FIELD_OUT_LIF3_CUD_OUT_LIF3VALID},
    [DBAL_ENUM_FVAL_ACE_ACTION_PPH_FORWARDING_LAYER_ADDITIONAL_INFO]  = {DBAL_FIELD_PPH_FORWARDING_LAYER_ADDITIONAL_INFO, {"PPH_Fwd_Layer_Additional_Info"}},
    [DBAL_ENUM_FVAL_ACE_ACTION_PPH_FWD_LAYER_INDEX]                   = {DBAL_FIELD_PPH_FWD_LAYER_INDEX, {"PPH_Fwd_Layer_Index"}},
    [DBAL_ENUM_FVAL_ACE_ACTION_PPH_FORWANDING_STRENGTH]               = {DBAL_FIELD_PPH_FORWANDING_STRENGTH, {"PPH_Fwd_Strength"}},
    [DBAL_ENUM_FVAL_ACE_ACTION_PPH_END_OF_PACKET_EDITING]             = {DBAL_FIELD_PPH_END_OF_PACKET_EDITING, {"PPH_End_of_Pkt_Editing"}},
    [DBAL_ENUM_FVAL_ACE_ACTION_PPH_APPLICATION_SPECIFIC_EXT_31TO00]   = {DBAL_FIELD_PPH_APPLICATION_SPECIFIC_EXT_31TO00, {"App_Specific_Ext"}},
    [DBAL_ENUM_FVAL_ACE_ACTION_PPH_APPLICATION_SPECIFIC_EXT_47TO32]   = {DBAL_FIELD_PPH_APPLICATION_SPECIFIC_EXT_47TO32, {"App_Specific_Ext"}},
    [DBAL_ENUM_FVAL_ACE_ACTION_SNIFF_PROFILE]                         = {DBAL_FIELD_SNIFF_PROFILE, {"Sniff_Profile"}},
    [DBAL_ENUM_FVAL_ACE_ACTION_FTMH_SOURCE_SYSTEM_PORT]               = {DBAL_FIELD_FTMH_SOURCE_SYSTEM_PORT, {"FTMH_Src_Sys_Port"}},
    [DBAL_ENUM_FVAL_ACE_ACTION_ACE_CONTEXT_VALUE]                     = {DBAL_FIELD_ACE_CONTEXT_VALUE, {"ACE_Context_Value"}},
    [DBAL_ENUM_FVAL_ACE_ACTION_ACE_STAMP_VALUE]                       = {DBAL_FIELD_ACE_STAMP_VALUE, {"ACE_Stamp_Value"}},
    [DBAL_ENUM_FVAL_ACE_ACTION_ACE_STAT_METER_OBJ_CMD_ID]             = {DBAL_FIELD_ACE_STAT_METER_OBJ_CMD_ID, {"ACE_Stats_Meter_Obj_Cmd"}},
    [DBAL_ENUM_FVAL_ACE_ACTION_ACE_STAT_OBJ_CMD_ID_VALID]             = {DBAL_FIELD_CE_STAT_OBJ_CMD_ID_VALID, {"ACE_Stats_Counter_Obj_Cmd"}},
    [DBAL_ENUM_FVAL_ACE_ACTION_INVALIDATE_NEXT]                       = {DBAL_FIELD_INVALIDATE_NEXT},
    [DBAL_ENUM_FVAL_ACE_ACTION_INVALID]                               = {DBAL_FIELD_INVALID}
};
/*
 * End of action related data
 * }
 */

static const dnx_field_map_stage_info_t dnx_field_map_stage_info[DNX_FIELD_STAGE_NOF] = {
    [DNX_FIELD_STAGE_IPMF1] = {
        .pp_stage                            = DNX_PP_STAGE_IPMF1,
        .container_qual_field_type           = DBAL_FIELD_TYPE_DEF_IPMF1_QUALIFIERS_CONTAINER,
        .container_act_field_type            = DBAL_FIELD_TYPE_DEF_IPMF1_ACTIONS_CONTAINER,
        .bcm_stage                           = bcmFieldStageIngressPMF1,
        /** DNX to HW/DBAL mapping for context select quals */
        .cs_table_id                         = DBAL_TABLE_TCAM_CS_IPMF1,
        .cs_hit_indication_table_id          = DBAL_TABLE_KLEAP_IPMF1_TCAM_HIT_INDICATION,
        /** DNX to HW/DBAL mapping for qualifiers */
        .layer_qual_info                     = dnx_layer_record_qual_info,
        .meta_qual_nof                       = sizeof(dnx_ipmf1_meta_qual_info) / sizeof(dnx_ipmf1_meta_qual_info[0]),
        .meta_qual_info                      = dnx_ipmf1_meta_qual_info,
        /** DNX to HW/DBAL mapping for actions */
        .action_field_id                     = DBAL_FIELD_IPMF1_ACTION,
        .static_action_nof                   = sizeof(dnx_ipmf1_action_info) / sizeof(dnx_ipmf1_action_info[0]),
        .static_action_info                  = dnx_ipmf1_action_info,
        /** BCM to DNX maps for cs/quals/actions */
        .cs_qual_map                         = dnx_ipmf1_cs_qual_map,
        .meta_qual_map                       = dnx_ipmf1_meta_qual_map,
        .static_action_id_map                = dnx_ipmf1_action_map,
        /** KBR/FFC parameters per stage */
        .lookup_enabler_table_id                         = DBAL_TABLE_FIELD_IPMF1_LOOKUP_ENABLER,
        .ffc_instruction                                 = DBAL_FIELD_IPMF1_FFC_INSTRUCTION,
        .ctx_id                                          = DBAL_FIELD_FIELD_PMF_CTX_ID,
        .key_field                                       = DBAL_FIELD_FIELD_KEY,
        .ffc_table                                       = DBAL_TABLE_KLEAP_IPMF1_FFC_INSTRUCTION,
        .ffc_type_field_a[DNX_FIELD_FFC_RELATIVE_HEADER] = DBAL_FIELD_IPMF1_FFC_RELATIVE_HEADER,
        .ffc_type_field_a[DNX_FIELD_FFC_ABSOLUTE_HEADER] = DBAL_FIELD_IPMF1_FFC_HEADER,
        .ffc_type_field_a[DNX_FIELD_FFC_LITERALLY]       = DBAL_FIELD_IPMF1_FFC_LITERALLY,
        .ffc_type_field_a[DNX_FIELD_FFC_LAYER_RECORD]    = DBAL_FIELD_IPMF1_FFC_LAYER_RECORD,
    },
    [DNX_FIELD_STAGE_IPMF2] = {
        .pp_stage                            = DNX_PP_STAGE_IPMF2,
        .container_qual_field_type           = DBAL_FIELD_TYPE_DEF_IPMF2_QUALIFIERS_CONTAINER,
        .container_act_field_type            = DBAL_FIELD_TYPE_DEF_IPMF1_ACTIONS_CONTAINER,
        .bcm_stage                           = bcmFieldStageIngressPMF2,
        /** DNX to HW/DBAL mapping for context select quals */
        .cs_table_id                         = DBAL_TABLE_TCAM_CS_IPMF2,
        .cs_hit_indication_table_id          = DBAL_TABLE_KLEAP_IPMF2_TCAM_HIT_INDICATION,
        /** DNX to HW/DBAL mapping for qualifiers */
        .layer_qual_info                     = dnx_layer_record_qual_info,
        .meta_qual_nof                       = sizeof(dnx_ipmf2_meta_qual_info) / sizeof(dnx_ipmf2_meta_qual_info[0]),
        .meta_qual_info                      = dnx_ipmf2_meta_qual_info,
        /** DNX to HW/DBAL mapping for actions */
        .action_field_id                     = DBAL_FIELD_IPMF1_ACTION,
        .static_action_nof                   = sizeof(dnx_ipmf1_action_info) / sizeof(dnx_ipmf1_action_info[0]),
        .static_action_info                  = dnx_ipmf1_action_info,
        /** BCM to DNX maps for cs/quals/actions */
        .cs_qual_map                         = dnx_ipmf2_cs_qual_map,
        .meta_qual_map                       = dnx_ipmf2_meta_qual_map,
        .static_action_id_map                = dnx_ipmf1_action_map,
        /** KBR/FFC parameters per stage */
        .lookup_enabler_table_id                         = DBAL_TABLE_FIELD_IPMF2_LOOKUP_ENABLER,
        .ffc_instruction                                 = DBAL_FIELD_IPMF2_FFC_INSTRUCTION,
        .ctx_id                                          = DBAL_FIELD_FIELD_PMF_CTX_ID,
        .key_field                                       = DBAL_FIELD_FIELD_KEY,
        .ffc_table                                       = DBAL_TABLE_KLEAP_IPMF2_FFC_INSTRUCTION,
        .ffc_type_field_a[DNX_FIELD_FFC_RELATIVE_HEADER] = DBAL_FIELD_EMPTY,
        .ffc_type_field_a[DNX_FIELD_FFC_ABSOLUTE_HEADER] = DBAL_FIELD_EMPTY,
        .ffc_type_field_a[DNX_FIELD_FFC_LITERALLY]       = DBAL_FIELD_IPMF2_FFC_LITERALLY,
        .ffc_type_field_a[DNX_FIELD_FFC_LAYER_RECORD]    = DBAL_FIELD_EMPTY,
    },
    [DNX_FIELD_STAGE_IPMF3] = {
        .pp_stage                            = DNX_PP_STAGE_IPMF3,
        .container_qual_field_type           = DBAL_FIELD_TYPE_DEF_IPMF3_QUALIFIERS_CONTAINER,
        .container_act_field_type            = DBAL_FIELD_TYPE_DEF_IPMF3_ACTIONS_CONTAINER,
        .bcm_stage                           = bcmFieldStageIngressPMF3,
        /** DNX to HW/DBAL mapping for context select quals */
        .cs_table_id                         = DBAL_TABLE_TCAM_CS_IPMF3,
        .cs_hit_indication_table_id          = DBAL_TABLE_KLEAP_IPMF3_TCAM_HIT_INDICATION,
        /** DNX to HW/DBAL mapping for qualifiers */
        .layer_qual_info                     = dnx_layer_record_qual_info,
        .meta_qual_nof                       = sizeof(dnx_ipmf3_meta_qual_info) / sizeof(dnx_ipmf3_meta_qual_info[0]),
        .meta_qual_info                      = dnx_ipmf3_meta_qual_info,
        /** DNX to HW/DBAL mapping for actions */
        .action_field_id                     = DBAL_FIELD_IPMF3_ACTION,
        .static_action_nof                   = sizeof(dnx_ipmf3_action_info) / sizeof(dnx_ipmf3_action_info[0]),
        .static_action_info                  = dnx_ipmf3_action_info,
        /** BCM to DNX maps for cs/quals/actions */
        .cs_qual_map                         = dnx_ipmf3_cs_qual_map,
        .meta_qual_map                       = dnx_ipmf3_meta_qual_map,
        .static_action_id_map                = dnx_ipmf3_action_map,
        /** KBR/FFC parameters per stage */
        .lookup_enabler_table_id                         = DBAL_TABLE_FIELD_IPMF3_LOOKUP_ENABLER,
        .ffc_instruction                                 = DBAL_FIELD_IPMF3_FFC_INSTRUCTION,
        .ctx_id                                          = DBAL_FIELD_FIELD_PMF_CTX_ID,
        .key_field                                       = DBAL_FIELD_FIELD_KEY,
        .ffc_table                                       = DBAL_TABLE_KLEAP_IPMF3_FFC_INSTRUCTION,
        .ffc_type_field_a[DNX_FIELD_FFC_RELATIVE_HEADER] = DBAL_FIELD_EMPTY,
        .ffc_type_field_a[DNX_FIELD_FFC_ABSOLUTE_HEADER] = DBAL_FIELD_EMPTY,
        .ffc_type_field_a[DNX_FIELD_FFC_LITERALLY]       = DBAL_FIELD_IPMF3_FFC_LITERALLY,
        .ffc_type_field_a[DNX_FIELD_FFC_LAYER_RECORD]    = DBAL_FIELD_IPMF3_FFC_LAYER_RECORD,
    },
    [DNX_FIELD_STAGE_EPMF]  = {
        .pp_stage                            = DNX_PP_STAGE_EPMF,
        .container_qual_field_type           = DBAL_FIELD_TYPE_DEF_EPMF_QUALIFIERS_CONTAINER,
        .container_act_field_type            = DBAL_FIELD_TYPE_DEF_EPMF_ACTIONS_CONTAINER,
        .bcm_stage                           = bcmFieldStageEgress,
        /** DNX to HW/DBAL mapping for context select quals */
        .cs_table_id                         = DBAL_TABLE_TCAM_CS_EPMF,
        .cs_hit_indication_table_id          = DBAL_TABLE_KLEAP_EPMF_TCAM_HIT_INDICATION,
        /** DNX to HW/DBAL mapping for qualifiers */
        .layer_qual_info                     = dnx_layer_record_qual_info,
        .meta_qual_nof                       = sizeof(dnx_epmf_meta_qual_info) / sizeof(dnx_epmf_meta_qual_info[0]),
        .meta_qual_info                      = dnx_epmf_meta_qual_info,
        /** DNX to HW/DBAL mapping for actions */
        .action_field_id                     = DBAL_FIELD_EPMF_ACTION,
        .static_action_nof                   = sizeof(dnx_epmf_action_info) / sizeof(dnx_epmf_action_info[0]),
        .static_action_info                  = dnx_epmf_action_info,
        /** BCM to DNX maps for cs/quals/actions */
        .cs_qual_map                         = dnx_epmf_cs_qual_map,
        .meta_qual_map                       = dnx_epmf_meta_qual_map,
        .static_action_id_map                = dnx_epmf_action_map,
        /** KBR/FFC parameters per stage */
        .lookup_enabler_table_id                         = DBAL_NOF_TABLES,
        .ffc_instruction                                 = DBAL_FIELD_EPMF_FFC_INSTRUCTION,
        .ctx_id                                          = DBAL_FIELD_FIELD_PMF_CTX_ID,
        .key_field                                       = DBAL_FIELD_FIELD_KEY,
        .ffc_table                                       = DBAL_TABLE_KLEAP_E_PMF_FFC_INSTRUCTION,
        .ffc_type_field_a[DNX_FIELD_FFC_RELATIVE_HEADER] = DBAL_FIELD_EPMF_FFC_RELATIVE_HEADER,
        .ffc_type_field_a[DNX_FIELD_FFC_ABSOLUTE_HEADER] = DBAL_FIELD_EPMF_FFC_HEADER,
        .ffc_type_field_a[DNX_FIELD_FFC_LITERALLY]       = DBAL_FIELD_EPMF_FFC_LITERALLY,
        .ffc_type_field_a[DNX_FIELD_FFC_LAYER_RECORD]    = DBAL_FIELD_EPMF_FFC_LAYER_RECORD,
    },
    [DNX_FIELD_STAGE_EXTERNAL] = {
        .pp_stage                            = DNX_PP_STAGE_IFWD2,
        .container_qual_field_type           = DBAL_FIELD_TYPE_DEF_EXTERNAL_QUALIFIERS_CONTAINER,
        .bcm_stage                           = bcmFieldStageExternal,
        /** DNX to HW/DBAL mapping for context select quals */
        .cs_table_id                         = DBAL_TABLE_FWD2_ACL_CONTEXT_SELECTION,
        /** DNX to HW/DBAL mapping for qualifiers */
        .layer_qual_info                     = dnx_layer_record_qual_info,
        .meta_qual_nof                       = sizeof(dnx_external_meta_qual_info) / sizeof(dnx_external_meta_qual_info[0]),
        .meta_qual_info                      = dnx_external_meta_qual_info,
        /** BCM to DNX maps for cs/quals/actions */
        .cs_qual_map                         = dnx_external_cs_qual_map,
        .meta_qual_map                       = dnx_external_meta_qual_map,
        /** KBR/FFC parameters per stage */
        .lookup_enabler_table_id                         = DBAL_NOF_TABLES,
        .ffc_instruction                                 = DBAL_FIELD_FWD12_FFC_INSTRUCTION,
        .ctx_id                                          = DBAL_FIELD_CONTEXT_PROFILE,
        .key_field                                       = DBAL_FIELD_FWD12_KBR_ID,
        .ffc_table                                       = DBAL_TABLE_KLEAP_FWD12_FFC_INSTRUCTION,
        .ffc_type_field_a[DNX_FIELD_FFC_RELATIVE_HEADER] = DBAL_FIELD_FWD12_FFC_RELATIVE_HEADER,
        .ffc_type_field_a[DNX_FIELD_FFC_ABSOLUTE_HEADER] = DBAL_FIELD_FWD12_FFC_HEADER,
        .ffc_type_field_a[DNX_FIELD_FFC_LITERALLY]       = DBAL_FIELD_FWD12_FFC_LITERALLY,
        .ffc_type_field_a[DNX_FIELD_FFC_LAYER_RECORD]    = DBAL_FIELD_FWD12_FFC_LAYER_RECORD,
    },
    [DNX_FIELD_STAGE_ACE] = {
        .pp_stage                            = DNX_PP_STAGE_ACE,
        .container_act_field_type            = DBAL_FIELD_TYPE_DEF_ACE_ACTIONS_CONTAINER,
        .bcm_stage                           = bcmFieldStageEgressExtension,
        /** DNX to HW/DBAL mapping for actions */
        .action_field_id                     = DBAL_FIELD_ACE_ACTION,
        .static_action_nof                   = sizeof(dnx_ace_action_info) / sizeof(dnx_ace_action_info[0]),
        .static_action_info                  = dnx_ace_action_info,
        /** BCM to DNX map for actions */
        .static_action_id_map                = dnx_ace_action_map,
    },
    [DNX_FIELD_STAGE_L4_OPS] = {
        .container_qual_field_type           = DBAL_FIELD_TYPE_DEF_L4OPS_QUALIFIERS_CONTAINER,
        /** DNX to HW/DBAL mapping for qualifiers */
        .layer_qual_info                     = dnx_layer_record_qual_info,
        .meta_qual_nof                       = sizeof(dnx_l4ops_meta_qual_info) / sizeof(dnx_l4ops_meta_qual_info[0]),
        .meta_qual_info                      = dnx_l4ops_meta_qual_info,
        /** BCM to DNX maps for cs/quals/actions */
        .meta_qual_map                       = dnx_l4ops_meta_qual_map,
        /** KBR/FFC parameters per stage */
        .ffc_instruction                                 = DBAL_FIELD_L4OPS_FFC_INSTRUCTION,
        .ffc_table                                       = DBAL_TABLE_FIELD_IPMF1_L4_OPS_RANGES_FFC_CFG,
        .ffc_type_field_a[DNX_FIELD_FFC_RELATIVE_HEADER] = DBAL_FIELD_L4OPS_FFC_RELATIVE_HEADER,
        .ffc_type_field_a[DNX_FIELD_FFC_ABSOLUTE_HEADER] = DBAL_FIELD_L4OPS_FFC_HEADER,
        .ffc_type_field_a[DNX_FIELD_FFC_LITERALLY]       = DBAL_FIELD_L4OPS_FFC_LITERALLY,
        .ffc_type_field_a[DNX_FIELD_FFC_LAYER_RECORD]    = DBAL_FIELD_L4OPS_FFC_LAYER_RECORD,
    },
};

const dnx_field_static_apptype_map_t dnx_field_standard_1_static_apptype_map[bcmFieldAppTypeCount] = {
        [bcmFieldAppTypeL2]                = { .opcode_id = DBAL_ENUM_FVAL_KBP_FORWARD_APP_TYPES_BRIDGE_SVL, },
        [bcmFieldAppTypeTrafficManagement] = { .opcode_id = DBAL_ENUM_FVAL_KBP_FORWARD_APP_TYPES_NOP, },
        [bcmFieldAppTypeIp4UcastRpf]       = { .opcode_id = DBAL_ENUM_FVAL_KBP_FORWARD_APP_TYPES_IPV4_UNICAST_PRIVATE_W_UC_RPF, },
        [bcmFieldAppTypeIp6UcastRpf]       = { .opcode_id = DBAL_ENUM_FVAL_KBP_FORWARD_APP_TYPES_IPV6_UNICAST_PRIVATE_W_UC_RPF, },
        [bcmFieldAppTypeIp4McastRpf]       = { .opcode_id = DBAL_ENUM_FVAL_KBP_FORWARD_APP_TYPES_IPV4_MULTICAST_PRIVATE_W_UC_RPF, },
        [bcmFieldAppTypeIp6McastRpf]       = { .opcode_id = DBAL_ENUM_FVAL_KBP_FORWARD_APP_TYPES_IPV6_MULTICAST_PRIVATE_W_UC_RPF, },
        [bcmFieldAppTypeMpls]              = { .opcode_id = DBAL_ENUM_FVAL_KBP_FORWARD_APP_TYPES_MPLS, },
        [bcmFieldAppTypeVip4]              = { .opcode_id = DBAL_ENUM_FVAL_KBP_FORWARD_APP_TYPES_VIRTUAL_IPV4_PRIVATE_UC, },
        [bcmFieldAppTypeVip6]              = { .opcode_id = DBAL_ENUM_FVAL_KBP_FORWARD_APP_TYPES_VIRTUAL_IPV6_PRIVATE_UC, },
        [bcmFieldAppTypeVip4Direct]        = { .opcode_id = DBAL_ENUM_FVAL_KBP_FORWARD_APP_TYPES_SIMPLE_IPV4_PRIVATE_UC, },
        [bcmFieldAppTypeVip6Direct]        = { .opcode_id = DBAL_ENUM_FVAL_KBP_FORWARD_APP_TYPES_SIMPLE_IPV6_PRIVATE_UC, },
        [bcmFieldAppTypeSrv6Endpoint]      = { .opcode_id = DBAL_ENUM_FVAL_KBP_FORWARD_APP_TYPES_SRV6_ENDPOINT_CLASSIC, },
        [bcmFieldAppTypeSrv6UsidEndpoint]  = { .opcode_id = DBAL_ENUM_FVAL_KBP_FORWARD_APP_TYPES_SRV6_ENDPOINT_USID, },
        [bcmFieldAppTypeSrv6GsidEndpoint]  = { .opcode_id = DBAL_ENUM_FVAL_KBP_FORWARD_APP_TYPES_SRV6_ENDPOINT_GSID, },
};

dnx_field_static_opcode_info_t dnx_field_static_opcode_info[DNX_FIELD_EXTERNAL_MAX_STATIC_OPCODES_NOF] = { { 0 } };

static const dnx_field_map_port_profile_info_t dnx_field_map_port_profile_info[bcmPortClassCount] = {
        /** Profiles for ports.*/
        [bcmPortClassFieldIngressPMF1PacketProcessingPort]    = {
                .port_profile_type = DNX_FIELD_PORT_PROFILE_TYPE_PORT,
                .dbal_table_id = DBAL_TABLE_FIELD_PMF_PP_PORT_INFO_IPMF1, .dbal_key_field = DBAL_FIELD_PP_PORT,
                .dbal_result_field = DBAL_FIELD_IN_PORT_PMF_DATA,    },
        [bcmPortClassFieldIngressPMF3PacketProcessingPort]    = {
                .port_profile_type = DNX_FIELD_PORT_PROFILE_TYPE_PORT,
                .dbal_table_id = DBAL_TABLE_FIELD_PMF_PP_PORT_INFO_IPMF3, .dbal_key_field = DBAL_FIELD_PP_PORT,
                .dbal_result_field = DBAL_FIELD_IN_PORT_PMF_DATA,    },
        [bcmPortClassFieldEgressPacketProcessingPort]        = {
                .port_profile_type = DNX_FIELD_PORT_PROFILE_TYPE_PORT,
                .dbal_table_id = DBAL_TABLE_EGRESS_PP_PORT, .dbal_key_field = DBAL_FIELD_PP_PORT,
                .dbal_result_field = DBAL_FIELD_OUT_PP_PORT_PMF_DATA,},
        [bcmPortClassFieldIngressPMF1TrafficManagementPort]   = {
                .port_profile_type = DNX_FIELD_PORT_PROFILE_TYPE_PORT,
                .dbal_table_id = DBAL_TABLE_FIELD_PMF_PTC_INFO_IPMF1, .dbal_key_field = DBAL_FIELD_PTC,
                .dbal_result_field = DBAL_FIELD_PTC_PMF_DATA,    },
        [bcmPortClassFieldIngressPMF3TrafficManagementPort]   = {
                .port_profile_type = DNX_FIELD_PORT_PROFILE_TYPE_PORT,
                .dbal_table_id = DBAL_TABLE_FIELD_PMF_PTC_INFO_IPMF3, .dbal_key_field = DBAL_FIELD_PTC,
                .dbal_result_field = DBAL_FIELD_PTC_PMF_DATA,    },
        [bcmPortClassFieldEgressTrafficManagementPort]   = {
                .port_profile_type = DNX_FIELD_PORT_PROFILE_TYPE_PORT,
                .dbal_table_id = DBAL_TABLE_FIELD_PMF_OUT_TM_PORT_INFO_EPMF, .dbal_key_field = DBAL_FIELD_OUT_TM_PORT,
                .dbal_result_field = DBAL_FIELD_OUT_TM_PORT_PMF_DATA,    },
        [bcmPortClassFieldIngressPMF1PacketProcessingPortCs]    = {
                .port_profile_type = DNX_FIELD_PORT_PROFILE_TYPE_PORT,
                .dbal_table_id = DBAL_TABLE_FIELD_PMF_PP_PORT_INFO_IPMF1, .dbal_key_field = DBAL_FIELD_PP_PORT,
                .dbal_result_field = DBAL_FIELD_PP_PORT_CS_PROFILE,    },
        [bcmPortClassFieldIngressPMF3PacketProcessingPortCs]    = {
                .port_profile_type = DNX_FIELD_PORT_PROFILE_TYPE_PORT,
                .dbal_table_id = DBAL_TABLE_FIELD_PMF_PP_PORT_INFO_IPMF3, .dbal_key_field = DBAL_FIELD_PP_PORT,
                .dbal_result_field = DBAL_FIELD_PP_PORT_CS_PROFILE,    },
        [bcmPortClassFieldEgressPacketProcessingPortCs]    = {
                .port_profile_type = DNX_FIELD_PORT_PROFILE_TYPE_PORT,
                .dbal_table_id = DBAL_TABLE_EGRESS_PP_PORT, .dbal_key_field = DBAL_FIELD_PP_PORT,
                .dbal_result_field = DBAL_FIELD_PP_PORT_CS_PROFILE,    },
        [bcmPortClassFieldExternalPacketProcessingPortCs]    = {
                .port_profile_type = DNX_FIELD_PORT_PROFILE_TYPE_PORT,
                .dbal_table_id = DBAL_TABLE_FIELD_PMF_PP_PORT_INFO_IFWD, .dbal_key_field = DBAL_FIELD_PP_PORT,
                .dbal_result_field = DBAL_FIELD_PP_PORT_CS_PROFILE,    },
        [bcmPortClassFieldIngressPMF1TrafficManagementPortCs]    = {
                .port_profile_type = DNX_FIELD_PORT_PROFILE_TYPE_PORT,
                .dbal_table_id = DBAL_TABLE_FIELD_PMF_PTC_INFO_IPMF1, .dbal_key_field = DBAL_FIELD_PTC,
                .dbal_result_field = DBAL_FIELD_PTC_CS_PROFILE,    },
        [bcmPortClassFieldIngressPMF3TrafficManagementPortCs]    = {
                .port_profile_type = DNX_FIELD_PORT_PROFILE_TYPE_PORT,
                .dbal_table_id = DBAL_TABLE_FIELD_PMF_PTC_INFO_IPMF3, .dbal_key_field = DBAL_FIELD_PTC,
                .dbal_result_field = DBAL_FIELD_PTC_CS_PROFILE,    },
        [bcmPortClassFieldExternalTrafficManagementPortCs]    = {
                .port_profile_type = DNX_FIELD_PORT_PROFILE_TYPE_PORT,
                .dbal_table_id = DBAL_TABLE_FIELD_PMF_PTC_INFO_IFWD, .dbal_key_field = DBAL_FIELD_PTC,
                .dbal_result_field = DBAL_FIELD_PTC_CS_PROFILE,    },
        [bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralData]    = {
                .port_profile_type = DNX_FIELD_PORT_PROFILE_TYPE_PORT_GENERAL_DATA,
                .dbal_table_id = DBAL_TABLE_FIELD_PMF_PP_PORT_INFO_IPMF1, .dbal_key_field = DBAL_FIELD_PP_PORT,
                .dbal_result_field = DBAL_FIELD_IN_PORT_PMF_DATA,    },
        [bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralDataHigh]    = {
                .port_profile_type = DNX_FIELD_PORT_PROFILE_TYPE_PORT_GENERAL_DATA_HIGH,
                .dbal_table_id = DBAL_TABLE_FIELD_PMF_PP_PORT_INFO_IPMF1, .dbal_key_field = DBAL_FIELD_PP_PORT,
                .dbal_result_field = DBAL_FIELD_IN_PORT_PMF_DATA,    },
        [bcmPortClassFieldIngressPMF3PacketProcessingPortGeneralData]    = {
                .port_profile_type = DNX_FIELD_PORT_PROFILE_TYPE_PORT_GENERAL_DATA,
                .dbal_table_id = DBAL_TABLE_FIELD_PMF_PP_PORT_INFO_IPMF3, .dbal_key_field = DBAL_FIELD_PP_PORT,
                .dbal_result_field = DBAL_FIELD_IN_PORT_PMF_DATA,    },
        [bcmPortClassFieldIngressPMF3PacketProcessingPortGeneralDataHigh]    = {
                .port_profile_type = DNX_FIELD_PORT_PROFILE_TYPE_PORT_GENERAL_DATA_HIGH,
                .dbal_table_id = DBAL_TABLE_FIELD_PMF_PP_PORT_INFO_IPMF3, .dbal_key_field = DBAL_FIELD_PP_PORT,
                .dbal_result_field = DBAL_FIELD_IN_PORT_PMF_DATA,    },

        /** Profiles for LIF/RIF.*/
        [bcmPortClassFieldIngressVport]    = {
                .port_profile_type = DNX_FIELD_PORT_PROFILE_TYPE_IN_LIF_INGRESS, },
        [bcmPortClassFieldEgressVport]    = {
                .port_profile_type = DNX_FIELD_PORT_PROFILE_TYPE_OUT_ETH_RIF_EGRESS, },
};

static const dnx_field_pp_app_map_t dnx_field_pp_app_map[bcmFieldAppDbCount] = {
        [bcmFieldAppDbVlanPort]              = { .dbal_table_id = DBAL_TABLE_IN_AC_TCAM_DB, .valid = TRUE, },
        [bcmFieldAppDbIpmcV4]                = { .dbal_table_id = DBAL_TABLE_IPV4_MULTICAST_TCAM_FORWARD, .valid = TRUE, },
        [bcmFieldAppDbIpmcV6]                = { .dbal_table_id = DBAL_TABLE_IPV6_MULTICAST_TCAM_FORWARD, .valid = TRUE, },
        [bcmFieldAppDbTunnelTerminatorV4]    = { .dbal_table_id = DBAL_TABLE_IPV4_TT_TCAM_BASIC, .valid = TRUE, },
        [bcmFieldAppDbTunnelTerminatorBudV4] = { .dbal_table_id = DBAL_TABLE_IPV4_TT_TCAM_2ND_PASS, .valid = TRUE, },
        [bcmFieldAppDbTunnelTerminatorV6]    = { .dbal_table_id = DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC, .valid = TRUE, },
        [bcmFieldAppDbTunnelTerminatorBudV6] = { .dbal_table_id = DBAL_TABLE_IPV6_MP_TT_TCAM_2ND_PASS, .valid = TRUE, },
        [bcmFieldAppDbMplsFrr]               = { .dbal_table_id = DBAL_TABLE_MPLS_SKIP_TCAM_DB, .valid = TRUE, },
        [bcmFieldAppDbOamIdentification]     = { .dbal_table_id = DBAL_TABLE_OAM_TCAM_IDENTIFICATION_DB, .valid = TRUE, },
        [bcmFieldAppDbL3VPbrV4]              = { .dbal_table_id = DBAL_TABLE_IPV4_VRF_DB_TCAM, .valid = TRUE, },
        [bcmFieldAppDbL3VPbrV6]              = { .dbal_table_id = DBAL_TABLE_IPV6_VRF_DB_TCAM, .valid = TRUE, },
};





/**see H file: field_map.h */
static shr_error_e
dnx_field_cs_qual_map_t_init(
    int unit,
    dnx_field_cs_qual_map_t * cs_qual_map_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(cs_qual_map_p, _SHR_E_PARAM, "cs_qual_map_p");

    sal_memset(cs_qual_map_p, 0x0, sizeof(*cs_qual_map_p));

    cs_qual_map_p->nof = 0;
    cs_qual_map_p->flags = 0;
    cs_qual_map_p->conversion_cb = NULL;
    cs_qual_map_p->qual_type_bcm_to_dnx_conversion_cb = NULL;
    cs_qual_map_p->qual_type_dnx_to_bcm_conversion_cb = NULL;
    cs_qual_map_p->field_id_array = NULL;
    cs_qual_map_p->field_id = DBAL_FIELD_EMPTY;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_meta_qual_info_t_init(
    int unit,
    dnx_field_meta_qual_info_t * meta_qual_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(meta_qual_info_p, _SHR_E_PARAM, "meta_qual_info_p");

    sal_memset(meta_qual_info_p, 0x0, sizeof(*meta_qual_info_p));

    meta_qual_info_p->field_id = DBAL_FIELD_EMPTY;

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_layer_record_qual_info_t_init(
    int unit,
    dnx_field_layer_record_qual_info_t *layer_qual_map_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(layer_qual_map_info_p, _SHR_E_PARAM, "layer_qual_map_info_p");

    sal_memset(layer_qual_map_info_p, 0x0, sizeof(*layer_qual_map_info_p));

    layer_qual_map_info_p->dbal_field_id = DBAL_FIELD_EMPTY;

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_static_action_info_t_init(
    int unit,
    dnx_field_base_action_info_t * static_action_info_p)
{
    uint32 signal_iter;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(static_action_info_p, _SHR_E_PARAM, "static_action_info_p");

    sal_memset(static_action_info_p, 0x0, sizeof(*static_action_info_p));

    static_action_info_p->field_id = DBAL_FIELD_EMPTY;
    for (signal_iter = 0; signal_iter < DNX_DATA_MAX_FIELD_DIAG_NOF_SIGNALS_PER_ACTION; signal_iter++)
    {
        static_action_info_p->signal_name[signal_iter] = 0;
    }
exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_map_local.h */
shr_error_e
dnx_field_map_stage_info_get(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_map_stage_info_t *map_stage_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(map_stage_info_p, _SHR_E_PARAM, "map_stage_info_p");
    DNX_FIELD_STAGE_VERIFY(stage);

    sal_memcpy(map_stage_info_p, &dnx_field_map_stage_info[stage], sizeof(*map_stage_info_p));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_map_local.h */
shr_error_e
dnx_field_map_static_action_map_get(
    int unit,
    dnx_field_stage_e stage,
    bcm_field_action_t bcm_action,
    dnx_field_action_map_t * static_action_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(static_action_info_p, _SHR_E_PARAM, "static_action_info_p");
    DNX_FIELD_STAGE_VERIFY(stage);
    DNX_FIELD_BCM_ACTION_VERIFY(bcm_action);

    SHR_IF_ERR_EXIT(dnx_field_action_map_t_init(unit, static_action_info_p));
    if (dnx_field_map_stage_info[stage].static_action_id_map)
    {
        sal_memcpy(static_action_info_p, &dnx_field_map_stage_info[stage].static_action_id_map[bcm_action], sizeof(*static_action_info_p));
        if (static_action_info_p->dnx_action == 0)
        {
            static_action_info_p->dnx_action = DNX_FIELD_ACTION_TYPE_INVALID;
        }
    }

    /*
     * Conditional mapping change.
     */
    if ((bcm_action == bcmFieldActionCbts) &&
        ((stage == DNX_FIELD_STAGE_IPMF1) || (stage == DNX_FIELD_STAGE_IPMF2)) &&
        dnx_data_field.features.ecmp_tunnel_priority_get(unit))
    {
        uint8 is_std_1;
        SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_std_1));
        if (is_std_1)
        {
            static_action_info_p->dnx_action = DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA0;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_map_local.h */
shr_error_e
dnx_field_map_meta_qual_map_get(
    int unit,
    dnx_field_stage_e stage,
    bcm_field_qualify_t bcm_qual,
    dnx_field_qual_map_t * meta_qual_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(meta_qual_info_p, _SHR_E_PARAM, "meta_qual_info_p");
    DNX_FIELD_STAGE_VERIFY(stage);
    DNX_FIELD_BCM_QUAL_VERIFY(bcm_qual);

    SHR_IF_ERR_EXIT(dnx_field_qual_map_t_init(unit, meta_qual_info_p));
    if (dnx_field_map_stage_info[stage].meta_qual_map)
    {
        sal_memcpy(meta_qual_info_p, &dnx_field_map_stage_info[stage].meta_qual_map[bcm_qual], sizeof(*meta_qual_info_p));
        if (meta_qual_info_p->dnx_qual == 0)
        {
            meta_qual_info_p->dnx_qual = DNX_FIELD_QUAL_TYPE_INVALID;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_map_local.h */
shr_error_e
dnx_field_map_cs_qual_map_get(
    int unit,
    dnx_field_stage_e stage,
    bcm_field_qualify_t bcm_qual,
    dnx_field_cs_qual_map_t * cs_qual_map_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(cs_qual_map_p, _SHR_E_PARAM, "cs_qual_map_p");
    DNX_FIELD_STAGE_VERIFY(stage);
    DNX_FIELD_BCM_QUAL_VERIFY(bcm_qual);

    SHR_IF_ERR_EXIT(dnx_field_cs_qual_map_t_init(unit, cs_qual_map_p));
    if (dnx_field_map_stage_info[stage].cs_qual_map)
    {
        sal_memcpy(cs_qual_map_p, &dnx_field_map_stage_info[stage].cs_qual_map[bcm_qual], sizeof(*cs_qual_map_p));
        if (cs_qual_map_p->field_id == 0)
        {
            cs_qual_map_p->field_id = DBAL_FIELD_EMPTY;
        }
    }

exit:
    SHR_FUNC_EXIT;
}




/**see H file: field_map_local.h */
shr_error_e
dnx_field_map_meta_qual_info_get(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_qual_id_t qual_id,
    dnx_field_meta_qual_info_t * meta_qual_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(meta_qual_info_p, _SHR_E_PARAM, "meta_qual_info_p");
    DNX_FIELD_STAGE_VERIFY(stage);
    DNX_FIELD_META_QUAL_VERIFY(stage, qual_id);

    SHR_IF_ERR_EXIT(dnx_field_meta_qual_info_t_init(unit, meta_qual_info_p));
    if (dnx_field_map_stage_info[stage].meta_qual_info)
    {
        sal_memcpy(meta_qual_info_p, &dnx_field_map_stage_info[stage].meta_qual_info[qual_id], sizeof(*meta_qual_info_p));
        if (meta_qual_info_p->field_id == 0)
        {
            meta_qual_info_p->field_id = DBAL_FIELD_EMPTY;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_map_local.h */
shr_error_e
dnx_field_map_layer_record_qual_info_get(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_qual_id_t qual_id,
    dnx_field_layer_record_qual_info_t * layer_record_qual_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(layer_record_qual_info_p, _SHR_E_PARAM, "layer_record_qual_info_p");
    DNX_FIELD_STAGE_VERIFY(stage);
    DNX_FIELD_LAYER_QUAL_VERIFY(stage, qual_id);

    SHR_IF_ERR_EXIT(dnx_field_layer_record_qual_info_t_init(unit, layer_record_qual_info_p));
    if (dnx_field_map_stage_info[stage].layer_qual_info)
    {
        sal_memcpy(layer_record_qual_info_p, &dnx_field_map_stage_info[stage].layer_qual_info[qual_id], sizeof(*layer_record_qual_info_p));
        if (layer_record_qual_info_p->dbal_field_id == 0)
        {
            layer_record_qual_info_p->dbal_field_id = DBAL_FIELD_EMPTY;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_map_local.h */
shr_error_e
dnx_field_map_static_action_info_get(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_action_id_t action_id,
    dnx_field_base_action_info_t * static_action_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(static_action_info_p, _SHR_E_PARAM, "static_action_info_p");
    DNX_FIELD_STAGE_VERIFY(stage);
    DNX_FIELD_STATIC_ACTION_VERIFY(stage, action_id);

    SHR_IF_ERR_EXIT(dnx_field_static_action_info_t_init(unit, static_action_info_p));
    if (dnx_field_map_stage_info[stage].static_action_info)
    {
        sal_memcpy(static_action_info_p, &dnx_field_map_stage_info[stage].static_action_info[action_id], sizeof(*static_action_info_p));
        if (static_action_info_p->field_id == 0)
        {
            static_action_info_p->field_id = DBAL_FIELD_EMPTY;
        }
    }

    /*
      * Conditional mapping change.
      */
     if ((action_id == DBAL_ENUM_FVAL_IPMF1_ACTION_PEM_GENERAL_DATA0) &&
         ((stage == DNX_FIELD_STAGE_IPMF1) || (stage == DNX_FIELD_STAGE_IPMF2)) &&
         dnx_data_field.features.ecmp_tunnel_priority_get(unit))
     {
         uint8 is_std_1;
         SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_std_1));
         if (is_std_1)
         {
             static_action_info_p->field_id = DBAL_FIELD_PEM_GENERAL_DATA_CBTS;
         }
     }

exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_map_local.h */
shr_error_e
dnx_field_context_param_info_get(
    int unit,
    bcm_field_context_param_type_t param_type,
    bcm_field_stage_t stage,
    dnx_field_context_param_t * context_param_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(context_param_p, _SHR_E_PARAM, "context_param_p");
    if ((stage < bcmFieldStageFirst) || (stage >= bcmFieldStageCount))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid BCM Stage:%d \n", stage);
    }
    if (((int)param_type < 0) || (param_type >= bcmFieldContextParamCount))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "param_type (%d) is out of range\r\n", param_type);
    }

    sal_memcpy(context_param_p, &context_param_set[param_type][stage], sizeof(*context_param_p));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_map_local.h */
shr_error_e
dnx_field_range_map_legacy_get(
    int unit,
    dnx_field_range_type_e range_type,
    dnx_field_stage_e stage,
    dnx_field_range_map_t * range_map_legacy_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(range_map_legacy_p, _SHR_E_PARAM, "range_map_legacy_p");
    DNX_FIELD_STAGE_VERIFY(stage);
    if ((range_type < 0) || (range_type >= DNX_FIELD_RANGE_TYPE_NOF))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "range_type (%d) is out of range\r\n", range_type);
    }

    sal_memcpy(range_map_legacy_p, &range_map_legacy[range_type][stage], sizeof(*range_map_legacy_p));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_map_local.h */
shr_error_e
dnx_field_map_pp_app_map_get(
    int unit,
    bcm_field_app_db_t app_db,
    dnx_field_pp_app_map_t * pp_app_map_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(pp_app_map_p, _SHR_E_PARAM, "pp_app_map_p");
    if (app_db == bcmFieldAppDbInvalid || app_db >= bcmFieldAppDbCount)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Given PP APP DB is invalid or out of range, maximum: %d\n", bcmFieldAppDbCount);
    }
    sal_memcpy(pp_app_map_p, &dnx_field_pp_app_map[app_db], sizeof(*pp_app_map_p));
    if (pp_app_map_p->dbal_table_id == 0)
    {
        pp_app_map_p->dbal_table_id = DBAL_TABLE_EMPTY;
    }

exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_map_local.h */
shr_error_e
dnx_field_map_input_type_info_get(
    int unit,
    dnx_field_input_type_e input_type,
    dnx_field_qual_input_type_info_t * input_type_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(input_type_info_p, _SHR_E_PARAM, "input_type_info_p");
    if ((input_type < 0) || (input_type >= DNX_FIELD_INPUT_TYPE_NOF) || input_type == DNX_FIELD_INPUT_TYPE_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "input_type (%d) is out of range\r\n", input_type);
    }

    sal_memcpy(input_type_info_p, &dnx_input_type_info[input_type], sizeof(*input_type_info_p));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_map_local.h */
shr_error_e
dnx_field_map_header_qual_info_get(
    int unit,
    dnx_field_qual_id_t qual_id,
    dnx_field_header_qual_info_t * header_qual_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(header_qual_info_p, _SHR_E_PARAM, "header_qual_info_p");
    DNX_FIELD_HEADER_QUAL_VERIFY(qual_id);

    sal_memcpy(header_qual_info_p, &dnx_header_qual_info[qual_id], sizeof(*header_qual_info_p));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_map_local.h */
shr_error_e
dnx_field_map_sw_qual_info_get(
    int unit,
    dnx_field_qual_id_t qual_id,
    dnx_field_sw_qual_info_t * sw_qual_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(sw_qual_info_p, _SHR_E_PARAM, "sw_qual_info_p");
    DNX_FIELD_SW_QUAL_VERIFY(qual_id);

    sal_memcpy(sw_qual_info_p, &dnx_sw_qual_info[qual_id], sizeof(*sw_qual_info_p));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_map_local.h */
shr_error_e
dnx_field_map_sw_action_info_get(
    int unit,
    dnx_field_action_id_t action_id,
    dnx_field_base_action_info_t * sw_action_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(sw_action_info_p, _SHR_E_PARAM, "sw_action_info_p");
    DNX_FIELD_SW_ACTION_VERIFY(action_id);

    sal_memcpy(sw_action_info_p, &dnx_sw_action_info[action_id], sizeof(*sw_action_info_p));

exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_map_local.h */
shr_error_e
dnx_field_map_global_qual_map_get(
    int unit,
    bcm_field_qualify_t bcm_qual,
    dnx_field_qual_map_t * global_qual_map_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(global_qual_map_p, _SHR_E_PARAM, "global_qual_map_p");
    DNX_FIELD_BCM_QUAL_VERIFY(bcm_qual);

    sal_memcpy(global_qual_map_p, &dnx_global_qual_map[bcm_qual], sizeof(*global_qual_map_p));
    if (global_qual_map_p->dnx_qual == 0)
    {
        global_qual_map_p->dnx_qual = DNX_FIELD_QUAL_TYPE_INVALID;
    }
exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_map_local.h */
shr_error_e
dnx_field_map_global_action_map_get(
    int unit,
    bcm_field_action_t bcm_action,
    dnx_field_action_map_t * global_action_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(global_action_info_p, _SHR_E_PARAM, "global_action_info_p");
    DNX_FIELD_BCM_ACTION_VERIFY(bcm_action);

    sal_memcpy(global_action_info_p, &dnx_global_action_map[bcm_action], sizeof(*global_action_info_p));
    if (global_action_info_p->dnx_action == 0)
    {
        global_action_info_p->dnx_action = DNX_FIELD_ACTION_TYPE_INVALID;
    }
exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_map_local.h */
shr_error_e
dnx_field_map_port_profile_type_get(
    int unit,
    bcm_port_class_t bcm_port_class,
    dnx_field_port_porfile_type_e * port_profile_type_p)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_NCM_PORT_CLASS_VERIFY(bcm_port_class);

    (*port_profile_type_p) = dnx_field_map_port_profile_info[bcm_port_class].port_profile_type;
    if((*port_profile_type_p) == DNX_FIELD_PORT_PROFILE_TYPE_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM port class:%d is unsupported for field map.\n", bcm_port_class);
    }

exit:
    SHR_FUNC_EXIT;
}

/**see H file: field_map_local.h */
shr_error_e
dnx_field_map_port_profile_info_get(
    int unit,
    bcm_port_class_t bcm_port_class,
    dnx_field_map_port_profile_info_t * port_profile_info_p)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_NCM_PORT_CLASS_VERIFY(bcm_port_class);

    sal_memcpy(port_profile_info_p, &dnx_field_map_port_profile_info[bcm_port_class], sizeof(*port_profile_info_p));
    if(port_profile_info_p->port_profile_type == DNX_FIELD_PORT_PROFILE_TYPE_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM port class:%d is unsupported for field map.\n", bcm_port_class);
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map.h
 */
shr_error_e
dnx_field_map_qual_description_get(
    int unit,
    bcm_field_qualify_t bcm_qual,
    char **qual_description)
{

    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_BCM_QUAL_VERIFY(bcm_qual);

    if (bcm_qual_description[bcm_qual] != NULL)
    {
        *qual_description = (char *) bcm_qual_description[bcm_qual];
    }
    else
    {
        *qual_description = "N/A";
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_qual_description_is_empty(
        int unit,
        bcm_field_qualify_t bcm_qual,
        uint8 *is_empty)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(is_empty, _SHR_E_INTERNAL, "is_empty");
    DNX_FIELD_BCM_QUAL_VERIFY(bcm_qual);

    if (ISEMPTY(bcm_qual_description[bcm_qual]))
    {
        *is_empty = TRUE;
    }
    else
    {
        *is_empty = FALSE;
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See header in field_map_local.h
 */
shr_error_e
dnx_field_action_description_get(
    int unit,
    bcm_field_action_t bcm_action,
    char **action_description)
{

    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_BCM_ACTION_VERIFY(bcm_action);

    if (bcm_action_description[bcm_action] != NULL)
    {
        *action_description = (char *) bcm_action_description[bcm_action];
    }
    else
    {
        *action_description = "N/A";
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_action_description_is_empty(
        int unit,
        bcm_field_action_t bcm_action,
        uint8 *is_empty)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(is_empty, _SHR_E_INTERNAL, "is_empty");
    DNX_FIELD_BCM_ACTION_VERIFY(bcm_action);

    if (ISEMPTY(bcm_action_description[bcm_action]))
    {
        *is_empty = TRUE;
    }
    else
    {
        *is_empty = FALSE;
    }
exit:
    SHR_FUNC_EXIT;
}

/**See header of dnx_field_map_static_opcode_to_apptype_get_dispatch in field_map.h */
shr_error_e
dnx_field_map_static_opcode_to_apptype_get(
    int unit,
    uint32 static_opcode_id,
    bcm_field_AppType_t * apptype_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(apptype_p, _SHR_E_PARAM, "apptype_p");

    if (static_opcode_id >= (sizeof(dnx_field_static_opcode_info) / sizeof(dnx_field_static_opcode_info[0])))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Opcode %d is out of range for predefined opcodes (0-%d)\n",
                     static_opcode_id,
                     ((int)(sizeof(dnx_field_static_opcode_info) / sizeof(dnx_field_static_opcode_info[0]))) - 1);
    }

    if (dnx_field_static_opcode_info[static_opcode_id].is_valid == 0)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    (*apptype_p) = dnx_field_static_opcode_info[static_opcode_id].app_type;

exit:
    SHR_FUNC_EXIT;
}

/**See header of dnx_field_map_static_opcode_to_cs_profile_get_dispatch in field_map.h */
shr_error_e
dnx_field_map_static_opcode_to_cs_profile_get(
    int unit,
    uint32 static_opcode_id,
    uint32 * cs_profile_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(cs_profile_p, _SHR_E_PARAM, "cs_profile_p");

    if (static_opcode_id >= (sizeof(dnx_field_static_opcode_info) / sizeof(dnx_field_static_opcode_info[0])))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Opcode %d is out of range for predefined opcodes (0-%d)\n",
                     static_opcode_id,
                     ((int)(sizeof(dnx_field_static_opcode_info) / sizeof(dnx_field_static_opcode_info[0]))) - 1);
    }

    if (dnx_field_static_opcode_info[static_opcode_id].is_valid == 0)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    (*cs_profile_p) = dnx_field_static_opcode_info[static_opcode_id].cs_profile_id;

exit:
    SHR_FUNC_EXIT;
}

/**See header of dnx_field_map_standard_1_predefined_apptype_to_opcode_get_dispatch in field_map.h */
shr_error_e
dnx_field_map_standard_1_predefined_apptype_to_opcode_get(
    int unit,
    bcm_field_AppType_t apptype,
    uint32 * static_opcode_id_p)
{
    uint32 opcode_id;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(static_opcode_id_p, _SHR_E_PARAM, "static_opcode_id_p");

    if ((apptype >=
         (sizeof(dnx_field_standard_1_static_apptype_map) / sizeof(dnx_field_standard_1_static_apptype_map[0])))
        || (((int) apptype) < 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Apptype %d is out of range for predefined apptype. Range is %d-%d.\n",
                     apptype, 0,
                     ((int)(sizeof(dnx_field_standard_1_static_apptype_map) /
                            sizeof(dnx_field_standard_1_static_apptype_map[0]))) - 1);
    }
    opcode_id = dnx_field_standard_1_static_apptype_map[apptype].opcode_id;

    if (opcode_id == DBAL_ENUM_FVAL_KBP_FORWARD_APP_TYPES_INVALID)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    (*static_opcode_id_p) = opcode_id;

exit:
    SHR_FUNC_EXIT;
}

/* *INDENT-ON* */
